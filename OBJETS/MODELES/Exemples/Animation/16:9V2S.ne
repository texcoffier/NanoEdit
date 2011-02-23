
classe vue
nom -1220562880
edite 135838152
fils (138923832 137046680)
titre Vue Perspective
pas_vue 0.1
angle_vue 0.1
point_de_vue 0 -10 2
point_vise 1.27743 -9.95048 2
geometrie 7 34 1268 786

classe operateur
nom 138923832
fils (136931048 136979504 136979824 136931696 136931784)
oper Exemples/Animation/obsV2

classe seconde
nom 136931048
edite 136976240
temps 0 0 100
increment 0.04

classe operateur
nom 136979504
fils (136931048)
oper Exemples/Animation/trajectoire_spline

classe operateur
nom 136979824
fils (136931592)
oper Exemples/Animation/trajectoire_spline

classe evaluateur
nom 136931592
fils (136931048)
expression $0 + 0.1

classe point3
nom 136931696
x 0
y 0
z 1

classe flottant
nom 136931784
valeur 0

classe operateur
nom 137046680
fils (136931048)
oper Exemples/Hemisphere/paysage

classe vue
nom -1219379136
edite 135757552
fils (152286424 152286576 152286648 152286720 152287600 137046680)
titre Vue Z
pas_vue 0.1
angle_vue 0.1
taille_vue 11.9
point_de_vue 0 0 19
haut 0 1 0
b_persp 0:Ortho
menu_qualite 4: Mauvaise
geometrie 668 473 560 539

classe observateur
nom 152286424
observateur_point_de_vue 0 0 19
observateur_haut 0 1 0
observateur_focale 11.9
observateur_ratio 1.19914

classe ligne_brisee
nom 152286576
fils (136979504)

classe ligne_brisee
nom 152286648
fils (136979824)

classe applique
nom 152286720
fils (152286792 152287160 152287440)
ferme 1
commentaire OEIL

classe translation
nom 152286792
fils (152287072)
cache_tag 1
cache_tag_symetrie 1

classe vecteur_triplet
nom 152287072
fils (138923832)

classe homothetie
nom 152287160
angle 0.4
cache_tag 1
cache_tag_symetrie 1

classe sphere
nom 152287440

classe applique
nom 152287600
fils (152287672 152288040 152288320)
ferme 1
commentaire REGARD

classe translation
nom 152287672
fils (152287952)
cache_tag 1
cache_tag_symetrie 1

classe vecteur_triplet
nom 152287952
fils (136979824)

classe homothetie
nom 152288040
angle 0.2
cache_tag 1
cache_tag_symetrie 1

classe sphere
nom 152288320
