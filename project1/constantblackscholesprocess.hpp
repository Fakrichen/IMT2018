/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2001, 2002, 2003 Sadruddin Rejeb
 Copyright (C) 2003 Ferdinando Ametrano
 Copyright (C) 2004, 2005, 2006, 2007, 2009 StatPro Italia srl
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

/*! \file blackscholesprocess.hpp
    \brief Black-Scholes processes
*/

#ifndef quantlib_black_scholes_process_hpp
#define quantlib_black_scholes_process_hpp

#include <ql/stochasticprocess.hpp>
#include <ql/processes/eulerdiscretization.hpp>
#include <ql/termstructures/yieldtermstructure.hpp>
#include <ql/termstructures/volatility/equityfx/blackvoltermstructure.hpp>
#include <ql/quote.hpp>

namespace QuantLib {


    //! Constant Black-Scholes stochastic process
    /*! This class describes the stochastic process \f$ S \f$ governed by
        \f[
            d\ln S(t) = (r(t) - q(t) - \frac{\sigma(t, S)^2}{2}) dt
                     + \sigma dW_t.
        \f]
        \warning while the interface is expressed in terms of \f$ S \f$,
                 the internal calculations work on \f$ ln S \f$.
        \ingroup processes
    */
    class ConstantBlackScholesProcess : public StochasticProcess1D {
        private:
            Date exercice_date;
            double strike;
            const Handle<Quote> x0_BS,
            const Handle<YieldTermStructure> dividendYield_BS,
            const Handle<YieldTermStructure> riskFreeRate_BS,
            const Handle<BlackVolTermStructure> blackVolatility_BS,
            double risk_rate;
            double Zero_Rate;
            double Dividend_yield;
            double Volat_diffusion;
            double Risk_drift;

        public:
            ConstantBlackScholesProcess(
                const Handle<Quote>& x0,
                const Handle<YieldTermStructure>& dividendTS,
                const Handle<YieldTermStructure>& riskFreeTS,
                const Handle<BlackVolTermStructure>& blackVolTS,
                const ext::shared_ptr<discretization>& d = ext::shared_ptr<discretization>(new EulerDiscretization),
                bool forceDiscretization = false,
                const Date exercice_date,
                const double strike);


            Real x0() const;
            Real drift(Time t, Real x) const;
            Real diffusion(Time t, Real x) const;
            Real variance(Time t0, Real x0, Time dt) const;
            Real stdDeviation(Time t0, Real x0, Time dt) const;
    };

}
#endif