
classe vue
trace2 1
nom 135447224
edite 135551544
fils (135447960)
titre Vue Axe X
pas_vue 0.1
angle_vue 0.1
point_de_vue 2.85153 -16.34 4.98737
haut 0.0513123 0.299726 0.952645
geometrie 458 86 372 477

classe observateur
nom 135447960
edite 135456360
observateur_point_de_vue 2.85153 -16.34 4.98737
observateur_haut 0.0513123 0.299726 0.952645
observateur_ratio 0.971279

classe applique
nom 135448112
fils (135448416 0 135448656 135448784 135452416 135453200 135453488 135453584 135453680)

classe lampe
nom 135448416
observateur 1 1.5 2.5
	0 0 -1
	0 0 0
	0 1

classe couleur
nom 135448656

classe intersection_solide
nom 135448784
fils (135448920 135449744)

classe applique
nom 135448920
fils (135449080 135449368 135449656)

classe translation
nom 135449080
t_origine 0 0 0.5

classe homothetie
nom 135449368
homo 1.05

classe sphere
nom 135449656

classe soustraction_solide
nom 135449744
fils (135449880 135451592)

classe union_solide
nom 135449880
fils (135450040 135450128 135450792)

classe cylindre
nom 135450040

classe applique
nom 135450128
fils (135450464 135450728)

classe translation
nom 135450464
t_origine -0.5 -0.5 1

classe cube
nom 135450728

classe applique
nom 135450792
fils (135450928 135451216 135451504)

classe homothetie
nom 135450928
homo 1.3

classe translation
nom 135451216
t_origine 0 0 -1

classe cone
nom 135451504

classe applique
nom 135451592
fils (135451752 135452040 135452328)

classe homothetie
nom 135451752
homo 5

classe translation
nom 135452040
t_origine 0 0 -0.5

classe cube
nom 135452328

classe applique
nom 135452416
fils (135452600 135452888 135452984 135453112)

classe homothetie
nom 135452600
homo 0.3

classe reflexion
nom 135452888
valeur 0.7

classe couleur
nom 135452984
r 0.250948
v 0.732523
b 0.362387

classe sphere
nom 135453112

classe translation
nom 135453200
t_origine 0 0 -0.5

classe reflexion
nom 135453488
valeur 0.5

classe diffuse
nom 135453584
valeur 0.4

classe plan
nom 135453680
affiche_miroir 1
