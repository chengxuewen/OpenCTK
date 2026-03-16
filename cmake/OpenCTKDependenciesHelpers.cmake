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

function(octk_copy_external_dependencies)
    set(multiValueArgs DEPENDENCIES)
    set(oneValueArgs TARGET DESTINATION)
    set(options PRE_BUILD PRE_LINK POST_BUILD VERBOSE)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" "${ARGN}")

    set(HAPPENS)
    if(ARG_PRE_BUILD)
        list(APPEND HAPPENS PRE_BUILD)
    endif()
    if(ARG_PRE_LINK)
        list(APPEND HAPPENS PRE_LINK)
    endif()
    if(ARG_POST_BUILD)
        list(APPEND HAPPENS POST_BUILD)
    endif()

    if(ARG_VERBOSE)
        message(STATUS "----VERBOSE---- octk_copy_external_dependencies")
        message(STATUS DEPENDENCIES=${ARG_DEPENDENCIES})
        message(STATUS TARGET=${ARG_TARGET})
        message(STATUS DESTINATION=${ARG_DESTINATION})
        message(STATUS PRE_BUILD=${ARG_PRE_BUILD})
        message(STATUS PRE_LINK=${ARG_PRE_LINK})
        message(STATUS POST_BUILD=${ARG_POST_BUILD})
        message(STATUS HAPPENS=${HAPPENS})
    endif()

    foreach(child ${ARG_DEPENDENCIES})
        add_custom_command(
                TARGET ${ARG_TARGET}
                COMMAND ${CMAKE_COMMAND} -E copy_if_different ${child} ${ARG_DESTINATION}
                COMMENT "copy the depends external dependencies libs file ${child} to the ${ARG_DESTINATION} folder"
                ${HAPPENS})
    endforeach()
endfunction(octk_copy_external_dependencies)


function(octk_copy_internal_dependencies)
    set(multiValueArgs SOURCE_TARGETS)
    set(oneValueArgs TARGET DESTINATION)
    set(options PRE_BUILD PRE_LINK POST_BUILD VERBOSE MATCHING)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" "${ARGN}")

    set(HAPPENS)
    if(ARG_PRE_BUILD)
        list(APPEND HAPPENS PRE_BUILD)
    endif()
    if(ARG_PRE_LINK)
        list(APPEND HAPPENS PRE_LINK)
    endif()
    if(ARG_POST_BUILD)
        list(APPEND HAPPENS POST_BUILD)
    endif()

    if(ARG_VERBOSE)
        message(STATUS "----VERBOSE---- octk_copy_internal_dependencies")
        message(STATUS TARGET=${ARG_TARGET})
        message(STATUS SOURCE_TARGETS=${ARG_SOURCE_TARGETS})
        message(STATUS DESTINATION=${ARG_DESTINATION})
        message(STATUS PRE_BUILD=${ARG_PRE_BUILD})
        message(STATUS PRE_LINK=${ARG_PRE_LINK})
        message(STATUS POST_BUILD=${ARG_POST_BUILD})
        message(STATUS MATCHING=${ARG_MATCHING})
        message(STATUS HAPPENS=${HAPPENS})
    endif()

    foreach(child ${ARG_SOURCE_TARGETS})
        if(ARG_MATCHING)
            add_custom_command(
                    TARGET ${ARG_TARGET}
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_LINKER_FILE:${child}>* ${ARG_DESTINATION}
                    COMMENT "copy the depends internal lib ${child} to the ${ARG_DESTINATION} folder"
                    ${HAPPENS})
        else()
            add_custom_command(
                    TARGET ${ARG_TARGET}
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:${child}> ${ARG_DESTINATION}
                    COMMENT "copy the depends internal lib ${child} to the ${ARG_DESTINATION} folder"
                    ${HAPPENS})
        endif()
    endforeach()
endfunction(octk_copy_internal_dependencies)


function(octk_get_dependencies)
    set(oneValueArgs TARGET)
    set(options VERBOSE)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "" "${ARGN}")
    file(GENERATE OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${ARG_TARGET}_OUTPUT_PATH CONTENT $<TARGET_FILE:${ARG_TARGET}>)
    if(EXISTS ${CMAKE_CURRENT_BINARY_DIR}/${ARG_TARGET}_OUTPUT_PATH)
        file(READ ${CMAKE_CURRENT_BINARY_DIR}/${ARG_TARGET}_OUTPUT_PATH OUTPUT_PATH)
        if(EXISTS ${OUTPUT_PATH})
            include(GetPrerequisites)
            get_prerequisites(${OUTPUT_PATH} DEP_FILES 1 1 "" "")
            foreach(DEP_FILE ${DEP_FILES})
                gp_resolve_item("${OUTPUT_PATH}" "${DEP_FILE}" "" "" DEP_RESOLVED_FILE)
                if(ARG_VERBOSE)
                    message(STATUS "Find ${ARG_TARGET} dependency file ${DEP_RESOLVED_FILE}")
                endif()
                list(APPEND DEP_RESOLVED_FILES ${DEP_RESOLVED_FILE})
            endforeach()
            set("${ARG_TARGET}_DEPENDENCY_FILES" ${DEP_RESOLVED_FILES} CACHE INTERNAL "Have ${DEP_RESOLVED_FILES}" FORCE)
        endif()
    endif()
endfunction(octk_get_dependencies)
