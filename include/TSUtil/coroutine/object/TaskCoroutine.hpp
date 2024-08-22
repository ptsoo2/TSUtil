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

            // �ڽ� �ڷ�ƾ�̶�� �̰����� �����Ѵ�.
            // �θ� �ڷ�ƾ�� ����Ǿ��� ������� �����ϰ�, �簳�Ͽ� ������ �̾��.
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
            // ptsoo todo - �ڷ�ƾ�� try~catch ������ ����ǹǷ� ���⼭ ĳġ�ؼ� �ٱ����� �����ؾ��Ѵ�.
            // �ϴ��� ������ ���� �α�
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
        /// ���� �������� ����
        /// </summary>
        TaskDispatchPoint dispatchPoint_;

        /// <summary>
        /// �θ� �ڵ�
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
