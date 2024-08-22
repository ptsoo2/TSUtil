# TSUtil

![Build Status](https://github.com/ptsoo2/TSUtil/actions/workflows/cmake-multi-platform.yml/badge.svg)


# include
````
\---TSUtil
    |   TaskDispatcher.h
    |   ThreadPool.h
    |   TSUtil.h
    |
    +---constraint
    |       chrono.h
    |       constraint.h
    |       function.h
    |       multiple.h
    |
    +---container
    |       container.h
    |       MPSCQueue.h
    |       MPSCQueue.hpp
    |       TimerQueue.h
    |
    +---coroutine
    |   |   Config.h
    |   |   Constraint.h
    |   |   coroutine.h
    |   |   CoroutineDispatcher.h
    |   |   CoroutineDispatcher.hpp
    |   |   CoroutineService.h
    |   |   Types.h
    |   |
    |   +---awaiter
    |   |       awaiter.h
    |   |       GetHandle.h
    |   |       OptionalSuspend.h
    |   |       SuspendThen.h
    |   |       SuspendThenStorage.h
    |   |
    |   +---object
    |   |       object.h
    |   |       SampleCoroutine.h
    |   |       TaskCoroutine.h
    |   |       TaskCoroutine.hpp
    |   |
    |   \---scope
    |           CoroutineScope.h
    |           CoroutineScope.hpp
    |           InnerCoroutineScope.h
    |           InnerCoroutineScope.hpp
    |           scope.h
    |
    \---synchronize
            SpinLock.h
            synchronize.h
            ThreadWaitState.h
````
