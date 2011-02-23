/*
    NanoÉdit: Un modeleur algébrique interactif.
    Copyright (C) 1996-2004 Thierry EXCOFFIER, Université Claude Bernard, LIRIS

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    Contact: Thierry.EXCOFFIER@liris.univ-lyon1.fr
*/
/* LIB: -lm */

#include "o_objet.h"
#include "t_geometrie.h"

static Booleen quadruplet(const void *o, const Champ *c)
{
  return( c->tp == &infos_codec_quadruplet ) ;
}
static Booleen triplet(const void *o, const Champ *c)
{
  return( c->tp == &infos_codec_triplet || quadruplet(o,c) ) ;
}
static Booleen transfo(const void *o, const Champ *c)
{
  return( c->tp == &infos_codec_transfo ) ;
}
static Booleen table_flottant(const void *o, const Champ *c)
{
  return( c->tp == &infos_codec_table_flottant ) ;
}
static Booleen table_int(const void *o, const Champ *c)
{
  return( c->tp == &infos_codec_L_int ) ;
}
static Booleen table_triplet(const void *o, const Champ *c)
{
  return( c->tp == &infos_codec_table_triplet ) ;
}
static Booleen table_quadruplet(const void *o, const Champ *c)
{
  return( c->tp == &infos_codec_table_quadruplet
	  || c->tp == &infos_codec_L_l4_int) ;
}
static Booleen table_table_triplet(const void *o, const Champ *c)
{
  return( c->tp == &infos_codec_table_table_triplet ) ;
}
static Booleen table_table_quadruplet(const void *o, const Champ *c)
{
  return( c->tp == &infos_codec_table_table_quadruplet ) ;
}
static Booleen grille_triplet(const void *o, const Champ *c)
{
  return( c->tp == &infos_codec_grille_triplet ) ;
}
static Booleen grille_flottant(const void *o, const Champ *c)
{
  return( c->tp == &infos_codec_grille_flottant ) ;
}
static Booleen grille_quadruplet(const void *o, const Champ *c)
{
  return( c->tp == &infos_codec_grille_quadruplet ) ;
}

TYPE_PHYSIQUE(codec_bitriplet,
	      Bitriplet,
	      {
		codec_triplet((t_l3_double*)&a->t1) ;
		codec_triplet((t_l3_double*)&a->t2) ;
	      }
	      )

TYPE_PHYSIQUE(codec_observateur,
	      t_observateur,
	      {
		codec_triplet((t_l3_double*)&a->point_de_vue) ;
		codec_triplet((t_l3_double*)&a->point_vise) ;
		codec_triplet((t_l3_double*)&a->haut) ;
		codec_flottant(&a->focale) ;
		codec_flottant1(&a->ratio) ;
	      }
	      )

DEFINITION_TYPE_LOGIQUE(L_transfo, L_grille_nombre,
			"Transformation affine",
			PREDICAT(transfo) ;
			c->tp = &infos_codec_transfo ;
			)
DEFINITION_TYPE_LOGIQUE(L_quadruplet, L_valeur,
			"Quadruplet de valeur",
			PREDICAT(quadruplet) ;
			c->tp = &infos_codec_quadruplet ;
			)
DEFINITION_TYPE_LOGIQUE(L_triplet, L_valeur,
			"Triplet de valeur",
			PREDICAT(triplet) ;
			c->tp = &infos_codec_triplet ;
			)
DEFINITION_TYPE_LOGIQUE(L_vecteur, L_valeur,
			"Vecteur",
			)
DEFINITION_TYPE_LOGIQUE(L_vecteur4, L_vecteur,
			"Vecteur à 4 composantes",
			c->tp = &infos_codec_quadruplet ;
	                )
DEFINITION_TYPE_LOGIQUE(L_vecteur3, L_vecteur,
			"Vecteur à 3 composantes",
			c->tp = &infos_codec_triplet ;
			)
DEFINITION_TYPE_LOGIQUE(L_point, L_valeur,
			"Point",
			)
DEFINITION_TYPE_LOGIQUE(L_cubeg, L_valeur,
			"Cube généralisé",
			)
DEFINITION_TYPE_LOGIQUE(L_point4, L_point,
			"Point à 4 composantes",
			c->tp = &infos_codec_quadruplet ;
			)
DEFINITION_TYPE_LOGIQUE(L_point3, L_point,
			"Point à 3 composantes",
			c->tp = &infos_codec_triplet ;
			)
DEFINITION_TYPE_LOGIQUE(L_couleur, L_valeur,
			"Couleur",
			)
DEFINITION_TYPE_LOGIQUE(L_couleur3, L_couleur,
			"Couleur à 3 composantes",
			c->tp = &infos_codec_triplet ;
			)

DEFINITION_TYPE_LOGIQUE(L_table, L_valeur,
			"Table",
			c->te_hauteur = 200 ;
			init_E_long_texte(c) ;
			)
DEFINITION_TYPE_LOGIQUE(L_table_nombre, L_table,
			"Table de nombre",
			c->te_hauteur = 0 ;
			init_E_texte(c) ;
			)
DEFINITION_TYPE_LOGIQUE(L_table_flottant, L_table_nombre,
			"Table de flottant",
			PREDICAT(table_flottant) ;
			c->tp = &infos_codec_table_flottant ;
			)
DEFINITION_TYPE_LOGIQUE(L_table_int, L_table_nombre,
			"Table de int",
			PREDICAT(table_int) ;
			c->tp = &infos_codec_L_int ;
			)
DEFINITION_TYPE_LOGIQUE(L_table_triplet, L_table,
			"Table de flottant",
			PREDICAT(table_triplet) ;
			c->tp = &infos_codec_table_triplet ;
			)
DEFINITION_TYPE_LOGIQUE(L_table_quadruplet, L_table,
			"Table de quadruplet",
			PREDICAT(table_quadruplet) ;
			c->tp = &infos_codec_table_quadruplet ;
			)
DEFINITION_TYPE_LOGIQUE(L_table_point, L_table,
			"Table de points",
			)
DEFINITION_TYPE_LOGIQUE(L_table_point3, L_table_point,
			"Table de points à 3 composantes",
			c->tp = &infos_codec_table_triplet ;
			)
DEFINITION_TYPE_LOGIQUE(L_table_point4, L_table_point,
			"Table de points à 4 composantes",
			c->tp = &infos_codec_table_quadruplet ;
			)
DEFINITION_TYPE_LOGIQUE(L_table_transfo, L_table,
			"Table de transformations",
			c->tp = &infos_codec_table_transfo ;
			)
DEFINITION_TYPE_LOGIQUE(L_table_table, L_table,
			"Table de table",
			)
DEFINITION_TYPE_LOGIQUE(L_table_table_triplet, L_table_table,
			"Table de table de triplet",
			PREDICAT(table_table_triplet) ;
			c->tp = &infos_codec_table_table_triplet ;	     
			)
DEFINITION_TYPE_LOGIQUE(L_table_table_quadruplet, L_table_table,
			"Table de table de points à 4 composantes",
			PREDICAT(table_table_quadruplet) ;
			c->tp = &infos_codec_table_table_quadruplet ;	     
			)
DEFINITION_TYPE_LOGIQUE(L_table_table_point, L_table_table,
			"Table de table de points à 4 composantes",
			)
DEFINITION_TYPE_LOGIQUE(L_table_table_point3, L_table_table_point,
			"Table de table de points à 3 composantes",
			c->tp = &infos_codec_table_table_triplet ;	     
			)
DEFINITION_TYPE_LOGIQUE(L_table_table_point4, L_table_table_point,
			"Table de table de points à 4 composantes",
			c->tp = &infos_codec_table_table_quadruplet ;	     
	     )
DEFINITION_TYPE_LOGIQUE(L_grille, L_valeur,
			"Grille",
			init_E_long_texte(c) ;
			c->te_hauteur = 200 ;
			)
DEFINITION_TYPE_LOGIQUE(L_grille_point, L_grille,
			"Grille de point",
			)
DEFINITION_TYPE_LOGIQUE(L_grille_point3, L_grille_point,
			"Grille de point à 3 composantes",
			c->tp = &infos_codec_grille_triplet ;	     
			)
DEFINITION_TYPE_LOGIQUE(L_grille_point4, L_grille_point,
			"Grille de point à 4 composantes",
			c->tp = &infos_codec_grille_quadruplet ;	     
			)
DEFINITION_TYPE_LOGIQUE(L_grille_nombre, L_grille,
			"Grille de nombre",
			)
DEFINITION_TYPE_LOGIQUE(L_grille_flottant, L_grille_nombre,
			"Grille de flottant",
			c->tp = &infos_codec_grille_flottant ;	     
			PREDICAT(grille_flottant) ;
			)
DEFINITION_TYPE_LOGIQUE(L_grille_triplet, L_grille,
			"Grille de triplet",
			c->tp = &infos_codec_grille_triplet ;	     
			PREDICAT(grille_triplet) ;
			)
DEFINITION_TYPE_LOGIQUE(L_grille_quadruplet, L_grille,
			"Grille de quadruplet",
			c->tp = &infos_codec_grille_quadruplet ;	     
			PREDICAT(grille_quadruplet) ;
			)
DEFINITION_TYPE_LOGIQUE(L_observateur, L_valeur,
			"Observateur (point de vue, point visé, haut, focale)",
			c->tp = &infos_codec_observateur ;	     
			)
DEFINITION_TYPE_LOGIQUE(L_boite_min_max, L_valeur,
			"Boite minmax de R3 parrallèle aux axes",
			c->tp = &infos_codec_boite_min_max ;	     
			c->te_hauteur = 40 ; /* 2 lignes */
			)
DEFINITION_TYPE_LOGIQUE(L_bipoint, L_valeur,
			"Paire de points",
			c->te_hauteur = 40 ; /* 2 lignes */
			)
DEFINITION_TYPE_LOGIQUE(L_vecteur_lie, L_valeur,
			"Vecteur lié défini par Point et Vecteur",
			c->te_hauteur = 40 ; /* 2 lignes */
			)

DEFINITION_TYPE_LOGIQUE(L_tenseur, L_valeur,
			"Tenseur",
			init_E_long_texte(c) ;
			c->te_hauteur = 200 ;
			)
DEFINITION_TYPE_LOGIQUE(L_tenseur_point, L_tenseur,
			"Tenseur de point",
			)
