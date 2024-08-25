#pragma once

#include "TSUtil/coroutine/scope/LambdaCoroutineScope.h"
#include "TSUtil/coroutine/impl/Launch.h"

namespace TSUtil::coro::detail
{
    /// <summary>
    /// ĸ���ص� ���ڸ� �����ϸ�, �ڷ�ƾ�� ������ �� �ִ� ������
    /// TAwait �� awaiter �� ���� �������̽��� �����̴�.
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
