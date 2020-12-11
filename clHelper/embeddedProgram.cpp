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

#include "embeddedProgram.h"
#include <dlfcn.h>
#include <sstream>
#include <cstdio>

namespace clHelper {

    enum embed_error {
        embed_fail = 0,
        kernel_not_found,
        kernel_size_is_wrong
    };

    extern "C" char *clhGetEmbeddedProgram(const char *fileName, size_t *kernelLength, embed_error* error)
    {
      char kernel_ptr_symbol_name[10000];
      sprintf(kernel_ptr_symbol_name,"%s",fileName);

      char kernel_len_symbol_name[10000];
      sprintf(kernel_len_symbol_name,"%s_len",fileName);

      for (char *s = kernel_ptr_symbol_name; *s; s++) {
        if (*s == '.') *s = '_';
        if (*s == '/') *s = '_';
      }
      for (char *s = kernel_len_symbol_name; *s; s++) {
        if (*s == '.') *s = '_';
        if (*s == '/') *s = '_';
      }

      void *mainProgram = dlopen(nullptr,RTLD_NOW|RTLD_GLOBAL);

      if (!mainProgram) {
        *error = embed_fail;
        return nullptr;
      }

      void *kernel_ptr_symbol = dlsym(mainProgram,kernel_ptr_symbol_name);
      if (!kernel_ptr_symbol) *error = kernel_not_found;
      if (!kernel_ptr_symbol) return nullptr;

      void *kernel_len_symbol = dlsym(mainProgram,kernel_len_symbol_name);
      if (!kernel_len_symbol) *error = kernel_size_is_wrong;
      if (!kernel_len_symbol) return nullptr;

      *kernelLength = *(size_t *)kernel_len_symbol;
      char *kernel_src = (char *)kernel_ptr_symbol;

#if CLH_PRINT_EMBEDDED_PROGRAM_SOURCE
      printf("(begin sanity check)\n");
    printf("source size: %li\n",*kernelLength);
    
    for (int i=0;i<*kernelLength;i++)
      printf("%c",kernel_src[i]);
    printf("(end sanity)\n");
#endif
      return kernel_src;
    }

    /*! get a std::string that contains the embedded OpenCL code. If the
      given file name couldn't be found among the embedded opencl codes
      a std::runtime_error will be thrown */
    std::string getEmbeddedProgram(const std::string &clFileName)
    {

      size_t len;

      embed_error error;

      const char *programSrc = clhGetEmbeddedProgram(clFileName.c_str(), &len, &error);

      if (!programSrc) {
        std::string mess;
        switch (error) {
          case embed_fail:
            mess = "embedding source is corrupted"; break;
          case kernel_not_found:
            mess = "kernel source is not found"; break;
          case kernel_size_is_wrong:
            mess = "kernel source size is wrong"; break;
        }
        throw std::runtime_error("could not find embedded opencl code for '" + clFileName + "' error: " + mess);
      }

      std::stringstream ss;

      for (int i=0;i<len;i++)
        ss << programSrc[i];

      return ss.str();
    }


} // ::clHelper

