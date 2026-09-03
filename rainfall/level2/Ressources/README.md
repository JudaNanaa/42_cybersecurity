# level 2 #

Dans ```p()```, on trouve un buffer ```char str[76]``` rempli par ```gets()```, donc encore une fois aucune verification de taille sur l'input.

Juste apres le ```gets()```, le code teste une variable ```check``` :

```asm
  lea    eax, [ebp-0x4c]           ; &str
  mov    DWORD PTR [esp], eax
  call   80483c0 <gets@plt>
  mov    eax, DWORD PTR [ebp+0x4]  ; eax = adresse de retour sauvegardee lors du call de p()
  mov    DWORD PTR [ebp-0xc], eax  ; check = [ebp+4]
  and    eax, 0xb0000000
  cmp    eax, 0xb0000000
  jne    8048527 <p+0x53>          ; si != -> on saute par-dessus _exit
  ...
  call   80483d0 <_exit@plt>
```

```check``` n'est donc pas une variable non initialisee laissee au hasard : elle est chargee depuis ```[ebp+0x4]```, c'est-a-dire **l'adresse de retour de ```p()```**. Le test ```check & 0xb0000000 == 0xb0000000``` ne bloque que si cette adresse pointe dans la stack (plage ```0xb0000000+```). Notre exploit va justement ecraser cette adresse de retour par une adresse en ```0x0804xxxx``` : le ```and``` donne ```0```, la condition est fausse, et ```_exit()``` est saute.

Le vrai probleme reste ```gets()```, qui permet d'ecraser cette adresse de retour. On construit un payload classique : un shellcode (```execve("/bin/sh")```) au debut, du padding pour completer jusqu'a 80 bytes, puis l'adresse ou se trouve notre shellcode en memoire.

Avant le ```ret```, ```p()``` appelle ```puts(str)``` puis ```strdup(str)``` : ```strdup()``` recopie notre buffer sur la heap a une adresse fixe (```0x0804a008```). C'est vers cette copie qu'on fait pointer l'adresse de retour.

```asm
  call   80483f0 <puts@plt>
  call   80483e0 <strdup@plt>   ; copie de str sur la heap -> 0x0804a008
  leave
  ret                           ; -> 0x0804a008 (notre shellcode)
```
