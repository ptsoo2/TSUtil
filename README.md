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
    |   +---impl
    |   |       Delay.h
    |   |       impl.h
    |   |       Launch.h
    |   |
    |   +---object
    |   |       object.h
    |   |       SampleCoroutine.h
    |   |       TaskCoroutine.h
    |   |       TaskCoroutine.hpp
    |   |
    |   \---scope
    |       |   CoroutineScope.h
    |       |   InnerCoroutineScope.h
    |       |   LambdaCoroutineScope.h
    |       |   scope.h
    |       |
    |       \---impl
    |               CapturedLaunchScope.h
    |               impl.h
    |
    \---synchronize
            SpinLock.h
            synchronize.h
            ThreadWaitState.h
````
