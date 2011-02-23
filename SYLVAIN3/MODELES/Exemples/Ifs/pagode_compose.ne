
classe vue
nom 1080123456
edite 135147008
fils (135409240)
titre Vue Perspective
pas_vue 0.1
angle_vue 0.1
point_de_vue 3.08383 2.77368 3.01583
haut -0.45382 -0.374568 0.808546
b_grille 0
b_repere 0
geometrie 4 -43 1299 1105

classe observateur
nom 135409240
observateur_point_de_vue 3.08383 2.77368 3.01583
observateur_haut -0.45382 -0.374568 0.808546
observateur_ratio 1.2575

classe applique
nom 135524760
fils (137123592 139002792 135964736)

classe type_affichage
nom 137123592
edite 134911880
affichage filaire

classe lampe
nom 139002792
observateur 2 3 4
	0 0 -1
	0 0 0
	0 1

classe composition
nom 135964736
edite 138051696
fils (137139096 135513856 136009936)
niveau 5
type Concatenation

classe applique
nom 137139096
fils (135513392 135440344)

classe repere
nom 135513392
t_origine -0.5 -0.5 -0.5
t_v1 2 0 0
t_v2 0 2 0
t_v3 0 0 2

classe cube
nom 135440344

classe operateur
nom 135513856
oper Exemples/Ifs/pagode_grille_2x2x2
designe_fils_au_lieu_operateur 1

classe operateur
nom 136009936
oper Exemples/Ifs/pagode_grille_2x2x2
designe_fils_au_lieu_operateur 1

classe vue_incluse
nom 1080303680
fils (137121440 137342488)
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
nom 137121440
observateur_point_de_vue 0 0.0995004 3.99002
observateur_point_vise 0 -0.299833 0.01
observateur_haut 0 0.995004 -0.0998334
observateur_focale 4
observateur_ratio 1.5625

classe applique
nom 137342488
fils (138815608 137488360 135502392 137128208 139705480 137590640 139706232 139706528 139707056 139707352 139707864 139708160)

classe lampe
nom 138815608
observateur 100 0 50
	0 0 -1
	0 0 0
	0 1

classe translation
nom 137488360
t_origine -45 0 0

classe couleur
nom 135502392
r 0

classe commande
nom 137128208
fils (135483544 135964736 135492928 137135656)
ferme 1

classe applique
nom 135483544
fils (136718016 135503360)

classe texte
nom 136718016
valeur Multiplication
hauteur 0.42285

classe facette
nom 135503360
t (-1 -1 -1
	15 -1 -1
	15 1.5 -1
	-1 1.5 -1)

classe texte
nom 135492928
valeur type

classe texte
nom 137135656
valeur Multiplication

classe translation
nom 139705480
fils (137130728)
ferme 1

classe vecteur3
nom 137130728
x 18
y 0
z 0

classe commande
nom 137590640
fils (139705904 135964736 135492928 139706128)
ferme 1

classe applique
nom 139705904
fils (139706024 135503360)

classe texte
nom 139706024
valeur Union
hauteur 0.29787

classe texte
nom 139706128
valeur Union
hauteur 0.310433

classe translation
nom 139706232
fils (137130728)
ferme 1

classe commande
nom 139706528
fils (139706712 135964736 135492928 139706944)
ferme 1

classe applique
nom 139706712
fils (139706832 135503360)

classe texte
nom 139706832
valeur Concatenation
hauteur 0.499337

classe texte
nom 139706944
valeur Concatenation
hauteur 0.437647

classe translation
nom 139707056
fils (137130728)
ferme 1

classe commande
nom 139707352
fils (139707536 135964736 135492928 139707760)

classe applique
nom 139707536
fils (139707656 135503360)

classe texte
nom 139707656
valeur Melange
hauteur 0.610351

classe texte
nom 139707760
valeur Melange
hauteur 0.600427

classe translation
nom 139707864
fils (137130728)
ferme 1

classe commande
nom 139708160
fils (139708344 135964736 135492928 139708568)

classe applique
nom 139708344
fils (139708464 135503360)

classe texte
nom 139708464
valeur Addition
hauteur 0.0897025

classe texte
nom 139708568
valeur Addition
hauteur 0.484422

classe vue_incluse
nom 1080483904
fils (137222672 138147344)
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
nom 137222672
observateur_point_de_vue 0.14 -0.1 4
observateur_point_vise 0.14 -0.1 0
observateur_haut 0 1 0
observateur_focale 17.93
observateur_ratio 1.5625

classe operateur
nom 138147344
fils (135513856)
oper pagode_menu_objet
designe_fils_au_lieu_operateur 1

classe vue_incluse
nom 1080664128
fils (137222672 135505928)
ferme 1
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
nom 135505928
fils (136009936)
oper pagode_menu_objet

classe applique
nom 137525328
fils (137525400 1080844352 1081024576)

classe affichage_ligne_de_rappel
nom 137525400
affiche_ligne_de_rappel 0

classe vue_incluse
nom 1080844352
fils (135409240 139002792 135513856)
xmin 0
xmax 300
ymin 50
ymax 350
est_designable 0
titre Vue Perspective
pas_vue 0.1
angle_vue 0.1
point_de_vue 3.08383 2.77368 3.01583
haut -0.45382 -0.374568 0.808546
b_grille 0
b_repere 0
geometrie 447 222 400 400

classe vue_incluse
nom 1081024576
fils (135409240 139002792 136009936)
xmin 900
xmax 1200
ymin 50
ymax 350
est_designable 0
titre Vue Perspective
pas_vue 0.1
angle_vue 0.1
point_de_vue 3.08383 2.77368 3.01583
haut -0.45382 -0.374568 0.808546
b_grille 0
b_repere 0
geometrie 398 179 400 400
