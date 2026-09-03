# Level 03

En faisant un ```ls``` dans le repertoire courrant on decouvre un file nomme ```level03```.

Avec ```ls -l level03``` on apprend que ce file appartient a flag03, le user sur lequel on doit reussir a se connecter pour executer la commande ```getflag```.   

```bash
-rwsr-sr-x 1 flag03 level03 8627 Mar  5  2016 level03
```

On constate aussi que le [SUID et le GUID](https://www.geeksforgeeks.org/linux-unix/advance-file-permissions-in-linux/) sont set. 
Donc si on execute ce fichier, le user ID sera celui du proprietaire, donc ***flag03***.   

Lorsqu'on l execute il print ```Exploit me```.

Il faut donc comprendre comment fonctionne ```level03```, mais c'est un binaire, on ne peut en voir le code source.

En utilisant la commande ```strings level03``` on peut retrouver le message ```Exploit me```.   
Il est affiche dans le terminal via la commande suivante :
```bash
/usr/bin/env echo Exploit me
```   
On en deduit donc que le programme fait appel a l ```env``` de l'utilisateur pour appeller ```echo```.   

Si on joue avec le ```PATH```, on peut changer le comportement du programme.   

Il faut donc modifier le ```PATH``` pour renvoyer vers un ```echo``` qui permettrait d'executer ```getflag```.

Sauf qu'on ne peut pas creer de fichier dans le repertoire courant...ni ailleurs. En remontant a la racine et en executant un ```ls -la```, on voit que le seul repertoir utilisable sur lequel on a des droits en ecriture est le ```/tmp```.

On cree donc un fichier appele ```echo``` dans ```/tmp``` avec la commande ```getflag``` dedans, on donne les autorisations sur ce fichier pour qu'il soit executable par flag03, on change le ```PATH``` pour qu'il renvoie vers ```/tmp``` et on execute !
```bash
echo '#!/bin/getflag' > /tmp/echo
chmod 777 /tmp/echo
PATH=/tmp
./level03 
```
On obtient ainsi le flag permettant de passer au level04, sans passer par le flag03, puisqu'on a directement execute ```getflag``` avec les privileges de flag03. 