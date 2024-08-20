#pragma once

#include <type_traits>

#include "TSUtil/constraint/multiple.h"

namespace TSUtil
{
    /// <summary>
    /// ���� ������ �Լ��� ���ؼ� ���� Ÿ���� ��ġ�ϴ���
    /// </summary>
    template <typename TCallable, typename TRet, typename ...TArgs>
    concept constraint_is_return =
        (std::invocable<TCallable, TArgs...> == true)
        && (constraint_is_one_of<std::invoke_result_t<TCallable, TArgs...>, TRet> == true)
        ;

    /// <summary>
    /// ���� ������ �Լ��� ���ؼ� ���� Ÿ���� ����ġ�ϴ���
    /// </summary>
    template <typename TCallable, typename TRet, typename ...TArgs>
    concept constraint_is_not_return =
        (std::invocable<TCallable, TArgs...> == true)
        && (constraint_is_none_of<std::invoke_result_t<TCallable, TArgs...>, TRet> == true)
        ;

    /// <summary>
    /// ��ȯ�� �ϴ� �Լ�����
    /// </summary>
    template <typename TCallable, typename ...TArgs>
    concept constriant_has_return =
        (constraint_is_not_return<TCallable, void, TArgs...> == true)
        ;

    /// <summary>
    /// ��ȯ���� �ʴ� �Լ�����
    /// </summary>
    template <typename TCallable, typename ...TArgs>
    concept constriant_has_not_return =
        (constraint_is_return<TCallable, void, TArgs...> == true)
        ;
}
