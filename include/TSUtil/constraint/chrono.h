#pragma once

#include <chrono>

namespace TSUtil
{
    /// <summary>
    /// std::chrono::duration<> ���� ���� ����
    /// </summary>
    template <typename T>
    concept constraint_duration = requires(T t)
    {
        typename T::rep;    // T�� rep Ÿ���� ������ ��
        typename T::period; // T�� period Ÿ���� ������ ��
        std::chrono::duration_cast<std::chrono::duration<typename T::rep, typename T::period>>(t);
    };
}
