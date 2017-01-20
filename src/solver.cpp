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

#include "solver.h"

namespace yapfs
{

    Solver::Solver()
    {

        L_LOG_INFO("Create Solver");

        mGravity      = getConfig<LReal>("gravity");
        mVoxelSize    = getConfig<LReal>("voxel_size");
        mMinBox       = Vec3d(getConfig<LReal>("min_x"), getConfig<LReal>("min_y"), getConfig<LReal>("min_z") );
        mMaxBox       = Vec3d(getConfig<LReal>("max_x"), getConfig<LReal>("max_y"), getConfig<LReal>("max_z") );
        mFramesPerSec = getConfig<uint32_t>("frames_per_sec");
        mNumFrames    = getConfig<uint32_t>("num_frames");

        mNX = (mMaxBox.x() - mMinBox.x()) / mVoxelSize;
        mNY = (mMaxBox.y() - mMinBox.y()) / mVoxelSize;
        mNZ = (mMaxBox.z() - mMinBox.z()) / mVoxelSize;
        L_LOG_INFO("Grid size: " + to_string(mNX) + ", " + to_string(mNY) + ", " + to_string(mNZ));
        mDimBox = mMaxBox - mMinBox;
        L_LOG_INFO("Grid box dimension: " + to_string(mDimBox.x()) + ", " + to_string(mDimBox.y()) + ", " + to_string(mDimBox.z()));

        mFrameTime = 1.0 / mFramesPerSec;
        mIdFrame = 0;
        mDt = mFrameTime;

        // Create grids
        mGVel        = new Grid<Vec3DGrid>(mVoxelSize); // velocity is in m/s
        mGVelSave    = new Grid<Vec3DGrid>(mVoxelSize);
        mGTypeVoxel  = new Grid<Int32Grid>(mVoxelSize);
        mGDivergence = new Grid<DoubleGrid>(mVoxelSize);
        mGP          = new Grid<DoubleGrid>(mVoxelSize);

        mParticles  = new Particles(mVoxelSize);

        mMinN = mGVel->getWorldToIndex(mMinBox);
        L_LOG_INFO("mMinN: " + to_string(mMinN.x()) + ", " + to_string(mMinN.y()) + ", " + to_string(mMinN.z()));

        mMaxN = mGVel->getWorldToIndex(mMaxBox);
        L_LOG_INFO("mMaxN: " + to_string(mMaxN.x()) + ", " + to_string(mMaxN.y()) + ", " + to_string(mMaxN.z()));

    }

    void Solver::initGrids()
    {

        L_LOG_INFO("Init Solver Grids");

        // Create particles somewhere just for first debugging
        for(int64_t i = mMinN.x(); i < mMaxN.x(); ++i)
            for(int64_t j = mMinN.y(); j < mMaxN.y(); ++j)
                for(int64_t k = mMinN.z(); k < mMaxN.z(); ++k)
                {
                    if ( ( ( j <= 45 ) && ( j > 20 ) && ( i < 30 ) && ( i > 20 ) && ( k < 30 ) && ( k > 20 )  )  || ( j < 10 ) ) 
                    //if ( j < 10 ) 
                    {
                        Vec3d worldVoxelCenter = mGVel->getIndexToWorld(i, j, k) + Vec3d(mVoxelSize / 2.0, mVoxelSize / 2.0, mVoxelSize / 2.0);
                        //L_LOG_INFO("worldVoxelCenter: " + to_string(worldVoxelCenter.x()) + ", " + to_string(worldVoxelCenter.y()) + ", " + to_string(worldVoxelCenter.z()));
                        mParticles->addParticlesInVoxel(worldVoxelCenter);
                    }
                }

        //TODO : assign velocity: it is just for first debugging
        for(int64_t i = 0; i < (mParticles->mPosition).size(); ++i){
            Vec3d particlePosition = (mParticles->mPosition)[i];
            Vec3d velCentr(0.0, 0.0, 0.0);
            mParticles->mVelocity.push_back(velCentr);
        }

        L_LOG_INFO("particles created: " + to_string( (mParticles->mPosition).size() ) );

        transferToGrid(); // Init grid velocities mGVel
        identifyTypeVoxels(); // Init grid type voxels mGTypeVoxel

    }

    Vec3d Solver::getAbsMax(Grid<Vec3DGrid> *grid)
    {
        Vec3d absMax(0.0, 0.0, 0.0);
        openvdb::tools::foreach(grid->mGrid->cbeginValueOn(), AbsMax(&absMax), true);
        //L_LOG_DEBUG("absMax: " + to_string(absMax.x()) + ", " + to_string(absMax.y()) + ", " + to_string(absMax.z()));
        return absMax;
    }

    LReal Solver::getCFL()
    {
        Vec3d absMax = getAbsMax(mGVel);
        LReal maxV3 = std::max(mVoxelSize*mGravity, sqr(absMax.x())+sqr(absMax.y())+sqr(absMax.z()) ); //TODO: when external force field is implemented, change mGravity with all the external forces not just gravity
        if(maxV3 < MIN_CONST)
        {
            maxV3 = MIN_CONST;
        }
        return mVoxelSize / sqrt(maxV3);
    }

    // Algorithm described at page 111 of Fluid Simulation for Computer Graphics by Robert Bridson (second edition 2015)
    bool Solver::doFrame()
    {

        if (mIdFrame >= mNumFrames)
        {
            return false;
        }

        L_LOG_INFO("*** FRAME ID: " + to_string(mIdFrame));

        // Export frame: e.g. rendering data
        exportFrame();

        // TODO: debugging this code

        LReal t = 0.0;
        LReal dt;
        bool frameCompleted = false;
        while(!frameCompleted)
        {
            dt = 2.0 * getCFL();
            //dt = mFrameTime / 5.0; //used for debugging
            if((t + dt) >= mFrameTime)
            {
                dt = mFrameTime - t;
                frameCompleted = true;
            }
            else if((t+1.5*dt) >= mFrameTime)
            {
                dt = 0.5 * (mFrameTime - t);
            }

            L_LOG_INFO("doStep: dt = " + to_string(dt) + " frame % = " + to_string(100*(t+dt) / mFrameTime) + " for FRAME ID: " + to_string(mIdFrame) );
            doStep(dt);
            t += dt;
        }

        mIdFrame++;
        return true;
    }

    void Solver::doStep(LReal dt)
    {

        for(int64_t i = 0; i < 5; ++i)
        {
            moveParticlesInGrid(0.2*dt);
        }
        transferToGrid();
        identifyTypeVoxels();
        saveVelocities();
        addGravity(dt);
        addExternalForces(dt);
        velocityExtrapolation();
        boundaryConditions();
        solvePressure();
        velocityExtrapolation();
        saveVelocitiesUpdate();
        updateParticlesVelocity();

    }

    void Solver::execute()
    {
        L_LOG_DEBUG("START Solver...");
        while (doFrame())
        {
            //L_LOG_DEBUG("Call solver.doFrame");
        }
        L_LOG_DEBUG("END Solver");
    }

    void Solver::exportFrame()
    {
        L_LOG_INFO("Export FRAME ID: " + to_string(mIdFrame));
        // TODO

        // TODO: remove and change, it is just for the OpenGL viewer
        mParticles->partFrame.push_back(mParticles->mPosition);
    }

    void Solver::moveParticlesInGrid(LReal dt)
    {

        // First implementation using tools::BoxSampler and Runge-Kutta implemented from scratch
        for(int64_t i = 0; i < (mParticles->mPosition).size(); ++i)
        {
            Vec3d particlePosition = (mParticles->mPosition)[i];

            // first stage of Runge-Kutta 2 (do a half Euler step)
            Vec3d indexSpacePoint = mGVel->getWorldToIndex(particlePosition); // particle grid index space point
            // Trilinear interpolation
            Vec3d gu = tools::BoxSampler::sample(mGVel->mGrid->tree(), indexSpacePoint);
            // Runge-Kutta second order
            Vec3d midPoint = particlePosition + 0.5 * dt * gu;

            indexSpacePoint = mGVel->getWorldToIndex(midPoint);
            gu = tools::BoxSampler::sample(mGVel->mGrid->tree(), indexSpacePoint);
            // second stage of Runge-Kutta 2
            particlePosition = particlePosition + dt * gu;

            //Clamp to grig
            clampToGrid(particlePosition);

            // save particle
            (mParticles->mPosition)[i] = particlePosition;

        }

        /*
        // Second implementation using openvdb::tools::VelocityIntegrator
        typedef tools::VelocityIntegrator<openvdb::Vec3DGrid, true>  VelocityIntg;
        VelocityIntg velInt(*(mGVel->mGrid));
        for(int64_t i = 0; i < (mParticles->mPosition).size(); ++i){
            Vec3d particlePosition = (mParticles->mPosition)[i];

            velInt.rungeKutta<4, openvdb::Vec3d>(dt, particlePosition); //4 runge kutta

            //Clamp to grig
            clampToGrid(particlePosition);

            // save particle
            (mParticles->mPosition)[i] = particlePosition;
        }
        */

    }

    inline void Solver::clampToGrid(Vec3d &vect)
    {
        vect.init( clamp(vect.x(), mMinBox.x(), mMaxBox.x()),
                    clamp(vect.y(), mMinBox.y(), mMaxBox.y()),
                    clamp(vect.z(), mMinBox.z(), mMaxBox.z()) );
    }

    inline LReal Solver::getComponentWeight(LReal val, uint8_t cxyz)
    {
       if (cxyz == 0)
       {
          return val;
       }
       else //cxyz == 1
       {
          return -val +1;
       }
    }

    void Solver::transferToGrid()
    {
        // init grid to save weights sum
        Grid<Vec3DGrid> *sum = new Grid<Vec3DGrid>(mVoxelSize);

        // clear all previous velocity values in mGVel
        mGVel->clear();

        Vec3i ijk[3];
        Vec3d fxyz[3];
        Vec3d component[3];

        // loop over all the particles
        for(int64_t i = 0; i < (mParticles->mPosition).size(); ++i)
        {
            Vec3d particlePosition = (mParticles->mPosition)[i];
            Vec3d cindexSpacePoint = mGVel->getWorldToIndex(particlePosition);
            Vec3i cijk = Vec3i( floor(cindexSpacePoint.x()), floor(cindexSpacePoint.y()), floor(cindexSpacePoint.z()) );
            Vec3d cfxyz = Vec3d( cindexSpacePoint.x() - floor(cindexSpacePoint.x()), cindexSpacePoint.y() - floor(cindexSpacePoint.y()), cindexSpacePoint.z() - floor(cindexSpacePoint.z()) ); //TODO: fix with floor
            // staggered translation
            Vec3d sparticlePosition = particlePosition - Vec3d(mVoxelSize/2.0, mVoxelSize/2.0, mVoxelSize/2.0);
            Vec3d sindexSpacePoint = mGVel->getWorldToIndex(sparticlePosition);
            Vec3i sijk = Vec3i( floor(sindexSpacePoint.x()), floor(sindexSpacePoint.y()), floor(sindexSpacePoint.z()) );
            Vec3d sfxyz = Vec3d( sindexSpacePoint.x() - floor(sindexSpacePoint.x()), sindexSpacePoint.y() - floor(sindexSpacePoint.y()), sindexSpacePoint.z() - floor(sindexSpacePoint.z()) ); //TODO: fix with floor

            /*
            L_LOG_DEBUG("-----------------------------------------------");
            L_LOG_DEBUG("particlePosition: " + to_string(particlePosition.x()) + ", " + to_string(particlePosition.y()) + ", " + to_string(particlePosition.z()));
            L_LOG_DEBUG("cindexSpacePoint: " + to_string(cindexSpacePoint.x()) + ", " + to_string(cindexSpacePoint.y()) + ", " + to_string(cindexSpacePoint.z()));
            L_LOG_DEBUG("cijk: " + to_string(cijk.x()) + ", " + to_string(cijk.y()) + ", " + to_string(cijk.z()));
            L_LOG_DEBUG("cfxyz: " + to_string(cfxyz.x()) + ", " + to_string(cfxyz.y()) + ", " + to_string(cfxyz.z()));
            L_LOG_DEBUG("sparticlePosition: " + to_string(sparticlePosition.x()) + ", " + to_string(sparticlePosition.y()) + ", " + to_string(sparticlePosition.z()));
            L_LOG_DEBUG("sindexSpacePoint: " + to_string(sindexSpacePoint.x()) + ", " + to_string(sindexSpacePoint.y()) + ", " + to_string(sindexSpacePoint.z()));
            L_LOG_DEBUG("sijk: " + to_string(sijk.x()) + ", " + to_string(sijk.y()) + ", " + to_string(sijk.z()));
            L_LOG_DEBUG("sfxyz: " + to_string(sfxyz.x()) + ", " + to_string(sfxyz.y()) + ", " + to_string(sfxyz.z()));
            */

            // prepare indexes vectors
            // X
            ijk[0].init(cijk.x(), sijk.y(), sijk.z());
            fxyz[0].init(cfxyz.x(), sfxyz.y(), sfxyz.z());
            component[0].init(1.0, 0.0, 0.0);
            // Y
            ijk[1].init(sijk.x(), cijk.y(), sijk.z());
            fxyz[1].init(sfxyz.x(), cfxyz.y(), sfxyz.z());
            component[1].init(0.0, 1.0, 0.0);
            // Z
            ijk[2].init(sijk.x(), sijk.y(), cijk.z());
            fxyz[2].init(sfxyz.x(), sfxyz.y(), cfxyz.z());
            component[2].init(0.0, 0.0, 1.0); 

            // loop on x, y, z
            for(uint8_t compIdx = 0; compIdx < 3; ++compIdx)
            {
                // loop all 8 vertex in voxel and accumulate
                // loop for x
                for(uint8_t cx = 0; cx < 2; ++cx)
                {
                    // loop for y
                    for(uint8_t cy = 0; cy < 2; ++cy)
                    {
                        // loop for z
                        for(uint8_t cz = 0; cz < 2; ++cz)
                        {

                            //LReal weight = ( 1 - fxyz[compIdx].x()*pow(-1,cx) -cx ) * ( 1 - fxyz[compIdx].y()*pow(-1,cy) -cy ) * ( 1 - fxyz[compIdx].y()*pow(-1,cz) -cz );
                            LReal weight = ( 1 - getComponentWeight(fxyz[compIdx].x(), cx) ) *
                                            ( 1 - getComponentWeight(fxyz[compIdx].y(), cy) ) *
                                            ( 1 - getComponentWeight(fxyz[compIdx].z(), cz) );

                            // applying formula same in book Fluid Simulation for Computer Graphics by Robert Bridson (second edition) at page 117
                            Vec3d vel = mGVel->getValue(ijk[compIdx].x()+cx, ijk[compIdx].y()+cy, ijk[compIdx].z()+cz) + weight * mParticles->mVelocity[i] * component[compIdx];
                            mGVel->setValue(vel, ijk[compIdx].x()+cx, ijk[compIdx].y()+cy, ijk[compIdx].z()+cz);
                            Vec3d sumVoxel = sum->getValue(ijk[compIdx].x()+cx, ijk[compIdx].y()+cy, ijk[compIdx].z()+cz) + weight * component[compIdx];
                            sum->setValue(sumVoxel, ijk[compIdx].x()+cx, ijk[compIdx].y()+cy, ijk[compIdx].z()+cz);

                        }
                    }
                }
            }

        }

        // denominator of furmula at page 117 (see comment above)
        for(int64_t i = mMinN.x(); i < mMaxN.x()+1; ++i)
            for(int64_t j = mMinN.y(); j < mMaxN.y()+1; ++j)
                for(int64_t k = mMinN.z(); k < mMaxN.z()+1; ++k)
                {
                    Vec3d vel = mGVel->getValue(i, j, k);
                    Vec3d sumVoxel = sum->getValue(i, j, k);

                    LReal xResult = sumVoxel.x() != 0 ? vel.x() / sumVoxel.x() : 0.0;
                    LReal yResult = sumVoxel.y() != 0 ? vel.y() / sumVoxel.y() : 0.0;
                    LReal zResult = sumVoxel.z() != 0 ? vel.z() / sumVoxel.z() : 0.0;

                    Vec3d result(xResult, yResult, zResult);
                    mGVel->setValue(result , i, j, k);

                }

        delete sum;
    }

    void Solver::saveVelocities()
    {
        mGVelSave->deepCopyFromGrid(mGVel);
    }

    void Solver::addGravity(LReal dt)
    {
        LReal dtg = dt * mGravity;

        for(int64_t i = mMinN.x(); i < mMaxN.x()+1; ++i)
            for(int64_t j = mMinN.y(); j < mMaxN.y()+1; ++j)
                for(int64_t k = mMinN.z(); k < mMaxN.z()+1; ++k)
                {
                    Vec3d vel = mGVel->getValue(i, j, k);

                    LReal xResult = vel.x();
                    LReal yResult = vel.y() - dtg;
                    LReal zResult = vel.z();

                    Vec3d result(xResult, yResult, zResult);
                    mGVel->setValue(result , i, j, k);
                }
    }

    void Solver::addExternalForces(LReal dt)
    {
        // TODO: implement in case of external forces: e.g. create a vortex
    }

    void Solver::identifyTypeVoxels()
    {
        // Loop on particle and where are particles mark as fluid the voxel
        // TODO: Solid case

        // Mark all as AIR
        for(int64_t i = mMinN.x(); i < mMaxN.x(); ++i)
            for(int64_t j = mMinN.y(); j < mMaxN.y(); ++j)
                for(int64_t k = mMinN.z(); k < mMaxN.z(); ++k)
                {
                    mGTypeVoxel->setValue(VoxelType::AIR, i, j, k);
                }


        for(int64_t i = 0; i < (mParticles->mPosition).size(); ++i)
        {
            Vec3d particlePosition = (mParticles->mPosition)[i];
            Vec3d cindexSpacePoint = mGVel->getWorldToIndex(particlePosition);
            Vec3i cijk = Vec3i( floor(cindexSpacePoint.x()), floor(cindexSpacePoint.y()), floor(cindexSpacePoint.z()) );
            mGTypeVoxel->setValue(VoxelType::FLUID, cijk.x(), cijk.y(), cijk.z());
            //L_LOG_DEBUG("FLUID: " + to_string(i) + " --> " + to_string(cijk.x()) + ", " + to_string(cijk.y()) + ", " + to_string(cijk.z()));
            //L_LOG_DEBUG("particlePosition: " + to_string(particlePosition.x()) + ", " + to_string(particlePosition.y()) + ", " + to_string(particlePosition.z()));
        }

    }

    void Solver::velocityExtrapolation()
    {
        // TODO
    }

    void Solver::boundaryConditions()
    {
        // TODO: for empty box only loop on boundary surfaces, to improve performance
        // TODO: implement for solid voxels

        for(int64_t i = mMinN.x(); i < mMaxN.x()+1; ++i)
            for(int64_t j = mMinN.y(); j < mMaxN.y()+1; ++j)
                for(int64_t k = mMinN.z(); k < mMaxN.z()+1; ++k)
                {
                    Vec3d vel = mGVel->getValue(i, j, k);

                    LReal xResult = vel.x();
                    LReal yResult = vel.y();
                    LReal zResult = vel.z();

                    if ( (i == mMinN.x()) || (i == mMaxN.x()+1) )
                        xResult = 0.0;
                    if ( (j == mMinN.y()) || (j == mMaxN.y()+1) )
                        yResult = 0.0;
                    if ( (k == mMinN.z()) || (k == mMaxN.z()+1) )
                        zResult = 0.0;

                    Vec3d result(xResult, yResult, zResult);
                    mGVel->setValue(result , i, j, k);
                }
    }

    void Solver::solvePressure()
    {

        computeDivergence();

        int64_t mNumX = mMaxN.x() - mMinN.x();
        int64_t mNumY = mMaxN.y() - mMinN.y();
        int64_t mNumZ = mMaxN.z() - mMinN.z();
        int M;
        int N;
        M = N = mNumX * mNumY * mNumZ; // tridiagonal symmetric matrix in CSR format with I,J,val,nz
        float *x = (float *)malloc(sizeof(float)*N); // x vector
        float *rhs = (float *)malloc(sizeof(float)*N); // b vector

        // Count all fluid voxels and populate x and rhs
        uint64_t numFluidVoxel = 0;
        uint64_t idxMat = 0;
        for(int64_t i = mMinN.x(); i < mMaxN.x(); ++i)
            for(int64_t j = mMinN.y(); j < mMaxN.y(); ++j)
                for(int64_t k = mMinN.z(); k < mMaxN.z(); ++k)
                {
                    // count all fluid voxels
                    if ( mGTypeVoxel->getValue(i, j, k) == VoxelType::FLUID )
                    {
                        numFluidVoxel++;
                    }

                    // populate x and rhs
                    LReal divergenceVal = mGDivergence->getValue(i, j, k);
                    rhs[idxMat] = divergenceVal;
                    x[idxMat] = 0.0;
                    idxMat++;
                }
        // maximize the array elements using numFluidVoxel => nz

        if ( numFluidVoxel == 0 )
        {
            L_LOG_WARN("numFluidVoxel is 0, skip solvePressure");
            return;
        }

        // A*x = b (b is rhs)
        int *I;
        int *J;
        float *val;
        int nz;
        nz = numFluidVoxel * 7; //Maximum: 7 elements per row 
        I = (int *)malloc(sizeof(int)*(N+1));
        J = (int *)malloc(sizeof(int)*nz);
        val = (float *)malloc(sizeof(float)*nz);

        //Populate val, I, J using sparse CSR format
        int64_t idxVal = 0; //TODO: remove it is reduntant
        int64_t idxI = 0;
        int64_t idxJ = 0;
        //idxMat = 0;
        for(int64_t i = mMinN.x(); i < mMaxN.x(); ++i)
            for(int64_t j = mMinN.y(); j < mMaxN.y(); ++j)
                for(int64_t k = mMinN.z(); k < mMaxN.z(); ++k)
                {

                    // check error
                    //if ( (idxVal != idxJ) || (idxJ >= nz) )
                    //{
                    //    L_LOG_ERROR("Error: array index is greater than nz: " + to_string(nz) + " idxJ:" + to_string(idxJ));
                    //    L_LOG_ERROR("idxVal: " + to_string(idxVal) + " idxJ:" + to_string(idxJ));
                    //    exit(-1);
                    //}

                    I[idxI++] = idxJ;

                    // sparse matrix has row with elements non zero only in fluid voxels
                    if ( mGTypeVoxel->getValue(i, j, k) == VoxelType::FLUID )
                    {

                        if ( ( mGTypeVoxel->getValue(i - 1, j, k) == VoxelType::FLUID ) && (i - 1 >= mMinN.x()) ) //TODO cambiare ordine condizioni
                        {
                            int64_t idx_col = (i -1 - mMinN.x())*mNumY*mNumZ + (j - mMinN.y())*mNumZ + (k - mMinN.z());
                            //printf("i-1: %d \n", idx_col);
                            val[idxVal++] = -1;
                            J[idxJ++] = idx_col;
                        }

                        if ( ( mGTypeVoxel->getValue(i, j - 1, k) == VoxelType::FLUID ) && (j - 1 >= mMinN.y()) )
                        {
                            int64_t idx_col = (i - mMinN.x())*mNumY*mNumZ + (j - 1 - mMinN.y())*mNumZ + (k - mMinN.z());
                            //printf("j-1: %d \n", idx_col);
                            val[idxVal++] = -1;
                            J[idxJ++] = idx_col;
                        }

                        if ( ( mGTypeVoxel->getValue(i, j, k - 1) == VoxelType::FLUID ) && (k - 1 >= mMinN.z()) )
                        {
                            int64_t idx_col = (i - mMinN.x())*mNumY*mNumZ + (j - mMinN.y())*mNumZ + (k - 1 - mMinN.z());
                            //printf("k-1: %d \n", idx_col);
                            val[idxVal++] = -1;
                            J[idxJ++] = idx_col;
                        }

                        //printf("ROW: %d \n", idx_row);
                        // Diagonal
                        LReal omega = 6.0;
                        if ( i <= mMinN.x() )
                            omega -= 1.0;
                        if ( i >= mMaxN.x() -1 )
                            omega -= 1.0;
                        if ( j <= mMinN.y() )
                            omega -= 1.0;
                        if ( j >= mMaxN.y() -1 )
                            omega -= 1.0;
                        if ( k <= mMinN.z() )
                            omega -= 1.0;
                        if ( k >= mMaxN.z() -1 )
                            omega -= 1.0;
                        if ( omega != 0.0 )
                        {
                            int64_t idx_col = (i - mMinN.x())*mNumY*mNumZ + (j - mMinN.y())*mNumZ + (k - mMinN.z());
                            //printf("idx_col: %d  omega: %f\n", idx_col, omega);
                            val[idxVal++] = omega;
                            J[idxJ++] = idx_col;
                        }

                        if ( ( mGTypeVoxel->getValue(i, j, k + 1) == VoxelType::FLUID ) && (k + 1 < mMaxN.z()) )
                        {
                            int64_t idx_col = (i - mMinN.x())*mNumY*mNumZ + (j - mMinN.y())*mNumZ + (k + 1 - mMinN.z());
                            //printf("k+1: %d \n", idx_col);
                            val[idxVal++] = -1;
                            J[idxJ++] = idx_col;
                        }

                        if ( ( mGTypeVoxel->getValue(i, j + 1, k) == VoxelType::FLUID ) && (j + 1 < mMaxN.y()) )
                        {
                            int64_t idx_col = (i - mMinN.x())*mNumY*mNumZ + (j + 1 - mMinN.y())*mNumZ + (k - mMinN.z());
                            //printf("j+1: %d \n", idx_col);
                            val[idxVal++] = -1;
                            J[idxJ++] = idx_col;
                        }

                        if ( ( mGTypeVoxel->getValue(i + 1, j, k) == VoxelType::FLUID ) && (i + 1 < mMaxN.x()) )
                        {
                            int64_t idx_col = (i + 1 - mMinN.x())*mNumY*mNumZ + (j - mMinN.y())*mNumZ + (k - mMinN.z());
                            //printf("i+1: %d \n", idx_col);
                            val[idxVal++] = -1;
                            J[idxJ++] = idx_col;
                        }

                    }

                }

        // for matrix in CSR format the last entry contains nnz: number of not zero values of val 
        I[N] = idxVal;
        nz = idxVal; // use the actual number of not zero values of val

        // everything is ready...

        // execute cuda solver
        spareSolverConjugateGradient(I, J, val, M, N, nz, x, rhs);

        // populate mGP: pressure grid
        mGP->clear();
        idxMat = 0;
        for(int64_t i = mMinN.x(); i < mMaxN.x(); ++i)
            for(int64_t j = mMinN.y(); j < mMaxN.y(); ++j)
                for(int64_t k = mMinN.z(); k < mMaxN.z(); ++k)
                {
                    if ( mGTypeVoxel->getValue(i, j, k) == VoxelType::FLUID )
                    {
                        mGP->setValue(x[idxMat], i, j, k);
                    }
                    idxMat++;
                }

        // free memory
        free(I);
        free(J);
        free(val);
        free(x);
        free(rhs);

        addGradient();

    }

    void Solver::computeDivergence()
    {

        /*
        // Divergence calculated by OpenVDB
        openvdb::GridClass prevGridClass = mGVel->mGrid->getGridClass();

        mGVel->mGrid->setTransform(openvdb::math::Transform::createLinearTransform(1.0)); //That's very important otherwise divergence is transformed

        mGVel->mGrid->setGridClass( openvdb::GridClass::GRID_STAGGERED ); // TODO: is it right?
        mGDivergence->mGrid = openvdb::tools::divergence(*(mGVel->mGrid));

        // Set divergence 0.0 for voxels not VoxelType::FLUID
        for(int64_t i = mMinN.x(); i < mMaxN.x(); ++i)
            for(int64_t j = mMinN.y(); j < mMaxN.y(); ++j)
                for(int64_t k = mMinN.z(); k < mMaxN.z(); ++k)
                {
                    if ( mGTypeVoxel->getValue(i, j, k) != VoxelType::FLUID )
                    {
                        mGDivergence->setValue(0.0, i, j, k);
                    }
                }

        mGVel->mGrid->setGridClass( prevGridClass ); // set back previous class not staggered
        mGVel->mGrid->setTransform(openvdb::math::Transform::createLinearTransform(mGVel->mVoxelSize)); //set back transformation as previous
        */

        // Divergence calculated without OpenVDB and applying formula
        mGDivergence->clear();
        for(int64_t i = mMinN.x(); i < mMaxN.x(); ++i)
            for(int64_t j = mMinN.y(); j < mMaxN.y(); ++j)
                for(int64_t k = mMinN.z(); k < mMaxN.z(); ++k)
                {
                    if ( mGTypeVoxel->getValue(i, j, k) == VoxelType::FLUID )
                    {
                        Vec3d velV = mGVel->getValue(i, j, k);
                        Vec3d velX = mGVel->getValue(i+1, j, k);
                        Vec3d velY = mGVel->getValue(i, j+1, k);
                        Vec3d velZ = mGVel->getValue(i, j, k+1);

                        LReal result = velX.x() - velV.x() + velY.y() - velV.y() + velZ.z() - velV.z();

                        mGDivergence->setValue(result , i, j, k);
                    }
                }

    }

    void Solver::addGradient()
    {
        //L_LOG_DEBUG("Solver::addGradient");
        for(int64_t i = mMinN.x()+1; i < mMaxN.x()+1; ++i)
            for(int64_t j = mMinN.y()+1; j < mMaxN.y()+1; ++j)
                for(int64_t k = mMinN.z()+1; k < mMaxN.z()+1; ++k)
                {

                    Vec3d vel = mGVel->getValue(i, j, k);

                    LReal xResult = vel.x();
                    LReal yResult = vel.y();
                    LReal zResult = vel.z();

                    // TODO: change completely all those conditions, improving performance and algorithm

                    if ( ((mGTypeVoxel->getValue(i-1, j, k) == VoxelType::AIR) && (mGTypeVoxel->getValue(i, j, k) == VoxelType::FLUID)) 
                        || ((mGTypeVoxel->getValue(i-1, j, k) == VoxelType::FLUID) && (mGTypeVoxel->getValue(i, j, k) == VoxelType::AIR))
                        || ((mGTypeVoxel->getValue(i-1, j, k) == VoxelType::FLUID) && (mGTypeVoxel->getValue(i, j, k) == VoxelType::FLUID)) )
                    {
                        xResult += mGP->getValue(i, j, k) - mGP->getValue(i-1, j, k);
                        //L_LOG_DEBUG("Gradient X : " + to_string( mGP->getValue(i, j, k) - mGP->getValue(i-1, j, k) ) + " --> " + to_string(xResult));
                    }

                    if ( ((mGTypeVoxel->getValue(i, j-1, k) == VoxelType::AIR) && (mGTypeVoxel->getValue(i, j, k) == VoxelType::FLUID)) 
                        || ((mGTypeVoxel->getValue(i, j-1, k) == VoxelType::FLUID) && (mGTypeVoxel->getValue(i, j, k) == VoxelType::AIR)) 
                        || ((mGTypeVoxel->getValue(i, j-1, k) == VoxelType::FLUID) && (mGTypeVoxel->getValue(i, j, k) == VoxelType::FLUID)) )
                    {
                        yResult += mGP->getValue(i, j, k) - mGP->getValue(i, j-1, k);
                    }

                    if ( ((mGTypeVoxel->getValue(i, j, k-1) == VoxelType::AIR) || (mGTypeVoxel->getValue(i, j, k) == VoxelType::FLUID)) 
                        || ((mGTypeVoxel->getValue(i, j, k-1) == VoxelType::FLUID) || (mGTypeVoxel->getValue(i, j, k) == VoxelType::AIR)) 
                        || ((mGTypeVoxel->getValue(i, j, k-1) == VoxelType::FLUID) || (mGTypeVoxel->getValue(i, j, k) == VoxelType::FLUID)) )
                    {
                        zResult += mGP->getValue(i, j, k) - mGP->getValue(i, j, k-1);
                    }

                    Vec3d result(xResult, yResult, zResult);
                    mGVel->setValue(result , i, j, k);

                }
    }

    void Solver::saveVelocitiesUpdate()
    {
        //L_LOG_DEBUG("Solver::saveVelocitiesUpdate start");
        for(int64_t i = mMinN.x(); i < mMaxN.x()+1; ++i)
            for(int64_t j = mMinN.y(); j < mMaxN.y()+1; ++j)
                for(int64_t k = mMinN.z(); k < mMaxN.z()+1; ++k)
                {
                    Vec3d vel = mGVel->getValue(i, j, k);
                    Vec3d velSave = mGVelSave->getValue(i, j, k);

                    LReal xResult = vel.x() - velSave.x();
                    LReal yResult = vel.y() - velSave.y();
                    LReal zResult = vel.z() - velSave.z();

                    Vec3d result(xResult, yResult, zResult);
                    mGVelSave->setValue(result , i, j, k);
                }

    }

    void Solver::updateParticlesVelocity()
    {
        for(int64_t i = 0; i < (mParticles->mPosition).size(); ++i)
        {
            Vec3d particlePosition = (mParticles->mPosition)[i];

            Vec3d indexSpacePoint = mGVel->getWorldToIndex(particlePosition); // particle grid index space point

            // Trilinear interpolation
            Vec3d gUpdate = tools::BoxSampler::sample(mGVelSave->mGrid->tree(), indexSpacePoint); //TODO see Solver::moveParticlesInGrid: or use StaggeredBoxSampler? Dont think so!! or PointSampler or QuadraticSampler
            Vec3d gVel = tools::BoxSampler::sample(mGVel->mGrid->tree(), indexSpacePoint);

            // FLIP
            //mParticles->mVelocity[i] = mParticles->mVelocity[i] + gUpdate;

            // PIC
            mParticles->mVelocity[i] = gVel;

            // TODO make velocity part of FLIP and part of PIC with weights

        }
    }


}

