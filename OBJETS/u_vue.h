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
 * Créé le Wed Feb  2 16:38:13 2000 par Thierry EXCOFFIER
 */

#ifndef _HOME_EXCO_NE_1_7_OBJETS_U_VUE_H
#define _HOME_EXCO_NE_1_7_OBJETS_U_VUE_H

#include "GL/gl.h"

const Liste* fils_a_afficher          (const void *o   ) ;
void destruction_liste_objet          (void *o         ) ;
int alloue_plan_decoupage_gl          (GLdouble plan[4]) ;
void libere_plan_decoupage_gl         (int i           ) ;
void u_empile_et_multiplie_line_width (float v         ) ;

void affiche_gl         (const Liste *a_afficher, const char *urgent) ;
void affiche_gl_recursif(const Liste *a_afficher, const char *urgent) ;

const char* me_prevenir (const void *o, const Champ *ch    ) ;
void affiche_modele     (void       *o, const char  *urgent) ;
void u_affiche_objet    (void       *o, const char  *v     ) ;

void Gl_Affiche_Erreur(void *f, const Champ *ch, const char *message) ;


#endif
