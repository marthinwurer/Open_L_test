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


































