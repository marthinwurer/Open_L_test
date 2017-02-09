/*
 * my_cl_helper.c
 *
 *  Created on: Feb 9, 2017
 *      Author: benjamin
 */

#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "my_cl_helper.h"


int createCLcontextonGL(
		cl_int platform_number,
		cl_int device_number,

		cl_context * context, // holds the context
		cl_command_queue * cl_queue){ // a command queue for the context

	int error_code;
	cl_int status;
	size_t size;
	char buf[1024];

	// get the device ID range to work with.
	// get number of platforms
	cl_uint plat_count;
	CALL_CL_GUARDED(clGetPlatformIDs, (0, NULL, &plat_count));

	if( plat_count < 1){
		printf("No Platforms available! Aborting\n");
		abort();
	}
	printf("Number of platforms: %d\n", plat_count);

	// allocate memory, get list of platform handles
	cl_platform_id *platforms =
			(cl_platform_id *) malloc(plat_count*sizeof(cl_platform_id));
	CHECK_SYS_ERROR(!platforms, "allocating platform array");
	CALL_CL_GUARDED(clGetPlatformIDs, (plat_count, platforms, NULL));

	// get the devices for the selected platform
	cl_platform_id plat = platforms[platform_number];

	// get number of devices in platform
	cl_uint dev_count;
	CALL_CL_GUARDED(clGetDeviceIDs, (plat, CL_DEVICE_TYPE_ALL,
			0, NULL, &dev_count));
	printf("Number of devices: %d\n", dev_count);

	// allocate memory, get list of device handles in platform
	cl_device_id *devices =
			(cl_device_id *) malloc(dev_count*sizeof(cl_device_id));
	CHECK_SYS_ERROR(!devices, "allocating device array");

	// get the list of devices (very important)
	CALL_CL_GUARDED(clGetDeviceIDs, (plat, CL_DEVICE_TYPE_ALL,
			dev_count, devices, NULL));

	cl_device_id dev = devices[device_number];

	// check if this device supports the OpenGL interoperability
	status = clGetDeviceInfo(dev, CL_DEVICE_EXTENSIONS, sizeof(buf), &buf, &size );
	puts(buf);


	// create a context
	cl_context_properties cps[3] = {
			CL_CONTEXT_PLATFORM, (cl_context_properties) plat, 0 };

	*context = clCreateContext(
			NULL, 1, &dev, NULL, NULL, &status);
	CHECK_CL_ERROR(status, "clCreateContext");



	// create a command queue
	cl_command_queue_properties qprops = 0;


	*cl_queue = clCreateCommandQueue(*context, dev, qprops, &status);
	CHECK_CL_ERROR(status, "clCreateCommandQueue");

	// free the device lists
	free(devices);
	free(platforms);

	return error_code;
}


































// His code

// from cl-helper.c by Andreas Kloeckner.
/*
 * Copyright (c) 2010, 2012 Andreas Kloeckner
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */



#define MAX_NAME_LEN 1000




const char *cl_error_to_str(cl_int e)
{
  switch (e)
  {
    case CL_SUCCESS: return "success";
    case CL_DEVICE_NOT_FOUND: return "device not found";
    case CL_DEVICE_NOT_AVAILABLE: return "device not available";
#if !(defined(CL_PLATFORM_NVIDIA) && CL_PLATFORM_NVIDIA == 0x3001)
    case CL_COMPILER_NOT_AVAILABLE: return "device compiler not available";
#endif
    case CL_MEM_OBJECT_ALLOCATION_FAILURE: return "mem object allocation failure";
    case CL_OUT_OF_RESOURCES: return "out of resources";
    case CL_OUT_OF_HOST_MEMORY: return "out of host memory";
    case CL_PROFILING_INFO_NOT_AVAILABLE: return "profiling info not available";
    case CL_MEM_COPY_OVERLAP: return "mem copy overlap";
    case CL_IMAGE_FORMAT_MISMATCH: return "image format mismatch";
    case CL_IMAGE_FORMAT_NOT_SUPPORTED: return "image format not supported";
    case CL_BUILD_PROGRAM_FAILURE: return "build program failure";
    case CL_MAP_FAILURE: return "map failure";

    case CL_INVALID_VALUE: return "invalid value";
    case CL_INVALID_DEVICE_TYPE: return "invalid device type";
    case CL_INVALID_PLATFORM: return "invalid platform";
    case CL_INVALID_DEVICE: return "invalid device";
    case CL_INVALID_CONTEXT: return "invalid context";
    case CL_INVALID_QUEUE_PROPERTIES: return "invalid queue properties";
    case CL_INVALID_COMMAND_QUEUE: return "invalid command queue";
    case CL_INVALID_HOST_PTR: return "invalid host ptr";
    case CL_INVALID_MEM_OBJECT: return "invalid mem object";
    case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR: return "invalid image format descriptor";
    case CL_INVALID_IMAGE_SIZE: return "invalid image size";
    case CL_INVALID_SAMPLER: return "invalid sampler";
    case CL_INVALID_BINARY: return "invalid binary";
    case CL_INVALID_BUILD_OPTIONS: return "invalid build options";
    case CL_INVALID_PROGRAM: return "invalid program";
    case CL_INVALID_PROGRAM_EXECUTABLE: return "invalid program executable";
    case CL_INVALID_KERNEL_NAME: return "invalid kernel name";
    case CL_INVALID_KERNEL_DEFINITION: return "invalid kernel definition";
    case CL_INVALID_KERNEL: return "invalid kernel";
    case CL_INVALID_ARG_INDEX: return "invalid arg index";
    case CL_INVALID_ARG_VALUE: return "invalid arg value";
    case CL_INVALID_ARG_SIZE: return "invalid arg size";
    case CL_INVALID_KERNEL_ARGS: return "invalid kernel args";
    case CL_INVALID_WORK_DIMENSION: return "invalid work dimension";
    case CL_INVALID_WORK_GROUP_SIZE: return "invalid work group size";
    case CL_INVALID_WORK_ITEM_SIZE: return "invalid work item size";
    case CL_INVALID_GLOBAL_OFFSET: return "invalid global offset";
    case CL_INVALID_EVENT_WAIT_LIST: return "invalid event wait list";
    case CL_INVALID_EVENT: return "invalid event";
    case CL_INVALID_OPERATION: return "invalid operation";
    case CL_INVALID_GL_OBJECT: return "invalid gl object";
    case CL_INVALID_BUFFER_SIZE: return "invalid buffer size";
    case CL_INVALID_MIP_LEVEL: return "invalid mip level";

#if defined(cl_khr_gl_sharing) && (cl_khr_gl_sharing >= 1)
    case CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR: return "invalid gl sharegroup reference number";
#endif

#ifdef CL_VERSION_1_1
    case CL_MISALIGNED_SUB_BUFFER_OFFSET: return "misaligned sub-buffer offset";
    case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST: return "exec status error for events in wait list";
    case CL_INVALID_GLOBAL_WORK_SIZE: return "invalid global work size";
#endif

    default: return "invalid/unknown error code";
  }
}


char *read_file(const char *filename)
{
  FILE *f = fopen(filename, "r");
  CHECK_SYS_ERROR(!f, "read_file: opening file");

  // figure out file size
  CHECK_SYS_ERROR(fseek(f, 0, SEEK_END) < 0, "read_file: seeking to end");
  size_t size = ftell(f);

  CHECK_SYS_ERROR(fseek(f, 0, SEEK_SET) != 0,
      "read_file: seeking to start");

  // allocate memory, slurp in entire file
  char *result = (char *) malloc(size+1);
  CHECK_SYS_ERROR(!result, "read_file: allocating file contents");
  CHECK_SYS_ERROR(fread(result, 1, size, f) < size,
      "read_file: reading file contents");

  // close, return
  CHECK_SYS_ERROR(fclose(f), "read_file: closing file");
  result[size] = '\0';

  return result;
}




static int printed_compiler_output_message = 0;

cl_kernel kernel_from_string(cl_context ctx,
    char const *knl, char const *knl_name, char const *options)
{
  // create an OpenCL program (may have multiple kernels)
  size_t sizes[] = { strlen(knl) };

  if (options && strlen(options) == 0)
  {
    // reportedly, some implementations dislike empty strings.
    options = NULL;
  }

  cl_int status;
  cl_program program = clCreateProgramWithSource(ctx, 1, &knl, sizes, &status);
  CHECK_CL_ERROR(status, "clCreateProgramWithSource");

  // build it
  status = clBuildProgram(program, 0, NULL, options, NULL, NULL);

  {
    // get build log and print it

    cl_device_id dev;
    CALL_CL_GUARDED(clGetProgramInfo, (program, CL_PROGRAM_DEVICES,
          sizeof(dev), &dev, NULL));

    size_t log_size;
    CALL_CL_GUARDED(clGetProgramBuildInfo, (program, dev, CL_PROGRAM_BUILD_LOG,
          0, NULL, &log_size));

    bool do_print = status != CL_SUCCESS;
    if (!do_print && log_size)
    {
      if (getenv("CL_HELPER_PRINT_COMPILER_OUTPUT"))
        do_print = true;
      else
      {
        if (!printed_compiler_output_message && !getenv("CL_HELPER_NO_COMPILER_OUTPUT_NAG"))
        {
          fprintf(stderr, "*** Kernel compilation resulted in non-empty log message.\n"
              "*** Set environment variable CL_HELPER_PRINT_COMPILER_OUTPUT=1 to see more.\n"
              "*** NOTE: this may include compiler warnings and other important messages\n"
              "***   about your code.\n"
              "*** Set CL_HELPER_NO_COMPILER_OUTPUT_NAG=1 to disable this message.\n");
          printed_compiler_output_message = true;
        }
      }
    }

    if (do_print)
    {
      char *log = (char *) malloc(log_size);
      CHECK_SYS_ERROR(!log, "kernel_from_string: allocate log");

      char devname[MAX_NAME_LEN];
      CALL_CL_GUARDED(clGetDeviceInfo, (dev, CL_DEVICE_NAME,
            sizeof(devname), devname, NULL));

      CALL_CL_GUARDED(clGetProgramBuildInfo, (program, dev, CL_PROGRAM_BUILD_LOG,
            log_size, log, NULL));
      fprintf(stderr, "*** build of '%s' on '%s' said:\n%s\n*** (end of message)\n",
          knl_name, devname, log);
    }
  }

  CHECK_CL_ERROR(status, "clBuildProgram");

  // fish the kernel out of the program
  cl_kernel kernel = clCreateKernel(program, knl_name, &status);
  CHECK_CL_ERROR(status, "clCreateKernel");

  CALL_CL_GUARDED(clReleaseProgram, (program));

  return kernel;
}












