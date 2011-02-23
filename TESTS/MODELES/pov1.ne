
classe vue
nom 135852456
titre Vue Axe X
pas_vue 0.1
angle_vue 0.1
point_de_vue 4 -1 1
point_vise 0 -1 1
b_persp 0
b_entier 1
geometrie 551 393 392 459

classe vue
nom 135853280
titre Vue Axe Y
pas_vue 0.1
angle_vue 0.1
point_de_vue 0 4 0
b_persp 0
geometrie 679 570 227 320

classe vue
nom 135854128
titre Vue Axe Z
pas_vue 0.1
angle_vue 0.1
point_de_vue 0 0 4
haut 0 1 0
b_persp 0
geometrie 914 570 227 320

classe applique
nom 135854976
fils (135855232 135855360 135856832 135859232 135859520 135859648 135859744)

classe couleur
nom 135855232
b 0

classe applique
nom 135855360
fils (135855496 135855784)

classe translation
nom 135855496
t_origine 0 -2 0

classe soustraction_solide
nom 135855784
fils (135855920 135856008)

classe sphere
nom 135855920

classe applique
nom 135856008
fils (135856168 135856456 135856744)

classe translation
nom 135856168
t_origine 1 0 0

classe homothetie
nom 135856456
angle 0.5

classe sphere
nom 135856744

classe applique
nom 135856832
fils (135857112 135857208 135857304 135857432 135857944 135858728 135859016 135859144)

classe surface
nom 135857112
valeur 20

classe speculaire
nom 135857208

classe couleur
nom 135857304
r 0

classe applique
nom 135857432
fils (135857568 135857856)

classe translation
nom 135857568

classe sphere
nom 135857856

classe applique
nom 135857944
fils (135858128 135858224 135858352 135858640)

classe surface
nom 135858128

classe couleur
nom 135858224
v 0
b 0

classe translation
nom 135858352
t_origine 0 -3 2

classe sphere
nom 135858640

classe translation
nom 135858728
t_origine 0 -1 2

classe couleur
nom 135859016
r 0
b 0

classe sphere
nom 135859144

classe translation
nom 135859232
t_origine 0 1 2

classe couleur
nom 135859520
r 0
v 0

classe reflexion
nom 135859648
valeur 0.5

classe sphere
nom 135859744

classe lampe
nom 135859832
observateur 1.5 -1 1
	0 0 -1
	0 0 0
	0 1
attenuation_0_ 0
attenuation_1_ 1

classe pov
nom 135860072
fils (135860384)
hau_max 256
lar_max 256
qualite 8
antialiasing 1
trace 1
trace2 1

classe vue
nom 135860384
titre Vue Perspective
pas_vue 0.1
angle_vue 0.1
point_de_vue 5.79603 -0.605486 2.28837
point_vise -0.957012 -0.500023 0.743955
haut -0.218125 -0.281085 0.934565
geometrie 446 21 699 565

classe applique
nom 135861232
fils (135861416 135861704 135861992 135862280)

classe translation
nom 135861416
t_origine 0 -2 -1

classe homothetie
nom 135861704
angle 4

classe rotation
nom 135861992
angle -1.57
vecteur 0 1 0

classe image_l3_unsigned_char
nom 135862280

classe applique
nom 135862448
fils (135911824 135913968 135914544 135915120 135915696)

classe applique
nom 135911824
fils (135911984 135912112 135913304)

classe couleur
nom 135911984
b 0

classe applique
nom 135912112
fils (135318584 135318872 135913240)

classe translation
nom 135318584
t_origine 0 1 0

classe couleur
nom 135318872
priorite -2

classe cube
nom 135913240

classe applique
nom 135913304
fils (135913464 135913752 135913880)

classe translation
nom 135913464
t_origine 0 -1 -1

classe couleur
nom 135913752
priorite 2
v 0

classe cone
nom 135913880

classe applique
nom 135913968
fils (135914128 135914256 135911824)

classe couleur
nom 135914128
priorite 3

classe translation
nom 135914256
t_origine 0.1 0.1 -0.1

classe applique
nom 135914544
fils (135914704 135914832 135911824)

classe couleur
nom 135914704
priorite 1

classe translation
nom 135914832
t_origine 0.2 0.2 -0.2

classe applique
nom 135915120
fils (135915280 135915408 135911824)

classe couleur
nom 135915280

classe translation
nom 135915408
t_origine 0.3 0.3 -0.3

classe applique
nom 135915696
fils (135915856 135915984 135911824)

classe couleur
nom 135915856
priorite -1

classe translation
nom 135915984
t_origine 0.4 0.4 -0.4

classe applique
nom 135916272
fils (135916408 135916504)

classe epaisseur
nom 135916408
epaisseur_trait 0.05

classe ligne_brisee
nom 135916504
t (1.3 -2 -0.5
	0 2 2.5)
