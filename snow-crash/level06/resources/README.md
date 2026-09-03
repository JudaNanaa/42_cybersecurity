# level 06

On ```ls -la``` dans le repertoire courant, et on constate la presence de deux files : un executable et un code en php.

Une fois encore, l'executable a son SUID set pour flag06 : on se doute donc qu'on va devoir lui faire executer ```getflag```.

Lorsqu'on ```cat``` le code php, qui, on l'imagine, est le code source de l'executable, on obtient :
```php
#!/usr/bin/php
<?php
function y($m) { $m = preg_replace("/\./", " x ", $m); $m = preg_replace("/@/", " y", $m); return $m; }
function x($y, $z) { $a = file_get_contents($y); $a = preg_replace("/(\[x (.*)\])/e", "y(\"\\2\")", $a); $a = preg_replace("/\[/", "(", $a); $a = preg_replace("/\]/", ")", $a); return $a; }
$r = x($argv[1], $argv[2]); print $r;
?>
```
En partant de la fin : 
- on va print la variable ```r```
- cette variable est le resultat de l'appel de la fonction ```x``` qui prend deux arguments
- la fonction ```x``` prend bien deux arguments, ```y``` et ```z```, mais ne se sert que de ```y```.
    - Elle [recupere le contenu du fichier](https://www.geeksforgeeks.org/php/php-file_get_contents-function/) ```y``` avec la fonction ```file_get_contents()```
    - Dans ce contenu, elle appelle la fonction ```preg_replace()``` qui a pour but de [remplacer le pattern](https://www.geeksforgeeks.org/php/php-preg_replace-function/) donne en premier argument ```[x (.*)]```, par le pattern donne en second argument.   
    - Ici ```/e``` est ajoute la fin du premier pattern, c'est un [modifier]() ce qui signifie qu'au lieu de le remplacer, le second pattern va etre considere comme une fonction a appliquer sur le premier pattern.
    - Le deuxieme pattern est donc l'appel de la fonction ```y```, appellee sur le deuxieme groupe du premier pattern soit ```(.*)```, en clair, tout ce qui se trouvera dans le pattern```[x ICI]```, sans le x ni les brackets.
    - La fonction ```y``` est egalement une fonction de remplacement, qui cherche a remplacer tous les ```.``` par des ```x```, puis tous les ```@``` par des ```y```. 
    - La fonction ```x``` remplace ensuite tous les brackets par des parentheses.  

Pour etre sur qu'on a bien compris le fonctionnement, on teste avec 
```bash
echo '[x .[]@.]' >/tmp/testphp
./level06 /tmp/testphp
```
On obtient l'output suivant : ``` x () y x ```.
La fonction ```y``` n'est pas vraiment utile, ce qui'il faut exploiter, c'est la vulnerabilite de ```preg_replace()``` avec modifier ```e```.
En effet, ce modifier est deprecie depuis [PHP >5.5.0](https://www.yeahhub.com/code-execution-preg_replace-php-function-exploitation/) a cause des risques d'exploits qu'il cause.   

L'idee est de forcer preg_replace() a executer une fonction en plus de celle prevue par son deuxieme argument. Il suffirait d'inserer la commande souhaitee dans le premier pattern, mais la fonction ```y``` est appelee sur le deuxieme groupe du premier pattern entre double quotes.   
Il faut donc reussir a forcer PHP a interpreter ce qui se trouve entre double quote. 
Pour y arriver on va utiliser la [Curly Syntax en PHP](https://devgex.com/en/article/00018720): on utilise ```${}``` pour dire a PHP qu'il doit resoudre le nom de la variable. Par exemple on va creer une variable dont le nom sera le resultat d'une addition :
```bash
echo '[x ${1+2}]' >/tmp/testphp
./level06 /tmp/testphp
```

Et on obtient l'erreur suivante : ```PHP Notice:  Undefined variable: 3 in /home/user/level06/level06.php(4) : regexp code on line 1```

PHP a bien realise l'operation, mais nous informe qu'il ne trouve aucune variable nommee ```3```.   
On comprend alors que la solution est de faire passer le resultat de la commande ```getflag``` comme nom de variable. Pour ce faire, il faut tromper le parsing de preg_replace() avec des backticks pour lui faire comprendre que ce qui se trouve dans la variable est une [commande a executer](https://www.madirish.net/402).
```bash
echo '[x ${`getflag`}]' >/tmp/testphp
./level06 /tmp/testphp
```
PHP nous informe alors d'une erreur :
```bash
PHP Notice:  Undefined variable: Check flag.Here is your token : wiok45aaoguiboiki2tuin6ub
```
Et voila le flag pour se log sur le level07 !
