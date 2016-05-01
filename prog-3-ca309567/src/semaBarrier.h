/*
 * Projet Programmation Concurrente - annee 2015-2016
 * Polytech Nice-Sophia - SI4
 * 
 * Auteurs		:	Aurelien COLOMBET	(ca309567) 
 * 
 * Creation		: 	24 avril 2016
 * Modification	: 	29 avril 2016
 * 
 */

 #include <semaphore.h>

#ifndef SEMABARRIER_H_
#define SEMABARRIER_H_

typedef struct semaBarrier{
	int count;
	int total;
	sem_t mutex;
	sem_t waiting;
	sem_t leave;
}semaBarrier;


int semaBarrier_init(semaBarrier *barr, int nbThreads);

int semaBarrier_wait(semaBarrier *barr);

int semaBarrier_destroy(semaBarrier *barr);


#endif