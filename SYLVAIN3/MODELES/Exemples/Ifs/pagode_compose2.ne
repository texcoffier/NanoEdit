
classe vue
nom 1080119360
edite 135333880
fils (136458424)
titre Vue Perspective
pas_vue 0.1
angle_vue 0.1
point_de_vue 4.50773 5.41414 5.64588
haut -0.437395 -0.448586 0.779395
geometrie -50 8 1299 1105

classe observateur
nom 136458424
observateur_point_de_vue 4.50773 5.41414 5.64588
observateur_haut -0.437395 -0.448586 0.779395
observateur_ratio 1.2575

classe vue_incluse
nom 1080299584
fils (136458424 137215008 137394216)
xmin 0
xmax 300
ymin 50
ymax 350
titre Vue Perspective
pas_vue 0.1
angle_vue 0.1
point_de_vue 4.50773 5.41414 5.64588
haut -0.437395 -0.448586 0.779395
b_grille 0
geometrie 447 222 400 400

classe lampe
nom 137215008
observateur 2 3 4
	0 0 -1
	0 0 0
	0 1

classe operateur
nom 137394216
oper pagode_point_z

classe vue_incluse
nom 1080664128
fils (136458424 137215008 136956296)
xmin 900
xmax 1200
ymin 50
ymax 350
titre Vue Perspective
pas_vue 0.1
angle_vue 0.1
point_de_vue 4.50773 5.41414 5.64588
haut -0.437395 -0.448586 0.779395
b_grille 0
geometrie 398 179 400 400

classe operateur
nom 136956296
oper pagode_ligne_y

classe applique
nom 137580448
fils (137215008 136259240)

classe composition
nom 136259240
fils (136931408 137394216 136956296)
niveau 4
type Multiplication

classe cube
nom 136931408

classe vue_incluse
nom 1085710400
fils (136422088 136493832)
ferme 1
xmin 0
xmax 1280
ymin 950
ymax 1024
titre Vue Z
pas_vue 0.1
angle_vue 0.1
taille_vue 4
point_de_vue 0 0.0995004 3.99002
point_vise 0 -0.299833 0.01
haut 0 0.995004 -0.0998334
b_persp 0
b_grille 0
b_repere 0
geometrie 763 450 400 400

classe observateur
nom 136422088
observateur_point_de_vue 0 0.0995004 3.99002
observateur_point_vise 0 -0.299833 0.01
observateur_haut 0 0.995004 -0.0998334
observateur_focale 4
observateur_ratio 1.5625

classe applique
nom 136493832
fils (137407824 136280984 136443496 136432992 136262240 136956920 136262992 136263288 136263816 136264112 136264624 136264920)

classe lampe
nom 137407824
observateur 100 0 50
	0 0 -1
	0 0 0
	0 1

classe translation
nom 136280984
t_origine -45 0 0

classe couleur
nom 136443496
r 0

classe commande
nom 136432992
fils (137218448 136259240 136433464 136274368)
ferme 1

classe applique
nom 137218448
fils (137403216 136455400)

classe texte
nom 137403216
valeur Multiplication
hauteur 0.42285

classe facette
nom 136455400
t (-1 -1 -1
	15 -1 -1
	15 1.5 -1
	-1 1.5 -1)

classe texte
nom 136433464
valeur type

classe texte
nom 136274368
valeur Multiplication

classe translation
nom 136262240
fils (137580536)
ferme 1

classe vecteur3
nom 137580536
x 18
y 0
z 0

classe commande
nom 136956920
fils (136262664 136259240 136433464 136262888)
ferme 1

classe applique
nom 136262664
fils (136262784 136455400)

classe texte
nom 136262784
valeur Union
hauteur 0.29787

classe texte
nom 136262888
valeur Union
hauteur 0.310433

classe translation
nom 136262992
fils (137580536)
ferme 1

classe commande
nom 136263288
fils (136263472 136259240 136433464 136263704)
ferme 1

classe applique
nom 136263472
fils (136263592 136455400)

classe texte
nom 136263592
valeur Concatenation
hauteur 0.499337

classe texte
nom 136263704
valeur Concatenation
hauteur 0.437647

classe translation
nom 136263816
fils (137580536)
ferme 1

classe commande
nom 136264112
fils (136264296 136259240 136433464 136264520)
ferme 1

classe applique
nom 136264296
fils (136264416 136455400)

classe texte
nom 136264416
valeur Melange
hauteur 0.610351

classe texte
nom 136264520
valeur Melange
hauteur 0.600427

classe translation
nom 136264624
fils (137580536)
ferme 1

classe commande
nom 136264920
fils (136265104 136259240 136433464 136929288)
ferme 1

classe applique
nom 136265104
fils (136265224 136455400)

classe texte
nom 136265224
valeur Addition
hauteur 0.0897025

classe texte
nom 136929288
valeur Addition
hauteur 0.484422

classe vue_incluse
nom 1096777792
fils (136281464 136624328)
xmin 0
ymin 300
ymax 600
titre Vue Z
pas_vue 0.01
angle_vue 0.1
taille_vue 17.93
point_de_vue 0.14 -0.1 4
point_vise 0.14 -0.1 0
haut 0 1 0
b_persp 0
b_grille 0
b_repere 0
geometrie 374 58 400 400

classe observateur
nom 136281464
observateur_point_de_vue 0.14 -0.1 4
observateur_point_vise 0.14 -0.1 0
observateur_haut 0 1 0
observateur_focale 17.93
observateur_ratio 1.5625

classe operateur
nom 136624328
fils (137394216)
oper pagode_menu_objet

classe vue_incluse
nom 1096958016
fils (136281464 136279792)
xmin 1100
xmax 1280
ymin 300
ymax 600
titre Création de ``operateur'' faite
pas_vue 0.1
angle_vue 0.1
taille_vue 17.93
point_de_vue 0.14 -0.1 4
point_vise 0.14 -0.1 0
haut 0 1 0
b_persp 0
b_grille 0
b_repere 0
geometrie 306 487 400 400

classe operateur
nom 136279792
fils (136956296)
oper pagode_menu_objet

classe fractale
nom 136253664
fils (137240184 136427960 136428224 137141624 137141888 137142152 137330576 137330840 137331104)

classe applique
nom 137240184
fils (136427696 136958944)

classe homothetie
nom 136427696
angle 2

classe cube
nom 136958944

classe repere
nom 136427960
t_origine -0.5 -0.5 -0.5
t_v1 0.5 0 0
t_v2 0 0.5 0
t_v3 0 0 0.5

classe repere
nom 136428224
t_origine -0.5 -0.5 0.5
t_v1 0.5 0 0
t_v2 0 0.5 0
t_v3 0 0 0.5

classe repere
nom 137141624
t_origine -0.5 0.5 -0.5
t_v1 0.5 0 0
t_v2 0 0.5 0
t_v3 0 0 0.5

classe repere
nom 137141888
t_origine -0.5 0.5 0.5
t_v1 0.5 0 0
t_v2 0 0.5 0
t_v3 0 0 0.5

classe repere
nom 137142152
t_origine 0.5 -0.5 -0.5
t_v1 0.5 0 0
t_v2 0 0.5 0
t_v3 0 0 0.5

classe repere
nom 137330576
t_origine 0.5 -0.5 0.5
t_v1 0.5 0 0
t_v2 0 0.5 0
t_v3 0 0 0.5

classe repere
nom 137330840
t_origine 0.5 0.5 -0.5
t_v1 0.5 0 0
t_v2 0 0.5 0
t_v3 0 0 0.5

classe repere
nom 137331104
t_origine 0.5 0.5 0.5
t_v1 0.5 0 0
t_v2 0 0.5 0
t_v3 0 0 0.5

classe lampe
nom 137331368
observateur 1 1 2
	0 0 -1
	0 0 0
	0 1
