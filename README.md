# THE-BIG-CAPACITOR-DISCHARGER-DFTA253
Déchargeur de condensateur

Système de décharge contrôlée pour condensateurs haute tension, volontairement surdimensionné… et totalement assumé.

Présentation

Ce projet a été conçu pour décharger rapidement mais progressivement des condensateurs haute tension tout en offrant un retour visuel sur leur état de charge.

L’objectif était de créer un appareil de laboratoire dédié, robuste, pédagogique et relativement sécurisant.

Principe de fonctionnement

Au départ, aucune résistance n’est court-circuitée :

5 résistances de 250 ohms / 100W en série

Résistance totale initiale :

1250 ohms

Au fur et à mesure de la chute de tension, le Seeeduino XIAO active successivement quatre MOSFETs qui court-circuitent progressivement certaines résistances :

1250 ohms
1000 ohms
750 ohms
500 ohms
250 ohms

Cela permet une décharge rapide tout en limitant le courant initial.

Fonctionnalités
Décharge de condensateurs haute tension
Décharge progressive
Détection des sondes non connectées
Détection de surtension
Affichage binaire de la tension
Affichage par zones
Sélection petit / gros condensateur
PCB personnalisé
Simulation LTSpice
Affichage binaire

Une barrette de 8 LEDs APA106 affiche une estimation de la tension selon un codage binaire pondéré :

128
64
32
16
8
4
2
1

Exemple :

128 + 32 + 16 = 176V

Affichage par zones

Une seconde barrette de 4 LEDs APA106 fournit une lecture rapide :

Rouge → >300V
Orange → 200V à 300V
Jaune → 100V à 200V
Vert → <100V
Seuils de fonctionnement
Condensateurs ≤ 500µF
S1 = 250V
S2 = 180V
S3 = 100V
S4 = 30V
Condensateurs > 500µF
S1 = 300V
S2 = 220V
S3 = 140V
S4 = 50V
Seuil de surtension
380V
Matériel utilisé
Seeeduino XIAO
MOSFETs haute tension
Drivers MOSFET
LEDs APA106
Convertisseur DC/DC
PCB personnalisé
Boîtier métallique plié
Contenu du dépôt
Code source Arduino
Schéma EasyEDA
Schéma KiCad
Fichiers PCB
Simulation LTSpice
Sécurité

⚠️ Ce projet manipule des tensions potentiellement mortelles.

Ne reproduisez pas ce montage sans expérience des hautes tensions.

Toujours débrancher le système de charge avant la phase de décharge.

Vidéo associée

Lien YouTube :https://youtu.be/2aQS_2V3w8Q



