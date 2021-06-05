//  Copyright (c) 2021 Nikunj Gupta
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/hpx_main.hpp>
#include <hpx/include/runtime.hpp>
#include <hpx/modules/actions.hpp>
#include <hpx/modules/async_distributed.hpp>
#include <hpx/modules/futures.hpp>
#include <hpx/modules/resiliency_distributed.hpp>

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

HPX_PLAIN_ACTION(vaguely_universal_ans, vaguely_universal_action);
HPX_PLAIN_ACTION(throwable_universal_ans, throwable_universal_action);

bool validate(int ans)
{
    // Check if the returned result is correct.
    return ans == 42;
}

int vote(std::vector<int>& results)
{
    return results.at(0);
}

int main(int argc, char* argv[])
{
    std::vector<hpx::id_type> locales = hpx::find_all_localities();

    // Allow task to replay on the same locality if there is only 1 locality
    if (locales.size() == 1)
        locales.insert(locales.end(), 9, hpx::find_here());

    throwable_universal_action throwable_action;
    vaguely_universal_action vague_action;

    // Code when no resilience is added. The code throw exceptions and
    // cause application to crash if not caught.
    hpx::future<int> plain_future_1 =
        hpx::async(throwable_action, hpx::find_here());

    // Code when no resilience is added. The code can return incorrect resutls!
    hpx::future<int> plain_future_2 =
        hpx::async(vague_action, hpx::find_here());

    // Adding replay resilience to throwable action by allowing it to "re-run"
    // up to n times before giving up.
    hpx::future<int> resilient_replay_future =
        hpx::resiliency::experimental::async_replay(locales, throwable_action);

    // Adding resilience to throwable_universal_ans by replicating it n times
    // and returning the first returned result.
    hpx::future<int> resilient_replicate_future =
        hpx::resiliency::experimental::async_replicate(locales, vague_action);

    // Adding resilience to vaguely_universal_ans by validating the result
    // returned and retrying up to n times if incorrect.
    hpx::future<int> resilient_replay_future_2 =
        hpx::resiliency::experimental::async_replay_validate(
            locales, validate, vague_action);

    // Adding resilience to vaguely_universal_ans by replicating it n times
    // and validating it against the returned result. First validated result is
    // then returned.
    hpx::future<int> resilient_replicate_future_2 =
        hpx::resiliency::experimental::async_replicate_validate(
            locales, validate, vague_action);

    // Adding resilience to vaguely_universal_ans by replicating it n times
    // and voting it against the returned result. Result from the consensus
    // function is returned.
    hpx::future<int> resilient_replicate_future_3 =
        hpx::resiliency::experimental::async_replicate_vote(
            locales, vote, vague_action);

    // Adding resilience to vaguely_universal_ans by replicating it n times
    // and voting it against all the validated result. Result from the consensus
    // function is returned.
    hpx::future<int> resilient_replicate_future_4 =
        hpx::resiliency::experimental::async_replicate_vote_validate(
            locales, vote, validate, vague_action);

    hpx::wait_all(plain_future_1, plain_future_2, resilient_replay_future,
        resilient_replay_future_2, resilient_replicate_future,
        resilient_replicate_future_2, resilient_replicate_future_3,
        resilient_replicate_future_4);
    
    return 0;
}