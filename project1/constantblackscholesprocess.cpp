/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

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



	constantBlackScholesProcess::constantBlackScholesProcess(
		const Handle<Quote> x0,
		const Date exercice_Date,
		const Real strike,
		const Handle<YieldTermStructure>& risk_free_BS,
		const Handle<BlackVolTermStructure>& volatility_BS,
		const Handle<YieldTermStructure>& dividend_yield_BS,
		boost::shared_ptr<discretization>& disc)
		:StochasticProcess1D(disc), x0_(x0), strike_(strike),

		riskFreeRate_(risk_free_BS), dividendYield_(dividend_yield_BS), blackVolatility_(volatility_BS) {

		exercice_date = exercice_Date;
		risk_drift = riskFreeRate_->zeroRate(exercice_date, 
									riskFreeRate_->dayCounter(), 
									Continuous,
									NoFrequency, 
									true) 
					- dividendYield_->zeroRate(exercice_date, 
											   riskFreeRate_->dayCounter(), 
											   Continuous,
											   NoFrequency, 
											   true);

		diffusion_ = blackVolatility_->blackVol(exercice_date,strike_);

	}

	Real constantBlackScholesProcess::x0()const {
		return x0_->value();
	}

	Real constantBlackScholesProcess::drift(Time t, Real s) const {
		return risk_drift*s;
	}

	Real constantBlackScholesProcess::diffusion(Time t, Real s) const {
		return diffusion_*s;
	}

	Real constantBlackScholesProcess::variance(const StochasticProcess1D&,
		Time t0, Real s0, Time dt) const {
		return discretization_->variance(*this, t0, s0, dt);

	}

}
