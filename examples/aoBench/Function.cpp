//
// Created by denn nevera on 06/11/2020.
//

#include "Function.h"
#include <cstring>

extern char exampleKernel_cl[];
extern unsigned int exampleKernel_cl_len;

namespace dehancer::opencl::example  {
    Function::Function(const void *command_queue,
                       const std::string &kernel_name
    ):
            command_queue_(command_queue),
            kernel_name_(kernel_name)
    {
      last_error_ = clGetCommandQueueInfo(get_command_queue(), CL_QUEUE_DEVICE, sizeof(cl_device_id), &device_id_,
                                          nullptr);
      if (last_error_ != CL_SUCCESS) {
        throw std::runtime_error("Unable to get OpenCL the device");
      }

      last_error_ = clGetCommandQueueInfo(get_command_queue(), CL_QUEUE_CONTEXT, sizeof(cl_context), &context_,
                                          nullptr);
      if (last_error_ != CL_SUCCESS) {
        throw std::runtime_error("Unable to get OpenCL context");
      }

      //const std::string source = exampleKernel_cl;//clHelper::getEmbeddedProgram("exampleKernel.cl");
      //const char *source_str = source.c_str();
      //size_t source_size = source.size();
  
      const char *source_str = exampleKernel_cl;
      size_t source_size = exampleKernel_cl_len;
      
      program_ = clCreateProgramWithSource(context_, 1, (const char **) &source_str,
                                           (const size_t *) &source_size, &last_error_);

      if (last_error_ != CL_SUCCESS) {
        throw std::runtime_error("Unable to create OpenCL program from exampleKernel.cl");
      }


      /* Build Kernel Program */
      last_error_ = clBuildProgram(program_, 1, &device_id_, nullptr, nullptr, nullptr);

      if (last_error_ != CL_SUCCESS) {
        throw std::runtime_error("Unable to build OpenCL program from exampleKernel.cl");
      }

      kernel_ = clCreateKernel(program_, kernel_name_.c_str(), &last_error_);

      if (last_error_ != CL_SUCCESS) {
        throw std::runtime_error("Unable to create kernel for: " + kernel_name_);
      }
    }

    cl_command_queue Function::get_command_queue() const {
      return static_cast<cl_command_queue>((void *) command_queue_);
    }


    Texture Function::make_texture(size_t width, size_t height, size_t depth) {

      cl_image_format format;
      cl_image_desc   desc;

      memset( &format, 0, sizeof( format ) );

      format.image_channel_order = CL_RGBA;
      format.image_channel_data_type = CL_FLOAT;

      memset( &desc, 0, sizeof( desc ) );
      desc.image_type = CL_MEM_OBJECT_IMAGE2D;
      desc.image_width = width;
      desc.image_height = height;
      desc.image_depth = 1;

      cl_mem memobj = clCreateImage(
              context_,
              CL_MEM_READ_WRITE,
              &format,
              &desc,
              nullptr,
              &last_error_);

      if (last_error_ != CL_SUCCESS) {
        throw std::runtime_error("Unable to create texture for: " + kernel_name_);
      }

      return {memobj, width, height, depth};
    }

    Function::~Function() {
      if (kernel_) clReleaseKernel(kernel_);
      if (program_) clReleaseProgram(program_);
    }

    void Function::execute(const FunctionHandler &block) {

      auto texture = block(kernel_);

      size_t localWorkSize[2] = {1,1};

      clGetKernelWorkGroupInfo(kernel_,  device_id_,
                               CL_KERNEL_WORK_GROUP_SIZE, sizeof(localWorkSize), localWorkSize, nullptr);

      if (localWorkSize[0]>=texture.width) localWorkSize[0] = texture.width;
      if (localWorkSize[1]>=texture.height) localWorkSize[1] = texture.height;

      size_t globalWorkSize[2] = {
              ((texture.width + localWorkSize[0] - 1) / localWorkSize[0]) * localWorkSize[0],
              ((texture.height + localWorkSize[1] - 1) / localWorkSize[1]) * localWorkSize[1]
      };

      cl_event    AlphaComposting12 = nullptr;

      last_error_ = clEnqueueNDRangeKernel(get_command_queue(), kernel_, 2, nullptr,
                                           globalWorkSize,
                                           localWorkSize,
                                           0,
                                           nullptr, &AlphaComposting12);

      if (last_error_ != CL_SUCCESS) {
        throw std::runtime_error("Unable to enqueue kernel: " + kernel_name_);
      }

      last_error_ = clWaitForEvents( 1, &AlphaComposting12 );

      if (last_error_ != CL_SUCCESS) {
        throw std::runtime_error("Unable to enqueue kernel: " + kernel_name_);
      }
    }
}