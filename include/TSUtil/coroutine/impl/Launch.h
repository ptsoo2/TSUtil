#pragma once

#include "TSUtil/TaskDispatcher.h"
#include "TSUtil/coroutine/object/TaskCoroutine.h"
#include "TSUtil/coroutine/awaiter/SuspendThen.h"
#include "TSUtil/coroutine/awaiter/SuspendThenStorage.h"

namespace TSUtil::coro::detail
{
    /// <summary>
    /// 즉시 실행되는 launch
    /// </summary>
    struct do_launch
    {
    public:
        template <typename TCallable, typename ...TArgs>
            requires constraint_is_coroutine_function<TCallable, TArgs...>
        [[nodiscard]] void operator()(size_t tid, TaskDispatcher* dispatcher, TCallable&& callable, std::tuple<TArgs...>&& args) const&&
        {
            // 디스패처 내에서 즉시 실행한다.
            dispatcher->dispatch(tid,
                [dispatcher, callable = std::move(callable), args = std::move(args)]()
                {
                    TaskCoroutine coro = std::apply(std::move(callable), std::move(args));
                    coro.resume(dispatcher->dispatchPoint());
                }
            );
        }
    };
}

namespace TSUtil::coro::detail
{
    /// <summary>
    /// awaiter 를 통해서 실행되는 launch
    /// </summary>
    struct do_launch_await
    {
        /// <summary>
        /// 일반 함수 / 반환 값 없는 경우
        /// </summary>
        template <
            typename TCallable,
            typename ...TArgs
        >
            requires (constraint_is_coroutine_function_flag<false, TCallable, TArgs...>) && (constraint_has_return_flag<false, TCallable, TArgs...>)
        [[nodiscard]] auto operator()(size_t tid, TaskDispatcher* dispatcher, TCallable&& callable, std::tuple<TArgs...>&& args) const&&
        {
            // 바깥 코루틴을 중단하고, 디스패처 내에서 실행하고, 끝나면 재개한다.
            return await_suspend_then<>(
                [tid, dispatcher, callable = std::move(callable), args = std::move(args)](std::coroutine_handle<> handle)
                {
                    dispatcher->dispatch(tid,
                        [handle, callable = std::move(callable), args = std::move(args)]()
                        {
                            std::apply(std::move(callable), std::move(args));
                            handle.resume();
                        }
                    );
                }
            );
        }

        /// <summary>
        /// 일반 함수 / 반환 값 있는 경우
        /// </summary>
        template <
            typename TCallable,
            typename ...TArgs,
            typename TReturn = std::invoke_result_t<TCallable>
        >
            requires (constraint_is_coroutine_function_flag<false, TCallable, TArgs...>) && (constraint_has_return_flag<true, TCallable, TArgs...>)
        [[nodiscard]] auto operator()(size_t tid, TaskDispatcher* dispatcher, TCallable&& callable, std::tuple<TArgs...>&& args) const&&
        {
            // 바깥 코루틴을 중단하고, 디스패처 내에서 실행하고, 끝나면 재개한다.
            // 실행 중에 반환할 값을 저장하며, 재개시 반환한다.
            return await_suspend_then_storage<TReturn>(
                [tid, dispatcher, callable = std::move(callable), args = std::move(args)](std::coroutine_handle<> handle, TReturn& storage)
                {
                    dispatcher->dispatch(tid,
                        [&storage, handle, callable = std::move(callable), args = std::move(args)]()
                        {
                            // executor 에서 실행 이후 재개
                            std::apply(std::move(callable), std::move(args));
                            handle.resume();
                        }
                    );
                }
            );
        }

        /// <summary>
        /// 코루틴 함수 / 리턴은 아직 미지원
        /// </summary>
        template <
            typename TCallable,
            typename ...TArgs
        >
            requires (constraint_is_coroutine_function_flag<true, TCallable, TArgs...>)
        [[nodiscard]] auto operator()(size_t tid, TaskDispatcher* dispatcher, TCallable&& callable, std::tuple<TArgs...>&& args) const&&
        {
            // 바깥 코루틴을 중단하고, 디스패처 내에서 실행하고, 끝나면 재개한다.
            // 실행 이전에 부모 코루틴 핸들을 세팅하며, 자식 코루틴이 종료되는 경우 부모 코루틴을 재개한다.
            return await_suspend_then<TaskCoroutine>(
                [tid, dispatcher, callable = std::move(callable), args = std::move(args)](into_coro_handle_t<TaskCoroutine> parentHandle)
                {
                    TaskCoroutine coro = std::apply(std::move(callable), std::move(args));

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