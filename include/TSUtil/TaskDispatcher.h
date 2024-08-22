#pragma once

#include <functional>
#include <map>

#include "TSUtil/container/MPSCQueue.h"
#include "TSUtil/synchronize/ThreadWaitState.h"
#include "TSUtil/ThreadPool.h"

namespace TSUtil
{
    /// <summary>
    /// TaskDispatcher �� ���� ���� ���� ���
    /// </summary>
    struct TaskDispatchPoint
    {
        TaskDispatchPoint() = default;
        explicit TaskDispatchPoint(class TaskDispatcher* dispatcher, size_t threadIdx)
            : dispatcher_(dispatcher)
            , threadIdx_(threadIdx)
        {}

        class TaskDispatcher* dispatcher_ = nullptr;
        size_t threadIdx_ = 0;
    };
}

namespace TSUtil
{
    /// <summary>
    /// ���� �ɼ�
    /// </summary>
    struct TaskDispatcherOption
    {
        [[nodiscard]] bool verify() const;

    public:
        /// <summary>
        /// Ǯ���� ������ ����
        /// </summary>
        size_t threadCount_ = std::thread::hardware_concurrency();

        /// <summary>
        /// �½�ũ ó�� üũ ����
        /// </summary>
        time_t deadlineMilliSec_ = 0;
    };
}

namespace TSUtil
{
    /// <summary>
    /// �񵿱�� ó���� �½�ũ�� ������ Ǯ�� �й�
    /// </summary>
    class TaskDispatcher
    {
    public:
        using fnTask_t = std::function<void()>;

    protected:
        /// <summary>
        /// ������ ���ν���
        /// </summary>
        struct Runnable
        {
            using queTask_t = MPSCQueue<fnTask_t>;

        public:
            explicit Runnable(time_t deadlineMilliSec);

        public:
            void post(fnTask_t&& task);
            void flush();
            void run();

        public:
            queTask_t queTask_;
            ThreadWaitState waitState_;
        };

        using lstRunnable_t = std::deque<Runnable>;

    public:
        virtual ~TaskDispatcher() = default;

    public:
        void start(const TaskDispatcherOption& option);
        void stop(bool isIncludeFlush = false);
        void flush();

        void dispatch(size_t tid, fnTask_t&& task);
        void dispatch(fnTask_t&& task);

        [[nodiscard]] size_t currentThreadIndex() const { return threadPool_.currentThreadIndex(); }
        [[nodiscard]] TaskDispatchPoint dispatchPoint() { return TaskDispatchPoint{ this, currentThreadIndex() }; }
        [[nodiscard]] size_t genThreadId();

    protected:
        ThreadPool threadPool_;
        lstRunnable_t lstRunnable_;

        std::atomic_size_t postKeyGenerator_ = 0;
    };
}
