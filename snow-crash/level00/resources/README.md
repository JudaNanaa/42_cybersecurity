# Level 00
En arrivant sur cette session on a tenté plusieurs choses:

```bash 
ls 
```
Mais le repo courant était vide.

```bash
find / -name "flag" 2>/dev/null
```
Comme on ne connaissait pas le format des flags, on esperait trouver un file qui contenait ce mot pour nous aiguiller. Mais rien. 

```bash
find / -user "level00" 2>/dev/null
```
Cette fois, trop de resultats pour etre utiles. 

```bash
find / -user "flag00" 2>/dev/null
```

Deux resultats s'affichaient : 
```bash
/usr/sbin/john
/rofs/usr/sbin/john
```

A l'interieur se trouvait la sequence suivante :

```bash
cdiiddwpgswtgt
```

Compte tenu du format etrange, mais aussi du nom du fichier, "john" qui fait penser a john the ripper, on a pense qu'il devait s'agir du mot de passe. 

Mais en tentant de l'utiliser pour se log sur flag00, ca ne fonctionnait pas. 

On a donc tente de le dechiffrer en utilisant le site [dcode.fr](https://www.dcode.fr/chiffre-cesar), en essayant de suite un Code Cesar, tres courrant dans ce genre d'exercice. 

L'une des solutions proposee etait ***nottoohardhere***, permettant de se log sur flag00 et d'utiliser la commande getflag pour recuperer le flag permettant de se connecter au level01.