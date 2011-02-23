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
#ifndef T_GEOMETRIE_H
#define T_GEOMETRIE_H

/*
 * Les structures suivantes sont conservées pour assurer
 * la compatibilités. (Sauf pour l'ordre ligne/colonne)
 * Elle ne doivent plus être utilisé.
 * Pour voir les structures possibles, cherchez dans
 * UTILITAIRES/codec.h
 */

typedef struct{int nb ;	void             *table ; } Table ;
typedef struct{int nb ;	Flottant         *table ; } Table_flottant ;
typedef struct{int nb ;	int              *table ; } Table_entier ;
typedef struct{int nb ;	Triplet          *table ; } Table_triplet ;
typedef struct{int nb ;	Quadruplet       *table ; } Table_quadruplet ;
typedef struct{int nb ;	Table            *table ; } Table_table ;
typedef struct{int nb ;	Table_triplet    *table ; } Table_table_triplet ;
typedef struct{int nb ;	Table_quadruplet *table ; } Table_table_quadruplet ;
typedef struct{int nb ; Transfo          *table ; } Table_transfo ;

typedef struct
{
  Triplet t1, t2 ;
} Bitriplet ;
typedef struct
{
  int nb_lignes ;		/* Nombre de ligne */
  int nb_colonnes ;		/* Nombre d'element par ligne */
  void **grille ;	        /* grille[nb_lignes-1][nb_colonnes-1] */
} Grille ;
typedef struct
{
  int nb_lignes ;		/* Nombre de ligne */
  int nb_colonnes ;		/* Nombre d'element par ligne */
  Triplet **grille ;	        /* grille[nb_lignes-1][nb_colonnes-1] */
} Grille_triplet ;
typedef struct
{
  int nb_lignes ;		/* Nombre de ligne */
  int nb_colonnes ;		/* Nombre d'element par ligne */
  Quadruplet **grille ;	        /* grille[nb_lignes-1][nb_colonnes-1] */
} Grille_quadruplet ;
typedef struct
{
  int nb_lignes ;		/* Nombre de ligne */
  int nb_colonnes ;		/* Nombre d'element par ligne */
  Flottant **grille ;	        /* grille[nb_lignes-1][nb_colonnes-1] */
} Grille_flottant ;
typedef struct nuage
{
  Table_triplet t ;
  char *nom_fichier ;			/* Fichier contenant le nuage */
  Liste *sous_actifs ;
  t_boite_min_max englobant ;
  t_observateur observateur ;	/* Un observateur pour le nuage */
  FILE *a_lire ;		/* Pour la lecture en arriere plan */
  void (*background_process)(struct nuage *o) ;
  Booleen repositionne_observateur ;
} Nuage ;

/*
 *
 */
PROTOTYPE_TYPE_PHYSIQUE(codec_bitriplet, Bitriplet)
PROTOTYPE_TYPE_PHYSIQUE(codec_observateur, t_observateur)
/*
 *
 */
#define P_flottant               TP(codec_flottant              )
#define P_triplet                TP(codec_triplet               )
#define P_triplet_int            TP(codec_l3_int                )
#define P_quadruplet             TP(codec_quadruplet            )
#define P_quadruplet_int         TP(codec_l4_int                )
#define P_quadruplet1            TP(codec_quadruplet1           )
#define P_6_quadruplets          TP(codec_6_quadruplet          )
#define P_transfo                TP(codec_transfo               )
#define P_observateur            TP(codec_observateur           )
#define P_boite_min_max          TP(codec_boite_min_max         )
#define P_table_flottant         TP(codec_table_flottant        )
#define P_table_int              TP(codec_L_int                 )
#define P_table_paire_float      TP(codec_L_l2_float            )
#define P_table_paire_double     TP(codec_L_l2_double           )
#define P_table_triplet          TP(codec_table_triplet         )
#define P_table_quadruplet       TP(codec_table_quadruplet      )
#define P_table_quadruplet_int   TP(codec_L_l4_int              )
#define P_table_quadruplet1      TP(codec_table_quadruplet1     )
#define P_table_transfo          TP(codec_table_transfo         )
#define P_table_Table_triplet    TP(codec_table_table_triplet   )
#define P_table_Table_quadruplet TP(codec_table_table_quadruplet)
#define P_grille_quadruplet      TP(codec_grille_quadruplet     )
#define P_grille_triplet         TP(codec_grille_triplet        )
#define P_grille_flottant        TP(codec_grille_flottant       )
#define P_bitriplet              TP(codec_bitriplet             )
#define P_paire_float            TP(codec_l2_float              )
#define P_paire_double           TP(codec_l2_double             )
#define P_tenseur_l3_float       TP(codec_P_H_L_l3_float        )
#define P_tenseur_l3_double      TP(codec_P_H_L_l3_double       )
#define P_tenseur_l4_float1      TP(codec_P_H_L_l4_float1       )
#define P_tenseur_l4_double1     TP(codec_P_H_L_l4_double1      )

PROTOTYPE_TYPE_LOGIQUE(L_table                 )
PROTOTYPE_TYPE_LOGIQUE(L_table_nombre          )
PROTOTYPE_TYPE_LOGIQUE(L_table_flottant        )
PROTOTYPE_TYPE_LOGIQUE(L_table_int             )
PROTOTYPE_TYPE_LOGIQUE(L_table_triplet         )
PROTOTYPE_TYPE_LOGIQUE(L_table_quadruplet      )
PROTOTYPE_TYPE_LOGIQUE(L_table_point           )
PROTOTYPE_TYPE_LOGIQUE(L_table_point3          )
PROTOTYPE_TYPE_LOGIQUE(L_table_point4          )
PROTOTYPE_TYPE_LOGIQUE(L_table_transfo         )
PROTOTYPE_TYPE_LOGIQUE(L_table_table           )
PROTOTYPE_TYPE_LOGIQUE(L_table_table_triplet   )
PROTOTYPE_TYPE_LOGIQUE(L_table_table_point     )
PROTOTYPE_TYPE_LOGIQUE(L_table_table_point3    )
PROTOTYPE_TYPE_LOGIQUE(L_table_table_point4    )
PROTOTYPE_TYPE_LOGIQUE(L_table_table_quadruplet)
PROTOTYPE_TYPE_LOGIQUE(L_grille                )
PROTOTYPE_TYPE_LOGIQUE(L_grille_quadruplet     )
PROTOTYPE_TYPE_LOGIQUE(L_grille_triplet        )
PROTOTYPE_TYPE_LOGIQUE(L_grille_nombre         )
PROTOTYPE_TYPE_LOGIQUE(L_grille_flottant       )
PROTOTYPE_TYPE_LOGIQUE(L_grille_point          )
PROTOTYPE_TYPE_LOGIQUE(L_grille_point3         )
PROTOTYPE_TYPE_LOGIQUE(L_grille_point4         )
PROTOTYPE_TYPE_LOGIQUE(L_tenseur               )
PROTOTYPE_TYPE_LOGIQUE(L_tenseur_point         )
PROTOTYPE_TYPE_LOGIQUE(L_vecteur               )
PROTOTYPE_TYPE_LOGIQUE(L_vecteur3              )
PROTOTYPE_TYPE_LOGIQUE(L_vecteur4              )
PROTOTYPE_TYPE_LOGIQUE(L_point                 )
PROTOTYPE_TYPE_LOGIQUE(L_point3                )
PROTOTYPE_TYPE_LOGIQUE(L_point4                )
PROTOTYPE_TYPE_LOGIQUE(L_triplet               )
PROTOTYPE_TYPE_LOGIQUE(L_quadruplet            )
PROTOTYPE_TYPE_LOGIQUE(L_cubeg                 )
PROTOTYPE_TYPE_LOGIQUE(L_transfo               )
PROTOTYPE_TYPE_LOGIQUE(L_couleur               )
PROTOTYPE_TYPE_LOGIQUE(L_couleur3              )
PROTOTYPE_TYPE_LOGIQUE(L_observateur           )
PROTOTYPE_TYPE_LOGIQUE(L_boite_min_max         )
PROTOTYPE_TYPE_LOGIQUE(L_bipoint               )
PROTOTYPE_TYPE_LOGIQUE(L_vecteur_lie           )


#define L_table                  TL(L_table                 )
#define L_table_nombre           TL(L_table_nombre          )
#define L_table_int              TL(L_table_int             )
#define L_table_flottant         TL(L_table_flottant        )
#define L_table_triplet          TL(L_table_triplet         )
#define L_table_quadruplet       TL(L_table_quadruplet      )
#define L_table_transfo          TL(L_table_transfo         )
#define L_table_point            TL(L_table_point           )
#define L_table_point3           TL(L_table_point3          )
#define L_table_point4           TL(L_table_point4          )
#define L_table_table            TL(L_table_table           )
#define L_table_table_triplet    TL(L_table_table_triplet   )
#define L_table_table_point      TL(L_table_table_point     )
#define L_table_table_point3     TL(L_table_table_point3    )
#define L_table_table_point4     TL(L_table_table_point4    )
#define L_table_table_quadruplet TL(L_table_table_quadruplet)
#define L_grille                 TL(L_grille                )
#define L_grille_quadruplet      TL(L_grille_quadruplet     )
#define L_grille_triplet         TL(L_grille_triplet        )
#define L_grille_nombre          TL(L_grille_nombre         )
#define L_grille_flottant        TL(L_grille_flottant       )
#define L_grille_point           TL(L_grille_point          )
#define L_grille_point3          TL(L_grille_point3         )
#define L_grille_point4          TL(L_grille_point4         )
#define L_tenseur                TL(L_tenseur               )
#define L_tenseur_point          TL(L_tenseur_point         )
#define L_vecteur                TL(L_vecteur               )
#define L_vecteur3               TL(L_vecteur3              )
#define L_vecteur4               TL(L_vecteur4              )
#define L_point                  TL(L_point                 )
#define L_point3                 TL(L_point3                )
#define L_point4                 TL(L_point4                )
#define L_triplet                TL(L_triplet               )
#define L_quadruplet             TL(L_quadruplet            )
#define L_cubeg                  TL(L_cubeg                 )
#define L_transfo                TL(L_transfo               )
#define L_couleur                TL(L_couleur               )
#define L_couleur3               TL(L_couleur3              )
#define L_observateur            TL(L_observateur           )
#define L_boite_min_max          TL(L_boite_min_max         )
#define L_bipoint                TL(L_bipoint               )
#define L_vecteur_lie            TL(L_vecteur_lie           )

#endif

