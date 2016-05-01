#	Projet Programmation Concurrente - annee 2015-2016
#	Polytech Nice-Sophia - SI4
#	
#	Auteurs					:	Aurelien COLOMBET	(ca309567) 
#	
#	Date de modification	:	13 mars 2016
#
diffusionChaleur : src/diffusionChaleur.o
	mkdir -p bin
	gcc src/diffusionChaleur.c src/matrixUtils.c src/myBarrier.c src/semaBarrier.c -lm -lpthread -o bin/diffusionChaleur
	mv src/diffusionChaleur.o bin
clean : 
	rm -f bin/diffusionChaleur.o bin/diffusionChaleur
	rmdir bin
