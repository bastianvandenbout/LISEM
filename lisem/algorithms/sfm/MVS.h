/*
* MVS.h
*
* Copyright (c) 2014-2015 SEACAVE
*
* Author(s):
*
*      cDc <cdc.seacave@gmail.com>
*
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Affero General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Affero General Public License for more details.
*
* You should have received a copy of the GNU Affero General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*
* Additional Terms:
*
*      You are required to preserve legal notices and author attributions in
*      that material or in the Appropriate Legal Notices displayed by works
*      containing it.
*/

#ifndef _MVS_MVS_H_
#define _MVS_MVS_H_


// D E F I N E S ///////////////////////////////////////////////////

#define OpenMVS_VERSION_AT_LEAST(x,y,z) \
	(OpenMVS_MAJOR_VERSION>x || (OpenMVS_MAJOR_VERSION==x && \
	(OpenMVS_MINOR_VERSION>y || (OpenMVS_MINOR_VERSION==y && OpenMVS_PATCH_VERSION>=z))))


// I N C L U D E S /////////////////////////////////////////////////

// OpenMVS version
#define OpenMVS_MAJOR_VERSION 1
#define OpenMVS_MINOR_VERSION 1
#define OpenMVS_PATCH_VERSION 1

// OpenMVS compiled as static or dynamic libs
/* #undef BUILD_SHARED_LIBS */

// Define to 1 if you have the <inttypes.h> header file
#define HAVE_INTTYPES_H 1

// Define to 1 if exceptions are enabled
#define _HAS_EXCEPTIONS 1

// Define to 1 if RTTI is enabled
#define _HAS_RTTI 1

// OpenMP support
#define _USE_OPENMP

// BREAKPAD support
/* #undef _USE_BREAKPAD */

// Boost support
#define _USE_BOOST

// Eigen Matrix & Linear Algebra Library
#define _USE_EIGEN

// CERES optimization library
/* #undef _USE_CERES */

// JPEG codec
#define _USE_JPG

// PNG codec
#define _USE_PNG

// TIFF codec
#define _USE_TIFF

// OpenGL support
#define _USE_OPENGL

// OpenCL support
/* #undef _USE_OPENCL */
/* #undef _USE_OPENCL_STATIC */
/* #undef _USE_OPENCL_SVM */

// NVidia Cuda Runtime API
/* #undef _USE_CUDA */

// Fast float to int support
#define _USE_FAST_FLOAT2INT

// Fast CBRT support
#define _USE_FAST_CBRT

// SSE support
#define _USE_SSE


#include "sfm/Common/Common.h"
#include "sfm/IO/Common.h"
#include "sfm/Math/Common.h"
#include "sfm/MVS/Common.h"
#include "sfm/MVS/Scene.h"


#endif // _MVS_MVS_H_
