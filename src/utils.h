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

#ifndef UTILS_H_
#define UTILS_H_

#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <chrono>
#include <cstdlib>
#include <cstdint>
#include <random>

#include "common.h"
#include "log.h"
#include "config.h"

using namespace std;

namespace yapfs
{

    LReal getRnd_0_1();

    template<class T>
    inline T sqr(const T &val)
    {
        return val * val;
    }

/*
    // use std::min
    template<class T>
    inline T min(const T &a1, const T &a2)
    {
        if(a1 < a2)
            return a1;
        else
            return a2;
    }
*/

    template<class T>
    inline T min(const T &a1, const T &a2, const T &a3)
    {
        if(a1 < a2)
            return min(a1, a3);
        else
            return min(a2, a3);
    }

/*
    // use std::max
    template<class T>
    inline T max(const T &a1, const T &a2)
    {
        if(a1 > a2)
            return a1;
        else
            return a2;
    }
*/

    template<class T>
    inline T max(const T &a1, const T &a2, const T &a3)
    {
        if(a1 > a2)
            return max(a1, a3);
        else
            return max(a2, a3);
    }

    // Not used for now
    template<class T>
    inline T clamp(T a, T lower, T upper)
    {
        if(a < lower)
            return lower;
        else if(a > upper)
            return upper;
        else
            return a;
    }


}

#endif /* UTILS_H_ */