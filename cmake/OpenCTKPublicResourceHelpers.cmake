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

function(octk_add_resources target)
    set(options "")
    set(oneValueArgs NAME)
    set(multiValueArgs COMPRESS RESOURCES)
    cmake_parse_arguments(arg "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT arg_NAME)
        set(arg_NAME ${target})
    endif()

    set(RESOURCES_MARK_STAMP "")
    set(RESOURCES_ABSOLUTE_PATHS "")
    foreach(resource ${arg_RESOURCES})
        get_filename_component(resource_absolute_path "${CMAKE_CURRENT_SOURCE_DIR}/${resource}" ABSOLUTE)
        if(NOT EXISTS ${resource_absolute_path})
            message(FATAL_ERROR, "Resource file ${resource} not exist!")
        endif()
        file(MD5 ${resource_absolute_path} resource_file_md5)
        list(APPEND RESOURCES_MARK_STAMP "${resource_file_md5}")
        list(APPEND RESOURCES_ABSOLUTE_PATHS "${resource_absolute_path}")
    endforeach()
    list(APPEND RESOURCES_MARK_STAMP "${RESOURCES_ABSOLUTE_PATHS}")
    if (NOT "${${target}_resources_mark_stamp_cache}" STREQUAL "${RESOURCES_MARK_STAMP}" OR ${OCTK_RCC_EXECUTABLE_CHANGED})
        set(${target}_resources_mark_stamp_cache "${RESOURCES_MARK_STAMP}" CACHE STRING "" FORCE)
        if ("${OCTK_RCC_EXECUTABLE}" STREQUAL "")
            execute_process(
                COMMAND ${OCTKPython_EXECUTABLE} ${OCTK_RCC_PYSCRIPT} -o ${CMAKE_CURRENT_BINARY_DIR} -n ${arg_NAME} -r ${RESOURCES_ABSOLUTE_PATHS}
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                RESULT_VARIABLE EXECUTE_RESULT)
        else()
            execute_process(
                COMMAND ${OCTK_RCC_EXECUTABLE} -o ${CMAKE_CURRENT_BINARY_DIR} -n ${arg_NAME} -r ${RESOURCES_ABSOLUTE_PATHS}
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                RESULT_VARIABLE EXECUTE_RESULT)
        endif()
    endif()

    if(TARGET ${target})
        target_sources(${target} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/octk_rc_${arg_NAME}.cpp")
    endif()
endfunction()
