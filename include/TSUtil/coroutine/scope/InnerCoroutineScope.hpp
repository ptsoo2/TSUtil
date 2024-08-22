namespace TSUtil::coro::detail
{
    struct do_launch
    {
        /// <summary>
        /// �Ϲ� �Լ� + ��ȯ �� ���� ���
        /// </summary>
        template <typename TCallable>
            requires constriant_has_not_return<TCallable>&& constraint_not_coroutine_function<TCallable>
        auto operator()(size_t tid, TaskDispatcher* dispatcher, TCallable&& callable) const&&
        {
            return await_suspend_then<>(
                [tid, dispatcher, callable = std::move(callable)](std::coroutine_handle<> handle)
                {
                    dispatcher->dispatch(tid,
                        [handle, callable = std::move(callable)]()
                        {
                            callable();
                            handle.resume();
                        }
                    );
                }
            );
        }

        /// <summary>
        /// �Ϲ� �Լ� + ��ȯ ���� �ִ� ���
        /// </summary>
        template <typename TCallable, typename TReturn = std::invoke_result_t<TCallable>>
            requires constriant_has_return<TCallable>&& constraint_not_coroutine_function<TCallable>
        auto operator()(size_t tid, TaskDispatcher* dispatcher, TCallable&& callable) const&&
        {
            return await_suspend_then_storage<TReturn>(
                [tid, dispatcher, callable = std::move(callable)](std::coroutine_handle<> handle, TReturn& storage)
                {
                    dispatcher->dispatch(tid,
                        [&storage, handle, callable = std::move(callable)]()
                        {
                            // executor ���� ���� ���� �簳
                            storage = std::move(callable());
                            handle.resume();
                        }
                    );
                }
            );
        }

        /// <summary>
        /// �ڷ�ƾ �Լ��� ��� (������ ���� ������)
        /// </summary>
        template <typename TCallable>
            requires constraint_is_coroutine_function<TCallable, InnerCoroutineScope>
        auto operator()(size_t tid, TaskDispatcher* dispatcher, TCallable&& callable) const&&
        {
            return await_suspend_then<TaskCoroutine>(
                [tid, dispatcher, callable = std::move(callable)](into_coro_handle_t<TaskCoroutine> parentHandle)
                {
                    TaskCoroutine coro = callable(InnerCoroutineScope{ dispatcher });

                    dispatcher->dispatch(tid,
                        [dispatcher, coro, parentHandle]() mutable
                        {
                            coro.setParent(parentHandle);
                            coro.resume(dispatcher->dispatchPoint());
                        }
                    );
                }
            );
        }
    };
}

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
                            dispatchPointBackup.dispatcher_->dispatch(dispatchPointBackup.threadIdx_,
                               [fnTask = std::move(fnTask)]()
                               {
                                   fnTask();
                               }
                            );
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

namespace TSUtil::coro
{
    template <typename TCallable>
        requires constraint_is_coroutine_function_flag<false, TCallable>
    inline auto InnerCoroutineScope::launch(TCallable&& callable) const
    {
        return detail::do_launch{}(dispatcher_->genThreadId(), dispatcher_, std::forward<TCallable>(callable));
    }

    template <typename TCallable>
        requires constraint_is_coroutine_function_flag<false, TCallable>
    inline auto InnerCoroutineScope::launch(size_t tid, TCallable&& callable) const
    {
        return detail::do_launch{}(tid, dispatcher_, std::forward<TCallable>(callable));
    }

    template <typename TCallable>
        requires constraint_is_coroutine_function_flag<true, TCallable, InnerCoroutineScope>
    inline auto InnerCoroutineScope::launch(TCallable&& callable) const
    {
        return detail::do_launch{}(dispatcher_->genThreadId(), dispatcher_, std::forward<TCallable>(callable));
    }

    template <typename TCallable>
        requires constraint_is_coroutine_function_flag<true, TCallable, InnerCoroutineScope>
    inline auto InnerCoroutineScope::launch(size_t tid, TCallable&& callable) const
    {
        return detail::do_launch{}(tid, dispatcher_, std::forward<TCallable>(callable));
    }

    template <constraint_duration TDuration>
    inline auto InnerCoroutineScope::delay(const TDuration& duration) const
    {
        return detail::do_delay{}(dispatcher_, duration);
    }

    inline auto InnerCoroutineScope::delay(time_t delayMiliiSec) const
    {
        return detail::do_delay{}(dispatcher_, std::chrono::milliseconds{ delayMiliiSec });
    }
}
