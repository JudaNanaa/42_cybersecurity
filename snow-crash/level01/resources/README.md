# Level 01

Une nouvelle fois, le repertoire courant etait vide. 
Donc on a recommence a essayer des commandes find pour trouver des choses interessantes, notamment :

```bash
find / -type f 2>/dev/null -exec cat {} \; | grep "flag"
```

Le but etant de trouver tous les fichiers auxquels on a acces, de rediriger les erreurs de la sortie 2 vers /dev/null pour ne pas encombrer le resultat, et d'executer la commande cat sur les resultats. Ensuite, on trie le contenu via un grep recherchant le mot "flag".

La encore, trop de resultats, alors on a restreint le champ de la commande, en ne l'utilisant plus sur la racine. 

```bash
find /etc -type f 2>/dev/null -exec cat {} \; | grep "flag"
```

Le resultat contenait ce qui ressemblait au fichier ***/etc/passwd***:
```bash
flag00:x:3000:3000::/home/flag/flag00:/bin/bash
flag01:42hDRfypTqqnw:3001:3001::/home/flag/flag01:/bin/bash
flag02:x:3002:3002::/home/flag/flag02:/bin/bash
flag03:x:3003:3003::/home/flag/flag03:/bin/bash
flag04:x:3004:3004::/home/flag/flag04:/bin/bash
flag05:x:3005:3005::/home/flag/flag05:/bin/bash
flag06:x:3006:3006::/home/flag/flag06:/bin/bash
flag07:x:3007:3007::/home/flag/flag07:/bin/bash
flag08:x:3008:3008::/home/flag/flag08:/bin/bash
flag09:x:3009:3009::/home/flag/flag09:/bin/bash
flag10:x:3010:3010::/home/flag/flag10:/bin/bash
flag11:x:3011:3011::/home/flag/flag11:/bin/bash
flag12:x:3012:3012::/home/flag/flag12:/bin/bash
flag13:x:3013:3013::/home/flag/flag13:/bin/bash
flag14:x:3014:3014::/home/flag/flag14:/bin/bash
```
Le user flag01 est anormal, parce qu'a la place du 'x' on y trouve ce qui ressemble a un mot de passe.
Apres tentative, pas possible de se log sur flag01 avec. 
Ca doit donc etre un hash. 
Normalement, les hash se trouvent dans ***/etc/shadow***, que seul root et le groupe shadow peuvent lire, pour eviter qu'on puisse les recuperer et tenter de les cracker. 

On a donc recuperer le hash en dehors de la vm, insere dans un fichier texte, et utilise john the ripper dessus.

```bash
echo '42hDRfypTqqnw' > tocrack.txt
john --show tocrack.txt
```

Ceci permettait d'obtenir un nouveau password basique : ***abcdefg*** permettant de se log sur flag01 et de recuperer le flag du niveau pour se log sur level02.