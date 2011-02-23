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
 * POV est tout le contraire d'un état courant.
 * Ceci est donc complètement illogique,mais bon, il faut
 * faire avec : descendre tous les attributs sur les feuilles
 */


#include <GL/gl.h>		/* Car il affiche du GL */
#include <math.h>		/* Pour M_PI */
#include "t_gl.h"
#include "t_geometrie.h"
#include "u_xt_interface.h"
#include "o_nuplet.h"
#include "u_hlsrvb.h"
#include "u_graphique.h"
#include "u_etat_courant.h"
#include "s_string.h"

#define mm globale_etat_courant

#define DEBUT_TAG_COULEUR (0*DEBUT_TAG_DEPLACABLE+1000)

typedef struct etat_courant
{
  int priorite ;
} Etat_courant ;

CLASSE(etat_courant, Etat_courant,
       CHAMP(priorite, L_nombre P_entier Edite Sauve DEFAUT("0")
	     BULLE_D_AIDE("Priorité de la modification d'état courant.\n"
			  "Lors du parcours de l'arbre la modification\n"
			  "n'est faite que si la priorité indiquée ici\n"
			  "est plus grande que la dernière prioritée\n"
			  "indiquée pour cet attribut.")
	     )
       MENU("NON")
       )

/*
 * Toutes les sous-classes de "etat_courant" doivent avoir
 * leur priorité en premier champ de leur structure C
 */

typedef struct couleur
{
  Etat_courant ec ;
  Triplet point ;	/* Les coordonnees du point (sous-classe point) */
  Gl gl ;		/* Pour l'affichage HLS */
} Couleur ;


/*
 * Affichage d'une couleur en HLS (Travaille sur le champ "point")
 */

static void u_hls_trace(const Interface_widget *iw)
{
  CLR_RGB rvb, couleur ;
  CLR_HLS hls, hls_initial ;
  int x ;
  Flottant lar ;
  Gl *gl ;

  gl = &T(Gl, iw->objet, iw->ch) ;

  if ( TRACE(iw->objet) )
    eprintf("Début tracé GL de la palette %dx%d\n", gl->lar, gl->hau) ;


  lar = gl->lar + .25 ;

  
  *((Triplet*)&rvb) =
    char_vers_triplet(c_recupere_sans_copie(iw->objet, "point")) ;
  hls_initial = CLR_rgb_to_hls(&rvb) ;

  Gl_start(gl) ;
  glDisable(GL_DEPTH_TEST) ;
  glBegin(GL_LINES) ;
  hls.l = 0.5 ;
  hls.s = 1. ;
  for(x=0;x<gl->lar;x++)
    {
      hls.h = x*2*M_PI/lar ;
      couleur = CLR_hls_to_rgb(&hls) ;
      glColor3f(couleur.r, couleur.g, couleur.b) ;
      glVertex2f(x/lar*2-1,1) ;
      glVertex2f(x/lar*2-1,1-2/3.) ;
    }
  hls.h = hls_initial.h ;
  hls.s = hls_initial.s ;
  for(x=0;x<gl->lar;x++)
    {
      hls.l = x/lar ;
      couleur = CLR_hls_to_rgb(&hls) ;
      glColor3f(couleur.r, couleur.g, couleur.b) ;
      glVertex2f(x/lar*2-1,1-2/3.) ;
      glVertex2f(x/lar*2-1,1-4/3.) ;
    }
  hls.h = hls_initial.h ;
  hls.l = hls_initial.l ;
  for(x=0;x<gl->lar;x++)
    {
      hls.s = x/lar ;
      couleur = CLR_hls_to_rgb(&hls) ;
      glColor3f(couleur.r, couleur.g, couleur.b) ;
      glVertex2f(x/lar*2-1,1-4/3.) ;
      glVertex2f(x/lar*2-1,-1) ;
    }
  hls = hls_initial ;
  glColor3f(hls.l>0.5 ? 0 : 1,hls.l>0.5 ? 0 : 1,hls.l>0.5 ? 0 : 1) ;
  glVertex2f(hls.h/M_PI-1,1) ;
  glVertex2f(hls.h/M_PI-1,1-2/3.) ;
  glVertex2f(hls.l*2-1,1-2/3.) ;
  glVertex2f(hls.l*2-1,1-4/3.) ;
  glVertex2f(hls.s*2-1,1-4/3.) ;
  glVertex2f(hls.s*2-1,-1) ;
  glEnd() ;
  glEnable(GL_DEPTH_TEST) ;
  Gl_stop(gl) ;
}

/*
 *
 */

Booleen u_hls_bouton(const Interface_widget *iw)
{
  CLR_RGB rvb ;
  CLR_HLS hls ;
  Gl *gl ;
  Flottant lar ;

  gl = &T(Gl, iw->objet, iw->ch) ;
  Gl_start(gl) ;
  lar = gl->lar ;

  *((Triplet*)&rvb) =
    char_vers_triplet(c_recupere_sans_copie(iw->objet, "point")) ;
  hls = CLR_rgb_to_hls(&rvb) ;

  if ( hls.l>=1. ) hls.l=.9999 ;
  if ( hls.l<=0. ) hls.l=.0001 ;
  if ( hls.s>=1. ) hls.s=.9999 ;
  if ( hls.s<=0. ) hls.s=.0001 ;
  if ( iw->y<gl->hau/3 )
    hls.h = 2*M_PI*iw->x/lar ;
  else
    if ( iw->y<2*gl->hau/3 )
      hls.l = iw->x/lar ;
    else
      hls.s = iw->x/lar ;

  rvb = CLR_hls_to_rgb(&hls) ;
  c_sauve(iw->objet, "point", triplet_vers_char((Triplet*)&rvb) ) ;
  return(Vrai) ; /* Continue le déplacement */
}

void u_hls_mouvement_bouton(const Interface_widget *iw)
{
  if ( iw->b >= 0 )		/* Si bouton appuyé */
    u_hls_bouton(iw) ;
}

static void u_vertex(float h, float l)
{
  glVertex2f(cos(h)*l, sin(h)*l) ;
}

static void u_couleur(float h, float l, float s)
{
  CLR_HLS hls ;
  CLR_RGB rgb ;

  hls.h = h ;
  hls.l = l ;
  hls.s = s ;
  rgb = CLR_hls_to_rgb(&hls) ;
  glColor3f(rgb.r, rgb.g, rgb.b) ;
}

Triplet intersection_droite_plan_OXY(Triplet *pt, Triplet *v)
{
  float t ;
  Triplet intersection ;

  if ( ABS(v->z) < .001 )
    {
      intersection = *pt ;
      intersection.z = 0 ;
    }
  else
    {
      t = -pt->z / v->z ;
      intersection = u_triplet_fois_flottant(v, t) ;
      intersection = u_ajoute_triplet(&intersection, pt) ;  
    }
  return(intersection) ;
}

Triplet xy_to_hl(Triplet *xy)
{
  Triplet hls ;

  hls.x = atan2(xy->y, xy->x) + 2*M_PI ;
  hls.y = sqrt(xy->y*xy->y + xy->x*xy->x) ;
  if ( hls.y > 1 )
    hls.y = 1 ;
  hls.z = 1 ;
  return(hls) ;
}


static void clique_palette(Couleur *o, const Champ *ch, const char *urgent)
{
  CLR_RGB rgb ;
  Triplet pt, vecteur, plan ;
  CLR_HLS hls ;

  pt = char_vers_triplet(urgent) ;
  vecteur = char_vers_triplet( strchr(urgent, '(')+1 ) ;
  plan = intersection_droite_plan_OXY(&pt, &vecteur) ;

  plan = xy_to_hl(&plan) ;

  switch ( mm.affichage_palette_couleur )
    {
    case TeinteLuminosite:
      hls.h = plan.x ;
      hls.l = plan.y ;
      hls.s = 1. ;
      break ;
    case TeinteSaturation:
      hls.h = plan.x ;
      hls.l = 0.5 ;
      hls.s = plan.y ;
      break ;
    case Point_de_R3:
      break ;
    case Palette_Invisible:
      EXIT ;
    }
  rgb = CLR_hls_to_rgb(&hls) ;

  o->point.x = rgb.r ;
  o->point.y = rgb.g ;
  o->point.z = rgb.b ;
  NOTE_UN_CHAMP_CHANGE(o) ; // XXX 27/07/2010 Pas utile avant version 2.0
}

static const char* lit_palette(Couleur *o, const Champ *ch)
{
  static Chaine s ;

  s_efface(&s) ;
  s_printf(&s, "%s RVB\n", triplet_vers_char(&o->point)) ;
  return(s_char(&s)) ;
}

#define DECOUPAGE_H mm.decoupage_hls[0]
#define DECOUPAGE_L mm.decoupage_hls[1]
#define DECOUPAGE_S mm.decoupage_hls[2]

static void affiche_couleur(Couleur *o, const Champ *ch, const char *urgent)
{
  float l, h, l2, s, s2 ;
  float h_delta, l_delta, l_delta2, s_delta, s_delta2 ;
  const float l_min=0, l_max=.93 ;
  const float s_min=0, s_max=1 ;
  CLR_HLS hls ;
  CLR_RGB rgb ;
  Triplet p ;

  switch ( mm.affichage_palette_couleur )
    {
    case TeinteLuminosite:
      glPushName(DEBUT_TAG_COULEUR) ;

      h_delta = 2*M_PI/DECOUPAGE_H ;
      l_delta = (l_max-l_min)/DECOUPAGE_L  ;
      l_delta2 = (l_max-l_min)/(DECOUPAGE_L-1) ;
      
      glPushAttrib(GL_LIGHTING_BIT) ;
      glShadeModel(GL_FLAT) ;
      glBegin(GL_QUADS) ;

      for(l=l_min; l<l_max-l_delta/2; l += l_delta )
	{
	  for(h=0;h<2*M_PI-h_delta/2;h += h_delta)
	    {
	      l2 = l_min+(l-l_min)/l_delta*l_delta2 ;
	      u_couleur(h+h_delta/2, l2, 1) ;
	      u_vertex(h, l) ;
	      u_vertex(h, l+l_delta) ;
	      u_vertex(h+h_delta, l+l_delta) ;
	      u_vertex(h+h_delta, l) ;
	    }
	}
      glEnd() ;
      glPopAttrib() ;

      rgb.r = o->point.x ;
      rgb.g = o->point.y ;
      rgb.b = o->point.z ;
      hls = CLR_rgb_to_hls(&rgb) ;
      p.x = cos(hls.h)*hls.l ;
      p.y = sin(hls.h)*hls.l ;
      p.z = 0 ;
      glPopName() ;
      t_tag(3333, &p) ;
      break ;

    case TeinteSaturation:
      glPushName(DEBUT_TAG_COULEUR) ;

      h_delta = 2*M_PI/DECOUPAGE_H ;
      s_delta = (s_max-s_min)/DECOUPAGE_S ;
      s_delta2 = (s_max-s_min)/(DECOUPAGE_S-1) ;
      
      glPushAttrib(GL_LIGHTING_BIT) ;
      glShadeModel(GL_FLAT) ;
      glBegin(GL_QUADS) ;

      for(s=s_min; s<s_max-s_delta/2; s += s_delta )
	{
	  for(h=0;h<2*M_PI-h_delta/2;h += h_delta)
	    {
	      s2 = s_min+(s-s_min)/s_delta*s_delta2 ;
	      u_couleur(h+h_delta/2, .5, s2) ;
	      u_vertex(h, s) ;
	      u_vertex(h, s+s_delta) ;
	      u_vertex(h+h_delta, s+s_delta) ;
	      u_vertex(h+h_delta, s) ;
	    }
	}
      glEnd() ;
      glPopAttrib() ;

      rgb.r = o->point.x ;
      rgb.g = o->point.y ;
      rgb.b = o->point.z ;
      hls = CLR_rgb_to_hls(&rgb) ;
      p.x = cos(hls.h)*hls.s ;
      p.y = sin(hls.h)*hls.s ;
      p.z = 0 ;
      glPopName() ;
      t_tag(3333, &p) ;
      break ;

    case Point_de_R3:
      if ( ACTIF(o) )
	{
	  t_tag_origine(0, &o->point) ;
	}
      else
	{
	  glBegin(GL_POINTS) ;		/* On veut afficher des points */
	  t_triplet(&o->point) ;
	  glEnd();			/* On arrete d'afficher des points */
	}
      break ;


    case Palette_Invisible:
      break ;
    }
}

static void applique_couleur(Couleur *o, const Champ *ch, Booleen *modif)
{
  if ( TRACE(o) )
    eprintf("applique couleur %s modif=%p\n", triplet_vers_char(&o->point), modif) ;
  if ( PLUS_PRIORITAIRE(couleur) )
    mm.couleur = o->point ;
}

static void applique_gl_couleur(Couleur *o, const Champ *ch, const char *urgent)
{
  GLfloat p[4] ;

  p[0] = o->point.x ;
  p[1] = o->point.y ;
  p[2] = o->point.z ;
  p[3] = mm.diffuse+mm.speculaire-mm.refraction-mm.reflexion ;

  if ( TRACE(o) )
    eprintf("applique gl couleur %g %g %g %g\n", p[0], p[1], p[2], p[3]) ;

  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, p) ;
  glBegin(GL_LINES) ;
  glColor4fv(p) ;
  glVertex3f(1000.,1000.,1000.) ; /* Bug GL ? */
  glVertex3f(1001.,1001.,1001.) ;
  glEnd() ;
}

static const char * attributs_gl_couleur(Couleur *o, const Champ *ch)
{
  if ( TRACE(o) )
    eprintf("Attributs GL = %d\n"
	    , int_vers_char(GL_CURRENT_BIT|GL_LIGHTING_BIT) ) ;
  return( int_vers_char(GL_CURRENT_BIT|GL_LIGHTING_BIT) ) ;
}

/*
 *
 */

CLASSE(couleur, Couleur,
       SUR_CLASSE("etat_courant")
       EVENEMENT("0c0o")
       MENU("Attributs/Graphique/Couleur")
       CHAMP(point, L_couleur3 Init3d)
       L_nombre_positif
       P_flottant
       Extrait
       Edite
       Sauve
       CHAMP(point.x, NOM("r") LABEL("Rouge") DEFAUT("1") Init3d)
       CHAMP(point.y, NOM("v") LABEL("Vert") DEFAUT("1") Init3d)
       CHAMP(point.z, NOM("b") LABEL("Bleu") DEFAUT("1") Init3d)
       NonInit3d NonExtrait DEFAUT("")
       CHAMP_VIRTUEL(L_applique_gl_rapide(applique_gl_couleur, attributs_gl_couleur))
       CHAMP_VIRTUEL(L_applique(applique_couleur))
       CHAMP(gl, L_gl
	     TE_ECRIT(u_hls_trace)
	     TE_ENTREE(u_hls_bouton)
	     TE_MOUVEMENT(u_hls_mouvement_bouton)
	     TE_HAUTEUR(36)
	     Edite
	     BULLE_D_AIDE
	     (
	      "Cette zone représente une palette HLS.\n"
	      "(Hue, Light, Saturation).\n"
	      "La bande 1 permet de choisir la teinte de la couleur.\n"
	      "La bande 2 permet de choisir sa luminosité.\n"
	      "La bande 3 permet de choisir sa saturation (pastel/vif).\n"
	      )
	     )
       CHAMP_VIRTUEL(L_attribut_pov)
       CHAMP_VIRTUEL(L_affiche_gl(affiche_couleur)
		     AFFICHAGE(Different_si_actif)
		     /*
		      * Problème de la gestion automatique des listes GL
		      * L'affichage de la couleur dépend du type d'affichage
		      * de couleur.
		      * Ceci nécessite une réflexion plus poussée
		      */
		     AFFICHAGE(Toujours_different)
		     )
       CHAMP_VIRTUEL(L_tag(clique_palette,lit_palette))
       A_EMPILER(mm.couleur)
       A_EMPILER(mm.couleur_priorite)
       )

CLASSE(couleur_triplet, Couleur,
       SUR_CLASSE("couleur") HERITE_PAS_DES_CHAMPS
       MENU("Attributs/Graphique/Couleur")
       CHAMP(point, L_couleur3 Extrait Edite Sauve Obligatoire
	     TYPE_DANS_FILS(L_triplet))
       CHAMP_VIRTUEL(L_affiche_gl(affiche_couleur)
		     AFFICHAGE(Different_si_actif))
       CHAMP_VIRTUEL(L_applique(applique_couleur))
       CHAMP_VIRTUEL(L_applique_gl_rapide(applique_gl_couleur, attributs_gl_couleur))
       CHAMP(gl, L_gl TE_ECRIT(u_hls_trace) TE_ENTREE(u_hls_bouton))
       CHAMP_VIRTUEL(L_attribut_pov)
       )

/*
 * Diffuse
 */

typedef struct
{
  Etat_courant ec ;
  Flottant valeur ;		/* 0...1 */
} Diffuse ;

static void applique_diffuse(Diffuse *o, const Champ *ch, Booleen *modif)
{
  if ( PLUS_PRIORITAIRE(diffuse) )
    mm.diffuse = o->valeur ;
}

static const char * attributs_gl_materiaux(Couleur *o, const Champ *ch)
{
  return( int_vers_char(GL_LIGHTING_BIT) ) ;
}

static void applique_gl_diffuse(Diffuse *o)
{
  Triplet tmp ;
  GLfloat f[4] ;

  tmp = u_triplet_fois_flottant(&mm.couleur, o->valeur) ;
  f[0] = tmp.x ;
  f[1] = tmp.y ;
  f[2] = tmp.z ;
  f[3] = mm.diffuse+mm.speculaire-mm.refraction-mm.reflexion ;
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE  , f) ;
}

CLASSE(diffuse, Diffuse,
       SUR_CLASSE("etat_courant")
       CHAMP(valeur   ,
	     L_nombre_positif P_flottant DEFAUT("0.6")
	     Extrait Sauve Edite Init3d
	     BULLE_D_AIDE
	     ("Indique la quantité de l'énergie recu par la lampe\n"
	      "qui est diffusée dans toutes les directions par l'objet.\n"
	      )
	     )
       CHAMP_VIRTUEL(L_attribut_pov)
       CHAMP_VIRTUEL(L_applique_gl_rapide(applique_gl_diffuse
					  , attributs_gl_materiaux))
       CHAMP_VIRTUEL(L_applique(applique_diffuse))
       MENU("Attributs/Graphique/Diffuse")
       EVENEMENT("0d0i0f")
       A_EMPILER(mm.diffuse)
       A_EMPILER(mm.diffuse_priorite)
       )

/*
 * Spéculaire
 * Dans POV, la spéculaire ne tient pas compte de la couleur de l'objet.
 */

typedef struct
{
  Etat_courant ec ;
  Flottant valeur ;		/* 0...1 */
} Speculaire ;

static void applique_speculaire(Speculaire *o, const Champ *ch, Booleen *modif)
{
  if ( PLUS_PRIORITAIRE(speculaire) )
    mm.speculaire = o->valeur ;
}

static void applique_gl_speculaire(Speculaire *o)
{
  GLfloat f[4] ;

  f[0] = o->valeur ;
  f[1] = o->valeur ;
  f[2] = o->valeur ;
  f[3] = mm.diffuse+mm.speculaire-mm.refraction-mm.reflexion ;
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR , f) ;
}

CLASSE(speculaire, Speculaire,
       SUR_CLASSE("etat_courant")
       CHAMP(valeur   ,
	     L_nombre_positif P_flottant DEFAUT("0.3")
	     Extrait Sauve Edite Init3d
	     BULLE_D_AIDE
	     ("Indique la quantité de l'énergie recu par la lampe\n"
	      "qui est réfléchie de manière spéculaire\n"
	      "par l'objet.\n"
	      )
	     )
       CHAMP_VIRTUEL(L_attribut_pov)
       CHAMP_VIRTUEL(L_applique(applique_speculaire))
       CHAMP_VIRTUEL(L_applique_gl_rapide(applique_gl_speculaire
					  , attributs_gl_materiaux))
       MENU("Attributs/Graphique/Spéculaire")
       EVENEMENT("0s0p")
       A_EMPILER(mm.speculaire)
       A_EMPILER(mm.speculaire_priorite)
       )

/*
 * Réflexion
 */

typedef struct
{
  Etat_courant ec ;
  Flottant valeur ;		/* 0...1 */
} Reflexion ;

static void applique_reflexion(Reflexion *o, const Champ *ch, Booleen *modif)
{
  if ( PLUS_PRIORITAIRE(reflexion) )
    mm.reflexion = o->valeur ;
}

CLASSE(reflexion, Reflexion,
       SUR_CLASSE("etat_courant")
       CHAMP(valeur   ,
	     L_nombre_positif P_flottant DEFAUT("0")
	     Extrait Sauve Edite Init3d
	     BULLE_D_AIDE
	     ("Pour le lancé de rayons : indique la quantité\n"
	      "de lumière refleté par l'objet.\n"
	      "La valeur 1 indique un mirroir parfait.\n"
	      )
	     )
       CHAMP_VIRTUEL(L_attribut_pov)
       CHAMP_VIRTUEL(L_applique(applique_reflexion))
       MENU("Attributs/Graphique/Réflexion")
       EVENEMENT("0r0e0f0l")
       A_EMPILER(mm.reflexion)
       A_EMPILER(mm.reflexion_priorite)
       )
/*
 * Réfraction
 */

typedef struct
{
  Etat_courant ec ;
  Flottant valeur ;		/* 0...1 */
} Refraction ;

static void applique_refraction(Refraction *o, const Champ *ch, Booleen *modif)
{
  if ( PLUS_PRIORITAIRE(refraction) )
    mm.refraction = o->valeur ;
}

CLASSE(refraction, Refraction,
       SUR_CLASSE("etat_courant")
       CHAMP(valeur   ,
	     L_nombre_positif P_flottant DEFAUT("0")
	     Extrait Sauve Edite Init3d
	     BULLE_D_AIDE
	     ("Pour le lancé de rayons : indique la quantité\n"
	      "de lumière refracté par l'objet.\n"
	      "La valeur 1 indique une transparence parfaite.\n"
	      )
	     )
       CHAMP_VIRTUEL(L_attribut_pov)
       CHAMP_VIRTUEL(L_applique(applique_refraction))
       MENU("Attributs/Graphique/Réfraction")
       EVENEMENT("0r0e0f0r")
       A_EMPILER(mm.refraction)
       A_EMPILER(mm.refraction_priorite)
       )

/*
 * Éclairage ambiant
 */

typedef struct
{
  Etat_courant ec ;
  Triplet valeur ;		/* 0...1 */
} Ambiante ;

static void applique_ambiante(Ambiante *o, const Champ *ch, Booleen *modif)
{
  if ( PLUS_PRIORITAIRE(ambiante) )
    mm.ambiante = o->valeur ;
}

static void applique_gl_ambiante(Ambiante *o)
{
  GLfloat f[4] ;

  f[0] = o->valeur.x ;
  f[1] = o->valeur.y ;
  f[2] = o->valeur.z ;
  f[3] = mm.diffuse+mm.speculaire-mm.refraction-mm.reflexion ;
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT  , f) ;
}

CLASSE(ambiante, Ambiante,
       SUR_CLASSE("etat_courant")
       CHAMP(valeur   ,
	     L_couleur3 DEFAUT("0")
	     Extrait Sauve Edite Init3d
	     BULLE_D_AIDE
	     ("Energie a ajouter en plus de l'énergie apportée\n"
	      "par les différentes lampe.\n"
	      "Lorsqu'il n'y a pas lampe, il ne reste que cette couleur.\n"
	      )
	     )
       CHAMP_VIRTUEL(L_attribut_pov)
       CHAMP_VIRTUEL(L_applique_gl_rapide(applique_gl_ambiante
					  , attributs_gl_materiaux))
       CHAMP_VIRTUEL(L_applique(applique_ambiante))
       MENU("Attributs/Graphique/Ambiante")
       EVENEMENT("0a0m0b")
       A_EMPILER(mm.ambiante)
       A_EMPILER(mm.ambiante_priorite)
       )


/*
 * État de surface
 */

typedef struct
{
  Etat_courant ec ;
  Flottant valeur ;		/* 0...1 */
} Surface ;

static void applique_surface(Surface *o, const Champ *ch, Booleen *modif)
{
  if ( PLUS_PRIORITAIRE(n) )
    mm.n = o->valeur ;
}

static void applique_gl_surface(Surface *o)
{
  glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, o->valeur) ;
}

CLASSE(surface, Surface,
       SUR_CLASSE("etat_courant")
       CHAMP(valeur   ,
	     L_nombre_positif P_flottant DEFAUT("4")
	     Extrait Sauve Edite Init3d
	     BULLE_D_AIDE
	     ("Indique l'état de surface de l'objet.\n"
	      "Plus la valeur est grande, plus l'objet est lisse."
	      )
	     )
       CHAMP_VIRTUEL(L_attribut_pov)
       CHAMP_VIRTUEL(L_applique_gl_rapide(applique_gl_surface
					  , attributs_gl_materiaux))
       CHAMP_VIRTUEL(L_applique(applique_surface))
       MENU("Attributs/Graphique/Surface")
       EVENEMENT("0s0u")
       A_EMPILER(mm.n)
       A_EMPILER(mm.n_priorite)
       )

/*
 * État de surface
 */

typedef struct
{
  Etat_courant ec ;
  Flottant valeur ;		/* 0...1 */
} Indice ;

static void applique_indice(Indice *o, const Champ *ch, Booleen *modif)
{
  if ( PLUS_PRIORITAIRE(indice) )
    mm.indice = o->valeur ;
}

CLASSE(indice, Indice,
       SUR_CLASSE("etat_courant")
       CHAMP(valeur   ,
	     L_nombre_positif P_flottant DEFAUT("1.5")
	     Extrait Sauve Edite Init3d
	     BULLE_D_AIDE
	     ("Dans le cas d'objet transparent (réfraction)\n"
	      "Indice l'indice de réfraction.\n"
	      )
	     )
       CHAMP_VIRTUEL(L_attribut_pov)
       CHAMP_VIRTUEL(L_applique(applique_indice))
       MENU("Attributs/Graphique/Indice")
       EVENEMENT("0i0n")
       A_EMPILER(mm.indice)
       A_EMPILER(mm.indice_priorite)
       )

/*
 * Type d'affichage
 */

struct type_affichage
{
  Etat_courant ec ;
  Menu affichage ;
} ;

static void applique_type_affichage(struct type_affichage *o
				    , const Champ *ch, Booleen *modif)
{
  if ( PLUS_PRIORITAIRE(type_affichage) )
    mm.type_affichage = (Type_affichage)o->affichage.index ;
}

CLASSE(type_affichage, struct type_affichage,
       SUR_CLASSE("etat_courant")
       CHAMP(affichage,
	     L_option_menu
	     TE_TEXTE(
		      "ponctuel\n"
		      "filaire\n"
		      "facette\n"
		      "affichedebug\n"
		      "invisible\n"
		      )
	     DEFAUT("facette")
	     Sauve Edite InitChaine
	     BULLE_D_AIDE
	     ("Type d'affichage des objets.\n"
	      )
	     )
       MENU("Attributs/Graphique/Type affichage")
       EVENEMENT("0t0a")
       CHAMP_VIRTUEL(L_applique(applique_type_affichage))
       A_EMPILER(mm.type_affichage)
       A_EMPILER(mm.type_affichage_priorite)
       CHAMP_VIRTUEL(L_attribut_pov)
       )

/*
 * Type d'affichage
 */

struct epaisseur_trait
{
  Etat_courant ec ;
  Flottant epaisseur_trait ;
} ;

static void applique_epaisseur_trait(struct epaisseur_trait *o
				     , const Champ *ch, Booleen *modif)
{
  if ( PLUS_PRIORITAIRE(epaisseur_trait) )
    mm.epaisseur_trait = o->epaisseur_trait ;
}

CLASSE(epaisseur, struct epaisseur_trait,
       SUR_CLASSE("etat_courant")
       CHAMP(epaisseur_trait,
	     L_nombre P_flottant Sauve Edite Extrait
	     DEFAUT("0.1")
	     BULLE_D_AIDE
	     ("Épaisseur des cylindres pour faire les traits sous POV.\n"
	      )
	     )
       MENU("Attributs/Geométrique/Épaisseur Traits")
       EVENEMENT("0e0p")
       CHAMP_VIRTUEL(L_applique(applique_epaisseur_trait))
       CHAMP_VIRTUEL(L_attribut_pov)
       A_EMPILER(mm.epaisseur_trait)
       A_EMPILER(mm.epaisseur_trait_priorite)
       )

/*
 * Type d'affichage
 */

struct affiche_ligne_de_rappel
{
  Etat_courant ec ;
  Booleen affiche_ligne_de_rappel ;
} ;

static void applique_affiche_ligne_de_rappel(struct affiche_ligne_de_rappel *o
					     , const Champ *ch, Booleen *modif)
{
  if ( PLUS_PRIORITAIRE(affiche_ligne_de_rappel) )
    mm.affiche_ligne_de_rappel = o->affiche_ligne_de_rappel ;
}

CLASSE(affichage_ligne_de_rappel, struct affiche_ligne_de_rappel,
       SUR_CLASSE("etat_courant")
       CHAMP(affiche_ligne_de_rappel,
	     L_booleen Sauve Edite Extrait
	     DEFAUT("1")
	     BULLE_D_AIDE
	     ("Affiche les lignes de rappel 3D des points vers l'origine.\n"
	      )
	     )
       MENU("Attributs/Graphique/Affiche ligne de rappel")
       EVENEMENT("ALR")
       CHAMP_VIRTUEL(L_applique(applique_affiche_ligne_de_rappel))
       A_EMPILER(mm.affiche_ligne_de_rappel)
       A_EMPILER(mm.affiche_ligne_de_rappel_priorite)
       )

/*
 * Désignabilité
 */

struct designable
{
  Etat_courant ec ;
  Booleen designable ;
} ;

static void applique_gl_designabilite(Indice *o)
{
  glLoadName(Faux) ;
}

static void applique_designable(struct designable *o
					     , const Champ *ch, Booleen *modif)
{
  if ( PLUS_PRIORITAIRE(designable) )
    mm.designable = o->designable ;
}

CLASSE(designabilite, struct designable,
       SUR_CLASSE("etat_courant")
       CHAMP(designable,
	     L_booleen Sauve Edite Extrait
	     DEFAUT("1")
	     BULLE_D_AIDE
	     ("Change l'état pour choisir si les objets graphiques suivants.\n"
	      "seront désignable ou non.\n"
	      )
	     )
       MENU("Attributs/Graphique/Désignabilité")
       EVENEMENT("DES")
       CHAMP_VIRTUEL(L_applique(applique_designable))
       CHAMP_VIRTUEL(L_applique_gl(applique_gl_designabilite))
       A_EMPILER(mm.designable)
       A_EMPILER(mm.designable_priorite)
       )
/*
 * Découpage de la palette HLS de l'objet couleur
 */

struct choix_du_decoupage_hls
{
  Etat_courant ec ;
  int decoupage_hls[3] ;
} ;

static void applique_choix_du_decoupage_hls(struct choix_du_decoupage_hls *o
					     , const Champ *ch, Booleen *modif)
{
  if ( PLUS_PRIORITAIRE(decoupage_hls) )
    memcpy(mm.decoupage_hls, o->decoupage_hls, sizeof(o->decoupage_hls)) ;
}

CLASSE(choix_decoupage_hls, struct choix_du_decoupage_hls,
       SUR_CLASSE("etat_courant")
       CHAMP(decoupage_hls,
	     L_point P_triplet_int Sauve Edite Extrait Init3d
	     DEFAUT("12 6 6")
	     BULLE_D_AIDE("Indique le nombre de Teinte/Luminosité/Saturation\n"
			  "différentes affichées quand on affiche la couleur\n"
			  "avec une palette graphique dans R3\n"
			  )
	     )
       MENU("Attributs/Palette/Choix du découpage HLS de la palette couleur")
       EVENEMENT("DH")
       CHAMP_VIRTUEL(L_applique(applique_choix_du_decoupage_hls))
       A_EMPILER(mm.decoupage_hls)
       A_EMPILER(mm.decoupage_hls_priorite)
       )

/*
 * Choix d'affichage pour l'objet couleur
 */

struct choix_du_affichage_palette_couleur
{
  Etat_courant ec ;
  Menu affichage_palette_couleur ;
} ;

static void applique_choix_du_affichage_palette_couleur(struct choix_du_affichage_palette_couleur *o
					     , const Champ *ch, Booleen *modif)
{
  if ( PLUS_PRIORITAIRE(affichage_palette_couleur) )
    mm.affichage_palette_couleur = o->affichage_palette_couleur.index ;
}

CLASSE(choix_affichage_palette_couleur, struct choix_du_affichage_palette_couleur,
       SUR_CLASSE("etat_courant")
       CHAMP(affichage_palette_couleur,
	     L_option_menu
	     TE_TEXTE(
		      "Point de R3\n"
		      "Disque teinte & saturation\n"
		      "Disque teinte & luminosité\n"
		      "Invisible\n"
		      )
	     DEFAUT("Point de R3")
	     Sauve Edite
	     BULLE_D_AIDE
	     ("Choix de la représentation et de la manipulation\n"
	      "graphique de la palette couleur dans R3.\n"
	      "\n"
	      "On choisi la finesse de découpage de la palette\n"
	      "en appliquant un objet 'choix_du_decoupage_hls'\n"
	      )
	     )
       MENU("Attributs/Palette/Choix Palette de couleur")
       EVENEMENT("CP")
       CHAMP_VIRTUEL(L_applique(applique_choix_du_affichage_palette_couleur))
       A_EMPILER(mm.affichage_palette_couleur)
       A_EMPILER(mm.affichage_palette_couleur_priorite)
       )
/*
 * Choix d'offset
 */

struct choix_stl_offset
{
  Etat_courant ec ;
  Flottant offset ;
} ;

static void applique_choix_stl_offset(struct choix_stl_offset *o
				      , const Champ *ch, Booleen *modif)
{
  if ( PLUS_PRIORITAIRE(stl_offset) )
    mm.stl_offset = o->offset ;
}

CLASSE(choix_stl_offset, struct choix_stl_offset,
       SUR_CLASSE("etat_courant")
       CHAMP(offset, L_nombre P_flottant DEFAUT("0.1") Sauve Edite Init3d
	     BULLE_D_AIDE
	     ("Choix de l'offset STL.\n"
	      "Cette valeur ne subit aucune transformation\n"
	      "Si vous agrandissez une sphère, l'offset restera\n"
	      "le même dans l'objet STL\n"
	      "\n"
	      "Si l'offset est nul, l'objet sera plein\n"
	      )
	     )
       MENU("Attributs/STL/Choix offset")
       EVENEMENT("OSTL")
       CHAMP_VIRTUEL(L_applique(applique_choix_stl_offset))
       A_EMPILER(mm.stl_offset)
       A_EMPILER(mm.stl_offset_priorite)
       )








