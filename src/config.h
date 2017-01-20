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

#ifndef CONFIG_H_
#define CONFIG_H_

#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <fstream>
#include <cstdint>

#include <boost/program_options.hpp>

#include "common.h"
#include "log.h"

#define YAPFS_CONFIG_FILENAME "yapfs.ini"

//using namespace boost::program_options;

namespace yapfs
{

    boost::program_options::variables_map & getConfigVariablesMap();

    template<typename T>
    T getConfig(std::string configName)
    {
        try
        {
            const boost::program_options::variable_value value = getConfigVariablesMap()[configName];
            T result = value.as<T>();

#ifdef LOG_DEBUG
            std::ostringstream _buf;
            _buf << result;
            std::string debugValue = _buf.str();
            L_LOG_DEBUG("getConfig: " + configName + " = " + debugValue);
#endif

            return result;
        }
        catch (std::exception& e)
        {
            std::cout << "Exception getConfig: message: " << e.what() << std::endl;
            exit(0);
        }
    }

    void configInit(int argc, char** argv);

}

#endif /* CONFIG_H_ */