#pragma once

#include "TSUtil/coroutine/scope/LambdaCoroutineScope.h"
#include "TSUtil/coroutine/impl/Launch.h"

namespace TSUtil::coro::detail
{
    /// <summary>
    /// 캡쳐해둔 인자를 보유하며, 코루틴을 시작할 수 있는 스코프
    /// TAwait 은 awaiter 로 감싼 인터페이스의 여부이다.
    /// </summary>
    template<bool TAwait, typename ...TArgs>
    class CapturedLaunchScopeBase : public LambdaCoroutineScope<TArgs...>
    {
    public:
        explicit CapturedLaunchScopeBase(TaskDispatcher* dispatcher, TArgs&&... args)
            : LambdaCoroutineScope<TArgs...>(dispatcher, std::forward<TArgs>(args)...)
        {}

    public:
        template<typename TCallable>
        auto launch(size_t tid, TCallable&& callable)
        {
            if constexpr (TAwait == true)
            {
                return do_launch_await{}(
                    tid,
                    this->dispatcher_,
                    std::forward<TCallable>(callable),
                    std::move(this->args_)
                );
            }
            else
            {
                // return void
                return do_launch{}(
                    tid,
                    this->dispatcher_,
                    std::forward<TCallable>(callable),
                    std::move(this->args_)
                );
            }
        }

        template<typename TCallable>
        auto launch(TCallable&& callable)
        {
            return launch(
                this->dispatcher_->genThreadId(),
                std::forward<TCallable>(callable)
            );
        }
    };

    template<typename ...TArgs>
    using CapturedLaunchScope = CapturedLaunchScopeBase<false, TArgs...>;

    template<typename ...TArgs>
    using CapturedLaunchAwaitScope = CapturedLaunchScopeBase<true, TArgs...>;
}
