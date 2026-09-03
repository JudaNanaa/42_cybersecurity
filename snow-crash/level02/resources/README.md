# Level 02

Cette fois, on trouve un fichier ***level02.pcap*** dans le repertoire courant en utilisant ```ls```.

Dans la video de presentation du projet sur l'intra, on nous avait informe du fait qu'on aurait besoin d'un ***lecteur de pcap***.

En recherchant un pcap reader on tombe tres vite sur [wireshark](https://www.wireshark.org/), qu'on telecharge sur nos machines, en dehors de la vm. 

Du coup, on a besoin de recuperer le fichier en dehors de la vm aussi. On utilise donc la commande ```scp``` qui permet de copier un file via ssh. 

```bash
scp -P 4242 level02@192.168.1.3:/home/user/level02/level02.pcap .
```
On doit alors taper le mot de passe pour se log sur level02 (recuperer en utilisant la commande ```getflag``` en tant que flag01), et le fichier est envoye sur la machine hote. 

Un petit ```chmod 777``` sur le .pcap pour pouvoir l'ouvrir avec wireshark, et on en voit apparaitre le contenu. 

Il y a beaucoup d'informations, et ce n'est pas tres lisible. On essaye alors d'identifier ce qui se repete :

Il y a une adresse source et une adresse de destination pour chaque paquet. Toujours les deux memes qui se terminent par 218 ou 223. 

On voit ensuite que presque tous les paquets sont envoyes via le protocole TCP, via les ports 12121 vers 39247, et qu'une sequence est identifiable grace aux [flags TCP](https://www.geeksforgeeks.org/computer-networks/tcp-flags/) : 

```
SYN -> SYN ACK ->ACK 
```
Il s'agit la de l etablissement de la connection entre un client et un serveur
```
PSH -> PSH ACK
```
Il s'agit d'envoi de donnees.
```
FIN -> FIN ACK -> ACK.
```
Il s'agit de la fin de la connection. 

Ce qui nous interesse se trouve probablement dans les donnees envoyees.   
On comprend que c'est ***218(client)*** qui demande a etablir la connexion vers ***223(serveur)***.   
Dans le paquet 43 on voit que le serveur envoie "Password".   
Dans le paquet 91 on voit que le serveur envoie "Login incorrect".   
On peut donc en deduire qu'entre les deux, le client a tente de s'identifier : on filtre alors les paquets en fonction de leur origine, pour ne retenir que ceux envoyes par le client entre les paquets 44 et 90.   
On remarque que wireshark nous indique la longueur des datas envoyes via l'information ```Len=```.   
Les paquets qui nous interessent on donc au moins une longueur egale a 1. En lisant les bytes 1 par 1 on obtient

```bash
ft_wandr{7F}{7F}{7F}NDRel{7F}L0L{0D}
```
Le password contient donc des caracteres ascii non imprimables:

- 7F correspond a DEL (effacer)
- 0D correspond a CR (carriage return).

En ignorant le dernier character, et en effacant chaque charactere precedant un DEL, on obtient ***ft_waNDReL0L*** qui nous permet de nous log sur flag02 et d'obtenir le flag pour passer au level03.
