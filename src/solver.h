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

#ifndef SOLVER_H_
#define SOLVER_H_

#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <chrono>
#include <cstdlib>
#include <cstdint>

#include <sys/time.h>

#include <openvdb/Types.h>
#include <openvdb/openvdb.h>
#include <openvdb/tools/ValueTransformer.h>
#include <openvdb/tools/Interpolation.h>
#include <openvdb/tools/VelocityFields.h>
#include <openvdb/tools/GridOperators.h>

#include "common.h"
#include "log.h"
#include "config.h"
#include "utils.h"
#include "grid.h"
#include "particles.h"
#include "sparse_solver.h"

using namespace openvdb;
using namespace std;

namespace yapfs
{

    enum VoxelType { NDF=0, FLUID=1, SOLID=2, AIR=3 };

    struct AbsMax
    {
        Vec3d *mAbsMax;
        AbsMax(Vec3d *_absMax): mAbsMax(_absMax) {}
        template<typename TreeIterT> void operator()(const TreeIterT& it) const
        {
            Vec3d v(*it);
            if (std::fabs(v.x()) > mAbsMax->x()) mAbsMax->init(std::fabs(v.x()), mAbsMax->y(), mAbsMax->z() );
            if (std::fabs(v.y()) > mAbsMax->y()) mAbsMax->init(mAbsMax->x(), std::fabs(v.y()), mAbsMax->z() );
            if (std::fabs(v.z()) > mAbsMax->z()) mAbsMax->init(mAbsMax->x(), mAbsMax->y(), std::fabs(v.z()) );
        }
    };

    class Solver
    {

        public:

            LReal    mGravity;
            LReal    mVoxelSize;
            Vec3d    mMinBox; // lower point box MAC grid
            Vec3d    mMaxBox; // upper point box MAC grid
            uint32_t mNX; // num voxel grid in X
            uint32_t mNY; // num voxel grid in X
            uint32_t mNZ; // num voxel grid in Z
            Vec3d    mDimBox; // dimension MAC grid
            Vec3i    mMinN; // lower grid index
            Vec3i    mMaxN; // upper grid index
            uint32_t mFramesPerSec;
            LReal    mFrameTime; // 1.0 / mFramePerSec
            LReal    mDt;
            uint32_t mNumFrames;
            uint32_t mIdFrame;

            Grid<Vec3DGrid>    *mGVel; //Staggered MAC Grid
            Grid<Vec3DGrid>    *mGVelSave;
            Grid<Int32Grid>    *mGTypeVoxel;
            Grid<DoubleGrid>   *mGDivergence;
            Grid<DoubleGrid>   *mGP; //Pressure

            // TODO: using DNeg OpenVDBPoints
            Particles          *mParticles;

            Solver();

            void initGrids();

            void doStep(LReal dt);
            bool doFrame();
            void execute();
            void exportFrame();

            Vec3d getAbsMax(Grid<Vec3DGrid> *grid);
            LReal getCFL();

            inline void clampToGrid(Vec3d &vect);
            inline LReal getComponentWeight(LReal val, uint8_t cxyz);

            void moveParticlesInGrid(LReal dt);
            void transferToGrid();
            void saveVelocities();
            void addGravity(LReal dt);
            void addExternalForces(LReal dt);
            void identifyTypeVoxels();
            void velocityExtrapolation();
            void boundaryConditions();
            void solvePressure();
            void computeDivergence();
            void addGradient();
            void saveVelocitiesUpdate();
            void updateParticlesVelocity();

    };

}

#endif /* SOLVER_H_ */