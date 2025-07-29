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
#-----------------------------------------------------------------------------------------------------------------------
function(octk_internal_clear_repo_known_libraries)
    set(OCTK_REPO_KNOWN_LIBRARIES "" CACHE INTERNAL "Known current repo OpenCTK libraries" FORCE)
endfunction()


#-----------------------------------------------------------------------------------------------------------------------
#-----------------------------------------------------------------------------------------------------------------------
function(octk_internal_add_repo_known_library)
    if(NOT (${ARGN} IN_LIST OCTK_REPO_KNOWN_LIBRARIES))
        set(OCTK_REPO_KNOWN_LIBRARIES ${OCTK_REPO_KNOWN_LIBRARIES} ${ARGN}
            CACHE INTERNAL "Known current repo OpenCTK libraries" FORCE)
    endif()
endfunction()


#-----------------------------------------------------------------------------------------------------------------------
#-----------------------------------------------------------------------------------------------------------------------
function(octk_internal_get_repo_known_libraries out_var)
    set("${out_var}" "${OCTK_REPO_KNOWN_LIBRARIES}" PARENT_SCOPE)
endfunction()


#-----------------------------------------------------------------------------------------------------------------------
#-----------------------------------------------------------------------------------------------------------------------
macro(octk_internal_set_octk_known_plugins)
    set(OCTK_KNOWN_PLUGINS ${ARGN} CACHE INTERNAL "Known OpenCTK plugins" FORCE)
endmacro()


#-----------------------------------------------------------------------------------------------------------------------
# Gets the list of all known OpenCTK modules both found and that were built as part of the current project.
#-----------------------------------------------------------------------------------------------------------------------
function(octk_internal_get_all_known_modules out_var)
    octk_internal_get_repo_known_libraries(repo_known_modules)
    set(known_modules ${OCTK_ALL_MODULES_FOUND_VIA_FIND_PACKAGE} ${repo_known_modules})
    list(REMOVE_DUPLICATES known_modules)
    set("${out_var}" "${known_modules}" PARENT_SCOPE)
endfunction()


#-----------------------------------------------------------------------------------------------------------------------
#-----------------------------------------------------------------------------------------------------------------------
function(octk_internal_add_octk_repo_known_plugin_types)
    set(OCTK_REPO_KNOWN_PLUGIN_TYPES ${OCTK_REPO_KNOWN_PLUGIN_TYPES} ${ARGN}
        CACHE INTERNAL "Known current repo OpenCTK plug-in types" FORCE)
endfunction()


#-----------------------------------------------------------------------------------------------------------------------
#-----------------------------------------------------------------------------------------------------------------------
macro(octk_internal_append_known_libraries_with_tools library)
    if(NOT ${library} IN_LIST OCTK_KNOWN_LIBRARIES_WITH_TOOLS)
        set(OCTK_KNOWN_LIBRARIES_WITH_TOOLS "${OCTK_KNOWN_LIBRARIES_WITH_TOOLS};${library}"
            CACHE INTERNAL "Known OpenCTK libraries with tools" FORCE)
        set(OCTK_KNOWN_LIBRARIES_${library}_TOOLS ""
            CACHE INTERNAL "Known OpenCTK libraries ${library} tools" FORCE)
    endif()
endmacro()


#-----------------------------------------------------------------------------------------------------------------------
#-----------------------------------------------------------------------------------------------------------------------
macro(octk_internal_append_known_library_tool library tool)
    if(NOT ${tool} IN_LIST OCTK_KNOWN_LIBRARIES_${library}_TOOLS)
        list(APPEND OCTK_KNOWN_LIBRARIES_${library}_TOOLS "${tool}")
        set(OCTK_KNOWN_LIBRARIES_${library}_TOOLS "${OCTK_KNOWN_LIBRARIES_${library}_TOOLS}"
            CACHE INTERNAL "Known OpenCTK library ${library} tools" FORCE)
    endif()
endmacro()


#-----------------------------------------------------------------------------------------------------------------------
#-----------------------------------------------------------------------------------------------------------------------
macro(octk_build_repo_begin)
    octk_build_internals_set_up_private_api()
    octk_internal_generate_binary_strip_wrapper()
endmacro()


#-----------------------------------------------------------------------------------------------------------------------
#-----------------------------------------------------------------------------------------------------------------------
macro(octk_build_repo_end)
    if(NOT OCTK_BUILD_STANDALONE_TESTS)
        # Delayed actions on some of the OpenCTK targets:
        include(OCTKPostProcess)

        # Install the repo-specific cmake find modules.
        octk_path_join(__octk_repo_install_dir ${OCTK_CONFIG_INSTALL_DIR} ${OCTK_CMAKE_INSTALL_NAMESPACE})
        octk_path_join(__octk_repo_build_dir ${OCTK_CONFIG_BUILD_DIR} ${OCTK_CMAKE_INSTALL_NAMESPACE})

        if(NOT PROJECT_NAME STREQUAL "OCTKBase")
            if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
                octk_copy_or_install(DIRECTORY cmake/
                    DESTINATION "${__octk_repo_install_dir}"
                    FILES_MATCHING PATTERN "Find*.cmake")
                if(OCTK_SUPERBUILD AND OCTK_BUILD_INSTALL)
                    file(COPY cmake/
                        DESTINATION "${__octk_repo_build_dir}"
                        FILES_MATCHING PATTERN "Find*.cmake")
                endif()
            endif()
        endif()

        if(NOT OCTK_SUPERBUILD)
            octk_print_feature_summary()
        endif()
    endif()

    octk_build_internals_add_toplevel_targets()

    if(NOT OCTK_SUPERBUILD)
        octk_print_build_instructions()
    endif()
endmacro()
