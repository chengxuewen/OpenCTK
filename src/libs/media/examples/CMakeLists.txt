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
# Set example output path
#-----------------------------------------------------------------------------------------------------------------------
set(OCTK_EXAMPLE_OUTPUT_DIR ${OCTK_BUILD_DIR}/${OCTK_INSTALL_EXAMPLESDIR})


#-----------------------------------------------------------------------------------------------------------------------
# Set example link libraries
#-----------------------------------------------------------------------------------------------------------------------
#octk_find_package(WrapFFmpeg PROVIDED_TARGETS OCTK3rdparty::WrapFFmpeg)
octk_find_package(WrapSDL PROVIDED_TARGETS OCTK3rdparty::WrapSDL)
list(APPEND OCTK_EXAMPLE_LINK_LIBRARIES octk::media)


#-----------------------------------------------------------------------------------------------------------------------
# Add examples
#-----------------------------------------------------------------------------------------------------------------------
include(InstallFFmpegTools)
if(NOT EXISTS "${OCTK_EXAMPLE_OUTPUT_DIR}/big_buck_bunny_360x240.mp4")
	# ffmpeg -i big_buck_bunny_360x240.mp4 -s 360x240 -pix_fmt rgba big_buck_bunny_360x240.rgb
	# ffplay -pixel_format rgba -video_size 360x240 -i big_buck_bunny_360x240.rgb
	execute_process(
		COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/big_buck_bunny_360x240.mp4" "./"
		COMMAND ${OCTKFFmpeg_EXECUTABLE} -i ./big_buck_bunny_360x240.mp4 -vn -acodec copy ./big_buck_bunny_360x240.aac -y
		COMMAND ${OCTKFFmpeg_EXECUTABLE} -i ./big_buck_bunny_360x240.mp4 -s 360x240 -an -f h264 ./big_buck_bunny_360x240.h264 -y
		COMMAND ${OCTKFFmpeg_EXECUTABLE} -i ./big_buck_bunny_360x240.mp4 -s 360x240 -pix_fmt rgba ./big_buck_bunny_360x240_rgba.rgb -y
		COMMAND ${OCTKFFmpeg_EXECUTABLE} -i ./big_buck_bunny_360x240.mp4 -s 360x240 -pix_fmt argb ./big_buck_bunny_360x240_argb.rgb -y
		COMMAND ${OCTKFFmpeg_EXECUTABLE} -i ./big_buck_bunny_360x240.mp4 -s 360x240 -pix_fmt bgra ./big_buck_bunny_360x240_bgra.rgb -y
		COMMAND ${OCTKFFmpeg_EXECUTABLE} -i ./big_buck_bunny_360x240.mp4 -s 360x240 -pix_fmt abgr ./big_buck_bunny_360x240_abgr.rgb -y
		COMMAND ${OCTKFFmpeg_EXECUTABLE} -i ./big_buck_bunny_360x240.mp4 -s 360x240 -pix_fmt rgb24 ./big_buck_bunny_360x240_rgb24.rgb -y
		COMMAND ${OCTKFFmpeg_EXECUTABLE} -i ./big_buck_bunny_360x240.mp4 -s 360x240 -pix_fmt yuv420p ./big_buck_bunny_360x240_i420.yuv -y
		COMMAND ${OCTKFFmpeg_EXECUTABLE} -i ./big_buck_bunny_360x240.mp4 -t 20 -codec:a pcm_s16le -ar 44100 -ac 2 -f s16le ./big_buck_bunny_44100_16bit_2ch.pcm -y
		WORKING_DIRECTORY "${OCTK_EXAMPLE_OUTPUT_DIR}")
endif()

add_subdirectory(capture)
add_subdirectory(ffmpeg)