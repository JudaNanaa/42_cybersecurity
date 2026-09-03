# level 09

```ls -la``` et une nouvelle fois, deux files, un executable ```level09``` avec SUID et GUID set, et un ```token``` avec les autorisations de lecture.

On peut donc ```cat token``` et on obtient ```f4kmm6p|=�p�n��DB�Du{��```. Ca ressemble un peu a un flag, mais ca n est pas exploitable en tant que tel. 

Alors on tente ```./level09``` qui nous dit ```You need to provied only one arg.``` 

```bash
./level09 token 
tpmhr
```

On essaye donc de regarder un peu les appels de l executable aux librairies :

```bash
ltrace ./level09 token
```
On voit alors apparaitre la ligne : ```You should not reverse this```

Donc ... on doit reverse ce programme. 

Pour ce faire, il faut comprendre comment il fonctionne. Si on lui donne le file ```token``` il print ```tpmhr```.

Si on lui donne ```/tmp/test``` il print ```/uos3ykz|```. Il ne semble donc pas s'executer sur le contenu du file, mais sur la string passee en parametre. 

```bash
./level09 1234
1357
./level09 1111111
1234567
```

Ca ressemble a un chiffrement cesar, comme dans le premier exercice, sauf que cette fois, le decalage n'est pas fixe, mais s'incremente en fonction de l'index du char dans la string.

La question c'est maintenant de savoir si ce qui se trouve dans ```token``` a ete transforme ou doit etre transforme. 

On tente donc :

```bash
./level09 'f4kmm6p|=�p�n��DB�Du{��'
f5mpq;v�E���|���~����[��`������
```

Le resultat est encore plus illisible qu'avant. Donc il faut surement inverser le fonctionnement du programme pour obtenir le flag. 

Le premier probleme c'est que le contenu de token contient des chars illisibles. Leur valeur doit surement etre superieure au ascii max. Du coup, plutot que d'essayer de recuprer ces chars, il faut recuperer leur valeur. 
Pour ca, il y a la commande hexdump, avec l'option -C pour rendre l'output lisible.

```bash
hexdump -C token
00000000  66 34 6b 6d 6d 36 70 7c  3d 82 7f 70 82 6e 83 82  |f4kmm6p|=..p.n..|
00000010  44 42 83 44 75 7b 7f 8c  89 0a                    |DB.Du{....|
0000001a
```

On peut donc recuperer la sequence de valeurs comme suit, en considerant que ```0a``` est juste une line feed ajoutee a la creation du fichier, et pas une composante du flag: ```66 34 6b 6d 6d 36 70 7c 3d 82 7f 70 82 6e 83 82 44 42 83 44 75 7b 7f 8c 89```

On passe cette string dans un programme en python qui reduit chacune de ces valeurs par leur position dans la suite, puis les convertit en chars et print le resultat:

```bash
 python3 flag09_asciireverse.py '66 34 6b 6d 6d 36 70 7c 3d 82 7f 70 82 6e 83 82 44 42 83 44 75 7b 7f 8c 89'
f3iji1ju5yuevaus41q1afiuq
```

On peut donc se log sur flag09 et recuperer le dernier flag! GGWP