#include <ql/quantlib.hpp>
#include <iostream>
using namespace QuantLib;

int main() {
    Date today = Date::todaysDate();
    std::cout << "Today's date is " << today << std::endl;
    return 0;
}
