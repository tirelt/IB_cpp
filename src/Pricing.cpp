#include <iostream>
#include <cmath>
#include "Slice.h"
#include "Pricing.h"
#include <functional>

using std::function;
using std::isnan;

double normal_cdf(double x, double mean = 0.0, double stddev = 1.0) {
    return 0.5 * (1.0 + std::erf((x - mean) / (stddev * std::sqrt(2))));
}

double gaussian_density(double x, double mean = 0.0, double stddev = 1.0) {
    return (1.0 / (stddev * std::sqrt(2 * M_PI))) * std::exp(-0.5 * std::pow((x - mean) / stddev, 2));
}

double black_formula( const Option::Right right, const double forward, const double strike, const double vol, const double time_to_maturity, const double discount ){
   double temp = vol * std::sqrt( time_to_maturity); 
   double d_1 = ( std::log(forward/strike) + std::pow(temp,2) / 2 ) / temp; 
   double d_2 = d_1 - temp;
   if(right == Option::CALL)
    return discount*( forward * normal_cdf( d_1) - strike * normal_cdf( d_2) );
    else
    return discount*( strike * normal_cdf( -d_2) - forward * normal_cdf( -d_1) );
}

double vega( const double forward, const double strike, const double vol, const double time_to_maturity, const double discount ){
    double temp = vol * std::sqrt( time_to_maturity); 
    double d_1 = ( std::log(forward/strike) + std::pow(temp,2) / 2 ) / temp; 
    return discount * forward * gaussian_density( d_1) * std::sqrt( time_to_maturity);
}

// Newton's method to find the root, thank you copilot
double newton_method( function<double(double)> f, function<double(double)> d, double initialGuess, const double tolerance, const int maxIterations ){
    double x = initialGuess;
    for (int i = 0; i < maxIterations; ++i) {
        double fx = f(x);
        if(isnan(fx) || fx<0.00000001)
            return -1;
        double dfx = d(x);
        if(isnan(dfx) || dfx<0.00000001)
            return -1;
        double xNext = x - fx / dfx;
        if (std::fabs(xNext - x) < tolerance){
            return xNext;
        }
        x = xNext;
    }
    return x;
}