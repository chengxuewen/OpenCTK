/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2025~Present chengxuewen.
**
** License: MIT License
**
** Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
** documentation files (the "Software"), to deal in the Software without restriction, including without limitation
** the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
** and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
**
** The above copyright notice and this permission notice shall be included in all copies or substantial portions
** of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
** TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
** THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
** CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
** IN THE SOFTWARE.
**
***********************************************************************************************************************/

#ifndef _OCTK_MACROS_HPP
#define _OCTK_MACROS_HPP

#include <octk_system.hpp>
#include <octk_macros.hpp>
#include <octk_compiler.hpp>

/***********************************************************************************************************************
 * version macro
***********************************************************************************************************************/
// OCTK_VERSION is (major << 16) + (minor << 8) + patch.
#define OCTK_VERSION OCTK_VERSION_CHECK(OCTK_VERSION_MAJOR, OCTK_VERSION_MINOR, OCTK_VERSION_PATCH)
// can be used like #if (OCTK_VERSION >= OCTK_VERSION_CHECK(0, 3, 1))
#define OCTK_VERSION_CHECK(major, minor, patch) ((major << 16) | (minor << 8) | (patch))


/***********************************************************************************************************************
 * namespace macro
***********************************************************************************************************************/
#define OCTK_NAMESPACE octk
#define OCTK_PREPEND_NAMESPACE(name) ::OCTK_NAMESPACE::name
#define OCTK_USE_NAMESPACE using namespace ::OCTK_NAMESPACE;
#define OCTK_BEGIN_NAMESPACE OCTK_WARNING_PUSH OCTK_WARNING_DISABLE_MSVC(4251) namespace OCTK_NAMESPACE {
#define OCTK_END_NAMESPACE } OCTK_WARNING_POP
#define OCTK_BEGIN_INCLUDE_NAMESPACE }
#define OCTK_END_INCLUDE_NAMESPACE namespace OCTK_NAMESPACE {
#define OCTK_FORWARD_DECLARE_CLASS(name) \
    OCTK_BEGIN_NAMESPACE class name; OCTK_END_NAMESPACE \
    using OCTK_PREPEND_NAMESPACE(name);

#define OCTK_FORWARD_DECLARE_STRUCT(name) \
    OCTK_BEGIN_NAMESPACE struct name; OCTK_END_NAMESPACE \
    using OCTK_PREPEND_NAMESPACE(name);

#define OCTK_MANGLE_NAMESPACE0(x) x
#define OCTK_MANGLE_NAMESPACE1(a, b) a##_##b
#define OCTK_MANGLE_NAMESPACE2(a, b) OCTK_MANGLE_NAMESPACE1(a,b)
#define OCTK_MANGLE_NAMESPACE(name) OCTK_MANGLE_NAMESPACE2( \
    OCTK_MANGLE_NAMESPACE0(name), OCTK_MANGLE_NAMESPACE0(OCTK_NAMESPACE))

namespace OCTK_NAMESPACE {}


/***********************************************************************************************************************
 * compiler cxx11 feature macro declare
***********************************************************************************************************************/
#if OCTK_CC_FEATURE_NULLPTR
#   define OCTK_NULLPTR nullptr
#else
#   define OCTK_NULLPTR NULL
#endif

#if OCTK_CC_FEATURE_CONSTEXPR
#   define OCTK_CONSTEXPR constexpr
#   define OCTK_RELAXED_CONSTEXPR constexpr
#else
#   define OCTK_CONSTEXPR
#   define OCTK_RELAXED_CONSTEXPR const
#endif

#if OCTK_CC_FEATURE_EXPLICIT_OVERRIDES
#   define OCTK_OVERRIDE override
#   define OCTK_FINAL final
#else
#   define OCTK_OVERRIDE
#   define OCTK_FINAL
#endif

#if OCTK_CC_FEATURE_NOEXCEPT
#   define OCTK_NOEXCEPT noexcept
#   define OCTK_NOEXCEPT_EXPR(x) noexcept(x)
#else
#   define OCTK_NOEXCEPT
#   define OCTK_NOEXCEPT_EXPR(x)
#endif
#define OCTK_NOTHROW OCTK_NOEXCEPT

#if OCTK_CC_FEATURE_DEFAULT_MEMBERS
#   define OCTK_EQ_DEFAULT = default
#   define OCTK_EQ_DEFAULT_FUNC = default;
#else
#   define OCTK_EQ_DEFAULT
#   define OCTK_EQ_DEFAULT_FUNC {}
#endif

#if OCTK_CC_FEATURE_DELETE_MEMBERS
#   define OCTK_EQ_DELETE = delete
#   define OCTK_EQ_DELETE_FUNC = delete;
#else
#   define OCTK_EQ_DELETE
#   define OCTK_EQ_DELETE_FUNC {}
#endif

#if OCTK_CC_FEATURE_ALIGNOF
#   define OCTK_ALIGNOF(x)  alignof(x)
#else
#   define OCTK_ALIGNOF(x)
#endif

#if OCTK_CC_FEATURE_ALIGNAS
#   define OCTK_ALIGN(n)   alignas(n)
#else
#   define OCTK_ALIGN(n)
#endif


/***********************************************************************************************************************
  * disable copy move macro declare
***********************************************************************************************************************/
#define OCTK_DECLARE_DISABLE_COPY(Class) \
    Class(const Class &) OCTK_EQ_DELETE; \
    Class &operator=(const Class &) OCTK_EQ_DELETE;

#if OCTK_CC_FEATURE_RVALUE_REFS
#   define OCTK_DECLARE_DISABLE_MOVE(Class) \
    Class(Class &&) OCTK_EQ_DELETE; \
    Class &operator=(Class &&) OCTK_EQ_DELETE;
#else
#   define OCTK_DECLARE_DISABLE_MOVE(Class)
#endif

#define OCTK_DISABLE_COPY_MOVE(Class) \
    OCTK_DECLARE_DISABLE_COPY(Class) \
    OCTK_DECLARE_DISABLE_MOVE(Class)


/***********************************************************************************************************************
  * static variable macro
***********************************************************************************************************************/
#define OCTK_STATIC_CONSTANT_NUMBER(name, number) static constexpr decltype(number) name = number;
#define OCTK_STATIC_CONSTANT_STRING(name, string) static constexpr char name[] = string;


/***********************************************************************************************************************
 * class private implementation macro
***********************************************************************************************************************/
OCTK_BEGIN_NAMESPACE
template <typename T> inline T *getPointerHelper(T *ptr) { return ptr; }
template <typename Wrapper>
static inline typename Wrapper::pointer getPointerHelper(const Wrapper &p) { return p.get(); }
template <typename Wrapper>
static inline typename Wrapper::Pointer getPointerHelper(const Wrapper &p) { return p.data(); }
OCTK_END_NAMESPACE

// The body must be a statement:
#define OCTK_CAST_IGNORE_ALIGN(body) \
    OCTK_WARNING_PUSH \
    OCTK_WARNING_DISABLE_GCC("-Wcast-align") \
    body \
    OCTK_WARNING_POP

#define OCTK_DEFINE_DPTR(Class) \
    std::unique_ptr<Class##Private> mDPtr;

#define OCTK_DECLARE_PRIVATE(Class) \
    inline Class##Private *dFunc() \
    { OCTK_CAST_IGNORE_ALIGN(return reinterpret_cast<Class##Private *>(octk::getPointerHelper(mDPtr));) } \
    inline const Class##Private *dFunc() const \
    { OCTK_CAST_IGNORE_ALIGN(return reinterpret_cast<const Class##Private *>(octk::getPointerHelper(mDPtr));) } \
    friend class Class##Private;

#define OCTK_DECLARE_PRIVATE_D(DPtr, Class) \
    inline Class##Private *dFunc() \
    { OCTK_CAST_IGNORE_ALIGN(return reinterpret_cast<Class##Private *>(octk::getPointerHelper(DPtr));) } \
    inline const Class##Private *dFunc() const \
    { OCTK_CAST_IGNORE_ALIGN(return reinterpret_cast<const Class##Private *>(octk::getPointerHelper(DPtr));) } \
    friend class Class##Private;

#define OCTK_DEFINE_PPTR(Class) \
    Class * const mPPtr;

#define OCTK_DECLARE_PUBLIC(Class) \
    inline Class* pFunc() { return static_cast<Class *>(mPPtr); } \
    inline const Class* pFunc() const { return static_cast<const Class *>(mPPtr); } \
    friend class Class;

#define OCTK_DECLARE_PUBLIC_P(PPtr, Class) \
    inline Class* pFunc() { return static_cast<Class *>(PPtr); } \
    inline const Class* pFunc() const { return static_cast<const Class *>(PPtr); } \
    friend class Class;

#define OCTK_D(Class) Class##Private * const d = dFunc()
#define OCTK_P(Class) Class * const p = pFunc()


/***********************************************************************************************************************
 * OpenCTK force inline macro declare
***********************************************************************************************************************/
#if defined(OCTK_CC_MSVC)
#   define OCTK_FORCE_INLINE   __forceinline
#   define OCTK_NO_INLINE      __declspec(noinline)
#   define OCTK_USED
#elif defined(OCTK_CC_GNU)
#   define OCTK_FORCE_INLINE   inline __attribute__((always_inline))
#   define OCTK_NO_INLINE      __attribute__((noinline))
#   define OCTK_USED           __attribute__((used))
#elif defined(OCTK_CC_CLANG)
#   define OCTK_FORCE_INLINE   inline __attribute__((always_inline))
#   define OCTK_NO_INLINE
#   define OCTK_USED           __attribute__((used))
#else
#   define OCTK_FORCE_INLINE   inline
#   define OCTK_NO_INLINE
#   define OCTK_USED
#endif


/***********************************************************************************************************************
 * noreturn macro declare, annotate a function that will not return control flow to the caller.
***********************************************************************************************************************/
#if defined(OCTK_CC_MSVC)
#   define OCTK_NORETURN   __declspec(noreturn)
#elif defined(OCTK_CC_GNU) || defined(OCTK_CC_CLANG)
#   define OCTK_NORETURN   __attribute__((__noreturn__))
#else
#   define OCTK_NORETURN
#endif


/***********************************************************************************************************************
 * provide a string identifying the current function, non-concatenatable macro
***********************************************************************************************************************/
#if defined (__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
    #define OCTK_STRFUNC     ((const char*) (__func__))
#elif defined (__GNUC__) && defined (__cplusplus)
    #define OCTK_STRFUNC     ((const char*) (__PRETTY_FUNCTION__))
#elif defined (__GNUC__) || (defined(_MSC_VER) && (_MSC_VER > 1300))
    #define OCTK_STRFUNC     ((const char*) (__FUNCTION__))
#elif defined(OCTK_CC_MSVC)
    #define OCTK_STRFUNC     ((const char*) (__FUNCSIG__))
#else
    #define OCTK_STRFUNC     ((const char*) ("???"))
#endif

/***********************************************************************************************************************
 * deprecated macro
***********************************************************************************************************************/
#if __has_cpp_attribute(deprecated)
    #define OCTK_DEPRECATED [[deprecated]]
    #define OCTK_DEPRECATED_X(text) [[deprecated(text)]]
#elif (OCTK_CC_GNU >= 405) || defined(OCTK_CC_CLANG) || OCTK_CC_HAS_ATTRIBUTE(__deprecated__)
#   define OCTK_DEPRECATED __attribute__((__deprecated__))
#   define OCTK_DEPRECATED_X(text) __attribute__((__deprecated__(text)))
#elif defined(OCTK_CC_MSVC) && (OCTK_CC_MSVC >= 1300)
    #define OCTK_DEPRECATED __declspec(deprecated)
    #define OCTK_DEPRECATED_X(text) __declspec(deprecated(text))
#elif defined(OCTK_CC_INTEL) && (__INTEL_COMPILER >= 1300) && !defined(__APPLE__)
    #define OCTK_DEPRECATED __attribute__(__deprecated__)
    #define OCTK_DEPRECATED_X(text) __attribute__((__deprecated__(text)))
#else
    #define OCTK_DEPRECATED
    #define OCTK_DEPRECATED_X(text) OCTK_DEPRECATED
#endif


/***********************************************************************************************************************
 * likely unlikely macro define
***********************************************************************************************************************/
#if defined(OCTK_CC_GNU) && (OCTK_CC_GNU >= 200) && defined(__OPTIMIZE__)
    #define OCTK_LIKELY(expr)    __builtin_expect(!!(expr), true)
    #define OCTK_UNLIKELY(expr)  __builtin_expect(!!(expr), false)
#else
    #define OCTK_LIKELY(expr) (expr)
    #define OCTK_UNLIKELY(expr) (expr)
#endif


/***********************************************************************************************************************
 * utils macro
***********************************************************************************************************************/
/* Avoid "unused parameter" warnings */
#define OCTK_UNUSED(x) (void)x;

/* Pragma keyword */
#if defined(_MSC_VER)
#   define OCTK_PRAGMA(X) __pragma(X)
#else
#   define OCTK_PRAGMA(X) _Pragma(#X)
#endif

/* Stringify macro or string */
#define OCTK_STRINGIFY(macro_or_string)     OCTK_STRINGIFY_ARG(macro_or_string)
#define OCTK_STRINGIFY_ARG(contents)        #contents

/* Count the number of elements in an array. The array must be defined
 * as such; using this with a dynamically allocated array will give
 * incorrect results.
 */
#define OCTK_ELEMENTS_NUM(arr)  (sizeof(arr) / sizeof((arr)[0]))

#define OCTK_ZERO_INIT  { 0 }

#define OCTK_ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))


/***********************************************************************************************************************
 * var exported in windows dlls macro define
***********************************************************************************************************************/
#ifdef OCTK_OS_WIN32
#   ifndef OCTK_BUILD_SHARED
#       define OCTK_EXTERN_VAR extern
#   else /* !OCTK_BUILD_SHARED */
#       ifdef OCTK_BUILD_CORE_LIB
#           define OCTK_EXTERN_VAR extern __declspec(dllexport)
#       else /* !OCTK_BUILD_CORE_LIB */
#           define OCTK_EXTERN_VAR extern __declspec(dllimport)
#       endif /* !OCTK_BUILD_CORE_LIB */
#   endif /* !OCTK_BUILD_SHARED */
#else /* !OCTK_OS_WIN32 */
#   define OCTK_EXTERN_VAR extern
#endif /* !OCTK_OS_WIN32 */


/***********************************************************************************************************************
 * limits macro
***********************************************************************************************************************/
/* # chars in a path name including nul */
#ifdef PATH_MAX
#   define OCTK_PATH_MAX PATH_MAX
#else
#   define OCTK_PATH_MAX (4096)
#endif
#ifdef LINE_MAX
#   define OCTK_LINE_MAX LINE_MAX
#else
#   define OCTK_LINE_MAX (4096)
#endif


/***********************************************************************************************************************
 * has feature macro define
***********************************************************************************************************************/
/*
 * Clang feature detection: http://clang.llvm.org/docs/LanguageExtensions.html These are not available on GCC, but since
 * the pre-processor doesn't do operator short-circuiting, we can't use it in a statement or we'll get:
 *
 * error: missing binary operator before token "(" So we define it to 0 to satisfy the pre-processor.
 */
#ifdef __has_feature
#   define OCTK_CC_HAS_FEATURE __has_feature
#else
#   define OCTK_CC_HAS_FEATURE(x) 0
#endif

#ifdef __has_builtin
#   define OCTK_CC_HAS_BUILTIN __has_builtin
#else
#   define OCTK_CC_HAS_BUILTIN(x) 0
#endif

#ifdef __has_extension
#   define OCTK_CC_HAS_EXTENSION __has_extension
#else
#   define OCTK_CC_HAS_EXTENSION(x) 0
#endif

/*
 * Attribute support detection. Works on clang and GCC >= 5
 * https://clang.llvm.org/docs/LanguageExtensions.html#has-attribute
 * https://gcc.gnu.org/onlinedocs/cpp/_005f_005fhas_005fattribute.html
 */
#ifdef __has_attribute
#   define OCTK_CC_HAS_ATTRIBUTE __has_attribute
#else
#   define OCTK_CC_HAS_ATTRIBUTE(X) 0
#endif

#ifdef __has_include
#   define OCTK_CC_HAS_INCLUDE __has_include
#else
#   define OCTK_CC_HAS_INCLUDE(X) 0
#endif


/***********************************************************************************************************************
 * attribute macro define
***********************************************************************************************************************/
#if defined(__clang__) && (!defined(SWIG))
#   define OCTK_ATTRIBUTE(x) __attribute__((x))
#else
#   define OCTK_ATTRIBUTE(x)  // no-op
#endif

/**
 * @brief It is used for declaring functions and arguments which may never be used.
 * It avoids possible compiler warnings.
 * For functions, place the attribute after the declaration, just before the semicolon.
 * It cannot go in the definition of a function, only the declaration. For arguments, place the attribute at the
 * beginning of the argument declaration.
 * @code
 * void my_unused_function(OCTK_ATTRIBUTE_UNUSED int unused_argument, int other_argument) OCTK_ATTRIBUTE_UNUSED;
 * @endcode
 */
#if OCTK_CC_HAS_ATTRIBUTE(__unused__)
#   define OCTK_ATTRIBUTE_UNUSED __attribute__((__unused__))
#else
#   define OCTK_ATTRIBUTE_UNUSED
#endif

/**
 * Declaring a function as `pure` enables better optimization of calls to the function.
 * A `pure` function has no effects except its return value and the return value depends only on the parameters
 * and/or global variables.
 * Place the attribute after the declaration, just before the semicolon.
 * @code
 * bool octk_type_check_value(const Value *value) OCTK_ATTRIBUTE_PURE;
 * @endcode
 */
#if OCTK_CC_HAS_ATTRIBUTE(__pure__)
#   define OCTK_ATTRIBUTE_PURE __attribute__((__pure__))
#else
#   define OCTK_ATTRIBUTE_PURE
#endif

/**
 * @param format_idx: the index of the argument corresponding to the format string (the arguments are numbered from 1)
 * @param arg_idx: the index of the first of the format arguments, or 0 if there are no format arguments
 * This is used for declaring functions which take a variable number of arguments, with the same syntax as `printf()`.
 * It allows the compiler to type-check the arguments passed to the function.
 * Place the attribute after the function declaration, just before the semicolon.
 * @code
 * int my_snprintf(char *string, long n, char const *format, ...) OCTK_ATTRIBUTE_FORMAT_PRINTF(3, 4);
 * @endcode
 */
#if OCTK_CC_HAS_ATTRIBUTE(__format__)
#   if !defined(__clang__) && OCTK_CC_GNU_CHECK_VERSION(4, 4)
#       define OCTK_ATTRIBUTE_FORMAT_PRINTF(format_idx, arg_idx) \
            __attribute__((__format__(gnu_printf, (format_idx), (arg_idx))))
#   else
#       define OCTK_ATTRIBUTE_FORMAT_PRINTF(format_idx, arg_idx) \
            __attribute__((__format__(__printf__, (format_idx), (arg_idx))))
#   endif
#elif defined(OCTK_CC_GNU) && !defined(__INSURE__)
#   if defined(OCTK_CC_MINGW) && !defined(OCTK_CC_CLANG)
#       define OCTK_ATTRIBUTE_FORMAT_PRINTF(format_idx, arg_idx) \
            __attribute__((format(gnu_printf, (format_idx), (arg_idx))))
#   else
#       define OCTK_ATTRIBUTE_FORMAT_PRINTF(format_idx, arg_idx) \
            __attribute__((format(printf, (format_idx), (arg_idx))))
#   endif
#else
#   define OCTK_ATTRIBUTE_FORMAT_PRINTF(A, B)
#endif

/**
 * @brief Document if a shared variable/field needs to be protected by a lock.
 * GUARDED_BY allows the user to specify a particular lock that should be held when accessing the annotated variable.
 */
#if OCTK_CC_HAS_ATTRIBUTE(guarded_by)
#   define OCTK_ATTRIBUTE_GUARDED_BY(x) __attribute__((guarded_by(x)))
#else
#   define OCTK_ATTRIBUTE_GUARDED_BY(x)
#endif

/**
 * @brief Document if the memory location pointed to by a pointer should be guarded by a lock when dereferencing the
 * pointer. Note that a pointer variable to a shared memory location could itself be a shared variable.
 * For example, if a shared global pointer q, which is guarded by mu1, points to a shared memory location that is
 * guarded by mu2, q should be annotated as follows:
 * @code
 * int *qGUARDED_BY(mu1) OCTK_ATTRIBUTE_PT_GUARDED_BY(mu2);
 * @endcode
 */
#if OCTK_CC_HAS_ATTRIBUTE(pt_guarded_by)
#   define OCTK_ATTRIBUTE_PT_GUARDED_BY(x) __attribute__((pt_guarded_by(x)))
#else
#   define OCTK_ATTRIBUTE_PT_GUARDED_BY(x)
#endif

/**
 * @brief Document the lock the annotated function returns without acquiring it.
 */
#if OCTK_CC_HAS_ATTRIBUTE(lock_returned)
#   define OCTK_ATTRIBUTE_LOCK_RETURNED(x) __attribute__((lock_returned(x)))
#else
#   define OCTK_ATTRIBUTE_LOCK_RETURNED(x)
#endif

/**
 * @brief Document if a class/type is a lockable type (such as the mutex type).
 */
#if OCTK_CC_HAS_ATTRIBUTE(lockable)
#   define OCTK_ATTRIBUTE_LOCKABLE __attribute__((lockable))
#else
#   define OCTK_ATTRIBUTE_LOCKABLE
#endif

/**
 * @brief Document if a class is a scoped lockable type (such as the MutexLock class).
 */
#if OCTK_CC_HAS_ATTRIBUTE(scoped_lockable)
#   define OCTK_ATTRIBUTE_SCOPED_LOCKABLE __attribute__((scoped_lockable))
#else
#   define OCTK_ATTRIBUTE_SCOPED_LOCKABLE
#endif

// The following annotations specify lock and unlock primitives.
#if OCTK_CC_HAS_ATTRIBUTE(exclusive_lock_function)
#   define OCTK_ATTRIBUTE_EXCLUSIVE_LOCK_FUNCTION(...) __attribute__((exclusive_lock_function(__VA_ARGS__)))
#else
#   define OCTK_ATTRIBUTE_EXCLUSIVE_LOCK_FUNCTION(...)
#endif
#if OCTK_CC_HAS_ATTRIBUTE(shared_lock_function)
#   define OCTK_ATTRIBUTE_SHARED_LOCK_FUNCTION(...) __attribute__((shared_lock_function(__VA_ARGS__)))
#else
#   define OCTK_ATTRIBUTE_SHARED_LOCK_FUNCTION(...)
#endif
#if OCTK_CC_HAS_ATTRIBUTE(exclusive_trylock_function)
#   define OCTK_ATTRIBUTE_EXCLUSIVE_TRYLOCK_FUNCTION(...) __attribute__((exclusive_trylock_function(__VA_ARGS__)))
#else
#   define OCTK_ATTRIBUTE_EXCLUSIVE_TRYLOCK_FUNCTION(...)
#endif
#if OCTK_CC_HAS_ATTRIBUTE(shared_trylock_function)
#   define OCTK_ATTRIBUTE_SHARED_TRYLOCK_FUNCTION(...) __attribute__((shared_trylock_function(__VA_ARGS__)))
#else
#   define OCTK_ATTRIBUTE_SHARED_TRYLOCK_FUNCTION(...)
#endif
#if OCTK_CC_HAS_ATTRIBUTE(unlock_function)
#   define OCTK_ATTRIBUTE_UNLOCK_FUNCTION(...) __attribute__((unlock_function(__VA_ARGS__)))
#else
#   define OCTK_ATTRIBUTE_UNLOCK_FUNCTION(...)
#endif
#if OCTK_CC_HAS_ATTRIBUTE(assert_exclusive_lock)
#   define OCTK_ATTRIBUTE_ASSERT_EXCLUSIVE_LOCK(...) __attribute__((assert_exclusive_lock(__VA_ARGS__)))
#else
#   define OCTK_ATTRIBUTE_ASSERT_EXCLUSIVE_LOCK(...)
#endif

/**
 * @brief An escape hatch for thread safety analysis to ignore the annotated function.
 */
#if OCTK_CC_HAS_ATTRIBUTE(no_thread_safety_analysis)
#   define OCTK_ATTRIBUTE_NO_THREAD_SAFETY_ANALYSIS __attribute__((no_thread_safety_analysis))
#else
#   define OCTK_ATTRIBUTE_NO_THREAD_SAFETY_ANALYSIS
#endif

/**
 * @brief OCTK_NO_UNIQUE_ADDRESS is a portable annotation to tell the compiler that a data member need not have an
 * address distinct from all other non-static data members of its class.
 * It allows empty types to actually occupy zero bytes as class members, instead of occupying at least one byte just
 * so that they get their own address.
 * There is almost never any reason not to use it on class members that could possibly be empty.
 * The macro expands to [[no_unique_address]] if the compiler supports the attribute, it expands to nothing otherwise.
 * Clang should supports this attribute since C++11, while other compilers should add support for it starting from
 * C++20. Among clang compilers, clang-cl doesn't support it yet and support is unclear also when the target platform
 * is iOS.
 */
#if OCTK_CC_HAS_ATTRIBUTE(no_unique_address)
#   define OCTK_ATTRIBUTE_NO_UNIQUE_ADDRESS [[no_unique_address]] // NOLINTNEXTLINE(whitespace/braces)
#else
#   define OCTK_ATTRIBUTE_NO_UNIQUE_ADDRESS
#endif

#endif // _OCTK_MACROS_HPP
