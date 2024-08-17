#pragma once

#include <mutex>
#include <condition_variable>

namespace TSUtil
{
    struct ThreadWaitState
    {
        ThreadWaitState(time_t deadlineMilliSec);

        // �Ϻ� lost wakeup �߻��� �� ����
        void notify();
        void wait();

    protected:
        time_t deadlineMilliSec_ = 0;

        std::atomic_bool isWait_ = false;
        std::mutex lock_;
        std::condition_variable cond_;
    };
}