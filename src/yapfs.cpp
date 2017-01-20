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

#include "yapfs.h"

//#include <openvdb/math/Stats.h>
//#include <openvdb/tools/Statistics.h>
//#include <openvdb/tools/ValueTransformer.h>

using namespace yapfs;


void yapfs_solver()
{
    L_LOG_INFO("Starting YAPFS solver");

    // create istance of solver
    Solver solver = Solver();
    solver.initGrids();

    // Execute the solver
    solver.execute();

    // Open viewer
    openViewer(solver);

}

int yapfs_test()
{
    L_LOG_INFO("Starting YAPFS test");

    int result = runTest();

    return result;
}

int main(int argc, char** argv)
{

    std::cout << "LarmorFluid or YAPFS (Yet Another PIC/FLIP Solver) by Pier Paolo Ciarravano (http://www.larmor.com) - Dec.2016-Jan.2017\n" << std::endl;

    // init log
    yapfs::logInit();

    // init config
    yapfs::configInit(argc, argv);

    std::string action = yapfs::getConfig<std::string>("action");

    int result = EXIT_SUCCESS;
    if (action == "solver")
    {
        yapfs_solver();
    }
    else if (action == "test")
    {
        // use this command to execute cppunit tests: ./yapfs --action test
        result = yapfs_test();
    }

    return result;
}

