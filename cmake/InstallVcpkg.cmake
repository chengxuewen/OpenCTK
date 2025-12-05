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


function(octk_vcpkg_install)
	if(EXISTS "${OCTKVcpkg_EXECUTABLE}" AND EXISTS "${OCTKVcpkgTools_EXECUTABLE}")
		set(OCTKVcpkg_FOUND ON)
		return()
	endif()

    set(OCTKVcpkg_NAME "vcpkg")
    set(OCTKVcpkg_ROOT_DIR "${OCTK_TOP_LEVEL_SOURCE_DIR}/vcpkg" CACHE INTERNAL "" FORCE)
    set(OCTKVcpkg_INSTALL_DIR "${OCTKVcpkg_ROOT_DIR}/installed" CACHE INTERNAL "" FORCE)
    set(OCTKVcpkgTools_ROOT_DIR "${OCTK_TOP_LEVEL_SOURCE_DIR}/vcpkg-tools" CACHE INTERNAL "" FORCE)
    set(OCTKVcpkgTools_INSTALL_DIR "${OCTKVcpkgTools_ROOT_DIR}/installed" CACHE INTERNAL "" FORCE)
    find_package(Git REQUIRED)
    if(GIT_EXECUTABLE)
        if(WIN32)
            set(OCTKVcpkg_EXECUTABLE_NAME "vcpkg.exe")
            set(OCTKVcpkg_BOOTSTRAP_NAME "bootstrap-vcpkg.bat")
        else()
            set(OCTKVcpkg_EXECUTABLE_NAME "vcpkg")
            set(OCTKVcpkg_BOOTSTRAP_NAME "./bootstrap-vcpkg.sh")
        endif()
        if(NOT EXISTS "${OCTKVcpkg_ROOT_DIR}/${OCTKVcpkg_BOOTSTRAP_NAME}")
            if(EXISTS "${OCTKVcpkg_ROOT_DIR}")
                execute_process(
                    COMMAND ${CMAKE_COMMAND} -E remove_directory "${OCTKVcpkg_ROOT_DIR}"
                    COMMAND ${CMAKE_COMMAND} -E echo "rmdir ${OCTKVcpkg_ROOT_DIR}"
                    WORKING_DIRECTORY "${OCTK_TOP_LEVEL_SOURCE_DIR}"
                    RESULT_VARIABLE RMDIR_RESULT)
                if(NOT (RMDIR_RESULT MATCHES 0))
                    message(FATAL_ERROR "${OCTKVcpkg_ROOT_DIR} dir remove failed.")
                endif()
            endif()
            message(STATUS "Start clone ${OCTKVcpkg_NAME} in ${OCTK_TOP_LEVEL_SOURCE_DIR}.")
            execute_process(
                COMMAND "${GIT_EXECUTABLE}" clone https://github.com/microsoft/vcpkg.git --depth 1
                WORKING_DIRECTORY "${OCTK_TOP_LEVEL_SOURCE_DIR}"
                RESULT_VARIABLE CLONE_RESULT
                COMMAND_ECHO STDOUT)
            if(NOT (CLONE_RESULT MATCHES 0))
                message(FATAL_ERROR "${OCTKVcpkg_NAME} clone failed.")
            endif()
        endif()
        if(NOT EXISTS "${OCTKVcpkg_ROOT_DIR}/${OCTKVcpkg_EXECUTABLE_NAME}")
            execute_process(
                COMMAND "${OCTKVcpkg_BOOTSTRAP_NAME}"
                WORKING_DIRECTORY "${OCTKVcpkg_ROOT_DIR}"
                RESULT_VARIABLE INIT_RESULT
                COMMAND_ECHO STDOUT)
            if(NOT (INIT_RESULT MATCHES 0))
                message(FATAL_ERROR "${OCTKVcpkg_NAME} init failed.")
            endif()
        endif()
        set(OCTKVcpkg_EXECUTABLE "${OCTKVcpkg_ROOT_DIR}/${OCTKVcpkg_EXECUTABLE_NAME}" CACHE INTERNAL "" FORCE)
        if(NOT EXISTS "${OCTKVcpkgTools_ROOT_DIR}/.git")
            execute_process(
                COMMAND ${CMAKE_COMMAND} -E copy_directory "${OCTKVcpkg_ROOT_DIR}" "${OCTKVcpkgTools_ROOT_DIR}"
                WORKING_DIRECTORY "${OCTK_TOP_LEVEL_SOURCE_DIR}"
                RESULT_VARIABLE COPY_RESULT
                COMMAND_ECHO STDOUT)
            if(NOT (COPY_RESULT MATCHES 0))
                message(FATAL_ERROR "${OCTKVcpkg_NAME} copy to vcpkg-tools failed.")
            endif()
        endif()
        set(OCTKVcpkgTools_EXECUTABLE "${OCTKVcpkgTools_ROOT_DIR}/${OCTKVcpkg_EXECUTABLE_NAME}" CACHE INTERNAL "" FORCE)
    endif()
    set(OCTKVcpkg_FOUND ON)
endfunction()


function(octk_vcpkg_install_package NAME)
    octk_parse_all_arguments(arg
        "octk_vcpkg_install_package"
        "NOT_IMPORT;TOOLS;DYNAMIC"
        "TARGET;PREFIX;OUTPUT_DIR;PACK_NAME"
        "COMPONENTS;IMPORTED_TARGETS" ${ARGN})

    if("X${arg_TARGET}" STREQUAL "X")
        set(arg_TARGET ${NAME})
    endif()
    if("X${arg_PREFIX}" STREQUAL "X")
        string(REGEX REPLACE "[^a-zA-Z0-9]" "" arg_PREFIX "${arg_TARGET}")
    endif()
    if("X${arg_OUTPUT_DIR}" STREQUAL "X")
        set(arg_OUTPUT_DIR "${PROJECT_BINARY_DIR}/3rdparty/vcpkg")
    endif()
    if("X${arg_PACK_NAME}" STREQUAL "X")
        set(arg_PACK_NAME ${NAME})
    endif()
    if(TARGET ${arg_TARGET})
        set(${arg_PREFIX}_FOUND ON)
        return()
    endif()
    if(${arg_TOOLS})
        set(Vcpkg_EXECUTABLE ${OCTKVcpkgTools_EXECUTABLE})
        set(Vcpkg_ROOT_DIR ${OCTKVcpkgTools_ROOT_DIR})
    else()
        set(Vcpkg_EXECUTABLE ${OCTKVcpkg_EXECUTABLE})
        set(Vcpkg_ROOT_DIR ${OCTKVcpkg_ROOT_DIR})
    endif()
    if(WIN32)
        if(${arg_DYNAMIC})
            set(${arg_PREFIX}_VCPKG_TRIPLET ${OCTK_VCPKG_TRIPLET})
        else()
            set(${arg_PREFIX}_VCPKG_TRIPLET ${OCTK_VCPKG_TRIPLET}-static-md)
        endif()
    else()
        if(${arg_DYNAMIC})
            set(${arg_PREFIX}_VCPKG_TRIPLET ${OCTK_VCPKG_TRIPLET}-dynamic)
        else()
            set(${arg_PREFIX}_VCPKG_TRIPLET ${OCTK_VCPKG_TRIPLET})
        endif()
    endif()
    set(${arg_PREFIX}_NAME "${arg_PACK_NAME}" CACHE INTERNAL "" FORCE)
    set(${arg_PREFIX}_ROOT_DIR "${arg_OUTPUT_DIR}/${arg_PACK_NAME}"  CACHE INTERNAL "" FORCE)
    set(${arg_PREFIX}_PACKAGE_NAME "${arg_PACK_NAME}-${${arg_PREFIX}_VCPKG_TRIPLET}.7z"  CACHE INTERNAL "" FORCE)
    set(${arg_PREFIX}_PACKAGE_PATH "${OCTK_3RDPARTY_PACKAGES_DIR}/${${arg_PREFIX}_PACKAGE_NAME}"  CACHE INTERNAL "" FORCE)
    set(${arg_PREFIX}_INSTALL_DIR "${${arg_PREFIX}_ROOT_DIR}/installed/${${arg_PREFIX}_VCPKG_TRIPLET}" CACHE INTERNAL "" FORCE)
	set(${arg_PREFIX}_VCPKG_TOOLCHAIN_FILE "${${arg_PREFIX}_ROOT_DIR}/scripts/buildsystems/vcpkg.cmake" CACHE INTERNAL "" FORCE)
    if(NOT EXISTS "${${arg_PREFIX}_INSTALL_DIR}/include")
        if(EXISTS "${${arg_PREFIX}_PACKAGE_PATH}")
            message(STATUS "${${arg_PREFIX}_PACKAGE_NAME} exist, start unpack...")
            if(NOT EXISTS "${arg_OUTPUT_DIR}")
                execute_process(
                    COMMAND ${CMAKE_COMMAND} -E make_directory "${arg_OUTPUT_DIR}"
                    WORKING_DIRECTORY "${Vcpkg_ROOT_DIR}"
                    RESULT_VARIABLE MKDIR_RESULT)
                if(NOT MKDIR_RESULT MATCHES 0)
                    message(FATAL_ERROR "${arg_OUTPUT_DIR} mkdir failed.")
                endif()
            endif()
            execute_process(
                COMMAND ${CMAKE_COMMAND} -E tar xzvf "${${arg_PREFIX}_PACKAGE_PATH}"
                WORKING_DIRECTORY "${arg_OUTPUT_DIR}"
                RESULT_VARIABLE UNPACK_RESULT
                COMMAND_ECHO STDOUT)
            if(NOT (UNPACK_RESULT MATCHES 0))
                message(FATAL_ERROR "${${arg_PREFIX}_NAME} unpack failed.")
            endif()
        else()
            octk_vcpkg_install()
            unset(${arg_PREFIX}_COMPONENTS_CONFIG)
            if("X${arg_COMPONENTS}" STREQUAL "X")
                set(${arg_PREFIX}_COMPONENTS_CONFIG "")
            else()
                set(${arg_PREFIX}_COMPONENTS_CONFIG "[")
                foreach(component IN LISTS arg_COMPONENTS)
                    if(NOT "${${arg_PREFIX}_COMPONENTS_CONFIG}" STREQUAL "[")
                        set(${arg_PREFIX}_COMPONENTS_CONFIG "${${arg_PREFIX}_COMPONENTS_CONFIG},")
                    endif()
                    set(${arg_PREFIX}_COMPONENTS_CONFIG "${${arg_PREFIX}_COMPONENTS_CONFIG}${component}")
                endforeach()
                set(${arg_PREFIX}_COMPONENTS_CONFIG "${${arg_PREFIX}_COMPONENTS_CONFIG}]")
            endif()
            set(${arg_PREFIX}_VCPKG_NAME ${NAME}${${arg_PREFIX}_COMPONENTS_CONFIG}:${${arg_PREFIX}_VCPKG_TRIPLET})
            execute_process(
                COMMAND ${Vcpkg_EXECUTABLE} list ${${arg_PREFIX}_VCPKG_NAME}
                WORKING_DIRECTORY "${Vcpkg_ROOT_DIR}"
                OUTPUT_VARIABLE FIND_OUTPUT
                RESULT_VARIABLE FIND_RESULT)
            string(FIND "${FIND_OUTPUT}" "${${arg_PREFIX}_VCPKG_NAME} " FOUND_POSITION)
            if(FOUND_POSITION EQUAL -1)
                message(STATUS "${${arg_PREFIX}_NAME} not installed, start install...")
                set(${arg_PREFIX}_VCPKG_CONFIGS ${NAME}${${arg_PREFIX}_COMPONENTS_CONFIG}:${${arg_PREFIX}_VCPKG_TRIPLET})
                message(STATUS "${${arg_PREFIX}_NAME} vcpkg install configs: ${${arg_PREFIX}_VCPKG_CONFIGS}")
                execute_process(
                    COMMAND "${Vcpkg_EXECUTABLE}" install ${${arg_PREFIX}_VCPKG_CONFIGS} --recurse
                    WORKING_DIRECTORY "${Vcpkg_ROOT_DIR}"
                    RESULT_VARIABLE INSTALL_RESULT
                    COMMAND_ECHO STDOUT)
                if(NOT (INSTALL_RESULT MATCHES 0))
                    message(FATAL_ERROR "${${arg_PREFIX}_NAME} install failed.")
                endif()
            endif()

            message(STATUS "${${arg_PREFIX}_NAME} not exported, start export...")
            execute_process(
                COMMAND "${Vcpkg_EXECUTABLE}" export ${NAME}:${${arg_PREFIX}_VCPKG_TRIPLET}
                --raw --output=${${arg_PREFIX}_NAME} --output-dir=${arg_OUTPUT_DIR}
                WORKING_DIRECTORY "${Vcpkg_ROOT_DIR}"
                RESULT_VARIABLE EXPORT_RESULT
                COMMAND_ECHO STDOUT)
            if(NOT (EXPORT_RESULT MATCHES 0))
                message(FATAL_ERROR "${${arg_PREFIX}_NAME} export failed.")
            endif()
        endif()
    endif()

    if(NOT "X${OCTK_3RDPARTY_PACKAGES_DIR}" STREQUAL "X")
        if(NOT EXISTS "${${arg_PREFIX}_PACKAGE_PATH}")
            message(STATUS "${${arg_PREFIX}_PACKAGE_NAME} not exist, start pack...")
            execute_process(
                COMMAND ${CMAKE_COMMAND} -E tar cvf "${${arg_PREFIX}_PACKAGE_PATH}" --format=7zip "${${arg_PREFIX}_NAME}"
                WORKING_DIRECTORY "${arg_OUTPUT_DIR}"
                RESULT_VARIABLE PACK_RESULT
                COMMAND_ECHO STDOUT)
            if(NOT (PACK_RESULT MATCHES 0))
                message(FATAL_ERROR "${${arg_PREFIX}_NAME} pack failed.")
            endif()
        endif()
    endif()

    add_library(${arg_TARGET} INTERFACE IMPORTED GLOBAL)
    #	message(${arg_PREFIX}_NAME=${${arg_PREFIX}_NAME})
    #	message(arg_NOT_IMPORT=${arg_NOT_IMPORT})
    #	message(arg_PREFIX=${arg_PREFIX})
    if (NOT arg_NOT_IMPORT)
        message(STATUS "Start find and import ${${arg_PREFIX}_NAME}...")
        if(EXISTS "${${arg_PREFIX}_INSTALL_DIR}/share/${NAME}/Find${NAME}.cmake" OR
                EXISTS "${${arg_PREFIX}_INSTALL_DIR}/share/${NAME}/${NAME}Targets.cmake")
            set(CMAKE_MODULE_PATH_CACHE ${CMAKE_MODULE_PATH})
            set(CMAKE_MODULE_PATH "${${arg_PREFIX}_INSTALL_DIR}/share/${NAME}")
            set(${NAME}_DIR "${${arg_PREFIX}_INSTALL_DIR}/share/${NAME}")
            find_package(${NAME} REQUIRED)
            if(TARGET "${NAME}::${NAME}")
                target_link_libraries(${arg_TARGET} INTERFACE ${NAME}::${NAME})
            endif()
            target_link_libraries(${arg_TARGET} INTERFACE ${arg_IMPORTED_TARGETS})
            target_include_directories(${arg_TARGET} INTERFACE "${${arg_PREFIX}_INSTALL_DIR}/include")
            set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH_CACHE})
        else()
            if(CMAKE_BUILD_TYPE STREQUAL "Debug")
                set(${arg_PREFIX}_PKGCONFIG_DIR "${${arg_PREFIX}_INSTALL_DIR}/debug/lib")
            else()
                set(${arg_PREFIX}_PKGCONFIG_DIR "${${arg_PREFIX}_INSTALL_DIR}/lib")
            endif()
            if("X${arg_COMPONENTS}" STREQUAL "X")
                octk_pkgconf_check_modules(${NAME} REQUIRED
                    PATH "${${arg_PREFIX}_PKGCONFIG_DIR}/pkgconfig")
            else()
                octk_pkgconf_check_modules(${NAME} REQUIRED
                    PATH "${${arg_PREFIX}_PKGCONFIG_DIR}/pkgconfig"
                    IMPORTED_TARGET
                    ${arg_COMPONENTS})
            endif()
            target_link_libraries(${arg_TARGET} INTERFACE PkgConfig::${NAME})
        endif()
    endif()
    set(${arg_PREFIX}_FOUND ON PARENT_SCOPE)
endfunction()

