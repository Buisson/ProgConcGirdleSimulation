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
#include <pthread.h> //Pour les threads et barrier POSIX

#ifndef MYBARRIER_H_
#define MYBARRIER_H_

typedef struct myBarrier{
	int count;
	int total;
	pthread_mutex_t mutex;
	pthread_cond_t condvar;
}myBarrier;


int myBarrier_init(myBarrier *barr, int nbThreads);

int myBarrier_wait(myBarrier *barr);

int myBarrier_destroy(myBarrier *barr);


#endif