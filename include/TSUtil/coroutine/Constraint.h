#pragma once

#include <coroutine>

#include "TSUtil/constraint/function.h"

/// <summary>
/// 코루틴을 안전하게 사용하기 위한 제약 조건 정의
/// </summary>
namespace TSUtil::coro
{
    /// <summary>
    /// 코루틴 핸들을 감싸는 코루틴 오브젝트 유형에 해당하는지 체크
    /// </summary>
    template <typename TObject>
    concept constraint_is_coroutine_object = requires
    {
        typename TObject::promise_type;
        typename std::coroutine_handle<typename TObject::promise_type>;
    };

    /// <summary>
    /// 코루틴 함수인지 체크
    /// 
    /// co_return, co_await, co_yield 가 함수 본문에 들어있는지와는 상관 없이
    /// 리턴 타입의 코루틴 오브젝트 여부만 체크한다.
    /// </summary>
    template <
        bool TFlag,
        typename TCallable,
        typename ...TArgs,
        typename TRet = std::remove_cvref_t<std::invoke_result_t<TCallable, TArgs...>>
    >
    concept constraint_is_coroutine_function_flag =
        (std::invocable<TCallable, TArgs...> == true)
        && (constraint_is_coroutine_object<TRet> == TFlag)
#ifdef COROUTINE_AVOID_LAMBDA_CAPTURE
        &&
        (
            (TFlag == false)
            || (constraint_has_lambda_capture<TCallable, TArgs...> == false) // 코루틴 함수여야하는 경우만 체크
        )
#endif // COROUTINE_AVOID_LAMBDA_CAPTURE
        ;

    /// <summary>
    /// 코루틴 함수가 맞는지 체크
    /// </summary>
    template <
        typename TCallable,
        typename ...TArgs
    >
    concept constraint_is_coroutine_function =
        constraint_is_coroutine_function_flag<true, TCallable, TArgs...>;

    /// <summary>
    /// 코루틴 함수가 아닌지 체크
    /// 
    /// co_return, co_await, co_yield 가 함수 본문에 들어있는지와는 상관 없이
    /// 리턴 타입의 코루틴 오브젝트 여부만 체크한다.
    /// </summary>
    template <
        typename TCallable,
        typename ...TArgs,
        typename TRet = std::remove_cvref_t<std::invoke_result_t<TCallable, TArgs...>>
    >
    concept constraint_not_coroutine_function =
        constraint_is_coroutine_function_flag<false, TCallable, TArgs...>;
}
