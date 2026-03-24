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


#-----------------------------------------------------------------------------------------------------------------------
# Add examples
#-----------------------------------------------------------------------------------------------------------------------
octk_add_executable(OpenCTKMediaExpFFmpegAac
    SOURCES
    exp_ffmpeg_aac.cpp
    INCLUDE_DIRECTORIES
    ${CMAKE_CURRENT_SOURCE_DIR}
    LIBRARIES
	OCTK3rdparty::WrapFFmpeg
    ${OCTK_EXAMPLE_LINK_LIBRARIES}
    OUTPUT_DIRECTORY
    ${OCTK_EXAMPLE_OUTPUT_DIR})
octk_add_executable(OpenCTKMediaExpFFmpegAacdec
    SOURCES
    exp_ffmpeg_aacdec.cpp
    INCLUDE_DIRECTORIES
    ${CMAKE_CURRENT_SOURCE_DIR}
    LIBRARIES
	OCTK3rdparty::WrapFFmpeg
    ${OCTK_EXAMPLE_LINK_LIBRARIES}
    OUTPUT_DIRECTORY
    ${OCTK_EXAMPLE_OUTPUT_DIR})
octk_add_executable(OpenCTKMediaExpFFmpegBuffer
    SOURCES
    exp_ffmpeg_buffer.cpp
    INCLUDE_DIRECTORIES
    ${CMAKE_CURRENT_SOURCE_DIR}
    LIBRARIES
	OCTK3rdparty::WrapFFmpeg
    ${OCTK_EXAMPLE_LINK_LIBRARIES}
    OUTPUT_DIRECTORY
    ${OCTK_EXAMPLE_OUTPUT_DIR})
octk_add_executable(OpenCTKMediaExpFFmpegDemux
    SOURCES
    exp_ffmpeg_demux.cpp
    INCLUDE_DIRECTORIES
    ${CMAKE_CURRENT_SOURCE_DIR}
    LIBRARIES
	OCTK3rdparty::WrapFFmpeg
    ${OCTK_EXAMPLE_LINK_LIBRARIES}
    OUTPUT_DIRECTORY
    ${OCTK_EXAMPLE_OUTPUT_DIR})
octk_add_executable(OpenCTKMediaExpFFmpegH264dec
    SOURCES
    exp_ffmpeg_h264dec.cpp
    INCLUDE_DIRECTORIES
    ${CMAKE_CURRENT_SOURCE_DIR}
    LIBRARIES
	OCTK3rdparty::WrapFFmpeg
    ${OCTK_EXAMPLE_LINK_LIBRARIES}
    OUTPUT_DIRECTORY
    ${OCTK_EXAMPLE_OUTPUT_DIR})
octk_add_executable(OpenCTKMediaExpFFmpegH264enc
    SOURCES
    exp_ffmpeg_h264enc.cpp
    INCLUDE_DIRECTORIES
    ${CMAKE_CURRENT_SOURCE_DIR}
    LIBRARIES
	OCTK3rdparty::WrapFFmpeg
    ${OCTK_EXAMPLE_LINK_LIBRARIES}
    OUTPUT_DIRECTORY
    ${OCTK_EXAMPLE_OUTPUT_DIR})