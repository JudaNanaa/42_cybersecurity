# level 6 #

Ici, on constate deux appels a ```malloc()```, un call a ```strcpy()```, deux fonctions ```m()``` et ```n()```. 

```m()``` appelle ```puts()```
```n()``` appelle ```system()```. 

On aimerait bien que ```n()``` soit appele... Mais lorsqu'on lance level6 avec des arguments (sans ca segfault), c'est bien ```m()``` qui est appele et qui nous nargue en disant : "Nope"

On veut donc faire en sorte que ```n()``` soit appelee a la place. 

Quand on suit l'execution, on s'appercoit que l'adresse de m() est placee dans l'adresse sur la heap retournee par le 2ème malloc(). 
Ensuite, notre input est copie via ```strcpy()``` dans le premier ```malloc()```. 

Enfin, c'est l'adresse de la fonction stockee dans le deuxieme ```malloc()``` qui est appele a la fin de la fonction.

Mais on sait que les adresses retournees par ```malloc()``` se suivent. Alors ce qu'on doit faire,  c'est donner un gros input au programme, pour que le ```strcpy()``` qui est une fonction vulnerable ecrive plus loin que prevu... Et reecrive l'adresse qui se trouve au debut du 2eme ```malloc()``` et qui finira par etre call. 

Au lieu de l'adresse de ```m()```, on va ecrire l'adresse de ```n()``` apres un padding de 72bytes... Et voila, ```n()``` est appele et un shell pop ! 