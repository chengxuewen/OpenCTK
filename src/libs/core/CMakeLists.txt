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

octk_find_package(WrapJson PROVIDED_TARGETS OCTK3rdparty::WrapJson)
octk_find_package(WrapAbseil PROVIDED_TARGETS OCTK3rdparty::WrapAbseil)
octk_find_package(WrapSpdlog PROVIDED_TARGETS OCTK3rdparty::WrapSpdlog)
octk_find_package(WrapVariant PROVIDED_TARGETS OCTK3rdparty::WrapVariant)
octk_find_package(WrapExpected PROVIDED_TARGETS OCTK3rdparty::WrapExpected)
octk_find_package(WrapOptional PROVIDED_TARGETS OCTK3rdparty::WrapOptional)
octk_find_package(WrapFunction2 PROVIDED_TARGETS OCTK3rdparty::WrapFunction2)
octk_find_package(WrapFilesystem PROVIDED_TARGETS OCTK3rdparty::WrapFilesystem)
octk_find_package(WrapStringViewLite PROVIDED_TARGETS OCTK3rdparty::WrapStringViewLite)


#-----------------------------------------------------------------------------------------------------------------------
# Add lib target
#-----------------------------------------------------------------------------------------------------------------------
octk_add_library(core
	EXCEPTIONS
	SOURCES
	source/functional/octk_function_view.hpp
	source/functional/octk_invocable.hpp
	source/functional/octk_unique_function.hpp
	source/global/octk_compiler.hpp
	source/global/octk_global.hpp
	source/global/octk_macros.hpp
	source/global/octk_preprocessor.hpp
	source/global/octk_processor.hpp
	source/global/octk_system.hpp
	source/global/octk_types.hpp
	source/kernel/octk_signals.cpp
	source/kernel/octk_signals.hpp
	source/memory/octk_aligned_malloc.cpp
	source/memory/octk_aligned_malloc.hpp
	source/memory/octk_memory.hpp
	source/memory/octk_nullability.hpp
	source/memory/octk_ref_count.hpp
	source/memory/octk_ref_counted_object.hpp
	source/memory/octk_ref_ptr.hpp
	source/memory/octk_shared_data.hpp
	source/memory/octk_shared_memory.cpp
	source/memory/octk_shared_memory.hpp
	source/memory/octk_shared_ref_ptr.hpp
	source/memory/octk_zero_memory.cpp
	source/memory/octk_zero_memory.hpp
	source/numerics/octk_divide_round.hpp
	source/numerics/octk_numeric.hpp
	source/numerics/octk_safe_compare.hpp
	source/numerics/octk_safe_conversions.hpp
	source/text/octk_ascii.cpp
	source/text/octk_ascii.hpp
	source/text/octk_base64.cpp
	source/text/octk_base64.hpp
	source/text/octk_json.hpp
	source/text/octk_string.cpp
	source/text/octk_string.hpp
	source/text/octk_string_encode.cpp
	source/text/octk_string_encode.hpp
	source/text/octk_string_to_number.hpp
	source/text/octk_string_utils.cpp
	source/text/octk_string_utils.hpp
	source/text/octk_string_view.hpp
	source/thread/octk_concurrent.cpp
	source/thread/octk_concurrent.hpp
#	source/thread/octk_future.cpp
#	source/thread/octk_future.hpp
	source/thread/octk_mutex.hpp
	source/thread/octk_pending_task_safety_flag.cpp
	source/thread/octk_pending_task_safety_flag.hpp
	source/thread/octk_platform_thread.cpp
	source/thread/octk_platform_thread.hpp
	source/thread/octk_platform_thread_p.hpp
	source/thread/octk_reference_counter.hpp
	source/thread/octk_repeating_task.cpp
	source/thread/octk_repeating_task.hpp
	source/thread/octk_semaphore.hpp
	source/thread/octk_sequence_checker.cpp
	source/thread/octk_sequence_checker.hpp
	source/thread/octk_sleep.hpp
	source/thread/octk_socket.cpp
	source/thread/octk_socket.hpp
	source/thread/octk_spinlock.hpp
	source/thread/octk_task_event.cpp
	source/thread/octk_task_event.hpp
	source/thread/octk_task_queue.cpp
	source/thread/octk_task_queue.hpp
	source/thread/octk_task_queue_factory.cpp
	source/thread/octk_task_queue_factory.hpp
#	source/thread/octk_task_queue_stdlib.cpp
	source/thread/octk_task_thread.cpp
	source/thread/octk_task_thread.hpp
	source/thread/octk_task_thread_p.hpp
	source/thread/octk_thread.cpp
	source/thread/octk_thread.hpp
	source/thread/octk_thread_p.hpp
	source/thread/octk_thread_pool.cpp
	source/thread/octk_thread_pool.hpp
	source/thread/octk_thread_pool_p.hpp
	source/thread/octk_time_controller.cpp
	source/thread/octk_time_controller.hpp
	source/thread/octk_yield_policy.cpp
	source/thread/octk_yield_policy.hpp
	source/time/octk_elapsed_timer.cpp
	source/time/octk_elapsed_timer.hpp
	source/tools/octk_algorithm.hpp
	source/tools/octk_array_view.hpp
	source/tools/octk_assert.cpp
	source/tools/octk_assert.hpp
	source/tools/octk_buffer.hpp
	source/tools/octk_checks.hpp
	source/tools/octk_clock.cpp
	source/tools/octk_clock.hpp
	source/tools/octk_crypto_random.cpp
	source/tools/octk_crypto_random.hpp
	source/tools/octk_date_time.cpp
	source/tools/octk_date_time.hpp
	source/tools/octk_error.cpp
	source/tools/octk_error.hpp
	source/tools/octk_exception.hpp
	source/tools/octk_expected.hpp
	source/tools/octk_fake_clock.cpp
	source/tools/octk_fake_clock.hpp
	source/tools/octk_file_utils.cpp
	source/tools/octk_file_utils.hpp
	source/tools/octk_id_registry.cpp
	source/tools/octk_id_registry.hpp
	source/tools/octk_inlined_vector.hpp
	source/tools/octk_iterator.hpp
	source/tools/octk_limits.hpp
	source/tools/octk_logging.cpp
	source/tools/octk_logging.hpp
	source/tools/octk_logging_p.hpp
	source/tools/octk_metrics.cpp
	source/tools/octk_metrics.hpp
	source/tools/octk_move_wrapper.hpp
	source/tools/octk_ntp_time.cpp
	source/tools/octk_ntp_time.hpp
	source/tools/octk_once_flag.cpp
	source/tools/octk_once_flag.hpp
	source/tools/octk_optional.hpp
	source/tools/octk_random.cpp
	source/tools/octk_random.hpp
	source/tools/octk_result.hpp
	source/tools/octk_scope_guard.hpp
	source/tools/octk_shared_buffer.cpp
	source/tools/octk_shared_buffer.hpp
	source/tools/octk_source_location.hpp
	source/tools/octk_tag_id.hpp
	source/tools/octk_type_traits.hpp
	source/tools/octk_utility.hpp
	source/tools/octk_variant.hpp
	source/units/octk_data_rate.cpp
	source/units/octk_data_rate.hpp
	source/units/octk_data_size.cpp
	source/units/octk_data_size.hpp
	source/units/octk_frequency.cpp
	source/units/octk_frequency.hpp
	source/units/octk_time_delta.cpp
	source/units/octk_time_delta.hpp
	source/units/octk_timestamp.cpp
	source/units/octk_timestamp.hpp
	source/units/octk_unit_base.hpp
	source/octk_core.cpp
	source/octk_core.hpp
	PRECOMPILED_HEADER
	source/octk_pch.hpp
	EXTERNAL_HEADERS_DIRS
	${OCTKWrapJson_INSTALL_DIR}/include
#	${OCTKWrapAbseil_INSTALL_DIR}/include
	${OCTKWrapVariant_INSTALL_DIR}/include
	${OCTKWrapOptional_INSTALL_DIR}/include
	${OCTKWrapExpected_INSTALL_DIR}/include
	${OCTKWrapFilesystem_INSTALL_DIR}/include
	${OCTKWrapStringviewLite_INSTALL_DIR}/include
	LIBRARIES
    OCTK3rdparty::WrapSpdlog
	PUBLIC_LIBRARIES
    OCTK3rdparty::WrapJson
#    OCTK3rdparty::WrapAbseil
	OCTK3rdparty::WrapFunction2
	OCTK3rdparty::WrapVariant
	OCTK3rdparty::WrapExpected
	OCTK3rdparty::WrapOptional
	OCTK3rdparty::WrapFilesystem
	OCTK3rdparty::WrapStringViewLite
	${OCTK_LIB_LINK_LIBRARIES})
octk_internal_extend_target(octk_core
	SOURCES
	source/thread/octk_platform_thread_win.cpp
	PUBLIC_DEFINES
	NOMINMAX
	CONDITION OCTK_SYSTEM_WIN)
octk_internal_extend_target(octk_core
	SOURCES
	source/thread/octk_platform_thread_posix.cpp
	CONDITION NOT OCTK_SYSTEM_WIN)
octk_internal_extend_target(octk_core
	SOURCES
	source/thread/octk_task_thread.mm
	source/tools/octk_file_utils.mm
	CONDITION OCTK_SYSTEM_MAC)


#-----------------------------------------------------------------------------------------------------------------------
# Add lib examples and tests
#-----------------------------------------------------------------------------------------------------------------------
octk_add_subdirectory(examples OCTK_BUILD_EXAMPLES)
octk_add_subdirectory(tests OCTK_BUILD_TESTS)