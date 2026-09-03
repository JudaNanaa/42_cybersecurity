# level9 # 

Quand on lance le programme, rien ne se passe. 

On fait un ```ls -la``` et on voit que le SUID est set. 

Du coup on essaye un ```objdump -d -M intel level9```  

On voit que la technique habituelle de regarder les ```call``` du main ne fonctionne pas : on ne comprend pas de quoi il s'agit. 

On voit un call a ```strlen()``` et a ```memcpy()``` dans la fonction ```<_ZN1N13setAnnotationEPc>```. 

Parcontre aucun call a ```system()``` qui permettrait d'ouvrir un shell.

En lancant ```gdb level9``` pui en entrant la commande ```info functions``` on constate qu'il s agit probablement d un executable code en C++, du fait de la presence d'operateurs et de constructeurs. 

On voit une comparaison entre 0x1 et ebp+0x8: c'est la verification de la presence d'un argument. 
Donc sans argument, ca exit. 

Du coup on relance avec "AAAA" comme argument, pour pouvoir l'identifier facilement.

Cette fois, on exit pas de suite. 

La valeur 108 est ensuite passee dans la fonction ```_Znwj@plt```.
Lorsqu'on recupere l'adresse 0x08048530, on constate que c'est la meme que l'operateur new.
On comprend donc qu'une zone de 108 bytes va etre allouee sur la heap.

L'adresse pointant au debut de cette zone est ensuite recuperee, et passee en parametre avec le chiffre 5 a la fonction ```_ZN1NC2Ei```.
Une fois encore, en examinant l adresse de la fonction 0x80486f6 et en la comparrant a celles des fonctions  de l'output de ```info functions```, on constate qu'il s'agit de l'adresse de ```N::N(int)``` : un constructeur de classe qui prend un int en parametre. 
On a donc un objet de la classe N qui est construit, place sur la heap. 
Au premier emplacement + 0x8, on y trouve l'adresse 0x8048848 : il s'agit d'un pointeur sur la fonction 0x0804873a, l'operateur +, sur la table de fonctions propre a la classe N.

Avec ```info proc map``` on recupere l adresse de la heap(0x804a000), et on constate via x/60wx 0x804a000 que le chiffre 5 a ete place au dernier emplacement (0x00000005) de l'espace reserve pour cette instance de classe.

Cette procedure est repetee une nouvelle fois, on a donc deux instance de la classe N sur la heap, une contenant 5, l'autre 6. 


Des valeurs sont ensuite placees sur la stack, notamment l'input donne au programme(0xbffff903), et les adresses des deux instances de la classe N (0x0804a008 pour la premiere et 0x0804a078 pour la deuxieme). 

La fonction ```_ZN1N13setAnnotationEPc``` va realiser un ```strlen()``` sur notre input, puis 4 est ajoute a l'adresse de la premiere instance de la classe N (on vise donc le deuxieme emplacmenet memoire a l interieur de cette classe, apres l adresse des fonctions stockees au premier emplacement). 
```memcpy()``` va ensuite copier notre input a ce second emplacement de la premiere classe N.

Quand on retourne dans le ```main()```, l'adresse des deux objets sont push sur la stack, et la veleur de edx est desormais l'adresse de la fonction stockee au premier element du deuxieme objet (0x804873a). Il s'agit de l operateur +. 

Lorsque edx est call a la fin du main(), il va donc operer une addition entre les deux objets de la classe N, contenant 5 et 6, donnant un resultat de 11 qui ne nous sert a rien. 

L'objectif est en fait de faire en sorte de modifier edx pour qu'il pointe vers une adresse memoire qu'on maitrise, ou un shellcode sera execute (puisuq'on a aucun autre moyen de faire pop un shell en l absence d'appel a system() dans le code, et que pour appeler system nous meme, il faudrait egalement pouvoir lui fournir un argument).

On sait qu'on peut ecrire a l interieur du premier objet de la classe N. On sait aussi que l'adresse chargee dans edx au moment du dernier call est en fait l'adresse stockee dans le premier emplacemebt du deuxieme objet de la classe N. 

Il faut donc reecrire par dessus une adresse qui sera chargee dans eax, contenant une autre adresse qui elle meme sera chargee dans edx, qui elle meme pointera sur le shellcode. 

En effet, si on ecrit directement le shellcode, ```mov    edx,DWORD PTR [eax]``` fail parce que le shellcode n'est pas une adresse accessible. 

Et si on ne met pas une adresse utilisable a l'interieur de cette meme adresse, c'est ```call   edx``` qui va fail, puisqu'il s'agit d un call a l adresse d'une fonction. 

On doit donc mettre notre d'abord mettre le padding (108 characteres), puis au premier emplacement de l'objet (0x0804a078)l'adresse de l'adresse du shellcode(0x0804a07c), puis l'adresse du shellcode(0x0804a080), puis le shellcode lui meme(le meme que dans le level2, pour ```system("/bin/bash")```). 

Et voila, le tour est joue !


