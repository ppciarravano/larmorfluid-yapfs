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

#ifndef SPARSE_SOLVER_H_
#define SPARSE_SOLVER_H_

#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <chrono>
#include <cstdlib>
#include <cstdint>

#include <sys/time.h>

#include "log.h"
#include "config.h"

using namespace std;

namespace yapfs
{

    float spareSolverConjugateGradient(int *I, int *J, float *val, int M, int N, int nz, float *x, float *rhs);

}

#endif /* SPARSE_SOLVER_H_ */