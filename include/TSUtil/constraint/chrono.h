#pragma once

#include <chrono>

namespace TSUtil
{
    /// <summary>
    /// std::chrono::duration<> 인자 제약 조건
    /// </summary>
    template <typename T>
    concept constraint_duration = requires(T t)
    {
        typename T::rep;    // T가 rep 타입을 가져야 함
        typename T::period; // T가 period 타입을 가져야 함
        std::chrono::duration_cast<std::chrono::duration<typename T::rep, typename T::period>>(t);
    };
}
