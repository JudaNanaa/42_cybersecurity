# level 6 #

Pour cet exercice on voit que les protections suivantes sont activées : `Partial RELRO`, `Stack canary` et `NX`. On sait déjà qu'on ne va pas pouvoir faire de buffer overflow trop sale, et qu'on ne pourra pas exécuter la stack. 

Quand on lance le programme, on nous demande un login,  puis un serial (un int). 

Quand on essaye de se servir de GDB pour suivre le process d'exécution, on se prend un `Tampering detected` ... Donc on doit plutot partir sur une traduction de l'ASM pour y voir plus clair. 

On découvre alors que les inputs sont récupérés, et récupérés par une fonction qui, si elle renvoie 0, nous ouvre un shell. 


```
lea     eax, [esp+0x2c]
call    8048550 <fgets@plt>             fgets(buff, 0x20, stdin)   (login)


lea     edx, [esp+0x28]
mov     DWORD PTR [esp+0x4],edx
mov     DWORD PTR [esp+0x4], edx
call    80485e0 <__isoc99_scanf@plt>    scanf("%u", &nb)          (serial)


mov     eax, DWORD PTR [esp+0x28]
mov     DWORD PTR [esp+0x4], eax        nb
lea     eax, [esp+0x2c]
mov     DWORD PTR [esp], eax            buff
call    8048748 <auth>                  auth(buff, nb)
test    eax, eax
jne     8048969                         auth != 0 on jump au dessus du /bin/sh
mov     DWORD PTR [esp], 0x8048b61      "/bin/sh"
call    80485a0 <system@plt>            system("/bin/sh")
```

Tout se joue donc dans `auth()`: nos inputs sont traités par un algo. Si on veut tomber dans la branche qui lance `/bin/sh`, il faut réussir à obtenir un résultat précis. 

```c
serial = ((int)buff[3] ^ 0x1337) + 0x5eeded;  
for (int i = 0; i < (int)len; i++) 
{
    if (buff[i] < ' ')          //que des chars printables
        return 1;
    serial += ((int)buff[i] ^ serial) % 0x539;
}
	if (nb == serial)  // le but est que les deux inputs ressortent égaux de cet algo
		ret_val = 0; 
	else
		ret_val = 1;
```

Pour logins, on a 3 contraintes : sa longueur doit être de 6 minimum et de 31 maximum, et il ne doit être constitué que de chars printables.

Mais à part ça, l'algorithme peut etre recréé en dehors de l'exécutable à exploiter, il suffit de bien le reproduire pour arriver au résultat souhaité. 


```
la formule appliquée à chaque char : serial = (buff[3] ^ 0x1337) + 0x5eeded
add     eax,0x3
movzx   eax, BYTE PTR [eax]
xor     eax, 0x1337
add     eax, 0x5eeded
mov     DWORD PTR [ebp-0x10], eax

la boucle : serial += (buff[i] ^ serial) % 0x539
xor     ecx, DWORD PTR [ebp-0x10]           ; ecx = buff[i] ^ serial
mov     edx, 0x88233b2b                     ; division par 0x539 (1337)
mov     eax,ecx
mul     edx
...
imul    eax, eax, 0x539
sub     edx, eax                            ; edx = ecx % 0x539
add     DWORD PTR [ebp-0x10], eax           ; serial += reste
```

On reproduit exactement ce calcul dans un programme, à côté, pour générer le serial correspondant à n'importe quel login :

```c
int compute_serial(char *login) {
    int len = strcspn(login, "\n");
    login[len] = '\0';
    len = strnlen(login, 32);
    if (len < 6) { /* login trop court */ exit(EXIT_FAILURE); }

    int serial = (login[3] ^ 0x1337) + 0x5eeded;
    for (int i = 0; i < len; i++) {
        if (login[i] < ' ') { /* octet interdit */ exit(EXIT_FAILURE); }
        serial += (login[i] ^ serial) % 0x539;
    }
    return serial;
}
```

```
-> Enter Login: madamou
-> Enter Serial: 6231029
Authenticated!
$ cat /home/users/level07/.pass
```
On récupère nos deux inputs, qu'on met dans un petit payload d'une ligne : 

```bash
(python -c 'print ("madamou") ;print ("6231029")'; echo cat /home/users/level07/.pass) | ./level06
```
