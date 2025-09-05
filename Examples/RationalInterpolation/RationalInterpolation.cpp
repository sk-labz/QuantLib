/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*!
 Copyright (C) 2024 Klaus Spanderen

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

#include <ql/qldefines.hpp>
#if !defined(BOOST_ALL_NO_LIB) && defined(BOOST_MSVC)
#  include <ql/auto_link.hpp>
#endif

#include <ql/quantlib.hpp>
#include <ql/math/interpolations/rationalinterpolation.hpp>
#include <ql/math/interpolations/linearinterpolation.hpp>
#include <ql/math/interpolations/cubicinterpolation.hpp>

#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>

using namespace QuantLib;

namespace {
    
    // Helper function to print interpolation comparison
    void printComparison(const std::string& title,
                        const std::vector<Real>& x,
                        const std::vector<Real>& y,
                        const Interpolation& rational,
                        const Interpolation& linear,
                        const Interpolation& cubic) {
        
        std::cout << "\n" << title << std::endl;
        std::cout << std::string(60, '=') << std::endl;
        std::cout << std::setw(8) << "x" 
                  << std::setw(12) << "Original" 
                  << std::setw(12) << "Rational"
                  << std::setw(12) << "Linear"
                  << std::setw(12) << "Cubic" << std::endl;
        std::cout << std::string(60, '-') << std::endl;
        
        // Print original data points
        for (Size i = 0; i < x.size(); ++i) {
            std::cout << std::setw(8) << std::fixed << std::setprecision(2) << x[i]
                      << std::setw(12) << std::setprecision(6) << y[i]
                      << std::setw(12) << rational(x[i])
                      << std::setw(12) << linear(x[i])
                      << std::setw(12) << cubic(x[i]) << std::endl;
        }
        
        std::cout << std::string(60, '-') << std::endl;
        
        // Print interpolated values between data points
        for (Size i = 0; i < x.size() - 1; ++i) {
            Real midX = (x[i] + x[i+1]) / 2.0;
            std::cout << std::setw(8) << midX << " (interp)"
                      << std::setw(12) << "---"
                      << std::setw(12) << rational(midX)
                      << std::setw(12) << linear(midX)
                      << std::setw(12) << cubic(midX) << std::endl;
        }
    }
    
    // Test function with known rational form: f(x) = (x^2 + 1)/(x + 1)
    Real testFunction(Real x) {
        return (x * x + 1.0) / (x + 1.0);
    }
    
    // Volatility smile data (simplified)
    void volatilitySmileExample() {
        std::cout << "\n\n";
        std::cout << "VOLATILITY SMILE INTERPOLATION EXAMPLE" << std::endl;
        std::cout << std::string(80, '=') << std::endl;
        std::cout << "Demonstrates rational interpolation for volatility smiles\n" << std::endl;
        
        // Strike prices (as moneyness = K/S)
        std::vector<Real> strikes = {0.8, 0.9, 1.0, 1.1, 1.2};
        
        // Implied volatilities (typical smile shape)
        std::vector<Real> vols = {0.25, 0.20, 0.18, 0.19, 0.22};
        
        RationalInterpolation rational(strikes.begin(), strikes.end(), vols.begin(), 2, 2);
        LinearInterpolation linear(strikes.begin(), strikes.end(), vols.begin());
        CubicInterpolation cubic(strikes.begin(), strikes.end(), vols.begin(),
                               CubicInterpolation::Spline, false,
                               CubicInterpolation::SecondDerivative, 0.0,
                               CubicInterpolation::SecondDerivative, 0.0);
        
        printComparison("Volatility Smile Interpolation", strikes, vols, 
                       rational, linear, cubic);
        
        // Test extrapolation
        std::cout << "\nExtrapolation test:" << std::endl;
        std::cout << "Strike = 0.7: Rational = " << rational(0.7, true) 
                  << ", Linear = " << linear(0.7, true) << std::endl;
        std::cout << "Strike = 1.3: Rational = " << rational(1.3, true) 
                  << ", Linear = " << linear(1.3, true) << std::endl;
    }
    
    void yieldCurveExample() {
        std::cout << "\n\n";
        std::cout << "YIELD CURVE INTERPOLATION EXAMPLE" << std::endl;
        std::cout << std::string(80, '=') << std::endl;
        std::cout << "Demonstrates rational interpolation for yield curves\n" << std::endl;
        
        // Time to maturity (years)
        std::vector<Real> times = {0.25, 0.5, 1.0, 2.0, 5.0, 10.0};
        
        // Zero rates
        std::vector<Real> rates = {0.02, 0.025, 0.03, 0.035, 0.04, 0.038};
        
        RationalInterpolation rational(times.begin(), times.end(), rates.begin(), 3, 2);
        LinearInterpolation linear(times.begin(), times.end(), rates.begin());
        CubicInterpolation cubic(times.begin(), times.end(), rates.begin(),
                               CubicInterpolation::Spline, false,
                               CubicInterpolation::SecondDerivative, 0.0,
                               CubicInterpolation::SecondDerivative, 0.0);
        
        printComparison("Yield Curve Interpolation", times, rates, 
                       rational, linear, cubic);
    }
    
    void mathematicalExample() {
        std::cout << "\n\n";
        std::cout << "MATHEMATICAL FUNCTION EXAMPLE" << std::endl;
        std::cout << std::string(80, '=') << std::endl;
        std::cout << "Interpolating f(x) = (x^2 + 1)/(x + 1) - a rational function\n" << std::endl;
        
        // Data points from the rational function
        std::vector<Real> x = {1.0, 2.0, 3.0, 4.0, 5.0};
        std::vector<Real> y;
        
        for (Real xi : x) {
            y.push_back(testFunction(xi));
        }
        
        RationalInterpolation rational(x.begin(), x.end(), y.begin(), 2, 2);
        LinearInterpolation linear(x.begin(), x.end(), y.begin());
        CubicInterpolation cubic(x.begin(), x.end(), y.begin(),
                               CubicInterpolation::Spline, false,
                               CubicInterpolation::SecondDerivative, 0.0,
                               CubicInterpolation::SecondDerivative, 0.0);
        
        printComparison("Rational Function f(x) = (x^2 + 1)/(x + 1)", x, y, 
                       rational, linear, cubic);
        
        // Test accuracy at non-grid points
        std::cout << "\nAccuracy test at x = 2.5:" << std::endl;
        Real testX = 2.5;
        Real exact = testFunction(testX);
        std::cout << "Exact value: " << exact << std::endl;
        std::cout << "Rational interpolation: " << rational(testX) 
                  << " (error: " << std::abs(rational(testX) - exact) << ")" << std::endl;
        std::cout << "Linear interpolation: " << linear(testX) 
                  << " (error: " << std::abs(linear(testX) - exact) << ")" << std::endl;
        std::cout << "Cubic interpolation: " << cubic(testX) 
                  << " (error: " << std::abs(cubic(testX) - exact) << ")" << std::endl;
    }
    
    void performanceTest() {
        std::cout << "\n\n";
        std::cout << "PERFORMANCE COMPARISON" << std::endl;
        std::cout << std::string(80, '=') << std::endl;
        
        // Create larger dataset
        const Size n = 50;
        std::vector<Real> x(n), y(n);
        
        for (Size i = 0; i < n; ++i) {
            x[i] = static_cast<Real>(i) / 10.0;
            y[i] = std::sin(x[i]) * std::exp(-x[i] / 2.0);
        }
        
        RationalInterpolation rational(x.begin(), x.end(), y.begin());
        LinearInterpolation linear(x.begin(), x.end(), y.begin());
        CubicInterpolation cubic(x.begin(), x.end(), y.begin(),
                               CubicInterpolation::Spline, false,
                               CubicInterpolation::SecondDerivative, 0.0,
                               CubicInterpolation::SecondDerivative, 0.0);
        
        const Size numEvaluations = 10000;
        std::cout << "Testing " << numEvaluations << " evaluations on " << n << " data points..." << std::endl;
        
        // Time rational interpolation
        clock_t start = clock();
        for (Size i = 0; i < numEvaluations; ++i) {
            Real testX = static_cast<Real>(i % (n-1)) / 10.0 + 0.05;
            volatile Real result = rational(testX);
            (void)result; // Suppress unused variable warning
        }
        clock_t rationalTime = clock() - start;
        
        // Time linear interpolation
        start = clock();
        for (Size i = 0; i < numEvaluations; ++i) {
            Real testX = static_cast<Real>(i % (n-1)) / 10.0 + 0.05;
            volatile Real result = linear(testX);
            (void)result;
        }
        clock_t linearTime = clock() - start;
        
        // Time cubic interpolation
        start = clock();
        for (Size i = 0; i < numEvaluations; ++i) {
            Real testX = static_cast<Real>(i % (n-1)) / 10.0 + 0.05;
            volatile Real result = cubic(testX);
            (void)result;
        }
        clock_t cubicTime = clock() - start;
        
        std::cout << "Rational interpolation: " << rationalTime << " ticks" << std::endl;
        std::cout << "Linear interpolation:   " << linearTime << " ticks" << std::endl;
        std::cout << "Cubic interpolation:    " << cubicTime << " ticks" << std::endl;
        
        std::cout << "\nRelative performance (vs linear):" << std::endl;
        std::cout << "Rational: " << std::fixed << std::setprecision(2) 
                  << static_cast<Real>(rationalTime) / linearTime << "x" << std::endl;
        std::cout << "Cubic: " << static_cast<Real>(cubicTime) / linearTime << "x" << std::endl;
    }
}

int main(int, char* []) {
    try {
        std::cout << "QUANTLIB RATIONAL INTERPOLATION DEMONSTRATION" << std::endl;
        std::cout << std::string(80, '=') << std::endl;
        std::cout << "This example demonstrates the rational function interpolation" << std::endl;
        std::cout << "implementation in QuantLib and compares it with linear and" << std::endl;
        std::cout << "cubic spline interpolation methods." << std::endl;
        
        // Run all examples
        mathematicalExample();
        volatilitySmileExample();
        yieldCurveExample();
        performanceTest();
        
        std::cout << "\n\nSUMMARY" << std::endl;
        std::cout << std::string(80, '=') << std::endl;
        std::cout << "Rational interpolation is particularly useful for:" << std::endl;
        std::cout << "- Volatility surfaces with asymptotic behavior" << std::endl;
        std::cout << "- Yield curves requiring smooth extrapolation" << std::endl;
        std::cout << "- Data with known rational function structure" << std::endl;
        std::cout << "- Applications where avoiding overshooting is important" << std::endl;
        
        std::cout << "\nThe method uses Thiele's continued fraction algorithm" << std::endl;
        std::cout << "which provides numerical stability and avoids solving" << std::endl;
        std::cout << "linear systems directly." << std::endl;
        
        return 0;
        
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "unknown error" << std::endl;
        return 1;
    }
}
