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

#ifndef GRID_H_
#define GRID_H_

#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <chrono>
#include <cstdlib>
#include <cstdint>

#include <sys/time.h>

#include <openvdb/openvdb.h>

#include "common.h"
#include "log.h"
#include "config.h"
#include "utils.h"
#include "particles.h"

using namespace openvdb;
using namespace std;

namespace yapfs
{

    template<typename T>
    class Grid
    {

        typedef typename T::Ptr GridTypePtr;
        typedef typename T::Accessor AccessorType;
        typedef typename T::ValueType ValueT;

        public:

            GridTypePtr mGrid;
            openvdb::math::Transform::Ptr mLinearTransform;
            LReal mVoxelSize;

            Grid(LReal voxelSize)
            {
                mVoxelSize = voxelSize;

                openvdb::initialize();
                mGrid = T::create();
                mLinearTransform =  openvdb::math::Transform::createLinearTransform(mVoxelSize);
                // Add the offset for cell-centered transform, otherwise cells are Vertex-Centered
                //const Vec3d offset(mVoxelSize/2.0, mVoxelSize/2.0, mVoxelSize/2.0);
                //mLinearTransform->postTranslate(offset);
                mGrid->setTransform(mLinearTransform);

            }

            void setValue(ValueT value, int64_t i, int64_t j, int64_t k)
            {
                AccessorType accessor = mGrid->getAccessor(); //TODO: Create one accessor per thread
                openvdb::Coord ijk(i, j, k);
                accessor.setValue(ijk, value);
            }

            ValueT getValue(int64_t i, int64_t j, int64_t k)
            {
                AccessorType accessor = mGrid->getAccessor(); //TODO: Create one accessor per thread
                openvdb::Coord ijk(i, j, k);
                return accessor.getValue(ijk);
            }

            // share/doc/openvdb/html/transformsAndMaps.html
            Vec3d getIndexToWorld(int64_t i, int64_t j, int64_t k)
            {
                openvdb::Coord ijk(i, j, k);
                return mLinearTransform->indexToWorld(ijk);
            }

            Vec3d getIndexToWorld(Vec3d indexSpacePoint)
            {
                return mLinearTransform->indexToWorld(indexSpacePoint);
            }

            // returns i, j, k
            Vec3d getWorldToIndex(Vec3d worldSpacePoint)
            {
                return mLinearTransform->worldToIndex(worldSpacePoint);
            }

            // Empty this grid, so that all voxels become inactive background voxels
            void clear()
            {
                mGrid->clear();
            }

            void deepCopyFromGrid(Grid *grid)
            {
                mGrid = grid->mGrid->deepCopy();
                mLinearTransform = grid->mLinearTransform;
                mVoxelSize = grid->mVoxelSize;
            }

            uint64_t getMemUsage()
            {
                return mGrid->memUsage();
            }

    };


}

#endif /* GRID_H_ */