
classe observateur
nom 137732600
fils (137745928 137746448 137746912)
observateur_ratio 1.77778

classe point3
nom 137745928
fils (137746016 137746344)
z 2

classe evaluateur
nom 137746016
fils (137746120)
expression sin $0 * 4

classe evaluateur
nom 137746120
fils (137732488)
expression $0

classe variable
nom 137732488
fils (137746224)
nom_variable secondes

classe seconde
nom 137746224
temps 0 0 100
increment 0.04

classe evaluateur
nom 137746344
fils (137746120)
expression cos $0 * 4

classe point3
nom 137746448
fils (137746536 137746760)
z 2

classe evaluateur
nom 137746536
fils (137746640)
expression sin $0 * 4

classe evaluateur
nom 137746640
fils (137732488)
expression $0 + 0.1

classe evaluateur
nom 137746760
fils (137746640)
expression cos $0 * 4

classe point3
nom 137746912
x 0
y 0
z 1
