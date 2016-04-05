#	Projet Programmation Concurrente - annee 2015-2016
#	Polytech Nice-Sophia - SI4
#	
#	Auteurs					:	Aurelien COLOMBET	(ca309567)
#	
#	Date de modification	:	13 mars 2016
#
for i in 0 2 4 #Temporaire car segfault declenche si je passe plusieurs taille à mon programme.(pas le temps de trouver l'erreur).
do
	bin/diffusionChaleur -i 1000 -e 01 -s $i -t 13 -m
done
