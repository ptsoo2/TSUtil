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
    class CoroutineScope final
    {
    public:
        explicit CoroutineScope(TaskDispatcher* dispatcher = CoroutineService::getInstance().defaultDispatcher())
            : dispatcher_(dispatcher)
        {}

    protected:
        CoroutineScope(const CoroutineScope&) = delete;
        CoroutineScope& operator=(const CoroutineScope&) = delete;

    public:
        /// <summary>
        /// ĸ���� ���ڸ� tuple �� ���޹ް�, �����Ѵ�.
        /// 
        /// ���� �ڷ�ƾ���� ĸó ����� �����ϴ�.
        /// ���ٴ� ����� �ش� ������ ���� ������ �ڸ���� ����Ǹ�,
        /// ���� ������ �ߴ��ϰ�, �������� ����� �簳�� ĸó�� ����� �޸𸮰� ��ȿȭ �ǹǷ� �̷��� �ڵ� �ۼ� ����� ub �̴�.
        /// 
        /// ���� ���������� ĸó�� �� ������ ����Ѵ�. (Config.h �� COROUTINE_AVOID_LAMBDA_CAPTURE ����)
        /// ���� �ڷ�ƾ������ ĸó�� �ƿ� �Ⱦ��� ���� best practice �̴�. �ڵ� �ۼ����� �̰��� ����ϱ� �ſ� ��ƴ�.
        /// 
        /// ���ڷ� ������ ����� tuple �� ��Ƽ� ��ȸ�Ͽ� �����ϰ� ����Ѵ�.
        /// </summary>
        template <typename ...TArgs>
        [[nodiscard]] auto capture(TArgs&&... args) const&&
        {
            return detail::CapturedLaunchScope(
                dispatcher_,
                InnerCoroutineScope{ dispatcher_ }, std::forward<TArgs>(args)...
            );
        }

        /// <summary>
        /// �ڷ�ƾ�� �����Ѵ�.
        /// ���� �κ����� �����Ѵ�.
        /// </summary>
        template <typename TCallable>
        void launch(TCallable&& callable) const&&
        {
            CoroutineScope(dispatcher_)
                .capture()
                .launch(std::forward<TCallable>(callable));
        }

        /// <summary>
        /// �ڷ�ƾ�� �����Ѵ�.
        /// ������ ������� �����Ѵ�.
        /// </summary>
        template <typename TCallable>
        void launch(size_t tid, TCallable&& callable) const&&
        {
            CoroutineScope(dispatcher_)
                .capture()
                .launch(tid, std::forward<TCallable>(callable));
        }

    protected:
        TaskDispatcher* dispatcher_ = nullptr;
    };
}
