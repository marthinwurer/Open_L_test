/*
 * gl_structs.h
 *
 *  Created on: Mar 6, 2017
 *      Author: benjamin
 */

#ifndef GL_SETUP_H_
#define GL_SETUP_H_

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>
// GLFW
#include <GLFW/glfw3.h>

typedef struct shader_data_s{
    GLuint shaderProgram;
    GLint rotationLocation;
    GLint viewLocation;
    GLint projectionLocation;
    GLint heightscale;
    GLint camera_pos;

} shader_data;

typedef struct gl_global_data_s{
	GLFWwindow* gl_window;

};

GLuint compile_shader(const char * filename, GLuint shader_type);

#endif /* GL_SETUP_H_ */
