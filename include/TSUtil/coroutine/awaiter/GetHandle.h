#pragma once

#include "TSUtil/coroutine/Types.h"

namespace TSUtil::coro
{
    /// <summary>
    /// 특정 코루틴 오브젝트에서 std::coroutine_handle 얻기
    /// </summary>
    /// <typeparam name="TObject"></typeparam>
    template <typename TObject>
    struct await_get_handle
    {
        constexpr bool await_ready() const noexcept { return false; }
        constexpr bool await_suspend(into_coro_handle_t<TObject> handle) noexcept
        {
            currentHandle_ = handle;
            return false;
        }
        auto await_resume() const noexcept { return currentHandle_; }

    protected:
        into_coro_handle_t<TObject> currentHandle_;
    };
}
