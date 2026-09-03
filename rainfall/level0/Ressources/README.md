# level 0 #

En regardant ```source.c```, on voit que ```main()``` recupere ```av[1]```, le convertit en entier avec ```atoi()```, et compare le resultat a ```423```.

Cote assembleur, toute la logique tient dans le bloc ```atoi``` / ```cmp``` / ```jne``` :

```asm
mov    eax, DWORD PTR [ebp+0xc]   ; eax = av
add    eax, 0x4                   ; eax = &av[1]
mov    eax, DWORD PTR [eax]       ; eax = av[1]
mov    DWORD PTR [esp], eax
call   0x8049710 <atoi>           ; eax = atoi(av[1])
cmp    eax, 0x1a7                 ; 0x1a7 == 423
jne    0x8048f58 <main+152>       ; sinon -> branche "No !"
```

Si l'egalite est verifiee, le programme droppe les privileges avec ```setresgid()```/```setresuid()``` (on herite donc bien des droits du binaire, qui est setuid ```level1```), reconstruit ```argv = {"/bin/sh", NULL}``` puis appelle ```execv("/bin/sh", argv)``` :

```asm
mov    DWORD PTR [esp], 0x80c5348     ; "/bin/sh"
call   0x8050bf0 <strdup>
mov    DWORD PTR [esp+0x10], eax      ; argv[0] = strdup("/bin/sh")
mov    DWORD PTR [esp+0x14], 0x0      ; argv[1] = NULL
...
lea    eax, [esp+0x10]               ; eax = argv
mov    DWORD PTR [esp+0x4], eax
mov    DWORD PTR [esp], 0x80c5348     ; "/bin/sh"
call   0x8054640 <execv>
```

Il n'y a aucune faille a exploiter ici, il suffit de lancer le binaire avec le bon argument. Comme ```execv``` remplace le processus par un ```/bin/sh``` qui lit son stdin, on lui envoie directement la commande a executer.

