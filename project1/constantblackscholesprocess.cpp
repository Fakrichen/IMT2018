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
#include <ql/termstructures/yield/flatforward.hpp>
#include <ql/time/calendars/nullcalendar.hpp>
#include <ql/time/daycounters/actual365fixed.hpp>
#include "constantblackscholesprocess.hpp"

namespace QuantLib {

    ConstantBlackScholesProcess::ConstantBlackScholesProcess(
            const Handle<Quote>& x0,
            const Handle<YieldTermStructure>& dividendTS,
            const Handle<YieldTermStructure>& riskFreeTS,
            const Handle<BlackVolTermStructure>& blackVolTS,
            const ext::shared_ptr<discretization>& disc,
            bool forceDiscretization,
            const Date exercice_date,
            const double strike)
    : StochasticProcess1D(disc), x0_BS(x0), riskFreeRate_BS(riskFreeTS),
      dividendYield_BS(dividendTS), blackVolatility_BS(blackVolTS),
      forceDiscretization_(forceDiscretization), updated_(false) {
        exercice_date = exercice_date; 
        strike = strike;
        risk_rate = riskFreeRate_->dayCounter();
        Zero_Rate = riskFreeRate_->zeroRate(exercice_date, risk_rate, Continuous, NoFrequency, true);
        Dividend_yield = dividendYield_->zeroRate(exercice_date, risk_rate, Continuous, NoFrequency, true);
        Volat_diffusion = blackVolatility_->blackVol(exercice_date,strike);
        Risk_drift = Zero_Rate - Dividend_yield;

    }    

    Real ConstantBlackScholesProcess::x0() const {
        return x0_->value();
    }

    Real ConstantBlackScholesProcess::diffusion(Time t, Real S) const {
        return S * Volat_diffusion;
    }  

    Real ConstantBlackScholesProcess::drift(Time t, Real S) const {
        return S * Risk_drift;
    }

    Real ConstantBlackScholesProcess::variance(Time t0, Real x0, Time dt) const {
 
        return discretization_->variance(*this,t0,x0,dt);
    }

    Real GeneralizedBlackScholesProcess::stdDeviation(Time t0, Real x0, Time dt) const {

        return discretization_->diffusion(*this,t0,x0,dt);
    }

}
