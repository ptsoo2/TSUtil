#pragma once

/// <summary>
/// ������Ʈ�� ���Ե��� ������, �ڷ�ƾ cheat sheet �뵵�� �ۼ�
/// </summary>
#if 0

struct SampleCoroutine
{
    struct promise_type;

    SampleCoroutine(std::coroutine_handle<promise_type> handle)
        : handle_(handle)
    {}

    std::coroutine_handle<promise_type> handle_;
};

struct SampleCoroutine::promise_type
{
    SampleCoroutine get_return_object() { return std::coroutine_handle<promise_type>::from_promise(*this); }
    std::suspend_never initial_suspend() noexcept { return {}; }
    std::suspend_never final_suspend() noexcept { return {}; }
    void return_void() {}
    void unhandled_exception() {}
};

#endif
