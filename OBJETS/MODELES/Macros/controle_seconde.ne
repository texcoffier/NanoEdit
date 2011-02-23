
classe vue_incluse
nom 1222152256
edite 151061456
fils (162429216 150868232 165843920)
xmin 0
ymin 0
ymax 50
titre Vue Z
pas_vue 0.1
angle_vue 0.1
point_de_vue 2.4 0.6 4
point_vise 2.4 0.6 0
haut 0 1 0
b_persp 0
b_grille 0
b_repere 0
geometrie 531 80 408 826

classe observateur
nom 162429216
observateur_point_de_vue 2.4 0.6 4
observateur_point_vise 2.4 0.6 0
observateur_haut 0 1 0
observateur_ratio 3.73256

classe applique
nom 150868232
fils (160114416 160050640 164733296 165840248 165840520 165843664 165840912 165841184 165841480 164255232 165841864 165842152 165843336 163692200)

classe lampe
nom 160114416
observateur 1 1 2
	0 0 -1
	0 0 0
	0 1

classe couleur
nom 160050640

classe commande
nom 164733296
fils (159871240 160622832 165840104 165840176)
ferme 1

classe applique
nom 159871240
fils (162805080 165839952)

classe facette
nom 162805080
t (-1 0 0
	-1 1 0
	-1.2 1 0
	-1.2 0 0)

classe facette
nom 165839952
t (0 0 0
	0 1 0
	-1 0.5 0)

classe variable
nom 160622832
fils (164313872)
ferme 1
nom_variable 0=seconde

classe seconde
nom 164313872
temps 1 0 1
increment 0.1

classe texte
nom 165840104
valeur temps

classe texte
nom 165840176
valeur 0
hauteur -1.44408

classe translation
nom 165840248
t_origine 0.2 0 0
cache_tag 1

classe commande
nom 165840520
fils (165840592 160622832 165840768 165840840)
ferme 1

classe facette
nom 165840592
t (0 0 0
	1 0 0
	1 1 0
	0 1 0)

classe texte
nom 165840768
valeur onoff

classe texte
nom 165840840
valeur 0

classe commande
nom 165843664
fils (165840592 165843816)
ferme 1
nom_champ valeur
valeur Arret

classe texte
nom 165843816
valeur Arret

classe translation
nom 165840912
t_origine 1.2 0 0
cache_tag 1

classe commande
nom 165841184
fils (165841256 160622832 165840768 165841408)
ferme 1

classe facette
nom 165841256
t (0 1 0
	0 0 0
	1 0.5 0)

classe texte
nom 165841408
valeur 1

classe commande
nom 165841480
fils (165841256 160622832 165841616 165788456)
ferme 1

classe texte
nom 165841616
valeur dilatation
hauteur -1.61691

classe variable
nom 165788456
fils (165841776)
ferme 1
nom_variable 1=vitesse_normale

classe flottant
nom 165841776
valeur 0.25

classe commande
nom 164255232
fils (165841256 165843816)
ferme 1
nom_champ valeur
valeur Marche

classe translation
nom 165841864
t_origine 1.2 0 0
cache_tag 1

classe commande
nom 165842152
fils (165842336 160622832 165840768 165841408)
ferme 1

classe applique
nom 165842336
fils (165842488 165842912 165843200)

classe applique
nom 165842488
fils (165842624 165841256)

classe dilatation
nom 165842624
vecteur 0.5 1 1
cache_tag 1

classe translation
nom 165842912
t_origine 0.5 0 0
cache_tag 1

classe applique
nom 165843200
fils (165842624 165841256)

classe commande
nom 165843336
fils (165842336 160622832 165841616 162630752)
ferme 1

classe variable
nom 162630752
fils (165843576)
nom_variable 2=vitesse_rapide

classe flottant
nom 165843576
valeur 1

classe commande
nom 163692200
fils (165842336 165843816)
ferme 1
nom_champ valeur
valeur Accelere

classe applique
nom 165843920
fils (165844056 165843816)

classe translation
nom 165844056
t_origine 4 0.5 0
cache_tag 1
