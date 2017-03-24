#include <arbitrary_precision_calculation/arbitraryprecisioncalculation.h>
#include <iostream>
#include <string>

int main(){

    double d_1 = 1.0;
    double d_small = 1e-50;

    std::cout << "double calculation:\n";
    std::cout << "((" << d_1 << "+" << d_small << ")-" << d_1 <<")=" << ((d_1 + d_small) -d_1) << "\n";

    arbitraryprecisioncalculation::Configuration::setInitialPrecision(230);
    // 230 is approximately ld(10)*(50+20)
    // 50 decimal digits of needed precision and additional 20 digits to guarantee the precision of the output
    arbitraryprecisioncalculation::Configuration::setPrecisionSafetyMargin(50);
    arbitraryprecisioncalculation::Configuration::setOutputPrecision(20);

    mpf_t* v_1 = arbitraryprecisioncalculation::mpftoperations::ToMpft(d_1);
    mpf_t* v_small = arbitraryprecisioncalculation::mpftoperations::ToMpft(d_small);

    mpf_t* v_sum = arbitraryprecisioncalculation::mpftoperations::Add(v_1, v_small);
    mpf_t* v_result = arbitraryprecisioncalculation::mpftoperations::Subtract(v_sum, v_1);

    std::string s_1 = arbitraryprecisioncalculation::mpftoperations::MpftToString(v_1);
    std::string s_small = arbitraryprecisioncalculation::mpftoperations::MpftToString(v_small);
    std::string s_sum = arbitraryprecisioncalculation::mpftoperations::MpftToString(v_sum);
    std::string s_result = arbitraryprecisioncalculation::mpftoperations::MpftToString(v_result);

    std::cout << "mpf_t calculation:\n";
    std::cout << "(((" << s_1 << ")+(" << s_small << "))-(" << s_1 <<"))=(" << s_sum << ")-(" << s_1 << ")=" << s_result << "\n";
    std::cout << "The non precise value of v_small depends on the non precise conversion from 1e-50 to double not from double to mpf_t!!!\n";

    // After the last use of variable the value should be released.
    // This is actually not really necessary at the end of a program.
    arbitraryprecisioncalculation::mpftoperations::ReleaseValue(v_1);
    arbitraryprecisioncalculation::mpftoperations::ReleaseValue(v_small);
    arbitraryprecisioncalculation::mpftoperations::ReleaseValue(v_sum);
    arbitraryprecisioncalculation::mpftoperations::ReleaseValue(v_result);
}
