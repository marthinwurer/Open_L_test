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
/* An error check macro for OpenCL.
 *
 * Usage:
 * CHECK_CL_ERROR(status_code_from_a_cl_operation, "function_name")
 *
 * It will abort with a message if an error occurred.
 */

#define CHECK_CL_ERROR(STATUS_CODE, WHAT) \
  if ((STATUS_CODE) != CL_SUCCESS) \
  { \
    fprintf(stderr, \
        "*** '%s' in '%s' on line %d failed with error '%s'.\n", \
        WHAT, __FILE__, __LINE__, \
        cl_error_to_str(STATUS_CODE)); \
    abort(); \
  }

/* A more automated error check macro for OpenCL, for use with clXxxx
 * functions that return status codes. (Not all of them do, notably
 * clCreateXxx do not.)
 *
 * Usage:
 * CALL_CL_GUARDED(clFunction, (arg1, arg2));
 *
 * Note the slightly strange comma between the function name and the
 * argument list.
 */

#define CALL_CL_GUARDED(NAME, ARGLIST) \
  { \
    cl_int status_code; \
      status_code = NAME ARGLIST; \
    CHECK_CL_ERROR(status_code, #NAME); \
  }

/* An error check macro for Unix system functions. If "COND" is true, then the
 * last system error ("errno") is printed along with MSG, which is supposed to
 * be a string describing what you were doing.
 *
 * Example:
 * CHECK_SYS_ERROR(dave != 0, "opening hatch");
 */
#define CHECK_SYS_ERROR(COND, MSG) \
  if (COND) \
  { \
    perror(MSG); \
    abort(); \
  }

/* Return a string describing the OpenCL error code 'e'.
 */
const char *cl_error_to_str(cl_int e);

/* Read contents of file 'filename'.
 * Return as a new string. You must free the string when you're done with it.
 *
 * This function always succeeds. (If an error occurs, the program
 * is aborted.
 */
char *read_file(const char *filename);

/* Create a new OpenCL kernel from the code in the string 'knl'.
 * 'knl_name' is the name of the kernel function, and 'options',
 * if not NULL, is a string containing compiler flags.
 *
 * You must release the resulting kernel when you're done
 * with it.
 *
 * This function always succeeds. (If an error occurs, the program
 * is aborted.
 */
cl_kernel kernel_from_string(cl_context ctx,
    char const *knl, char const *knl_name, char const *options);



// Original code by Benjamin Maitland

int createCLcontextonGL(
		cl_int platform_number,
		cl_int device_number,

		cl_context * context, // holds the context
		cl_command_queue * cl_queue); // a command queue for the context


#endif /* MY_CL_HELPER_H_ */
