#pragma once

#include <atomic>
#include <thread>

namespace TSUtil
{
    class SpinLock
    {
        enum
        {
            MAX_SPIN_COUNT = 1 << 7, // 128
        };

        using fnSleep_t = void(*)();

    public:
        [[nodiscard]] bool try_lock();
        void lock();
        void unlock();

    protected:
        [[nodiscard]] bool _try_lock(fnSleep_t fnSleep);

    protected:
        std::atomic_flag flag_ = ATOMIC_FLAG_INIT;
    };
}
