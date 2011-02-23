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
 * Créé le Wed Feb  9 13:27:10 2000 par Thierry EXCOFFIER
 */

#ifndef _HOME_EXCO_NE_1_8_OBJETS_U_APPLIQUE_H
#define _HOME_EXCO_NE_1_8_OBJETS_U_APPLIQUE_H

void pov_fait_union(FILE *f, const Liste *l_fils, const char *v) ;
void stl_fait_union(FILE *f, const Liste *l_fils, const char *v) ;
void obj_fait_union(FILE *f, const Liste *l_fils, const char *v) ;


#endif
