#include <cstdio>
#include <cstdlib>
     
#include "dehancer/opencl/embeddedProgram.h"
#include <string>

#define MEM_SIZE (128)
#define MAX_SOURCE_SIZE (0x100000)

/* extern char _expanded_opencl__hello_cl[]; */
/* extern unsigned int _expanded_opencl__hello_cl_len; */

extern "C" char helloKernel_cl[];
extern "C" unsigned int helloKernel_cl_len;

int main()
{
  cl_device_id device_id = NULL;
  cl_context context = NULL;
  cl_command_queue command_queue = NULL;
  cl_mem memobj = NULL;
  cl_program program = NULL;
  cl_kernel kernel = NULL;
  cl_platform_id platform_id = NULL;
  cl_uint ret_num_devices;
  cl_uint ret_num_platforms;
  cl_int ret;
     
  char string[MEM_SIZE];
     
  /* FILE *fp; */
  /* char fileName[] = "../hello.cl"; */
  /* const char *source_str = _expanded_opencl__hello_cl; */
  /* const size_t source_size = _expanded_opencl__hello_cl_len; */
  
  const char *source_str = helloKernel_cl;
  size_t source_size = helloKernel_cl_len;
  //char *source_str = clhGetEmbeddedProgram("helloKernel.cl",&source_size);
  //const char *source_str = clhGetEmbeddedProgram("exampleKernel.cl",&source_size);

  if (!source_str) {
    printf("could not find embedded program 'helloKernel.cl'\n");
    exit(1);
  }
  
  ret = clGetPlatformIDs(0,nullptr,&ret_num_platforms);
  cl_platform_id clPlatformIDs[ret_num_platforms];
  ret = clGetPlatformIDs(ret_num_platforms, clPlatformIDs, nullptr);
  platform_id = clPlatformIDs[0];
  //ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
  printf("found num platforms: %d\n",ret_num_platforms);
  
  ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, &ret_num_devices);
  printf("found num devices: %d\n",ret_num_devices);

  /* Create OpenCL context */
  context = clCreateContext(nullptr, 1, &device_id, nullptr, nullptr, &ret);
     
  /* Create Command Queue */
  command_queue = clCreateCommandQueue(context, device_id, 0, &ret); 
  /* command_queue = clCreateCommandQueueWithProperties(context, device_id, 0, &ret); */
     
  /* Create Memory Buffer */
  memobj = clCreateBuffer(context, CL_MEM_READ_WRITE,MEM_SIZE * sizeof(char), nullptr, &ret);
     
   printf("(begin sanity check)\n");
   printf("source size: %zu\n", source_size);
   for (int i=0;i<source_size;i++)
      printf("%c",source_str[i]);
  /*   /\* printf("%i: %c\n",i,source_str[i]); *\/ */
   printf("(end sanity)\n");

   /* Create Kernel Program from the source */
  program = clCreateProgramWithSource(context, 1, (const char **)&source_str,
                                      (const size_t *)&source_size, &ret);

  /* Build Kernel Program */
  ret = clBuildProgram(program, 1, &device_id, nullptr, nullptr, nullptr);

  if (ret != CL_SUCCESS) {

    std::string log = std::string("Unable to build OpenCL program from: ") + "hello";

    // Determine the size of the log
    size_t log_size;
    clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, 0, nullptr, &log_size);
    log.resize(log_size);

    // Get the log
    clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, log_size, log.data(), nullptr);

    std::cerr << log << std::endl;

    return -1;
  }

  /* Create OpenCL Kernel */
  kernel = clCreateKernel(program, "hello", &ret);
     
  /* Set OpenCL Kernel Parameters */
  ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&memobj);
     
  /* Execute OpenCL Kernel */
  ret = clEnqueueTask(command_queue, kernel, 0, NULL,NULL);
     
  /* Copy results from the memory buffer */
  ret = clEnqueueReadBuffer(command_queue, memobj, CL_TRUE, 0,
                            MEM_SIZE * sizeof(char),string, 0, NULL, NULL);
     
  /* Display Result */
  puts(string);
     
  /* Finalization */
  ret = clFlush(command_queue);
  ret = clFinish(command_queue);
  ret = clReleaseKernel(kernel);
  ret = clReleaseProgram(program);
  ret = clReleaseMemObject(memobj);
  ret = clReleaseCommandQueue(command_queue);
  ret = clReleaseContext(context);
     
  /* free(source_str); */
     
  return 0;
}
