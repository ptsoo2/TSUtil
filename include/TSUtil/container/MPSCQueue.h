#pragma once

#include <array>
#include <queue>
#include <functional>
#include <mutex>

#include "TSUtil/synchronize/SpinLock.h"

namespace TSUtil
{
    /// <summary>
    /// n ������, 1 �Һ��� ť
    /// �Һ��ڰ� ������ ������ ������� �� ��
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
        /// ���� �������� ��ó ó������ ���� ��Ҹ� ��� ó���ϱ� ���� �뵵
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
