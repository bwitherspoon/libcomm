/*
 * Copyright 2015 C. Brett Witherspoon
 */

#include <stdexcept>

#include <pthread.h>

namespace signum
{
namespace utility
{

void set_realtime_priority()
{
    sched_param param = { sched_get_priority_max(SCHED_RR) };

    if (pthread_setschedparam(pthread_self(), SCHED_RR, &param) != 0)
        throw std::runtime_error("Unable to set scheduling priority");
}

void set_normal_priority()
{
    sched_param param = { sched_get_priority_min(SCHED_OTHER) };

    if (pthread_setschedparam(pthread_self(), SCHED_OTHER, &param) != 0)
        throw std::runtime_error("Unable to set scheduling priority");
}


} // end namespace utility
} // end namespace signum
