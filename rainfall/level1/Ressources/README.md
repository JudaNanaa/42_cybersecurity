# level 1 #

On constate deux fonctions : ```run()```, qui appelle ```system("/bin/sh")```, et ```main()```, qui declare un buffer ```char str[76]``` et recupere notre input via ```gets()```.

```main()``` se resume a un ```gets()``` sur un buffer de la stack, sans aucune borne :

```asm
08048480 <main>:
  push   ebp
  mov    ebp, esp
  and    esp, 0xfffffff0
  sub    esp, 0x50           ; reserve le frame
  lea    eax, [esp+0x10]     ; eax = &str
  mov    DWORD PTR [esp], eax
  call   8048340 <gets@plt>  ; gets(str) : aucune verification de taille
  leave
  ret                        ; <- saute a l'adresse de retour lue sur la stack
```

```gets()``` ne verifie jamais la taille de ce qu'on lui donne : c'est une fonction non securisee. On peut donc ecrire au-dela des 72 bytes du buffer et venir ecraser l'adresse de retour de ```main()``` sur la stack. Au moment du ```ret```, le CPU saute a l'adresse qu'on a placee la.

```run()``` n'est jamais appelee normalement, mais son adresse est connue et fixe (```0x08048444```) :

```asm
08048444 <run>:
  ...
  call   8048350 <fwrite@plt>         ; "Good... Wait what?"
  mov    DWORD PTR [esp], 0x8048584   ; "/bin/sh"
  call   8048360 <system@plt>         ; system("/bin/sh")
```

Il suffit donc de remplir 76 bytes de padding pour atteindre l'adresse de retour, puis d'ecrire l'adresse de ```run()``` a la place, en little-endian (```\x44\x84\x04\x08```).

Quand ```main()``` termine et essaie de retourner, c'est donc ```run()``` qui est executee a la place, et qui pop un shell via ```system()```.


