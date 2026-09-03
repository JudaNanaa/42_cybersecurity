# level 3 #

## 1. `main` — lecture de l'input

```asm
call __isoc99_scanf@plt              ; scanf("%d", &input)
mov DWORD PTR [esp+0x4], 0x1337d00d  ; second argument fixe
mov DWORD PTR [esp], eax             ; premier argument = input
call test
```

`0x1337d00d` = **322424845** en décimal. `main` appelle donc :

```c
test(input, 322424845);
```

## 2. `test` — calcul de la différence et branchement

```asm
mov eax, [ebp+0x8]      ; eax = input
mov edx, [ebp+0xc]      ; edx = 322424845
sub ecx, eax             ; ecx = 322424845 - input  (= diff)
cmp DWORD PTR [ebp-0xc], 0x15   ; compare diff à 21 (0x15)
ja  804884a              ; si diff > 21 (non signé) -> branche "défaut"
```

`diff = 322424845 - input`, calculé en **non signé**.

- **`diff > 21`** → branche par défaut : `rand()` est appelé, sa valeur passée à `decrypt()` → non maîtrisable.
- **`diff` dans `[0, 21]`** → table de sauts (22 cases), chaque case appelle `decrypt(diff)` avec `diff` lui-même → valeur **entièrement contrôlée** en choisissant `input`.

## 3. `decrypt` — XOR et comparaison

```asm
mov eax, [ebp+0x8]   ; eax = diff (paramètre reçu)
xor eax, edx          ; XOR avec un octet du buffer chiffré
mov [eax], dl         ; buffer[i] = buffer[i] XOR (diff & 0xFF)
...
test eax, eax
jne 8048723            ; différent -> puts() (échec)
call system@plt        ; égal -> system(commande décodée)
```

Un buffer de 16 octets chiffré en dur est XORé octet par octet avec l'octet bas de `diff`, puis comparé à une chaîne de référence. Une **seule** valeur de `diff` (parmi les 22 possibles) produit le bon résultat et déclenche `system()`.

→ Brute-force trivial sur 22 valeurs (0 à 21).

## 4. Résultat

`diff = 18` fait matcher la comparaison :

```
322424845 - 18 = 322424827
```

Envoyer `322424827` en input déclenche `system()`.

## Exploit final

```bash
(python -c 'print(322424845 - 18)'; echo "cat /home/users/level04/.pass") | ./level03
```
