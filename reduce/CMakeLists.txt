add_executable(reduce main.cpp)
target_link_libraries(reduce HPX::hpx HPX::wrap_main HPX::iostreams_component)
