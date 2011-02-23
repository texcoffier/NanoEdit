
classe vue
nom 991338560
edite 135859016
fils (136240752 141293792)
titre Vue Perspective
pas_vue 0.1
angle_vue 0.1
zmin 1
zmax 100
point_de_vue -1.13825 9.96176 2
point_vise -2.44531 9.8221 2
haut -0.00137068 0.012828 0.999917
geometrie 585 343 398 283

classe operateur
nom 136240752
fils (141225536 137246344 137240416 141140800 141140888)
oper Exemples/Animation/obsV2

classe seconde
nom 141225536
edite 136314896
temps 63.04 0 100
increment 0.04

classe operateur
nom 137246344
fils (141225536)
oper Exemples/Animation/trajectoire_spline

classe operateur
nom 137240416
fils (141319456)
oper Exemples/Animation/trajectoire_spline

classe evaluateur
nom 141319456
fils (141225536)
expression $0 + 0.1

classe point3
nom 141140800
x 0.00714002
y 0.0137369
z 0.99988

classe flottant
nom 141140888
valeur 0

classe applique
nom 141293792
fils (137826368 137250480 140966608 137647472 141401352)

classe lampe
nom 137826368
observateur 1 1 2
	0 0 -1
	0 0 0
	0 1

classe translation
nom 137250480
t_origine 0.0326975 0.0980926 2.06812
cache_tag 1
cache_tag_symetrie 1

classe type_affichage
nom 140966608
affichage filaire

classe homothetie
nom 137647472
angle 10
cache_tag 1
cache_tag_symetrie 1

classe sphere
nom 141401352
