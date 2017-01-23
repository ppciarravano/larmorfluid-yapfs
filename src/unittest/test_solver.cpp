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

#include <cppunit/extensions/HelperMacros.h>
#include <tbb/tick_count.h>

class TestCaseSolver : public CppUnit::TestCase {

    public:

        CPPUNIT_TEST_SUITE( TestCaseSolver );
        CPPUNIT_TEST( testSolver );
        CPPUNIT_TEST_SUITE_END();

        // returns the numeric error
        float runSolver(uint64_t gridDim)
        {
            L_LOG_INFO("Run Solver on grid dimension: " + to_string( gridDim ) + "x" + to_string( gridDim ) + "x" + to_string( gridDim ) );

            //TODO: build properly A and b in order to have always a solution

            // A*x = b (b is rhs)
            // cubic grid: same dimension gridDim in x, y and z a axes
            int N;
            N = gridDim * gridDim * gridDim; // tridiagonal symmetric matrix in CSR format with I,J,val,nz
            int *I;
            int *J;
            float *val;
            int nz;
            nz = N * 7; //number of elements in matrix: Maximum: 7 elements per row
            I = (int *)malloc(sizeof(int)*(N+1));
            J = (int *)malloc(sizeof(int)*nz);
            val = (float *)malloc(sizeof(float)*nz);
            float *x = (float *)malloc(sizeof(float)*N); // x vector
            float *rhs = (float *)malloc(sizeof(float)*N); // b vector

            for(int64_t i = 0; i < N; ++i)
            {
                //rhs[i] = yapfs::getRnd_0_1() * 10.0;
                rhs[i] = 0.0;
                x[i] = 0.0; // init
            }

            //Populate val, I, J using sparse CSR format
            int64_t idxI = 0;
            int64_t idxJ = 0;
            for(int64_t i = 0; i < gridDim; ++i)
                for(int64_t j = 0; j < gridDim; ++j)
                    for(int64_t k = 0; k < gridDim; ++k)
                    {
                        I[idxI++] = idxJ;
                        // sparse matrix has row with elements non zero only in fluid voxels
                        if (j < 10)
                        {

                            if ( i - 1 >= 0 )
                            {
                                int64_t idx_col = (i -1)*gridDim*gridDim + (j)*gridDim + (k);
                                val[idxJ] = -1;
                                J[idxJ++] = idx_col;
                            }

                            if ( j - 1 >= 0 )
                            {
                                int64_t idx_col = (i)*gridDim*gridDim + (j - 1)*gridDim + (k);
                                val[idxJ] = -1;
                                J[idxJ++] = idx_col;
                            }

                            if ( k - 1 >= 0 )
                            {
                                int64_t idx_col = (i)*gridDim*gridDim + (j)*gridDim + (k - 1);
                                val[idxJ] = -1;
                                J[idxJ++] = idx_col;
                            }

                            // Diagonal
                            LReal omega = 6.0;
                            if ( i <= 0 )
                                omega -= 1.0;
                            if ( i >= gridDim -1 )
                                omega -= 1.0;
                            if ( j <= 0 )
                                omega -= 1.0;
                            if ( j >= gridDim -1 )
                                omega -= 1.0;
                            if ( k <= 0 )
                                omega -= 1.0;
                            if ( k >= gridDim -1 )
                                omega -= 1.0;
                            if ( omega != 0.0 )
                            {
                                int64_t idx_col = (i)*gridDim*gridDim + (j)*gridDim + (k);
                                val[idxJ] = omega;
                                J[idxJ++] = idx_col;
                            }

                            if ( k + 1 < gridDim )
                            {
                                int64_t idx_col = (i)*gridDim*gridDim + (j)*gridDim + (k + 1);
                                val[idxJ] = -1;
                                J[idxJ++] = idx_col;
                            }

                            if (j + 1 < gridDim )
                            {
                                int64_t idx_col = (i)*gridDim*gridDim + (j + 1)*gridDim + (k);
                                val[idxJ] = -1;
                                J[idxJ++] = idx_col;
                            }

                            if ( i + 1 < gridDim )
                            {
                                int64_t idx_col = (i + 1)*gridDim*gridDim + (j)*gridDim + (k);
                                val[idxJ] = -1;
                                J[idxJ++] = idx_col;
                            }

                        }

                    }
            I[N] = idxJ;
            nz = idxJ;

            // everything is ready...

            // execute cuda solver
            float error = yapfs::spareSolverConjugateGradient(I, J, val, N, N, nz, x, rhs);

           // free memory
            free(I);
            free(J);
            free(val);
            free(x);
            free(rhs);

            return error;

        }

        void testSolver()
        {
            printf("\n\n");

            float total_error = 0.0;
            for(int64_t i = 10; i < 400; i+=10)
            {
                tbb::tick_count t0 = tbb::tick_count::now();

                float error = runSolver(i);
                total_error += error;

                tbb::tick_count t1 = tbb::tick_count::now();
                tbb::tick_count::interval_t time_test = t1 - t0;
                double total_millisec = 1000.0*time_test.seconds();
                L_LOG_INFO("TOTAL TIME IN MILLIESEC: " + to_string( total_millisec ) );
                L_LOG_INFO("TOTAL ERROR: " + to_string( total_error ) );
            }

            CPPUNIT_ASSERT( total_error < 10.0);

        }

};

CPPUNIT_TEST_SUITE_REGISTRATION(TestCaseSolver);
