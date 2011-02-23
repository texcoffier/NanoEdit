
classe vue
nom -1220231104
edite 135297248
fils (135824496)
titre Vue Hémisphérique
pas_vue 0.1
angle_vue 0.1
point_de_vue -4.6 0 0
point_vise -4.6 0 1
haut -1.25 0 0
b_persp 2:Hémis
geometrie 178 4 909 1003

classe observateur
nom 135824496
fils (135811352 135826400 135826792)
observateur_ratio 0.976369

classe point_triplet
nom 135811352
fils (135824648)
ferme 1

classe interpolateur_quadruplet
nom 135824648
fils (135823384 135825016 135825416 134941520)
chemin boucle

classe flottant
nom 135823384
valeur 0

classe table_flottant
nom 135825016
fils (135825152 135825240 135825328)

classe flottant
nom 135825152
valeur 4

classe flottant
nom 135825240
valeur 4

classe flottant
nom 135825328
valeur 4

classe evaluateur
nom 135825416
fils (135825768)
expression $0

classe seconde
nom 135825768
edite 135854528
temps 0.32 0 10
increment 0.04
onoff 1

classe table_point4
nom 134941520
fils (135825984 135826088 135826192 135826296)

classe point3
nom 135825984
x -5
y 0
z 0

classe point3
nom 135826088
x 0
y 0
z 0

classe point3
nom 135826192
x 0
y 5
z 0

classe point3
nom 135826296
x 5
y 10
z 0

classe point_triplet
nom 135826400
fils (135826520)
ferme 1

classe evaluateur
nom 135826520
fils (135811352 135826688)
expression $0 + $1

classe point3
nom 135826688
x 0
y 0
z 1

classe point_triplet
nom 135826792
fils (135826912)

classe evaluateur
nom 135826912
fils (135827080 135811352)
expression $1 - $0

classe point_triplet
nom 135827080
fils (135827200)
ferme 1

classe interpolateur_quadruplet
nom 135827200
fils (135827648 135825016 135827736 134941520)
chemin boucle

classe flottant
nom 135827648
valeur 0

classe evaluateur
nom 135827736
fils (135825768)
expression $0 + 1

classe operateur
nom 135827888
fils (135825768)
oper Exemples/Hemisphere/paysage
