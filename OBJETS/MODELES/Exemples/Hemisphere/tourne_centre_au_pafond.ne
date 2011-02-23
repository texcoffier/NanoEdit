
classe vue
nom -1222954944
edite 135278008
fils (477739448)
titre Vue Hémisphérique
pas_vue 0.1
angle_vue 0.1
point_de_vue 0.923558 1.77399 0.3
haut -0.0685005 -0.131577 0.988936
b_persp 2:Hémis
geometrie 139 19 911 1007

classe observateur
nom 477739448
fils (478004096 478004304 478350760)
observateur_ratio 0.974332

classe point3
nom 478004096
fils (474223416 403715256)
z 0.3

classe evaluateur
nom 474223416
fils (405645040)
expression sin $0 * 2

classe evaluateur
nom 405645040
fils (478004184)
expression $0

classe seconde
nom 478004184
edite 134960440
temps 0.48 0 100
increment 0.04
onoff 1

classe evaluateur
nom 403715256
fils (405645040)
expression cos $0 * 2

classe point3
nom 478004304
x 0
y 0
z 0

classe point3
nom 478350760
fils (474223416 403715256)
z 0.988936

classe operateur
nom 478316224
fils (478004184)
oper Exemples/Hemisphere/paysage
