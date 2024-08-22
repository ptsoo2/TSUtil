#pragma once

#include <mutex>
#include <condition_variable>

namespace TSUtil
{
    struct ThreadWaitState
    {
        explicit ThreadWaitState(time_t deadlineMilliSec);

        // 일부 lost wakeup 발생할 수 있음
        void notify();
        void wait();

    protected:
        time_t deadlineMilliSec_ = 0;

        std::atomic_bool isWait_ = false;
        std::mutex lock_;
        std::condition_variable cond_;
    };
}
