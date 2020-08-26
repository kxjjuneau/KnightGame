/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:        project 6: Rabbit game
//  Filename:       safequeue.h
//  Description:    header for template class of safequeue
//  Author:         Joseph Juneau (juneau@etsu.edu)
//  Created:        Thursday, 4/24/2020
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef SAFEQUEUE_H
#define SAFEQUEUE_H
#include <stdio.h>
#include <iostream>
#include <string>
#include <queue>
#include <pthread.h>
#include <semaphore.h>
using namespace std;

template <class T>
class safequeue
{
private:
	queue<T> messagequeue;
	sem_t messageSem;
	pthread_mutex_t messageLock;

public:
	safequeue();
	void enqueue(T message); //enqueue message with thread saftey
	T dequeue(); //dequeue message with thread saftey
	bool empty();

};
#endif

