
classe vue
nom 1085689920
edite 135947008
titre Vue X
pas_vue 0.1
angle_vue 0.1
taille_vue 4
point_de_vue 4 0 0
b_persp 0
geometrie 444 570 227 320

classe vue
nom 1085874240
edite 135836768
titre Vue Y
pas_vue 0.1
angle_vue 0.1
taille_vue 4
point_de_vue 0 4 0
b_persp 0
geometrie 677 568 227 320

classe vue
nom 1086058560
edite 135727560
titre Vue Z
pas_vue 0.1
angle_vue 0.1
taille_vue 4
point_de_vue 0 0 4
haut 0 1 0
b_persp 0
geometrie 912 568 227 320

classe applique
nom 136637456
fils (136143648 136639816 136603920 136640064 136135384 136136104 136136384 136136472 136136560)

classe lampe
nom 136143648
observateur 1 1.5 2.5
	0 0 -1
	0 0 0
	0 1

classe lampe
nom 136639816
observateur 3 -4 2
	0 0 -1
	0 0 0
	0 1
couleur 1 0 0

classe couleur
nom 136603920

classe intersection_solide
nom 136640064
fils (136640176 136641408)

classe applique
nom 136640176
fils (136640312 136640592 136640872)

classe translation
nom 136640312
t_origine 0 0 0.5

classe homothetie
nom 136640592
homo 1.05

classe sphere
nom 136640872

classe soustraction_solide
nom 136641408
fils (136641520 136642984)

classe union_solide
nom 136641520
fils (136641648 136641728 136642208)

classe cylindre
nom 136641648

classe applique
nom 136641728
fils (136641848 136642128)

classe translation
nom 136641848
t_origine -0.5 -0.5 1

classe cube
nom 136642128

classe applique
nom 136642208
fils (136642344 136642624 136642904)

classe homothetie
nom 136642344
homo 1.3

classe translation
nom 136642624
t_origine 0 0 -1

classe cone
nom 136642904

classe applique
nom 136642984
fils (136134808 136135088 136643120)

classe homothetie
nom 136134808
homo 5

classe translation
nom 136135088
t_origine 0 0 -0.5

classe cube
nom 136643120

classe applique
nom 136135384
fils (136135536 136135816 136135904 136136024)

classe homothetie
nom 136135536
homo 0.3

classe reflexion
nom 136135816
valeur 0.7

classe couleur
nom 136135904
r 0.250948
v 0.732523
b 0.362387

classe sphere
nom 136136024

classe translation
nom 136136104
t_origine 0 0 -0.5

classe reflexion
nom 136136384
valeur 0.5

classe diffuse
nom 136136472
valeur 0.4

classe plan
nom 136136560
affiche_miroir 1

classe pov
nom 136136648
fils (1086242880)
hau_max -2147483648
qualite 9

classe vue
nom 1086242880
edite 135590272
titre Vue Perspective
pas_vue 0.1
angle_vue 0.1
taille_vue 2
point_de_vue 2.4655 2.95403 1.54051
point_vise -0.478616 -0.562916 -0.264144
haut -0.192407 -0.315706 0.929144
geometrie 404 81 392 394
