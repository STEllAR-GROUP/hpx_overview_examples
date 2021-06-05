set(examples
    distributed_resilience
    resilience_replay
    resilience_replicate
)

foreach(example ${examples})
    add_executable(${example} ${example}.cpp)
    target_link_libraries(${example} HPX::hpx HPX::wrap_main)    
endforeach(example ${examples})
