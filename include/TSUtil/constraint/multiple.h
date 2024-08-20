#pragma once

#include <type_traits>

namespace TSUtil
{
    /// <summary>
    /// n �� �� �ϳ��� Ÿ�԰� ��ġ�ϴ���
    /// </summary>
    template <typename T, typename ...TArgs>
    concept constraint_is_one_of = (std::is_same_v<T, TArgs> || ...);

    /// <summary>
    /// n �� Ÿ�� ���ο� �ٸ� Ÿ������
    /// </summary>
    template <typename T, typename ...TArgs>
    concept constraint_is_none_of = ((std::is_same_v<T, TArgs> == false) && ...);
}
