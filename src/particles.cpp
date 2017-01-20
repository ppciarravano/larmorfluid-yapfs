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

#include "particles.h"

namespace yapfs
{

    // TODO: rewrite completely using Dneg's OpenVDBPoints

    Particles::Particles(LReal voxelSize)
    {
        mVoxelSize = voxelSize;
    }

    // add 8 particles
    void Particles::addParticlesInVoxel(Vec3d worldVoxelCenter)
    {
        LReal ds = mVoxelSize / 2.0;

        // TODO: put in place a better solution with jitter costant
        //mPosition.push_back(Vec3d( worldVoxelCenter.x(), worldVoxelCenter.y(), worldVoxelCenter.z() ));
        
        mPosition.push_back(Vec3d( worldVoxelCenter.x() + ds * getRnd_0_1(), worldVoxelCenter.y() + ds * getRnd_0_1(), worldVoxelCenter.z() + ds * getRnd_0_1() ));
        mPosition.push_back(Vec3d( worldVoxelCenter.x() + ds * getRnd_0_1(), worldVoxelCenter.y() + ds * getRnd_0_1(), worldVoxelCenter.z() - ds * getRnd_0_1() ));
        mPosition.push_back(Vec3d( worldVoxelCenter.x() + ds * getRnd_0_1(), worldVoxelCenter.y() - ds * getRnd_0_1(), worldVoxelCenter.z() + ds * getRnd_0_1() ));
        mPosition.push_back(Vec3d( worldVoxelCenter.x() - ds * getRnd_0_1(), worldVoxelCenter.y() + ds * getRnd_0_1(), worldVoxelCenter.z() + ds * getRnd_0_1() ));
        mPosition.push_back(Vec3d( worldVoxelCenter.x() + ds * getRnd_0_1(), worldVoxelCenter.y() - ds * getRnd_0_1(), worldVoxelCenter.z() - ds * getRnd_0_1() ));
        mPosition.push_back(Vec3d( worldVoxelCenter.x() - ds * getRnd_0_1(), worldVoxelCenter.y() - ds * getRnd_0_1(), worldVoxelCenter.z() + ds * getRnd_0_1() ));
        mPosition.push_back(Vec3d( worldVoxelCenter.x() - ds * getRnd_0_1(), worldVoxelCenter.y() + ds * getRnd_0_1(), worldVoxelCenter.z() - ds * getRnd_0_1() ));
        mPosition.push_back(Vec3d( worldVoxelCenter.x() - ds * getRnd_0_1(), worldVoxelCenter.y() - ds * getRnd_0_1(), worldVoxelCenter.z() - ds * getRnd_0_1() ));


    }

}
