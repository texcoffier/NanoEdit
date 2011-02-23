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
#include <GL/gl.h>
#include <math.h>
#include "o_objet.h"
#include "t_geometrie.h"
#include "s_string.h"

/*
 * Un sommet sur la surface extrudée par rotation autour de l'axe OZ
 * est défini par les paramètres (u,v)
 * u : varie entre 0 et contour.nb-1, c'est la position sur le contour.
 * v : varie entre "min_v" et "max_v" avec un pas de "delta_v"
 *
 * Les coordonnées du point sont :
 *   contour[u].x*cos(v) - contour[i].y*sin(v) 
 *   contour[u].x*sin(v) + contour[i].y*cos(v) 
 *   contour[u].z
 *
 */

struct rotation
{
  Table_triplet contour ;	/* Table des points en entrée */
  Flottant      min_v, max_v ;
  Flottant      delta_v ;
} ;

/*
 * Affichage.
 * Pour le moment cette fonction affiche le contour et le vecteur.
 */

static void affiche_rotation(struct rotation *o)
{
  int i ;
  /*
   * Affiche le contour initial sans aucune transformation
   */
  glBegin(GL_LINE_LOOP) ;
  for( i = 0             ; /* Premier point du contour */
       i < o->contour.nb ; /* Pour tous les points du contour */
       i++
       )
    glVertex3f(o->contour.table[i].x,
	       o->contour.table[i].y,
	       o->contour.table[i].z
	       ) ;
  glEnd() ;
  /*EXCOIN*/
  {
    Flottant v ;

    for( v=o->min_v; v<o->max_v ; v+=o->delta_v )
      {
	glBegin(GL_LINE_STRIP) ;
	for( i = 0             ; /* Premier point du contour */
	     i < o->contour.nb ; /* Pour tous les points du contour */
	     i++
	     )
	  glVertex3f(o->contour.table[i].x*cos(v)
		     - o->contour.table[i].y*sin(v) ,
		     o->contour.table[i].x*sin(v)
		     + o->contour.table[i].y*cos(v),
		     o->contour.table[i].z) ;
	glEnd() ;
      }
	for( i = 0             ; /* Premier point du contour */
	     i < o->contour.nb ; /* Pour tous les points du contour */
	     i++
	     )
      {
	glBegin(GL_LINE_STRIP) ;
    for( v=o->min_v; v<o->max_v ; v+=o->delta_v )
	  glVertex3f(o->contour.table[i].x*cos(v)
		     - o->contour.table[i].y*sin(v) ,
		     o->contour.table[i].x*sin(v)
		     + o->contour.table[i].y*cos(v),
		     o->contour.table[i].z) ;
	glEnd() ;
      }
  }
  /*EXCOOUT*/
}
/*
 * Normalement, vous n'avez pas besoin de toucher la fonction suivante.
 * Elle affiche dans la fenêtre graphe le petit texte à droite
 * du nom "extrusion_rotation"
 */
static char *information_rotation(struct rotation *o)
{
  static Chaine s ;

  s_efface(&s) ;
  s_printf(&s,
	   "angle %g->%g pas:%g",
	   o->min_v,
	   o->max_v,
	   o->delta_v
	   ) ;
  return( s_char(&s) ) ;
}
/*
 * Normalement, vous n'avez pas besoin de toucher la fonction suivante.
 * Elle vérifie la cohérence des valeurs numeriques à chaque
 * fois que les valeurs changent.
 */
static void changement_rotation(struct rotation *o)
{
  float tmp ;
  const float delta_min = .01 ;

  if ( o->min_v > o->max_v )
    {
      tmp = o->min_v ;
      o->min_v = o->max_v ;
      o->max_v = tmp ;
    }
  if ( o->delta_v < 0 )
    o->delta_v *= -1 ;
  if ( o->delta_v < delta_min )
    o->delta_v = delta_min ;  
}
/*
 * Définition de la Classe
 */
CLASSE(extrusion_rotation, struct rotation,
       Edite Extrait Sauve
       CHAMP(contour, L_table_point3
	     DEFAUT("0 0 -1\n1 0 -1\n1 0 1\n0 0 1")
	     BULLE_D_AIDE("Contour à faire tourner")
	     )
       CHAMP(min_v       , L_flottant
	     DEFAUT("0")
	     BULLE_D_AIDE("Angle de départ de la rotation.\n"
			  )
	     )
       CHAMP(max_v       , L_flottant
	     DEFAUT("6.283")
	     BULLE_D_AIDE("Angle final de rotation.\n"
			  "2*Pi : tour complet\n"
			  "Pi   : demi-tour\n"
			  "...\n"
			  )
	     )
       CHAMP(delta_v     , L_flottant
	     DEFAUT("0.1")
	     BULLE_D_AIDE("Angle séparant 2 longitudes adjacentes")
	     )

       MENU("TP Infographie/Extrusion Rotation")
       CHAMP_VIRTUEL(L_affiche_gl(affiche_rotation))
       INFORMATIONS(information_rotation)
       CHANGEMENT(changement_rotation)
       )



