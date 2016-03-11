#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "matrixUtils.h"

#define TEMP_FROID 0.00
#define TEMP_CHAUD 256.00

int centreInf, centreSup;

// Definir les constantes du programme
void defineConstants(int size, int n) {
	centreInf = (int) (pow(2, n - 1) - pow(2, n - 4)) + 1;
	centreSup = (int) (pow(2, n - 1) + pow(2, n - 4)) + 1;
}

// Afficher une matrice de floats 
void printMatrix(float* mat, int size) {
	int i, j;	
	for(i = 0; i < size; i++) {
		for(j = 0; j < size; j++) {
			printf("%06.2f |",mat[size * j + i]);
		}
		printf("\n");
	}
}

// Afficher le quart superieur gauche de la matrice
void printQuarterMatrix(float* mat, int size) {
	int quarter = size / 2;
	int i, j;
	for (i = 0; i < quarter; i++) {
		for (j = 0; j < quarter; j++) {
			printf("%06.2f |", mat[size * j + i]);
		}
		printf("\n");
	}
}

// Initialiser la matrice avec les TEMP_CHAUD et TEMP_FROID
void initializeMatrix(float* mat, int size){
	int i, j;
	for(i = 0; i < size; i++) {
		for(j = 0; j < size; j++) {
			if((i >= centreInf) && (i < centreSup) && (j >= centreInf) && (j < centreSup)) {
				mat[i + j * size] = TEMP_CHAUD;
			} else {
				mat[i + j * size] = TEMP_FROID;
			}
		}
	}
}

// Remplir le centre de la matrice avec TEMP_CHAUD
void fillCenter(float* mat, int size) {
	int i = centreInf;
	int j = centreSup;
    for (i = centreInf; i < centreSup; i++) {
        for (j = centreInf; j < centreSup; j++) {
            mat[i + j * size] = TEMP_CHAUD;
        }
    }
}


InfoThread* decoupageMatrice(int nbThread, int sizeMatrice){
	int nbLigColThread = sqrt(nbThread);
	int nbLigColMatrice = sizeMatrice;
	int nbCelluleParSectionLigCol = 0;

	if(nbLigColMatrice < nbLigColThread){
		printf("Le nombre de threads demander (%d) est trop important pour le nombre de cellule de la plaque (%d). Creation de %d thread.\n",nbLigColThread,nbLigColMatrice,nbLigColMatrice );
		nbCelluleParSectionLigCol = nbLigColMatrice;
	}
	else if(nbLigColMatrice == nbLigColThread){
		nbCelluleParSectionLigCol = nbLigColMatrice;
	}
	else{
		nbCelluleParSectionLigCol = nbLigColMatrice/nbLigColThread;
	}
	// le nombre de section d'une ligne ou d'une collonne de la plaque.
	int nbSectionLigCol = nbLigColMatrice/nbCelluleParSectionLigCol;

	int nbSection = nbSectionLigCol*nbSectionLigCol;

	// On creer un tableau qui contient les info de chaque thread creer
	InfoThread* tab;
	if ((tab = malloc(sizeof(InfoThread)*nbSection)) == NULL){
		fprintf(stderr,"Allocation impossible \n");
		exit(EXIT_FAILURE);
	}


	int pas = nbCelluleParSectionLigCol;
	int indiceTab = 0;
	int i;
	int y;
	for(i = 1 ; i < sizeMatrice ; i = i + pas){
		for(y = 1 ; y < sizeMatrice ; y = y + pas){
			tab[indiceTab].iMin = i;
			tab[indiceTab].iMax = i + pas - 1;
			tab[indiceTab].jMin = y;
			tab[indiceTab].jMax = y + pas - 1;
			//printf("xmin : %d, xmax : %d, ymin : %d , ymax : %d\n",tab[indiceTab].iMin,tab[indiceTab].iMax,tab[indiceTab].jMin,tab[indiceTab].jMax);
			indiceTab++;
		}
	}

	return tab;
}