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
 * Créé le Wed Jul 29 15:22:22 1998 par Thierry EXCOFFIER
 */

#ifndef T_GL_H
#define T_GL_H

#include "o_objet.h"
#include "t_base.h"
#include "u_base.h"

/*
 * GL générique
 */

typedef struct gl
{
  NanoWidget gl ;		/* Le Widget GL d'affichage graphique */
  int lar, hau ;
} Gl ;



long Gl_start(Gl *gl) ;
void Gl_stop(Gl *gl) ;
void Gl_stop_sans_swapping(Gl *gl) ;

PROTOTYPE_TYPE_PHYSIQUE(codec_gl, Gl)
PROTOTYPE_TYPE_LOGIQUE(L_gl)
PROTOTYPE_TYPE_EDITEUR(E_gl)

#define E_gl  TE(E_gl)
#define L_gl  TL(L_gl)
#define P_gl  TP(P_gl)

#endif
