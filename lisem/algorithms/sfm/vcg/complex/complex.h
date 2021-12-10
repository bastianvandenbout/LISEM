/****************************************************************************
* VCGLib                                                            o o     *
* Visual and Computer Graphics Library                            o     o   *
*                                                                _   O  _   *
* Copyright(C) 2004-2016                                           \/)\/    *
* Visual Computing Lab                                            /\/|      *
* ISTI - Italian National Research Council                           |      *
*                                                                    \      *
* All rights reserved.                                                      *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
*                                                                           *
****************************************************************************/

#ifndef __VCG_MESH_H
#define __VCG_MESH_H
#define __VCG_MESH

#include <cassert>
#include <cstring>
#include <string>
#include <ctime>
#include <vector>
#include <set>
#include <stack>
#include <queue>
#include <map>
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <limits>
#include <iterator>
#include <typeindex>
#include <sfm/wrap/callback.h>
#include <sfm/vcg/complex/exception.h>
#include <sfm/vcg/container/simple_temporary_data.h>
#include <sfm/vcg/complex/used_types.h>
#include <sfm/vcg/complex/base.h>
#include <sfm/vcg/complex/allocate.h>
#include <sfm/vcg/simplex/face/pos.h>
#include <sfm/vcg/simplex/face/topology.h>
#include <sfm/vcg/simplex/edge/pos.h>
#include <sfm/vcg/simplex/edge/topology.h>
#include <sfm/vcg/simplex/tetrahedron/pos.h>
#include <sfm/vcg/simplex/tetrahedron/topology.h>
#include <sfm/vcg/complex/foreach.h>
#include <sfm/vcg/complex/algorithms/update/flag.h>
#include <sfm/vcg/complex/algorithms/update/selection.h>
#include <sfm/vcg/complex/algorithms/update/topology.h>
#include <sfm/vcg/complex/algorithms/update/normal.h>
#include <sfm/vcg/complex/algorithms/update/bounding.h>
#include <sfm/vcg/complex/algorithms/mesh_assert.h>
#include <sfm/vcg/complex/append.h>

#undef __VCG_MESH
#endif

