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

include(CheckCXXCompilerFlag)
include(CheckCCompilerFlag)
include(CheckLibraryExists)
include(CheckSymbolExists)
include(CheckIncludeFile)
include(CheckTypeSize)

#-----------------------------------------------------------------------------------------------------------------------
# octk_configure_library_begin finction
#-----------------------------------------------------------------------------------------------------------------------
function(octk_configure_library_begin)
    octk_parse_all_arguments(arg
        "octk_configure_library_begin"
        "NO_MODULE;ONLY_EVALUATE_CONFIG"
        "LIBRARY;PRIVATE_FILE;PUBLIC_FILE"
        "PUBLIC_DEPENDENCIES;PRIVATE_DEPENDENCIES" ${ARGN})

    if(NOT arg_ONLY_EVALUATE_CONFIG)
        if("${arg_LIBRARY}" STREQUAL "" AND (NOT ${arg_NO_MODULE}))
            message(FATAL_ERROR "octk_configure_library_begin needs a LIBRARY name! (or specify NO_MODULE)")
        endif()
        if("${arg_PUBLIC_FILE}" STREQUAL "")
            message(FATAL_ERROR "octk_configure_library_begin needs a PUBLIC_FILE name!")
        endif()
        if("${arg_PRIVATE_FILE}" STREQUAL "")
            message(FATAL_ERROR "octk_configure_library_begin needs a PRIVATE_FILE name!")
        endif()
        set(__octk_configure_only_evaluate_features OFF PARENT_SCOPE)
    else()
        set(__octk_configure_only_evaluate_features ON PARENT_SCOPE)
    endif()

    set(__octk_configure_library "${arg_LIBRARY}" PARENT_SCOPE)

    set(__octk_configure_public_file "${arg_PUBLIC_FILE}" PARENT_SCOPE)
    set(__octk_configure_private_file "${arg_PRIVATE_FILE}" PARENT_SCOPE)

    set(__octk_configure_public_features "" PARENT_SCOPE)
    set(__octk_configure_private_features "" PARENT_SCOPE)
    set(__octk_configure_internal_features "" PARENT_SCOPE)

    set(__octk_configure_public_features_definitions "" PARENT_SCOPE)
    set(__octk_configure_private_features_definitions "" PARENT_SCOPE)

    set(__octk_configure_public_definitions "" PARENT_SCOPE)
    set(__octk_configure_private_definitions "" PARENT_SCOPE)

    set(__octk_configure_reset OFF PARENT_SCOPE)

endfunction()


#-----------------------------------------------------------------------------------------------------------------------
# octk_configure_reset finction
#-----------------------------------------------------------------------------------------------------------------------
function(octk_configure_reset reset)
    octk_evaluate_expression(reset ${reset})
    set(__octk_configure_reset ${reset} PARENT_SCOPE)
endfunction()


#-----------------------------------------------------------------------------------------------------------------------
# octk_configure_definition finction
#-----------------------------------------------------------------------------------------------------------------------
function(octk_configure_definition name)
    octk_normalize_name("${name}" name)
    octk_parse_all_arguments(arg "octk_configure_definition" "PUBLIC;PRIVATE" "VALUE" "" ${ARGN})

    if(arg_PUBLIC)
        string(APPEND __octk_configure_public_definitions "#define ${name} ${arg_VALUE}\n\n")
    elseif(arg_PRIVATE)
        string(APPEND __octk_configure_private_definitions "#define ${name} ${arg_VALUE}\n\n")
    else()
        message(FATAL_ERROR "Must specified PUBLIC or PRIVATE to definition ${name}.")
    endif()

    set(__octk_configure_public_definitions ${__octk_configure_public_definitions} PARENT_SCOPE)
    set(__octk_configure_private_definitions ${__octk_configure_private_definitions} PARENT_SCOPE)
endfunction()


#-----------------------------------------------------------------------------------------------------------------------
# octk_configure_feature finction
#-----------------------------------------------------------------------------------------------------------------------
function(octk_configure_feature feature)
    set(original_name "${feature}")
    octk_normalize_name("${feature}" feature)
    set_property(GLOBAL PROPERTY OCTK_CONFIGURE_FEATURE_ORIGINAL_NAME_${feature} "${original_name}")

    octk_parse_all_arguments(arg
        "octk_configure_feature"
        "PRIVATE;PUBLIC"
        "LABEL;PURPOSE;SECTION"
        "AUTODETECT;CONDITION;ENABLE;DISABLE;EMIT_IF" ${ARGN})

    set(_OCTK_CONFIGURE_FEATURE_DEFINITION_${feature} ${ARGN} PARENT_SCOPE)

    # Check Redefinition of feature
    if("${feature}" IN_LIST __octk_configure_public_features)
        message(FATAL_ERROR "Redefinition of public feature '${feature}'!")
    endif()
    if("${feature}" IN_LIST __octk_configure_private_features)
        message(FATAL_ERROR "Redefinition of private feature '${feature}'!")
    endif()
    if("${feature}" IN_LIST __octk_configure_internal_features)
        message(FATAL_ERROR "Redefinition of feature '${feature}'!")
    endif()

    # Register feature for future use:
    if(arg_PUBLIC)
        list(APPEND __octk_configure_public_features "${feature}")
    endif()
    if(arg_PRIVATE)
        list(APPEND __octk_configure_private_features "${feature}")
    endif()
    if(NOT arg_PUBLIC AND NOT arg_PRIVATE)
        list(APPEND __octk_configure_internal_features "${feature}")
    endif()

    set(__octk_configure_public_features ${__octk_configure_public_features} PARENT_SCOPE)
    set(__octk_configure_private_features ${__octk_configure_private_features} PARENT_SCOPE)
    set(__octk_configure_internal_features ${__octk_configure_internal_features} PARENT_SCOPE)
endfunction()


#-----------------------------------------------------------------------------------------------------------------------
# octk_configure_feature_definition finction
#-----------------------------------------------------------------------------------------------------------------------
function(octk_configure_feature_definition feature name)
    octk_normalize_name("${feature}" feature)
    octk_parse_all_arguments(arg "octk_configure_feature_definition"
        "PUBLIC;PRIVATE;NEGATE"
        "VALUE;PREREQUISITE"
        "" ${ARGN})


    # Store all the define related info in a unique variable key.
    if(arg_PUBLIC)
        set(key_name "_OCTK_CONFIGURE_FEATURE_PUBLIC_DEFINITION_${feature}_${name}")
        set(${key_name} "FEATURE;${feature};NAME;${name};${ARGN}" PARENT_SCOPE)
        list(APPEND __octk_configure_public_features_definitions "${key_name}")
    elseif(arg_PRIVATE)
        set(key_name "_OCTK_CONFIGURE_FEATURE_PRIVATE_DEFINITION_${feature}_${name}")
        set(${key_name} "FEATURE;${feature};NAME;${name};${ARGN}" PARENT_SCOPE)
        list(APPEND __octk_configure_private_features_definitions "${key_name}")
    else()
        message(FATAL_ERROR "Must specified PUBLIC or PRIVATE to feature definition ${feature}.")
    endif()

    # Store the key for later evaluation and subsequent define generation:
    set(__octk_configure_public_features_definitions ${__octk_configure_public_features_definitions} PARENT_SCOPE)
    set(__octk_configure_private_features_definitions ${__octk_configure_private_features_definitions} PARENT_SCOPE)
endfunction()


#-----------------------------------------------------------------------------------------------------------------------
# octk_configure_internal_generate_feature_line finction
#-----------------------------------------------------------------------------------------------------------------------
function(octk_configure_internal_generate_feature_line line feature)
    string(TOUPPER "${OCTK_FEATURE_${feature}}" value)
    if(value STREQUAL "ON" OR value STREQUAL "TRUE")
        set(line "#define OCTK_FEATURE_${feature} 1\n\n" PARENT_SCOPE)
    elseif(value STREQUAL "OFF" OR value STREQUAL "FALSE")
        set(line "#define OCTK_FEATURE_${feature} 0\n\n" PARENT_SCOPE)
    elseif(value STREQUAL "UNSET")
        set(line "/* #define OCTK_FEATURE_${feature} */\n\n" PARENT_SCOPE)
    else()
        message(FATAL_ERROR "${feature} has unexpected value \"${OCTK_FEATURE_${feature}}\"! Valid values are ON/TRUE, OFF/FALSE and UNSET.")
    endif()
endfunction()


#-----------------------------------------------------------------------------------------------------------------------
# octk_configure_internal_feature_write_file finction
#-----------------------------------------------------------------------------------------------------------------------
function(octk_configure_internal_feature_write_file file features extra)
    set(contents "")

    get_filename_component(file_name ${file} NAME_WE)
    octk_normalize_name("${file_name}" file_name)
    string(TOUPPER "${file_name}" file_name)
    get_filename_component(file_ext ${file} LAST_EXT)
    octk_normalize_name("${file_ext}" file_ext)
    string(REPLACE "." "_" file_ext ${file_ext})
    string(TOUPPER "${file_ext}" file_ext)
    string(CONCAT macro_name "_" "${file_name}" "${file_ext}")

    string(APPEND contents "#ifndef ${macro_name}\n")
    string(APPEND contents "#define ${macro_name}\n\n")

    foreach(it ${features})
        octk_configure_internal_generate_feature_line(line "${it}")
        string(APPEND contents "${line}")
    endforeach()

    string(APPEND contents "${extra}\n")

    string(APPEND contents "#endif /* ${macro_name} */\n")

    file(GENERATE OUTPUT "${file}" CONTENT "${contents}")
endfunction()


#-----------------------------------------------------------------------------------------------------------------------
# octk_configure_library_end finction
#-----------------------------------------------------------------------------------------------------------------------
function(octk_configure_library_end)
    set(flags ONLY_EVALUATE_FEATURES)
    set(options OUT_VAR_PREFIX)
    set(multiopts)
    cmake_parse_arguments(arg "${flags}" "${options}" "${multiopts}" ${ARGN})
    set(target ${arg_UNPARSED_ARGUMENTS})

    # The value of OUT_VAR_PREFIX is used as a prefix for output variables that should be
    # set in the parent scope.
    if(NOT arg_OUT_VAR_PREFIX)
        set(arg_OUT_VAR_PREFIX "")
    endif()

    set(all_features
        ${__octk_configure_public_features}
        ${__octk_configure_private_features}
        ${__octk_configure_internal_features})
    list(REMOVE_DUPLICATES all_features)

    foreach(feature ${all_features})
        octk_configure_evaluate_feature(${feature})
    endforeach()

    # Evaluate custom cache assignments.
    foreach(cache_var_name ${__octk_configure_custom_enabled_cache_variables})
        set(${cache_var_name} ON CACHE BOOL "Force enabled by platform requirements." FORCE)
    endforeach()
    foreach(cache_var_name ${__octk_configure_custom_disabled_cache_variables})
        set(${cache_var_name} OFF CACHE BOOL "Force disabled by platform requirements." FORCE)
    endforeach()

    set(enabled_public_features "")
    set(disabled_public_features "")
    set(enabled_private_features "")
    set(disabled_private_features "")

    foreach(feature ${__octk_configure_public_features})
        if(OCTK_FEATURE_${feature})
            list(APPEND enabled_public_features ${feature})
        else()
            list(APPEND disabled_public_features ${feature})
        endif()
    endforeach()

    foreach(feature ${__octk_configure_private_features})
        if(OCTK_FEATURE_${feature})
            list(APPEND enabled_private_features ${feature})
        else()
            list(APPEND disabled_private_features ${feature})
        endif()
    endforeach()

    foreach(key ${__octk_configure_public_features_definitions})
        octk_configure_evaluate_feature_definition(${key} PUBLIC)
        unset(${key} PARENT_SCOPE)
    endforeach()

    foreach(key ${__octk_configure_private_features_definitions})
        octk_configure_evaluate_feature_definition(${key} PRIVATE)
        unset(${key} PARENT_SCOPE)
    endforeach()

    foreach(feature ${all_features})
        unset(_OCTK_FEATURE_DEFINITION_${feature} PARENT_SCOPE)
    endforeach()

    if(NOT arg_ONLY_EVALUATE_FEATURES)
        octk_configure_internal_feature_write_file("${CMAKE_CURRENT_BINARY_DIR}/${__octk_configure_public_file}"
            "${__octk_configure_public_features}" "${__octk_configure_public_definitions}")

        octk_configure_internal_feature_write_file("${CMAKE_CURRENT_BINARY_DIR}/${__octk_configure_private_file}"
            "${__octk_configure_private_features}" "${__octk_configure_private_definitions}")
    endif()

    # Extra header injections which have to have forwarding headers created by octk_install_injections.
    # Skip creating forwarding headers if octk_configure_library_begin was called with NO_MODULE, aka
    # there is no include/<module_name> so there's no place to put the forwarding headers.
    if(__octk_configure_library)
        set(injections "")
        octk_compute_injection_forwarding_header("${__octk_configure_library}"
            SOURCE "${__octk_configure_public_file}" CONFIGURE
            OUT_VAR injections)
        octk_compute_injection_forwarding_header("${__octk_configure_library}"
            SOURCE "${__octk_configure_private_file}" CONFIGURE PRIVATE
            OUT_VAR injections)
        set(${arg_OUT_VAR_PREFIX}extra_library_injections ${injections} PARENT_SCOPE)
    endif()

    if(NOT ("${target}" STREQUAL "NO_MODULE") AND NOT arg_ONLY_EVALUATE_FEATURES)
        get_target_property(target_type "${target}" TYPE)
        if("${target_type}" STREQUAL "INTERFACE_LIBRARY")
            set(property_prefix "INTERFACE_")
        else()
            set(property_prefix "")
            set_property(TARGET "${target}" APPEND PROPERTY EXPORT_PROPERTIES
                "OCTK_ENABLED_PUBLIC_FEATURES;OCTK_DISABLED_PUBLIC_FEATURES;OCTK_ENABLED_PRIVATE_FEATURES;OCTK_DISABLED_PRIVATE_FEATURES")
        endif()
        foreach(visibility public private)
            string(TOUPPER "${visibility}" capital_visibility)
            foreach(state enabled disabled)
                string(TOUPPER "${state}" capitalState)
                set_property(TARGET "${target}" PROPERTY
                    ${property_prefix}OCTK_${capitalState}_${capital_visibility}_FEATURES "${${state}_${visibility}_features}")
            endforeach()
        endforeach()
    endif()

    octk_configure_feature_unset_state_vars()
endfunction()


#-----------------------------------------------------------------------------------------------------------------------
# octk_configure_evaluate_feature finction
#-----------------------------------------------------------------------------------------------------------------------
function(octk_configure_evaluate_feature feature)
    if(NOT DEFINED _OCTK_CONFIGURE_FEATURE_DEFINITION_${feature})
        octk_debug_print_variables(DEDUP MATCH "^OCTK_FEATURE")
        message(FATAL_ERROR "Attempting to evaluate feature ${feature} but its definition is missing.
            Either the feature does not exist or a dependency to the module that defines it is missing")
    endif()

    cmake_parse_arguments(arg
        "PRIVATE;PUBLIC"
        "LABEL;PURPOSE;SECTION;"
        "AUTODETECT;CONDITION;ENABLE;DISABLE;EMIT_IF" ${_OCTK_CONFIGURE_FEATURE_DEFINITION_${feature}})

    if("${arg_ENABLE}" STREQUAL "")
        set(arg_ENABLE OFF)
    endif()

    if("${arg_DISABLE}" STREQUAL "")
        set(arg_DISABLE OFF)
    endif()

    if("${arg_AUTODETECT}" STREQUAL "")
        set(arg_AUTODETECT ON)
    endif()

    if("${arg_CONDITION}" STREQUAL "")
        set(condition ON)
    else()
        octk_evaluate_expression(condition ${arg_CONDITION})
    endif()

    if("${arg_EMIT_IF}" STREQUAL "")
        set(emit_if ON)
    else()
        octk_evaluate_expression(emit_if ${arg_EMIT_IF})
    endif()

    octk_evaluate_expression(disable_result ${arg_DISABLE})
    octk_evaluate_expression(enable_result ${arg_ENABLE})
    octk_evaluate_expression(auto_detect ${arg_AUTODETECT})
    if(${disable_result})
        set(computed OFF)
        set(force ON)
    elseif((${enable_result}) OR (${auto_detect}))
        set(computed ${condition})
        set(force OFF)
    else()
        set(computed ${condition})
        set(force ON)
    endif()

    set(input OFF)
    # If FEATURE_ is not defined trying to use INPUT_ variable to enable/disable feature.
    if(DEFINED "INPUT_OCTK_FEATURE_${feature}")
        set(input ON)

        if("${INPUT_OCTK_FEATURE_${feature}}" MATCHES "^(FALSE|OFF|NO|N|TRUE|YES|ON|Y)$" OR (INPUT_OCTK_FEATURE_${feature} GREATER_EQUAL 0))
            # All good!
        else()
            message(FATAL_ERROR "Sanity check failed: INPUT_OCTK_FEATURE_${feature}${feature} has invalid value \"${INPUT_OCTK_FEATURE_${feature}}\"!")
        endif()

        if(INPUT_OCTK_FEATURE_${feature})
            set(input_result ON)
        else()
            set(input_result OFF)
        endif()
    endif()

    # Warn about a feature which is not emitted, but the user explicitly provided a value for it.
    set(result "${computed}")
    set(use_input OFF)
    if(input)
        if(emit_if)
#            if(NOT condition AND input_result)
#                message(WARNING "Reset OCTK_FEATURE_${feature} value to ${computed}, because it doesn't meet its condition after reconfiguration.")
#            else()
                set(result "${input_result}")
                set(use_input ON)
#            endif()
            set(force ON)
        else()
            message(WARNING "Feature ${feature} is not emitted, but the user explicitly provided a value for it.")
        endif()
    endif()

    if(force)
        unset(OCTK_FEATURE_${feature} CACHE)
        if(use_input)
            set(description "${arg_LABEL}, forced to use input value!")
        else()
            set(description "${arg_LABEL}, forced to use condition value!")
        endif()
        set("OCTK_FEATURE_${feature}" "${result}" CACHE STRING "${description}" FORCE)
        set(_feature_old_force_${feature} ON CACHE INTERNAL "feature ${feature} old force value!" FORCE)
    else()
        if(_feature_old_force_${feature} OR __octk_configure_reset)
            set(_feature_old_force_${feature} OFF CACHE INTERNAL "feature ${feature} old force value!" FORCE)
            unset(OCTK_FEATURE_${feature} CACHE)
        endif()
        set("OCTK_FEATURE_${feature}" "${result}" CACHE BOOL "${arg_LABEL}")
    endif()

    # Store each feature's label for summary info.
    set(OCTK_CONFIGURE_FEATURE_LABEL_${feature} "${arg_LABEL}" CACHE INTERNAL "")
    set(OCTK_CONFIGURE_FEATURE_PURPOSE_${feature} "${arg_PURPOSE}" CACHE INTERNAL "")
    set(OCTK_CONFIGURE_FEATURE_SECTION_${feature} "${arg_SECTION}" CACHE INTERNAL "")
endfunction()


#-----------------------------------------------------------------------------------------------------------------------
# octk_configure_evaluate_feature_definition finction
#-----------------------------------------------------------------------------------------------------------------------
function(octk_configure_evaluate_feature_definition key)
    if(NOT DEFINED ${key})
        octk_debug_print_variables(DEDUP MATCH "^_OCTK_FEATURE_DEFINE_DEFINITION")
        message(FATAL_ERROR "Attempting to evaluate feature define ${key} but its definition is missing. ")
    endif()

    cmake_parse_arguments(arg
        "NEGATE;PUBLIC;PRIVATE"
        "FEATURE;NAME;VALUE;PREREQUISITE" "" ${${key}})

    if(NOT arg_PUBLIC AND NOT arg_PRIVATE)
        message(FATAL_ERROR "Must specified PUBLIC or PRIVATE to evaluate feature definition.")
    endif()

    set(expected ON)
    if(arg_NEGATE)
        set(expected OFF)
    endif()

    set(actual OFF)
    if(OCTK_FEATURE_${arg_FEATURE})
        set(actual ON)
    endif()

    set(msg "")
    if(actual STREQUAL expected)
        set(indent "")
        if(arg_PREREQUISITE)
            string(APPEND msg "#if ${arg_PREREQUISITE}\n")
            set(indent "  ")
        endif()
        if(arg_VALUE)
            string(APPEND msg "${indent}#define ${arg_NAME} ${arg_VALUE}\n")
        else()
            string(APPEND msg "${indent}#define ${arg_NAME}\n")
        endif()
        if(arg_PREREQUISITE)
            string(APPEND msg "#endif\n")
        endif()

        if(arg_PUBLIC)
            string(APPEND __octk_configure_public_definitions "${msg}")
        elseif(arg_PRIVATE)
            string(APPEND __octk_configure_private_definitions "${msg}")
        endif()
    endif()

    set(__octk_configure_public_definitions ${__octk_configure_public_definitions} PARENT_SCOPE)
    set(__octk_configure_private_definitions ${__octk_configure_private_definitions} PARENT_SCOPE)
endfunction()


#-----------------------------------------------------------------------------------------------------------------------
# octk_configure_feature_unset_state_vars finction
#-----------------------------------------------------------------------------------------------------------------------
macro(octk_configure_feature_unset_state_vars)
    unset(__octk_configure_library PARENT_SCOPE)

    unset(__octk_configure_public_file PARENT_SCOPE)
    unset(__octk_configure_private_file PARENT_SCOPE)

    unset(__octk_configure_public_features PARENT_SCOPE)
    unset(__octk_configure_private_features PARENT_SCOPE)
    unset(__octk_configure_internal_features PARENT_SCOPE)

    unset(__octk_configure_public_features_definitions PARENT_SCOPE)
    unset(__octk_configure_private_features_definitions PARENT_SCOPE)

    unset(__octk_configure_public_definitions PARENT_SCOPE)
    unset(__octk_configure_private_definitions PARENT_SCOPE)

    unset(__octk_configure_only_evaluate_features PARENT_SCOPE)
endmacro()


#-----------------------------------------------------------------------------------------------------------------------
# octk_configure_compile_test finction
#-----------------------------------------------------------------------------------------------------------------------
function(octk_configure_compile_test name)
    cmake_parse_arguments(arg "FORCE;CXX_SOURCE" "LABEL"
        "FLAGS;DEFINITIONS;INCLUDES;LINK_OPTIONS;LIBRARIES;CODE" ${ARGN})

    if(${arg_FORCE} OR __octk_configure_reset)
        unset("TEST_${name}" CACHE)
        unset("HAVE_${name}" CACHE)
    endif()

    if(NOT arg_CXX_SOURCE)
        set(arg_CXX_SOURCE OFF)
    endif()

    if(NOT arg_FLAGS)
        set(arg_FLAGS ${CMAKE_C_FLAGS})
    endif()

    if(DEFINED "TEST_${name}")
        return()
    endif()

    foreach(library IN ITEMS ${arg_LIBRARIES})
        if(NOT TARGET "${library}")
            # If the dependency looks like a cmake target, then make this compile test
            # fail instead of cmake abort later via CMAKE_REQUIRED_LIBRARIES.
            string(FIND "${library}" "::" cmake_target_namespace_separator)
            if(NOT cmake_target_namespace_separator EQUAL -1)
                set(HAVE_${name} FALSE)
                break()
            endif()
        endif()
    endforeach()

    if(NOT DEFINED HAVE_${name})
        set(_save_CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS}")
        set(_save_CMAKE_REQUIRED_DEFINITIONS "${CMAKE_REQUIRED_DEFINITIONS}")
        set(_save_CMAKE_REQUIRED_INCLUDES "${CMAKE_REQUIRED_INCLUDES}")
        set(_save_CMAKE_REQUIRED_LINK_OPTIONS "${CMAKE_REQUIRED_LINK_OPTIONS}")
        set(_save_CMAKE_REQUIRED_LIBRARIES "${CMAKE_REQUIRED_LIBRARIES}")
        set(CMAKE_REQUIRED_FLAGS "${arg_FLAGS}")
        set(CMAKE_REQUIRED_DEFINITIONS "${arg_DEFINITIONS}")
        set(CMAKE_REQUIRED_INCLUDES "${arg_INCLUDES}")
        set(CMAKE_REQUIRED_LINK_OPTIONS "${arg_LINK_OPTIONS}")
        set(CMAKE_REQUIRED_LIBRARIES "${arg_LIBRARIES}")
        if(arg_CXX_SOURCE)
            check_cxx_source_compiles("${arg_UNPARSED_ARGUMENTS} ${arg_CODE}" HAVE_${name})
        else()
            check_c_source_compiles("${arg_UNPARSED_ARGUMENTS} ${arg_CODE}" HAVE_${name})
        endif()
        set(CMAKE_REQUIRED_FLAGS "${_save_CMAKE_REQUIRED_FLAGS}")
        set(CMAKE_REQUIRED_DEFINITIONS "${_save_CMAKE_REQUIRED_DEFINITIONS}")
        set(CMAKE_REQUIRED_INCLUDES "${_save_CMAKE_REQUIRED_INCLUDES}")
        set(CMAKE_REQUIRED_LINK_OPTIONS "${_save_CMAKE_REQUIRED_LINK_OPTIONS}")
        set(CMAKE_REQUIRED_LIBRARIES "${_save_CMAKE_REQUIRED_LIBRARIES}")
    endif()

    if(${HAVE_${name}})
        set(TEST_${name} "1" CACHE INTERNAL "${arg_LABEL}" FORCE)
    else()
        set(TEST_${name} "0" CACHE INTERNAL "${arg_LABEL}" FORCE)
    endif()
endfunction()


#-----------------------------------------------------------------------------------------------------------------------
# octk_configure_compile_test_type finction
#-----------------------------------------------------------------------------------------------------------------------
function(octk_configure_compile_test_type name)
    cmake_parse_arguments(arg "FORCE" "TYPE;LABEL"
        "FLAGS;DEFINITIONS;INCLUDES;LINK_OPTIONS;LIBRARIES;INCLUDE_FILES" ${ARGN})

    if(${arg_FORCE} OR __octk_configure_reset)
        unset("TEST_${name}" CACHE)
        unset("HAVE_${name}" CACHE)
    endif()

    if(DEFINED "TEST_${name}")
        return()
    endif()

    if(NOT arg_FLAGS)
        set(arg_FLAGS ${CMAKE_C_FLAGS})
    endif()

    if(NOT DEFINED HAVE_${name})
        set(_save_CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS}")
        set(_save_CMAKE_REQUIRED_DEFINITIONS "${CMAKE_REQUIRED_DEFINITIONS}")
        set(_save_CMAKE_REQUIRED_INCLUDES "${CMAKE_REQUIRED_INCLUDES}")
        set(_save_CMAKE_REQUIRED_LINK_OPTIONS "${CMAKE_REQUIRED_LINK_OPTIONS}")
        set(_save_CMAKE_REQUIRED_LIBRARIES "${CMAKE_REQUIRED_LIBRARIES}")
        set(_save_CMAKE_EXTRA_INCLUDE_FILES "${CMAKE_EXTRA_INCLUDE_FILES}")
        list(APPEND CMAKE_REQUIRED_FLAGS ${arg_FLAGS})
        list(APPEND CMAKE_REQUIRED_DEFINITIONS ${arg_DEFINITIONS})
        list(APPEND CMAKE_REQUIRED_INCLUDES ${arg_INCLUDES})
        list(APPEND CMAKE_REQUIRED_LINK_OPTIONS "${arg_LINK_OPTIONS}")
        list(APPEND CMAKE_REQUIRED_LIBRARIES "${arg_LIBRARIES}")
        list(APPEND CMAKE_EXTRA_INCLUDE_FILES "${arg_INCLUDE_FILES}")
        check_type_size(${arg_TYPE} ${name})
        set(CMAKE_REQUIRED_FLAGS "${_save_CMAKE_REQUIRED_FLAGS}")
        set(CMAKE_REQUIRED_DEFINITIONS "${_save_CMAKE_REQUIRED_DEFINITIONS}")
        set(CMAKE_REQUIRED_INCLUDES "${_save_CMAKE_REQUIRED_INCLUDES}")
        set(CMAKE_REQUIRED_LINK_OPTIONS "${_save_CMAKE_REQUIRED_LINK_OPTIONS}")
        set(CMAKE_REQUIRED_LIBRARIES "${_save_CMAKE_REQUIRED_LIBRARIES}")
        set(CMAKE_EXTRA_INCLUDE_FILES "${_save_CMAKE_EXTRA_INCLUDE_FILES}")
    endif()

    if(${HAVE_${name}})
        set(TEST_${name} "1" CACHE INTERNAL "${arg_LABEL}" FORCE)
        set(SIZEOF_${name} "${${name}}" CACHE INTERNAL "${name} size" FORCE)
    else()
        set(TEST_${name} "0" CACHE INTERNAL "${arg_LABEL}" FORCE)
        set(SIZEOF_${name} "0" CACHE INTERNAL "${name} size" FORCE)
    endif()
endfunction()


#-----------------------------------------------------------------------------------------------------------------------
# octk_configure_compile_test_symbol finction
#-----------------------------------------------------------------------------------------------------------------------
function(octk_configure_compile_test_symbol name)
    cmake_parse_arguments(arg "FORCE" "SYMBOL;LABEL"
        "FLAGS;LINK_OPTIONS;LIBRARIES;INCLUDES;INCLUDE_FILES" ${ARGN})

    if(${arg_FORCE} OR __octk_configure_reset)
        unset("TEST_${name}" CACHE)
        unset("HAVE_${name}" CACHE)
    endif()

    if(DEFINED "TEST_${name}")
        return()
    endif()

    if(NOT arg_FLAGS)
        set(arg_FLAGS ${CMAKE_C_FLAGS})
    endif()

    if(NOT DEFINED HAVE_${name})
        set(_save_CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS}")
        set(_save_CMAKE_REQUIRED_DEFINITIONS "${CMAKE_REQUIRED_DEFINITIONS}")
        set(_save_CMAKE_REQUIRED_INCLUDES "${CMAKE_REQUIRED_INCLUDES}")
        set(_save_CMAKE_REQUIRED_LIBRARIES "${CMAKE_REQUIRED_LIBRARIES}")
        set(_save_CMAKE_REQUIRED_LINK_OPTIONS "${CMAKE_REQUIRED_LINK_OPTIONS}")
        set(CMAKE_REQUIRED_FLAGS ${arg_FLAGS})
        set(CMAKE_REQUIRED_DEFINITIONS ${arg_DEFINITIONS})
        set(CMAKE_REQUIRED_INCLUDES ${arg_INCLUDES})
        set(CMAKE_REQUIRED_LIBRARIES "${arg_LIBRARIES}")
        set(CMAKE_REQUIRED_LINK_OPTIONS "${arg_LINK_OPTIONS}")
        check_symbol_exists(${arg_SYMBOL} ${arg_INCLUDE_FILES} HAVE_${name})
        set(CMAKE_REQUIRED_FLAGS "${_save_CMAKE_REQUIRED_FLAGS}")
        set(CMAKE_REQUIRED_DEFINITIONS "${_save_CMAKE_REQUIRED_DEFINITIONS}")
        set(CMAKE_REQUIRED_INCLUDES "${_save_CMAKE_REQUIRED_INCLUDES}")
        set(CMAKE_REQUIRED_LIBRARIES "${_save_CMAKE_REQUIRED_LIBRARIES}")
        set(CMAKE_REQUIRED_LINK_OPTIONS "${_save_CMAKE_REQUIRED_LINK_OPTIONS}")
    endif()

    if(${HAVE_${name}})
        set(TEST_${name} "1" CACHE INTERNAL "${arg_LABEL}" FORCE)
    else()
        set(TEST_${name} "0" CACHE INTERNAL "${arg_LABEL}" FORCE)
    endif()
endfunction()


#-----------------------------------------------------------------------------------------------------------------------
# octk_configure_compile_test_include finction
#-----------------------------------------------------------------------------------------------------------------------
function(octk_configure_compile_test_include name)
    cmake_parse_arguments(arg "FORCE" "INCLUDE;LABEL"
        "FLAGS;LINK_OPTIONS;DEFINITIONS;LIBRARIES;INCLUDES" ${ARGN})

    if(${arg_FORCE} OR __octk_configure_reset)
        unset("TEST_${name}" CACHE)
        unset("HAVE_${name}" CACHE)
    endif()

    if(DEFINED "TEST_${name}")
        return()
    endif()

    if(NOT arg_FLAGS)
        set(arg_FLAGS ${CMAKE_C_FLAGS})
    endif()

    if(NOT DEFINED HAVE_${name})
        set(_save_CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS}")
        set(_save_CMAKE_REQUIRED_INCLUDES "${CMAKE_REQUIRED_INCLUDES}")
        set(_save_CMAKE_REQUIRED_LIBRARIES "${CMAKE_REQUIRED_LIBRARIES}")
        set(_save_CMAKE_REQUIRED_DEFINITIONS "${CMAKE_REQUIRED_DEFINITIONS}")
        set(_save_CMAKE_REQUIRED_LINK_OPTIONS "${CMAKE_REQUIRED_LINK_OPTIONS}")
        set(CMAKE_REQUIRED_FLAGS ${arg_FLAGS})
        set(CMAKE_REQUIRED_INCLUDES ${arg_INCLUDES})
        set(CMAKE_REQUIRED_LIBRARIES "${arg_LIBRARIES}")
        set(CMAKE_REQUIRED_DEFINITIONS "${arg_DEFINITIONS}")
        set(CMAKE_REQUIRED_LINK_OPTIONS "${arg_LINK_OPTIONS}")
        check_include_file(${arg_INCLUDE} HAVE_${name})
        set(CMAKE_REQUIRED_FLAGS "${_save_CMAKE_REQUIRED_FLAGS}")
        set(CMAKE_REQUIRED_INCLUDES "${_save_CMAKE_REQUIRED_INCLUDES}")
        set(CMAKE_REQUIRED_LIBRARIES "${_save_CMAKE_REQUIRED_LIBRARIES}")
        set(CMAKE_REQUIRED_DEFINITIONS "${_save_CMAKE_REQUIRED_DEFINITIONS}")
        set(CMAKE_REQUIRED_LINK_OPTIONS "${_save_CMAKE_REQUIRED_LINK_OPTIONS}")
    endif()

    if(${HAVE_${name}})
        set(TEST_${name} "1" CACHE INTERNAL "${arg_LABEL}" FORCE)
    else()
        set(TEST_${name} "0" CACHE INTERNAL "${arg_LABEL}" FORCE)
    endif()
endfunction()


#-----------------------------------------------------------------------------------------------------------------------
# octk_configure_compile_test_library finction
#-----------------------------------------------------------------------------------------------------------------------
function(octk_configure_compile_test_library name)
    cmake_parse_arguments(arg "FORCE" "LABEL;LIBRARY;FUNCTION;LOCATION"
        "FLAGS;DEFINITIONS;LIBRARIES" ${ARGN})

    if(${arg_FORCE} OR __octk_configure_reset)
        unset("TEST_${name}" CACHE)
        unset("HAVE_${name}" CACHE)
    endif()

    if(DEFINED "TEST_${name}")
        return()
    endif()

    if(NOT arg_LIBRARY)
        message(FATAL_ERROR, "LIBRARY not set")
    endif()

    if(NOT arg_FUNCTION)
        message(FATAL_ERROR, "FUNCTION not set")
    endif()

    if(NOT arg_LOCATION)
        set(arg_LOCATION "")
    endif()

    if(NOT DEFINED HAVE_${name})
        set(_save_CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS}")
        set(_save_CMAKE_REQUIRED_LIBRARIES "${CMAKE_REQUIRED_LIBRARIES}")
        set(_save_CMAKE_REQUIRED_DEFINITIONS "${CMAKE_REQUIRED_DEFINITIONS}")
        set(_save_CMAKE_REQUIRED_QUIET "${CMAKE_REQUIRED_QUIET}")
        set(CMAKE_REQUIRED_FLAGS ${arg_FLAGS})
        set(CMAKE_REQUIRED_LIBRARIES "${arg_LIBRARIES}")
        set(CMAKE_REQUIRED_DEFINITIONS "${arg_DEFINITIONS}")
        CHECK_LIBRARY_EXISTS(${arg_LIBRARY} ${arg_FUNCTION} "${arg_LOCATION}" HAVE_${name})
        set(CMAKE_REQUIRED_FLAGS "${_save_CMAKE_REQUIRED_FLAGS}")
        set(CMAKE_REQUIRED_LIBRARIES "${_save_CMAKE_REQUIRED_LIBRARIES}")
        set(CMAKE_REQUIRED_DEFINITIONS "${_save_CMAKE_REQUIRED_DEFINITIONS}")
        set(CMAKE_REQUIRED_QUIET "${_save_CMAKE_REQUIRED_QUIET}")
    endif()

    if(${HAVE_${name}})
        set(TEST_${name} "1" CACHE INTERNAL "${arg_LABEL}" FORCE)
    else()
        set(TEST_${name} "0" CACHE INTERNAL "${arg_LABEL}" FORCE)
    endif()
endfunction()


#-----------------------------------------------------------------------------------------------------------------------
#-----------------------------------------------------------------------------------------------------------------------
function(octk_internal_get_active_linker_flags out_var)
    set(flags "")
    if(GCC OR CLANG)
        if(OCTK_FEATURE_use_gold_linker)
            list(APPEND flags "-fuse-ld=gold")
        elseif(OCTK_FEATURE_use_bfd_linker)
            list(APPEND flags "-fuse-ld=bfd")
        elseif(OCTK_FEATURE_use_lld_linker)
            list(APPEND flags "-fuse-ld=lld")
        elseif(OCTK_FEATURE_use_mold_linker)
            octk_internal_get_mold_linker_flags(mold_flags ERROR_IF_EMPTY)
            list(APPEND flags "${mold_flags}")
        endif()
    endif()
    set(${out_var} "${flags}" PARENT_SCOPE)
endfunction()


#-----------------------------------------------------------------------------------------------------------------------
# gcc expects -fuse-ld=mold (no absolute path can be given) (gcc >= 12.1)
#             or an 'ld' symlink to 'mold' in a dir that is passed via -B flag (gcc < 12.1)
#
# clang expects     -fuse-ld=mold
#                or -fuse-ld=<mold-abs-path>
#                or --ldpath=<mold-abs-path>  (clang >= 12)
# https://github.com/rui314/mold/#how-to-use
# TODO: In the gcc < 12.1 case, the octk_internal_check_if_linker_is_available(mold) check will
#       always return TRUE because gcc will not error out if it is given a -B flag pointing to an
#       invalid dir, as well as when the the symlink to the linker in the -B dir is not actually
#       a valid linker.
#       It would be nice to handle that case in a better way, but it's not that important
#       given that gcc > 12.1 now supports -fuse-ld=mold
# NOTE: In comparison to clang, in the gcc < 12.1 case, we pass the full path to where mold is
#       and that is recorded in PlatformCommonInternal's INTERFACE_LINK_OPTIONS target.
#       Moving such a OpenCTK to a different machine and trying to build another repo won't
#       work because the recorded path will be invalid. This is not a problem with
#       the gcc >= 12.1 case
#-----------------------------------------------------------------------------------------------------------------------
function(octk_internal_get_mold_linker_flags out_var)
    cmake_parse_arguments(PARSE_ARGV 1 arg "ERROR_IF_EMPTY" "" "")

    find_program(OCTK_INTERNAL_LINKER_MOLD mold)

    set(flag "")
    if(OCTK_INTERNAL_LINKER_MOLD)
        if(GCC)
            if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "12.1")
                set(flag "-fuse-ld=mold")
            else()
                set(mold_linker_dir "${CMAKE_CURRENT_BINARY_DIR}/.octk_linker")
                set(mold_linker_path "${mold_linker_dir}/ld")
                if(NOT EXISTS "${mold_linker_dir}")
                    file(MAKE_DIRECTORY "${mold_linker_dir}")
                endif()
                if(NOT EXISTS "${mold_linker_path}")
                    file(CREATE_LINK
                        "${OCTK_INTERNAL_LINKER_MOLD}"
                        "${mold_linker_path}"
                        SYMBOLIC)
                endif()
                set(flag "-B${mold_linker_dir}")
            endif()
        elseif(CLANG)
            if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "12")
                set(flag "--ld-path=mold")
            else()
                set(flag "-fuse-ld=mold")
            endif()
        endif()
    endif()
    if(arg_ERROR_IS_EMPTY AND NOT flag)
        message(FATAL_ERROR "Could not determine the flags to use the mold linker.")
    endif()
    set(${out_var} "${flag}" PARENT_SCOPE)
endfunction()
