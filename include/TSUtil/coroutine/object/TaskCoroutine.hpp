#include <iostream>
#include "TaskCoroutine.h"

namespace TSUtil::coro
{
    struct TaskCoroutine::promise_type
    {
        friend class TaskCoroutine;

        ~promise_type()
        {
            auto parentHandle = parentHandle_;
            if (parentHandle == nullptr)
                return;

            // 자식 코루틴이라면 이곳으로 진입한다.
            // 부모 코루틴이 실행되었던 스레드로 복귀하고, 재개하여 진행을 이어간다.
            const TaskDispatchPoint& parentDispatchPoint = parentHandle.promise().dispatchPoint_;
            TaskDispatcher* parentDispatcher = parentDispatchPoint.dispatcher_;

            parentDispatcher->dispatch(parentDispatchPoint.threadIdx_,
                [parentDispatcher, parentHandle]()
                {
                    TaskCoroutine parentTask = parentHandle.promise().get_return_object();
                    parentTask.resume(parentDispatcher->dispatchPoint());
                }
            );
        }

        TaskCoroutine get_return_object()
        {
            return TaskCoroutine{
                std::coroutine_handle<promise_type>::from_promise(*this)
            };
        }

        std::suspend_always initial_suspend() noexcept { return {}; }
        std::suspend_never final_suspend() noexcept { return {}; }

        void return_void() {}
        void unhandled_exception()
        {
            // ptsoo todo - 코루틴은 try~catch 내에서 실행되므로 여기서 캐치해서 바깥으로 전파해야한다.
            // 일단은 관측을 위해 로깅
            try
            {
                std::rethrow_exception(std::current_exception());
            }
            catch (const std::exception& e)
            {
                PROBE("unhandled_exception: %s", e.what());
            }
            catch (...)
            {
                PROBE("unhandled_exception: unknown exception");
            }
        }

    protected:
        /// <summary>
        /// 현재 실행중인 지점
        /// </summary>
        TaskDispatchPoint dispatchPoint_;

        /// <summary>
        /// 부모 핸들
        /// </summary>
        std::coroutine_handle<promise_type> parentHandle_ = nullptr;
    };

    void TaskCoroutine::resume(const TaskDispatchPoint& dispatchPoint)
    {
        promise().dispatchPoint_ = dispatchPoint;
        handle_.resume();
    }

    void TaskCoroutine::setParent(std::coroutine_handle<promise_type> parentHandle)
    {
        promise().parentHandle_ = parentHandle;
    }
}
