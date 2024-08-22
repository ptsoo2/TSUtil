#pragma once

#include "TSUtil/TaskDispatcher.h"
#include "TSUtil/container/TimerQueue.h"

namespace TSUtil::coro
{
    /// <summary>
    /// �ڷ�ƾ ���� ��Ÿ�ӿ� ���������� ����� �ʿ��� �ν��Ͻ� ����
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

        // ptsoo todo - ���� �ٸ� ����ó�� ���� �; �� ���� ����. �� �ɼ��� �����ؾ� �Ѵ�.
        TaskDispatcherOption dispatcherOption_;
        TaskDispatcher defaultDispatcher_;
    };
}
