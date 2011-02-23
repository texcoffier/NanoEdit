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
 * Vous devez faire le calcul des diagonales
 * dans la fonction "calcul_diagonales"
 *
 * Les sommets a relier par une diagonale sont stockes dans une liste.
 *
 * Si vous avez besoin de fonction sur les listes, elles sont donnees
 * dans : /home/exco/NE/stable/UTILITAIRES/l_liste.h
 */

#include "o_objet.h"
#include "t_geometrie.h"
#include <GL/gl.h>

struct triangule
{
  Table_triplet points ;	/* Tableau de points */
  Liste *diagonales ;		/* Indices des points à relier par paire */
} ;

/*
 * Affiche le polygone et les diagonales.
 * Vous n'avez pas besoin de modifier cette fonction.
 */

static void affiche_diagonales(struct triangule *o)
{
  int i, j ;

  /*
   * Affiche le contour du polygone
   */

  glBegin(GL_LINE_LOOP) ;
  for(i=0;i<o->points.nb;i++)
    glVertex2f(o->points.table[i].x, o->points.table[i].y) ;
  glEnd() ;

  /*
   * Affiche les diagonales.
   */

  glEnable(GL_LINE_STIPPLE);
  glLineStipple(1,255) ;
  glBegin(GL_LINES) ;
  for( i = l_longueur(o->diagonales)-1 ; i>=0 ; i-=2 )
    {
      j = (int)l_element(o->diagonales,i) ;
      glVertex2f( o->points.table[j].x, o->points.table[j].y) ;
      j = (int)l_element(o->diagonales,i-1) ;
      glVertex2f( o->points.table[j].x, o->points.table[j].y) ;
    }
  glEnd() ;
  glDisable(GL_LINE_STIPPLE);
}

/*
 * Calcul de la diagonalisation
 */

static void calcul_diagonales(struct triangule *o)
{
  int i, j ;

  l_detruit(&o->diagonales) ;		/* Detruit la liste des diagonales */

  /*
   * Comme exemple, je construis la liste de toutes les diagonales
   * entre tous les sommets.
   */
  for(i=0;i<o->points.nb;i++)
    for(j=i+1;j<o->points.nb;j++)
      {
	l_ajoute(&o->diagonales, (void*)i) ;	/* Indice point de depart */
	l_ajoute(&o->diagonales, (void*)j) ;	/* Indice point d'arrive */
      }
}


/*
 * Definition de la classe
 */

CLASSE(triangulation, struct triangule,
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
       CHAMP(diagonales, L_liste Affiche)

       CHAMP_VIRTUEL(L_affiche_gl(affiche_diagonales))
       CHANGEMENT(calcul_diagonales)
       MENU("Squelettes/Diagonalise Polygone")
       EVENEMENT("Shft+DP")
       )

