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

#include "sparse_solver.h"

// Using updated (v2) interfaces to cublas
#include <cuda_runtime.h>
#include <cusparse.h>
#include <cublas_v2.h>

// Utilities and system includes
#include <helper_cuda.h> 


namespace yapfs
{
    //TODO: use CUSPARSE_MATRIX_TYPE_SYMMETRIC for better memory performance

    // Solve Ax = b using cuda conjugate gradient cuSPARSE API
    // A is the CSR matrix: see http://docs.nvidia.com/cuda/cusparse/#compressed-sparse-row-format-csr for CSR format
    // b is rhs
    // returns the numeric error
    float spareSolverConjugateGradient(int *I, int *J, float *val, int M, int N, int nz, float *x, float *rhs)
    {
        // take input as a tridiagonal symmetric matrix MxN in CSR format with I, J, val and nz
        const float tol = 1e-5f;
        const int max_iter = 10000;
        float a, b, na, r0, r1;
        int *d_col, *d_row;
        float *d_val, *d_x, dot;
        float *d_r, *d_p, *d_Ax;
        int k;
        float alpha, beta, alpham1;

        // This will pick the best possible CUDA capable device
        cudaDeviceProp deviceProp;
        int argc = 1;
        char *argv[1];
        int devID = findCudaDevice(argc, (const char **)argv);

        if (devID < 0)
        {
            printf("exiting: devID < 0\n");
            exit(-1);
        }

        checkCudaErrors(cudaGetDeviceProperties(&deviceProp, devID));

        // Statistics about the GPU device
        printf("> GPU device has %d Multi-Processors, SM %d.%d compute capabilities\n",
               deviceProp.multiProcessorCount, deviceProp.major, deviceProp.minor);

        int version = (deviceProp.major * 0x10 + deviceProp.minor);

        if (version < 0x11)
        {
            printf("Requires a minimum CUDA compute 1.1 capability\n");
            exit(-1);
        }

        /* Get handle to the CUBLAS context */
        cublasHandle_t cublasHandle = 0;
        cublasStatus_t cublasStatus;
        cublasStatus = cublasCreate(&cublasHandle);

        checkCudaErrors(cublasStatus);

        /* Get handle to the CUSPARSE context */
        cusparseHandle_t cusparseHandle = 0;
        cusparseStatus_t cusparseStatus;
        cusparseStatus = cusparseCreate(&cusparseHandle);

        checkCudaErrors(cusparseStatus);

        cusparseMatDescr_t descr = 0;
        cusparseStatus = cusparseCreateMatDescr(&descr);

        checkCudaErrors(cusparseStatus);

        cusparseSetMatType(descr,CUSPARSE_MATRIX_TYPE_GENERAL);
        cusparseSetMatIndexBase(descr,CUSPARSE_INDEX_BASE_ZERO);

        checkCudaErrors(cudaMalloc((void **)&d_col, nz*sizeof(int)));
        checkCudaErrors(cudaMalloc((void **)&d_row, (N+1)*sizeof(int)));
        checkCudaErrors(cudaMalloc((void **)&d_val, nz*sizeof(float)));
        checkCudaErrors(cudaMalloc((void **)&d_x, N*sizeof(float)));
        checkCudaErrors(cudaMalloc((void **)&d_r, N*sizeof(float)));
        checkCudaErrors(cudaMalloc((void **)&d_p, N*sizeof(float)));
        checkCudaErrors(cudaMalloc((void **)&d_Ax, N*sizeof(float)));

        cudaMemcpy(d_col, J, nz*sizeof(int), cudaMemcpyHostToDevice);
        cudaMemcpy(d_row, I, (N+1)*sizeof(int), cudaMemcpyHostToDevice);
        cudaMemcpy(d_val, val, nz*sizeof(float), cudaMemcpyHostToDevice);
        cudaMemcpy(d_x, x, N*sizeof(float), cudaMemcpyHostToDevice);
        cudaMemcpy(d_r, rhs, N*sizeof(float), cudaMemcpyHostToDevice);

        alpha = 1.0;
        alpham1 = -1.0;
        beta = 0.0;
        r0 = 0.;

        cusparseScsrmv(cusparseHandle,CUSPARSE_OPERATION_NON_TRANSPOSE, N, N, nz, &alpha, descr, d_val, d_row, d_col, d_x, &beta, d_Ax);

        cublasSaxpy(cublasHandle, N, &alpham1, d_Ax, 1, d_r, 1);
        cublasStatus = cublasSdot(cublasHandle, N, d_r, 1, d_r, 1, &r1);

        k = 1;

        while (r1 > tol*tol && k <= max_iter)
        {
            if (k > 1)
            {
                b = r1 / r0;
                cublasStatus = cublasSscal(cublasHandle, N, &b, d_p, 1);
                cublasStatus = cublasSaxpy(cublasHandle, N, &alpha, d_r, 1, d_p, 1);
            }
            else
            {
                cublasStatus = cublasScopy(cublasHandle, N, d_r, 1, d_p, 1);
            }

            cusparseScsrmv(cusparseHandle, CUSPARSE_OPERATION_NON_TRANSPOSE, N, N, nz, &alpha, descr, d_val, d_row, d_col, d_p, &beta, d_Ax);
            cublasStatus = cublasSdot(cublasHandle, N, d_p, 1, d_Ax, 1, &dot);
            a = r1 / dot;

            cublasStatus = cublasSaxpy(cublasHandle, N, &a, d_p, 1, d_x, 1);
            na = -a;
            cublasStatus = cublasSaxpy(cublasHandle, N, &na, d_Ax, 1, d_r, 1);

            r0 = r1;
            cublasStatus = cublasSdot(cublasHandle, N, d_r, 1, d_r, 1, &r1);
            cudaThreadSynchronize();
            //printf("iteration = %3d, residual = %e   N-M = %d\n", k, sqrt(r1), N);
            k++;
        }
        printf("SPARSE SOLVER: Total iterations = %3d, residual = %e   NxM = %dx%d\n", k, sqrt(r1), N, M);

        cudaMemcpy(x, d_x, N*sizeof(float), cudaMemcpyDeviceToHost);

        // Check error
        float rsum, diff, err = 0.0;
        for (int i = 0; i < N; i++)
        {
            rsum = 0.0;

            for (int j = I[i]; j < I[i+1]; j++)
            {
                rsum += val[j]*x[J[j]];
            }

            diff = fabs(rsum - rhs[i]);

            if (diff > err)
            {
                err = diff;
            }
        }
        printf("SPARSE SOLVER:  Test Summary:  Error amount = %f\n", err);

        cusparseDestroy(cusparseHandle);
        cublasDestroy(cublasHandle);
        cudaFree(d_col);
        cudaFree(d_row);
        cudaFree(d_val);
        cudaFree(d_x);
        cudaFree(d_r);
        cudaFree(d_p);
        cudaFree(d_Ax);

        return err;

    }


}
