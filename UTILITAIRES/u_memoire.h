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
 * Créé le Mon Apr  3 13:31:00 2000 par Thierry EXCOFFIER
 */

#ifndef _HOME_EXCO_NE_1_10_UTILITAIRES_U_MEMOIRE_H
#define _HOME_EXCO_NE_1_10_UTILITAIRES_U_MEMOIRE_H

typedef struct
{
  void **objets_alloues ;
  unsigned int nb_objets_alloues ;
  unsigned int nb_max_objets_alloues ;

  int *emplacements_vides ;
  unsigned int nb_emplacements_vides ;
  unsigned int nb_max_emplacements_vides ;
  
} Gestion_memoire ;


#define INDEX_OBJET(O) (((unsigned int*)(O))[-1])

#define MEMOIRE_VALIDE(GM, O)				\
(							\
 INDEX_OBJET(O) < (GM).nb_objets_alloues		\
 && (GM).objets_alloues[INDEX_OBJET(O)] == (O)	\
)


void *alloue_zone_memoire(Gestion_memoire *gm, unsigned int taille) ;
void *realloue_zone_memoire(Gestion_memoire *gm, void *, unsigned int taille) ;
void libere_zone_memoire(Gestion_memoire *gm, void *objet) ;
int nb_zones_memoires(Gestion_memoire *gm) ;



#endif
