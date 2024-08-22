#pragma once

#include <set>
#include <map>
#include <chrono>
#include <thread>
#include <functional>
#include <stop_token>

#include "TSUtil/synchronize/ThreadWaitState.h"
#include "TSUtil/container/MPSCQueue.h"
#include "TSUtil/constraint/chrono.h"

using timerKey_t = uint64_t;

namespace TSUtil
{
    /// <summary>   
    /// Ÿ�̸� ���۰� ���õ� �ɼ�
    /// </summary>
    struct TimerQueueOption
    {
        using fnHookExecute_t = std::function<void(std::function<void()>&&)>;

        /// <summary>
        /// Ÿ�̸� üũ ����
        /// </summary>
        time_t deadlineMilliSec_ = 100;

        /// <summary>
        /// ƽ�� �ִ� ó����
        /// </summary>
        size_t maxOnceThroughput_ = 10;

        /// <summary>
        /// Ÿ�̸� ���� ������ task ����� ��ŷ ó��
        /// 
        /// Ÿ�̸� �ð� üũ ó���� ���� �����忡�� �����ϴ� ���踦 �����Ƿ� 
        /// �½�ũ ������ ���� �����忡�� ����Ǿ�� �Ѵ�.
        /// Ÿ�̸� �����忡�� �����ϴ� ��� Ÿ�̸� ó���� �����ϴ� ��Ұ� �� ���̴�.
        /// </summary>
        fnHookExecute_t fnHookExecute_;
    };
}

namespace TSUtil
{
    /// <summary>
    /// Ÿ�̸� ť
    /// ���� �����带 ����ϹǷ� start, stop ȣ���� �ʿ�
    /// </summary>
    class TimerQueue
    {
        using fnTask_t = std::function<void()>;
        using clock_t = std::chrono::steady_clock;

        enum class REQUEST_TYPE
        {
            ADD,
            REMOVE,
        };

        struct context_t
        {
            REQUEST_TYPE type_;
            timerKey_t key_;

            fnTask_t fnTask_;
            clock_t::time_point deadline_;

            bool operator<(const context_t& rhs) const { return deadline_ < rhs.deadline_; }
            void operator()() const { fnTask_(); }
        };

        using quePendingRequest_t = MPSCQueue<context_t>;
        using setTimer_t = std::set<context_t>;
        using mapTimer_t = std::map<timerKey_t, setTimer_t::const_iterator>;

    public:
        void start(TimerQueueOption&& timerOption);
        void stop();

        template <constraint_duration TDuration>
        [[nodiscard]] timerKey_t addTimer(const TDuration& duration, fnTask_t&& fnTask);
        [[nodiscard]] timerKey_t addTimer(time_t milliSec, fnTask_t&& fnTask);

        void removeTimer(timerKey_t key);

    protected:
        void _run();
        size_t _runDeadline();
        size_t _runPendingRequest();

        void _addTimer(context_t&& context);
        void _removeTimer(context_t&& context);

        [[nodiscard]] clock_t::time_point _getNowTime() const { return clock_t::now(); }
        [[nodiscard]] timerKey_t _genTimerKey() { return timerKeyGenerator_.fetch_add(1); }

    protected:
        TimerQueueOption timerOption_;

        std::stop_source cancellation_;

        std::unique_ptr<ThreadWaitState> waitState_;
        std::unique_ptr<std::thread> thread_;
        std::thread::id threadId_;

        quePendingRequest_t quePendingRequest_;
        setTimer_t setTimer_;
        mapTimer_t mapTimer_;

        std::atomic_uint64_t timerKeyGenerator_ = 0;
    };
}

namespace TSUtil
{
    template <constraint_duration TDuration>
    inline timerKey_t TimerQueue::addTimer(const TDuration& duration, fnTask_t&& fnTask)
    {
        const timerKey_t newTimerKey = _genTimerKey();

        auto wrappedTask =
            [timerThreadId = threadId_, fnTask = std::forward<fnTask_t>(fnTask)]()
            {
                // �½�ũ�� Ÿ�̸� ������ ������ ����Ǹ� �ȵȴ�.
                if (timerThreadId == std::this_thread::get_id())
                    throw std::runtime_error("TimerQueue task must be called from another thread.");

                fnTask();
            };

        {
            context_t context;
            context.type_ = REQUEST_TYPE::ADD;
            context.key_ = newTimerKey;
            context.deadline_ = (_getNowTime() + duration);
            context.fnTask_ =
                [fnHookExecute = timerOption_.fnHookExecute_, wrappedTask = std::move(wrappedTask)]()
                {
                    fnHookExecute(std::move(wrappedTask));
                };

            quePendingRequest_.emplace(std::move(context));
        }

        waitState_->notify();
        return newTimerKey;
    }
}
