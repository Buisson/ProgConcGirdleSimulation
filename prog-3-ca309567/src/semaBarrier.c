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

#include "semaBarrier.h"

int semaBarrier_init(semaBarrier *barr, int nbThreads){
	sem_init(&barr->mutex,0,1);
	sem_init(&barr->waiting,0,0);
	sem_init(&barr->leave,0,0);
	barr->count = 0;
	barr->total = nbThreads;
	return 0;
}

int semaBarrier_wait(semaBarrier *barr){
	sem_wait(&barr->mutex);
	barr->count++;
	if(barr->count == barr->total){
		int i;
		for(i=0;i<(barr->total-1);i++){
			sem_post(&barr->waiting);
		}
		for(i=0;i<(barr->total-1);i++){
			sem_wait(&barr->leave);
		}

		barr->count=0;
		sem_post(&barr->mutex);
		return -1;
	}
	else{
		sem_post(&barr->mutex);
		sem_wait(&barr->waiting);
		sem_post(&barr->leave);
		return 0;
	}
}

int semaBarrier_destroy(semaBarrier *barr){
	sem_destroy(&barr->mutex);
	sem_destroy(&barr->waiting);
	sem_destroy(&barr->leave);
}