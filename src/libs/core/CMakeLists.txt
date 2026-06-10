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
if(APPLE)
	find_library(OCTK_FWCoreFoundation CoreFoundation REQUIRED)
	find_library(OCTK_FWFoundation Foundation REQUIRED)
	find_library(OCTK_FWCocoa Cocoa REQUIRED)
	list(APPEND OCTK_LIB_LINK_LIBRARIES
		${OCTK_FWCoreFoundation}
		${OCTK_FWFoundation}
		${OCTK_FWCocoa})
endif()
find_package(Threads REQUIRED)
list(APPEND OCTK_LIB_LINK_LIBRARIES ${CMAKE_THREAD_LIBS_INIT})

octk_find_package(WrapFmt PROVIDED_TARGETS OpenCTKWrapFmt::WrapFmt)
octk_find_package(WrapJson PROVIDED_TARGETS OpenCTKWrapJson::WrapJson)
octk_find_package(WrapCLI11 PROVIDED_TARGETS OpenCTKWrapCLI11::WrapCLI11)
octk_find_package(WrapJwtcpp PROVIDED_TARGETS OpenCTKWrapJwtcpp::WrapJwtcpp)
octk_find_package(WrapSpdlog PROVIDED_TARGETS OpenCTKWrapSpdlog::WrapSpdlog)
octk_find_package(WrapYamlCpp PROVIDED_TARGETS OpenCTKWrapYamlCpp::WrapYamlCpp)
octk_find_package(WrapAnyLite PROVIDED_TARGETS OpenCTKWrapAnyLite::WrapAnyLite)
octk_find_package(WrapVariant PROVIDED_TARGETS OpenCTKWrapVariant::WrapVariant)
octk_find_package(WrapExpected PROVIDED_TARGETS OpenCTKWrapExpected::WrapExpected)
octk_find_package(WrapOptional PROVIDED_TARGETS OpenCTKWrapOptional::WrapOptional)
octk_find_package(WrapFunction2 PROVIDED_TARGETS OpenCTKWrapFunction2::WrapFunction2)
octk_find_package(WrapFilesystem PROVIDED_TARGETS OpenCTKWrapFilesystem::WrapFilesystem)
octk_find_package(WrapStringViewLite PROVIDED_TARGETS OpenCTKWrapStringViewLite::WrapStringViewLite)
octk_find_package(WrapConcurrentQueue PROVIDED_TARGETS OpenCTKWrapConcurrentQueue::WrapConcurrentQueue)
octk_find_package(WrapReaderWriterQueue PROVIDED_TARGETS OpenCTKWrapReaderWriterQueue::WrapReaderWriterQueue)


#-----------------------------------------------------------------------------------------------------------------------
# Add lib target
#-----------------------------------------------------------------------------------------------------------------------
octk_add_library(Core
	LIBRARY_INCLUDE_NAME "openctk/core"
	EXCEPTIONS
	SOURCES
	source/containers/array_view.hpp
	source/containers/concurrent_queue.hpp
	source/containers/flat_set.hpp
	source/containers/inlined_vector.hpp
	source/containers/reader_writer_queue.hpp
	source/containers/vector.hpp
	source/containers/vector_map.hpp
	source/functional/function_view.hpp
	source/functional/invocable.hpp
	source/functional/unique_function.hpp
	source/global/compiler.hpp
	source/global/global.hpp
	source/global/macros.hpp
	source/global/preprocessor.hpp
	source/global/processor.hpp
	source/global/system.hpp
	source/global/types.hpp
	source/io/file_wrapper.cpp
	source/io/file_wrapper.hpp
	source/kernel/application.cpp
	source/kernel/application.hpp
	source/kernel/application_p.hpp
	source/kernel/event.cpp
	source/kernel/event.hpp
	source/kernel/event_loop.cpp
	source/kernel/event_loop.hpp
	source/kernel/event_loop_p.hpp
	source/kernel/object.cpp
	source/kernel/object.hpp
	source/kernel/object_p.hpp
	source/kernel/signals.cpp
	source/kernel/signals.hpp
	source/memory/aligned_malloc.cpp
	source/memory/aligned_malloc.hpp
	source/memory/memory.hpp
	source/memory/nullability.hpp
	source/memory/ref_count.hpp
	source/memory/ref_counted_object.hpp
	source/memory/ref_ptr.hpp
	source/memory/shared_data.hpp
	source/memory/shared_memory.cpp
	source/memory/shared_memory.hpp
	source/memory/shared_pointer.hpp
	source/memory/shared_ref_ptr.hpp
	source/memory/unique_pointer.hpp
	source/memory/zero_memory.cpp
	source/memory/zero_memory.hpp
	source/numerics/bits.hpp
	source/numerics/divide_round.hpp
	source/numerics/numeric.hpp
	source/numerics/safe_compare.hpp
	source/numerics/safe_conversions.hpp
	source/numerics/safe_minmax.hpp
	source/patterns/singleton.hpp
	source/test/file_utils_p.hpp
	source/text/ascii.cpp
	source/text/ascii.hpp
	source/text/base64.cpp
	source/text/base64.hpp
	source/text/bit_buffer.cpp
	source/text/bit_buffer.hpp
	source/text/format.hpp
	source/text/json.hpp
	source/text/jwt.hpp
	source/text/string.cpp
	source/text/string.hpp
	source/text/string_builder.cpp
	source/text/string_builder.hpp
	source/text/string_encode.cpp
	source/text/string_encode.hpp
	source/text/string_to_number.hpp
	source/text/string_utils.cpp
	source/text/string_utils.hpp
	source/text/string_view.hpp
	source/text/yaml.hpp
	source/thread/concurrent.cpp
	source/thread/concurrent.hpp
	source/thread/context_checker.cpp
	source/thread/context_checker.hpp
	source/thread/event_loop_thread.cpp
	source/thread/event_loop_thread.hpp
	#	source/thread/future.cpp
	#	source/thread/future.hpp
	source/thread/mutex.hpp
	source/thread/platform_thread.cpp
	source/thread/platform_thread.hpp
	source/thread/platform_thread_p.hpp
	source/thread/race_checker.cpp
	source/thread/race_checker.hpp
	source/thread/reference_counter.hpp
	source/thread/repeating_task.cpp
	source/thread/repeating_task.hpp
	source/thread/semaphore.hpp
	source/thread/spinlock.hpp
	source/thread/task.cpp
	source/thread/task.hpp
	source/thread/task_queue.cpp
	source/thread/task_queue.hpp
	source/thread/task_queue_factory.cpp
	source/thread/task_queue_factory.hpp
	source/thread/task_queue_thread.cpp
	source/thread/task_queue_thread.hpp
	source/thread/thread_pool.cpp
	source/thread/thread_pool.hpp
	source/thread/thread_pool_p.hpp
	source/time/date_time.cpp
	source/time/date_time.hpp
	source/time/elapsed_timer.cpp
	source/time/elapsed_timer.hpp
	source/tools/algorithm.hpp
	source/tools/any.hpp
	source/tools/argopt.hpp
	source/tools/assert.cpp
	source/tools/assert.hpp
	source/tools/buffer.hpp
	source/tools/checks.hpp
	source/tools/clock.cpp
	source/tools/clock.hpp
	source/tools/enum_flags.hpp
	source/tools/error.cpp
	source/tools/error.hpp
	source/tools/exception.hpp
	source/tools/expected.hpp
	source/tools/fake_clock.cpp
	source/tools/fake_clock.hpp
	source/tools/id_registry.cpp
	source/tools/id_registry.hpp
	source/tools/iterator.hpp
	source/tools/limits.hpp
	source/tools/logging.cpp
	source/tools/logging.hpp
	source/tools/logging_p.hpp
	source/tools/metrics.cpp
	source/tools/metrics.hpp
	source/tools/ntp_time.cpp
	source/tools/ntp_time.hpp
	source/tools/once_flag.cpp
	source/tools/once_flag.hpp
	source/tools/optional.hpp
	source/tools/random.cpp
	source/tools/random.hpp
	source/tools/result.hpp
	source/tools/sanitizer.hpp
	source/tools/scope_guard.hpp
	source/tools/shared_buffer.cpp
	source/tools/shared_buffer.hpp
	source/tools/source_location.hpp
    source/tools/source_sink.hpp
	source/tools/status.cpp
	source/tools/status.hpp
	source/tools/tag_id.hpp
	source/tools/type_info.hpp
	source/tools/type_list.hpp
	source/tools/type_traits.hpp
	source/tools/utility.hpp
	source/tools/variant.hpp
	source/units/data_rate.cpp
	source/units/data_rate.hpp
	source/units/data_size.cpp
	source/units/data_size.hpp
	source/units/frequency.cpp
	source/units/frequency.hpp
	source/units/time_delta.cpp
	source/units/time_delta.hpp
	source/units/timestamp.cpp
	source/units/timestamp.hpp
	source/units/unit_base.hpp
	source/core.cpp
	source/core.hpp
	PRECOMPILED_HEADER
	source/pch.hpp
	EXTERNAL_HEADERS_DIRS
	${OpenCTKWrapFmt_INSTALL_DIR}/include
	${OpenCTKWrapJson_INSTALL_DIR}/include
	${OpenCTKWrapJwtcpp_INSTALL_DIR}/include
	${OpenCTKWrapYamlCpp_INSTALL_DIR}/include
	${OpenCTKWrapVariant_INSTALL_DIR}/include
	${OpenCTKWrapAnyLite_INSTALL_DIR}/include
	${OpenCTKWrapOptional_INSTALL_DIR}/include
	${OpenCTKWrapExpected_INSTALL_DIR}/include
	${OpenCTKWrapFilesystem_INSTALL_DIR}/include
	${OpenCTKWrapStringViewLite_INSTALL_DIR}/include
	${OpenCTKWrapConcurrentQueue_INSTALL_DIR}/include
	${OpenCTKWrapReaderWriterQueue_INSTALL_DIR}/include
	LIBRARIES
	OpenCTKWrapSpdlog::WrapSpdlog
	PUBLIC_LIBRARIES
	OpenCTKWrapFmt::WrapFmt
	OpenCTKWrapJson::WrapJson
	OpenCTKWrapCLI11::WrapCLI11
	OpenCTKWrapJwtcpp::WrapJwtcpp
	OpenCTKWrapYamlCpp::WrapYamlCpp
	OpenCTKWrapVariant::WrapVariant
	OpenCTKWrapAnyLite::WrapAnyLite
	OpenCTKWrapExpected::WrapExpected
	OpenCTKWrapOptional::WrapOptional
	OpenCTKWrapFunction2::WrapFunction2
	OpenCTKWrapFilesystem::WrapFilesystem
	OpenCTKWrapStringViewLite::WrapStringViewLite
	OpenCTKWrapConcurrentQueue::WrapConcurrentQueue
	OpenCTKWrapReaderWriterQueue::WrapReaderWriterQueue
	${OCTK_LIB_LINK_LIBRARIES})
octk_internal_extend_target(Core
	SOURCES
	source/thread/platform_thread_win.cpp
	PUBLIC_DEFINES
	NOMINMAX
	CONDITION OCTK_SYSTEM_WIN)
octk_internal_extend_target(Core
	SOURCES
	source/thread/platform_thread_posix.cpp
	CONDITION NOT OCTK_SYSTEM_WIN)
octk_install_public_wrap_headers(Core
    WRAPS
    OpenCTKWrapFmt::WrapFmt|fmt
    OpenCTKWrapJson::WrapJson|nlohmann
    OpenCTKWrapCLI11::WrapCLI11|CLI
    OpenCTKWrapJwtcpp::WrapJwtcpp|jwt-cpp
    OpenCTKWrapYamlCpp::WrapYamlCpp|yaml-cpp
    OpenCTKWrapVariant::WrapVariant|mpark
    OpenCTKWrapAnyLite::WrapAnyLite|nonstd
    OpenCTKWrapExpected::WrapExpected|tl
    OpenCTKWrapOptional::WrapOptional|tl
    OpenCTKWrapFunction2::WrapFunction2|function2
    OpenCTKWrapFilesystem::WrapFilesystem|ghc
    OpenCTKWrapStringViewLite::WrapStringViewLite|nonstd
    OpenCTKWrapConcurrentQueue::WrapConcurrentQueue|concurrentqueue
    OpenCTKWrapReaderWriterQueue::WrapReaderWriterQueue|readerwriterqueue
)
# Add lib examples and tests
#-----------------------------------------------------------------------------------------------------------------------
octk_add_subdirectory(examples OCTK_BUILD_EXAMPLES)
octk_add_subdirectory(tests OCTK_BUILD_TESTS)
