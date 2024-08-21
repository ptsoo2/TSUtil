#pragma once

#include <type_traits>

#include "TSUtil/constraint/multiple.h"

namespace TSUtil::detail
{
    template<typename TRet, typename ...TArgs>
    struct std_function_trait {};

    template<typename TRet, typename ...TArgs>
    struct std_function_trait<std::function<TRet(TArgs...)>>
    {
        using type = std::function<TRet(TArgs...)>;
    };

    template<typename TCallable, typename ...TArgs>
    using std_function_t = typename detail::std_function_trait<TCallable, TArgs...>::type;
}

namespace TSUtil
{
    /// <summary>
    /// 실행 가능한 함수에 대해서 리턴 타입이 일치하는지
    /// </summary>
    template <typename TCallable, typename TRet, typename ...TArgs>
    concept constraint_match_return_type =
        (std::invocable<TCallable, TArgs...> == true)
        && (constraint_is_one_of<std::invoke_result_t<TCallable, TArgs...>, TRet> == true)
        ;

    /// <summary>
    /// 실행 가능한 함수에 대해서 리턴 타입이 불일치하는지
    /// </summary>
    template <typename TCallable, typename TRet, typename ...TArgs>
    concept constraint_not_match_return_type =
        (std::invocable<TCallable, TArgs...> == true)
        && (constraint_is_none_of<std::invoke_result_t<TCallable, TArgs...>, TRet> == true)
        ;

    /// <summary>
    /// 반환을 하는 함수인지
    /// </summary>
    template <typename TCallable, typename ...TArgs>
    concept constriant_has_return =
        (constraint_not_match_return_type<TCallable, void, TArgs...> == true)
        ;

    /// <summary>
    /// 반환하지 않는 함수인지
    /// </summary>
    template <typename TCallable, typename ...TArgs>
    concept constriant_has_not_return =
        (constraint_match_return_type<TCallable, void, TArgs...> == true)
        ;

    /// <summary>
    /// std::function 함수인지
    /// </summary>
    template<typename TCallable, typename ...TArgs>
    concept constraint_is_std_function = std::is_same_v<
        detail::std_function_t<TCallable, TArgs...>,
        TCallable
    >;

    /// <summary>
    /// 전역 함수 포인터인지
    /// </summary>
    template<typename TCallable>
    concept constraint_is_global_function =
        (std::is_function_v<TCallable> == true)
        ;

    /// <summary>
    /// 멤버 함수 포인터인지
    /// </summary>
    template<typename TCallable>
    concept constraint_is_member_function =
        (std::is_member_function_pointer_v<TCallable> == true)
        ;

    /// <summary>
    /// 람다인지
    /// </summary>
    template<typename TCallable, typename ...TArgs>
    concept constraint_is_lambda =
        (std::invocable<TCallable, TArgs...> == true)
        && (constraint_is_std_function<TCallable, TArgs...> == false)
        && (constraint_is_global_function<TCallable> == false)
        && (constraint_is_member_function<TCallable> == false)
        ;

    /// <summary>
    /// 람다에 캡처한게 있는지
    /// ptsoo todo - 람다의 구현체는 functor 이므로 캡처하면 그 대상은 구조체 멤버로써 존재하게 된다.
    /// 따라서 size 로 체크할 수 있지만 이게 정석적인 방법인지에는 의문이 있는 상태
    /// </summary>
    template<typename TCallable, typename ...TArgs>
    concept constraint_has_lambda_capture =
        (constraint_is_lambda<TCallable, TArgs...> == true)
        && (sizeof(decltype([]() {})) != sizeof(TCallable))
        ;
}
