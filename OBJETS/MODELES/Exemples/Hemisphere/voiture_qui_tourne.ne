
classe vue
nom -1223397312
edite 135278008
fils (479858976)
titre Vue Hémisphérique
pas_vue 0.1
angle_vue 0.1
point_de_vue 0.704548 1.87179 0.3
point_vise 0.887896 1.7921 0.2999
b_persp 2:Hémis
geometrie 139 19 909 1003

classe observateur
nom 479858976
fils (479331912 479332120 479952112)
observateur_ratio 0.976369

classe point3
nom 479331912
fils (399907872 402627104)
ferme 1
z 0.3

classe evaluateur
nom 399907872
fils (473575960)
expression sin $0 * 2

classe evaluateur
nom 473575960
fils (479332000)
expression $0

classe seconde
nom 479332000
edite 134960440
temps 0.36 0 100
increment 0.04
onoff 1

classe evaluateur
nom 402627104
fils (473575960)
expression cos $0 * 2

classe point3
nom 479332120
fils (403456696 147913136)
ferme 1
z 0.2999

classe evaluateur
nom 403456696
fils (312015368)
expression sin $0 * 2

classe evaluateur
nom 312015368
fils (479332000)
expression $0 + 0.1

classe evaluateur
nom 147913136
fils (312015368)
expression cos $0 * 2

classe point3
nom 479952112
x 0
y 0
z 1

classe operateur
nom 479921744
fils (479332000)
oper Exemples/Hemisphere/paysage
