#include <functional>

double normal_cdf(double x, double mean, double stddev );

double gaussian_density(double x, double mean , double stddev );

double normal_cdf(double x, double mean , double stddev);

double black_formula( const Option::Right right, const double forward, const double strike, const double vol, const double time_to_maturity, const double discount );

double vega( const double forward, const double strike, const double vol, const double time_to_maturity, const double discount );

double newton_method( std::function<double(double)> f, std::function<double(double)> d, double initialGuess, const double tolerance, const int maxIterations);