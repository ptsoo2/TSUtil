#pragma once

#include "TSUtil/TaskDispatcher.h"

namespace TSUtil::coro::detail
{
    template <typename ...TArgs>
    class LambdaCoroutineScope
    {
    public:
        explicit LambdaCoroutineScope(TaskDispatcher* dispatcher, TArgs&&... args)
            : dispatcher_(dispatcher)
            , args_(std::forward<TArgs>(args)...)
        {}

    protected:
        TaskDispatcher* dispatcher_ = nullptr;
        std::tuple<TArgs...> args_;
    };
}
