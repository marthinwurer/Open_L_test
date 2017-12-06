/*
 * gl_setup.c
 *
 *  Created on: Mar 7, 2017
 *      Author: benjamin
 */

#include "gl_setup.h"
#include "my_cl_helper.h"
#include <stdlib.h>
#include <stdio.h>

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


