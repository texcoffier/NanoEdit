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
/*
 * Créé le Tue Jun  1 11:50:47 1999 par Thierry EXCOFFIER
 * modifs le mar mar 14 11:26:08 CET 2000 par Eric GUERIN
 */

#ifndef T_ERIC_H
#define T_ERIC_H

#include "util.h"
#include "point.h"
#include "matrice.h"
#include "controle.h"
#include "ifs.h"
#include "gifs.h"
#include "graphisme.h"
#include "figure.h"
#include "pov.h"
#include "affgl.h"
#include "image.h"


#define      PREMIERE_COORDONNEE 1
#define      DERNIERE_COORDONNEE(X) ((X)->espace.dim)
#define      HOMOGENE(X) (X)->c[0]


PROTOTYPE_TYPE_PHYSIQUE(codec_coordonnees     , VECH         )
/* ajout  */   
PROTOTYPE_TYPE_PHYSIQUE(codec_controle        , L_VECH       )
/* /ajout  */   
PROTOTYPE_TYPE_PHYSIQUE(codec_math            , MATH         )
PROTOTYPE_TYPE_PHYSIQUE(codec_enum_type_espace, t_type_espace)
PROTOTYPE_TYPE_PHYSIQUE(codec_semig           , SEMIG        )
PROTOTYPE_TYPE_PHYSIQUE(codec_ifs             , L_MATH       )
PROTOTYPE_TYPE_PHYSIQUE(codec_gifs            , GIFS         )
PROTOTYPE_TYPE_PHYSIQUE(codec_iter            , ITER         )

#define P_coord            TP(codec_coordonnees     )
/* ajout */   
#define P_controle         TP(codec_controle        )
/* /ajout */   
#define P_math             TP(codec_math            )
#define P_enum_type_espace TP(codec_enum_type_espace)
#define P_semig            TP(codec_semig)
#define P_ifs              TP(codec_ifs)
#define P_gifs             TP(codec_gifs)
#define P_iter             TP(codec_iter)

PROTOTYPE_TYPE_LOGIQUE(L_coord        )
/* ajout */   
PROTOTYPE_TYPE_LOGIQUE(L_controle     )
/* /ajout */   
PROTOTYPE_TYPE_LOGIQUE(L_coord_point  )
PROTOTYPE_TYPE_LOGIQUE(L_coord_vecteur)
PROTOTYPE_TYPE_LOGIQUE(L_math         )
PROTOTYPE_TYPE_LOGIQUE(L_type_espace  )
PROTOTYPE_TYPE_LOGIQUE(L_semig        )
PROTOTYPE_TYPE_LOGIQUE(L_ifs          )
PROTOTYPE_TYPE_LOGIQUE(L_gifs         )
PROTOTYPE_TYPE_LOGIQUE(L_iter         )

#define L_coord         TL(L_coord        )
/* ajout */   
#define L_controle      TL(L_controle     )
/* /ajout */   
#define L_coord_point   TL(L_coord_point  )
#define L_coord_vecteur TL(L_coord_vecteur)
#define L_math          TL(L_math         )
#define L_type_espace   TL(L_type_espace  )
#define L_semig         TL(L_semig        )
#define L_ifs           TL(L_ifs          )
#define L_gifs          TL(L_gifs         )
#define L_iter          TL(L_iter         )

#endif
