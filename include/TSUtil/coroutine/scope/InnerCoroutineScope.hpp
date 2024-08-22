namespace TSUtil::coro::detail
{
    struct do_launch
    {
        /// <summary>
        /// 일반 함수 + 반환 값 없는 경우
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
        /// 일반 함수 + 반환 값이 있는 경우
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
                            // executor 에서 실행 이후 재개
                            storage = std::move(callable());
                            handle.resume();
                        }
                    );
                }
            );
        }

        /// <summary>
        /// 코루틴 함수인 경우 (리턴은 아직 미지원)
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
                            // 도래시 재개하여 빠져나간다.
                            handle.resume();
                        };

                    auto fnHookExecute = [dispatchPointBackup](TimerQueue::fnTask_t&& fnTask)
                        {
                            // 그대로 resume 한다면 타이머 스레드에서 실행되므로 원래 스레드로 돌아와서 재개 시켜준다.
                            dispatchPointBackup.dispatcher_->dispatch(dispatchPointBackup.threadIdx_,
                               [fnTask = std::move(fnTask)]()
                               {
                                   fnTask();
                               }
                            );
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
