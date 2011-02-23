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
#include "o_objet.h"		/* Definition des noeuds */
#include "t_geometrie.h"

struct interseg
{
  Triplet   inter ;
  Triplet   p1, p2, q1, q2 ;
  Flottant  s ;			/* Taille croix affichant intersection */
} ;

static void affiche_interseg(struct interseg *o)
{
  glBegin(GL_LINES) ;
  glVertex3f(o->p1.x, o->p1.y, o->p1.z) ;
  glVertex3f(o->p2.x, o->p2.y, o->p2.z) ;
  glVertex3f(o->q1.x, o->q1.y, o->q1.z) ;
  glVertex3f(o->q2.x, o->q2.y, o->q2.z) ;

  glVertex3f(o->inter.x-o->s, o->inter.y     , o->inter.z     ) ;
  glVertex3f(o->inter.x+o->s, o->inter.y     , o->inter.z     ) ;
  glVertex3f(o->inter.x	    , o->inter.y-o->s, o->inter.z     ) ;
  glVertex3f(o->inter.x	    , o->inter.y+o->s, o->inter.z     ) ;
  glVertex3f(o->inter.x	    , o->inter.y     , o->inter.z-o->s) ;
  glVertex3f(o->inter.x	    , o->inter.y     , o->inter.z+o->s) ;
  glEnd() ;
}

static void calcul_inter(struct interseg *o)
{
  /*
   * mon_intersection((struct point*)&o->p1,
   *                  (struct point*)&o->p2,
   *                  (struct point*)&o->q1,
   *                  (struct point*)&o->q2,
   *                  (struct point*)&o->inter) ;
   *
   * Un triplet est formé de trois doubles X, Y, Z
   */
  o->inter.x = o->p1.x + o->p2.x ;
  o->inter.y = o->q1.y + o->q2.y ;
  o->inter.z = o->p1.z + o->q2.z ;
}

/*
 * Définition classe
 */

CLASSE(intersection_segment, struct interseg,

       CHAMP(inter, L_point3 Affiche)

       L_point3 Edite Extrait Sauve Init3d
       CHAMP(p1, DEFAUT("0 0 0"))
       CHAMP(p2, DEFAUT("1 .5 0"))
       CHAMP(q1, DEFAUT("0.5 0 0"))
       CHAMP(q2, DEFAUT("0 1 0"))

       CHAMP(s, L_flottant DEFAUT("0.1")
	     BULLE_D_AIDE
	     ("Ce champ indique la taille du marqueur d'intersection")
	     )

       CHAMP_VIRTUEL(L_affiche_gl(affiche_interseg))

       CHANGEMENT(calcul_inter)


       EVENEMENT("IS")
       MENU("Squelettes/Intersection de 2 segments")
       )
