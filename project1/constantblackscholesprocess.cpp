* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2003 Ferdinando Ametrano
 Copyright (C) 2001, 2002, 2003 Sadruddin Rejeb
 Copyright (C) 2004, 2005, 2006, 2007 StatPro Italia srl
 Copyright (C) 2015 Peter Caspers
 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/
 QuantLib is free software: you can redistribute it and/or modify it
 under the terms of the QuantLib license.  You should have received a
 copy of the license along with this program; if not, please email
 <quantlib-dev@lists.sf.net>. The license is also available online at
 <http://quantlib.org/license.shtml>.
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

#include <ql/processes/blackscholesprocess.hpp>
#include <ql/termstructures/volatility/equityfx/localvolsurface.hpp>
#include <ql/termstructures/volatility/equityfx/localvolcurve.hpp>
#include <ql/termstructures/volatility/equityfx/localconstantvol.hpp>
#include <ql/termstructures/yield/flatforward.hpp>
#include <ql/time/calendars/nullcalendar.hpp>
#include <ql/time/daycounters/actual365fixed.hpp>


namespace QuantLib {

    ConstantBlackScholesProcess::ConstantBlackScholesProcess(
        const Handle<Quote>& x0,
        const Handle<YieldTermStructure>& dividendTS,
        const Handle<YieldTermStructure>& riskFreeTS,
        const Handle<BlackVolTermStructure>& blackVolTS,
        const Handle<LocalVolTermStructure>& localVolTS)
    : StochasticProcess1D(ext::make_shared<EulerDiscretization>()),
      x0_(x0), riskFreeRate_(riskFreeTS),
      dividendYield_(dividendTS), blackVolatility_(blackVolTS),
      externalLocalVolTS_(localVolTS),
      forceDiscretization_(false), hasExternalLocalVol_(true), updated_(false) {
        registerWith(x0_);
        registerWith(riskFreeRate_);
        registerWith(dividendYield_);
        registerWith(blackVolatility_);
        registerWith(externalLocalVolTS_);
    }

    ConstantBlackScholesProcess::ConstantBlackScholesProcess(
             const Handle<Quote>& x0,
             const Handle<YieldTermStructure>& dividendTS,
             const Handle<YieldTermStructure>& riskFreeTS,
             const Handle<BlackVolTermStructure>& blackVolTS,
             const ext::shared_ptr<discretization>& disc,
             bool forceDiscretization)
    : StochasticProcess1D(disc), x0_(x0), riskFreeRate_(riskFreeTS),
      dividendYield_(dividendTS), blackVolatility_(blackVolTS),
      forceDiscretization_(forceDiscretization),
      hasExternalLocalVol_(false), updated_(false) {
        registerWith(x0_);
        registerWith(riskFreeRate_);
        registerWith(dividendYield_);
        registerWith(blackVolatility_);
    }

    Real ConstantBlackScholesProcess::x0() const {
        return x0_->value();
    }

    Real ConstantBlackScholesProcess::drift(Time t, Real x) const {
        Real sigma = diffusion(t,x);
        // we could be more anticipatory if we know the right dt
        // for which the drift will be used
        return riskFreeRate_->zeroRate(const Time & t, riskFreeRate_->dayCounter(), Continuous, NoFrequency, true)
         - dividendYield_->zeroRate(const Time & t, riskFreeRate_->dayCounter(), Continuous, NoFrequency, true)
         - 0.5 * sigma * sigma;
    }

    Real ConstantBlackScholesProcess::diffusion(Time t, Real x) const {
        return localVolatility()->localVol(t, x, true);
    }

    Real ConstantBlackScholesProcess::apply(Real x0, Real dx) const {
        return x0 * std::exp(dx);
    }

    Real ConstantBlackScholesProcess::expectation(Time t0,
                                                     Real x0,
                                                     Time dt) const {
        localVolatility(); // trigger update
        if(isStrikeIndependent_ && !forceDiscretization_) {
            // exact value for curves
            return x0 *
                std::exp(dt * (riskFreeRate_->zeroRate(const Time & t, riskFreeRate_->dayCounter(), Continuous, NoFrequency, true)
                               - dividendYield_->zeroRate(const Time & d, riskFreeRate_->dayCounter(), Continuous, NoFrequency, true)));
        } else {
            QL_FAIL("not implemented");
        }
    }

    Real ConstantBlackScholesProcess::stdDeviation(Time t0, Real x0, Time dt) const {
        //localVolatility(); // trigger update
        //if(isStrikeIndependent_ && !forceDiscretization_) {
            // exact value for curves
            //return std::sqrt(variance(t0,x0,dt));
        //}
        //else{
        return discretization_->diffusion(*this,t0,x0,dt);
        //}
    //}

    Real ConstantBlackScholesProcess::variance(Time t0, Real x0, Time dt) const {
        /*localVolatility(); // trigger update
        #if(isStrikeIndependent_ && !forceDiscretization_) {
            // exact value for curves
            #return blackVolatility_->blackVariance(t0 + dt, 0.01) -
                   #blackVolatility_->blackVariance(t0, 0.01);
        #}*/
        //else{
        return discretization_->variance(*this,t0,x0,dt);
        //}
    // }

    Real ConstantBlackScholesProcess::evolve(Time t0, Real x0,
                                                Time dt, Real dw) const {
        #localVolatility(); // trigger update
        if (isStrikeIndependent_ && !forceDiscretization_) {
            // exact value for curves
            Real var = variance(t0, x0, dt);
            Real drift = (riskFreeRate_->zeroRate(const Time & t, riskFreeRate_->dayCounter(), Continuous, NoFrequency, true)  -
            dividendYield_->zeroRate(const Time & t, riskFreeRate_->dayCounter(), Continuous, NoFrequency, true) ) *dt - 0.5 * var;
            return apply(x0, std::sqrt(var) * dw + drift);
        } else
            return apply(x0, discretization_->drift(*this, t0, x0, dt) +
                                 stdDeviation(t0, x0, dt) * dw);
    }

    Time ConstantBlackScholesProcess::time(const Date& d) const {
        return riskFreeRate_->dayCounter().yearFraction(
                                           riskFreeRate_->referenceDate(), d);
    }

    void ConstantBlackScholesProcess::update() {
        updated_ = false;
        StochasticProcess1D::update();
    }

    const Handle<Quote>&
    ConstantBlackScholesProcess::stateVariable() const {
        return x0_;
    }

    const Handle<YieldTermStructure>&
    ConstantBlackScholesProcess::dividendYield() const {
        return dividendYield_->zeroRate(const Time & t, riskFreeRate_->dayCounter(), Continuous, NoFrequency, true) ;
    }

    const Handle<YieldTermStructure>&
    ConstantBlackScholesProcess::riskFreeRate() const {
        return riskFreeRate_->zeroRate(const Time & t, riskFreeRate_->dayCounter(), Continuous, NoFrequency, true) ;
    }

    const Handle<BlackVolTermStructure>&
    ConstantBlackScholesProcess::blackVolatility() const {
        return blackVolatility_;
    }

    const Handle<LocalVolTermStructure>&
    ConstantBlackScholesProcess::localVolatility() const {
        /*if (hasExternalLocalVol_)   //c'est quoi ExternalLocalVol_??
            return externalLocalVolTS_;*/

        if (!updated_) {
            isStrikeIndependent_=true;

            // constant Black vol?
            ext::shared_ptr<BlackConstantVol> constVol =
                ext::dynamic_pointer_cast<BlackConstantVol>(
                                                          *blackVolatility());
            if (constVol) {
                // ok, the local vol is constant too.
                localVolatility_.linkTo(ext::make_shared<LocalConstantVol>(
                    constVol->referenceDate(),
                    constVol->blackVol(0.0, x0_->value()),
                    constVol->dayCounter()));
                updated_ = true;
                return localVolatility_;
            }

            
        } else {
            return localVolatility_;
        }
}

    // specific models

    BlackScholesProcess::BlackScholesProcess(
                              const Handle<Quote>& x0,
                              const Handle<YieldTermStructure>& riskFreeTS,
                              const Handle<BlackVolTermStructure>& blackVolTS,
                              const ext::shared_ptr<discretization>& d,
                              bool forceDiscretization)
    : ConstantBlackScholesProcess(
             x0,
             // no dividend yield
             Handle<YieldTermStructure>(ext::shared_ptr<YieldTermStructure>(
                  new FlatForward(0, NullCalendar(), 0.0, Actual365Fixed()))),
             riskFreeTS,
             blackVolTS,
             d,forceDiscretization) {}


    BlackScholesMertonProcess::BlackScholesMertonProcess(
                              const Handle<Quote>& x0,
                              const Handle<YieldTermStructure>& dividendTS,
                              const Handle<YieldTermStructure>& riskFreeTS,
                              const Handle<BlackVolTermStructure>& blackVolTS,
                              const ext::shared_ptr<discretization>& d,
                              bool forceDiscretization)
    : ConstantBlackScholesProcess(x0,dividendTS,riskFreeTS,blackVolTS,d,
                                     forceDiscretization) {}


    BlackProcess::BlackProcess(const Handle<Quote>& x0,
                               const Handle<YieldTermStructure>& riskFreeTS,
                               const Handle<BlackVolTermStructure>& blackVolTS,
                               const ext::shared_ptr<discretization>& d,
                               bool forceDiscretization)
    : ConstantBlackScholesProcess(x0,riskFreeTS,riskFreeTS,blackVolTS,d,
                                     forceDiscretization) {}


    GarmanKohlagenProcess::GarmanKohlagenProcess(
                          const Handle<Quote>& x0,
                          const Handle<YieldTermStructure>& foreignRiskFreeTS,
                          const Handle<YieldTermStructure>& domesticRiskFreeTS,
                          const Handle<BlackVolTermStructure>& blackVolTS,
                          const ext::shared_ptr<discretization>& d,
                          bool forceDiscretization)
    : ConstantBlackScholesProcess(x0,foreignRiskFreeTS,domesticRiskFreeTS,
                                     blackVolTS,d,forceDiscretization) {}

}
