/*
 * eventqueue.h
 *
 *  Created on: Feb 15, 2017
 *      Author: benjamin
 */

#ifndef EVENTQUEUE_H_
#define EVENTQUEUE_H_

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>
// GLFW
#include <GLFW/glfw3.h>

#include <stdlib.h>

typedef struct key_event{
	GLFWwindow* window;
	int key;
	int scancode;
	int action;
	int mode;
} key_event;


typedef struct event_node{
	struct event_node * next;
	key_event * data;
} event_node;

typedef struct eventqueue{
	int size;
	event_node * head;
} eventqueue;

eventqueue * create_eventqueue();

int isempty(eventqueue * queue);
void enqueue(eventqueue * queue, key_event * data);
key_event * dequeue(eventqueue * queue);
key_event * next_event(eventqueue * queue);


#endif /* EVENTQUEUE_H_ */
