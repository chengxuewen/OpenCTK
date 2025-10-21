########################################################################################################################
# Copyright (C) 2025~Present MaxSense, Created by ChengXueWen.
########################################################################################################################

#-----------------------------------------------------------------------------------------------------------------------
# Set example output path
#-----------------------------------------------------------------------------------------------------------------------
set(OCTK_EXAMPLE_OUTPUT_DIR ${OCTK_BUILD_DIR}/${OCTK_INSTALL_EXAMPLESDIR})


#-----------------------------------------------------------------------------------------------------------------------
# Set example link libraries
#-----------------------------------------------------------------------------------------------------------------------
octk_find_package(WrapFFmpeg PROVIDED_TARGETS OCTK3rdparty::WrapFFmpeg)
list(APPEND OCTK_EXAMPLE_LINK_LIBRARIES OCTK3rdparty::WrapFFmpeg)


#-----------------------------------------------------------------------------------------------------------------------
# Add examples
#-----------------------------------------------------------------------------------------------------------------------
octk_add_executable(octk_ffmpeg_exp_aac
    SOURCES
    exp_ffmpeg_aac.cpp
    INCLUDE_DIRECTORIES
    ${CMAKE_CURRENT_SOURCE_DIR}
    LIBRARIES
    ${OCTK_EXAMPLE_LINK_LIBRARIES}
    OUTPUT_DIRECTORY
    ${OCTK_EXAMPLE_OUTPUT_DIR})
octk_add_executable(octk_ffmpeg_exp_aacdec
    SOURCES
    exp_ffmpeg_aacdec.cpp
    INCLUDE_DIRECTORIES
    ${CMAKE_CURRENT_SOURCE_DIR}
    LIBRARIES
    ${OCTK_EXAMPLE_LINK_LIBRARIES}
    OUTPUT_DIRECTORY
    ${OCTK_EXAMPLE_OUTPUT_DIR})
octk_add_executable(octk_ffmpeg_exp_buffer
    SOURCES
    exp_ffmpeg_buffer.cpp
    INCLUDE_DIRECTORIES
    ${CMAKE_CURRENT_SOURCE_DIR}
    LIBRARIES
    ${OCTK_EXAMPLE_LINK_LIBRARIES}
    OUTPUT_DIRECTORY
    ${OCTK_EXAMPLE_OUTPUT_DIR})
octk_add_executable(octk_ffmpeg_exp_demux
    SOURCES
    exp_ffmpeg_demux.cpp
    INCLUDE_DIRECTORIES
    ${CMAKE_CURRENT_SOURCE_DIR}
    LIBRARIES
    ${OCTK_EXAMPLE_LINK_LIBRARIES}
    OUTPUT_DIRECTORY
    ${OCTK_EXAMPLE_OUTPUT_DIR})
octk_add_executable(octk_ffmpeg_exp_h264dec
    SOURCES
    exp_ffmpeg_h264dec.cpp
    INCLUDE_DIRECTORIES
    ${CMAKE_CURRENT_SOURCE_DIR}
    LIBRARIES
    ${OCTK_EXAMPLE_LINK_LIBRARIES}
    OUTPUT_DIRECTORY
    ${OCTK_EXAMPLE_OUTPUT_DIR})
octk_add_executable(octk_ffmpeg_exp_h264enc
    SOURCES
    exp_ffmpeg_h264enc.cpp
    INCLUDE_DIRECTORIES
    ${CMAKE_CURRENT_SOURCE_DIR}
    LIBRARIES
    ${OCTK_EXAMPLE_LINK_LIBRARIES}
    OUTPUT_DIRECTORY
    ${OCTK_EXAMPLE_OUTPUT_DIR})