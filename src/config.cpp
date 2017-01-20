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

#include "config.h"

namespace yapfs
{

    static boost::program_options::variables_map _configVariablesMap;

    boost::program_options::variables_map & getConfigVariablesMap()
    {
        return _configVariablesMap;
    }

    void configInit(int argc, char** argv)
    {
        try
        {

            L_LOG_INFO("Reading command line");
            std::string configFile = YAPFS_CONFIG_FILENAME;
            std::string action = "solver"; //default action: TODO use enum

            // command line mapping
            boost::program_options::options_description descArgv("YAPFS Command line options");
            descArgv.add_options() ("help", "help message");
            descArgv.add_options() ("action", boost::program_options::value<std::string>(&action), "main action");
            descArgv.add_options() ("config", boost::program_options::value<std::string>(&configFile), "input config file");

            // parse command line
            boost::program_options::variables_map commandLineVariablesMap;
            boost::program_options::store(boost::program_options::parse_command_line(argc, argv, descArgv), commandLineVariablesMap);
            boost::program_options::notify(commandLineVariablesMap);

            if (commandLineVariablesMap.count("help"))
            {
                std::cout << descArgv << std::endl;
                exit(0);
            }

            L_LOG_INFO("Reading Configs file");

            L_LOG_INFO("Config file name: " + configFile);

            // Config type mapping
            boost::program_options::options_description desc("YAPFS CONFIGS FROM FILE");
            desc.add_options() ("action",       boost::program_options::value<std::string>()->default_value(action));
            desc.add_options() ("gravity",      boost::program_options::value<LReal>());
            desc.add_options() ("voxel_size",   boost::program_options::value<LReal>());
            desc.add_options() ("min_x",        boost::program_options::value<LReal>());
            desc.add_options() ("min_y",        boost::program_options::value<LReal>());
            desc.add_options() ("min_z",        boost::program_options::value<LReal>());
            desc.add_options() ("max_x",        boost::program_options::value<LReal>());
            desc.add_options() ("max_y",        boost::program_options::value<LReal>());
            desc.add_options() ("max_z",        boost::program_options::value<LReal>());
            desc.add_options() ("frames_per_sec", boost::program_options::value<uint32_t>());
            desc.add_options() ("num_frames",     boost::program_options::value<uint32_t>());

            // reading configs
            std::ifstream settings_file( configFile.c_str() );
            _configVariablesMap = boost::program_options::variables_map();
            boost::program_options::store(boost::program_options::parse_config_file( settings_file , desc, true ), _configVariablesMap);
            settings_file.close();

        }
        catch (std::exception& e)
        {
            std::cout << "Exception configInit: message: " << e.what() << std::endl;
            exit(0);
        }

    }
}
