
classe vue
nom -1216282560
edite 135345752
fils (134958336)
titre Vue Hémisphérique
pas_vue 0.1
angle_vue 0.1
point_de_vue -0.279409 -7.75951 1.87184
point_vise 0.261633 5.05447 5.01249
haut -0.00229265 -0.237957 0.971273
b_persp 2:Hémis
geometrie 57 8 1115 1011

classe observateur
nom 134958336
fils (134958520 134958984)
observateur_haut -0.00234086 -0.239099 0.970992
observateur_ratio 1.18743

classe point3
nom 134958520
fils (134958672 134958760 134958848)

classe flottant
nom 134958672
valeur -0.619677

classe flottant
nom 134958760
valeur 0.00324218

classe seconde
nom 134958848
edite 135324968
temps 8.0971 0 8
increment 0.04

classe point3
nom 134958984
fils (134959136 134959376 135578808)

classe evaluateur
nom 134959136
fils (134959256)
expression cos $0 * 4

classe evaluateur
nom 134959256
fils (134958848)
expression 6 * $0

classe flottant
nom 134959376
valeur 4.08329

classe evaluateur
nom 135578808
fils (134958848)
expression $0 + 1

classe applique
nom 134959552
fils (134959936 134960048 134960480 134940656 134961464 134962672 134962968)

classe type_affichage
nom 134959936
affichage invisible

classe lampe
nom 134960048
observateur 1 1 2
	0 0 -1
	0 0 0
	0 1

classe type_affichage
nom 134960480

classe applique
nom 134940656
fils (134960680 134960992 134961288)
ferme 1
commentaire sphere blanche

classe rotation
nom 134960680
fils (134958848)
cache_tag 1
cache_tag_symetrie 1

classe translation
nom 134960992
t_origine 3.9552 1.08759 1.17015
cache_tag 1
cache_tag_symetrie 1

classe sphere
nom 134961288

classe applique
nom 134961464
fils (134961640 134962080 134962376 134962496)
commentaire sphere jaune

classe rotation
nom 134961640
fils (134961952)
cache_tag 1
cache_tag_symetrie 1

classe evaluateur
nom 134961952
fils (134958848)
expression $0 * 4 - 10

classe translation
nom 134962080
t_origine 2 0 0
cache_tag 1
cache_tag_symetrie 1

classe couleur
nom 134962376
b 0

classe sphere
nom 134962496

classe translation
nom 134962672
t_origine 0 0 -2
cache_tag 1
cache_tag_symetrie 1

classe applique
nom 134962968
fils (134963104 134964592)
ferme 1
commentaire 8 spheres

classe applique
nom 134963104
fils (134963240 134964160)

classe applique
nom 134963240
fils (134963376 134963984)

classe applique
nom 134963376
fils (134963512 134963808)

classe translation
nom 134963512
t_origine 2 0 0
cache_tag 1
cache_tag_symetrie 1

classe sphere
nom 134963808

classe sphere
nom 134963984

classe applique
nom 134964160
fils (134964296 134963240)

classe translation
nom 134964296
t_origine 0 2 0
cache_tag 1
cache_tag_symetrie 1

classe applique
nom 134964592
fils (134964728 134963104)

classe translation
nom 134964728
t_origine 0 0 2
cache_tag 1
cache_tag_symetrie 1
