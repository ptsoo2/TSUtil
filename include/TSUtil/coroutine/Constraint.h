#pragma once

#include <coroutine>

#include "TSUtil/constraint/function.h"

/// <summary>
/// �ڷ�ƾ�� �����ϰ� ����ϱ� ���� ���� ���� ����
/// </summary>
namespace TSUtil::coro
{
    /// <summary>
    /// �ڷ�ƾ �ڵ��� ���δ� �ڷ�ƾ ������Ʈ ������ �ش��ϴ��� üũ
    /// </summary>
    template <typename TObject>
    concept constraint_is_coroutine_object = requires
    {
        typename TObject::promise_type;
        typename std::coroutine_handle<typename TObject::promise_type>;
    };

    /// <summary>
    /// �ڷ�ƾ �Լ����� üũ
    /// 
    /// co_return, co_await, co_yield �� �Լ� ������ ����ִ����ʹ� ��� ����
    /// ���� Ÿ���� �ڷ�ƾ ������Ʈ ���θ� üũ�Ѵ�.
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
            || (constraint_has_lambda_capture<TCallable, TArgs...> == false) // �ڷ�ƾ �Լ������ϴ� ��츸 üũ
        )
#endif // COROUTINE_AVOID_LAMBDA_CAPTURE
        ;

    /// <summary>
    /// �ڷ�ƾ �Լ��� �´��� üũ
    /// </summary>
    template <
        typename TCallable,
        typename ...TArgs
    >
    concept constraint_is_coroutine_function =
        constraint_is_coroutine_function_flag<true, TCallable, TArgs...>;

    /// <summary>
    /// �ڷ�ƾ �Լ��� �ƴ��� üũ
    /// 
    /// co_return, co_await, co_yield �� �Լ� ������ ����ִ����ʹ� ��� ����
    /// ���� Ÿ���� �ڷ�ƾ ������Ʈ ���θ� üũ�Ѵ�.
    /// </summary>
    template <
        typename TCallable,
        typename ...TArgs,
        typename TRet = std::remove_cvref_t<std::invoke_result_t<TCallable, TArgs...>>
    >
    concept constraint_not_coroutine_function =
        constraint_is_coroutine_function_flag<false, TCallable, TArgs...>;
}
