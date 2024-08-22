#pragma once

#include <thread>
#include <coroutine>

#include "TSUtil/TaskDispatcher.h"
#include "TSUtil/coroutine/Types.h"

namespace TSUtil::coro
{
    /// <summary>
    /// �θ�-�ڽ� ���踦 �����ϸ�, �½�ũ ���࿡ ������ ������ �ڷ�ƾ ������Ʈ
    ///
    /// �ڷ�ƾ �Լ� ȣ��� caller/callee ���� ������ ����� ���ڷ� �Ѱ��ִ� ���� �Ϲ����̳�
    /// �׷��� ����ϸ� ����ڰ� ���� ���ڷ� �Ѱ��־�� �ϸ�, �ŷ��� �� ���� ������ ���� ����� �ִ�.
    /// 
    /// (�ڷ�ƾ �Լ� ȣ�� => "�߰� ó��" => �簳 �� ���� ����)
    /// �� �帧�� �ǵ��ϰ�, �� ������ ���ο��� ó���ϱ� ���� ���۽� ��ٷ� �ߴ��ϵ��� �����Ѵ�.
    /// (std::suspend_always initial_suspend)
    /// 
    /// �ڽ� �ڷ�ƾ�� �����ϴ� ��� �θ� �ڷ�ƾ�� ���޹޾� �����ϸ�, 
    /// ������ �Ҹ�� �θ� �ڷ�ƾ�� �簳�Ѵ�.
    /// (��Ʋ�� �ڷ�ƾ�� ����(������ ���ü�)�� �����Ͽ� ����)
    /// </summary>
    class TaskCoroutine
    {
    public:
        struct promise_type;

    public:
        explicit TaskCoroutine(std::coroutine_handle<promise_type> handle)
            : handle_(handle)
        {}

    public:
        promise_type& promise() const { return handle_.promise(); }
        void resume(const TaskDispatchPoint& dispatchPoint);
        void setParent(std::coroutine_handle<promise_type> parentHandle);

    protected:
        std::coroutine_handle<promise_type> handle_ = nullptr;
    };

    /// <summary>
    /// TaskCoroutine �� ���� �θ� ���� ���̹Ƿ� ��������� task alias �� ����ϴ� ���� ����
    /// </summary>
    using task = TaskCoroutine;
}

#include "TaskCoroutine.hpp"
