#pragma once

#include "TSUtil/TaskDispatcher.h"
#include "TSUtil/constraint/chrono.h"
#include "TSUtil/container/TimerQueue.h"
#include "TSUtil/coroutine/CoroutineService.h"
#include "TSUtil/coroutine/awaiter/SuspendThen.h"

namespace TSUtil::coro::detail
{
    struct do_delay
    {
        template <constraint_duration TDuration>
        auto operator()(TaskDispatcher* dispatcher, const TDuration& duration) const&&
        {
            TaskDispatchPoint dispatchPointBackup{ dispatcher->dispatchPoint() };

            return await_suspend_then<>(
                [duration, dispatchPointBackup](auto handle)
                {
                    auto fnTaskDeadline = [handle]()
                        {
                            // 도래시 재개하여 빠져나간다.
                            handle.resume();
                        };

                    auto fnHookExecute = [dispatchPointBackup](TimerQueue::fnTask_t&& fnTask)
                        {
                            // 그대로 resume 한다면 타이머 스레드에서 실행되므로 원래 스레드로 돌아와서 재개 시켜준다.
                            dispatchPointBackup.dispatcher_->dispatch(dispatchPointBackup.threadIdx_, std::move(fnTask));
                        };

                    // ptsoo todo
                    // 현재로썬 취소(cancellation) 구현이 없지만 
                    // 추후 취소시 여기서 타이머 키를 받지 않으므로 취소 시켜줄 방법이 없다.
                    //
                    // 코루틴이 취소되어도 타이머는 여전히 실행 예약이 걸려있으므로 실행되게 된다.
                    // 취소 구현시 이거 신경써야 한다.
                    std::ignore = CoroutineService::getInstance().timerQueue()->addTimer(
                        duration,
                        std::move(fnTaskDeadline),
                        std::move(fnHookExecute)
                    );
                }
            );
        }
    };
}