
classe vue
nom -1222512576
edite 135278008
fils (471524360)
titre Vue Hémisphérique
pas_vue 0.1
angle_vue 0.1
point_de_vue 1.84212 0.778837 0.3
point_vise 1.84212 0.778837 1
haut 1.84212 0.778837 0
b_persp 2:Hémis
geometrie 139 19 910 1000

classe observateur
nom 471524360
fils (311920536 470745416 470745504)
observateur_ratio 0.980603

classe point3
nom 311920536
fils (312068096 312097808)
z 0.3

classe evaluateur
nom 312068096
fils (405219480)
expression cos $0 * 2

classe evaluateur
nom 405219480
fils (311920624)
expression $0

classe seconde
nom 311920624
edite 134960440
temps 0.4 0 100
increment 0.04
onoff 1

classe evaluateur
nom 312097808
fils (405219480)
expression sin $0 * 2

classe point3
nom 470745416
fils (312068096 312097808 400448744)

classe flottant
nom 400448744
valeur 1

classe point3
nom 470745504
fils (312068096 312097808)
z 0

classe operateur
nom 471484448
fils (311920624)
oper Exemples/Hemisphere/paysage
