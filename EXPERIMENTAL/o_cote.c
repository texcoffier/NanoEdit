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
#include "u_graphique.h"
#include "u_vue.h"

const Triplet z = {0, 0, 1} ;

typedef struct
{
  Bitriplet segment_en_entree ;
  Bitriplet segment_en_sortie ;
  Flottant abscisse ;
  Flottant distance_cote ;
  Booleen aller_a_droite ;
} Cote ;

static void affiche_cote(Cote *o, const Champ *ch, const char *vv)
{
  void *f ;
  Triplet v, p, q ;

  v = u_soustrait_triplet(&o->segment_en_sortie.t2, &o->segment_en_sortie.t1) ;
  v = u_normalise_triplet(&v) ;
  v = u_triplet_fois_flottant(&v, o->distance_cote) ;
  v = u_rotation_triplet(&v, &z, M_PI/2 ) ;

  q = u_ajoute_triplet(&o->segment_en_sortie.t1, &v) ;

  glEnable(GL_LINE_STIPPLE);
  glLineStipple(1,0xAAAA) ;
  glBegin(GL_LINES) ;
  t_triplet(&o->segment_en_sortie.t1) ;
  t_triplet(&q) ;
  glEnd() ;

  p = u_ajoute_triplet(&o->segment_en_entree.t1, &v) ;
  glLineStipple(1,0x1111) ;
  glBegin(GL_LINES) ;
  t_triplet(&p) ;
  t_triplet(&q) ;
  glEnd() ;
  glDisable(GL_LINE_STIPPLE) ;

  t_fleche_proportionnelle(&p, &q, ACTIF(o), 2/o->abscisse) ;

  v = u_milieu_2_triplet(&p, &q) ;
  glRasterPos3f(v.x, v.y, v.z) ;
  t_chaine(flottant_vers_char(o->abscisse)) ;


  /*
   * Affiche se qui a permit de construire
   */

  f = FILS(o, 0) ;
  if ( f )
    u_affiche_objet(f, vv) ;
}


static void changement_cote(Cote *o)
{
  Triplet v ;

  v = u_soustrait_triplet(&o->segment_en_entree.t2, &o->segment_en_entree.t1) ;
  v = u_normalise_triplet(&v) ;
  v = u_triplet_fois_flottant(&v, o->abscisse) ;

  o->segment_en_sortie.t1 = u_ajoute_triplet(&o->segment_en_entree.t1, &v) ;
  o->segment_en_sortie.t2 = u_ajoute_triplet(&o->segment_en_entree.t2, &v) ;
}

static void changement_cote_perpendiculaire(Cote *o)
{
  Triplet v ;

  v = u_soustrait_triplet(&o->segment_en_entree.t2, &o->segment_en_entree.t1) ;
  if ( o->aller_a_droite )
    v = u_rotation_triplet(&v, &z, -M_PI/2 ) ;
  else
    v = u_rotation_triplet(&v, &z, M_PI/2 ) ;

  v = u_normalise_triplet(&v) ;
  v = u_triplet_fois_flottant(&v, o->abscisse) ;

  o->segment_en_sortie.t1 = u_ajoute_triplet(&o->segment_en_entree.t1, &v) ;
  o->segment_en_sortie.t2 = u_ajoute_triplet(&o->segment_en_sortie.t1, &v) ;
}



CLASSE(cote, Cote,
       CHAMP(segment_en_sortie   , L_bipoint P_bitriplet Affiche            )
       CHAMP(segment_en_sortie.t1, L_point   P_triplet                      )
       CHAMP(segment_en_entree   , L_bipoint P_bitriplet Extrait Edite Sauve)
       CHAMP(abscisse            , L_nombre  P_flottant  Edite Sauve
	     DEFAUT("100")
	     Init3d
	     )
       CHAMP(distance_cote       , L_nombre  P_flottant  Edite Sauve
	     DEFAUT("0")
	     Init3d
	     )

       CHAMP_VIRTUEL(L_affiche_gl(affiche_cote) AFFICHAGE(Toujours_different))
       CHANGEMENT(changement_cote)
       MENU("Cotations/Cotation")
       EVENEMENT("CT")
       )

CLASSE(cote_perpendiculaire, Cote,
       SUR_CLASSE("cote")
       CHAMP(aller_a_droite, L_booleen Edite Sauve)
       CHANGEMENT(changement_cote_perpendiculaire)
       MENU("Cotations/Cotation Perpendiculaire")
       EVENEMENT("CP")
       )
