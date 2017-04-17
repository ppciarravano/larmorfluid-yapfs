# LarmorFluid - YAPFS

YAPFS: Yet Another PIC/FLIP fluid simulation Solver

Author: Pier Paolo Ciarravano [http://www.larmor.com](http://www.larmor.com)

[![Larmorfluid YAPFS](https://github.com/ppciarravano/larmorfluid-yapfs/blob/master/doc/images/visual_id.jpg)](https://github.com/ppciarravano/larmorfluid-yapfs/blob/master/doc/images/visual_id.jpg)

This project is another classical implementation of PIC/FLIP fluid simulation solver based on algorithms described in the book “Fluid Simulation for Computer Graphics” by Robert Bridson, but with some particular future goals:

* Use of [OpenVDB](http://www.openvdb.org/) and future implementation using Dneg's [OpenVDBPoints](https://github.com/dneg/openvdb_points_dev) lib
* NVIDIA CUDA 8 and cuSPARSE API
* Multithread using TBB
* Handling of large grid and large dataset
* Use of [NVIDIA GVDB Sparse Volumes](https://developer.nvidia.com/gvdb)
* Export particles with [Disney Partio](https://www.disneyanimation.com/technology/partio.html) or other formats
* Import from [Alembic](http://www.alembic.io/)
* Unit test using CppUnit
* Availability of AWS EC2 AMI image already configured to run the solver easily on a [AWS EC2 P2 Istances](http://docs.aws.amazon.com/AWSEC2/latest/UserGuide/accelerated-computing-instances.html)

The project is still in development and debugging.

[![Youtube video of DEBUG artifacts and issues PIC/FLIP LarmorFluid](http://img.youtube.com/vi/jkg3ykTz2xo/0.jpg)](http://www.youtube.com/watch?v=jkg3ykTz2xo)
Youtube Video of DEBUG artifacts and issues PIC/FLIP LarmorFluid.

### Build and test:

Use CMake to build the binary.

To run the unit test of CUDA pressure solver e.g.:
```
$ ./yapfs --action test
LarmorFluid or YAPFS (Yet Another PIC/FLIP Solver) by Pier Paolo Ciarravano (http://www.larmor.com) - Dec.2016-Jan.2017

INFO - logInit initialized
INFO - Reading command line
INFO - Reading Configs file
INFO - Config file name: yapfs.ini
DEBUG - getConfig: action = test
INFO - Starting YAPFS test
INFO - RUN TEST
TestCaseSolver::testSolver

INFO - Run Solver on grid dimension: 100x100x100
GPU Device 0: "GeForce GTX 1070" with compute capability 6.1

> GPU device has 15 Multi-Processors, SM 6.1 compute capabilities
SPARSE SOLVER: Total iterations =   8, residual = 8.643779e-06   NxM = 1000000x1000000
SPARSE SOLVER:  Test Summary:  Error amount = 0.000006
INFO - TOTAL TIME IN MILLIESEC: 12.639971
INFO - TOTAL ERROR: 0.000000
.....

```


### License:

LarmorFluid-YAPFS Version 1.0 2017

Copyright (c) 2017 Pier Paolo Ciarravano

[http://www.larmor.com](http://www.larmor.com)

All rights reserved.

LarmorFluid-YAPFS is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

LarmorFluid-YAPFS is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with LarmorFluid-YAPFS. If not, see <http://www.gnu.org/licenses/>.

Licensees holding a valid commercial license may use this file in
accordance with the commercial license agreement provided with the
software.


---
