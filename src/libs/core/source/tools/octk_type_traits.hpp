#ifndef _OCTK_TYPE_TRAITS_H_
#define _OCTK_TYPE_TRAITS_H_

#include <octk_global.hpp>

#include <cstddef>
#include <string>
#include <utility>
#include <type_traits>

OCTK_BEGIN_NAMESPACE

namespace type_traits
{
/***********************************************************************************************************************
 * like cxx17 std::void_t
 *
 * Ignores the type of any its arguments and returns `void`.
 * In general, this metafunction allows you to create a general case that maps to `void` while allowing
 * specializations that map to specific types.
 *
 * NOTE: `octk::type_traits::void_t` does not use the standard-specified implementation so that it can remain
 * compatible with gcc < 5.1.
 * This can introduce slightly different behavior, such as when ordering partial specializations.
***********************************************************************************************************************/
namespace detail
{
template <typename... Ts> struct Void
{
    using type = void;
};
} // namespace detail
template <typename... Ts> using void_t = typename detail::Void<Ts...>::type;


} // namespace type_traits
template <typename... Ts> using VoidType = type_traits::void_t<Ts...>; //TODO::DEL

/***********************************************************************************************************************
  * like cxx17 std::conjunction
***********************************************************************************************************************/
template <typename...> struct Conjunction : std::true_type
{
};
template <typename Arg> struct Conjunction<Arg> : Arg
{
};
template <typename Arg, typename... Args>
struct Conjunction<Arg, Args...> : std::conditional<!bool(Arg::value), Arg, Conjunction<Args...>>::type
{
};


/***********************************************************************************************************************
  * like cxx17 std::invoke_result
***********************************************************************************************************************/
#if OCTK_CC_CPP17_OR_GREATER
template <typename F, typename... Args> using InvokeResult = std::invoke_result<F, Args...>;
#else
namespace detail
{
template <typename F, typename... Args> struct InvokeResultImpl
{
    using type = typename std::result_of<F(Args...)>::type;
};
template <typename F, typename... Args> using InvokeResultType = typename InvokeResultImpl<F, Args...>::type;
} // namespace detail
template <typename F, typename... Args> struct InvokeResult
{
    using type = detail::InvokeResultType<F, Args...>;
};
#endif

/***********************************************************************************************************************
  * like cxx17 std::conjunction
***********************************************************************************************************************/
template <typename F>
struct ReturnsVoid
    : std::conditional<std::is_same<typename InvokeResult<F>::type, void>::value, std::true_type, std::false_type>::type
{
};

/***********************************************************************************************************************
  * Determines if the given class has zero-argument .data() and .size() methods
  * whose return values are convertible to T* and size_t, respectively.
***********************************************************************************************************************/
template <typename DS, typename T> class HasDataAndSize
{
private:
    template <typename C,
              typename std::enable_if<std::is_convertible<decltype(std::declval<C>().data()), T *>::value &&
                                      std::is_convertible<decltype(std::declval<C>().size()), std::size_t>::value>::type
                  * = nullptr>
    static int Test(int);

    template <typename> static char Test(...);

public:
    static constexpr bool value = std::is_same<decltype(Test<DS>(0)), int>::value;
};

namespace test_has_data_and_size
{

template <typename DR, typename SR> struct Test1
{
    DR data();
    SR size();
};

static_assert(HasDataAndSize<Test1<int *, int>, int>::value, "");
static_assert(HasDataAndSize<Test1<int *, int>, const int>::value, "");
static_assert(HasDataAndSize<Test1<const int *, int>, const int>::value, "");
static_assert(!HasDataAndSize<Test1<const int *, int>, int>::value, "implicit cast of const int* to int*");
static_assert(!HasDataAndSize<Test1<char *, size_t>, int>::value, "implicit cast of char* to int*");

struct Test2
{
    int *data;
    size_t size;
};
static_assert(!HasDataAndSize<Test2, int>::value, ".data and .size aren't functions");

struct Test3
{
    int *data();
};

static_assert(!HasDataAndSize<Test3, int>::value, ".size() is missing");

class Test4
{
    int *data();
    size_t size();
};

static_assert(!HasDataAndSize<Test4, int>::value, ".data() and .size() are private");
} // namespace test_has_data_and_size

namespace detail
{

// Determines if the given type is an enum that converts implicitly to an integral type.
template <typename T> struct IsIntEnum
{
private:
    // This overload is used if the type is an enum, and unary plus
    // compiles and turns it into an integral type.
    template <typename X,
              typename std::enable_if<std::is_enum<X>::value &&
                                      std::is_integral<decltype(+std::declval<X>())>::value>::type * = nullptr>
    static int Test(int);

    // Otherwise, this overload is used.
    template <typename> static char Test(...);

public:
    static constexpr bool value = std::is_same<decltype(Test<typename std::remove_reference<T>::type>(0)), int>::value;
};
} // namespace detail
// Determines if the given type is integral, or an enum that converts implicitly to an integral type.
template <typename T> struct IsIntLike
{
private:
    using X = typename std::remove_reference<T>::type;

public:
    static constexpr bool value = std::is_integral<X>::value || detail::IsIntEnum<X>::value;
};
namespace test_enum_intlike
{

enum E1
{
    e1
};
enum
{
    e2
};
enum class E3
{
    e3
};
struct S
{
};

static_assert(detail::IsIntEnum<E1>::value, "");
static_assert(detail::IsIntEnum<decltype(e2)>::value, "");
static_assert(!detail::IsIntEnum<E3>::value, "");
static_assert(!detail::IsIntEnum<int>::value, "");
static_assert(!detail::IsIntEnum<float>::value, "");
static_assert(!detail::IsIntEnum<S>::value, "");

static_assert(IsIntLike<E1>::value, "");
static_assert(IsIntLike<decltype(e2)>::value, "");
static_assert(!IsIntLike<E3>::value, "");
static_assert(IsIntLike<int>::value, "");
static_assert(!IsIntLike<float>::value, "");
static_assert(!IsIntLike<S>::value, "");
} // namespace test_enum_intlike

OCTK_END_NAMESPACE

#endif // _OCTK_TYPE_TRAITS_H_
