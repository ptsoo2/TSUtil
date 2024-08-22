#include "CoroutineScope.h"

namespace TSUtil::coro
{
    template <typename ...TArgs>
    struct CoroutineScope::CaptureWrapper
    {
        /// <summary>
        /// 인자를 포함해서 코루틴을 시작한다.
        /// </summary>
        template <typename TCallable>
            requires constraint_is_coroutine_function<TCallable, TArgs...>
        void launch(TCallable&& callable)&&
        {
            dispatcher_->dispatch(
                [dispatcher = dispatcher_, callable = std::move(callable), args = std::move(args_)]()
                {
                    TaskDispatchPoint dispatchPoint{ dispatcher->dispatchPoint() };
                    TaskCoroutine task = std::apply(std::move(callable), std::move(args));

                    task.resume(dispatchPoint);
                }
            );
        }

        TaskDispatcher* dispatcher_ = nullptr;
        std::tuple<TArgs...> args_;
    };

    template <typename ...TArgs>
    inline auto CoroutineScope::capture(TArgs&& ...args) const&&
    {
        return CaptureWrapper<InnerCoroutineScope, TArgs...>{
            dispatcher_,
            { InnerCoroutineScope{ dispatcher_ }, std::forward<TArgs>(args)... }
        };
    }

    template <typename TCallable>
    inline void CoroutineScope::launch(TCallable&& callable) const&&
    {
        CoroutineScope{ dispatcher_ }
            .capture()
            .launch(std::move(callable));
    }
}
