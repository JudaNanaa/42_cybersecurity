# level 4 #

Meme schema que le level3 : ```n()``` recupere notre input avec ```fgets()``` (taille limitee, donc pas d'overflow direct), puis le passe a ```p()```, qui fait ```printf(str)``` sans specifier de format. On retrouve donc une faille de format string.

```asm
08048444 <p>:
  mov    eax, DWORD PTR [ebp+0x8]
  mov    DWORD PTR [esp], eax
  call   8048340 <printf@plt>       ; printf(str) -> format string

08048457 <n>:
  ...
  call   8048444 <p>
  mov    eax, ds:0x8049810          ; eax = m
  cmp    eax, 0x1025544             ; m == 16930116 ?
  jne    80484a5 <n+0x4e>
  mov    DWORD PTR [esp], 0x8048590 ; "/bin/cat /home/user/level5/.pass"
  call   8048360 <system@plt>
```

Ici, la variable globale ```m``` (adresse ```0x08049810```) doit valoir ```16930116``` (```0x01025544```) pour que ```system("/bin/cat /home/user/level5/.pass")``` soit appele directement : le flag est print sans meme avoir besoin de pop un shell.

On utilise a nouveau ```%n``` pour ecrire a l'adresse de ```m```, placee au debut de notre input. Comme la valeur cible est enorme, on se sert d'un specificateur de largeur (```%016930042x```) pour forcer ```printf()``` a "afficher" un tres grand nombre de caracteres de padding sans avoir a tous les envoyer, puis quelques ```%x``` supplementaires pour ajuster le nombre de paramettres donne a printf, et enfin ```%n``` pour ecrire le total dans ```m```.

```m``` vaut alors ```16930116```, la condition est verifiee, et le ```.pass``` du level5 est affiche directement.