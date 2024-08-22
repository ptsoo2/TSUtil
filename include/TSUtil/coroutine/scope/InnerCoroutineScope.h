#pragma once

#include "TSUtil/coroutine/CoroutineService.h"

#include "TSUtil/coroutine/awaiter/SuspendThen.h"
#include "TSUtil/coroutine/awaiter/SuspendThenStorage.h"

#include "TSUtil/coroutine/object/TaskCoroutine.h"

#include "TSUtil/constraint/chrono.h"
#include "TSUtil/constraint/function.h"

namespace TSUtil::coro::detail
{
    struct do_delay;
    struct do_launch;
}

namespace TSUtil::coro
{
    /// <summary>
    /// �ڷ�ƾ ���� ������
    /// CoroutineScope �� �ݸ��� ���� �������̽��� ������ ����
    /// </summary>
    class InnerCoroutineScope final
    {
        friend class CoroutineScope;
        friend struct detail::do_delay;
        friend struct detail::do_launch;

    protected:
        explicit InnerCoroutineScope(TaskDispatcher* dispatcher)
            : dispatcher_(dispatcher)
        {}

    public:
        /// <summary>
        /// �Ϲ� �Լ� ����(���� �κ� ������)
        /// </summary>
        template <typename TCallable>
            requires constraint_is_coroutine_function_flag<false, TCallable>
        [[nodiscard]] auto launch(TCallable&& callable) const;

        /// <summary>
        /// �Ϲ� �Լ� ����(���� ������)
        /// </summary>
        template <typename TCallable>
            requires constraint_is_coroutine_function_flag<false, TCallable>
        [[nodiscard]] auto launch(size_t tid, TCallable&& callable) const;

        /// <summary>
        /// �ڷ�ƾ �Լ� ����(���� �κ� ������)
        /// </summary>
        template <typename TCallable>
            requires constraint_is_coroutine_function_flag<true, TCallable, InnerCoroutineScope>
        [[nodiscard]] auto launch(TCallable&& callable) const;

        /// <summary>
        /// �ڷ�ƾ �Լ� ����(���� ������)
        /// </summary>
        template <typename TCallable>
            requires constraint_is_coroutine_function_flag<true, TCallable, InnerCoroutineScope>
        [[nodiscard]] auto launch(size_t tid, TCallable&& callable) const;

        /// <summary>
        /// ������ �ð���ŭ Ÿ�̸� ť���� ��� �� �簳
        /// </summary>
        template <constraint_duration TDuration>
        [[nodiscard]] auto delay(const TDuration& duration) const;

        /// <summary>
        /// ������ �ð���ŭ Ÿ�̸� ť���� ��� �� �簳
        /// </summary>
        [[nodiscard]] auto delay(time_t delayMiliiSec) const;

    protected:
        TaskDispatcher* dispatcher_ = nullptr;
    };
}

#include "InnerCoroutineScope.hpp"
