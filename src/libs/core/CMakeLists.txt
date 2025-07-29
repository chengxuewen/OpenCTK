########################################################################################################################
#
# Library: OpenCTK
#
# Copyright (C) 2025~Present ChengXueWen.
#
# License: MIT License
#
# Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
# documentation files (the "Software"), to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and
# to permit persons to whom the Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all copies or substantial portions
# of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
# WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE  AUTHORS
# OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
# OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#
########################################################################################################################

#-----------------------------------------------------------------------------------------------------------------------
# Add lib link libraries
#-----------------------------------------------------------------------------------------------------------------------
if(WIN32)
    list(APPEND OCTK_LIB_LINK_LIBRARIES
        shlwapi.lib     # shell,string
        ws2_32.lib      # network
        Strmiids.lib    # IID_*
        Winmm.lib)      # timeGetTime
    if(NOT MINGW)
        list(APPEND OCTK_LIB_LINK_LIBRARIES dbghelp)
    endif()
elseif(UNIX)
    list(APPEND OCTK_LIB_LINK_LIBRARIES ${CMAKE_DL_LIBS})
endif()
find_package(Threads REQUIRED)
list(APPEND OCTK_LIB_LINK_LIBRARIES ${CMAKE_THREAD_LIBS_INIT})

octk_find_package(WrapJson PROVIDED_TARGETS OCTK3rdparty::WrapJson)
octk_find_package(WrapAbseil PROVIDED_TARGETS OCTK3rdparty::WrapAbseil)
octk_find_package(WrapSpdlog PROVIDED_TARGETS OCTK3rdparty::WrapSpdlog)
octk_find_package(WrapVariant PROVIDED_TARGETS OCTK3rdparty::WrapVariant)
octk_find_package(WrapExpected PROVIDED_TARGETS OCTK3rdparty::WrapExpected)
octk_find_package(WrapStringViewLite PROVIDED_TARGETS OCTK3rdparty::WrapStringViewLite)


#-----------------------------------------------------------------------------------------------------------------------
# Add lib target
#-----------------------------------------------------------------------------------------------------------------------
octk_add_library(core
    EXCEPTIONS
    SOURCES
    source/global/octk_compiler.hpp
    source/global/octk_global.hpp
    source/global/octk_macros.hpp
    source/global/octk_preprocessor.hpp
    source/global/octk_processor.hpp
    source/global/octk_system.hpp
    source/global/octk_types.hpp
    source/memory/octk_memory.hpp
    source/numerics/octk_safe_compare.hpp
    source/text/octk_ascii.cpp
    source/text/octk_ascii.hpp
    source/text/octk_json.hpp
    source/text/octk_string_utils.cpp
    source/text/octk_string_utils.hpp
    source/text/octk_string_view.hpp
    source/thread/octk_thread.cpp
    source/thread/octk_thread.hpp
    source/thread/octk_thread_p.hpp
    source/thread/octk_thread_pool.cpp
    source/thread/octk_thread_pool.hpp
    source/thread/octk_thread_pool_p.hpp
    source/tools/octk_assert.cpp
    source/tools/octk_assert.hpp
    source/tools/octk_checks.hpp
    source/tools/octk_exception.hpp
    source/tools/octk_expected.hpp
    source/tools/octk_limits.hpp
    source/tools/octk_logging.cpp
    source/tools/octk_logging.hpp
    source/tools/octk_logging_p.hpp
    source/tools/octk_mutex.hpp
    source/tools/octk_tag_id.hpp
    source/tools/octk_type_traits.hpp
    source/tools/octk_variant.hpp
    source/octk_core.cpp
    source/octk_core.hpp
    source/octk_core.h
    PRECOMPILED_HEADER
    source/octk_pch.hpp
    EXTERNAL_HEADERS_DIRS
    ${OCTKWrapJson_INSTALL_DIR}/include
    ${OCTKWrapVariant_INSTALL_DIR}/include
    ${OCTKWrapExpected_INSTALL_DIR}/include
    ${OCTKWrapStringViewLite_INSTALL_DIR}/include
    LIBRARIES
    OCTK3rdparty::WrapAbseil
    OCTK3rdparty::WrapSpdlog
    PUBLIC_LIBRARIES
    OCTK3rdparty::WrapJson
    OCTK3rdparty::WrapVariant
    OCTK3rdparty::WrapExpected
    OCTK3rdparty::WrapStringViewLite
    ${OCTK_LIB_LINK_LIBRARIES})
octk_internal_extend_target(octk_core
    PUBLIC_DEFINES
    NOMINMAX
    SOURCES
    source/platform/win/octk_windows_version.cpp
    source/platform/win/octk_windows_version.hpp
    CONDITION OCTK_SYSTEM_WIN)


#-----------------------------------------------------------------------------------------------------------------------
# Add lib examples and tests
#-----------------------------------------------------------------------------------------------------------------------
octk_add_subdirectory(examples OCTK_BUILD_EXAMPLES)
octk_add_subdirectory(tests OCTK_BUILD_TESTS)
