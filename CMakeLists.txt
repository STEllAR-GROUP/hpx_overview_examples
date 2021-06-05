cmake_minimum_required(VERSION 3.13)

project(hpx_overview CXX)

find_package(HPX REQUIRED)

add_subdirectory(reduce)
add_subdirectory(resilience)
