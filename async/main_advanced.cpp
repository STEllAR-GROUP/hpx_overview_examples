#include <hpx/hpx_main.hpp>   
#include <hpx/include/lcos.hpp>
#include <cmath>
#include <iostream>
#include <vector>

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

int main(int argc, char *argv[])
{
 // Compute the Talor series sin(2.0) for 100 iterations
 size_t n = 100;
 size_t partitions = 2;
 std::vector<hpx::future<double>> res; 

 // Launch the computation
 for(size_t i = 0 ; i < partitions; i++)
	 res.push_back(hpx::async(taylor, i*n/2, (i+1)*n/2, n, 2.));


 hpx::when_all(res).then([](auto&&f){
 	double res = 0;
	std::vector<hpx::future<double>> futures = f.get();
	for(size_t i =0; i < futures.size(); i++)
		res += futures[i].get();
 	// Print the result
 	std::cout << "Sin(2.) = " << res << std::endl;
	});
}
