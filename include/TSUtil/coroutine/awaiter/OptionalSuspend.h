#pragma once

#include <coroutine>

namespace TSUtil::coro
{
    /// <summary>
    /// 선택적으로 중단하려는 경우
    /// </summary>
    struct await_optional_suspend_t
    {
        constexpr await_optional_suspend_t(bool isSuspend)
            : isSuspend_(isSuspend)
        {}

        constexpr bool await_ready() const noexcept { return (isSuspend_ == false); }
        void await_suspend(std::coroutine_handle<>) const noexcept {}
        void await_resume() const noexcept {}

    protected:
        bool isSuspend_ = false;
    };
}
