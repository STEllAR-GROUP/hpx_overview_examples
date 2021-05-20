#include <hpx/hpx_main.hpp>
#include <hpx/modules/actions.hpp>
#include <hpx/modules/futures.hpp>

// Local Resilience module in HPX
#include <hpx/modules/resiliency.hpp>

#include <exception>
#include <random>

int vaguely_universal_ans()
{
    // Randomly return incorrect ans from time to time.
    // This is our way of adding faults within the code.
    // Actual codes with Silent Data Corruptions will
    // have this nature built in.
    if (std::rand() % 10 == 0)
        return 41;

    // Correct result
    return 42;
}

int throwable_universal_ans()
{
    // Randomly throw exceptions from time to time.
    // This is our way of adding faults within the code.
    // Actual codes with silent Data Corruptions will
    // have this nature built in.
    if (std::rand() % 10 == 0)
        throw std::logic_error("SDC occured!");

    // Correct result
    return 42;
}

bool validate(int result)
{
    // Check if the returned result is correct.
    return result == 42;
}

int vote(std::vector<int>& results)
{
    return results.at(0);
}

int main(int argc, char* argv[])
{
    constexpr int n = 3;
    hpx::parallel::execution::parallel_executor base_exec;

    // Code when no resilience is added. The code can throw exceptions and
    // cause application to crash if not caught.
    hpx::future<int> plain_future_1 = hpx::async(throwable_universal_ans);

    // Code when no resilience is added. The code can return incorrect results!
    hpx::future<int> plain_future_2 = hpx::async(vaguely_universal_ans);

    // Adding resilience to throwable_universal_ans by replicating it n times
    // and returning the first returned result.
    hpx::future<int> resilient_future_1 =
        hpx::resiliency::experimental::async_replicate(
            n, throwable_universal_ans);

    // Adding resilience to vaguely_universal_ans by replicating it n times
    // and validating it against the returned result. First validated result is
    // then returned.
    hpx::future<int> resilient_future_2 =
        hpx::resiliency::experimental::async_replicate_validate(
            n, validate, vaguely_universal_ans);

    // Adding resilience to vaguely_universal_ans by replicating it n times
    // and voting it against the returned result. Result from the consensus
    // function is returned.
    hpx::future<int> resilient_future_2 =
        hpx::resiliency::experimental::async_replicate_vote(
            n, vote, vaguely_universal_ans);

    // Adding resilience to vaguely_universal_ans by replicating it n times
    // and voting it against all the validated result. Result from the consensus
    // function is returned.
    hpx::future<int> resilient_future_2 =
        hpx::resiliency::experimental::async_replicate_vote_validate(
            n, vote, validate, vaguely_universal_ans);

    // Creating resilient replicate executor to pass as an argument to hpx::async
    auto exec =
        hpx::resiliency::experimental::make_replicate_executor(base_exec, n);
    hpx::future<int> resilient_executor_future =
        hpx::async(exec, vaguely_universal_ans);

    hpx::wait_all(plain_future_1, plain_future_2, resilient_future_1,
        resilient_future_2, resilient_executor_future);

    return 0;
}