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

#ifndef LOG_H_
#define LOG_H_

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/configurator.h>

#include "common.h"

// very helpful defines copied from OpenVDB source code, and just changed a bit, thanks DWA
#define LARMOR_LOG(level, message) \
    { \
        if (yapfs::getLogger().isEnabledFor(log4cplus::level##_LOG_LEVEL)) { \
            std::ostringstream _buf; \
            _buf << message; \
            yapfs::getLogger().forcedLog(log4cplus::level##_LOG_LEVEL, _buf.str(), __FILE__, __LINE__); \
        } \
    };

#define L_LOG_INFO(message)   LARMOR_LOG(INFO, message)
#define L_LOG_WARN(message)   LARMOR_LOG(WARN, message)
#define L_LOG_ERROR(message)  LARMOR_LOG(ERROR, message)
#define L_LOG_FATAL(message)  LARMOR_LOG(FATAL, message)
#ifdef LOG_DEBUG
#define L_LOG_DEBUG(message)  LARMOR_LOG(DEBUG, message)
#else
#define L_LOG_DEBUG(message)
#endif

namespace yapfs
{

    log4cplus::Logger & getLogger();

    void logInit();

}

#endif /* LOG_H_ */