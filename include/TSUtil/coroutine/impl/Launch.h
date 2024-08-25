#pragma once

#include "TSUtil/TaskDispatcher.h"
#include "TSUtil/coroutine/object/TaskCoroutine.h"
#include "TSUtil/coroutine/awaiter/SuspendThen.h"
#include "TSUtil/coroutine/awaiter/SuspendThenStorage.h"

namespace TSUtil::coro::detail
{
    /// <summary>
    /// ��� ����Ǵ� launch
    /// </summary>
    struct do_launch
    {
    public:
        template <typename TCallable, typename ...TArgs>
            requires constraint_is_coroutine_function<TCallable, TArgs...>
        [[nodiscard]] void operator()(size_t tid, TaskDispatcher* dispatcher, TCallable&& callable, std::tuple<TArgs...>&& args) const&&
        {
            // ����ó ������ ��� �����Ѵ�.
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
    /// awaiter �� ���ؼ� ����Ǵ� launch
    /// </summary>
    struct do_launch_await
    {
        /// <summary>
        /// �Ϲ� �Լ� / ��ȯ �� ���� ���
        /// </summary>
        template <
            typename TCallable,
            typename ...TArgs
        >
            requires (constraint_is_coroutine_function_flag<false, TCallable, TArgs...>) && (constraint_has_return_flag<false, TCallable, TArgs...>)
        [[nodiscard]] auto operator()(size_t tid, TaskDispatcher* dispatcher, TCallable&& callable, std::tuple<TArgs...>&& args) const&&
        {
            // �ٱ� �ڷ�ƾ�� �ߴ��ϰ�, ����ó ������ �����ϰ�, ������ �簳�Ѵ�.
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
        /// �Ϲ� �Լ� / ��ȯ �� �ִ� ���
        /// </summary>
        template <
            typename TCallable,
            typename ...TArgs,
            typename TReturn = std::invoke_result_t<TCallable>
        >
            requires (constraint_is_coroutine_function_flag<false, TCallable, TArgs...>) && (constraint_has_return_flag<true, TCallable, TArgs...>)
        [[nodiscard]] auto operator()(size_t tid, TaskDispatcher* dispatcher, TCallable&& callable, std::tuple<TArgs...>&& args) const&&
        {
            // �ٱ� �ڷ�ƾ�� �ߴ��ϰ�, ����ó ������ �����ϰ�, ������ �簳�Ѵ�.
            // ���� �߿� ��ȯ�� ���� �����ϸ�, �簳�� ��ȯ�Ѵ�.
            return await_suspend_then_storage<TReturn>(
                [tid, dispatcher, callable = std::move(callable), args = std::move(args)](std::coroutine_handle<> handle, TReturn& storage)
                {
                    dispatcher->dispatch(tid,
                        [&storage, handle, callable = std::move(callable), args = std::move(args)]()
                        {
                            // executor ���� ���� ���� �簳
                            std::apply(std::move(callable), std::move(args));
                            handle.resume();
                        }
                    );
                }
            );
        }

        /// <summary>
        /// �ڷ�ƾ �Լ� / ������ ���� ������
        /// </summary>
        template <
            typename TCallable,
            typename ...TArgs
        >
            requires (constraint_is_coroutine_function_flag<true, TCallable, TArgs...>)
        [[nodiscard]] auto operator()(size_t tid, TaskDispatcher* dispatcher, TCallable&& callable, std::tuple<TArgs...>&& args) const&&
        {
            // �ٱ� �ڷ�ƾ�� �ߴ��ϰ�, ����ó ������ �����ϰ�, ������ �簳�Ѵ�.
            // ���� ������ �θ� �ڷ�ƾ �ڵ��� �����ϸ�, �ڽ� �ڷ�ƾ�� ����Ǵ� ��� �θ� �ڷ�ƾ�� �簳�Ѵ�.
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