
classe vue
nom -1220427712
edite 135327784
titre Vue Z
pas_vue 0.1
angle_vue 0.1
taille_vue 1.5
point_de_vue 0.492472 1.25351 4
point_vise 0.492472 1.25351 0
haut 0 1 0
b_persp 0
geometrie 407 169 680 851

classe fractale
nom 143567600
edite 135851952
fils (143567760 143567920 143568200 143568496)
niveau 4
affiche_transformations 1

classe sphere
nom 143567760

classe repere
nom 143567920
t_v1 0.5 0 0
t_v2 0 0.5 0
t_v3 0 0 0.5

classe repere
nom 143568200
t_origine 0.550654 -1.4429e-07 0
t_v1 0.5 0 0
t_v2 0 0.5 0
t_v3 0 0 0.5

classe repere
nom 143568496
fils (143568856 143568960 143569064 143569560)

classe point3
nom 143568856
x 0.30762
y 0.586497
z 0

classe vecteur3
nom 143568960
x 0.5
y 0
z 0

classe vecteur3
nom 143569064
fils (143569200 143569456)
z 0.401675

classe evaluateur
nom 143569200
fils (143569320)
expression cos $0 * 0.3

classe seconde
nom 143569320
edite 135822120
temps 4.24 0 100
increment 0.04
onoff 1

classe evaluateur
nom 143569456
fils (143569320)
expression 0.3 * sin $0 + 0.6

classe vecteur3
nom 143569560
x 0
y 0
z 0.5

classe lampe
nom 143569664
observateur 0.188236 0.682774 4
	0 0 -1
	0 0 0
	0 1
