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
# Set tests output path
#-----------------------------------------------------------------------------------------------------------------------
set(OCTK_TEST_OUTPUT_DIR ${OCTK_BUILD_DIR}/${OCTK_INSTALL_TESTSDIR})


#-----------------------------------------------------------------------------------------------------------------------
# Add tests link libraries
#-----------------------------------------------------------------------------------------------------------------------
#octk_find_package(WrapAbseil PROVIDED_TARGETS OCTK3rdparty::WrapAbseil)
set(OCTK_TEST_LINK_LIBRARIES OpenCTK::CorePrivate OCTK3rdparty::WrapGTest OCTK3rdparty::WrapBenchmark)


#-----------------------------------------------------------------------------------------------------------------------
# Add tests
#-----------------------------------------------------------------------------------------------------------------------
#octk_add_test(OpenCTKCoreTstAlignedMalloc
#	SOURCES
#	tst_aligned_malloc.cpp
#	INCLUDE_DIRECTORIES
#	LIBRARIES
#	${OCTK_TEST_LINK_LIBRARIES}
#	OUTPUT_DIRECTORY
#	${OCTK_TEST_OUTPUT_DIR})
#octk_add_test(OpenCTKCoreTstArrayView
#	SOURCES
#	tst_array_view.cpp
#	INCLUDE_DIRECTORIES
#	LIBRARIES
#	${OCTK_TEST_LINK_LIBRARIES}
#	OUTPUT_DIRECTORY
#	${OCTK_TEST_OUTPUT_DIR})
#octk_add_test(OpenCTKCoreTstBase64
#	SOURCES
#	tst_base64.cpp
#	INCLUDE_DIRECTORIES
#	LIBRARIES
#	${OCTK_TEST_LINK_LIBRARIES}
#	OUTPUT_DIRECTORY
#	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(OpenCTKCoreTstBitBuffer
	SOURCES
	tst_bit_buffer.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
#octk_add_test(OpenCTKCoreTstBuffer
#	SOURCES
#	tst_buffer.cpp
#	INCLUDE_DIRECTORIES
#	LIBRARIES
#	${OCTK_TEST_LINK_LIBRARIES}
#	OUTPUT_DIRECTORY
#	${OCTK_TEST_OUTPUT_DIR})
#octk_add_test(OpenCTKCoreTstChecks
#	SOURCES
#	tst_checks.cpp
#	INCLUDE_DIRECTORIES
#	LIBRARIES
#	${OCTK_TEST_LINK_LIBRARIES}
#	OUTPUT_DIRECTORY
#	${OCTK_TEST_OUTPUT_DIR})
#octk_add_test(OpenCTKCoreTstClock
#	SOURCES
#	tst_clock.cpp
#	INCLUDE_DIRECTORIES
#	LIBRARIES
#	${OCTK_TEST_LINK_LIBRARIES}
#	OUTPUT_DIRECTORY
#	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(OpenCTKCoreTstConcurrent
	SOURCES
	tst_concurrent.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(OpenCTKCoreTstContextChecker
	SOURCES
	tst_context_checker.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
#octk_add_test(OpenCTKCoreTstCryptoRandom
#	SOURCES
#	tst_crypto_random.cpp
#	INCLUDE_DIRECTORIES
#	LIBRARIES
#	${OCTK_TEST_LINK_LIBRARIES}
#	OUTPUT_DIRECTORY
#	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(OpenCTKCoreTstCtorDtor
	SOURCES
	tst_ctordtor.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
#octk_add_test(OpenCTKCoreTstDataRate
#	SOURCES
#	tst_data_rate.cpp
#	INCLUDE_DIRECTORIES
#	LIBRARIES
#	${OCTK_TEST_LINK_LIBRARIES}
#	OUTPUT_DIRECTORY
#	${OCTK_TEST_OUTPUT_DIR})
#octk_add_test(OpenCTKCoreTstDataSize
#	SOURCES
#	tst_data_size.cpp
#	INCLUDE_DIRECTORIES
#	LIBRARIES
#	${OCTK_TEST_LINK_LIBRARIES}
#	OUTPUT_DIRECTORY
#	${OCTK_TEST_OUTPUT_DIR})
#octk_add_test(OpenCTKCoreTstDateTime
#	SOURCES
#	tst_date_time.cpp
#	INCLUDE_DIRECTORIES
#	LIBRARIES
#	${OCTK_TEST_LINK_LIBRARIES}
#	OUTPUT_DIRECTORY
#	${OCTK_TEST_OUTPUT_DIR})
#octk_add_test(OpenCTKCoreTstDivideRound
#	SOURCES
#	tst_divide_round.cpp
#	INCLUDE_DIRECTORIES
#	LIBRARIES
#	${OCTK_TEST_LINK_LIBRARIES}
#	OUTPUT_DIRECTORY
#	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(OpenCTKCoreTstElapsedTimer
	SOURCES
	tst_elapsed_timer.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(OpenCTKCoreTstEnumFlags
	SOURCES
	tst_enum_flags.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(OpenCTKCoreTstError
	SOURCES
	tst_error.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(OpenCTKCoreTstException
	SOURCES
	tst_exception.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
#octk_add_test(OpenCTKCoreTstFakeClock
#	SOURCES
#	tst_fake_clock.cpp
#	INCLUDE_DIRECTORIES
#	LIBRARIES
#	${OCTK_TEST_LINK_LIBRARIES}
#	OUTPUT_DIRECTORY
#	${OCTK_TEST_OUTPUT_DIR})
#octk_add_test(OpenCTKCoreTstFileUtils
#	SOURCES
#	tst_file_utils.cpp
#	INCLUDE_DIRECTORIES
#	LIBRARIES
#	${OCTK_TEST_LINK_LIBRARIES}
#	OUTPUT_DIRECTORY
#	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(OpenCTKCoreTstFileWrapper
	SOURCES
	tst_file_wrapper.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(OpenCTKCoreTstFileSystem
	SOURCES
	tst_filesystem.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
#octk_add_test(OpenCTKCoreTstFrequency
#	SOURCES
#	tst_frequency.cpp
#	INCLUDE_DIRECTORIES
#	LIBRARIES
#	${OCTK_TEST_LINK_LIBRARIES}
#	OUTPUT_DIRECTORY
#	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(OpenCTKCoreTstFunctionView
	SOURCES
	tst_function_view.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
#octk_add_test(OpenCTKCoreTstFuture
#	SOURCES
#	tst_future.cpp
#	INCLUDE_DIRECTORIES
#	LIBRARIES
#	${OCTK_TEST_LINK_LIBRARIES}
#	OUTPUT_DIRECTORY
#	${OCTK_TEST_OUTPUT_DIR})
#octk_add_test(OpenCTKCoreTstInlinedVector
#	SOURCES
#	${OCTKWrapAbseil_SOURCE_DIR}/absl/container/internal/test_instance_tracker.cc
#	tst_inlined_vector.cpp
#	INCLUDE_DIRECTORIES
#	LIBRARIES
#	OCTK3rdparty::WrapAbseil
#	${OCTK_TEST_LINK_LIBRARIES}
#	OUTPUT_DIRECTORY
#	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(OpenCTKCoreTstMetrics
	SOURCES
	tst_metrics.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(OpenCTKCoreTstMoveWrapper
	SOURCES
	tst_move_wrapper.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(OpenCTKCoreTstMutex
	SOURCES
	tst_mutex.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(OpenCTKCoreTstMutexBenchmark
	SOURCES
	tst_mutex_benchmark.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
#octk_add_test(OpenCTKCoreTstNtpTime
#	SOURCES
#	tst_ntp_time.cpp
#	INCLUDE_DIRECTORIES
#	LIBRARIES
#	${OCTK_TEST_LINK_LIBRARIES}
#	OUTPUT_DIRECTORY
#	${OCTK_TEST_OUTPUT_DIR})
#octk_add_test(OpenCTKCoreTstNumeric
#	SOURCES
#	tst_numeric.cpp
#	INCLUDE_DIRECTORIES
#	LIBRARIES
#	${OCTK_TEST_LINK_LIBRARIES}
#	OUTPUT_DIRECTORY
#	${OCTK_TEST_OUTPUT_DIR})
#octk_add_test(OpenCTKCoreTstPendingTaskSafetyFlag
#	SOURCES
#	tst_pending_task_safety_flag.cpp
#	INCLUDE_DIRECTORIES
#	LIBRARIES
#	${OCTK_TEST_LINK_LIBRARIES}
#	OUTPUT_DIRECTORY
#	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(OpenCTKCoreTstPlatformThread
	SOURCES
	tst_platform_thread.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
#octk_add_test(OpenCTKCoreTstRandom
#	SOURCES
#	tst_random.cpp
#	INCLUDE_DIRECTORIES
#	LIBRARIES
#	${OCTK_TEST_LINK_LIBRARIES}
#	OUTPUT_DIRECTORY
#	${OCTK_TEST_OUTPUT_DIR})
#octk_add_test(OpenCTKCoreTstRefCountedObject
#	SOURCES
#	tst_ref_counted_object.cpp
#	INCLUDE_DIRECTORIES
#	LIBRARIES
#	${OCTK_TEST_LINK_LIBRARIES}
#	OUTPUT_DIRECTORY
#	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(OpenCTKCoreTstRepeatingTask
	SOURCES
	tst_repeating_task.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(OpenCTKCoreTstResult
	SOURCES
	tst_result.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(OpenCTKCoreTstSafeMinMax
	SOURCES
	tst_safe_minmax.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(OpenCTKCoreTstSanitizer
	SOURCES
	tst_sanitizer.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(OpenCTKCoreTstScopeGuard
	SOURCES
	tst_scope_guard.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
#octk_add_test(OpenCTKCoreTstScopedRefPtr
#	SOURCES
#	tst_scoped_refptr.cpp
#	INCLUDE_DIRECTORIES
#	LIBRARIES
#	${OCTK_TEST_LINK_LIBRARIES}
#	OUTPUT_DIRECTORY
#	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(OpenCTKCoreTstSemaphore
	SOURCES
	tst_semaphore.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
#octk_add_test(OpenCTKCoreTstSharedBuffer
#	SOURCES
#	tst_shared_buffer.cpp
#	INCLUDE_DIRECTORIES
#	LIBRARIES
#	${OCTK_TEST_LINK_LIBRARIES}
#	OUTPUT_DIRECTORY
#	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(OpenCTKCoreTstSharedData
	SOURCES
	tst_shared_data.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(OpenCTKCoreTstSharedPointer
	SOURCES
	tst_shared_pointer.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
#octk_add_test(OpenCTKCoreTstSignals
#	SOURCES
#	tst_signals.cpp
#	INCLUDE_DIRECTORIES
#	LIBRARIES
#	${OCTK_TEST_LINK_LIBRARIES}
#	OUTPUT_DIRECTORY
#	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(OpenCTKCoreTstSourceLocation
	SOURCES
	tst_source_location.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(OpenCTKCoreTstStatus
	SOURCES
	tst_status.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(OpenCTKCoreTstStringBuilder
	SOURCES
	tst_string_builder.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
#octk_add_test(OpenCTKCoreTstStringEncode
#	SOURCES
#	tst_string_encode.cpp
#	INCLUDE_DIRECTORIES
#	LIBRARIES
#	${OCTK_TEST_LINK_LIBRARIES}
#	OUTPUT_DIRECTORY
#	${OCTK_TEST_OUTPUT_DIR})
#octk_add_test(OpenCTKCoreTstStringToNumber
#	SOURCES
#	tst_string_to_number.cpp
#	INCLUDE_DIRECTORIES
#	LIBRARIES
#	${OCTK_TEST_LINK_LIBRARIES}
#	OUTPUT_DIRECTORY
#	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(OpenCTKCoreTstStringUtils
	SOURCES
	tst_string_utils.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
#octk_add_test(OpenCTKCoreTstStrongAlias
#	SOURCES
#	tst_strong_alias.cpp
#	INCLUDE_DIRECTORIES
#	LIBRARIES
#	${OCTK_TEST_LINK_LIBRARIES}
#	OUTPUT_DIRECTORY
#	${OCTK_TEST_OUTPUT_DIR})
#octk_add_test(OpenCTKCoreTstTaskEvent
#	SOURCES
#	tst_task_event.cpp
#	INCLUDE_DIRECTORIES
#	LIBRARIES
#	${OCTK_TEST_LINK_LIBRARIES}
#	OUTPUT_DIRECTORY
#	${OCTK_TEST_OUTPUT_DIR})
#octk_add_test(OpenCTKCoreTstTaskQueue
#	SOURCES
#	tst_task_queue.cpp
#	INCLUDE_DIRECTORIES
#	LIBRARIES
#	${OCTK_TEST_LINK_LIBRARIES}
#	OUTPUT_DIRECTORY
#	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(OpenCTKCoreTstTaskQueueThread
	SOURCES
	tst_task_queue_thread.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
#octk_add_test(OpenCTKCoreTstTaskThread
#	SOURCES
#	tst_task_thread.cpp
#	INCLUDE_DIRECTORIES
#	LIBRARIES
#	${OCTK_TEST_LINK_LIBRARIES}
#	OUTPUT_DIRECTORY
#	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(OpenCTKCoreTstThreadPool
	SOURCES
	tst_thread_pool.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
#octk_add_test(OpenCTKCoreTstTimeDelta
#	SOURCES
#	tst_time_delta.cpp
#	INCLUDE_DIRECTORIES
#	LIBRARIES
#	${OCTK_TEST_LINK_LIBRARIES}
#	OUTPUT_DIRECTORY
#	${OCTK_TEST_OUTPUT_DIR})
#octk_add_test(OpenCTKCoreTstTimestamp
#	SOURCES
#	tst_timestamp.cpp
#	INCLUDE_DIRECTORIES
#	LIBRARIES
#	${OCTK_TEST_LINK_LIBRARIES}
#	OUTPUT_DIRECTORY
#	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(OpenCTKCoreTstTypeTraits
	SOURCES
	tst_type_traits.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(OpenCTKCoreTstUniqueFunction
	SOURCES
	tst_unique_function.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
#octk_add_test(OpenCTKCoreTstUnitBase
#	SOURCES
#	tst_unit_base.cpp
#	INCLUDE_DIRECTORIES
#	LIBRARIES
#	${OCTK_TEST_LINK_LIBRARIES}
#	OUTPUT_DIRECTORY
#	${OCTK_TEST_OUTPUT_DIR})
#octk_add_test(OpenCTKCoreTstyield_policy
#	SOURCES
#	tst_yield_policy.cpp
#	INCLUDE_DIRECTORIES
#	LIBRARIES
#	${OCTK_TEST_LINK_LIBRARIES}
#	OUTPUT_DIRECTORY
#	${OCTK_TEST_OUTPUT_DIR})
#octk_add_test(OpenCTKCoreTstZeroMemory
#	SOURCES
#	tst_zero_memory.cpp
#	INCLUDE_DIRECTORIES
#LIBRARIES
#	${OCTK_TEST_LINK_LIBRARIES}
#	OUTPUT_DIRECTORY
#	${OCTK_TEST_OUTPUT_DIR})