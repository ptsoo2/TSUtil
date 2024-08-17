#include "TSUtil/ThreadPool.h"

namespace TSUtil
{
    ThreadPool::~ThreadPool() noexcept(false)
    {
        if (isRun() == true)
            throw std::runtime_error("Not stopped");
    }

    void ThreadPool::start(size_t size, fnRun_t&& fnRun)
    {
        if (isRun() == true)
            throw std::runtime_error("Already start");

        lstThread_.reserve(size);

        for (size_t idx = 0; idx < size; ++idx)
        {
            auto fnInit =
                [idx, stopToken = cancellation_.get_token(), fnRun]()
                {
                    while (stopToken.stop_requested() == false)
                        fnRun(idx);
                };

            std::thread& thread = lstThread_.emplace_back(std::move(fnInit));
            mapThreadIndex_.emplace(thread.get_id(), idx);
        }
    }

    void ThreadPool::stop()
    {
        if (isRun() == false)
            throw std::runtime_error("Not running");

        cancellation_.request_stop();

        for (std::thread& thread : lstThread_)
        {
            if (thread.joinable() == true)
                thread.join();
        }
    }

    const size_t* ThreadPool::findThreadIndex(const std::thread::id& id) const
    {
        auto findIter = mapThreadIndex_.find(id);
        return (findIter == mapThreadIndex_.end())
            ? nullptr
            : &(findIter->second);
    }

    size_t ThreadPool::currentThreadIndex() const
    {
        const size_t* findIdx = findThreadIndex(std::this_thread::get_id());
        if (findIdx == nullptr)
            throw std::runtime_error("Not found thread index");

        return *findIdx;
    }
}
