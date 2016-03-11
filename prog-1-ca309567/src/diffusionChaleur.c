/*
 * Projet Programmation Concurrente - annee 2015-2016
 * Polytech Nice-Sophia - SI4
 * 
 * Auteurs		:	Aurelien COLOMBET	(ca309567) 
 					Quentin SALMERON	(sq000508)
 * 
 * Creation		: 	02 février 2016
 * Modification	: 	21 février 2016
 * 
 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h> //Pour les threads et barrier POSIX
#include "matrixUtils.h"

#define TEMP_FROID 0.00
#define TEMP_CHAUD 256.00

 typedef struct argument_barriere{
	float* mat1;
	float* mat2;
	int size;
	int minI;
	int maxI;
	int minJ;
	int maxJ;
 }argument_barriere;

 pthread_barrier_t barrierHorizontal;//barriere POSIX pour l'etape du calcul horizontal
 pthread_barrier_t barrierVertical;//barriere POSIX pour l'etape du calcul vertical
 pthread_barrier_t barrierFillCenter;//barriere POSIX pour l'etape de la remise du centre aux valeurs TEMP_CHAUD

InfoThread* tab;

// Calculer la moyenne d'un tableau de floats en supprimant le minimum et le maximum
float getAverageClockWithoutExtremes(float* clocks, int size) {
 	float min;
 	float max;
 	float average = 0.0f;

 	int i = 0;
 	int value = clocks[i];
 	min = value;
 	max = value;
 	value = ++i;

 	if (value > max) {
 		max = value;
 	} else {
 		min = value;
 	}
 	i++;

 	for (; i < size; i++) {
 		value = clocks[i];
 		if (value > max) {
 			average += max;
 			max = value;
 		} else if (value < min) {
 			average += min;
 			min = value;
 		} else {
 			average += value;
 		}
 	}
 	return average / (float)(size - 2);
 }

// Calculer la prochaine etape de la diffusion (methode iterative)
void nextStep(float* mat1, float* mat2, int size){
	int i;
	int j;

	// Premiere etape : calcul horizontal
	for (i = 1; i < size - 1; i++) {
		for (j = 1; j < size - 1; j++) {
			mat2[j * size + i] = (mat1[(j - 1) * size + i] / 6.0)
				+ (mat1[(j + 1) * size + i] / 6.0)
				+ (mat1[j * size + i]) * 2.0 / 3.0;
		}
	}

	// Deuxieme etape : calcul vertical
	for (i = 1; i < size - 1; i++) {
		for (j = 1; j < size; j++) {
			mat1[j * size + i] = (mat2[j * size + i - 1] / 6.0)
				+ (mat2[j * size + i + 1] / 6.0)
				+ (mat2[j * size + i]) * 2.0 / 3.0;
		}
	}

    fillCenter(mat1, size);
}

void* calculSubMatrixHorizontal(void * arguments){
	argument_barriere *args = arguments;
	int i,j;

	int debi = args->minI;
	int fini = args->maxI;
	int debj = args->minJ;
	int finj = args->maxJ;
	int size = args->size;
	float* mat1 = args->mat1;
	float* mat2 = args->mat2;
	// Premiere etape : calcul horizontal
	for (i = debi; i <= fini; i++) {
		for (j = debj; j <= finj; j++) {
			if(i<(size-1) && j!=(size-1)){
				mat2[j * size + i] = (mat1[(j - 1) * size + i] / 6.0)
				+ (mat1[(j + 1) * size + i] / 6.0)
				+ (mat1[j * size + i]) * 2.0 / 3.0;
			}
		}
	}
	
	pthread_barrier_wait (&barrierHorizontal);
	pthread_exit(NULL);
	return 0;
}

void* calculSubMatrixVertical(void * arguments){
	argument_barriere *args = arguments;
	int i,j;
	int size = args->size;
	float* mat1 = args->mat1;
	float* mat2 = args->mat2;
	int debi = args->minI;
	int fini = args->maxI;
	int debj = args->minJ;
	int finj = args->maxJ;
	// Deuxieme etape : calcul vertical
	for (i = debi; i <= fini; i++) {
		for (j = debj; j <= finj; j++) {
			if(i< (size-1) && j!=(size-1)){
				mat1[j * size + i] = (mat2[j * size + i - 1] / 6.0)
				+ (mat2[j * size + i + 1] / 6.0)
				+ (mat2[j * size + i]) * 2.0 / 3.0;
			}
		}
	}
	pthread_barrier_wait (&barrierVertical);
	pthread_exit(NULL);
	return 0;
}

// Calculer la prochaine etape de la diffusion (methode avec barrier POSIX)
void nextStepBarrier(float* mat1, float* mat2, int size, int nbThreads){
	int indThread;
	pthread_t *thread_id;
	if ( (thread_id = malloc(sizeof(pthread_t)*(nbThreads+1))) == NULL ){
		fprintf(stderr,"Allocation impossible \n");
		exit(EXIT_FAILURE);
 	}
	if(pthread_barrier_init(&barrierHorizontal, NULL, nbThreads+1)!=0){
		printf("Barrier fail to init\n");
	}

	for(indThread=0;indThread<nbThreads;indThread++){
		argument_barriere* args;
		if( (args = malloc(sizeof(argument_barriere))) == NULL){
			fprintf(stderr,"Allocation impossible \n");
			exit(EXIT_FAILURE);
		}
		args->mat1=mat1;
		args->mat2=mat2;
		args->size=size;
		args->minI=tab[indThread].iMin;
		args->maxI=tab[indThread].iMax;
		args->minJ=tab[indThread].jMin;
		args->maxJ=tab[indThread].jMax;

		if(pthread_create (&thread_id[indThread], NULL, &calculSubMatrixHorizontal, (void *)args) !=0){
			printf("[ERREUR] Creation de la thread fail\n");
		}
	}
	//printf("ATTENTE PREMIERE BARRIERE....\n");
	pthread_barrier_wait (&barrierHorizontal);
	for(indThread=0;indThread<nbThreads;indThread++)
		pthread_join(thread_id[indThread],NULL);//On fait un join de toutes les thread.
	//printf("FIN ATTENTE PREMIERE BARRIERE\n");

	pthread_barrier_destroy(&barrierHorizontal);
	if(pthread_barrier_init(&barrierVertical, NULL, nbThreads+1)!=0){
		printf("Barrier fail to init\n");
	}

	for(indThread=0;indThread<nbThreads;indThread++){
		argument_barriere* args;
		if( (args = malloc(sizeof(argument_barriere))) == NULL){
			fprintf(stderr,"Allocation impossible \n");
			exit(EXIT_FAILURE);
		}
		args->mat1=mat1;
		args->mat2=mat2;
		args->size=size;
		args->minI=tab[indThread].iMin;
		args->maxI=tab[indThread].iMax;
		args->minJ=tab[indThread].jMin;
		args->maxJ=tab[indThread].jMax;

		int errorPthread;
		if(errorPthread = pthread_create (&thread_id[indThread], NULL, &calculSubMatrixVertical, (void *)args) != 0){
			fprintf(stderr, "error: pthread_create, rc: %d\n", errorPthread);
		}
	}
	//printf("ATTENTE DEUXIEME BARRIERE....\n");
	pthread_barrier_wait (&barrierVertical);
	for(indThread=0;indThread<nbThreads;indThread++)
		pthread_join(thread_id[indThread],NULL);//On fait un join de toutes les thread.
	//printf("FIN DEUXIEME BARRIERE\n");
	pthread_barrier_destroy(&barrierVertical);

	pthread_barrier_init(&barrierFillCenter, NULL, 1);
	fillCenter(mat1, size);
	pthread_barrier_wait(&barrierFillCenter);
	pthread_barrier_destroy(&barrierFillCenter);
}

int main(int argc, char** argv){
	printf("DEBUT DU PROGRAMME\n");
	int opt;	
	int* s;//listes des tailles
	int* e;//liste des etapes
	int* t = NULL;//liste des nombres de threads
	
	// Declaration des options
	int i = 0;
	int m = 0, M = 0, a = 0;

	int nbProblems = 0;
	int nbEtapes = 0;
	int nbDifferentThreads = 0;

	// Recuperer les options du programme
	while ((opt = getopt(argc,argv,"s:mMai:e:t:")) != -1){
		switch(opt){
			// [0-9] : taille du probleme a executer, 2**(s+4) cases sur une ligne
			case 's':
				// Verifier si l'option contient des valeurs non autorisees
				if (atoi(optarg) == 0) {
					optarg = "0";
				} 
				// Recuperer le nombre de problemes a traiter et leur taille, puis initialiser le tableau
				nbProblems = strlen(optarg);
				if( (s = malloc(sizeof(int) * nbProblems)) == NULL){
					fprintf(stderr,"Allocation impossible \n");
					exit(EXIT_FAILURE);
				}
				int j = 0;
				for(j = 0; j < nbProblems; j++) {
					s[j] = optarg[j] - '0';
				}
				break;
			
			// mesure et affichage du temps d'execution (consommation du CPU),
			//	suppression de toute trace d'execution
			case 'm':
				// on executera 10 fois chaque scenario puis on eliminera les deux
				//	extremes pour faire la moyenne des 8 executions restantes
				m = 1;
				if (a) {
					a = 0;
				}
				break;

			// mesure et affichage du temps d'execution (temps de reponse utilisateur),
			//	suppression de tout trace d'executuion
			case 'M':
				// on executera 10 fois chaque scenario puis on eliminera les deux
				//	extremes pour faire la moyenne des 8 executions restantes
				M = 1;
				if (a) {
					a = 0;
				}
				break;

			// affichage de la temperature initiale et de la temperature finale pour les indices
			//	correspondant au quart superieur gauche du tableau, pour les indices modulo 2**s
			case 'a':
				// par defaut : on n'affiche rien
				if (!(m || M)) {
					a = 1;
				}
				break;

			// number : nombre d'iteration a executer
			case 'i':
				// par defaut 10 000 iterations
				i = atoi(optarg);
				if (i == 0) {
					i = 10000;
				}
				break;

			// [0-5] : etape du programme a executer
			case 'e':
				// 0 : iteratif
				// 1 : thread avec barriere Posix
				// 2 : thread avec barriere implementee avec variable condition
				// 3 : thread avec barriere implementee avec semaphore
				// 4 : programme OpenCL calcul sur CPU
				// 5 : programme OpenCL calcul sur GPU
								// Verifier si l'option contient des valeurs non autorisees
				if (atoi(optarg) == 0) {
					optarg = "0";
				} 
				// Recupere les etapes du programme a executer
				nbEtapes = strlen(optarg);
				
				if( (e = malloc(sizeof(int) * nbEtapes)) == NULL){
					fprintf(stderr,"Allocation impossible \n");
					exit(EXIT_FAILURE);
				}

				int k = 0;
				for(k = 0; k < nbEtapes; k++) {
					e[k] = optarg[k] - '0';
				}
				break;

			// [0-5] : nombre de thread a creer
			case 't':
				// 4**t thread a creer, t varie entre 0 (iteratif) et 5 (1024 threads)
				// concerne bien evidemment les etapes 1 a 5, sans effet sur etape 0
				nbDifferentThreads = strlen(optarg);
				if( (t = malloc(sizeof(int) * nbDifferentThreads)) == NULL){
					fprintf(stderr,"Allocation impossible \n");
					exit(EXIT_FAILURE);
				}
				int l;
				for(l=0;l<nbDifferentThreads;l++){
					t[l] = optarg[l] - '0';
				}
				break;
		}
	}

	int j, k, l;
	int size = 0;
	float** mat1;
	float** mat2;
	int iteration = 0;
	int n = 0;


	time_t debutTemp, finTemp;
	clock_t start_t, end_t;

	//iteration sur le nombre d'etapes a executer (option -e)
	for(l = 0 ; l<nbEtapes ; l++){
		// Iteration sur le nombre de problemes a lancer (otpion -s)
		for (j = 0; j < nbProblems; j++) {
			n = s[j] + 4;
			size = (1 << n) + 2;
			// Allocation memoire des matrices a utiliser
			
			if( (mat1 = (float**) malloc(sizeof(float*) * size)) == NULL){
				fprintf(stderr,"Allocation impossible \n");
				exit(EXIT_FAILURE);
			}
			
			for (k = 0; k < size; k++) {
				if( (mat1[k] = (float*) malloc(sizeof(float) * size)) == NULL){
					fprintf(stderr,"Allocation impossible \n");
					exit(EXIT_FAILURE);
				}
			}
			mat2 = (float**) malloc(sizeof(float*) * size);
			for (k = 0; k < size; k++) {
				if( (mat2[k] = (float*) malloc(sizeof(float) * size)) == NULL){
					fprintf(stderr,"Allocation impossible \n");
					exit(EXIT_FAILURE);
				}
			}

			// Initialisation des matrices
			defineConstants(size, n);
			initializeMatrix(*mat1, size);
			initializeMatrix(*mat2, size);

			int counter = 1;
			float* times;
			float* clocks;

			// Si l'option m ou M a ete entree, le compteur est mis a 10 pour calculer la moyenne de temps
			if (m) {
				counter = 10;
				if( (clocks = malloc(sizeof(float) * counter)) == NULL){
					fprintf(stderr,"Allocation impossible \n");
					exit(EXIT_FAILURE);
				}
			}
			if (M) {
				counter = 10;
				if( (times = malloc(sizeof(float) * counter)) == NULL){
					fprintf(stderr,"Allocation impossible \n");
					exit(EXIT_FAILURE);
				}
			}
			int indexThread;
			int endAllThreads=nbDifferentThreads;
			if(t==NULL || e[l]==0){endAllThreads=1;}//pour passer une fois dans la boucle for.
			for(indexThread=0;indexThread<endAllThreads;indexThread++){
				int numberOfThreads = (1 << (2*t[indexThread]));//pow(4,t[indexThread]);
				for (k = 0; k < counter; k++) {
					if (m) {
						start_t = clock();
					}
					if (M) {
						debutTemp = time(NULL);
					}
					switch(e[l]){
						case 0:
							printf("Execution de l'etape 0 ...[size matrix : %d]\n",size-2);
							//printf("AVANT\n");
							//printMatrix(*mat1,size);
							for (iteration = 0; iteration < i; iteration++) {
								nextStep(*mat1, *mat2, size);
							}
							//printf("APRES\n");
							//printMatrix(*mat1,size);
						break;

						case 1:
							printf("Execution de l'etape 1 ...[size matrix : %d, nb Threads : %d]\n",size-2,numberOfThreads);

							//printf("AVANT\n");
							//printMatrix(*mat1,size);
							tab = decoupageMatrice(numberOfThreads,size-2);
							for (iteration = 0; iteration < i; iteration++) {
								nextStepBarrier(*mat1, *mat2, size, numberOfThreads);
							}
							//printf("APRES\n");
							//printMatrix(*mat1,size);
						break;

						case 2:
							printf("Execution de l'etape 2 ...\n");
						break;

						case 3:
							printf("Execution de l'etape 3 ...\n");
						break;

						case 4:
							printf("Execution de l'etape 4 ...\n");
						break;
						case 5:
							printf("Execution de l'etape 5 ...\n");
						break;					

						default:
							//ne rien faire
							printf("Veuillez entrer une valeur correcte pour l'option -e\n");
						break;
					}
					if (m) {
						end_t = clock();
						clocks[k] = (float)(end_t - start_t) / CLOCKS_PER_SEC;
					}
					if (M) {
						finTemp = time(NULL);
						times[k] = (float)(finTemp - debutTemp);				
					}
				}
				if (a) {
					printf("PRINT QUARTER\n");
					printQuarterMatrix(*mat1, size);
				}

				if (m) {
					printf("Average clock for s = %d : %f\n", s[j], getAverageClockWithoutExtremes(clocks, counter));
				}
				if (M) {
					printf("Average time for s = %d : %f\n", s[j], getAverageClockWithoutExtremes(times, counter));
				}
			}
		}
	}

	return 0;
}