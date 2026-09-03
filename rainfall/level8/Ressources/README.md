# level8 #

Quand on lance ```level8``` on constate que le programme print "(nil)(nil)".
Il recupere notre input, puis print une nouvelle fois la meme chose.

En executant ```objdump -d -M intel``` on constate la presence de nombreuses fonctions connues: ```printf(), free(), strdup(), fgets(), fwrite(), strcpy(), malloc(), system()```

On peut deja se dire qu'on va devoir interagir avec la heap compte tenu de la presence de ```malloc()``` et de ```free()```, mais aussi que ```system()``` permettra probablement de faire pop un shell.


En examinant le ```main()```, on trouve un ```printf()``` non exploitable. Les 3 arguments lui sont passes en dur, notamment le format stocke a 0x8048810, ``` "%p, %p \n"```. Les valeurs qui sont print lorsqu'on lance le programme devraient donc etre des adresses.  


On remarque ensuite un ```fgets()``` avec un buffer de 128 bytes qui va recuperer notre input. C'est securise, mais c'est notre seule maniere d interagir avec le programme.

Ensuite, notre imput est compare a plusieurs valeurs predefinies via l'instruction ```repz cmps```.  

***Si l'input commence par "auth ":***

- ```malloc()``` de 4 bytes ligne 135
- ```strcypy()``` - ligne 217
    - eax comme dest (le malloc de 4 0x8049aac)
    - edx comme source ( le deuxieme argument, 5 bytes apres 'auth')


***Si l'input est "reset":***
- ```free()``` ligne 271
    - prend l'adresse stockee a 0x8049aac - l'adresse du premier ```malloc()``` dans le cas "auth"


***Si l'input est "service":***
- ```strdup()```  - ligne 327
    - la string source est esp+0x20, donc notre input a partir de la 7 eme position (apres "service" du coup)
    - l'adresse de la string ```malloc()``` et dupliquee est stockee a 0x8049ab0

***Si l'input est "login" et si eax different de 0:***

*C'est ici qu'on veut reussir a arriver* <==

- ```system()``` - ligne 401
    - prend en argument la sting stockee a 0x8048833 - ```"/bin/sh"```


Si l'input est "login" et si eax egal a 0:
- fwrite() - ligne 446
    - prend en argument la string stockee a 0x804883b: "Password:\n";
    - la size de 0xa(10) et le nombre d items(1)
    - ```stdout()``` pointe par 0x8049aa0
- Ce print semble demander le password, mais aucun input n'est recupere apres (et de toutes facons ce qui nous interesse se trouve avant).

***Comparaison cle lignes 382 a 390:***
- L'adresse de eax est changee pour etre egale au contenu de 0x8049aac
    - C'est l'adresse ou sont stockes les retours de mallocs utilises par ```strcpy()``` 
    - L'adresse de eax est donc egale a l'adresse de la string copiee par ```strcpy()```, sont contenu est donc egal a l'input qu'on a donne au programme avec la commande ```auth ____```
- L'adresse de eax est ensuite egale au contenu de eax+0x20, soit l'adresse retournee par le malloc() de ```auth ___``` + 0x20. 

Normalement, on ne peut pas ecrire a cet endroit en ajoutant juste 32bytes, puisque les inputs superieurs a 30bytes (29bytes + \n) ne sont pas transferes a ```strcpy()``` donc ensuite lorsque le code verifie si eax est egal a 0, il jump au dessus de ```system()```. 

La solution d'origine devait probablement consister dans le fait d'utiliser une faille use-after-free() : on ```malloc()``` via la commande auth, puis on free, et lorsqu'on malloc via service, la string est stockee a la meme adresse dans la heap que le malloc precedent, on peut donc ecrire a +0x20 via ```strdup()``` puisque la size n'y est pas verifiee.  

Mais la la solution est encore plus simple, il suffit de malloc() en appelant auth, puis d'appeler service 2 fois. L'adresse du deuxieme service se trouvera pile 0x20 apres l'adresse de auth, donc sera non nulle, donc la comparaison recherchee fonctionne, et ```system()``` est appele.
