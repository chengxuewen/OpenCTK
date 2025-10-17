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


#-----------------------------------------------------------------------------------------------------------------------
# Add examples
#-----------------------------------------------------------------------------------------------------------------------
add_learn_opengl_executable(7_1_debugging
	DIRECTORY 1.debugging)
add_learn_opengl_executable(7_2_text_rendering
	DIRECTORY 2.text_rendering)
#add_learn_opengl_executable(7_3_2d_game
#	INCLUDE_DIRS
#	${CMAKE_CURRENT_SOURCE_DIR}/3.2d_game/0.full_source
#	${CMAKE_CURRENT_SOURCE_DIR}/3.2d_game/0.full_source/progress
#	DIRECTORY 3.2d_game)