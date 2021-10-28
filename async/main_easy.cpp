#include <hpx/hpx.hpp>
#include <cmath>
#include <iostream>

// Define the function for the factorial
double factorial(size_t n)
{
    double factorial = 0;
    for(int i = 1; i <= n; ++i) 
    {
        factorial *= i;
    }

    return factorial;
}

// Define the partial taylor function
double taylor(size_t begin, size_t end, size_t n, double x)
{
 double denom = factorial(2 * n);
 double res = 0;
 for (size_t i = begin; i != end; ++i)
 {
 res += std::pow(-1, i - 1) * std::pow(x, 2 * n) / denom;
 }
 return res;
}

int main()
{
 // Compute the Talor series sin(2.0) for 100 iterations
 size_t n = 100;

 // Launch two concurrent computations of each partial result
 hpx::future<double> f1 = hpx::async(taylor, 0, n / 2, n, 2.);
 hpx::future<double> f2 = hpx::async(taylor, (n / 2) + 1, n, n, 2.);

 // Introduce a barrier to gather the results
 double res = f1.get() + f2.get();

 // Print the result
 std::cout << "Sin(2.) = " << res << std::endl;
}
