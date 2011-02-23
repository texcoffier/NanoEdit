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
 * Vous devez modifier la fonction "calcul_inter" de ce fichier
 */
#include <math.h>		/* Pour sin et cos */
#include <GL/gl.h>		/* Pour OpenGL */
#include "o_objet.h"
#include "t_geometrie.h"

struct inter_seg_bez
{
  Triplet intersection ;	/* Coordonnées de l'intersection */
  Triplet p, q ;		/* Le segment */
  Table_triplet bezier ;	/* Les pts de controle de la bézier */
  Flottant s ;			/* Taille croix affichant intersection */
} ;


static void affiche_inter_seg_bez(struct inter_seg_bez *o)
{
  glBegin(GL_LINES) ;
  glVertex3f(o->p.x, o->p.y, o->p.z) ;
  glVertex3f(o->q.x, o->q.y, o->q.z) ;

  glVertex3f(o->intersection.x-o->s, o->intersection.y, o->intersection.z) ;
  glVertex3f(o->intersection.x+o->s, o->intersection.y, o->intersection.z) ;
  glVertex3f(o->intersection.x, o->intersection.y-o->s, o->intersection.z) ;
  glVertex3f(o->intersection.x, o->intersection.y+o->s, o->intersection.z) ;
  glVertex3f(o->intersection.x, o->intersection.y, o->intersection.z-o->s) ;
  glVertex3f(o->intersection.x, o->intersection.y, o->intersection.z+o->s) ;
  glEnd() ;
}

static void calcul_inter(struct inter_seg_bez *o)
{
  /*
   * mon_intersection((struct point*)&o->p,
   *                  (struct point*)&o->q,
   *                  o->bezier.nb,
   *                  (struct point*)o->bezier.table,
   *                  (struct point*)&o->inter) ;
   */
  /*
   * Cei dessous, pour exemple, on calcul le milieu du segment
   */
  o->intersection.x = (o->p.x + o->q.x) / 2 ;
  o->intersection.y = (o->p.y + o->q.y) / 2 ;
  o->intersection.z = (o->p.z + o->q.z) / 2 ;
}

/*
 * Définition de la classe
 */

CLASSE(intersection_bezier_segment, struct inter_seg_bez,
       CHAMP(intersection, L_point3 Affiche)
       L_point3 Extrait Edite Sauve
       CHAMP(p, DEFAUT("0 0 0"))
       CHAMP(q, DEFAUT("1 1 0"))
       CHAMP(bezier, L_table_point3 DEFAUT("0 0 0\n1 0 0\n1 1 0\n0 1 0"))
       CHAMP(s, L_flottant DEFAUT("0.1")
	     BULLE_D_AIDE
	     ("Ce champ indique la taille du marqueur d'intersection")
	     )
       CHAMP_VIRTUEL(L_affiche_gl(affiche_inter_seg_bez))

       CHANGEMENT(calcul_inter)


       EVENEMENT("IBS")
       MENU("Squelettes/Intersection Bézier, Segment")
)
