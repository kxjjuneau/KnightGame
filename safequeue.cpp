/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:        project 6: Rabbit game
//  Filename:       safequeue.cpp
//  Description:    function definitions for safequeue template class
//  Author:         Joseph Juneau (juneau@etsu.edu)
//  Created:        Thursday, 4/24/2020
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <iostream>
#include <string>
#include <pthread.h>
#include <semaphore.h>
#include "safequeue.h"

//initialize mutex and sem
template<class T> safequeue<T>::safequeue()
{
	sem_init(&messageSem, 0, 0);
	messageLock = PTHREAD_MUTEX_INITIALIZER;
}

//push T onto queue
template<class T>
void safequeue<T>::enqueue(T message)
{
	pthread_mutex_lock(&messageLock);
	messagequeue.push(message);
	pthread_mutex_unlock(&messageLock);
	sem_post(&messageSem);
}

//pop and return T from queue
template<class T>
T safequeue<T>::dequeue()
{
	sem_wait(&messageSem);
	pthread_mutex_lock(&messageLock);
	T message = messagequeue.front();
	messagequeue.pop();
	pthread_mutex_unlock(&messageLock);
	return message;
}

//return true if the queue is empty
template<class T>
bool safequeue<T>::empty()
{
	return messagequeue.empty();
}