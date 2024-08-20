#pragma once

#include <type_traits>

namespace TSUtil
{
    /// <summary>
    /// n 개 중 하나의 타입과 일치하는지
    /// </summary>
    template <typename T, typename ...TArgs>
    concept constraint_is_one_of = (std::is_same_v<T, TArgs> || ...);

    /// <summary>
    /// n 개 타입 전부와 다른 타입인지
    /// </summary>
    template <typename T, typename ...TArgs>
    concept constraint_is_none_of = ((std::is_same_v<T, TArgs> == false) && ...);
}
