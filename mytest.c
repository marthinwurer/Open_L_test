/*
 * mytest.c
 *
 *  Created on: Jan 4, 2017
 *      Author: benjamin
 */

#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#include "cl-helper.h"

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <time.h>
#include <sys/time.h>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>
// GLFW
#include <GLFW/glfw3.h>


#include "map2d.h"
#include "DiamondSquare.h"
#include "interpolation.h"
#include "linmath.h"

const int SCREEN_WIDTH = 1024; // the width of the screen in pixels
const int SCREEN_HEIGHT = 1024; // the height of the screen in pixels

#define DO_CL
#ifdef DO_CL
#define DO_AVERAGE
#endif


// this struct holds the data needed to render a vertex
typedef struct vertex_data{
	cl_float4 coords;
	cl_float4 normal;
} vertex_data;


static inline void create_X_rot_mat4x4(mat4x4 M, float angle);
static inline void create_Y_rot_mat4x4(mat4x4 M, float angle);
static inline void create_Z_rot_mat4x4(mat4x4 M, float angle);


/**
 * rotate the position in toRotate around the given axis by angle radians
 */
void rotate_vec4_x(vec4 toRotate, float angle){
	vec4 temp;
	// rotate it around the origin
	mat4x4 rotation_matrix;
	create_X_rot_mat4x4(rotation_matrix, -angle);
	mat4x4_mul_vec4(temp, rotation_matrix, toRotate);

	toRotate[0] = temp[0];
	toRotate[1] = temp[1];
	toRotate[2] = temp[2];
	toRotate[3] = temp[3];
}
void rotate_vec4_y(vec4 toRotate, float angle){
	vec4 temp;
	// rotate it around the origin
	mat4x4 rotation_matrix;
	create_Y_rot_mat4x4(rotation_matrix, -angle);
	mat4x4_mul_vec4(temp, rotation_matrix, toRotate);

	toRotate[0] = temp[0];
	toRotate[1] = temp[1];
	toRotate[2] = temp[2];
	toRotate[3] = temp[3];
}
void rotate_vec4_z(vec4 toRotate, float angle){
	vec4 temp;
	// rotate it around the origin
	mat4x4 rotation_matrix;
	create_Z_rot_mat4x4(rotation_matrix, -angle);
	mat4x4_mul_vec4(temp, rotation_matrix, toRotate);

	toRotate[0] = temp[0];
	toRotate[1] = temp[1];
	toRotate[2] = temp[2];
	toRotate[3] = temp[3];
}

// Key callback and stuff

vec3 camera_position = {0.0f, 100.0f, 0.0f};

mat4x4 view_matrix; // moves from world space to camera space
float camera_rotation = 0.0f; // in rads
float camera_pitch = 0.0f;



void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	// When a user presses the escape key, we set the WindowShouldClose property to true,
	// closing the application
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	vec4 camera_movement = {0.0f, 0.0f, 0.0f, 1.0f};


	mat4x4 temp;

	switch( key ){

	case GLFW_KEY_J:
		camera_movement[1] -= 0.5;
		break;
	case GLFW_KEY_U:
		camera_movement[1] += 0.5;
		break;
	case GLFW_KEY_W:
		camera_movement[0] += 0.5;
		break;
	case GLFW_KEY_S:
		camera_movement[0] -= 0.5;
		break;
	case GLFW_KEY_D:
		camera_movement[2] += 0.5;
		break;
	case GLFW_KEY_A:
		camera_movement[2] -= 0.5;
		break;
	case GLFW_KEY_O:
		camera_rotation += 0.1;
		break;
	case GLFW_KEY_P:
		camera_rotation -= 0.1;
		break;
	case GLFW_KEY_I:
		camera_pitch += 0.1;
		break;
	case GLFW_KEY_K:
		camera_pitch -= 0.1;
		break;
	}

	// rotate the movement vector and add it to the camera position
	rotate_vec4_y(camera_movement, camera_rotation);
	vec3_add(camera_position, camera_movement, camera_position);




	// does void casts to blank warnings
	(void) scancode;
	(void) mode;
}



GLuint compile_shader(const char * filename, GLuint shader_type){
    GLint success;
    GLchar infoLog[512];

	GLchar const * shaderSource = read_file(filename);

	// do the fragment shader stuff
	GLuint shader;
	shader = glCreateShader(shader_type);
	glShaderSource(shader, 1, &shaderSource, NULL);
	glCompileShader(shader);
	// check for errors
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if(!success)
	{
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		printf( "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s", infoLog );
		abort();
	}

	free((char*)shaderSource);
	return shader;
}

/**
 * create a rotation matrix around the origin around the given axis for angle radians.
 */
static inline void create_X_rot_mat4x4(mat4x4 M, float angle)
{
	float s = sinf(angle);
	float c = cosf(angle);
	mat4x4 R = {
		{1.f, 0.f, 0.f, 0.f},
		{0.f,   c,   s, 0.f},
		{0.f,  -s,   c, 0.f},
		{0.f, 0.f, 0.f, 1.f}
	};
	mat4x4_dup( M, R);
}
static inline void create_Y_rot_mat4x4(mat4x4 M, float angle)
{
	float s = sinf(angle);
	float c = cosf(angle);
	mat4x4 R = {
		{   c, 0.f,   s, 0.f},
		{ 0.f, 1.f, 0.f, 0.f},
		{  -s, 0.f,   c, 0.f},
		{ 0.f, 0.f, 0.f, 1.f}
	};
	mat4x4_dup( M, R);
}
static inline void create_Z_rot_mat4x4(mat4x4 M, float angle)
{
	float s = sinf(angle);
	float c = cosf(angle);
	mat4x4 R = {
		{   c,   s, 0.f, 0.f},
		{  -s,   c, 0.f, 0.f},
		{ 0.f, 0.f, 1.f, 0.f},
		{ 0.f, 0.f, 0.f, 1.f}
	};
	mat4x4_dup( M, R);
}

struct timeval old;
void inittime(){
	gettimeofday(&old, NULL);
}

void printtime(){
    // timing stuff
    struct timeval current;

    long mtime, seconds, useconds;

    gettimeofday(&current, NULL);
    seconds  = current.tv_sec  - old.tv_sec;
    useconds = current.tv_usec - old.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;

    printf("Elapsed time: %ld milliseconds\n", mtime);
    old = current;
}

void cl_average(cl_command_queue cl_queue, cl_kernel test_kern,
		cl_int dimension, cl_int buff_size, cl_mem * buf_1, cl_mem * buf_2   ){
	CALL_CL_GUARDED(clFinish, (cl_queue));
	//        gettimeofday(&start, NULL);

	// set up parameters to the call
	size_t ldim[] = { 32 };
	size_t gdim[] = { buff_size };


	// set the arguments
	CALL_CL_GUARDED(clSetKernelArg, (test_kern, 0, sizeof(buf_1), buf_1));
	CALL_CL_GUARDED(clSetKernelArg, (test_kern, 1, sizeof(buf_2), buf_2));
	CALL_CL_GUARDED(clSetKernelArg, (test_kern, 2, sizeof(cl_int), &dimension));
	CALL_CL_GUARDED(clSetKernelArg, (test_kern, 3, sizeof(cl_int), &buff_size));

	// enqueue the kernel (plus magic constant arrays copied from the example
	CALL_CL_GUARDED(clEnqueueNDRangeKernel,
			(cl_queue, test_kern,
					/*dimensions*/ 1, NULL, gdim, ldim,
					0, NULL, NULL));
#if 0
extern CL_API_ENTRY cl_int CL_API_CALL
clEnqueueNDRangeKernel(cl_command_queue /* command_queue */,
                       cl_kernel        /* kernel */,
                       cl_uint          /* work_dim */,
                       const size_t *   /* global_work_offset */,
                       const size_t *   /* global_work_size */,
                       const size_t *   /* local_work_size */,
                       cl_uint          /* num_events_in_wait_list */,
                       const cl_event * /* event_wait_list */,
                       cl_event *       /* event */) CL_API_SUFFIX__VERSION_1_0;
#endif

	// wait until the kernel is finished
	CALL_CL_GUARDED(clFinish, (cl_queue));

}

void calc_normals(cl_command_queue cl_queue, cl_kernel kern,
		cl_int dimension, cl_int buff_size, cl_mem * vertexes, cl_mem * normals, cl_float dx, cl_float dy, cl_float dz  ){
	CALL_CL_GUARDED(clFinish, (cl_queue));

	// set up parameters to the call
	size_t ldim[] = { 32 };
	size_t gdim[] = { buff_size };


	// set the arguments
	CALL_CL_GUARDED(clSetKernelArg, (kern, 0, sizeof(vertexes), vertexes));
	CALL_CL_GUARDED(clSetKernelArg, (kern, 1, sizeof(normals), normals));
	CALL_CL_GUARDED(clSetKernelArg, (kern, 2, sizeof(cl_int), &dimension));
	CALL_CL_GUARDED(clSetKernelArg, (kern, 3, sizeof(cl_int), &buff_size));
	CALL_CL_GUARDED(clSetKernelArg, (kern, 4, sizeof(cl_float), &dx));
	CALL_CL_GUARDED(clSetKernelArg, (kern, 5, sizeof(cl_float), &dy));
	CALL_CL_GUARDED(clSetKernelArg, (kern, 6, sizeof(cl_float), &dz));

	// enqueue the kernel (plus magic constant arrays copied from the example
	CALL_CL_GUARDED(clEnqueueNDRangeKernel,
			(cl_queue, kern,
					/*dimensions*/ 1, NULL, gdim, ldim,
					0, NULL, NULL));

	// wait until the kernel is finished
	CALL_CL_GUARDED(clFinish, (cl_queue));

}

int main( int argc, char ** argv){

	//////////////////////////////////////////////
	// HEIGHTMAP GENERATION
	//////////////////////////////////////////////

	rng_state_t rand;
//	seed(&rand, 12346, 12345);

    time_t currtime;

    time(&currtime);


    currtime = 5554;
    seed(&rand, (int) currtime, currtime * 3); // seed the RNG

	// do local memory allocation
	map2d * a = DSCreate(9, &rand);
//	map2d * a = new_map2d(4, 4);//DSCreate(3, &rand);

	cl_int dimension = a->height;

	map2d * b = new_map2d(dimension, dimension);

	cl_int buff_size = a->height * a->width;
    GLuint num_vertexes = dimension * dimension;

	// malloc space for the normals
	cl_float4 * normals = malloc(buff_size * sizeof(cl_float4));

	float height_scale = (float) dimension / (float) 4;


#ifdef DO_CL
	///////////////////////////////////
	// OPENCL SETUP
	///////////////////////////////////
	cl_int platform_number = 0;
	cl_int device_number = 0;


	cl_context context; // holds the context
	cl_command_queue cl_queue; // a command queue for the context

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

	// create a context
	cl_context_properties cps[3] = {
			CL_CONTEXT_PLATFORM, (cl_context_properties) plat, 0 };

	cl_int status;
	context = clCreateContext(
			NULL, 1, &dev, NULL, NULL, &status);
	CHECK_CL_ERROR(status, "clCreateContext");

	// free the device lists
	free(devices);
	free(platforms);


	// create a command queue
	cl_command_queue_properties qprops = 0;


	cl_queue = clCreateCommandQueue(context, dev, qprops, &status);
	CHECK_CL_ERROR(status, "clCreateCommandQueue");


	// load a kernel
	char * s_kernel = read_file("mytest.cl");
	cl_kernel test_kern = kernel_from_string( context, s_kernel, "action", NULL);
	free(s_kernel);

	s_kernel = read_file("calc_normals.cl");
	cl_kernel normal_kern = kernel_from_string( context, s_kernel, "calc_normals", NULL);
	free(s_kernel);

	//////////////////////////////////////////
	// RUN ONCE
	//////////////////////////////////////////





	// do GPU memory allocation
	cl_mem buf_a = clCreateBuffer(context, CL_MEM_READ_WRITE,
			sizeof(float) * buff_size, 0, &status);
	CHECK_CL_ERROR(status, "clCreateBuffer");
	cl_mem buf_b = clCreateBuffer(context, CL_MEM_READ_WRITE,
			sizeof(float) * buff_size, 0, &status);
	CHECK_CL_ERROR(status, "clCreateBuffer");

	// allocate memory for the normals
	cl_mem buf_normals = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
			sizeof(cl_float4) * buff_size, 0, &status);



	// Transfer the buffer to the device
	CALL_CL_GUARDED(clEnqueueWriteBuffer, (
			cl_queue, buf_a, /*blocking*/ CL_TRUE, /*offset*/ 0,
			sizeof(float) * buff_size, a->values,
	        0, NULL, NULL));

	// wait until the device is free
	CALL_CL_GUARDED(clFinish, (cl_queue));
#ifdef DO_AVERAGE

	size_t ldim[] = { 32 };
	size_t gdim[] = { buff_size };

	cl_average(cl_queue, test_kern, dimension, buff_size, &buf_a, &buf_b);

	// wait until the kernel is finished
	CALL_CL_GUARDED(clFinish, (cl_queue));

	// get the results back
	CALL_CL_GUARDED(clEnqueueReadBuffer, (
	        cl_queue, buf_b, /*blocking*/ CL_TRUE, /*offset*/ 0,
			buff_size * sizeof(float), b->values,
	        0, NULL, NULL));
#else
    for( size_t ii = 0; ii < num_vertexes; ii++){
    	b->values[ii] = a->values[ii];
    }
#endif // DO_AVERAGE


    // calculate the normals
    calc_normals(cl_queue, normal_kern, dimension, buff_size, &buf_a, &buf_normals, 1.0, height_scale, 1.0);

    // read them back
	CALL_CL_GUARDED(clEnqueueReadBuffer, (
	        cl_queue, buf_normals, /*blocking*/ CL_TRUE, /*offset*/ 0,
			buff_size * sizeof(cl_float4), normals,
	        0, NULL, NULL));
#endif // DO_CL
	/////////////////////////////////////////////
	// RENDERING
	/////////////////////////////////////////////

    //////////////////////////////////////////
    // OpenGL
    //////////////////////////////////////////

    // it turns out that you can't do SDL and GLFW together. Oh well.

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);


    GLFWwindow* gl_window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (gl_window == NULL)
    {
        puts( "Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(gl_window);


    // setup glew
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
    	puts( "Failed to initialize GLEW\n" );
        return -1;
    }

    // set up the rendering area
    int width, height;
    glfwGetFramebufferSize(gl_window, &width, &height);

    glViewport(0, 0, width, height);

    // set up the z buffer
    glEnable(GL_DEPTH_TEST);


    // set up blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    // link a callback function for keypresses
    glfwSetKeyCallback(gl_window, key_callback);

    // variables for error reporting
    GLint success;
    GLchar infoLog[512];


    ////////////////////////////////////////////////
    // LOAD SHADERS
    ////////////////////////////////////////////////

    // make a vertex shader object
    GLuint vertexShader = compile_shader("vertex.vert", GL_VERTEX_SHADER);

    // do the fragment shader stuff
    GLuint fragmentShader = compile_shader("fragment.frag", GL_FRAGMENT_SHADER);

    // link both of the shaders together by making a shader program
    GLuint shaderProgram;
    shaderProgram = glCreateProgram();

    // attach the two shaders to the program
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // check for errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        printf( "ERROR::PROGRAM::LINKING_FAILED\n%s", infoLog );
    }


    // get global locations
    // get the location of the rotation matrix in the uniform stuff
    GLint rotationLocation = glGetUniformLocation(shaderProgram, "model2world");
    GLint viewLocation = glGetUniformLocation(shaderProgram, "view");
    GLint projectionLocation = glGetUniformLocation(shaderProgram, "projection");
    GLint heightscale = glGetUniformLocation(shaderProgram, "heightscale");


    // delete the shaders and strings, as they are not needed anymore
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);



    // water shader
    vertexShader = compile_shader("water.vert", GL_VERTEX_SHADER);
    fragmentShader = compile_shader("water.frag", GL_FRAGMENT_SHADER);

    // link both of the shaders together by making a shader program
    GLuint water_shader;
    water_shader = glCreateProgram();

    // attach the two shaders to the program
    glAttachShader(water_shader, vertexShader);
    glAttachShader(water_shader, fragmentShader);
    glLinkProgram(water_shader);

    // check for errors
    glGetProgramiv(water_shader, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(water_shader, 512, NULL, infoLog);
        printf( "ERROR::PROGRAM::LINKING_FAILED\n%s", infoLog );
    }
    GLint rotationLocationWater = glGetUniformLocation(shaderProgram, "model2world");
    GLint viewLocationWater = glGetUniformLocation(shaderProgram, "view");
    GLint projectionLocationWater = glGetUniformLocation(shaderProgram, "projection");
    GLint heightscaleWater = glGetUniformLocation(shaderProgram, "heightscale");
    // delete the shaders and strings, as they are not needed anymore
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);


    ////////////////////////////////////////////
    // CREATE ARRAYS
    ////////////////////////////////////////////


    // make the array for indexes (VBO)
    vertex_data * vertexes = malloc(num_vertexes * sizeof( vertex_data));
    for( int ii = 0; ii < dimension; ii++){
    	for(int jj = 0; jj < dimension; jj++){
    		int index = ii * dimension + jj;
    		vertexes[index].coords.x = jj;
    		vertexes[index].coords.z = ii;
        	vertexes[index].coords.w = 0.0;
    	}
    }
    float * g_v = (float *)vertexes;

    // make the EBO
    GLuint num_triangles = (dimension - 1) * (dimension - 1) * 2;
    GLuint num_corners = num_triangles * 3;
    GLuint * ebo = malloc(num_corners * sizeof(GLuint) );
    int cur_vertex = 0;
    for( int yy = 0; yy < dimension - 1; yy++){
    	for(int xx = 0; xx < dimension - 1; xx++){
    		int vertex = yy * dimension + xx;

    		// get the indexes of x + 1 and y + 1 with wrapping (east and south)
			// south index
			int southy = (yy + 1);
			// east index
			int eastx = (xx + 1);
    		int index = cur_vertex * 6;

    		// first triangle
    		ebo[index] = vertex;
    		ebo[index + 1] = (southy * dimension) + xx;
    		ebo[index + 2] = (southy * dimension) + eastx;
    		// second triangle
    		ebo[index + 3] = (yy * dimension) + eastx;
    		ebo[index + 4] = vertex;
    		ebo[index + 5] = (southy * dimension) + eastx;

    		// increment the current vertex
    		cur_vertex ++;
    	}
    }


    ////////////////////////////////////////////////
    // SET UP VBO, VAO, AND EBO
    ////////////////////////////////////////////////

    // set up the Vertex Buffer Object (VBO)
    GLuint VBO;
    glGenBuffers(1, &VBO);  // create 1 VBO and assign it to the array VBO

    // create a vertex array object.
    GLuint VAO;
    glGenVertexArrays(1, &VAO);

    // bind the VAO
    glBindVertexArray(VAO);

    // set the type of the VBO to an array buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // write the vertices to the buffer
    glBufferData(GL_ARRAY_BUFFER, num_vertexes * sizeof(vertex_data), vertexes, GL_STREAM_DRAW);

    // set up the schema (pointers) for the data for the vertexes. not really, one VAO can point to multiple VBOs
    //           parameter  #values  type  Normalize?  size in bytes  offset in the buffer
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(vertex_data), (GLvoid*)0);
    glEnableVertexAttribArray(0); // enable that pointer in the VAO

    // normals
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(vertex_data), (GLvoid*)sizeof(cl_float4));
    glEnableVertexAttribArray(1); // enable that pointer in the VAO

    // make an Element Buffer Object
    GLuint EBO;
    glGenBuffers(1, &EBO);

    // bind it and copy data to the buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, num_corners * sizeof(GLuint), ebo, GL_STATIC_DRAW);


    // unbind the VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // unbind the VAO
    glBindVertexArray(0);


    // generate view things
    // make my rotation matrix
    float rotation = 0.0f;
    mat4x4 model2world;

    // make the view matrix
//    vec3 camera_position = {0.0f, 0.0f, -4.0f}; // made it a global for keypress callback becasue I'm a wimp.
    vec3 origin = {0.0f, 0.0f, 0.0f};
    vec3 up = {0.0f, 1.0f, 0.0f};
    vec3 nw_corner = {0.0f, 100.0f, 0.0f};

//    camera_position[3] = 200.0;

    mat4x4_look_at(view_matrix, camera_position, origin, up);

    // make the projection/perspective matrix
    mat4x4 proj_matrix;
    mat4x4_perspective(proj_matrix, 0.785398f, 800.0f/600.0f, 0.1f, 2000.0f);


#ifdef DO_AVERAGE
    // make some pointers for swapping the arrays
    cl_mem * buf_1 = &buf_a;
    cl_mem * buf_2 = &buf_b;
    cl_mem * swap_buff = NULL;
#endif

    // timing stuff
    struct timeval start, end;

    long mtime, seconds, useconds;

    gettimeofday(&start, NULL);
    gettimeofday(&end, NULL);
    seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;

    printf("Elapsed time: %ld milliseconds\n", mtime);



    while(!glfwWindowShouldClose(gl_window)){

        gettimeofday(&start, NULL);

        glfwPollEvents();

        // transfer the heightmap from the openCL buffer to the openGL one
        for( size_t ii = 0; ii < num_vertexes; ii++){
        	vertexes[ii].coords.y = b->values[ii];
        	vertexes[ii].coords.w = 0.0;
        	vertexes[ii].normal.x = normals[ii].x;
        	vertexes[ii].normal.y = normals[ii].y;
        	vertexes[ii].normal.z = normals[ii].z;
        	vertexes[ii].normal.w = normals[ii].w;
        }

        cl_float4 cur_norm = vertexes[buff_size - 1].normal;

        printf("norm1{%f, %f, %f, %f}\n", cur_norm.x, cur_norm.y, cur_norm.z, cur_norm.w);

//        for( int ii = 0; ii < num_corners; ii++){
//        	if(vertexes[ebo[ii]].y == 0.f){
//        		printf("zero at vertex %d ebo &d\n",ebo[ii], ii );
//        	}
//        }

        // transfer the new GL buffer
        // set the type of the VBO to an array buffer
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        // write the vertices to the buffer
        glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_STREAM_DRAW); // invalidate the old buffer
        glBufferData(GL_ARRAY_BUFFER, num_vertexes * sizeof(vertex_data), vertexes, GL_STREAM_DRAW);

		// unbind the VBO
		glBindBuffer(GL_ARRAY_BUFFER, 0);



        // render the current state

        // clear the screen
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer

        // pick the shader program to use
        glUseProgram(shaderProgram);

        // set up uniform vars
//        rotation -= 0.01f;
        create_Y_rot_mat4x4(model2world, rotation);
        glUniformMatrix4fv(rotationLocation, 1, GL_FALSE, (GLfloat *) model2world);

        // view matrix
//        mat4x4_identity(view_matrix); // reset the view matrix to the identity matrix
//        // translate it to the camera position
//        mat4x4_translate(view_matrix, camera_position[0], camera_position[1], camera_position[2]);
//        // rotate it to look in the camera position
//        mat4x4_rotate_Y(view_matrix, view_matrix, -camera_rotation);

        // make the eye point
        vec4 eye = {-1.0f, 0.0f, 0.0f, 0.0f};
        rotate_vec4_z(eye, camera_pitch);
//        printf("%f eyex{%f, %f, %f} ",camera_pitch, eye[0], eye[1], eye[2]);
        rotate_vec4_y(eye, camera_rotation);
//        printf("eyey{%f, %f, %f}\n", eye[0], eye[1], eye[2]);
        // add it to the camera position
        vec3_add(eye, eye, camera_position);
//        printf("eye{%f, %f, %f}\n", eye[0], eye[1], eye[2]);
        // look from it to the camera position
        mat4x4_look_at(view_matrix, eye, camera_position, up);


        glUniformMatrix4fv(viewLocation, 1, GL_FALSE, (GLfloat *) view_matrix);

        // projection matrix
//        glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, (GLfloat *) view_matrix);
        glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, (GLfloat *) proj_matrix);

        glUniform1f(heightscale, height_scale);


        // bind the VAO
        glBindVertexArray(VAO);
        // render with an EBO
//        glDrawElements(GL_LINE_LOOP, num_corners, GL_UNSIGNED_INT, 0);
        glDrawElements(GL_TRIANGLES, num_corners, GL_UNSIGNED_INT, 0);

        // unbind the VAO
        glBindVertexArray(0);
//        printtime();


        // render the water
        glBindVertexArray(VAO);
        glUseProgram(water_shader);
        glUniformMatrix4fv(rotationLocationWater, 1, GL_FALSE, (GLfloat *) model2world);
        glUniformMatrix4fv(viewLocationWater, 1, GL_FALSE, (GLfloat *) view_matrix);
        glUniformMatrix4fv(projectionLocationWater, 1, GL_FALSE, (GLfloat *) proj_matrix);
        glUniform1f(heightscaleWater, height_scale);
        glDrawElements(GL_TRIANGLES, num_corners, GL_UNSIGNED_INT, 0);
//        glDrawElements(GL_LINE_LOOP, num_corners, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);



//        glfwSwapInterval(1);
        glfwSwapBuffers(gl_window);


    	// recompute the next step
    	// wait until the device is free
#ifdef DO_AVERAGE
//		cl_average(cl_queue, test_kern, dimension, buff_size, buf_1, buf_2);
//
//
////    	// get the results back
//    	CALL_CL_GUARDED(clEnqueueReadBuffer, (
//    	        cl_queue, *buf_2, /*blocking*/ CL_TRUE, /*offset*/ 0,
//    			buff_size * sizeof(float), b->values,
//    	        0, NULL, NULL));
//    	CALL_CL_GUARDED(clFinish, (cl_queue));
//
//        gettimeofday(&end, NULL);

		// calculate the new normals
		calc_normals(cl_queue, normal_kern, dimension, buff_size, buf_1, &buf_normals, 1.0, height_scale, 1.0);
    	CALL_CL_GUARDED(clFinish, (cl_queue));

		// read them back
		CALL_CL_GUARDED(clEnqueueReadBuffer, (
				cl_queue, buf_normals, /*blocking*/ CL_TRUE, /*offset*/ 0,
				buff_size * sizeof(cl_float4), normals,
				0, NULL, NULL));
    	CALL_CL_GUARDED(clFinish, (cl_queue));

    	// swap the openCL buffers
//    	swap_buff = buf_1;
//    	buf_1 = buf_2;
//    	buf_2 = swap_buff;

#endif

    	// Do the timing
        gettimeofday(&end, NULL);

        seconds  = end.tv_sec  - start.tv_sec;
        useconds = end.tv_usec - start.tv_usec;

        mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;

//        printf("Elapsed time: %ld milliseconds\n", mtime);


    }


}


















































