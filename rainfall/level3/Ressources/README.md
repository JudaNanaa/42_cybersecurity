# level 3 #

Cette fois, ```v()``` recupere notre input avec ```fgets(buffer, 512, stdin)``` : la taille est bien limitee, donc pas de buffer overflow classique possible.

Par contre, l'input est passe directement a ```printf``` comme **premier** argument, sans ```"%s"``` :

```asm
  lea    eax, [ebp-0x208]
  mov    DWORD PTR [esp], eax
  call   8048390 <printf@plt>   ; printf(buffer) -> notre input est la chaine de format
  mov    eax, ds:0x804988c      ; eax = m
  cmp    eax, 0x40              ; m == 64 ?
  jne    8048518 <v+0x74>
  ...
  mov    DWORD PTR [esp], 0x804860d   ; "/bin/sh"
  call   80483c0 <system@plt>
```

C'est une faille de **format string** : notre input est directement interprete comme une chaine de format. On voit aussi que la variable globale ```m``` (adresse ```0x0804988c```) doit valoir exactement ```64``` pour que le programme appelle ```system("/bin/sh")```.

Grace au format string, on peut utiliser ```%n``` pour ecrire, a l'adresse pointee par l'argument courant, le nombre de caracteres deja affiches par ```printf()```. On place donc l'adresse de ```m``` au tout debut de notre input, puis on ajoute du padding et des ```%x``` pour consommer les arguments intermediaires sur la stack et ajuster precisement le compteur de caracteres, jusqu'a tomber exactement sur ```64```, et on termine par ```%n``` pour ecrire cette valeur a l'adresse de ```m```.

```m``` vaut alors ```64```, la condition est verifiee, et ```system("/bin/sh")``` est appele.


