#include "constantBlackScholesProcess.hpp"
#include "mceuropeanengine.hpp"
#include <ql/pricingengines/vanilla/mceuropeanengine.hpp>
#include <ql/quantlib.hpp>
#include <time.h>

using namespace QuantLib;

int main() {

	try {
		int rand_num = 0;
		srand(time(NULL));
		rand_num = rand();
		std::cout << rand_num << std::endl;
		Calendar calendar = TARGET();
		DayCounter dayCounter = Actual365Fixed();
		//date du jour
		Date t0(1, March, 2019);
		//maturite
		Date T(1, March, 2020);
		//type d'option 
		Option::Type type(Option::Put);
		//Sous jacent
		Real stock_price = 100;
		//Prix d'exercice
		Real strike = 80;
		//Taux de dividende
		Spread q = 0.03;
		//Taux d'interet
		Rate r = 0.05;
		//volatilite
		Volatility vol = 0.15;
		
		// Construction du taux d'interet
		Handle<YieldTermStructure> rate(boost::shared_ptr<YieldTermStructure>(new FlatForward(t0, r, dayCounter)));
		// Construction du sous jacent
		Handle<Quote> underlying(boost::shared_ptr<Quote>(new SimpleQuote(stock_price)));
		// Construction de la vol
		Handle<BlackVolTermStructure> volatility(boost::shared_ptr<BlackVolTermStructure>(new BlackConstantVol(t0, calendar, vol, dayCounter)));
		// Construction du dividende
		Handle<YieldTermStructure> dividend(boost::shared_ptr<YieldTermStructure>(new FlatForward(t0, q, dayCounter)));
		// Construction du Process Black and scholes
		boost::shared_ptr<GeneralizedBlackScholesProcess> process_BS(new GeneralizedBlackScholesProcess(underlying, dividend, rate, volatility));
		// Construction de deux options europeennes
		boost::shared_ptr<Exercise> europeanExercise(new EuropeanExercise(T));
		boost::shared_ptr<StrikedTypePayoff> payoff(new PlainVanillaPayoff(type, strike));
		VanillaOption option_1(payoff, europeanExercise);
		VanillaOption option_2(payoff, europeanExercise);

		//Modele de princing
		option_1.setPricingEngine(boost::shared_ptr<PricingEngine>(new AnalyticEuropeanEngine(process_BS)));
		option_2.setPricingEngine(boost::shared_ptr<PricingEngine>(new AnalyticEuropeanEngine(process_BS)));

		std::cout << "     " << std::endl;
		Real price = option_1.NPV();
		std::cout << "prix = option_1.NPV()=" << price << std::endl;
		std::cout << "     " << std::endl;
		std::cout << "MCEuropeanEngine avec GeneralizedBlackScholesProcess" << std::endl;
		std::cout << "     " << std::endl;


		option_1.setPricingEngine(boost::shared_ptr<PricingEngine>(new MCEuropeanEngine_2<PseudoRandom>(process_BS,10,Null<Size>(),
																											true,false,
																											10000,Null<Real>(),Null<Size>(), 
																											SeedGenerator::instance().get(),
																											false)));
						
		clock_t t_debut = clock();
		Real price1 = option_1.NPV();
		std::cout << "Prix de l'option " << price1 << std::endl;
		printf("Temps d'execution: %.2fs\n", (double)(clock() - t_debut) / CLOCKS_PER_SEC);
		std::cout << "Erreur d'estimation " << option_1.errorEstimate() << std::endl;	
		
		std::cout << "     " << std::endl;
		std::cout << "     " << std::endl;
		std::cout << "MCEuropeanEngine avec constantBlackScholesProcess" << std::endl;
		std::cout << "     " << std::endl;
	
		option_2.setPricingEngine(boost::shared_ptr<PricingEngine>(new MCEuropeanEngine_2<PseudoRandom>(process_BS,10,Null<Size>(),
																										true,false,10000,Null<Real>(),
																										Null<Size>(), SeedGenerator::instance().get(),
																										true)));
	
		clock_t t_debut_2 = clock();

		std::cout << "Prix de l'option  " << option_2.NPV() << std::endl;
		printf("Temps d'execution: %.2fs\n", (double)(clock() - t_debut_2) / CLOCKS_PER_SEC);
		std::cout << "Erreur d'estimation " << option_2.errorEstimate() << std::endl;
		std::cout << "     " << std::endl;

		return 0;

	}

	catch (std::exception& e) {

		std::cerr << e.what() << std::endl;
		system("pause");
		return 1;
	}
	catch (...) {
		std::cerr << "Erreur inconnue" << std::endl;
		system("pause");
		return 1;
	} 
}

