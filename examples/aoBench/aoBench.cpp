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

/*
  Copyright (c) 2010-2011, Intel Corporation
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are
  met:

  * Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.

  * Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution.

  * Neither the name of Intel Corporation nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.


  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
  IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
  TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
  PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
  OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  
*/

#include <chrono>
#include <ctime>

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#define NOMINMAX
#pragma warning (disable: 4244)
#pragma warning (disable: 4305)
#endif


#include <cassert>
#ifdef __linux__
#include <malloc.h>
#endif
#include <map>
#include <string>
#include <algorithm>
// clhelper

#include "dehancer/opencl/embeddedProgram.h"
#include "clHelper/device.h"
#include "Image.h"
#include "Function.h"

#define NSUBSAMPLES     4

cl_command_queue make_command_queue(const std::shared_ptr<clHelper::Device>& device) {
  /* Create OpenCL context */
  cl_int ret;

  cl_device_id device_id = device->clDeviceID;
  cl_context context = clCreateContext(nullptr, 1, &device_id, nullptr, nullptr, &ret);

  /* Create Command Queue */
  return clCreateCommandQueue(context, device_id, 0, &ret);
}

int run_bench2(int num, const std::shared_ptr<clHelper::Device>& device) {

  cl_uint width = 800, height = 600;

  Image image(width, height);

  auto command_queue = make_command_queue(device);

  auto bench_kernel = dehancer::opencl::example::Function(command_queue, "ao_bench_kernel");
  auto ao_bench_text = bench_kernel.make_texture(width/2,height/2);

  auto blend_kernel = dehancer::opencl::example::Function(command_queue, "blend_kernel");
  auto destination_text = blend_kernel.make_texture(width,height);

  std::chrono::time_point<std::chrono::system_clock> clock_begin
          = std::chrono::system_clock::now();

  bench_kernel.execute([&ao_bench_text](auto kernel){
      int numSubSamples = NSUBSAMPLES, count = 0;

      auto ret = clSetKernelArg(kernel, count++, sizeof(numSubSamples), (void *)&numSubSamples);
      if (ret != CL_SUCCESS) throw std::runtime_error("Unable to pass to kernel the number of samples");

      ret = clSetKernelArg(kernel, count++, sizeof(cl_mem), (void *)&ao_bench_text.buffer);
      if (ret != CL_SUCCESS) throw std::runtime_error("Unable to pass to kernel the texture buffer");

      return ao_bench_text;
  });

  blend_kernel.execute([&ao_bench_text,&destination_text](auto kernel){
      int count = 0;

      auto ret = clSetKernelArg(kernel, count++, sizeof(cl_mem), (void *)&ao_bench_text.buffer);
      if (ret != CL_SUCCESS) throw std::runtime_error("Unable to pass to kernel the source texture buffer");

      ret = clSetKernelArg(kernel, count++, sizeof(cl_mem), (void *)&destination_text.buffer);
      if (ret != CL_SUCCESS) throw std::runtime_error("Unable to pass to kernel the destination texture buffer");

      return destination_text;
  });

  /* Copy results from the memory buffer */

  size_t originst[3];
  size_t regionst[3];
  size_t  rowPitch = 0;
  size_t  slicePitch = 0;
  originst[0] = 0; originst[1] = 0; originst[2] = 0;
  regionst[0] = width; regionst[1] = height; regionst[2] = 1;

  cl_int ret = clEnqueueReadImage(
          command_queue,
          destination_text.buffer,
          CL_TRUE,
          originst,
          regionst,
          rowPitch,
          slicePitch,
          image.pix,
          0,
          nullptr,
          nullptr );

  if (ret != CL_SUCCESS) {
    throw std::runtime_error("Unable to create texture");
  }

  std::chrono::time_point<std::chrono::system_clock> clock_end
          = std::chrono::system_clock::now();
  std::chrono::duration<double> seconds = clock_end-clock_begin;

  // Report results and save image
  std::cout << "[aobench cl]:\t" << seconds.count() << "s "
            << ", for a " << width << "x" << height << " pixels" << std::endl;

  std::string out_file = "ao-cl-"; out_file.append(std::to_string(num)); out_file.append(".ppm");

  image.savePPM(out_file.c_str());

  dehancer::opencl::example::release(ao_bench_text);
  dehancer::opencl::example::release(destination_text);

  return 0;
}

int main(int argc, char **argv)
{

  try {
    std::vector<std::shared_ptr<clHelper::Device>> devices
            = clHelper::getAllDevices();

    std::shared_ptr<clHelper::Device> device;

    assert(!devices.empty());

    int dev_num = 0;
    std::cout << "Info: " << std::endl;
    for (auto d: devices) {
      std::cout << " #" << dev_num++ << std::endl;
      d->print(" ", std::cout);
    }

    std::cout << "Bench: " << std::endl;
    dev_num = 0;
    for (auto d: devices) {
      if (run_bench2(dev_num++, d)!=0) return -1;
    }
    return 0;
  }
  catch (const std::runtime_error &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return -1;
  }

}
