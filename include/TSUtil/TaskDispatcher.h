#pragma once

#include <functional>
#include <map>

#include "TSUtil/container/MPSCQueue.h"
#include "TSUtil/synchronize/ThreadWaitState.h"
#include "TSUtil/ThreadPool.h"

namespace TSUtil
{
    struct TaskDispatcherOption
    {
        [[nodiscard]] bool verify() const;

    public:
        /// <summary>
        /// Ǯ���� ������ ����
        /// </summary>
        size_t threadCount_ = 1u;

        /// <summary>
        /// �½�ũ ó�� üũ ����
        /// </summary>
        time_t deadlineMilliSec_ = 0;
    };
}

namespace TSUtil
{
    class TaskDispatcher
    {
    public:
        using fnTask_t = std::function<void()>;

    protected:
        struct Runnable
        {
            using queTask_t = MPSCQueue<fnTask_t>;

        public:
            Runnable(time_t deadlineMilliSec);

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
        void start(const TaskDispatcherOption& option);
        void stop(bool isIncludeFlush = false);
        void flush();

        void post(size_t tid, fnTask_t&& task);
        void post(fnTask_t&& task);

        [[nodiscard]] size_t currentThreadIndex() const { return threadPool_.currentThreadIndex(); }

    protected:
        [[nodiscard]] size_t _gentid();

    protected:
        ThreadPool threadPool_;
        lstRunnable_t lstRunnable_;

        std::atomic_size_t postKeyGenerator_ = 0;
    };
}
