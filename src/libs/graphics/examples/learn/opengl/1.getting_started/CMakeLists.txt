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
add_learn_opengl_executable(1_1_1_hello_window
	DIRECTORY 1.1.hello_window)
add_learn_opengl_executable(1_1_2_hello_window_clear
	DIRECTORY 1.2.hello_window_clear)
add_learn_opengl_executable(1_2_1_hello_triangle
	DIRECTORY 2.1.hello_triangle)
add_learn_opengl_executable(1_2_2_hello_triangle_indexed
	DIRECTORY 2.2.hello_triangle_indexed)
add_learn_opengl_executable(1_2_3_hello_triangle_exercise1
	DIRECTORY 2.3.hello_triangle_exercise1)
add_learn_opengl_executable(1_2_4_hello_triangle_exercise2
	DIRECTORY 2.4.hello_triangle_exercise2)
add_learn_opengl_executable(1_2_5_hello_triangle_exercise3
	DIRECTORY 2.5.hello_triangle_exercise3)
add_learn_opengl_executable(1_3_1_shaders_uniform
	DIRECTORY 3.1.shaders_uniform)
add_learn_opengl_executable(1_3_2_shaders_interpolation
	DIRECTORY 3.2.shaders_interpolation)
add_learn_opengl_executable(1_3_3_shaders_class
	DIRECTORY 3.3.shaders_class)
add_learn_opengl_executable(1_4_1_textures
	DIRECTORY 4.1.textures)
add_learn_opengl_executable(1_4_2_textures_combined
	DIRECTORY 4.2.textures_combined)
#add_learn_opengl_executable(1_4_3_textures_exercise1
#	DIRECTORY 4.3.textures_exercise1)
#add_learn_opengl_executable(1_4_4_textures_exercise2
#	DIRECTORY 4.4.textures_exercise2)
#add_learn_opengl_executable(1_4_5_textures_exercise3
#	DIRECTORY 4.5.textures_exercise3)
#add_learn_opengl_executable(1_4_6_textures_exercise4
#	DIRECTORY 4.6.textures_exercise4)
add_learn_opengl_executable(1_5_1_transformations
	DIRECTORY 5.1.transformations)
#add_learn_opengl_executable(1_5_2_transformations_exercise1
#	DIRECTORY 5.2.transformations_exercise1)
#add_learn_opengl_executable(1_5_2_transformations_exercise2
#	DIRECTORY 5.2.transformations_exercise2)
add_learn_opengl_executable(1_6_1_coordinate_systems
	DIRECTORY 6.1.coordinate_systems)
add_learn_opengl_executable(1_6_2_coordinate_systems_depth
	DIRECTORY 6.2.coordinate_systems_depth)
add_learn_opengl_executable(1_6_3_coordinate_systems_multiple
	DIRECTORY 6.3.coordinate_systems_multiple)
add_learn_opengl_executable(1_7_1_camera_circle
	DIRECTORY 7.1.camera_circle)
add_learn_opengl_executable(1_7_2_camera_keyboard_dt
	DIRECTORY 7.2.camera_keyboard_dt)
add_learn_opengl_executable(1_7_3_camera_mouse_zoom
	DIRECTORY 7.3.camera_mouse_zoom)
add_learn_opengl_executable(1_7_4_camera_class
	DIRECTORY 7.4.camera_class)