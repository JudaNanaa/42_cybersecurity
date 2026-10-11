# level 8 #

Bon la quand on lance l'executable c'est la totale : `Full RELRO`, `Canary` et `NX`. 
La memoire est intouchable.

En fait ici une nouvelle fois, c'est pas vraiment un exploit, on doit juste reussir a comprendre le fonctionnement du programme suffisamment bien pour lui faire faire ce qu'on veut.

Le programme prend un argument : il veut un filename.
On remarque aussi que dans le home du level08 se trouve un dossier `backups` dans lequel se trouve un fichier `.log`.

```c
log_wrapper(logfile, "Starting back up: ", argv[1]);
flag = fopen(argv[1], "r");             // open du path passe en argv[1] tel quel
...
strcpy(dest, "./backups/");               // dest = "./backups/"  (10 octets)
strncat(dest, argv[1], 99 - strlen(dest));// le path de dest est egal a backup + 89 octets de argv[1]
fd = open(dest, 193, 432);                //cree et ouvre le path cree ci dessus s'il n'existe pas encore
while (1) {
  buf = fgetc(flag);
   if (buff == -1)
      break;
   write(fd, &buf, 1);}                   //copie le flag dans la dest qu'on vient de creer
```

Ce fichier .log ne va finalement servir qu'a print a l interieur `Starting back up:` et `Finished back up `.
Le fichier passe en argument, `argv[1]` sera en revanche copie dans le fichier `dest`.

Toute la subtilite de ce niveau, c'est de reussir a gerer la facon dont notre input sera traite.


`argv[1]` sert à deux endroits differents : 

- en lecture, via `fopen(argv[1], "r")` qui l'utilise sans transformation
- en ecriture, via le path de dest cree par `strncat(dest, argv[1], 99 - strlen(dest))`

En lecture, on a donc aucun probleme a passer directement le chemin du `.pass` du level09. Parcontre, si on file `/home/users/level09/.pass` pour la construction de `dest`, le open va fail, puisqu'on lui demande de creer non pas un file, mais aussi des sous dossiers qui n'existent pas encore. Et on ne peux pas les `mkdir` a l'avance, puisqu'on a pas les droits dans le repertoire courant pour ce faire. 

Il faut donc utiliser le fait que les `/` consecutifs sont reduits lors de la resolution d'un path : on va passer un chemin avec 84 `/` (99 moins la len de `./backups/` moins la len de `home`) pour que lors de la creation de `dest` le chemin soit `./backups/`, qui existe deja, 84 `/` qui disparaitront mais rempliront le buffer de `strncat()` pour que le path du flag ne soit inclus dans la construction du path de dest que jusqu'a `home`.

En gros : `./backups/home` une fois le flag resolu.

Et le payload donne :

```bash
./level08 $(python -c 'print ("/"*84 + "/home/users/level09/.pass")'); cat backups/home  
```