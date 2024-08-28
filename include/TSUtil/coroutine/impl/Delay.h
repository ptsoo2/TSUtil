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
                            // ������ �簳�Ͽ� ����������.
                            handle.resume();
                        };

                    auto fnHookExecute = [dispatchPointBackup](TimerQueue::fnTask_t&& fnTask)
                        {
                            // �״�� resume �Ѵٸ� Ÿ�̸� �����忡�� ����ǹǷ� ���� ������� ���ƿͼ� �簳 �����ش�.
                            dispatchPointBackup.dispatcher_->dispatch(dispatchPointBackup.threadIdx_, std::move(fnTask));
                        };

                    // ptsoo todo
                    // ����ν� ���(cancellation) ������ ������ 
                    // ���� ��ҽ� ���⼭ Ÿ�̸� Ű�� ���� �����Ƿ� ��� ������ ����� ����.
                    //
                    // �ڷ�ƾ�� ��ҵǾ Ÿ�̸Ӵ� ������ ���� ������ �ɷ������Ƿ� ����ǰ� �ȴ�.
                    // ��� ������ �̰� �Ű��� �Ѵ�.
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