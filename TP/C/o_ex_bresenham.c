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
#include <GL/gl.h>		/* Car il affiche du GL */
#include <math.h>		/* Pour rint */
#include "o_objet.h"
#include "t_geometrie.h"

struct segment_droite
{
  Triplet p ;			/* Point départ */
  Triplet q ;			/* Point d'arrivée */
} ;

/*
 * Affichage du segment de droite entre (o->p.x, o->p.y) et (o->q.x, o->q.y)
 * Vous n'avez pas le droit d'utiliser les fonctions GL modifiant
 * le repère courant (glRotate, ...).
 */
static void affiche_segment_droite(struct segment_droite *o)
{
  int  x1, y1, x2, y2, dx, dy ;

  x1 = rint(o->p.x) ;
  y1 = rint(o->p.y) ;
  x2 = rint(o->q.x) ;
  y2 = rint(o->q.y) ;
  dx = x2 - x1 ;
  dy = y2 - y1 ;
  /*
   * Trace la droite pixel par pixel en faisant
   * de gros carré de coté 1 pour chaque pixel.
   * N'oubliez pas que le centre du pixel est au centre
   * du carré et pas en bas à gauche...
   */
  glBegin(GL_QUADS) ;
  /*******************/
  /* Votre programme */
  /*******************/
  (void)dx ;
  (void)dy ;
  glEnd() ;				
  /*
   * Trace le segment de droite que l'on veut approximer.
   */
  glPushAttrib( GL_CURRENT_BIT ) ; /* Empile la couleur */
  glBegin(GL_LINES) ;
  glColor3f(1.,0.,0.) ;
  glVertex3f(x1, y1,.001) ;
  glVertex3f(x2, y2,.001) ;
  glEnd() ;
  glPopAttrib() ;		   /* Dépile la couleur */
}
/*
 * Les champs
 */
CLASSE(tp_segment_droite, struct segment_droite,
       L_point3 Edite Sauve Extrait Init3d
       CHAMP(p,)
       CHAMP(q,)
       CHAMP_VIRTUEL(L_affiche_gl(affiche_segment_droite))
       MENU("TP Infographie/Bresenham R2")
       )
