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

# We can't create the same interface imported target multiple times, CMake will complain if we do
# that. This can happen if the find_package call is done in multiple different subdirectories.
if(TARGET OCTK3rdparty::WrapAbseil)
    set(OCTKWrapAbseil_FOUND ON)
    return()
endif()

set(OCTKWrapAbseil_NAME "abseil-cpp-20220623.2")
set(OCTKWrapAbseil_PKG_NAME "${OCTKWrapAbseil_NAME}.tar.gz")
set(OCTKWrapAbseil_DIR_NAME "${OCTKWrapAbseil_NAME}-${OCTK_LOWER_BUILD_TYPE}")
set(OCTKWrapAbseil_URL_PATH "${PROJECT_SOURCE_DIR}/3rdparty/${OCTKWrapAbseil_PKG_NAME}")
set(OCTKWrapAbseil_ROOT_DIR "${PROJECT_BINARY_DIR}/3rdparty/${OCTKWrapAbseil_DIR_NAME}")
set(OCTKWrapAbseil_BUILD_DIR "${OCTKWrapAbseil_ROOT_DIR}/build" CACHE INTERNAL "" FORCE)
set(OCTKWrapAbseil_SOURCE_DIR "${OCTKWrapAbseil_ROOT_DIR}/source" CACHE INTERNAL "" FORCE)
set(OCTKWrapAbseil_INSTALL_DIR "${OCTKWrapAbseil_ROOT_DIR}/install" CACHE INTERNAL "" FORCE)
octk_stamp_file_info(OCTKWrapAbseil OUTPUT_DIR "${OCTKWrapAbseil_ROOT_DIR}")
octk_fetch_3rdparty(OCTKWrapAbseil URL "${OCTKWrapAbseil_URL_PATH}" OUTPUT_NAME "${OCTKWrapAbseil_DIR_NAME}")
if(NOT EXISTS "${OCTKWrapAbseil_STAMP_FILE_PATH}")
    if(NOT EXISTS ${OCTKWrapAbseil_SOURCE_DIR})
        message(FATAL_ERROR "${OCTKWrapAbseil_NAME} FetchContent failed.")
    endif()
    octk_reset_dir(${OCTKWrapAbseil_BUILD_DIR})

    message(STATUS "Configure ${OCTKWrapAbseil_NAME} lib...")
    execute_process(
        COMMAND ${CMAKE_COMMAND}
        -G ${CMAKE_GENERATOR}
        -DBUILD_SHARED_LIBS=OFF
        -DABSL_PROPAGATE_CXX_STD=ON
        -DABSL_MSVC_STATIC_RUNTIME=OFF
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_CONFIGURATION_TYPES=${CMAKE_BUILD_TYPE}
        -DCMAKE_INSTALL_PREFIX=${OCTKWrapAbseil_INSTALL_DIR}
        ${OCTKWrapAbseil_SOURCE_DIR}
        WORKING_DIRECTORY "${OCTKWrapAbseil_BUILD_DIR}"
        RESULT_VARIABLE CONFIGURE_RESULT)
    if(NOT CONFIGURE_RESULT MATCHES 0)
        message(FATAL_ERROR "${OCTKWrapAbseil_NAME} configure failed.")
    endif()
    message(STATUS "${OCTKWrapAbseil_NAME} configure success")

    execute_process(
        COMMAND ${CMAKE_COMMAND} --build ./ --parallel ${OCTK_NUMBER_OF_ASYNC_JOBS} --config
        ${CMAKE_BUILD_TYPE} --target install
        WORKING_DIRECTORY "${OCTKWrapAbseil_BUILD_DIR}"
        RESULT_VARIABLE BUILD_RESULT)
    if(NOT BUILD_RESULT MATCHES 0)
        message(FATAL_ERROR "${OCTKWrapAbseil_NAME} build failed.")
    endif()
    message(STATUS "${OCTKWrapAbseil_NAME} build success")

    execute_process(
        COMMAND ${CMAKE_COMMAND} --install ./
        WORKING_DIRECTORY "${OCTKWrapAbseil_BUILD_DIR}"
        RESULT_VARIABLE INSTALL_RESULT)
    if(NOT INSTALL_RESULT MATCHES 0)
        message(FATAL_ERROR "${OCTKWrapAbseil_NAME} install failed.")
    endif()
    message(STATUS "${OCTKWrapAbseil_NAME} install success")
    octk_make_stamp_file("${OCTKWrapAbseil_STAMP_FILE_PATH}")
endif()
# wrap lib
add_library(OCTK3rdparty::WrapAbseil INTERFACE IMPORTED)
set(absl_DIR "${OCTKWrapAbseil_INSTALL_DIR}/lib/cmake/absl")
find_package(absl PATHS "${OCTKWrapAbseil_INSTALL_DIR}" NO_DEFAULT_PATH REQUIRED)
target_link_libraries(OCTK3rdparty::WrapAbseil INTERFACE
    absl::atomic_hook
    absl::errno_saver
    absl::log_severity
    absl::raw_logging_internal
    absl::spinlock_wait
    absl::config
    absl::dynamic_annotations
    absl::core_headers
    absl::malloc_internal
    absl::base_internal
    absl::base
    absl::throw_delegate
    absl::pretty_function
    absl::endian
    absl::scoped_set_env
    absl::strerror
    absl::fast_type_id
    absl::prefetch
    absl::algorithm
    absl::algorithm_container
    absl::cleanup_internal
    absl::cleanup
    absl::btree
    absl::compressed_tuple
    absl::fixed_array
    absl::inlined_vector_internal
    absl::inlined_vector
    absl::counting_allocator
    absl::flat_hash_map
    absl::flat_hash_set
    absl::node_hash_map
    absl::node_hash_set
    absl::container_memory
    absl::hash_function_defaults
    absl::hash_policy_traits
    absl::hashtablez_sampler
    absl::hashtable_debug
    absl::hashtable_debug_hooks
    absl::node_slot_policy
    absl::raw_hash_map
    absl::container_common
    absl::raw_hash_set
    absl::layout
    absl::stacktrace
    absl::symbolize
    absl::examine_stack
    absl::failure_signal_handler
    absl::debugging_internal
    absl::demangle_internal
    absl::leak_check
    absl::debugging
    absl::flags_path_util
    absl::flags_program_name
    absl::flags_config
    absl::flags_marshalling
    absl::flags_commandlineflag_internal
    absl::flags_commandlineflag
    absl::flags_private_handle_accessor
    absl::flags_reflection
    absl::flags_internal
    absl::flags
    absl::flags_usage_internal
    absl::flags_usage
    absl::flags_parse
    absl::any_invocable
    absl::bind_front
    absl::function_ref
    absl::hash
    absl::city
    absl::low_level_hash
    absl::memory
    absl::type_traits
    absl::meta
    absl::bits
    absl::int128
    absl::numeric
    absl::numeric_representation
    absl::sample_recorder
    absl::exponential_biased
    absl::periodic_sampler
    absl::random_random
    absl::random_bit_gen_ref
    absl::random_internal_mock_helpers
    absl::random_distributions
    absl::random_seed_gen_exception
    absl::random_seed_sequences
    absl::random_internal_traits
    absl::random_internal_distribution_caller
    absl::random_internal_fast_uniform_bits
    absl::random_internal_seed_material
    absl::random_internal_pool_urbg
    absl::random_internal_salted_seed_seq
    absl::random_internal_iostream_state_saver
    absl::random_internal_generate_real
    absl::random_internal_wide_multiply
    absl::random_internal_fastmath
    absl::random_internal_nonsecure_base
    absl::random_internal_pcg_engine
    absl::random_internal_randen_engine
    absl::random_internal_platform
    absl::random_internal_randen
    absl::random_internal_randen_slow
    absl::random_internal_randen_hwaes
    absl::random_internal_randen_hwaes_impl
    absl::random_internal_distribution_test_util
    absl::random_internal_uniform_helper
    absl::status
    absl::statusor
    absl::strings
    absl::strings_internal
    absl::str_format
    absl::str_format_internal
    absl::cord_internal
    absl::cordz_update_tracker
    absl::cordz_functions
    absl::cordz_statistics
    absl::cordz_handle
    absl::cordz_info
    absl::cordz_sample_token
    absl::cordz_update_scope
    absl::cord
    absl::graphcycles_internal
    absl::kernel_timeout_internal
    absl::synchronization
    absl::time
    absl::civil_time
    absl::time_zone
    absl::any
    absl::bad_any_cast
    absl::bad_any_cast_impl
    absl::span
    absl::optional
    absl::bad_optional_access
    absl::bad_variant_access
    absl::variant
    absl::compare
    absl::utility)
target_include_directories(OCTK3rdparty::WrapAbseil INTERFACE "${OCTKWrapAbseil_INSTALL_DIR}/include")
set(OCTKWrapAbseil_FOUND ON)
