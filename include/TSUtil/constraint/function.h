#pragma once

#include <type_traits>

#include "TSUtil/constraint/multiple.h"

namespace TSUtil
{
    /// <summary>
    /// 실행 가능한 함수에 대해서 리턴 타입이 일치하는지
    /// </summary>
    template <typename TCallable, typename TRet, typename ...TArgs>
    concept constraint_is_return =
        (std::invocable<TCallable, TArgs...> == true)
        && (constraint_is_one_of<std::invoke_result_t<TCallable, TArgs...>, TRet> == true)
        ;

    /// <summary>
    /// 실행 가능한 함수에 대해서 리턴 타입이 불일치하는지
    /// </summary>
    template <typename TCallable, typename TRet, typename ...TArgs>
    concept constraint_is_not_return =
        (std::invocable<TCallable, TArgs...> == true)
        && (constraint_is_none_of<std::invoke_result_t<TCallable, TArgs...>, TRet> == true)
        ;

    /// <summary>
    /// 반환을 하는 함수인지
    /// </summary>
    template <typename TCallable, typename ...TArgs>
    concept constriant_has_return =
        (constraint_is_not_return<TCallable, void, TArgs...> == true)
        ;

    /// <summary>
    /// 반환하지 않는 함수인지
    /// </summary>
    template <typename TCallable, typename ...TArgs>
    concept constriant_has_not_return =
        (constraint_is_return<TCallable, void, TArgs...> == true)
        ;
}
