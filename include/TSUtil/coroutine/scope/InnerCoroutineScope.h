#pragma once

#include "TSUtil/constraint/chrono.h"
#include "TSUtil/constraint/function.h"

#include "TSUtil/coroutine/CoroutineService.h"
#include "TSUtil/coroutine/object/TaskCoroutine.h"
#include "TSUtil/coroutine/scope/impl/CapturedLaunchScope.h"

namespace TSUtil::coro
{
    /// <summary>
    /// 코루틴 내부 스코프
    /// CoroutineScope 와 격리한 것은 인터페이스를 나누기 위함
    /// </summary>
    class InnerCoroutineScope final
    {
        friend class CoroutineScope;

    protected:
        explicit InnerCoroutineScope(TaskDispatcher* dispatcher)
            : dispatcher_(dispatcher)
        {}

    public:
        /// <summary>
        /// 캡쳐할 인자를 tuple 로 전달받고, 실행한다.
        /// 
        /// 자세한 내용은 CoroutineScope::capture 를 참조
        /// </summary>
        template <typename ...TArgs>
        [[nodiscard]] auto capture(TArgs&&... args) const
        {
            return detail::CapturedLaunchAwaitScope(
                dispatcher_,
                InnerCoroutineScope{ dispatcher_ }, std::forward<TArgs>(args)...
            );
        }

        /// <summary>
        /// 코루틴을 시작한다.
        /// 지정한 스레드로 실행한다.
        /// </summary>
        template <typename TCallable>
        [[nodiscard]] auto launch(size_t tid, TCallable&& callable) const
        {
            if constexpr (constraint_is_coroutine_function<TCallable, InnerCoroutineScope> == true)
            {
                // 코루틴 함수인 경우 InnerCoroutineScope 를 추가로 전달하고,
                // awaiter 를 반환한다.
                return detail::do_launch_await{}(
                    tid,
                    dispatcher_,
                    std::forward<TCallable>(callable),
                    std::tuple{ InnerCoroutineScope{ dispatcher_ } }
                );
            }
            else
            {
                // 일반 함수인 경우 인자 전달은 없으며,
                // void 또는 함수 내에서의 리턴에 따라 반환한다.
                return detail::do_launch_await{}(
                    tid,
                    dispatcher_,
                    std::forward<TCallable>(callable),
                    {}
                );
            }
        }

        /// <summary>
        /// 코루틴을 시작한다.
        /// 라운드 로빈으로 실행한다.
        /// </summary>
        template <typename TCallable>
        [[nodiscard]] auto launch(TCallable&& callable) const
        {
            return launch(dispatcher_->genThreadId(), std::forward<TCallable>(callable));
        }

        /// <summary>
        /// 지정된 시간만큼 타이머 큐에서 대기 후 재개
        /// </summary>
        template <constraint_duration TDuration>
        [[nodiscard]] auto delay(const TDuration& duration) const
        {
            return detail::do_delay{}(dispatcher_, duration);
        }

        /// <summary>
        /// 지정된 시간만큼 타이머 큐에서 대기 후 재개
        /// </summary>
        [[nodiscard]] auto delay(time_t delayMiliiSec) const
        {
            return detail::do_delay{}(dispatcher_, std::chrono::milliseconds{ delayMiliiSec });
        }

    protected:
        TaskDispatcher* dispatcher_ = nullptr;
    };
}
