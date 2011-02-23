
classe vue
nom 136938616
titre Vue Axe X
pas_vue 0.1
angle_vue 0.1
point_de_vue 5.4 0.1 1
point_vise 1.4 0.1 1
b_persp 0
b_entier 1
b_grille 0
geometrie 919 612 227 320

classe pov
nom 136939440
fils (136939752)
lar_max 256
hau_max 256
qualite 8
antialiasing 1

classe vue
nom 136939752
titre Vue Perspective
pas_vue 0.1
angle_vue 0.1
point_de_vue 10.0659 -2.79434 3.47494
haut -0.264009 0.217521 0.939672
geometrie 442 0 494 565

classe applique
nom 136940600
fils (136940832 136940960 136941056 136941296 136943800 136945800)

classe couleur
nom 136940832

classe epaisseur
nom 136940960
epaisseur_trait 0.05

classe lampe
nom 136941056
observateur 2 1 1
	0 0 -1
	0 0 0
	0 1

classe applique
nom 136941296
fils (136941480 136941768 136942560 136943304)

classe translation
nom 136941480
t_origine 0 0 -2

classe applique
nom 136941768
fils (136942104 136942368)

classe translation
nom 136942104
t_origine 0 0 3

classe facette_grille
nom 136942368
fils (136942440)

classe grille_nc_point3
nom 136942440
t [(0 3 0
	0 4 0
	0 5 0)
	(0 3 -1
	1 4 -1
	0 5 -1)
	(0 3 -2
	0 4 -2
	0 5 -2)]

classe applique
nom 136942560
fils (136942696 136942440)

classe translation
nom 136942696
t_origine 0 0 6

classe applique
nom 136943304
fils (136943392 136943680)

classe translation
nom 136943392

classe iso_grille
nom 136943680
fils (136942440)

classe applique
nom 136943800
fils (136943960 136944632 136945216)

classe applique
nom 136943960
fils (136944120 136944256 136944544)

classe type_affichage
nom 136944120

classe translation
nom 136944256
t_origine 0 -3 3

classe sphere
nom 136944544

classe applique
nom 136944632
fils (136944792 136944928 136944544)

classe type_affichage
nom 136944792
affichage filaire

classe translation
nom 136944928
t_origine 0 -3 0

classe applique
nom 136945216
fils (136945376 136945512 136944544)

classe type_affichage
nom 136945376
affichage ponctuel

classe translation
nom 136945512
t_origine 0 -3 -3

classe applique
nom 136945800
fils (136946008 136946296 136947480 136941904 136942024)

classe translation
nom 136946008

classe applique
nom 136946296
fils (136946432 136946720)

classe homothetie
nom 136946432
angle 2

classe ligne_brisee
nom 136946720
fils (136946840)

classe table_point3
nom 136946840
fils (136947032 136947144 136947256 136947368)

classe point3
nom 136947032
x 0
y 1
z -1

classe point3
nom 136947144
x 0
y 1
z 1.01205

classe point3
nom 136947256
x 0
y -1
z 1

classe point3
nom 136947368
x 0
y -1
z -1

classe applique
nom 136947480
fils (136947616 136947904)

classe homothetie
nom 136947616
angle 1.5

classe polygone
nom 136947904
fils (136946840)

classe facette
nom 136941904
fils (136946840)

classe applique
nom 136942024
fils (136948552 136946840)

classe homothetie
nom 136948552
angle 2.5
