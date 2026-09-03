# level 7 #

Lorsqu'on examine l'assembleur, on remarque de nombreux calls a ```malloc()```, 4 pour etre exact, 2 calls a ```strcpy()```, un call a ```fopen()```, un call a ```fgets()``` et enfin un call a ```puts()```.

De meme, on remarque une fonction ```m()``` qui call ```time()``` et ```printf()```. 

En fait le code fait 4 mallocs: 


- Il stock l'adresse du 2eme malloc dans le premier
- Il stock l'adresse du 4eme malloc dans le troisieme
- L'adresse du deuxieme malloc, recuperee dans le premier, sert de dest au premier ```strcpy()```
- L'adresse du quatrieme malloc, recupere dans le troisime, sert de dest au deuxieme ```strcpy()```

On sait que ```strcpy()``` n'est pas protege, et que les adresses retorunees par malloc dans la heap se suivent. On peut dopnc l'exploiter: on va utiliser le premier ```strcpy()``` pour reecrire par dessus l'adresse stockee dans le 3eme ```malloc()```, en mettant 20 chars de padding. Cette adresse ne sera donc plus l'adresse du 4eme ```malloc()```, mais l'adresse d'une fonction. 

Quand le deuxieme ```strcpy()``` sera appele, il n'utilisera donc plus comme dest l adresse du 4eme ```malloc()```, mais l'adresse de ladite fonction, et reecrira ce qu'on voudra dedans.

Mais alors, quelle fonction ?

Quand on regarde le code, on s'appercoit que ```fopen()``` va ouvrir le contenu qu'on veut voir apparaitre : le .pass du level8. Sauf qu'il n'apparait jamais car l'output est recupere par ```fgets()```, puis ```puts()``` print toujours le meme contenu inutile. 

On veut donc faire en sorte que ce ne soit plus ```puts()```, mais la fonction ```m()``` qui soit appelee, dont on connait l'adresse, et qui appellera ```printf()```, sur l'adresse memoire remplie par ```fgets()```. 

On ecrit donc l'adresse de ```m()``` a l'adresse de ```puts()``` : a la fin du programme, quand ```puts()``` est appele, c'est en fait ```m()``` qui est appelee, et qui nous print le flag ! 