#pragma once

#include "TSUtil/coroutine/Constraint.h"
#include "TSUtil/coroutine/CoroutineService.h"

#include "TSUtil/coroutine/object/TaskCoroutine.h"
#include "TSUtil/coroutine/scope/InnerCoroutineScope.h"

namespace TSUtil::coro
{
    /// <summary>
    /// �ڷ�ƾ�� ó�� �����ϱ� ���� ������
    /// ��Ʋ�� �ڷ�ƾ�� ����(������ ���ü�)�� �����Ͽ� ����
    /// </summary>
    class CoroutineScope
    {
    public:
        explicit CoroutineScope(TaskDispatcher* dispatcher = CoroutineService::getInstance().defaultDispatcher())
            : dispatcher_(dispatcher)
        {}

    protected:
        CoroutineScope(const CoroutineScope&) = delete;
        CoroutineScope& operator=(const CoroutineScope&) = delete;

    protected:
        template <typename ...TArgs>
        struct CaptureWrapper;

    public:
        /// <summary>
        /// ���� �ڷ�ƾ���� ĸó ����� �����ϴ�.
        /// 
        /// ���ٴ� ����� �ش� ������ ���� ������ �ڸ���� ����Ǹ�,
        /// ���� ������ �ߴ��ϰ�, �������� ����� �簳�� ĸó�� ����� �޸𸮰� ��ȿȭ �ǹǷ� �̷��� �ڵ� �ۼ� ����� ub �̴�.
        /// 
        /// ���� ���������� ĸó�� �� ������ ����Ѵ�. (Config.h �� COROUTINE_AVOID_LAMBDA_CAPTURE ����)
        /// ���� �ڷ�ƾ������ ĸó�� �ƿ� �Ⱦ��� ���� best practice �̴�. �ڵ� �ۼ����� �̰��� ����ϱ� �ſ� ��ƴ�.
        /// 
        /// ���ڷ� ������ ����� tuple �� ��Ƽ� ��ȸ�Ͽ� �����ϰ� ����Ѵ�.
        /// </summary>
        template <typename ...TArgs>
        auto capture(TArgs&&... args) const&&;

        /// <summary>
        /// �ڷ�ƾ�� �����Ѵ�.
        /// </summary>
        template <typename TCallable>
        void launch(TCallable&& callable) const&&;

    protected:
        TaskDispatcher* dispatcher_ = nullptr;
    };
}

#include "CoroutineScope.hpp"
