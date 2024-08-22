#pragma once

#include "TSUtil/coroutine/Types.h"

namespace TSUtil::coro
{
    /// <summary>
    /// �ߴ� �� Ŀ���� ó���ϱ� ����
    /// return ���� tail-call �� �ٱ� �ڵ��� ����� �� �ִ� ���� ����ϱ� ����
    /// �簳�� caller ������ ���� ó�� �ʿ�
    /// </summary>
    /// <typeparam name="TObject"></typeparam>
    template <typename TObject = void, std::enable_if_t<constraint_is_coroutine_object<TObject>>* = nullptr>
    struct await_suspend_then
    {
        using handle_t = into_coro_handle_t<TObject>;
        using callable_t = std::function<void(handle_t)>;

        await_suspend_then(callable_t&& callable)
            : callable_(std::move(callable))
        {}

        constexpr bool await_ready() const noexcept { return false; }
        constexpr void await_suspend(handle_t handle) noexcept
        {
            callable_(handle);
        }
        constexpr void await_resume() const noexcept {}

    protected:
        callable_t callable_;
    };

    template <>
    struct await_suspend_then<void>
    {
        using handle_t = std::coroutine_handle<>;
        using callable_t = std::function<void(handle_t)>;

        await_suspend_then(callable_t&& callable)
            : callable_(std::move(callable))
        {}

        constexpr bool await_ready() const noexcept { return false; }
        void await_suspend(handle_t handle) noexcept
        {
            callable_(handle);
        }
        void await_resume() const noexcept {}

    protected:
        callable_t callable_;
    };
}