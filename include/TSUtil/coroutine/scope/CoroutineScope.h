#pragma once

#include "TSUtil/coroutine/Constraint.h"
#include "TSUtil/coroutine/CoroutineService.h"

#include "TSUtil/coroutine/object/TaskCoroutine.h"
#include "TSUtil/coroutine/scope/InnerCoroutineScope.h"

namespace TSUtil::coro
{
    /// <summary>
    /// 코루틴을 처음 시작하기 위한 진입점
    /// 코틀린 코루틴의 사용법(구조적 동시성)에 기인하여 설계
    /// </summary>
    class CoroutineScope final
    {
    public:
        explicit CoroutineScope(TaskDispatcher* dispatcher = CoroutineService::getInstance().defaultDispatcher())
            : dispatcher_(dispatcher)
        {}

    protected:
        CoroutineScope(const CoroutineScope&) = delete;
        CoroutineScope& operator=(const CoroutineScope&) = delete;

    public:
        /// <summary>
        /// 캡쳐할 인자를 tuple 로 전달받고, 실행한다.
        /// 
        /// 람다 코루틴에서 캡처 사용은 위험하다.
        /// 람다는 실행시 해당 지점에 로컬 변수로 자리잡아 실행되며,
        /// 람다 내에서 중단하고, 스코프를 벗어나서 재개시 캡처한 대상은 메모리가 무효화 되므로 이러한 코드 작성 방법은 ub 이다.
        /// 
        /// 따라서 직접적으로 캡처할 수 없도록 방어한다. (Config.h 의 COROUTINE_AVOID_LAMBDA_CAPTURE 참조)
        /// 람다 코루틴에서는 캡처를 아예 안쓰는 것이 best practice 이다. 코드 작성마다 이것을 고려하기 매우 어렵다.
        /// 
        /// 인자로 전달할 대상을 tuple 에 담아서 우회하여 안전하게 사용한다.
        /// </summary>
        template <typename ...TArgs>
        [[nodiscard]] auto capture(TArgs&&... args) const&&
        {
            return detail::CapturedLaunchScope(
                dispatcher_,
                InnerCoroutineScope{ dispatcher_ }, std::forward<TArgs>(args)...
            );
        }

        /// <summary>
        /// 코루틴을 시작한다.
        /// 라운드 로빈으로 실행한다.
        /// </summary>
        template <typename TCallable>
        void launch(TCallable&& callable) const&&
        {
            CoroutineScope(dispatcher_)
                .capture()
                .launch(std::forward<TCallable>(callable));
        }

        /// <summary>
        /// 코루틴을 시작한다.
        /// 지정한 스레드로 실행한다.
        /// </summary>
        template <typename TCallable>
        void launch(size_t tid, TCallable&& callable) const&&
        {
            CoroutineScope(dispatcher_)
                .capture()
                .launch(tid, std::forward<TCallable>(callable));
        }

    protected:
        TaskDispatcher* dispatcher_ = nullptr;
    };
}
