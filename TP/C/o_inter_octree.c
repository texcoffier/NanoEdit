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
 * Vous devez faire la fonction d'intersection d'octree
 */
#include <string.h>
#include "o_objet.h"

struct inter
{
  char *resultat ;		/* Octree resultat de l'intersection */
  char *octree_a ;
  char *octree_b ;
} ;

/*
 * Calcul de l'intersection des octrees A et B et stocke le resultat dans B
 */

static void calcul_intersection(struct inter *o)
{
  /* Libere ancienne valeur */
  free(o->resultat) ;

  /*
   * Calcul intersection
   */



  /* Sauvegarde l'intersection */
  o->resultat = strdup("Programme non-fonctionnel") ;
}

/*
 * Definition de la classe
 */

CLASSE(intersection_octree, struct inter,
       CHAMP(resultat, L_chaine Affiche)

       L_chaine Edite Extrait Sauve Obligatoire
       CHAMP(octree_a, DEFAUT("(11110000)"))
       CHAMP(octree_b, DEFAUT("(10101010)"))

       CHANGEMENT(calcul_intersection)
       MENU("TP Infographie/Intersection Octree")
       )

