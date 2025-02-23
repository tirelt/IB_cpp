#include <iostream>
#include <cmath>
#include "Slice.h"

double normal_cdf(double x, double mean = 0.0, double stddev = 1.0) {
    return 0.5 * (1.0 + std::erf((x - mean) / (stddev * std::sqrt(2))));
}

double black_formula( Option::Right right, double forward, double strike, double vol, double time_to_maturity, double discount ){
   double temp = vol * std::sqrt( time_to_maturity); 
   double d_1 = ( std::log(forward/strike) + std::pow(temp,2) / 2 ) / temp; 
   double d_2 = d_1 - temp;
   if(right == Option::CALL)
    return discount*( forward * normal_cdf( d_1) - strike * normal_cdf( d_2) );
    else
    return discount*( strike * normal_cdf( -d_2) - forward * normal_cdf( -d_1) );
}