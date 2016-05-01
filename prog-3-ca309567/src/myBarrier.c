/*
 * Projet Programmation Concurrente - annee 2015-2016
 * Polytech Nice-Sophia - SI4
 * 
 * Auteurs		:	Aurelien COLOMBET	(ca309567) 
 * 
 * Creation		: 	1 avril 2016
 * Modification	: 	1 avril 2016
 * 
 */

#include <stdio.h>
#include "myBarrier.h"


int myBarrier_init(myBarrier *barr, int nbThreads){
	pthread_mutex_init(&barr->mutex, NULL);
	pthread_cond_init(&barr->condvar, NULL);
	barr->count = nbThreads;
	barr->total = 0;
	return 0;
}

int myBarrier_wait(myBarrier *barr){
	pthread_mutex_lock(&barr->mutex);

	barr->total++;

	if (barr->total == barr->count){
		barr->total = 0;
		pthread_cond_broadcast(&barr->condvar);

		pthread_mutex_unlock(&barr->mutex);
		return -1;
	}
	else{
		pthread_cond_wait(&barr->condvar, &barr->mutex);

		pthread_mutex_unlock(&barr->mutex);

		return 0;
	}
}

int myBarrier_destroy(myBarrier *barr){
	pthread_cond_destroy(&barr->condvar);
	pthread_mutex_destroy(&barr->mutex);
}
