#include "TSUtil/synchronize/SpinLock.h"

/*
*	_mm_pause						: https://yonmy.com/archives/99 (대기 속도를 늦춤으로써 파이프라인의 더 적은 부분을 사용)// 겁나작은 sleep 이라 생각하면 된다. (us(마이크로세컨즈) 단위)
* 	(리눅스에서의 사용)				: https://stackoverflow.com/questions/7371869/minimum-time-a-thread-can-pause-in-linux
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
        // memory_order_acquire : 모든 메모리 명령들이 이 시점 이전으로 재배치 되는 것을 금지한다.
        // test_and_set			: 비교 후 true(1) 로 전환
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
        // flag_ = false; 와 같다.
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
