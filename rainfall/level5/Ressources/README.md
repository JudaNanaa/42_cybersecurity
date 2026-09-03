# level 5 #

On a deux fonctions interessantes : ```o()```, qui appelle ```system("/bin/sh")```, et ```n()```, qui recupere notre input via ```fgets()``` puis fait ```printf(buffer)``` sans format explicite : encore une faille de format string.

Contrairement aux levels precedents, il n'y a ici aucune variable globale a atteindre : juste apres le ```printf()```, ```n()``` appelle directement ```exit(1)```, sans aucune condition a satisfaire.

On va donc exploiter le format string autrement : au lieu d'ecrire dans une variable, on recupere l'adresse de ```exit()``` en faisant ```objdump -R intel level5```, et on y ecrit l'adresse de ```o()```. Comme pour les levels precedents, on place l'adresse cible (celle de ```exit()```) au debut de notre input, puis on utilise un specificateur de largeur et des ```%x``` pour ajuster le compteur de caracteres car l'objectif c'est que le nombre de caractere print converti en hexadecimale soit egale a l'adresse de ```o()``` . On termine par ```%n``` pour ecrire l'adresse de ```o()``` a l'adresse de ```exit()```.

Quand ```n()``` appelle ensuite ```exit()```, c'est en realite ```o()``` qui est executee a la place (puisque le processeur va chercher l'adresse de ```exit()``` qu'on vient de remplacer par celle de ```o```), et un shell est pop !
