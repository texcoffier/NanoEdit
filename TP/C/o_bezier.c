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
 * Vous devez modifier la fonction "affiche" de ce fichier
 */
#include "o_objet.h"
#include "t_geometrie.h"
#include <GL/gl.h>		/* Pour OpenGL */

struct bezier			/* Le nom "moi" est obligatoire */
{
  Flottant pas ;		/* Pour incrementer le parametre */
  Table_triplet points ;	/* Table des points de contrôle */
} ;


static void affiche_bezier(struct bezier *o)
{
  /*
   * Affichage de la courbe de Bézier
   * Le nombre de points de contrôle             : o->points.nb
   * Les coordonnées des points                  : o->points.table
   *
   * L'Exemple affiche une ligne brisée passant par tous les points
   * de contrôle.
   */
  int i ;
  glBegin(GL_LINE_STRIP) ;
  for(i=0;i<o->points.nb;i++)
    glVertex3f(o->points.table[i].x,
	       o->points.table[i].y,
	       o->points.table[i].z) ;
  glEnd() ;
}

/*
 * Définition de la classe
 */

CLASSE(bezier, struct bezier,
       Edite Extrait Sauve
       CHAMP(points, L_table_point3 Edite Extrait Sauve
	     DEFAUT("0 0 0\n1 0 0\n1 1 0\n0 1 0")
	     )
       CHAMP(pas, L_flottant DEFAUT("0.1"))

       CHAMP_VIRTUEL(L_affiche_gl(affiche_bezier))

       EVENEMENT("Shft+BE")
       MENU("Squelettes/Bézier")

)
