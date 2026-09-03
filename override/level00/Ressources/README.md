# level 0 #

En regardant le desassemblage de ```level00```, on voit que ```main()``` affiche une banniere, demande une valeur a l'utilisateur avec ```scanf("%d", &local)```, puis compare la valeur lue a ```0x149c``` (5276 en decimal).

Cote assembleur, toute la logique tient dans le bloc ```scanf``` / ```cmp``` / ```jne``` :

```asm
mov    eax,0x8048636                  ; eax = "%d" (format scanf)
lea    edx,[esp+0x1c]                 ; edx = &local
mov    DWORD PTR [esp+0x4],edx
mov    DWORD PTR [esp],eax
call   0x80483d0 <__isoc99_scanf@plt> ; scanf("%d", &local)
mov    eax,DWORD PTR [esp+0x1c]       ; eax = local
cmp    eax,0x149c                     ; 0x149c == 5276
jne    0x804850d <main+0x79>          ; sinon -> branche "Invalid Password!"
```

Si l'egalite est verifiee, le programme appelle directement ```system()``` sur une commande codee en dur, ce qui donne un shell heritant des droits du binaire (setuid ```level01```) :

```asm
mov    DWORD PTR [esp],0x8048649
call   0x80483a0 <system@plt>         ; system(cmd)
```

Il n'y a aucune faille a exploiter ici, il suffit de lancer le binaire et de lui fournir la bonne valeur (```5276```, soit ```0x149c```) sur son entree standard. Comme la branche gagnante appelle ```system()``` avant de rendre la main, on obtient directement un shell avec les droits ```level01```, dans lequel on peut lire le mot de passe.

```
./level00
5276
cat /home/users/level01/.pass
```