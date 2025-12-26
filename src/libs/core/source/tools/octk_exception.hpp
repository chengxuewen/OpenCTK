/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2025~Present ChengXueWen.
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

#ifndef _OCTK_EXCEPTION_HPP
#define _OCTK_EXCEPTION_HPP

#include <octk_global.hpp>
#include <octk_logging.hpp>
#include <octk_expected.hpp>
#include <octk_string_view.hpp>

#include <string>
#include <exception>

OCTK_BEGIN_NAMESPACE

namespace detail
{
static inline void noop(void) {}

struct ExceptionWhat final
{
    ExceptionWhat(const char *format, ...)
    {
        va_list args;
        va_start(args, format);
        char string[OCTK_LINE_MAX] = {0};
        std::vsnprintf(string, OCTK_LINE_MAX, format, args);
        va_end(args);
        what = string;
    }
    ExceptionWhat(StringView string)
        : what(string)
    {
    }
    ExceptionWhat(const std::string &string)
        : what(string)
    {
    }
    ExceptionWhat(const std::stringstream &stream)
        : what(stream.str())
    {
    }
    std::string what;
};

static inline const char *getCStrHelper(const char *string) { return string; }
static inline const char *getCStrHelper(const StringView &string) { return string.data(); }
static inline const char *getCStrHelper(const std::string &string) { return string.data(); }
static inline const char *getCStrHelper(const ExceptionWhat &exceptionWhat) { return exceptionWhat.what.c_str(); }
}; // namespace detail

namespace utils
{
template <typename R> Expected<R, std::string> tryCatchCall(const std::function<R()> func)
{
#if OCTK_HAS_EXCEPTIONS
    try
    {
        return func();
    }
    catch (std::exception &e)
    {
        return makeUnexpected(e.what());
    }
#else
    return func();
#endif
}
} // namespace utils

OCTK_END_NAMESPACE

#ifndef OCTK_HAS_EXCEPTIONS
#    define OCTK_TRY                              if (true)
#    define OCTK_CATCH(A)                         else
#    define OCTK_RETHROW                          throw
#    define OCTK_THROW_DELEGATE(Exception, what)  throw Exception(octk::detail::getCStrHelper(what))
#    define OCTK_THROW_NO_MSG_DELEGATE(Exception) throw Exception()
#else
#    define OCTK_TRY                              try
#    define OCTK_CATCH(A)                         catch (A)
#    define OCTK_RETHROW                          octk::detail::noop()
#    define OCTK_THROW_DELEGATE(Exception, what)  OCTK_FATAL("%s", detail::getCStrHelper(what))
#    define OCTK_THROW_NO_MSG_DELEGATE(Exception) OCTK_FATAL("%s", #Exception)
#endif
#define OCTK_THROW(Exception, ...)   OCTK_THROW_DELEGATE(Exception, octk::detail::ExceptionWhat(__VA_ARGS__))
#define OCTK_THROW_NO_MSG(Exception) OCTK_THROW_NO_MSG_DELEGATE(Exception)

#define OCTK_THROW_STD_LOGIC_ERROR(...)      OCTK_THROW(std::logic_error, __VA_ARGS__)
#define OCTK_THROW_STD_INVALID_ARGUMENT(...) OCTK_THROW(std::invalid_argument, __VA_ARGS__)
#define OCTK_THROW_STD_DOMAIN_ERROR(...)     OCTK_THROW(std::domain_error, __VA_ARGS__)
#define OCTK_THROW_STD_LENGTH_ERROR(...)     OCTK_THROW(std::length_error, __VA_ARGS__)
#define OCTK_THROW_STD_OUT_OF_RANGE(...)     OCTK_THROW(std::out_of_range, __VA_ARGS__)
#define OCTK_THROW_STD_RUNTIME_ERROR(...)    OCTK_THROW(std::runtime_error, __VA_ARGS__)
#define OCTK_THROW_STD_RANGE_ERROR(...)      OCTK_THROW(std::range_error, __VA_ARGS__)
#define OCTK_THROW_STD_OVERFLOW_ERROR(...)   OCTK_THROW(std::overflow_error, __VA_ARGS__)
#define OCTK_THROW_STD_UNDERFLOW_ERROR(...)  OCTK_THROW(std::underflow_error, __VA_ARGS__)
#define OCTK_THROW_STD_BAD_FUNCTION_CALL()   OCTK_THROW_NO_MSG(std::bad_function_call)
#define OCTK_THROW_STD_BAD_ALLOC()           OCTK_THROW_NO_MSG(std::bad_alloc)

#endif // _OCTK_EXCEPTION_HPP
