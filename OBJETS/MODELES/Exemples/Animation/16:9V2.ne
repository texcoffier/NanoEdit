
classe vue
nom -1220358080
edite 135882656
fils (140515848 142171512)
titre Vue Perspective
pas_vue 0.1
angle_vue 0.1
point_de_vue 10 -1.8 2
point_vise 10 3.2 2
geometrie 7 34 1268 786

classe operateur
nom 140515848
fils (135557464 135557584 142166496 142171424 135542544)
oper Exemples/Animation/obsV2

classe seconde
nom 135557464
edite 135320064
temps 37.64 0 100
increment 0.04

classe operateur
nom 135557584
fils (135557464)
oper Exemples/Animation/trajectoire_carree

classe operateur
nom 142166496
fils (142169640)
oper Exemples/Animation/trajectoire_carree

classe evaluateur
nom 142169640
fils (135557464)
expression $0 + 1

classe point3
nom 142171424
x 0
y 0
z 1

classe flottant
nom 135542544
valeur 0

classe operateur
nom 142171512
fils (135557464)
oper Exemples/Hemisphere/paysage

classe vue
nom -1220767680
edite 134964112
fils (142188480 142188632 142188704 142188776 142189656 142171512)
titre Vue Z
pas_vue 0.1
angle_vue 0.1
taille_vue 16.8
point_de_vue 0 0 4
haut 0 1 0
b_persp 0:Ortho
menu_qualite 4: Mauvaise
geometrie 835 600 400 400

classe observateur
nom 142188480
observateur_point_de_vue 0 0 4
observateur_haut 0 1 0
observateur_focale 16.8
observateur_ratio 1.21951

classe ligne_brisee
nom 142188632
fils (135557584)

classe ligne_brisee
nom 142188704
fils (142166496)

classe applique
nom 142188776
fils (142188848 142189216 142189496)
ferme 1
commentaire OEIL

classe translation
nom 142188848
fils (142189128)
cache_tag 1
cache_tag_symetrie 1

classe vecteur_triplet
nom 142189128
fils (140515848)

classe homothetie
nom 142189216
homo 0.4
cache_tag 1
cache_tag_symetrie 1

classe sphere
nom 142189496

classe applique
nom 142189656
fils (142189728 142190096 142190376)
ferme 1
commentaire REGARD

classe translation
nom 142189728
fils (142190008)
cache_tag 1
cache_tag_symetrie 1

classe vecteur_triplet
nom 142190008
fils (142166496)

classe homothetie
nom 142190096
homo 0.2
cache_tag 1
cache_tag_symetrie 1

classe sphere
nom 142190376
