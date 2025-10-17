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
add_learn_opengl_executable(5_1_advanced_lighting
	DIRECTORY 1.advanced_lighting)
add_learn_opengl_executable(5_2_gamma_correction
	DIRECTORY 2.gamma_correction)
add_learn_opengl_executable(5_3_1_1_shadow_mapping_depth
	DIRECTORY 3.1.1.shadow_mapping_depth)
add_learn_opengl_executable(5_3_1_2_shadow_mapping_base
	DIRECTORY 3.1.2.shadow_mapping_base)
add_learn_opengl_executable(5_3_1_3_shadow_mapping
	DIRECTORY 3.1.3.shadow_mapping)
add_learn_opengl_executable(5_3_2_1_point_shadows
	DIRECTORY 3.2.1.point_shadows)
add_learn_opengl_executable(5_3_2_2_point_shadows_soft
	DIRECTORY 3.2.2.point_shadows_soft)
add_learn_opengl_executable(5_3_3_csm
	DIRECTORY 3.3.csm)
add_learn_opengl_executable(5_4_normal_mapping
	DIRECTORY 4.normal_mapping)
add_learn_opengl_executable(5_5_1_parallax_mapping
	DIRECTORY 5.1.parallax_mapping)
add_learn_opengl_executable(5_5_2_steep_parallax_mapping
	DIRECTORY 5.2.steep_parallax_mapping)
add_learn_opengl_executable(5_5_3_parallax_occlusion_mapping
	DIRECTORY 5.3.parallax_occlusion_mapping)
add_learn_opengl_executable(5_6_hdr
	DIRECTORY 6.hdr)
add_learn_opengl_executable(5_7_bloom
	DIRECTORY 7.bloom)
add_learn_opengl_executable(5_8_1_deferred_shading
	DIRECTORY 8.1.deferred_shading)
add_learn_opengl_executable(5_8_2_deferred_shading_volumes
	DIRECTORY 8.2.deferred_shading_volumes)
add_learn_opengl_executable(5_9_ssao
	DIRECTORY 9.ssao)