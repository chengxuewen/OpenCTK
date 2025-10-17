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
add_learn_opengl_executable(8_20_oit
	DIRECTORY 2020/oit)
#add_learn_opengl_executable(8_20_skeletal_animation
#	DIRECTORY 2020/skeletal_animation)
#add_learn_opengl_executable(8_21_1_scene
#	DIRECTORY 2021/1.scene)
#add_learn_opengl_executable(8_21_2_csm
#	DIRECTORY 2021/2.csm)
#add_learn_opengl_executable(8_21_3_tessellation
#	DIRECTORY 2021/3.tessellation)
add_learn_opengl_executable(8_21_4_dsa
	DIRECTORY 2021/4.dsa)
add_learn_opengl_executable(8_22_5_computeshader_helloworld
	DIRECTORY 2022/5.computeshader_helloworld)
add_learn_opengl_executable(8_22_6_physically_based_bloom
	DIRECTORY 2022/6.physically_based_bloom)
#add_learn_opengl_executable(8_22_7_area_lights
#	DIRECTORY 2022/7.area_lights)