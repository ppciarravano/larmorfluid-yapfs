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

#include "log.h"

namespace yapfs
{

    static log4cplus::Logger _logger;

    log4cplus::Logger & getLogger()
    {
        return _logger;
    }

    void logInit()
    {
        log4cplus::Logger rootLogger = log4cplus::Logger::getRoot();
        if (rootLogger.getAllAppenders().empty())
        {
            log4cplus::BasicConfigurator::doConfigure();
#ifdef LOG_DEBUG
            rootLogger.setLogLevel(log4cplus::DEBUG_LOG_LEVEL);
#else
            rootLogger.setLogLevel(log4cplus::INFO_LOG_LEVEL); // DEBUG_LOG_LEVEL, INFO_LOG_LEVEL
#endif
        }

        // Or use:
        //log4cplus::BasicConfigurator config;
        //config.configure();

        _logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("main"));

        LOG4CPLUS_INFO(_logger, LOG4CPLUS_TEXT("logInit initialized"));
    }

}