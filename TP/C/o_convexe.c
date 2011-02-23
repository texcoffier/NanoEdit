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
 * Je suppose que vous travaillez avec Z=0.
 *
 * Vous devez faire le calcul du convexe
 * dans la fonction "calcul_convexe"
 *
 */

#include "o_objet.h"
#include "t_geometrie.h"
#include <GL/gl.h>

struct convexe
{
  Table_triplet points ;	/* Tableau de points */
  Liste *convexe ;		/* Indice des points du convexe */
} ;

/*
 * Affiche le polygone et le convexe
 */

static void affiche_convexe(struct convexe *o)
{
  int i ;

  glPushAttrib(GL_LINE_BIT) ;

  /*
   * Affiche le convexe
   */

  glBegin(GL_LINE_LOOP) ;
  POUR( int,			/* C'est une liste d'indice de points */
	s,			/* Nom de l'indice de la boucle */
	o->convexe,		/* La liste des indices */
	{
	  glVertex2f( o->points.table[s].x, o->points.table[s].y) ;
	}
	) ;
  glEnd() ;

  /*
   * Affiche le contour du polygone
   */

  glEnable(GL_LINE_STIPPLE);
  glLineStipple(1,255) ;
  glBegin(GL_LINE_LOOP) ;
  for(i=0;i<o->points.nb;i++)
    glVertex2f(o->points.table[i].x, o->points.table[i].y) ;
  glEnd() ;
  glDisable(GL_LINE_STIPPLE);

  glPopAttrib() ;
}

/*
 * Calcul de la diagonalisation
 */

static void calcul_convexe(struct convexe *o)
{
  int i ;

  l_detruit(&o->convexe) ;		/* Detruit la liste des sommets */

  /*
   * Comme exemple, je construis la liste des sommets du convexe
   * en prenant un sommet sur deux du polygone original.
   */
  for(i=0;i<o->points.nb;i+=2)
    l_ajoute(&o->convexe, (void*)i) ;
}


/*
 * Definition de la classe
 */

CLASSE(polygone_convexe, struct convexe,
       CHAMP(points, L_table_point3 Edite Extrait Sauve
	     DEFAUT("-1 -1 0\n"
		    "-0.5 0\n"
		    "-1 1\n"
		    "0 0.5\n"
		    "1 1\n"
		    "0.5 0\n"
		    "1 -1\n"
		    "0 -0.5\n"
		    )
	     )
       CHAMP(convexe, L_liste Affiche)

       CHAMP_VIRTUEL(L_affiche_gl(affiche_convexe))
       CHANGEMENT(calcul_convexe)
       MENU("Squelettes/Convexe Polygone")
       )
