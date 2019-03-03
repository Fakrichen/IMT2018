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

#ifndef quantlib_constant_black_scholes_process_hpp
#define quantlib_constant_black_scholes_process_hpp

#include <ql/stochasticprocess.hpp>
#include <ql/processes/eulerdiscretization.hpp>
#include <ql/termstructures/yieldtermstructure.hpp>
#include <ql/termstructures/volatility/equityfx/blackvoltermstructure.hpp>
#include <ql/quote.hpp>


   namespace QuantLib {

	class constantBlackScholesProcess : public StochasticProcess1D {
	 private:
		   Handle<Quote> x0_;
		   Handle<YieldTermStructure> riskFreeRate_;
		   Handle<YieldTermStructure> 	dividendYield_;
		   Handle<BlackVolTermStructure> blackVolatility_;
		   Real risk_drift;
		   Real diffusion_;
		   Date exercice_date;
		   Real strike_;

	public:
		 constantBlackScholesProcess(
			const Handle <Quote> x0,
			const Date exercice_Date,
			const Real strike,
			const Handle<YieldTermStructure>& risk_free_BS,
			const Handle<BlackVolTermStructure>& volatility_BS,
			const Handle<YieldTermStructure>& dividend_yield_BS,
			const boost::shared_ptr<discretization>& disc = boost::shared_ptr<discretization>(new EulerDiscretization));


		Real x0() const;
		Real drift(Time t, Real s) const;
		Real diffusion(Time t, Real s) const;
		
	};

}

#endif
