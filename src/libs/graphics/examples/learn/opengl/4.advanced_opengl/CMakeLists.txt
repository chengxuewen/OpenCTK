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
add_learn_opengl_executable(4_1_1_depth_testing
	DIRECTORY 1.1.depth_testing)
add_learn_opengl_executable(4_1_2_depth_testing_view
	DIRECTORY 1.2.depth_testing_view)
add_learn_opengl_executable(4_2_stencil_testing
	DIRECTORY 2.stencil_testing)
add_learn_opengl_executable(4_3_1_blending_discard
	DIRECTORY 3.1.blending_discard)
add_learn_opengl_executable(4_3_2_blending_sort
	DIRECTORY 3.2.blending_sort)
#add_learn_opengl_executable(4_4_face_culling_exercise1
#	DIRECTORY 4.face_culling_exercise1)
add_learn_opengl_executable(4_5_1_framebuffers
	DIRECTORY 5.1.framebuffers)
add_learn_opengl_executable(4_5_2_framebuffers_exercise1
	DIRECTORY 5.2.framebuffers_exercise1)
add_learn_opengl_executable(4_6_1_cubemaps_skybox
	DIRECTORY 6.1.cubemaps_skybox)
add_learn_opengl_executable(4_6_2_cubemaps_environment_mapping
	DIRECTORY 6.2.cubemaps_environment_mapping)
add_learn_opengl_executable(4_8_advanced_glsl_ubo
	DIRECTORY 8.advanced_glsl_ubo)
add_learn_opengl_executable(4_9_1_geometry_shader_houses
	DIRECTORY 9.1.geometry_shader_houses)
add_learn_opengl_executable(4_9_2_geometry_shader_exploding
	DIRECTORY 9.2.geometry_shader_exploding)
add_learn_opengl_executable(4_9_3_geometry_shader_normals
	DIRECTORY 9.3.geometry_shader_normals)
add_learn_opengl_executable(4_10_1_instancing_quads
	DIRECTORY 10.1.instancing_quads)
add_learn_opengl_executable(4_10_2_asteroids
	DIRECTORY 10.2.asteroids)
add_learn_opengl_executable(4_10_3_asteroids_instanced
	DIRECTORY 10.3.asteroids_instanced)
add_learn_opengl_executable(4_11_1_anti_aliasing_msaa
	DIRECTORY 11.1.anti_aliasing_msaa)
add_learn_opengl_executable(4_11_2_anti_aliasing_offscreen
	DIRECTORY 11.2.anti_aliasing_offscreen)