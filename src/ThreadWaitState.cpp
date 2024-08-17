#include "TSUtil/synchronize/ThreadWaitState.h"

namespace TSUtil
{
    ThreadWaitState::ThreadWaitState(time_t deadlineMilliSec)
        : deadlineMilliSec_(deadlineMilliSec)
    {
    }

    void ThreadWaitState::notify()
    {
        // isWait 이 켜져있는 상태라면 끄고, notify 
        bool expected = true;
        if (isWait_.compare_exchange_strong(expected, false) == true)
        {
            cond_.notify_one();
        }
    }

    void ThreadWaitState::wait()
    {
        isWait_.store(true);

        std::unique_lock<std::mutex> grab{ lock_ };
        cond_.wait_for(grab, std::chrono::milliseconds(deadlineMilliSec_),
            [this]()
            {
                // isWait 이 꺼져있는 상태라면 탈출
                bool expected = false;
                return isWait_.compare_exchange_strong(expected, false) == true;
            }
        );
    }
}