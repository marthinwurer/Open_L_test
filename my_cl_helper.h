/*
 * my_cl_helper.h
 *
 *  Created on: Feb 9, 2017
 *      Author: benjamin
 */

#ifndef MY_CL_HELPER_H_
#define MY_CL_HELPER_H_

#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#include <stdlib.h>
#include "cl-helper.h"

// Original code by Benjamin Maitland

int createCLcontextonGL(
		cl_int platform_number,
		cl_int device_number,

		cl_context * context, // holds the context
		cl_command_queue * cl_queue); // a command queue for the context


#endif /* MY_CL_HELPER_H_ */
