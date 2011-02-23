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
#include "t_segment.h"

DEFINITION_TYPE_LOGIQUE(L_bipoint2, /* Le nom du type logique */
			L_valeur, /* La surclasse du type logique */
			"Bipoint", /* Aide sur le type logique */
			/* On indique la hauteur du Widget (Facultatif) */
			c->te_hauteur = 40 ; /* car 2 lignes de texte */
			)
TYPE_PHYSIQUE(codec_bitriplet2,	/* Le nom du type physique */
	      Bitriplet2,	/* La structure C associée au type physique */
	      /* Ce qui suit est une fonction C */
	      {
		codec_triplet((t_l3_double*)&a->t1) ; /* Contient P */
		codec_triplet((t_l3_double*)&a->t2) ; /* et Q */
	      }
	      )
