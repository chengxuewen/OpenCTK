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
add_learn_opengl_executable(2_1_colors
	DIRECTORY 1.colors)
add_learn_opengl_executable(2_2_1_basic_lighting_diffuse
	DIRECTORY 2.1.basic_lighting_diffuse)
add_learn_opengl_executable(2_2_2_basic_lighting_specular
	DIRECTORY 2.2.basic_lighting_specular)
#add_learn_opengl_executable(2_2_3_basic_lighting_exercise1
#	DIRECTORY 2.3.basic_lighting_exercise1)
#add_learn_opengl_executable(2_2_4_basic_lighting_exercise2
#	DIRECTORY 2.4.basic_lighting_exercise2)
#add_learn_opengl_executable(2_2_5_basic_lighting_exercise3
#	DIRECTORY 2.5.basic_lighting_exercise3)
add_learn_opengl_executable(2_3_1_materials
	DIRECTORY 3.1.materials)
add_learn_opengl_executable(2_3_2_materials_exercise1
	DIRECTORY 3.2.materials_exercise1)
add_learn_opengl_executable(2_4_1_lighting_maps_diffuse_map
	DIRECTORY 4.1.lighting_maps_diffuse_map)
add_learn_opengl_executable(2_4_2_lighting_maps_specular_map
	DIRECTORY 4.2.lighting_maps_specular_map)
#add_learn_opengl_executable(2_4_3_lighting_maps_exercise2
#	DIRECTORY 4.3.lighting_maps_exercise2)
#add_learn_opengl_executable(2_4_4_lighting_maps_exercise4
#	DIRECTORY 4.4.lighting_maps_exercise4)
add_learn_opengl_executable(2_5_1_light_casters_directional
	DIRECTORY 5.1.light_casters_directional)
add_learn_opengl_executable(2_5_2_light_casters_point
	DIRECTORY 5.2.light_casters_point)
add_learn_opengl_executable(2_5_3_light_casters_spot
	DIRECTORY 5.3.light_casters_spot)
add_learn_opengl_executable(2_5_4_light_casters_spot_soft
	DIRECTORY 5.4.light_casters_spot_soft)
add_learn_opengl_executable(2_6_multiple_lights
	DIRECTORY 6.multiple_lights)
#add_learn_opengl_executable(2_6_multiple_lights_exercise1
#	DIRECTORY 6.multiple_lights_exercise1)