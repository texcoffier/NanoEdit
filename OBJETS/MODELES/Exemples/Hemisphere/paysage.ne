
classe applique
nom 896285520
fils (912776608 913782336 873227744 860715504 912759280 896523560 912772336 913691832 914512776 916466200 916466736)
commentaire Un paysage bougeant

classe type_affichage
nom 912776608
affichage invisible

classe lampe
nom 913782336
observateur 1 2 4
	0 0 -1
	0 0 0
	0 1

classe type_affichage
nom 873227744

classe applique
nom 860715504
fils (915557384 915557664)
ferme 1
commentaire super sphere

classe translation
nom 915557384
t_origine 5 5 0
cache_tag 1
cache_tag_symetrie 1

classe super_sphere
nom 915557664
fils (916459624 916459904)

classe dilatation
nom 916459624
vecteur 3 3 3
cache_tag 1
cache_tag_symetrie 1

classe translation
nom 916459904
fils (896614424)
cache_tag 1
cache_tag_symetrie 1

classe vecteur3
nom 896614424
fils (853244136 916460256 916460400)

classe evaluateur
nom 853244136
fils (916460184)
expression 2 + cos $0

classe evaluateur
nom 916460184
fils (597224568)
expression $0 * 4

classe variable
nom 597224568
fils (914513624)
nom_variable 0=secondes

classe seconde
nom 914513624
temps 0 0 1

classe evaluateur
nom 916460256
fils (916460328)
expression 2 + sin $0

classe evaluateur
nom 916460328
fils (597224568)
expression $0 * 5

classe evaluateur
nom 916460400
fils (913492480)
expression 3 + sin $0 + cos $0

classe evaluateur
nom 913492480
fils (597224568)
expression $0 * 7

classe applique
nom 912759280
fils (916460504 915970488 916460784 913782552)
ferme 1
commentaire couleur changeante

classe translation
nom 916460504
t_origine -5 -5 0
cache_tag 1
cache_tag_symetrie 1

classe couleur
nom 915970488
fils (912758416 912774472 912768064)

classe evaluateur
nom 912758416
fils (912854376)
expression 1 + cos $0

classe evaluateur
nom 912854376
fils (597224568)
expression $0 * 4

classe evaluateur
nom 912774472
fils (912770200)
expression 1 + sin $0

classe evaluateur
nom 912770200
fils (597224568)
expression $0 * 5

classe evaluateur
nom 912768064
fils (912758752)
expression 2 + sin $0 + cos $0

classe evaluateur
nom 912758752
fils (597224568)
expression $0 * 7

classe dilatation
nom 916460784
vecteur 1 1 10
cache_tag 1
cache_tag_symetrie 1

classe cube
nom 913782552

classe applique
nom 896523560
fils (916461064 916461344 916461624 916461904 912757192)
ferme 1
commentaire cube cone tournant

classe translation
nom 916461064
t_origine 5 -5 0
cache_tag 1
cache_tag_symetrie 1

classe rotation
nom 916461344
fils (862344968)
cache_tag 1
cache_tag_symetrie 1

classe evaluateur
nom 862344968
fils (597224568)
expression $0 * 3

classe dilatation
nom 916461624
vecteur 2 2 2
cache_tag 1
cache_tag_symetrie 1

classe cube
nom 916461904

classe applique
nom 912757192
fils (916462064 916462344)

classe translation
nom 916462064
t_origine 1 0 0
cache_tag 1
cache_tag_symetrie 1

classe cone
nom 916462344

classe applique
nom 912772336
fils (916462504 916462784 916463064)
ferme 1
commentaire cube bondissant

classe dilatation
nom 916462504
fils (896270888)
cache_tag 1
cache_tag_symetrie 1

classe vecteur3
nom 896270888
fils (912778744 916454328 914513480)

classe flottant
nom 912778744
valeur 1

classe flottant
nom 916454328
valeur 1

classe evaluateur
nom 914513480
fils (915553584)
expression 1.1 + cos $0 * 4

classe evaluateur
nom 915553584
fils (597224568)
expression 10 * $0

classe translation
nom 916462784
t_origine -5 5 0
cache_tag 1
cache_tag_symetrie 1

classe cube
nom 916463064

classe applique
nom 913691832
fils (913781152 916463224 0 916463504 916463784)
ferme 1
commentaire tore en haut

classe couleur
nom 913781152
v 0
b 0

classe translation
nom 916463224
t_origine 0 0 5
cache_tag 1
cache_tag_symetrie 1

classe dilatation
nom 916463504
vecteur 3 3 3
cache_tag 1
cache_tag_symetrie 1

classe tore
nom 916463784
rayon 0.2

classe applique
nom 914512776
fils (915035088 916463952 916465760 916466040)
commentaire sphere baladeuse

classe couleur
nom 915035088
r 0
b 0

classe translation
nom 916463952
fils (915557848)
cache_tag 1
cache_tag_symetrie 1

classe vecteur_triplet
nom 915557848
fils (916464232)

classe interpolateur_quadruplet
nom 916464232
fils (916464584 916464656 597224568 916465160)
chemin pingpong

classe flottant
nom 916464584
valeur 0

classe table_flottant
nom 916464656
fils (916464728 916464800 916464872 916464944 916465016 916465088)

classe flottant
nom 916464728
valeur 0.2

classe flottant
nom 916464800
valeur 0.2

classe flottant
nom 916464872
valeur 0.2

classe flottant
nom 916464944
valeur 0.2

classe flottant
nom 916465016
valeur 0.2

classe flottant
nom 916465088
valeur 1

classe table_point4
nom 916465160
fils (916465232 916465320 916465408 916465496 916465584 916465672)

classe point3
nom 916465232
x 0
y 0
z 0

classe point3
nom 916465320
x 1.50097
y 0.40616
z 0.282381

classe point3
nom 916465408
x 0
y 1.25318
z -0.488974

classe point3
nom 916465496
x 0
y 0.705069
z 0.931038

classe point3
nom 916465584
x 1
y 0
z 1

classe point3
nom 916465672
x 1
y -0.860745
z 0.018415

classe dilatation
nom 916465760
vecteur 0.1 0.1 0.1
cache_tag 1
cache_tag_symetrie 1

classe sphere
nom 916466040

classe applique
nom 916466200
fils (916466272 916466552 916466656)
ferme 1
commentaire sol

classe translation
nom 916466272
t_origine 0 0 -0.5
cache_tag 1
cache_tag_symetrie 1

classe couleur
nom 916466552
r 0.7
v 0.3
b 0.4

classe plan
nom 916466656
taille 500

classe applique
nom 916466736
fils (916466808 916467088 916467192 916467472)
ferme 1
commentaire ciel

classe translation
nom 916466808
t_origine 0 0 20
cache_tag 1
cache_tag_symetrie 1

classe couleur
nom 916467088
r 0.8
v 0.8

classe rotation
nom 916467192
angle 3.14159
vecteur 1 0 0
cache_tag 1
cache_tag_symetrie 1

classe plan
nom 916467472
affiche_ombres 0
taille 500

classe vue
nom -1219489728
edite 854851192
pas_vue 0.1
angle_vue 0.1
point_de_vue 1.58295 1.06268 1.50992
haut -0.519481 -0.340024 0.783915
geometrie 5 21 651 710

classe ligne_brisee
nom 916529752
fils (916465160)
