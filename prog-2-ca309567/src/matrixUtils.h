/*
 * Projet Programmation Concurrente - annee 2015-2016
 * Polytech Nice-Sophia - SI4
 * 
 * Auteurs		:	Aurelien COLOMBET	(ca309567) 
 * 
 * Creation		: 	2 mars 2016
 * Modification	: 	13 mars 2016
 * 
 */

#ifndef MATRIXUTILS_H_
#define MATRIXUTILS_H_

typedef struct InfoThread{
	int iMin;
	int iMax;
	int jMin;
	int jMax;
}InfoThread;

// Definir les constantes du programme
void defineConstants(int size, int n);
// Afficher une matrice de floats 
void printMatrix(float* mat, int size);
// Afficher le quart superieur gauche de la matrice
void printQuarterMatrix(float* mat, int size);
// Initialiser la matrice avec les TEMP_CHAUD et TEMP_FROID
void initializeMatrix(float* mat, int size);
// Remplir le centre de la matrice avec TEMP_CHAUD
void fillCenter(float* mat, int size);
// Decoupe la matrice en sous matrices selon le nombres de threads (remplit la struct InfoThread qui contient les indices des sous matrices).
InfoThread* decoupageMatrice(int nbThread, int sizeMatrice);

#endif