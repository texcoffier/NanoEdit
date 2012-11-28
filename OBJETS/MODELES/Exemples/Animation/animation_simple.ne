
classe applique
nom 167438824
fils (165994016 150530192)
ferme 1
commentaire Cylindre

classe translation
nom 165994016
fils (150509016)
cache_tag 1

classe vecteur3
nom 150509016
fils (184886344)
y 0
z 0

classe seconde
nom 184886344
edite 150903920
temps 0 0 20
increment 0.04

classe cylindre
nom 150530192

classe lampe
nom 160958872
observateur 6.22877 0.495751 6.00901
	0 0 -1
	0 0 0
	0 1

classe applique
nom 160022696
fils (150931704 162883896 160038504)
ferme 1
commentaire Cube

classe type_affichage
nom 150931704

classe rotation
nom 162883896
fils (184886344)
cache_tag 1

classe cube
nom 160038504

classe applique
nom 149458296
fils (162893208 164192968 189441072)
ferme 1
commentaire Cone

classe rotation
nom 162893208
fils (184886344)
vecteur 1 0 0
cache_tag 1

classe translation
nom 164192968
t_origine 0 2 0
cache_tag 1

classe cone
nom 189441072

classe applique
nom 160877008
fils (167311320 160305528)
ferme 1
commentaire Sphere sur ligne brisée

classe translation
nom 167311320
fils (160384648)
cache_tag 1

classe vecteur_triplet
nom 160384648
fils (190048056)

classe interpolateur_quadruplet
nom 190048056
fils (160316656 150340856 184886344 189434568)

classe flottant
nom 160316656
valeur 3

classe table_nc_flottant
nom 150340856
t (1 1 1 1 8)

classe table_nc_point3
nom 189434568
t (0.152461 -3.12769 -0.253509
	-1.96659 -3.73398 2.95523
	-1.99526 -3.37444 2.26096
	-2.86101 0.86939 2.60196
	-1.73221 0.847993 1.58004
	1.95112 3.68424 -1.89637)

classe applique
nom 160305528
fils (162473176 150481872)
commentaire sphere coloriee

classe couleur_triplet
nom 162473176
fils (162459088)

classe interpolateur_quadruplet
nom 162459088
fils (159842280 0 184886344 162642496)
temps_clefs (1 1 1 1 1 1 1 1)
chemin boucle

classe flottant
nom 159842280
valeur 0

classe table_nc_point3
nom 162642496
t (0 0 0
	1 0 0
	1 1 0
	0 1 0
	0 1 1
	1 1 1
	1 0 1
	0 0 1
	0 0 0)

classe sphere
nom 150481872

classe applique
nom 189928216
fils (166617144 160119112)
ferme 1
commentaire Sphere a vitesse constante

classe translation
nom 166617144
fils (166441072)
cache_tag 1

classe vecteur_triplet
nom 166441072
fils (150820952)

classe interpolateur_quadruplet
nom 150820952
fils (163938352 162854064 184886344 190028752)

classe flottant
nom 163938352
valeur 0

classe longueurs_table
nom 162854064
fils (190028752)

classe table_nc_point3
nom 190028752
t (2.85574 -4.31466 -2.49529
	2.71234 -4.31448 -2.33223
	2.5679 -4.36967 -2.15831
	1.37103 -4.6013 -0.756571
	1.16787 -4.67411 -0.514432
	-0.16649 -5.03023 1.05923
	-0.381924 -5.10328 1.32483
	-1.34439 -5.3119 2.45594)

classe sphere
nom 160119112

classe applique
nom 160760760
fils (189600184 162629968 163248328)
ferme 1
commentaire petite sphere boucle

classe translation
nom 189600184
fils (166470416)
cache_tag 1

classe vecteur_triplet
nom 166470416
fils (167572904)

classe interpolateur_quadruplet
nom 167572904
fils (163015976 0 184886344 163291952)
temps_clefs (1 0.5)
chemin boucle

classe flottant
nom 163015976
valeur 0

classe table_nc_point3
nom 163291952
t (-1.83424 3.42656 1.1878
	-2.39422 3.97534 1.63633
	-1.84848 4.32999 1.04559)

classe homothetie
nom 162629968
homo 0.2
cache_tag 1

classe sphere
nom 163248328

classe applique
nom 166516968
fils (163673880 162629968 163248328)
ferme 1
commentaire petite sphere ping pong

classe translation
nom 163673880
fils (161066728)
cache_tag 1

classe vecteur_triplet
nom 161066728
fils (189552320)

classe interpolateur_quadruplet
nom 189552320
fils (189605840 0 184886344 162475640)
temps_clefs (0.5 1)
chemin pingpong

classe flottant
nom 189605840
valeur 0

classe table_nc_point3
nom 162475640
t (-0.748471 3.46271 0.128125
	-1.30845 4.01149 0.576652
	-0.917441 4.52547 0.108508)

classe applique
nom 150481456
fils (162335320 166711760 162652472 166440936 162353888 162947832 162333560)
ferme 1
commentaire Les lignes brisees

classe ligne_brisee
nom 162335320
fils (189434568)
ferme 1

classe ligne_brisee
nom 166711760
fils (190028752)
ferme 1

classe ligne_brisee
nom 162652472
fils (163291952)
ferme 1

classe ligne_brisee
nom 166440936
fils (162475640)
ferme 1

classe iso_grille
nom 162353888
fils (167549896)

classe grille_point3
nom 167549896
fils (150559016 160961328 135836256)

classe table_nc_point3
nom 150559016
t (-0.758855 1.79054 3.30475
	0.183794 3.60218 2.07761
	1.22749 5.00958 2.6263)

classe table_nc_point3
nom 160961328
t (-0.714285 3.34037 2.66202
	0.234946 3.66422 2.97821
	1.15464 3.97946 2.55942)

classe table_nc_point3
nom 135836256
t (-0.67682 2.09827 3.51863
	0.311717 3.91401 3.23094
	1.34857 4.6559 3.0207)

classe iso_grille
nom 162947832
fils (161319352)

classe grille_point4
nom 161319352
fils (167456456 166696520 160943520)

classe table_point3
nom 167456456
fils (162964872 166387576 162811752 189980400)

classe point4
nom 162964872
x -1.04433
y 3.71736
z -1.29583

classe point4
nom 166387576
x -1.22538
y 4.19694
z -1.3677

classe point4
nom 162811752
x -1.39395
y 4.64344
z -1.43461

classe point4
nom 189980400
x -1.53616
y 5.04079
z -1.51022

classe table_point4
nom 166696520
fils (159923544 160047352 162459776 160909800)

classe point4
nom 159923544
x -0.87366
y 3.76157
z -1.68787
h 0

classe point4
nom 160047352
x -1.05472
y 4.24115
z -1.75975
h 0

classe point4
nom 162459776
x -1.21843
y 4.65411
z -1.80557
h 0

classe point4
nom 160909800
x -1.36827
y 5.051
z -1.86505
h 0

classe table_point3
nom 160943520
fils (165278120 162491688 164266288 167566768)

classe point4
nom 165278120
x -0.68773
y 3.80671
z -2.11218

classe point4
nom 162491688
x -0.85491
y 4.27021
z -2.1977

classe point4
nom 164266288
x -1.00336
y 4.6841
z -2.27579

classe point4
nom 167566768
x -1.18234
y 5.09613
z -2.28936

classe iso_grille
nom 162333560
fils (160483208)

classe grille_point3
nom 160483208
fils (166458304 150907272 190178168 165675040)

classe table_point3
nom 166458304
fils (160868960 166449744 162459976)

classe point3
nom 160868960
x -0.682157
y 4.43297
z -2.70387

classe point3
nom 166449744
x -0.808411
y 4.7467
z -2.73483

classe point3
nom 162459976
x -0.970736
y 5.17667
z -2.79927

classe table_point4
nom 150907272
fils (164757608 150866752 166515904)

classe point3
nom 164757608
x -0.504549
y 4.56218
z -3.18896

classe point3
nom 150866752
x -0.600974
y 4.77623
z -3.18892

classe point3
nom 166515904
x -0.777174
y 5.22227
z -3.2397

classe table_point3
nom 190178168
fils (162426712 189985792 189435824)

classe point3
nom 162426712
x -0.331104
y 4.64039
z -3.61822

classe point3
nom 189985792
x -0.485108
y 4.98628
z -3.62188

classe point3
nom 189435824
x -0.639111
y 5.33216
z -3.62553

classe table_point4
nom 165675040
fils (193361808 191073544 150462104)

classe point3
nom 193361808
x -0.133378
y 4.737
z -4.11448

classe point3
nom 191073544
x -0.281138
y 5.06634
z -4.11565

classe point3
nom 150462104
x -0.417103
y 5.44716
z -4.18878

classe applique
nom 193361896
fils (163062760 189995384)
ferme 1
commentaire Ligne jaune

classe couleur
nom 163062760
b 0

classe ligne_brisee
nom 189995384
fils (150895488)

classe interpolateur_table_quadruplet
nom 150895488
fils (165935368 165935456 184886344 167549896)
chemin pingpong

classe flottant
nom 165935368
valeur 0

classe table_nc_flottant
nom 165935456
t (1 2)

classe applique
nom 190672000
fils (190672136 190672256)
ferme 1
commentaire Grille 4 3

classe couleur
nom 190672136
r 1.06888
v -4.49767
b 1.38376

classe ligne_brisee
nom 190672256
fils (190672360)

classe interpolateur_table_quadruplet
nom 190672360
fils (160342608 160342680 184886344 161319352)
chemin boucle

classe flottant
nom 160342608
valeur 0

classe table_nc_flottant
nom 160342680
t (1 1)

classe applique
nom 166643864
fils (160137904 162893672)
ferme 1
commentaire Grille 3 4

classe couleur
nom 160137904
v 0.38376

classe ligne_brisee
nom 162893672
fils (167550016)

classe interpolateur_table_quadruplet
nom 167550016
fils (166448304 166448392 184886344 160483208)
chemin pingpong

classe flottant
nom 166448304
valeur 0

classe table_nc_flottant
nom 166448392
t (1 1 1)

classe applique
nom 160349256
fils (189574136 164701232 165992936)
ferme 1
commentaire Le texte changeant

classe translation
nom 189574136
t_origine -0.555354 -2.56483 1.77064
cache_tag 1

classe switch
nom 164701232
fils (184885248 184885336 184886344 165992696 165992816)

classe flottant
nom 184885248
valeur 4

classe table_nc_flottant
nom 184885336
t (2 2)

classe couleur
nom 165992696
b 0

classe couleur
nom 165992816
v 0

classe switch
nom 165992936
fils (160316656 150340856 184886344 189701120 189701208 189701296 189701384 189701472)

classe texte
nom 189701120
valeur SPHERE SUR  PREMIER SEGMENT

classe texte
nom 189701208
valeur SPHERE SUR  DEUXIEME SEGMENT

classe texte
nom 189701296
valeur SPHERE SUR TROISIEME SEGMENT

classe texte
nom 189701384
valeur SPHERE SUR  QUATRIEME SEGMENT

classe texte
nom 189701472
valeur SPHERE SUR  DERNIER SEGMENT

classe applique
nom 189701600
fils (166449928 190497016)
ferme 1
commentaire Sphere déplacée par cliquez-moi

classe translation
nom 166449928
fils (189701792)
cache_tag 1

classe vecteur_triplet
nom 189701792
fils (189566256)

classe interpolateur_quadruplet
nom 189566256
fils (189701912 161474312 184886344 190496776)

classe flottant
nom 189701912
valeur 1.36

classe table_nc_flottant
nom 161474312
t (0.5)

classe table_point3
nom 190496776
fils (161474384 190496912)

classe point3
nom 161474384
x 1.71033
y -0.0420863
z -3.49812

classe point3
nom 190496912
x 0.868222
y 2.39856
z -3.70784

classe applique
nom 190497016
fils (149464912 164757696)

classe homothetie
nom 149464912
homo 0.2
cache_tag 1

classe sphere
nom 164757696

classe commande
nom 190497184
fils (190497368 189701912 166629440 184886344)
ferme 1

classe applique
nom 190497368
fils (190497504 166474600 166629352)
commentaire Cliquez moi !

classe facette
nom 190497504
t (2.12214 0.21457 -1.72746
	2.06572 2.78814 -1.24467
	2.94361 2.83934 -1.10758
	4.00017 0.97911 -0.624809)

classe translation
nom 166474600
t_origine 4.62787 1.64537 0.292327
cache_tag 1

classe texte
nom 166629352
valeur Cliquez-moi !

classe texte
nom 166629440
valeur valeur

classe pov
nom 189739344
fils (1222152256)
hau_max -2147483648

classe vue
nom 1222152256
edite 135644056
titre Vue Perspective
pas_vue 0.1
angle_vue 0.1
point_de_vue 9.52193 4.01149 4.39919
point_vise 0.0855098 0.00519892 -0.180724
haut -0.427933 -0.024868 0.903468
geometrie 521 261 699 565

classe audio
nom 190054432
fils (159814080)
ferme 1
joue_si_modifie 1

classe switch
nom 159814080
fils (166619136 150340856 184886344 166619224 166619328 166619432 166619536 166619640)

classe flottant
nom 166619136
valeur 3

classe texte
nom 166619224
valeur seg1.wav

classe texte
nom 166619328
valeur seg2.wav

classe texte
nom 166619432
valeur seg3.wav

classe texte
nom 166619536
valeur seg4.wav

classe texte
nom 166619640
valeur seg5.wav

classe audio
nom 166619744
nom_fichier onoff.wav
joue_a_la_creation 1

classe commande
nom 166619848
fils (190497368 162665416)
ferme 1
nom_champ virtuel_audio_0
valeur 1

classe audio
nom 162665416
nom_fichier /usr/share/sounds/pop.wav

classe vue
nom 1223856192
edite 166686584
fils (216433240)
pas_vue 0.1
angle_vue 0.1
taille_vue 0.999999
point_de_vue 7 5 5
point_vise 6.353 4.46084 4.46084
haut -0.414197 -0.345164 0.842201
geometrie 851 38 400 400

classe observateur_repere
nom 216433240
fils (163010728)

classe repere_axe
nom 163010728
fils (165867296)
inverse_z 1

classe point_triplet
nom 165867296
fils (189586488)

classe interpolateur_quadruplet
nom 189586488
fils (190760056 190760128 184886344 190191648)
chemin boucle

classe flottant
nom 190760056
valeur 0

classe table_nc_flottant
nom 190760128
t (1 1 1)

classe table_point3
nom 190191648
fils (190191784 190191888 190191992 166212944)

classe point3
nom 190191784
x 7
y 5
z 5

classe point3
nom 190191888
x 7
y -5
z 5

classe point3
nom 190191992
x 7
y 0
z 0

classe point3
nom 166212944
x 7
y 5
z 5
