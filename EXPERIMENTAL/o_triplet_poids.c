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
#include "o_objet.h"
#include "t_geometrie.h"

typedef struct triplet_poid
{
  int lar ;
  Triplet *triplet ;
  Flottant *poids ;
} t_triplet_poids ;


PROTOTYPE_TYPE_PHYSIQUE(codec_triplet_poids , t_triplet_poids)

TYPE_PHYSIQUE(codec_triplet_poids, t_triplet_poids,
	      codec_L_hh((void**)&a->triplet, codec_l3_double_nolf,
			 (void**)&a->poids, codec_double_lf,
			 &a->lar) ;
	      )

#define P_triplet_poids TP(codec_triplet_poids)

struct toto
{
  t_triplet_poids tp ;
} ;

CLASSE(toto, struct toto,
       CHAMP(tp, L_table_point4 P_triplet_poids Edite Extrait Sauve)
)


