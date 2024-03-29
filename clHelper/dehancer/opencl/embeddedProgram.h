// ======================================================================== //
// Copyright 2017 Ingo Wald                                                 //
//                                                                          //
// Licensed under the Apache License, Version 2.0 (the "License");          //
// you may not use this file except in compliance with the License.         //
// You may obtain a copy of the License at                                  //
//                                                                          //
//     http://www.apache.org/licenses/LICENSE-2.0                           //
//                                                                          //
// Unless required by applicable law or agreed to in writing, software      //
// distributed under the License is distributed on an "AS IS" BASIS,        //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. //
// See the License for the specific language governing permissions and      //
// limitations under the License.                                           //
// ======================================================================== //

#pragma once

#include "dehancer/opencl/common.h"

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#include <sys/types.h>

#ifdef __cplusplus
#  include <string>

namespace clHelper {
  /*! get a std::string that contains the embedded OpenCL code. If the
    given file name couldn't be found among the embedded opencl codes
    a std::runtime_error will be thrown */
  std::string getEmbeddedProgram(const std::string &clFileName);
}

extern "C" {
#endif
  
//  /*! get a pointer to the embedded OpenCL code, as well as the length
//    of that opencl code (in *kernelLen).  Since the opencl code is
//    embedded into the program executable the resulting pointer does
//    _not_ have to be freed (in fact, trying to free should produce a
//    core dump).  If the symbol couldn't be found we will return NULL,
//    with 'kernelLength' then being undefined.
//
//    Usage: If your original opencl file (before embedding) was "src/program.cl"
//    then that is exactly the string you'll pass to this function.
//  */
//char *clhGetEmbeddedProgram(const char *clFileName, size_t *kernelLength);
  
#ifdef __cplusplus
} /* extern C */
#endif
