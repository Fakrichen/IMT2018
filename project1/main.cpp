
#include "constantblackscholesprocess.hpp"
#include "mceuropeanengine.hpp"
#include <ql/pricingengines/vanilla/mceuropeanengine.hpp>
#include <ql/quantlib.hpp>
#include <iostream>

using namespace QuantLib;

int main() {

    try {

        // add your code here

        return 0;

    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "unknown error" << std::endl;
        return 1;
    }
}

