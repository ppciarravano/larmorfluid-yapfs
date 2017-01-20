/*****************************************************************************
 * LarmorFluid-YAPFS Version 1.0 2017
 * Copyright (c) 2017 Pier Paolo Ciarravano - http://www.larmor.com
 * All rights reserved.
 *
 * This file is part of LarmorFluid-YAPFS 
 * (https://github.com/ppciarravano/larmorfluid-yapfs).
 *
 * LarmorFluid-YAPFS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * LarmorFluid-YAPFS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with LarmorFluid-YAPFS. If not, see <http://www.gnu.org/licenses/>.
 *
 * Licensees holding a valid commercial license may use this file in
 * accordance with the commercial license agreement provided with the
 * software.
 *
 * Author: Pier Paolo Ciarravano
 *
 ****************************************************************************/

#ifndef VIEWER_H_
#define VIEWER_H_

#include <vector>
//#include <mutex> 
#include <iostream>
#include <string>
#include <sstream>
#include <chrono>
#include <cstdlib>
#include <cstdint>
#include <fstream>
#include <list>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits>
#include <math.h>

#include <sys/time.h>

#include <openvdb/openvdb.h>

/*
#include <openvdb/Types.h>
#include <openvdb/util/CpuTimer.h>
#include <openvdb/Exceptions.h>
#include <openvdb/tools/MeshToVolume.h>
#include <openvdb/util/Util.h>

#include <openvdb_points/openvdb.h>
#include <openvdb_points/tools/PointDataGrid.h>
#include <openvdb_points/tools/PointConversion.h>
#include <openvdb_points/tools/PointCount.h>
*/

#include "common.h"
#include "log.h"
#include "config.h"
#include "utils.h"
#include "grid.h"
#include "particles.h"
#include "solver.h"


#if defined(WIN32) || defined(WIN64)
    #include "windows.h"
    #include "gl/gl.h"
    #include "gl/glu.h"
#elif defined(__APPLE__) || defined(LINUX) || defined(linux)
    #include "GL/gl.h"
    #include "GL/glu.h"
#endif
//#define FREEGLUT_STATIC
#include "GL/freeglut.h"

using namespace openvdb;
using namespace std;

#define DEGREES_PER_PIXEL_ROT 0.3f
#define DEGREES_PER_PIXEL_TLT 0.3f
#define UNITS_PER_PIXEL_ZOM 0.5f
#define CONV_PI  3.14159265358979323846f
#define STEP_PER_SEC 30

namespace yapfs
{

	void openViewer(Solver &solver);

}

#endif /* VIEWER_H_ */






