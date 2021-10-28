//  Copyright (c) 2014-2020 STE||AR GROUP

#include <hpx/algorithm.hpp>
#include <hpx/hpx_init.hpp>
#include <hpx/iostream.hpp>

#include <algorithm>
#include <vector>

int hpx_main(int, char **) {
  std::vector<int> v(10, 1);

  ///////////////////////////////////////////////////////////////
  auto sum1 = std::reduce(std::begin(v), std::end(v), 0);

  auto sum2 = hpx::reduce(v.begin(), v.end(), 0);
  
  auto sum3 = hpx::reduce(hpx::execution::seq, v.begin(), v.end(), 0);

  auto sum4 = hpx::reduce(hpx::execution::par, v.begin(), v.end(), 0);

  ///////////////////////////////////////////////////////////////
  auto  sum5 = hpx::reduce(hpx::execution::par(hpx::execution::task), v.begin(), v.end(), 0);
  //sum5.get();

  ///////////////////////////////////////////////////////////////
  auto sum6 = hpx::ranges::reduce(v, 0);
  
  auto sum7 = hpx::ranges::reduce(hpx::execution::par, v, 0);

  hpx::cout << sum1 << " " << sum2 << " " << sum3 << " " << sum4 << hpx::endl;
  hpx::cout << sum4 << hpx::endl;
  //hpx::cout << sum5 << " " << sum7 << hpx::endl;


  return hpx::finalize();
}

int main(int argc, char *argv[]) 
{ 
  return hpx::init(argc, argv);
}
