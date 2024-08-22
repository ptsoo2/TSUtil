#pragma once

#include <thread>
#include <coroutine>

#include "TSUtil/TaskDispatcher.h"
#include "TSUtil/coroutine/Types.h"

namespace TSUtil::coro
{
    /// <summary>
    /// 부모-자식 관계를 형성하며, 태스크 실행에 초점이 맞춰진 코루틴 오브젝트
    ///
    /// 코루틴 함수 호출시 caller/callee 간에 교류할 방법은 인자로 넘겨주는 것이 일반적이나
    /// 그렇게 사용하면 사용자가 직접 인자로 넘겨주어야 하며, 신뢰할 수 없는 동작이 생길 우려가 있다.
    /// 
    /// (코루틴 함수 호출 => "추가 처리" => 재개 후 최초 시작)
    /// 위 흐름을 의도하고, 이 과정을 내부에서 처리하기 위해 시작시 곧바로 중단하도록 동작한다.
    /// (std::suspend_always initial_suspend)
    /// 
    /// 자식 코루틴을 시작하는 경우 부모 코루틴을 전달받아 보관하며, 
    /// 프레임 소멸시 부모 코루틴을 재개한다.
    /// (코틀린 코루틴의 사용법(구조적 동시성)에 기인하여 설계)
    /// </summary>
    class TaskCoroutine
    {
    public:
        struct promise_type;

    public:
        explicit TaskCoroutine(std::coroutine_handle<promise_type> handle)
            : handle_(handle)
        {}

    public:
        promise_type& promise() const { return handle_.promise(); }
        void resume(const TaskDispatchPoint& dispatchPoint);
        void setParent(std::coroutine_handle<promise_type> parentHandle);

    protected:
        std::coroutine_handle<promise_type> handle_ = nullptr;
    };

    /// <summary>
    /// TaskCoroutine 은 가장 널리 쓰일 것이므로 대외적으로 task alias 로 사용하는 것을 권장
    /// </summary>
    using task = TaskCoroutine;
}

#include "TaskCoroutine.hpp"
