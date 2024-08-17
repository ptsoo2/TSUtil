#pragma once

#include <array>
#include <queue>
#include <functional>
#include <mutex>

#include "TSUtil/synchronize/SpinLock.h"

namespace TSUtil
{
    /// <summary>
    /// n 생산자, 1 소비자 큐
    /// 소비자가 여럿일 때에는 사용하지 말 것
    /// </summary>
    /// <typeparam name="TElement"></typeparam>
    template <typename TElement>
    class MPSCQueue
    {
    public:
        enum
        {
            READ = 0,
            WRITE,
            _MAX_,
        };

        using queue_t = std::queue<TElement>;
        using lstQueue_t = std::array<queue_t, _MAX_>;
        using fnIter_t = std::function<void(TElement&)>;

    public:
        MPSCQueue();

    public:
        void push(const TElement& element);
        void emplace(TElement&& element);

        size_t consumeReadQueue(fnIter_t&& fnIter);

        /// <summary>
        /// 종료 절차에서 미처 처리하지 못한 요소를 모두 처리하기 위한 용도
        /// </summary>
        /// <param name="fnIter"></param>
        size_t consumeBothQueue(fnIter_t fnIter);

    protected:
        [[nodiscard]] bool _swap();
        size_t _consumeQueue(queue_t* queue, fnIter_t fnIter);

    protected:
        SpinLock lock_;

        lstQueue_t lstQueue_;
        queue_t* readQueue_ = nullptr;
        queue_t* writeQueue_ = nullptr;
    };
}

#include "MPSCQueue.hpp"
