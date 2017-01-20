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

#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TextTestProgressListener.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <tbb/tick_count.h>

#include "main_test.h"

namespace yapfs
{

    bool runTest()
    {
        L_LOG_INFO("RUN TEST");

        try
        {

            //CppUnit::TextUi::TestRunner runner;
            //runner.addTest( ComplexNumberTest::runTest() );
            //bool wasSuccessful =  runner.run();
            //return wasSuccessful;

            CppUnit::TextUi::TestRunner runner;
            CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();
            runner.addTest( registry.makeTest() );

            CppUnit::TestResult controller;
            CppUnit::TestResultCollector result;
            controller.addListener(&result);

            // Verbose output
            CppUnit::BriefTestProgressListener vProgress;
            controller.addListener(&vProgress);

            // Not Verbose output
            //CppUnit::TextTestProgressListener progress;
            //controller.addListener(&progress);

            runner.run(controller, "TestCaseSolver");

            CppUnit::CompilerOutputter outputter(&result, std::cerr);
            outputter.write();

            return result.wasSuccessful() ? EXIT_SUCCESS : EXIT_FAILURE;

        }
        catch (std::exception& e)
        {
            std::cerr << "runTest exception Error: " << e.what() << std::endl;
            return EXIT_FAILURE;
        }

    }

}
