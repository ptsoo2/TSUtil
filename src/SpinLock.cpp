#include "TSUtil/synchronize/SpinLock.h"

/*
*	_mm_pause						: https://yonmy.com/archives/99 (��� �ӵ��� �������ν� ������������ �� ���� �κ��� ���)// �̳����� sleep �̶� �����ϸ� �ȴ�. (us(����ũ�μ�����) ����)
* 	(������������ ���)				: https://stackoverflow.com/questions/7371869/minimum-time-a-thread-can-pause-in-linux
*/

namespace TSUtil
{
    void _sleep()
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    void _pause()
    {
#ifdef _MSC_VER
        _mm_pause();
#else // _MSC_VER
        __asm volatile ("pause" ::: "memory");
#endif // _MSC_VER
    }
}

namespace TSUtil
{
    bool SpinLock::try_lock()
    {
        // memory_order_acquire : ��� �޸� ��ɵ��� �� ���� �������� ���ġ �Ǵ� ���� �����Ѵ�.
        // test_and_set			: �� �� true(1) �� ��ȯ
        return flag_.test_and_set(std::memory_order_acquire) == false;
    }

    void SpinLock::lock()
    {
        // try case - 1
        if (_try_lock(_pause) == true)
            return;

        // try case - 2
        if (_try_lock(std::this_thread::yield) == true)
            return;

        // try case - 3
        while (_try_lock(_sleep) == false);
    }

    void SpinLock::unlock()
    {
        // flag_ = false; �� ����.
        flag_.clear(std::memory_order_release);
    }

    bool SpinLock::_try_lock(fnSleep_t fnSleep)
    {
        size_t tryCount = 0;

        while (try_lock() == false)
        {
            if (tryCount >= MAX_SPIN_COUNT)
                return false;

            ++tryCount;
            fnSleep();
        }

        return true;
    }
}
