# payloadbyKG - TCP BIN loader

Ce projet compile un payload PS4 qui :

1. initialise le réseau ;
2. écoute sur TCP **9021** ;
3. reçoit un fichier `.bin` envoyé depuis un PC ;
4. tente de le placer dans une zone RWX ;
5. saute au début du payload reçu.

## Compilation

Le workflow GitHub Actions est déjà inclus.

- Ouvrir **Actions**
- Sélectionner **Build PS4 Payload**
- Cliquer **Run workflow**
- Télécharger l'artifact **payloadbyKG-loader**

Le fichier principal généré est :

`payloadbyKG.bin`

## Utilisation

1. Exécuter `payloadbyKG.bin` avec ton exploit/loader actuel.
2. La PS4 affiche une notification indiquant qu'elle attend sur le port 9021.
3. Depuis le PC, envoyer le second `.bin` vers `IP_PS4:9021`.
4. Fermer la connexion après l'envoi ; le loader tente alors d'exécuter le BIN.

## Important

Ce loader ne remplace pas l'exploit initial.

L'exécution d'un BIN reçu nécessite que l'environnement dans lequel
`payloadbyKG.bin` tourne autorise une mémoire exécutable. Si `mmap` RWX
est refusé, la PS4 affichera `mmap RWX refuse` et le loader n'exécutera rien.

Un payload utilisant de mauvais offsets firmware peut faire planter ou
redémarrer la console.
