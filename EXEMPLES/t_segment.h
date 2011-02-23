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
#ifndef T_SEGMENT_H
#define T_SEGMENT_H

#include "t_geometrie.h"

PROTOTYPE_TYPE_LOGIQUE(L_bipoint2) /* Création de prototypes nécessaires */
#define L_bipoint2 TL(L_bipoint2)  /* Pour simplifier les définitions */

typedef struct
{
  Triplet t1, t2 ;
} Bitriplet2 ;			  /* Le stockage Bi-Triplet  */

PROTOTYPE_TYPE_PHYSIQUE(codec_bitriplet2, Bitriplet2) /* Prototypes */
#define P_bitriplet2 TP(codec_bitriplet2) /* Pour simplifier les définitions */

#endif
