#pragma once

#include <functional>

#include "TSUtil/coroutine/Constraint.h"

namespace TSUtil::coro
{
    /// <summary>
    /// promise_type 얻기
    /// </summary>
    /// <typeparam name="TObject"></typeparam>
    template <constraint_is_coroutine_object TObject>
    using into_coro_promise_t = typename TObject::promise_type;

    /// <summary>
    /// coroutine_handle 타입
    /// </summary>
    /// <typeparam name="TObject"></typeparam>
    template <constraint_is_coroutine_object TObject>
    using into_coro_handle_t = std::coroutine_handle<into_coro_promise_t<TObject>>;

    /// <summary>
    /// 중단시 진행할 태스크
    /// </summary>
    /// <typeparam name="TObject"></typeparam>
    template <typename TRet, constraint_is_coroutine_object TObject>
    using fnSuspendTask_t = std::function<TRet(into_coro_handle_t<TObject>)>;
}

#include <thread>

// ptsoo todo - 임시
inline uint32_t get_thread_id()
{
    std::thread::id tid = std::this_thread::get_id();
    const uint32_t* nested_tid = (uint32_t*)(&tid);
    return *nested_tid;
}

#define PROBE(_format, ...) \
    { \
        char _buffer[128]; \
        sprintf_s(_buffer, sizeof(_buffer), _format, __VA_ARGS__); \
        printf("%s | %u | %s(%d) \n", \
            _buffer, get_thread_id(), __FUNCTION__, __LINE__); \
    }
