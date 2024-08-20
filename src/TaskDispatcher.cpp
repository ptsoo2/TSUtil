#include "TSUtil/TaskDispatcher.h"

namespace TSUtil
{
    bool TaskDispatcherOption::verify() const
    {
        return (threadCount_ < 1)
            ? false
            : true;
    }
}

namespace TSUtil
{
    TaskDispatcher::Runnable::Runnable(time_t deadlineMilliSec)
        : waitState_(deadlineMilliSec)
    {}

    void TaskDispatcher::Runnable::post(fnTask_t&& task)
    {
        queTask_.emplace(std::forward<fnTask_t>(task));
        waitState_.notify();
    }

    void TaskDispatcher::Runnable::flush()
    {
        queTask_.consumeBothQueue(
            [](auto& element)
            {
                element();
            }
        );
    }

    void TaskDispatcher::Runnable::run()
    {
        const size_t consumeCount = queTask_.consumeReadQueue(
            [](fnTask_t& fnTask)
            {
                fnTask();
            }
        );

        // 처리한게 아무 것도 없었다면 wait
        if (consumeCount == 0)
            waitState_.wait();
    }

    void TaskDispatcher::start(const TaskDispatcherOption& option)
    {
        if (threadPool_.isRun() == true)
            throw std::runtime_error("Already initialized");

        if (option.verify() == false)
            throw std::runtime_error("Invalid thread count");

        // runnable 생성
        for (size_t i = 0; i < option.threadCount_; ++i)
            lstRunnable_.emplace_back(option.deadlineMilliSec_);

        // pool 생성
        threadPool_.start(option.threadCount_,
            [this](size_t tid)
            {
                Runnable& executor = lstRunnable_.at(tid);
                executor.run();
            }
        );
    }

    void TaskDispatcher::stop(bool isIncludeFlush /*= false*/)
    {
        // 순서 중요
        threadPool_.stop();

        // ptsoo todo - waitState 에서 기다리는 시간이 길다면 여기서 깨우는 절차가 누락되므로 종료 절차가 길어질 수 있다.

        if (isIncludeFlush == true)
            flush();
    }

    void TaskDispatcher::flush()
    {
        for (Runnable& runnable : lstRunnable_)
            runnable.flush();
    }

    void TaskDispatcher::post(size_t tid, fnTask_t&& task)
    {
        if (threadPool_.isRun() == false)
            throw std::runtime_error("Not running");

        {
            const size_t size = lstRunnable_.size();
            if (tid >= size)
                tid %= size;
        }

        Runnable& runnable = lstRunnable_.at(tid);
        runnable.post(std::forward<fnTask_t>(task));
    }

    void TaskDispatcher::post(fnTask_t&& task)
    {
        // round robin 으로 runnable 을 선택해서 post
        post(_gentid(), std::forward<fnTask_t>(task));
    }

    size_t TaskDispatcher::_gentid()
    {
        // round robin
        size_t tid = postKeyGenerator_.fetch_add(1);
        tid %= lstRunnable_.size();
        return tid;
    }
}
