
classe vue
nom 1084022848
edite 161604560
titre Vue Perspective
pas_vue 0.1
angle_vue 0.1
point_de_vue 4.17715 12.3863 12.0113
point_vise 3.68083 4.00629 0.39973
haut 0.831275 -0.466993 0.301495
geometrie 447 23 699 565

classe lampe
nom 158178952
observateur 1 1 2
	0 0 -1
	0 0 0
	0 1

classe applique
nom 158179168
fils (158179288 158180176)

classe translation
nom 158179288
fils (158179600)
cache_tag 1
cache_tag_symetrie 1

classe vecteur3
nom 158179600
fils (158179736 158180016)
z 0

classe evaluateur
nom 158179736
fils (158179896)
expression $0:1 / 10000 + 4

classe joystick
nom 158179896

classe evaluateur
nom 158180016
fils (158179896)
expression $0:0 / 10000 + 4

classe sphere
nom 158180176

classe applique
nom 158180352
fils (158180504 158180656 158180952)

classe couleur
nom 158180504
fils (158179736 158180016)

classe translation
nom 158180656
t_origine -1 4 0
cache_tag 1
cache_tag_symetrie 1

classe tore
nom 158180952
rayon 0.5

classe vue
nom 1084211264
edite 158583608
fils (158181376)
titre vuejoystick
pas_vue 0.1
angle_vue 0.1
point_de_vue -5.97568 -8.77934 16.7284
point_vise -3.13468 -4.63044 8.7864
haut 0.950174 -0.0685292 0.304094
geometrie 1138 248 400 400

classe observateur_repere
nom 158181376
fils (158181656)

classe joy_transfo
nom 158181656
fils (158179896)
t [(2.841 4.1489 -7.942)
	(-0.717397 8.41021 4.13687)
	(0.950174 -0.0685292 0.304094)
	(-5.97568 -8.77934 16.7284)]
quoi gAGDth

classe applique
nom 164645080
fils (164567696 164779864)

classe homothetie
nom 164567696
angle 0.4
cache_tag 1
cache_tag_symetrie 1

classe cube
nom 164779864
fils (158181656)
