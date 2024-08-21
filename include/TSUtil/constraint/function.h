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
    /// ���� ������ �Լ��� ���ؼ� ���� Ÿ���� ��ġ�ϴ���
    /// </summary>
    template <typename TCallable, typename TRet, typename ...TArgs>
    concept constraint_match_return_type =
        (std::invocable<TCallable, TArgs...> == true)
        && (constraint_is_one_of<std::invoke_result_t<TCallable, TArgs...>, TRet> == true)
        ;

    /// <summary>
    /// ���� ������ �Լ��� ���ؼ� ���� Ÿ���� ����ġ�ϴ���
    /// </summary>
    template <typename TCallable, typename TRet, typename ...TArgs>
    concept constraint_not_match_return_type =
        (std::invocable<TCallable, TArgs...> == true)
        && (constraint_is_none_of<std::invoke_result_t<TCallable, TArgs...>, TRet> == true)
        ;

    /// <summary>
    /// ��ȯ�� �ϴ� �Լ�����
    /// </summary>
    template <typename TCallable, typename ...TArgs>
    concept constriant_has_return =
        (constraint_not_match_return_type<TCallable, void, TArgs...> == true)
        ;

    /// <summary>
    /// ��ȯ���� �ʴ� �Լ�����
    /// </summary>
    template <typename TCallable, typename ...TArgs>
    concept constriant_has_not_return =
        (constraint_match_return_type<TCallable, void, TArgs...> == true)
        ;

    /// <summary>
    /// std::function �Լ�����
    /// </summary>
    template<typename TCallable, typename ...TArgs>
    concept constraint_is_std_function = std::is_same_v<
        detail::std_function_t<TCallable, TArgs...>,
        TCallable
    >;

    /// <summary>
    /// ���� �Լ� ����������
    /// </summary>
    template<typename TCallable>
    concept constraint_is_global_function =
        (std::is_function_v<TCallable> == true)
        ;

    /// <summary>
    /// ��� �Լ� ����������
    /// </summary>
    template<typename TCallable>
    concept constraint_is_member_function =
        (std::is_member_function_pointer_v<TCallable> == true)
        ;

    /// <summary>
    /// ��������
    /// </summary>
    template<typename TCallable, typename ...TArgs>
    concept constraint_is_lambda =
        (std::invocable<TCallable, TArgs...> == true)
        && (constraint_is_std_function<TCallable, TArgs...> == false)
        && (constraint_is_global_function<TCallable> == false)
        && (constraint_is_member_function<TCallable> == false)
        ;

    /// <summary>
    /// ���ٿ� ĸó�Ѱ� �ִ���
    /// ptsoo todo - ������ ����ü�� functor �̹Ƿ� ĸó�ϸ� �� ����� ����ü ����ν� �����ϰ� �ȴ�.
    /// ���� size �� üũ�� �� ������ �̰� �������� ����������� �ǹ��� �ִ� ����
    /// </summary>
    template<typename TCallable, typename ...TArgs>
    concept constraint_has_lambda_capture =
        (constraint_is_lambda<TCallable, TArgs...> == true)
        && (sizeof(decltype([]() {})) != sizeof(TCallable))
        ;
}
