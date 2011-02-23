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
#include "t_gl.h"                                 /**/
#include "o_objet.h"
#include "t_geometrie.h"
#include "u_graphique.h"
struct segment {  Triplet depart, arrivee ;
                  Gl gl ;                         /**/
               } ;	   
static void segment_trace(const Interface_widget *iw)
{
  struct segment *o ;
  int i ;
  Flottant ac, di ;
  Triplet tmp, vec, vecd ;

  o = iw->objet ;
  vec = u_soustrait_triplet(&o->depart, &o->arrivee) ;
  vec.z = vec.x ;
  vec.x = vec.y ;
  vec.y = -vec.z ;
  vec.z = 0 ;
  Gl_start(&o->gl) ;
  glClearColor(0.,0.,0.,0.) ;
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) ;
  glBegin(GL_POINTS) ;
  for(i=0;i<1000;i++)
    {
      ac = (rand()%10000)/9999. ;
      di = (rand()%10000)/9999. - .5 ;
      di *= di * di ;
      tmp = u_ponderation_2_triplet(&o->depart, ac, &o->arrivee, 1-ac) ;
      vecd = u_triplet_fois_flottant(&vec, di) ;
      tmp = u_ajoute_triplet(&tmp, &vecd) ;
      t_triplet(&tmp) ;
    }
  glEnd() ;
  Gl_stop(&o->gl) ;
}
CLASSE(segment18,
       struct segment,
       CHAMP(depart , L_point P_triplet Edite Sauve Init3d Extrait)
       CHAMP(arrivee, L_point P_triplet Edite Sauve Init3d Extrait)
       CHAMP(gl,
	     L_gl Edite
	     TE_HAUTEUR(500) TE_LARGEUR(500)
	     TE_ECRIT(segment_trace))
      )
