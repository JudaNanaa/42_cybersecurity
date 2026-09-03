# Level 04

Une nouvelle fois, on trouve un file dans le repertoire courrant. 
En executant ```ls -l``` dessus, on obtient :

```bash
-rwsr-sr-x 1 flag04 level04 152 Mar  5  2016 level04.pl
```

Une nouvelle fois le SUID et le GUID sont set. On pense tout de suite au fait qu'une nouvelle fois, il va falloir ruser pour faire en sorte que la commandew ```getflag``` soit executee va cet executable pour beneficier du SUID de flag04, et ainsi recuperer le flag sans passer par l etape ```su flag04```. 

```cat level04.pl``` nous donne le resultat suivant : 

```perl
#!/usr/bin/perl
# localhost:4747
use CGI qw{param};
print "Content-type: text/html\n\n";
sub x {
  $y = $_[0];
  print `echo $y 2>&1`;
}
x(param("x"));

```
Il semble donc que ce soit un script perl, et le commentaire se;ble indiquer que quelque chose se passe sur localhost, sur le port 4747.   

En utilisant la commande ```telnet localhost 4747``` on parvient a se connecter sur ce port.   
Le terme ```CGI``` renvoie a Common Gateway Interface, un standard permettant d'executer differents types de codes, independamment du langage utilise pour coder le serveur: on imagine donc que le code perl qui nous est presente tourne sur le port 4747 de localhost.   

En faisant plus de recherches, on apprend que le mot cle ```use``` est un peu [l equivalent de import](https://perldoc.perl.org/functions/use) en python. De meme, ```qw``` sert a [extraire des strings d'un ensemble](https://www.geeksforgeeks.org/perl/perl-qw-operator/).   
En combinant ```use``` et ```qw``` le script importe la partie '''param''' de [l ensemble de modules CGI](https://perldoc.perl.org/5.10.0/CGI).   

La ligne suivante semble seulement etre un ```print```, puis vient le mot cle ```sub```. Il s agit la d une ["sous routine"](https://perldoc.perl.org/perlsub), en clair une fonction qui sera utilisable lors de l'execution.   
On constate qu'une variable ```y``` est remplie avec le contenu du premier emplacement d'un tableau, puis que ```echo``` est appele sur ce parametre, et que ```stderr``` est redirige vers ```stdout```.   

Enfin la sous routine est appellee de facon peu claire sur ```param``` de ```"x"``` qui semble donc litteralement etre le parametre nomme x dans la querry string. 

On tente donc de la commande suivante : ```curl 'localhost:4747/level04.pl?x=test&x=lol'```   

Cette commande print ```test``` sur ```stdout```. Elle print donc la premiere valeur du parametre x.

On comprend donc qu'on doit injecter la commande ```getflag``` dans le premier emplacement du parametre x de la querry string.

```bash
curl 'localhost:4747?x=$(getflag)'
```

Cette commande nous renvoie directement le flag comme si on avait execute ```getflag``` en tant que flag04, et on peut donc se connecter a level05.