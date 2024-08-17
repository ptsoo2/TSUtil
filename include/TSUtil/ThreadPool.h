#pragma once

#include <vector>
#include <thread>
#include <map>
#include <functional>
#include <stdexcept>

namespace TSUtil
{
    class ThreadPool
    {
        using lstThread_t = std::vector<std::thread>;
        using mapThreadIndex_t = std::map<std::thread::id, size_t>;
        using fnRun_t = std::function<void(size_t)>;

    public:
        ~ThreadPool() noexcept(false);

    public:
        void start(size_t size, fnRun_t&& fnRun);
        void stop();

        [[nodiscard]] bool isRun() const { return (cancellation_.stop_requested() == false); }

        [[nodiscard]] const size_t* findThreadIndex(const std::thread::id& id) const;
        [[nodiscard]] size_t currentThreadIndex() const;

    protected:
        std::stop_source cancellation_;
        lstThread_t lstThread_;
        mapThreadIndex_t mapThreadIndex_;
    };
}
