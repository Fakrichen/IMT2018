main : main.cpp constantBlackScholesProcess.o mceuropeanengine.hpp
	g++ -o main main.cpp constantBlackScholesProcess.o -lQuantLib
constantBlackScholesProcess.o : constantBlackScholesProcess.cpp constantBlackScholesProcess.hpp
	g++ -c constantBlackScholesProcess.cpp  -lQuantLib
