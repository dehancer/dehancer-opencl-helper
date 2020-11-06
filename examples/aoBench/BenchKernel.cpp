//
// Created by denn nevera on 06/11/2020.
//

#include "BenchKernel.h"

BenchKernel::BenchKernel(const void *command_queue,
                         const std::string &kernel_name,
                         size_t width,
                         size_t height,
                         size_t samples):
        command_queue_(command_queue),
        kernel_name_(kernel_name),
        width_(width),
        height_(height),
        samples_(samples)
{
  last_error_ = clGetCommandQueueInfo(get_command_queue(), CL_QUEUE_DEVICE, sizeof(cl_device_id), &device_id_, nullptr);
  if (last_error_ != CL_SUCCESS) {
    std::runtime_error("Unable to get OpenCL the device");
  }

  last_error_ = clGetCommandQueueInfo(get_command_queue(), CL_QUEUE_CONTEXT, sizeof(cl_context), &context_, nullptr);
  if (last_error_ != CL_SUCCESS) {
    std::runtime_error("Unable to get OpenCL context");
  }

  const std::string source = clHelper::getEmbeddedProgram("../shaders/exampleKernel.cl");

  const char *source_str = source.c_str();
  size_t source_size = source.size();
  program_ = clCreateProgramWithSource(context_, 1, (const char **)&source_str,
                                       (const size_t *)&source_size, &last_error_);

  if (last_error_ != CL_SUCCESS) {
    std::runtime_error("Unable to create OpenCL program from exampleKernel.cl");
  }


  /* Build Kernel Program */
  last_error_ = clBuildProgram(program_, 1, &device_id_, NULL, NULL, NULL);

  if (last_error_ != CL_SUCCESS) {
    std::runtime_error("Unable to build OpenCL program from exampleKernel.cl");
  }

  kernel_ = clCreateKernel(program_, kernel_name_.c_str(), &last_error_);

  if (last_error_ != CL_SUCCESS) {
    std::runtime_error("Unable to create kernel for: " + kernel_name_);
  }

  ao_texture_ = make_texture(width_,height_);

}

cl_command_queue BenchKernel::get_command_queue() const {
  return static_cast<cl_command_queue>((void *)command_queue_);
}

void BenchKernel::process() {

  /* Set OpenCL Kernel Parameters */
  int count = 0 ;
  last_error_ = clSetKernelArg(kernel_, count++, sizeof(width_),  (void *)&width_);
  last_error_ |= clSetKernelArg(kernel_, count++, sizeof(height_), (void *)&height_);
  last_error_ |= clSetKernelArg(kernel_, count++, sizeof(samples_), (void *)&samples_);
  last_error_ |= clSetKernelArg(kernel_, count++, sizeof(cl_mem), (void *)&ao_texture_);

  if (last_error_ != CL_SUCCESS) {
    std::runtime_error("Unable to create texture for: " + kernel_name_);
  }

  size_t localWorkSize[2], globalWorkSize[2];
  clGetKernelWorkGroupInfo(kernel_, device_id_, CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), localWorkSize, nullptr);
  localWorkSize[1] = 1;
  globalWorkSize[0] = ((width_ + localWorkSize[0] - 1) / localWorkSize[0]) * localWorkSize[0];
  globalWorkSize[1] = height_;

  last_error_ = clEnqueueNDRangeKernel(get_command_queue(), kernel_, 2, nullptr, globalWorkSize, localWorkSize, 0, nullptr, nullptr);


  if (last_error_ != CL_SUCCESS) {
    std::runtime_error("Unable to enqueue kernel: " + kernel_name_);
  }

}

Texture BenchKernel::make_texture(size_t width, size_t height, size_t depth) {
  size_t memSize = width*height*3*sizeof(float);
  cl_mem memobj = clCreateBuffer(context_, CL_MEM_READ_WRITE, memSize, NULL, &last_error_);
  if (last_error_ != CL_SUCCESS) {
    std::runtime_error("Unable to create texture for: " + kernel_name_);
  }

  return {memobj,width,height,depth};
}

BenchKernel::~BenchKernel() {
  release(ao_texture_);
  if (kernel_) clReleaseKernel(kernel_);
  if (program_) clReleaseProgram(program_);
}

Texture BenchKernel::get_destination() const {
  return ao_texture_;
}
