# level 3 #

A la connexion a ce level, on voit que plusieurs protections sont activees : partial RELRO, stack canary et la stack n'est pas executable.

Mais en realite il n'a pas besoin d'exploit ce lvl, surtout de comprendre comment il fonctionne.


Quand on lance l executable, on nous demande un password, et evidemment ca nous renvoie `invalid password` quand on tente un truc.


Lorqu'on objdump l'executable, on voit que notre input est recupere et passe a une fonction `test()`.
```
call __isoc99_scanf@plt               scanf("%d", &input)
mov eax,DWORD PTR [esp+0x1c]
mov DWORD PTR [esp+0x4], 0x1337d00d   second en dur
mov DWORD PTR [esp], eax              notre input
call test
```

On voit donc que notre input est passe a cette fonction, aux cotes d un autre argument passe en dur dedans : `0x1337d00d` = '322424845' en décimal. `main` appelle donc :

```c
test(input, 322424845);
```

Dans cette fonction test, on retrouve la logique suivante :

```
mov eax, [ebp+0x8]              eax, notre input
mov edx, [ebp+0xc]              edx, le nombre en dur, 322424845
mov ecx,edx
sub ecx,eax                     ecx = 322424845 - input  (la diff des deux)
mov eax,ecx
mov DWORD PTR [ebp-0xc],eax
cmp DWORD PTR [ebp-0xc], 0x15   compare diff à 21 (0x15)
ja  804884a                     si diff > 21 on passe dans la branche "défaut"
```

Donc, `diff = 322424845 - input`. Il faut donc trouver un input qui permettra d'avoir un diff inferieur ou egal a 21.
Si jamais on passe dans la branche par defaut, ce n'est plus le resulat de la soustraction, mais une valeur aleatoire (un `rand()`), qui est passee a la fonction `decrypt()`, et on ne peut donc pas la maitriser pour prevoir l'output de la fonction `decrypt()`.

Ensuite, la logique de la fonction `decrypt()` est la suivante :
```
mov eax,DWORD PTR [ebp+0x8]     eax = diff (paramètre reçu de la fonction test)
xor eax, edx                    XOR de cette dif avec un octet d'un buffer prerempli par decrypt()
→ Brute-force trivial sur 22 valeurs (0 à 21).
mov edx,eax                     bouge le resultat du xor dans edx
lea eax,[ebp-0x1d]              charge l'adresse du tableau dans eax
add eax,DWORD PTR [ebp-0x28]    ajoute i pour passer au prochain char dans le tableau
mov BYTE PTR [eax], dl          remplace le char par le nouveau char issu du xor

test eax, eax
jne 8048723                     si un byte est different, on jump au dessus de l appel a system()
mov DWORD PTR [esp],0x80489d4   charge "/bin/sh" sur la stack pour preparer l appel a system
call system@plt                 system(commande décodée)
```

Un buffer de 16 octets prerempli est XOR octet par octet avec l'octet inferieur de `diff`, puis compare à une string : une seule valeur de `diff` (parmi les 22 possibles) produit le bon résultat et déclenche `system()`.

Apres plusieurs essais, on realise que la valeur `18` fait matcher la comparaison :

```
322424845 - 18 = 322424827
```

Envoyer `322424827` en input déclenche donc `system()`, et fait pop le shell.

```bash
(python -c 'print(322424845 - 18)'; echo "cat /home/users/level04/.pass") | ./level03
```
