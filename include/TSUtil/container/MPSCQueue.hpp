#include "TSUtil/container/MPSCQueue.h"

namespace TSUtil
{
    template<typename TElement>
    inline MPSCQueue<TElement>::MPSCQueue()
        : readQueue_(&lstQueue_[READ])
        , writeQueue_(&lstQueue_[WRITE])
    {}

    template<typename TElement>
    inline void MPSCQueue<TElement>::push(const TElement& element)
    {
        std::lock_guard<SpinLock> _{ lock_ };
        writeQueue_->push(element);
    }

    template<typename TElement>
    inline void MPSCQueue<TElement>::emplace(TElement&& element)
    {
        std::lock_guard<SpinLock> _{ lock_ };
        writeQueue_->emplace(std::forward<TElement>(element));
    }

    template<typename TElement>
    inline size_t MPSCQueue<TElement>::consumeReadQueue(fnIter_t&& fnIter)
    {
        if (_swap() == false)
            return 0;

        return _consumeQueue(readQueue_, std::forward<fnIter_t>(fnIter));
    }

    template<typename TElement>
    inline size_t MPSCQueue<TElement>::consumeBothQueue(fnIter_t fnIter)
    {
        // readQueue => writeQueue ������ ���� ó���ϸ鼭 ����.
        size_t consumeCount = 0;

        consumeCount += _consumeQueue(readQueue_, fnIter);
        consumeCount += _consumeQueue(writeQueue_, fnIter);

        return consumeCount;
    }

    template<typename TElement>
    inline bool MPSCQueue<TElement>::_swap()
    {
        // ������ �ʿ䰡 �ִ� ��츸 ����
        if (readQueue_->empty() == true)
        {
            std::lock_guard<SpinLock> _{ lock_ };
            std::swap(readQueue_, writeQueue_);
        }

        // ó���Ұ� �ִ���
        return (readQueue_->empty() == false);
    }

    template<typename TElement>
    inline size_t MPSCQueue<TElement>::_consumeQueue(queue_t* queue, fnIter_t fnIter)
    {
        size_t consumeCount = 0;
        while (true)
        {
            if (queue->empty() == true)
                break;

            fnIter(queue->front());
            queue->pop();
        }

        return consumeCount;
    }
}
