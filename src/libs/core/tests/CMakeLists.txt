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
# Set tests output path
#-----------------------------------------------------------------------------------------------------------------------
set(OCTK_TEST_OUTPUT_DIR ${OCTK_BUILD_DIR}/${OCTK_INSTALL_TESTSDIR})


#-----------------------------------------------------------------------------------------------------------------------
# Add tests link libraries
#-----------------------------------------------------------------------------------------------------------------------
set(OCTK_TEST_LINK_LIBRARIES octk::core OCTK3rdparty::WrapGTest OCTK3rdparty::WrapBenchmark)


#-----------------------------------------------------------------------------------------------------------------------
# Add tests
#-----------------------------------------------------------------------------------------------------------------------
octk_add_test(octk_core_tst_checks
    SOURCES
    tst_checks.cpp
    INCLUDE_DIRECTORIES
    LIBRARIES
    ${OCTK_TEST_LINK_LIBRARIES}
    OUTPUT_DIRECTORY
    ${OCTK_TEST_OUTPUT_DIR})
#octk_add_test(octk_core_tst_mutex
#    SOURCES
#    tst_mutex.cpp
#    INCLUDE_DIRECTORIES
#    LIBRARIES
#    ${OCTK_TEST_LINK_LIBRARIES}
#    OUTPUT_DIRECTORY
#    ${OCTK_TEST_OUTPUT_DIR})
octk_add_test(octk_core_tst_string_utils
    SOURCES
    tst_string_utils.cpp
    INCLUDE_DIRECTORIES
    LIBRARIES
    ${OCTK_TEST_LINK_LIBRARIES}
    OUTPUT_DIRECTORY
    ${OCTK_TEST_OUTPUT_DIR})