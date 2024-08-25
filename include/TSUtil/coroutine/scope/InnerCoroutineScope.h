#pragma once

#include "TSUtil/constraint/chrono.h"
#include "TSUtil/constraint/function.h"

#include "TSUtil/coroutine/CoroutineService.h"
#include "TSUtil/coroutine/object/TaskCoroutine.h"
#include "TSUtil/coroutine/scope/impl/CapturedLaunchScope.h"

namespace TSUtil::coro
{
    /// <summary>
    /// �ڷ�ƾ ���� ������
    /// CoroutineScope �� �ݸ��� ���� �������̽��� ������ ����
    /// </summary>
    class InnerCoroutineScope final
    {
        friend class CoroutineScope;

    protected:
        explicit InnerCoroutineScope(TaskDispatcher* dispatcher)
            : dispatcher_(dispatcher)
        {}

    public:
        /// <summary>
        /// ĸ���� ���ڸ� tuple �� ���޹ް�, �����Ѵ�.
        /// 
        /// �ڼ��� ������ CoroutineScope::capture �� ����
        /// </summary>
        template <typename ...TArgs>
        [[nodiscard]] auto capture(TArgs&&... args) const
        {
            return detail::CapturedLaunchAwaitScope(
                dispatcher_,
                InnerCoroutineScope{ dispatcher_ }, std::forward<TArgs>(args)...
            );
        }

        /// <summary>
        /// �ڷ�ƾ�� �����Ѵ�.
        /// ������ ������� �����Ѵ�.
        /// </summary>
        template <typename TCallable>
        [[nodiscard]] auto launch(size_t tid, TCallable&& callable) const
        {
            if constexpr (constraint_is_coroutine_function<TCallable, InnerCoroutineScope> == true)
            {
                // �ڷ�ƾ �Լ��� ��� InnerCoroutineScope �� �߰��� �����ϰ�,
                // awaiter �� ��ȯ�Ѵ�.
                return detail::do_launch_await{}(
                    tid,
                    dispatcher_,
                    std::forward<TCallable>(callable),
                    std::tuple{ InnerCoroutineScope{ dispatcher_ } }
                );
            }
            else
            {
                // �Ϲ� �Լ��� ��� ���� ������ ������,
                // void �Ǵ� �Լ� �������� ���Ͽ� ���� ��ȯ�Ѵ�.
                return detail::do_launch_await{}(
                    tid,
                    dispatcher_,
                    std::forward<TCallable>(callable),
                    {}
                );
            }
        }

        /// <summary>
        /// �ڷ�ƾ�� �����Ѵ�.
        /// ���� �κ����� �����Ѵ�.
        /// </summary>
        template <typename TCallable>
        [[nodiscard]] auto launch(TCallable&& callable) const
        {
            return launch(dispatcher_->genThreadId(), std::forward<TCallable>(callable));
        }

        /// <summary>
        /// ������ �ð���ŭ Ÿ�̸� ť���� ��� �� �簳
        /// </summary>
        template <constraint_duration TDuration>
        [[nodiscard]] auto delay(const TDuration& duration) const
        {
            return detail::do_delay{}(dispatcher_, duration);
        }

        /// <summary>
        /// ������ �ð���ŭ Ÿ�̸� ť���� ��� �� �簳
        /// </summary>
        [[nodiscard]] auto delay(time_t delayMiliiSec) const
        {
            return detail::do_delay{}(dispatcher_, std::chrono::milliseconds{ delayMiliiSec });
        }

    protected:
        TaskDispatcher* dispatcher_ = nullptr;
    };
}
