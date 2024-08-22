#pragma once

#include "TSUtil/coroutine/CoroutineService.h"

#include "TSUtil/coroutine/awaiter/SuspendThen.h"
#include "TSUtil/coroutine/awaiter/SuspendThenStorage.h"

#include "TSUtil/coroutine/object/TaskCoroutine.h"

#include "TSUtil/constraint/chrono.h"
#include "TSUtil/constraint/function.h"

namespace TSUtil::coro::detail
{
    struct do_delay;
    struct do_launch;
}

namespace TSUtil::coro
{
    /// <summary>
    /// 코루틴 내부 스코프
    /// CoroutineScope 와 격리한 것은 인터페이스를 나누기 위함
    /// </summary>
    class InnerCoroutineScope final
    {
        friend class CoroutineScope;
        friend struct detail::do_delay;
        friend struct detail::do_launch;

    protected:
        explicit InnerCoroutineScope(TaskDispatcher* dispatcher)
            : dispatcher_(dispatcher)
        {}

    public:
        /// <summary>
        /// 일반 함수 실행(라운드 로빈 스레드)
        /// </summary>
        template <typename TCallable>
            requires constraint_is_coroutine_function_flag<false, TCallable>
        [[nodiscard]] auto launch(TCallable&& callable) const;

        /// <summary>
        /// 일반 함수 실행(지정 스레드)
        /// </summary>
        template <typename TCallable>
            requires constraint_is_coroutine_function_flag<false, TCallable>
        [[nodiscard]] auto launch(size_t tid, TCallable&& callable) const;

        /// <summary>
        /// 코루틴 함수 실행(라운드 로빈 스레드)
        /// </summary>
        template <typename TCallable>
            requires constraint_is_coroutine_function_flag<true, TCallable, InnerCoroutineScope>
        [[nodiscard]] auto launch(TCallable&& callable) const;

        /// <summary>
        /// 코루틴 함수 실행(지정 스레드)
        /// </summary>
        template <typename TCallable>
            requires constraint_is_coroutine_function_flag<true, TCallable, InnerCoroutineScope>
        [[nodiscard]] auto launch(size_t tid, TCallable&& callable) const;

        /// <summary>
        /// 지정된 시간만큼 타이머 큐에서 대기 후 재개
        /// </summary>
        template <constraint_duration TDuration>
        [[nodiscard]] auto delay(const TDuration& duration) const;

        /// <summary>
        /// 지정된 시간만큼 타이머 큐에서 대기 후 재개
        /// </summary>
        [[nodiscard]] auto delay(time_t delayMiliiSec) const;

    protected:
        TaskDispatcher* dispatcher_ = nullptr;
    };
}

#include "InnerCoroutineScope.hpp"
