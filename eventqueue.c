/*
 * eventqueue.c
 *
 *  Created on: Feb 15, 2017
 *      Author: benjamin
 */

#include "eventqueue.h"
/**
 *
 */

eventqueue * create_eventqueue(){
	eventqueue * toreturn = malloc(sizeof(eventqueue));
	toreturn->head = NULL;
	toreturn->size = 0;
	return toreturn;
}

int isempty(eventqueue * queue){
	return queue->size;
}
void enqueue(eventqueue * queue, key_event * data){
	// malloc the node
	event_node * toadd = malloc(sizeof(event_node));
	toadd->next = NULL;
	toadd->data = data;
	event_node ** current = &queue->head;
	while( *current != NULL){
		current = &(*current)->next;
	}
	(*current) = toadd;
	queue->size ++;
}
key_event * dequeue(eventqueue * queue){
	if(queue->head == NULL || queue->size <= 0) return NULL;
	event_node * tofree = queue->head;
	key_event * toreturn = tofree->data;
	queue->head = tofree->next;
	free(tofree);
	queue->size--;
	return toreturn;
}
