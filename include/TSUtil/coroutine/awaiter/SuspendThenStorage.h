#pragma once

#include "TSUtil/coroutine/Types.h"

namespace TSUtil::coro
{
    /// <summary>
    /// �ߴ� �� Ŀ���� ó��, ���� ������ �ʿ��� ���
    /// </summary>
    /// <typeparam name="TStorage"></typeparam>
    template <typename TStorage>
    struct await_suspend_then_storage
    {
        using handle_t = std::coroutine_handle<>;
        using callable_t = std::function<void(handle_t, TStorage&)>;

        await_suspend_then_storage(callable_t&& callable)
            : callable_(std::move(callable))
        {}

        constexpr bool await_ready() noexcept { return false; }
        void await_suspend(handle_t handle) noexcept
        {
            callable_(handle, std::ref(storage_));
        }
        [[nodiscard]] TStorage await_resume() noexcept
        {
            return std::move(storage_);
        }

    protected:
        callable_t callable_;
        TStorage storage_;
    };
}
