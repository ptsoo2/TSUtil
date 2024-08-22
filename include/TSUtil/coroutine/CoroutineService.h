#pragma once

#include "TSUtil/TaskDispatcher.h"
#include "TSUtil/container/TimerQueue.h"

namespace TSUtil::coro
{
    /// <summary>
    /// 코루틴 사용시 런타임에 전역적으로 사용이 필요한 인스턴스 관리
    /// </summary>
    class CoroutineService final
    {
    protected:
        CoroutineService() = default;

    public:
        static auto& getInstance()
        {
            static CoroutineService instance;
            return instance;
        }

    public:
        void start()
        {
            timerQueue_.start(std::move(timerQueueOption_));
            defaultDispatcher_.start(dispatcherOption_);
        }

        void stop()
        {
            timerQueue_.stop();
            defaultDispatcher_.stop();
        }

    public:
        [[nodiscard]] auto defaultDispatcher() { return &defaultDispatcher_; }
        [[nodiscard]] auto timerQueue() { return &timerQueue_; }

        [[nodiscard]] auto& configureTimerQueue(TimerQueueOption option)
        {
            timerQueueOption_ = std::move(option);
            return *this;
        }

    protected:
        TimerQueueOption timerQueueOption_;
        TimerQueue timerQueue_;

        // ptsoo todo - 현재 다른 디스패처를 쓰고 싶어도 쓸 수가 없다. 이 옵션을 제공해야 한다.
        TaskDispatcherOption dispatcherOption_;
        TaskDispatcher defaultDispatcher_;
    };
}
