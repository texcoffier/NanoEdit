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
#include "o_objet.h"
#include "t_geometrie.h"
#include "u_graphique.h"
#include "u_etat_courant.h"

typedef struct lampe
{
  t_observateur observateur ;
  Triplet couleur ;
  Flottant spot_exp, spot_cutoff, attenuation[3] ;
} Lampe ;

static void affiche_lampe(Lampe *o)
{
  if ( ACTIF(o) )
    t_tag_origine(0, &o->observateur.point_de_vue) ;
  if ( o->spot_cutoff >= 180 )
    {
      glBegin(GL_POINTS) ;
      t_triplet(&o->observateur.point_de_vue) ;
      glEnd();
    }
  else
    {
      glLineStipple(1,255) ;
      glEnable(GL_LINE_STIPPLE);
      glBegin(GL_LINES) ;
      t_triplet(&o->observateur.point_de_vue) ;
      t_triplet(&o->observateur.point_vise) ;
      glEnd();
      glDisable(GL_LINE_STIPPLE);
      if ( ACTIF(o) )
	t_tag_origine(1, &o->observateur.point_vise) ;
    }
}

static void applique_lampe(Lampe *o, const Champ *ch, Booleen *modif)
{
  globale_etat_courant.lampe++ ;
  *modif = Vrai ;
}

static void applique_gl_lampe(Lampe *o)
{
  GLfloat tmp[4] ;
  int l ;

  if ( TRACE(o) )
    eprintf("applique attributs lampe=%p\n", &globale_etat_courant.lampe) ;

  
  l = GL_LIGHT0 + globale_etat_courant.lampe ;
  glEnable(l) ;
  tmp[3] = 1 ; 

  tmp[0] = o->observateur.point_de_vue.x ; 
  tmp[1] = o->observateur.point_de_vue.y ; 
  tmp[2] = o->observateur.point_de_vue.z ;
  tmp[3] = 1 ;
  glLightfv(l, GL_POSITION             , tmp) ;

  tmp[0] = o->couleur.x ; 
  tmp[1] = o->couleur.y ; 
  tmp[2] = o->couleur.z ; 
  glLightfv(l, GL_DIFFUSE 	       , tmp) ;
  glLightfv(l, GL_SPECULAR	       , tmp) ;

  tmp[0] = tmp[1] = tmp[2] = 0 ;
  glLightfv(l, GL_AMBIENT	       , tmp) ;

  tmp[0] = o->observateur.point_vise.x - o->observateur.point_de_vue.x  ; 
  tmp[1] = o->observateur.point_vise.y - o->observateur.point_de_vue.y  ; 
  tmp[2] = o->observateur.point_vise.z - o->observateur.point_de_vue.z  ; 
  glLightfv(l, GL_SPOT_DIRECTION       , tmp) ;

  glLightf (l, GL_SPOT_EXPONENT        , o->spot_exp) ;
  glLightf (l, GL_SPOT_CUTOFF          , o->spot_cutoff) ;
  glLightf (l, GL_CONSTANT_ATTENUATION , o->attenuation[0]) ;
  glLightf (l, GL_LINEAR_ATTENUATION   , o->attenuation[1]) ;
  glLightf (l, GL_QUADRATIC_ATTENUATION, o->attenuation[2]) ;
}

static const char * attributs_gl_lampe(Lampe *o, const Champ *ch)
{
  return( int_vers_char(GL_LIGHTING_BIT) ) ;
}

/*
static void changement_lampe(Lampe *o)
{
  if ( !CREATION(o) && CHAMP_CHANGE(o, observateur) )
    {
      o->position = o->observateur.point_de_vue ;
	o->spot = u_soustrait_triplet(&o->observateur.point_vise,
				      &o->observateur.point_de_vue) ;
    }
  else
    if ( CHAMP_CHANGE(o, spot) || CHAMP_CHANGE(o, position) )
      {
	o->observateur.point_de_vue = o->position ;
	o->observateur.point_vise = u_ajoute_triplet(&o->position, &o->spot) ;
      }
}
*/
/*
 *
 */

static void ecrit_pov_lampe(Lampe *o, const Champ *c, const char *v)
{
  fprintf( (FILE*)atol(v),
	   "light_source {<%g,%g,%g> color rgb <%g,%g,%g>}\n",
	   o->observateur.point_de_vue.x,
	   o->observateur.point_de_vue.y,
	   o->observateur.point_de_vue.z,
	   o->couleur.x, o->couleur.y, o->couleur.z) ;
}

static void ecrit_obj_lampe(Lampe *o, const Champ *c, const char *v)
{
  fprintf( (FILE*)atol(v),
	   "## \n"
	   "## GL_POSITION %g %g %g\n"
	   "## GL_DIFFUSE+GL_SPECULAR %g %g %g\n"
	   "## GL_AMBIENT 0 0 0\n"
	   "## GL_SPOT_DIRECTION %g %g %g\n"
	   "## GL_SPOT_EXPONENT %g\n"
	   "## GL_SPOT_CUTOFF %g\n"
	   "## GL_CONSTANT_ATTENUATION %g\n"
	   "## GL_LINEAR_ATTENUATION %g\n"
	   "## GL_QUADRATIC_ATTENUATION %g\n"
	   ,
	   o->observateur.point_de_vue.x,
	   o->observateur.point_de_vue.y,
	   o->observateur.point_de_vue.z,
	   o->couleur.x,
	   o->couleur.y,
	   o->couleur.z,
	   o->observateur.point_vise.x - o->observateur.point_de_vue.x, 
	   o->observateur.point_vise.y - o->observateur.point_de_vue.y,
	   o->observateur.point_vise.z - o->observateur.point_de_vue.z,
	   o->spot_exp,
	   o->spot_cutoff,
	   o->attenuation[0],
	   o->attenuation[1],
	   o->attenuation[2]
	   ) ;
}

CLASSE(lampe, Lampe,
       CHAMP(observateur, L_observateur Sauve Extrait)
       Edite 
       BULLE_D_AIDE("Cet objet permet d'éclairer la scène sur lequel\n"
		    "on l'applique.\n"
		    "Pour avoir des informations plus précise concernant\n"
		    "les paramètres vous pouvez faire :\n"
		    "man glLigthf\n"
		    )
       CHAMP(observateur.point_de_vue,
	     NOM("position") LABEL("position")
	     L_point3
	     Init3d
	     DEFAUT("1 1 2")
	     BULLE_D_AIDE
	     ("Position de la lampe."
	      )
	     )
       CHAMP(observateur.point_vise,
	     L_point3
	     Init3d
	     DEFAUT("0 0 -1")
	     NOM("spot") LABEL("spot")
	     BULLE_D_AIDE
	     ("Point qu'elle éclaire si elle\n"
	      "n'est pas omnidirectionnelle (voir le «cutoff»)."
	      )
	     )
       Extrait Sauve
       CHAMP(couleur,
	     L_couleur3
	     Init3d
	     DEFAUT("1 1 1")
	     BULLE_D_AIDE
	     ("Couleur émise par la lampe.\n"
	      "Les valeurs peuvent être supérieures à 1.\n"
	      )
	     )
       L_nombre_positif P_flottant DEFAUT("0")
       CHAMP(spot_exp,
	     BULLE_D_AIDE
	     ("Si la lampe est un spot, cette valeur indique\n"
	      "comment l'intentsité du spot diminue\n"
	      "en fonction de la distance au centre du spot.\n"
	      )
	     )
       CHAMP(spot_cutoff, DEFAUT("180") 
	     BULLE_D_AIDE
	     ("Indique la taille du cone du spot.\n"
	      "180 : Pas de spot car omnidirectionnel.\n"
	      "0-90 : dimension du spot en degré.\n"
	      )
	     )
       CHAMP(attenuation[0], DEFAUT("1") 
	     BULLE_D_AIDE
	     ("L'atténuation de la lumière en fonction de la distance.\n"
	      "  1/(attenuation0 + d*attenuation1 + d*d*attenuation2"
	      )
	     )
       CHAMP(attenuation[1],
	     BULLE_D_AIDE
	     ("L'atténuation de la lumière en fonction de la distance.\n"
	      "  1/(attenuation0 + d*attenuation1 + d*d*attenuation2"
	      )
	     )
       CHAMP(attenuation[2],
	     BULLE_D_AIDE
	     ("L'atténuation de la lumière en fonction de la distance.\n"
	      "  1/(attenuation0 + d*attenuation1 + d*d*attenuation2"
	      )
	     )
       CHAMP_VIRTUEL(L_affiche_gl(affiche_lampe))
       CHAMP_VIRTUEL(L_applique_gl_rapide(applique_gl_lampe,
					  attributs_gl_lampe))
       CHAMP_VIRTUEL(L_applique(applique_lampe))
       CHAMP_VIRTUEL(L_affiche_pov(ecrit_pov_lampe))
       CHAMP_VIRTUEL(L_affiche_obj(ecrit_obj_lampe))
       EVENEMENT("0l0a")
       MENU("Attributs/Graphique/Lampe")
       A_EMPILER(globale_etat_courant.lampe)
       )
