#	Projet Programmation Concurrente - annee 2015-2016
#	Polytech Nice-Sophia - SI4
#	
#	Auteurs					:	Aurelien COLOMBET	(ca309567)
#	
#	Date de modification	:	1 Mai 2016
#

for j in 0 1 2 3
do
	for i in 0 2 4 #Temporaire car segfault declenche si je passe plusieurs taille à mon programme.(pas le temps de trouver l'erreur).
	do
			bin/diffusionChaleur -i 10000 -e $j -s $i -t 13 -m
	done
done
