
classe interpolateur_quadruplet
nom 136995576
fils (137633528 143723928 137122080 143723416)
chemin boucle

classe flottant
nom 137633528
valeur 0

classe table_nc_flottant
nom 143723928
t (0.0152544 0.0145148 0.0138243)

classe evaluateur
nom 137122080
fils (143723208 137143096)
expression $0 * $1 / 100

classe entier
nom 143723208
valeur 20

classe variable
nom 137143096
fils (143723296)
nom_variable secondes

classe seconde
nom 143723296
temps 0 0 100
increment 4

classe spline2
nom 143723416
fils (143723536 143723208)
fermee 1

classe table_nc_point3
nom 143723536
t (-10 -10 2
	10 -10 2
	10 10 2
	-10 10 2)

classe prog_if
nom 137122360
fils (136856040 143723536)

classe booleen
nom 136856040
valeur 1
