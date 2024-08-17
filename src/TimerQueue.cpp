#include "TSUtil/container/TimerQueue.h"

namespace TSUtil
{
    void TimerQueue::start(TimerQueueOption&& timerOption)
    {
        if (thread_ != nullptr)
            throw std::runtime_error("Already started");

        if (cancellation_.stop_requested() == true)
            throw std::runtime_error("Already stopped");

        timerOption_ = std::move(timerOption);

        waitState_ = std::make_unique<ThreadWaitState>(timerOption_.deadlineMilliSec_);
        thread_ = std::make_unique<std::thread>(
            [this]()
            {
                _run();
            }
        );
    }

    void TimerQueue::stop()
    {
        if (thread_ == nullptr)
            throw std::runtime_error("Not started");

        if (cancellation_.stop_requested() == true)
            throw std::runtime_error("Already stopped");

        cancellation_.request_stop();
        waitState_->notify();
        thread_->join();
    }

    timerKey_t TimerQueue::addTimer(time_t milliSec, fnTask_t&& fnTask)
    {
        const timerKey_t newTimerKey = _genTimerKey();

        {
            context_t context;
            context.type_ = REQUEST_TYPE::ADD;
            context.key_ = newTimerKey;
            context.fnTask_ = std::move(fnTask);
            context.deadline_ = (clock_t::now() + std::chrono::milliseconds(milliSec));

            quePendingRequest_.emplace(std::move(context));
        }

        waitState_->notify();
        return newTimerKey;
    }

    void TimerQueue::removeTimer(timerKey_t key)
    {
        context_t context;
        context.type_ = REQUEST_TYPE::REMOVE;
        context.key_ = key;

        quePendingRequest_.emplace(std::move(context));

        waitState_->notify();
    }

    void TimerQueue::_run()
    {
        size_t runCount = 0;

        while (cancellation_.stop_requested() == false)
        {
            runCount += _runDeadline();
            runCount += _runPendingRequest();

            // 처리한게 아무 것도 없었다면 wait
            if (runCount == 0)
                waitState_->wait();
        }
    }

    size_t TimerQueue::_runDeadline()
    {
        const auto now = clock_t::now();
        size_t remainCount = timerOption_.maxOnceThroughput_;
        size_t runCount = 0;

        while (true)
        {
            if (setTimer_.empty() == true)
                break;

            if (remainCount == 0)
                break;

            auto iter = setTimer_.begin();
            if (iter->deadline_ > now)
                break;

            context_t context = *iter;
            context();

            _removeTimer(std::move(context));

            --remainCount;
            ++runCount;
        }

        return runCount;
    }

    size_t TimerQueue::_runPendingRequest()
    {
        return quePendingRequest_.consumeReadQueue(
            [this](context_t& context)
            {
                switch (context.type_)
                {
                case REQUEST_TYPE::ADD:
                    _addTimer(std::move(context));
                    break;
                case REQUEST_TYPE::REMOVE:
                    _removeTimer(std::move(context));
                    break;
                default:
                    throw std::runtime_error("invalid request type");
                }
            }
        );
    }

    void TimerQueue::_addTimer(context_t&& context)
    {
        auto [iter, isSuccess] = setTimer_.emplace(context);
        if (isSuccess == false)
            throw std::runtime_error("already exist");

        mapTimer_.emplace(context.key_, iter);
    }

    void TimerQueue::_removeTimer(context_t&& context)
    {
        const auto findIter = mapTimer_.find(context.key_);
        if (findIter == mapTimer_.end())
        {
            // 이미 실행되어 버린 경우
            return;
        }

        setTimer_.erase(findIter->second);
        mapTimer_.erase(findIter->first);
    }
}
