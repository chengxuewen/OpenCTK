//
// Created by cxw on 25-8-25.
//

#ifndef _OCTK_SINGLETON_HPP
#define _OCTK_SINGLETON_HPP

#include <octk_memory.hpp>

#include <mutex>

OCTK_BEGIN_NAMESPACE

#define OCTK_DECLARE_SINGLETON(CLASS)                                                                                  \
    friend class Singleton<CLASS>;                                                                                     \
    friend struct SingletonScopedPointerDeleter<CLASS>;


template <typename T> struct SingletonScopedPointerDeleter
{
    constexpr SingletonScopedPointerDeleter() noexcept = default;
    void operator()(T *pointer) const noexcept
    {
        static_assert(sizeof(T) >= 0, "cannot delete an incomplete type");
        static_assert(!std::is_void<T>::value, "cannot delete an incomplete type");
        delete pointer;
    }
};


template <typename T> class Singleton
{
    typedef SingletonScopedPointerDeleter<T> Deleter;

public:
    typedef void (*InitFunc)(T *);

    static T *instance()
    {
        std::call_once(mOnceFlag, create);
        return mInstance;
    }

    template <InitFunc func = nullptr> static T *instance()
    {
        std::call_once(mOnceFlag,
                       []()
                       {
                           create();
                           if (func)
                           {
                               func(mInstance);
                           }
                       });
        return mInstance;
    }

    void destroy()
    {
        this->onAboutToBeDestroyed();
        delete this->detachScoped();
    }

protected:
    Singleton() { }
    virtual ~Singleton() { }

    virtual void onAboutToBeDestroyed() { }
    T *detachScoped()
    {
        mScoped.release();
        return mInstance;
    }

private:
    static void create()
    {
        mScoped.reset(new T);
        mInstance = mScoped.get();
    }

    static T *mInstance;
    static std::once_flag mOnceFlag;
    static std::unique_ptr<T, Deleter> mScoped;
    OCTK_DISABLE_COPY_MOVE(Singleton)
};

template <typename T> T *Singleton<T>::mInstance = nullptr;
template <typename T> std::once_flag Singleton<T>::mOnceFlag;
template <typename T> std::unique_ptr<T, SingletonScopedPointerDeleter<T>> Singleton<T>::mScoped(nullptr);

OCTK_END_NAMESPACE

#endif // _OCTK_SINGLETON_HPP
