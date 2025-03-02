
double normal_cdf(double x, double mean, double stddev );

double gaussian_density(double x, double mean , double stddev );

double normal_cdf(double x, double mean , double stddev);

double black_formula( double forward, double strike, double vol, double time_to_maturity, double discount );

double vega( double forward, double strike, double vol, double time_to_maturity, double discount );

double newtonMethod( double (*f)(double ), double (*d)(double ), double initialGuess, double tolerance, int maxIterations);