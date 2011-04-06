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
/* LIB: -lm */
#include <math.h>
#include <GL/gl.h>		/* Car il affiche du GL */
#include <stdlib.h>		/* Pour atoi */
#include "o_objet.h"		/* Car il faut definir un objet */
#include "s_string.h"		/* Utilitaires chaines de caractères */
#include "u_graphique.h"
#include "u_etat_courant.h"
#include "t_geometrie.h"
#include "u_table.h"
#include "u_vue.h"

#define type_affichage globale_etat_courant.type_affichage

static int globale_option_gl[] = { T_PONCTUEL, T_FILAIRE, T_FACETTE
				   ,0 , 0} ;
static int globale_option[] = { T_PONCTUEL
				, T_FILAIRE|T_PONCTUEL
				, T_FACETTE
				, 0 /* Debug */
				, 0 /* Invisible */
} ;

/*
 * Améliorations à faire :
 *   - Être certain du numéro de liste affecté
 *   - Ne pas regénérer tous les segments filaire en POV
 *   - Qualité d'affichage manquante (observateur)
 */

typedef struct
{
  Transfo t ;
} Primitive_Solide ;

CLASSE(solide, Primitive_Solide,
       CHAMP_VIRTUEL(L_solide)
       CHAMP(t, L_transfo P_transfo Edite Extrait Sauve DEFAUT("[(1 0 0)\n(0 1 0)\n(0 0 1)\n(0 0 0)]"))
       MENU("NON")
       )
CLASSE(primitive_solide, Primitive_Solide,
       SUR_CLASSE("solide")
       )
CLASSE(booleenne_solide, Primitive_Solide,
       SUR_CLASSE("solide")
       CHAMP_VIRTUEL(L_solide Obligatoire)
       PAQUET(NB_ACTIFS)
       )

void affichage_solide_debut(Primitive_Solide *ps)
{
  glPushMatrix() ;
  u_change_repere(&ps->t) ;
}

void affichage_solide_fin(Primitive_Solide *ps)
{
  glPopMatrix() ;
  if ( FILS(ps, 0) && (ACTIF(ps) || ACTIF(FILS(ps, 0))) )
    {
      glDisable(GL_DEPTH_TEST) ;
      u_affiche_objet(FILS(ps,0), "") ;
      glEnable(GL_DEPTH_TEST) ;
    }
}

Transfo global_transfo_sauve ;

void stl_solide_debut(Primitive_Solide *ps)
{
  global_transfo_sauve = globale_etat_courant.transfo ;
  globale_etat_courant.transfo = u_produit_transfo(&globale_etat_courant.transfo, &ps->t) ;
}

void stl_solide_fin(Primitive_Solide *ps)
{
  globale_etat_courant.transfo = global_transfo_sauve ;
}

void pov_solide_debut(Primitive_Solide *ps, FILE *f)
{
  fprintf( f, "union {\n" ) ;
}

void pov_solide_fin(Primitive_Solide *ps, FILE *f)
{
    fprintf( f,
	   "matrix <%g, %g, %g,  %g, %g, %g,  %g, %g, %g,  %g, %g, %g> }\n",
	   ps->t.v1.x     , ps->t.v1.y     , ps->t.v1.z,
	   ps->t.v2.x     , ps->t.v2.y     , ps->t.v2.z,
	   ps->t.v3.x     , ps->t.v3.y     , ps->t.v3.z,
	   ps->t.origine.x, ps->t.origine.y, ps->t.origine.z
	   ) ;

}

/*
 *****************************************************************************
 * Creation d'une sphere, de centre origine et de rayon 1
 *****************************************************************************
 */ 
static void points_de_sphere(int nblat, int nblong, Grille *points)
{
  float latitude ;
  float longitude ;
  float d_lat, d_long ;
  Chaine s ;

  s = s_ouvre_ecriture() ;

  d_lat = M_PI/nblat ;
  d_long = 2*M_PI/nblong ;

  s_putchar(&s, '[') ;
  s_putchar(&s, '(') ;
  for( longitude = 0; longitude < 2*M_PI - d_long/2 ; longitude += d_long )
      s_printf(&s, "0 0 -1\n") ;
  s_putchar(&s, ')') ;
  for( latitude = -M_PI/2+d_lat; latitude < M_PI/2 - d_lat/2;latitude += d_lat)
    {
      s_putchar(&s, '(') ;
      for( longitude = 0; longitude < 2*M_PI - d_long/2 ; longitude += d_long )
	{
	  s_printf(&s
		   , "%g %g %g\n"
		   , cos(latitude)*cos(longitude)
		   , cos(latitude)*sin(longitude)
		   , sin(latitude)
		   ) ;
	}
      s_putchar(&s, ')') ;
    }
  s_putchar(&s, '(') ;
  for( longitude = 0; longitude < 2*M_PI - d_long/2 ; longitude += d_long )
      s_printf(&s, "0 0 1\n") ;
  s_putchar(&s, ')') ;
  s_putchar(&s, ']') ;
  MET_A_0(points) ;
  codec_sauve_texte(codec_grille_triplet, points, s_char(&s)) ;
  s_ferme(&s) ;
}

static int decoupage(const char *v)
{
  static int nb[] = {    3,4,8   ,16,    32,64,96 } ;
  int i ;
  const char *v2 ;

  v2 = strchr(v, ' ') ;
  if ( v2==NULL )
    i = atol(v) ;
  else
    i = atol(v2) ;

  i -= 2 ;

  if ( i<0 )
    i = 0 ;

  if ( i>=TAILLE_TABLE(nb) )
    i = TAILLE_TABLE(nb)-1 ;

  return(nb[i]) ;
}

static void affiche_sphere(void *o, const Champ *c, const char*v)
{
  int i ;
  Grille g ;

  affichage_solide_debut(o) ;
  i = decoupage(v) ;
  points_de_sphere(i, i, &g) ;
  t_grille_general(&g
		   , &infos_codec_grille_triplet
		   , globale_option_gl[type_affichage]
		   | (ACTIF(o)?0:T_LISSAGE)
		   | T_CULL_FACE
		   | T_DEGENERE_HAUT
		   | T_DEGENERE_BAS
		   | T_RACCORDE_GD
		   , "5"
		   , &g
		   , &infos_codec_grille_triplet
		   ) ;
  codec_libere(codec_grille_triplet, &g) ;
  affichage_solide_fin(o) ;
}

static void ecrit_pov_sphere(void *o, const Champ *c, const char *v)
{
  Grille g ;
  int i ;
  FILE *f ;

  f = (FILE*)atol(v) ;
  pov_solide_debut(o, f) ;

  if ( type_affichage == Facette )
    fprintf( f, "sphere{<0, 0, 0>,1}\n") ;
  else
    {
      i = decoupage(v) ;
      points_de_sphere(i, i, &g) ;
      t_grille_pov_general(&g, &infos_codec_grille_triplet
			   , globale_option[type_affichage]
			     | T_DEGENERE_HAUT
			     | T_DEGENERE_BAS
			     | T_RACCORDE_GD
			   , f, NULL, NULL) ;
      codec_libere(codec_grille_triplet, &g) ;
    }
  pov_solide_fin(o, f) ;
}

static void ecrit_stl_sphere(void *o, const Champ *c, const char *v)
{
  Grille g ;
  int i ;
  FILE *f ;

  stl_solide_debut(o) ;
  f = (FILE*)atol(v) ;

  i = decoupage(v) ;

  points_de_sphere(i, i, &g) ;
  t_grille_stl_general(&g, &infos_codec_grille_triplet
	       , globale_option[type_affichage]
	       | T_DEGENERE_HAUT
	       | T_DEGENERE_BAS
	       | T_RACCORDE_GD
	       | T_CULL_FACE
	       , f) ;
  codec_libere(codec_grille_triplet, &g) ;
  stl_solide_fin(o) ;
}

static void ecrit_obj_sphere(void *o, const Champ *c, const char *v)
{
  Grille g ;
  int i ;
  FILE *f ;

  stl_solide_debut(o) ;
  f = (FILE*)atol(v) ;

  i = decoupage(v) ;

  points_de_sphere(i, i, &g) ;
  t_grille_obj_general(&g, &infos_codec_grille_triplet
	       , globale_option[type_affichage]
	       | T_DEGENERE_HAUT
	       | T_DEGENERE_BAS
	       | T_RACCORDE_GD
	       | T_CULL_FACE
	       , f) ;
  codec_libere(codec_grille_triplet, &g) ;
  stl_solide_fin(o) ;
}

CLASSE(sphere, Primitive_Solide,
       SUR_CLASSE("primitive_solide")
       EVENEMENT("3s3p")
       BULLE_D_AIDE("Sphère de centre Origine et de rayon unitaire.\n")
       MENU("Figures/3d/Sphère")
       CHAMP_VIRTUEL(L_affiche_gl(affiche_sphere)
		     AFFICHAGE(
			       Different_si_actif
			       |Different_suivant_type_affichage
			       |Different_suivant_qualite))
       CHAMP_VIRTUEL(L_affiche_pov(ecrit_pov_sphere))
       CHAMP_VIRTUEL(L_affiche_stl(ecrit_stl_sphere))
       CHAMP_VIRTUEL(L_affiche_obj(ecrit_obj_sphere))
       )

/*
 *****************************************************************************
 * Creation cylindre/cone, hauteur entre 0 et 1, rayon r0 et r1
 *****************************************************************************
 */ 

static void points_de_cylindre_cone(int nblong
				    , Grille *points, Grille *normales
				    , double r0, double r1)
{
  float longitude ;
  int latitude ;
  float d_long ;
  Chaine pt, no ;

  pt = s_ouvre_ecriture() ;
  no = s_ouvre_ecriture() ;

  d_long = 2*M_PI/nblong ;

  s_putchar(&pt, '[') ;
  s_putchar(&no, '[') ;
  for( latitude = 0 ; latitude < 2 ; latitude++ )
    {
      s_putchar(&pt, '(') ;
      s_putchar(&no, '(') ;
      for( longitude = 0; longitude < 2*M_PI - d_long/2 ; longitude += d_long )
	{
	  s_printf(&pt
		   , "%g %g %d\n"
		   , cos(longitude)*(latitude==0 ? r0 : r1 )
		   , sin(longitude)*(latitude==0 ? r0 : r1 )
		   , latitude
		   ) ;
	  s_printf(&no
		   , "%g %g %g\n"
		   , cos(longitude)
		   , sin(longitude)
		   , (r0-r1)
		   ) ;
	}
      s_putchar(&pt, ')') ;
      s_putchar(&no, ')') ;
    }
  s_putchar(&pt, ']') ;
  s_putchar(&no, ']') ;
  MET_A_0(points) ;
  MET_A_0(normales) ;
  codec_sauve_texte(codec_grille_triplet,points  ,s_char(&pt));
  codec_sauve_texte(codec_grille_triplet,normales,s_char(&no));
  s_ferme(&pt) ;
  s_ferme(&no) ;
}

/*
 * Cylindre
 */

static void affiche_cylindre(void *o, const Champ *c, const char*v)
{
  int i ;
  Grille points, normales ;

  affichage_solide_debut(o) ;
  i = decoupage(v) ;
  points_de_cylindre_cone(i, &points, &normales, 1., 1.) ;
  t_grille_general(&points
		   , &infos_codec_grille_triplet
		   , globale_option_gl[type_affichage]
		   | (ACTIF(o)?0:T_LISSAGE)
		   | T_CULL_FACE
		   | T_FACETTE_HAUT
		   | T_FACETTE_BAS
		   | T_RACCORDE_GD
		   , "5"
		   , &normales
		   , &infos_codec_grille_triplet
		   ) ;
  codec_libere(codec_grille_triplet, &points) ;
  codec_libere(codec_grille_triplet, &normales) ;
  affichage_solide_fin(o) ;
}

static void ecrit_pov_cylindre(void *o, const Champ *c, const char *v)
{
  Grille points, normales ;
  int i ;
  FILE *f ;

  f = (FILE*)atol(v) ;
  pov_solide_debut(o, f) ;

  if ( type_affichage == Facette )
    fprintf( f, "cylinder{<0,0,0>,<0,0,1>,1}\n") ;
  else
    {
      i = decoupage(v) ;
      points_de_cylindre_cone(i, &points, &normales, 1., 1.) ;
      t_grille_pov_general(&points, &infos_codec_grille_triplet
			   , globale_option[type_affichage]
			     | T_FACETTE_HAUT
			     | T_FACETTE_BAS
		             | T_RACCORDE_GD
			   ,f, NULL, NULL) ;
      codec_libere(codec_grille_triplet, &points) ;
      codec_libere(codec_grille_triplet, &normales) ;
    }
  pov_solide_fin(o, f) ;
}

static void ecrit_stl_cylindre(void *o, const Champ *c, const char *v)
{
  Grille points, normales ;
  int i ;
  FILE *f ;

  stl_solide_debut(o) ;
  f = (FILE*)atol(v) ;
  i = decoupage(v) ;

  points_de_cylindre_cone(i, &points, &normales, 1., 1.) ;
  t_grille_stl_general(&points, &infos_codec_grille_triplet
	       , globale_option[type_affichage]
	       | T_FACETTE_HAUT
	       | T_FACETTE_BAS
	       | T_RACCORDE_GD
	       | T_CULL_FACE
	       ,f) ;
  codec_libere(codec_grille_triplet, &points) ;
  codec_libere(codec_grille_triplet, &normales) ;
  stl_solide_fin(o) ;
}

static void ecrit_obj_cylindre(void *o, const Champ *c, const char *v)
{
  Grille points, normales ;
  int i ;
  FILE *f ;

  stl_solide_debut(o) ;
  f = (FILE*)atol(v) ;
  i = decoupage(v) ;

  points_de_cylindre_cone(i, &points, &normales, 1., 1.) ;
  t_grille_obj_general(&points, &infos_codec_grille_triplet
	       , globale_option[type_affichage]
	       | T_FACETTE_HAUT
	       | T_FACETTE_BAS
	       | T_RACCORDE_GD
	       | T_CULL_FACE
	       ,f) ;
  codec_libere(codec_grille_triplet, &points) ;
  codec_libere(codec_grille_triplet, &normales) ;
  stl_solide_fin(o) ;
}

CLASSE(cylindre, Primitive_Solide,
       SUR_CLASSE("primitive_solide")
       EVENEMENT("3c3y")
       BULLE_D_AIDE("Cylindre d'axe Z de hauteur entre 0 et 1.\n")
       MENU("Figures/3d/Cylindre")
       CHAMP_VIRTUEL(L_affiche_gl(affiche_cylindre)
		     AFFICHAGE(
			       Different_si_actif
			       |Different_suivant_type_affichage
			       |Different_suivant_qualite))
       CHAMP_VIRTUEL(L_affiche_pov(ecrit_pov_cylindre))
       CHAMP_VIRTUEL(L_affiche_stl(ecrit_stl_cylindre))
       CHAMP_VIRTUEL(L_affiche_obj(ecrit_obj_cylindre))
       )

/*
 * Cone
 */

static void affiche_cone(void *o, const Champ *c, const char*v)
{
  int i ;
  Grille points, normales ;

  affichage_solide_debut(o) ;
  i = decoupage(v) ;
  points_de_cylindre_cone(i, &points, &normales, 0., 1.) ;
  t_grille_general(&points
		   , &infos_codec_grille_triplet
		   , globale_option_gl[type_affichage]
		   | (ACTIF(o)?0:T_LISSAGE)
		   | T_CULL_FACE
		   | T_DEGENERE_BAS
		   | T_FACETTE_HAUT
		   | T_RACCORDE_GD
		   , "5"
		   , &normales
		   , &infos_codec_grille_triplet
		   ) ;
  codec_libere(codec_grille_triplet, &points) ;
  codec_libere(codec_grille_triplet, &normales) ;
  affichage_solide_fin(o) ;
}

static void ecrit_pov_cone(void *o, const Champ *c, const char *v)
{
  Grille points, normales ;
  int i ;
  FILE *f ;

  f = (FILE*)atol(v) ;
  pov_solide_debut(o, f) ;

  if ( type_affichage == Facette )
    fprintf( f, "cone{<0,0,0>,0,<0,0,1>,1}\n") ;
  else
    {
      i = decoupage(v) ;
      points_de_cylindre_cone(i, &points, &normales, 0., 1.) ;
      t_grille_pov_general(&points, &infos_codec_grille_triplet
			   , globale_option[type_affichage]
			     | T_DEGENERE_BAS
			     | T_FACETTE_HAUT
		             | T_RACCORDE_GD
			   , f, NULL, NULL) ;
      codec_libere(codec_grille_triplet, &points) ;
      codec_libere(codec_grille_triplet, &normales) ;
    }
  pov_solide_fin(o, f) ;
}

static void ecrit_stl_cone(void *o, const Champ *c, const char *v)
{
  Grille points, normales ;
  int i ;
  FILE *f ;

  stl_solide_debut(o) ;
  f = (FILE*)atol(v) ;
  i = decoupage(v) ;
  points_de_cylindre_cone(i, &points, &normales, 0., 1.) ;
  t_grille_stl_general(&points, &infos_codec_grille_triplet
	       , globale_option[type_affichage]
	       | T_DEGENERE_BAS
	       | T_FACETTE_HAUT
	       | T_RACCORDE_GD
	       | T_CULL_FACE
	       , f) ;
  codec_libere(codec_grille_triplet, &points) ;
  codec_libere(codec_grille_triplet, &normales) ;
  stl_solide_fin(o) ;
}

static void ecrit_obj_cone(void *o, const Champ *c, const char *v)
{
  Grille points, normales ;
  int i ;
  FILE *f ;

  stl_solide_debut(o) ;
  f = (FILE*)atol(v) ;
  i = decoupage(v) ;
  points_de_cylindre_cone(i, &points, &normales, 0., 1.) ;
  t_grille_obj_general(&points, &infos_codec_grille_triplet
	       , globale_option[type_affichage]
	       | T_DEGENERE_BAS
	       | T_FACETTE_HAUT
	       | T_RACCORDE_GD
	       | T_CULL_FACE
	       , f) ;
  codec_libere(codec_grille_triplet, &points) ;
  codec_libere(codec_grille_triplet, &normales) ;
  stl_solide_fin(o) ;
}

CLASSE(cone, Primitive_Solide,
       SUR_CLASSE("primitive_solide")
       EVENEMENT("3c3o")
       BULLE_D_AIDE("Cone d'axe Z de hauteur entre 0 et 1. (pointe en 0)\n")
       MENU("Figures/3d/Cône")
       CHAMP_VIRTUEL(L_affiche_gl(affiche_cone)
		     AFFICHAGE(
			       Different_si_actif
			       |Different_suivant_type_affichage
			       |Different_suivant_qualite))
       CHAMP_VIRTUEL(L_affiche_pov(ecrit_pov_cone))
       CHAMP_VIRTUEL(L_affiche_stl(ecrit_stl_cone))
       CHAMP_VIRTUEL(L_affiche_obj(ecrit_obj_cone))
       )

/*
 *****************************************************************************
 * Creation TORE
 *****************************************************************************
 */ 

static void points_de_tore(int nblat, int nblong, double rayon
			   , Grille *points, Grille *normales)
{
  float latitude ;
  float longitude ;
  float d_lat, d_long ;
  Chaine s, sn ;
  Triplet n, p, r, verticale ;

  s = s_ouvre_ecriture() ;
  sn = s_ouvre_ecriture() ;

  d_lat = 2*M_PI/nblat ;
  d_long = 2*M_PI/nblong ;
  verticale.x = 0 ;
  verticale.y = 0 ;
  verticale.z = 1 ;

  s_putchar(&s, '[') ;
  s_putchar(&sn, '[') ;
  for( latitude = 0 ; latitude < 2*M_PI - d_lat/2 ; latitude += d_lat )
    {
      s_putchar(&s, '(') ;
      n.x = rayon*cos(latitude) ;
      n.y = 0 ;
      n.z = rayon * sin(latitude) ;
      p = n ;
      p.x++ ;
      for( longitude = 0; longitude < 2*M_PI - d_long/2 ; longitude += d_long )
	{
	  r = u_rotation_triplet(&p, &verticale, longitude) ;
	  s_printf(&s, "%g %g %g\n", r.x, r.y, r.z) ;
	  r = u_rotation_triplet(&n, &verticale, longitude) ;
	  s_printf(&sn, "%g %g %g\n", r.x, r.y, r.z) ;
	}
      s_putchar(&s, ')') ;
      s_putchar(&sn, ')') ;
    }
  s_putchar(&s, ']') ;
  s_putchar(&sn, ']') ;
  MET_A_0(points) ;
  MET_A_0(normales) ;
  codec_sauve_texte(codec_grille_triplet, points, s_char(&s)) ;
  codec_sauve_texte(codec_grille_triplet, normales, s_char(&sn)) ;
  s_ferme(&s) ;
  s_ferme(&sn) ;
}

typedef struct
{
  Primitive_Solide ps ;
  Flottant rayon ;
  int gl_list ;
} Tore ;

static void ecrit_pov_tore(Tore *o, const Champ *c, const char *v)
{
  Grille points, normales ;
  int i ;
  FILE *f ;

  f = (FILE*)atol(v) ;
  pov_solide_debut(&o->ps, f) ;

  if ( type_affichage == Facette )
    fprintf( f, "torus{1,%g\nrotate -90*x}\n", o->rayon) ;
  else
    {
      i = decoupage(v) ;
      points_de_tore(i, i*2, o->rayon, &points, &normales) ;
      t_grille_pov_general(&points, &infos_codec_grille_triplet
			   , globale_option[type_affichage]
		             | T_RACCORDE_GD
		             | T_RACCORDE_HB
			   ,f
			   , &normales
			   , &infos_codec_grille_triplet) ;
      codec_libere(codec_grille_triplet, &points) ;
      codec_libere(codec_grille_triplet, &normales) ;
    }
  pov_solide_fin(&o->ps, f) ;
}


static void ecrit_stl_tore(Tore *o, const Champ *c, const char *v)
{
  Grille points, normales ;
  int i ;
  FILE *f ;

  stl_solide_debut(&o->ps) ;
  f = (FILE*)atol(v) ;
  i = decoupage(v) ;
  points_de_tore(i, i*2, o->rayon, &points, &normales) ;
  t_grille_stl_general(&points, &infos_codec_grille_triplet
		       , globale_option[type_affichage]
		       | T_RACCORDE_GD
		       | T_RACCORDE_HB
		       | T_CULL_FACE
		       ,f) ;
  codec_libere(codec_grille_triplet, &points) ;
  codec_libere(codec_grille_triplet, &normales) ;
  stl_solide_fin(&o->ps) ;
}

static void ecrit_obj_tore(Tore *o, const Champ *c, const char *v)
{
  Grille points, normales ;
  int i ;
  FILE *f ;

  stl_solide_debut(&o->ps) ;
  f = (FILE*)atol(v) ;
  i = decoupage(v) ;
  points_de_tore(i, i*2, o->rayon, &points, &normales) ;
  t_grille_obj_general(&points, &infos_codec_grille_triplet
		       , globale_option[type_affichage]
		       | T_RACCORDE_GD
		       | T_RACCORDE_HB
		       | T_CULL_FACE
		       ,f) ;
  codec_libere(codec_grille_triplet, &points) ;
  codec_libere(codec_grille_triplet, &normales) ;
  stl_solide_fin(&o->ps) ;
}


static void affiche_tore(Tore *o, const Champ *c, const char*v)
{
  int i ;
  Grille points, normales ;

  affichage_solide_debut(&o->ps) ;
  i = decoupage(v) ;
  points_de_tore(i, i*2, o->rayon, &points, &normales) ;
  t_grille_general(&points
		   , &infos_codec_grille_triplet
		   , globale_option_gl[type_affichage]
		   | (ACTIF(o)?0:T_LISSAGE)
		   | T_CULL_FACE
		   | T_RACCORDE_GD
		   | T_RACCORDE_HB
		   , "5"
		   , &normales
		   , &infos_codec_grille_triplet
		   ) ;
  codec_libere(codec_grille_triplet, &points) ;
  codec_libere(codec_grille_triplet, &normales) ;
  affichage_solide_fin(&o->ps) ;
}
/*
static void destruction_tore(Tore *o)
{
  if ( o->gl_list )
    glDeleteLists(o->gl_list, 1000) ;
}

static void changement_tore(Tore *o)
{
  if ( CHAMP_CHANGE(o, rayon) || CREATION(o) )
    {
      destruction_tore(o) ;
      o->gl_list = (int)o ;
    }
}
*/
CLASSE(tore, Tore,
       SUR_CLASSE("primitive_solide")
       EVENEMENT("3t3o")
       BULLE_D_AIDE("Tore d'axe vertical de grand rayon 1\n")
       MENU("Figures/3d/Tore")
       CHAMP(rayon, L_nombre P_flottant Extrait Edite Sauve Init3d DEFAUT(".1")) ;
       CHAMP_VIRTUEL(L_affiche_gl(affiche_tore)
		     AFFICHAGE(Different_si_actif
			       |Different_suivant_type_affichage
			       |Different_suivant_qualite))
       CHAMP_VIRTUEL(L_affiche_pov(ecrit_pov_tore))
       CHAMP_VIRTUEL(L_affiche_stl(ecrit_stl_tore))
       CHAMP_VIRTUEL(L_affiche_obj(ecrit_obj_tore))
       /*
       CHANGEMENT(changement_tore)
       DESTRUCTION(destruction_tore)
       */
       )


/*
 *****************************************************************************
 * Creation cylindre/cone, hauteur entre 0 et 1, rayon r0 et r1
 *****************************************************************************
 */

static void points_de_cube(Grille *points)
{
  MET_A_0(points) ;
  codec_sauve_texte(codec_grille_triplet,
			      points  ,
			      "[(0 0 0\n"
			      "1 0 0\n"
			      "1 1 0\n"
			      "0 1 0\n"
			      ")(0 0 1\n"
			      "1 0 1\n"
			      "1 1 1\n"
			      "0 1 1\n"
			      ")]"
			      ) ;
}  


static void affiche_cube(void *o, const Champ *c, const char*v)
{
  Grille points ;

  affichage_solide_debut(o) ;
  points_de_cube(&points) ;
  t_grille_general(&points
		   , &infos_codec_grille_triplet
		   , globale_option_gl[type_affichage]
		   | T_CULL_FACE
		   | T_FACETTE_HAUT
		   | T_FACETTE_BAS
		   | T_RACCORDE_GD
		   , "5"
		   , NULL
		   , NULL
		   ) ;
  codec_libere(codec_grille_triplet, &points) ;
  affichage_solide_fin(o) ;
}

static void ecrit_pov_cube(void *o, const Champ *c, const char *v)
{
  Grille points ;
  FILE *f ;

  f = (FILE*)atol(v) ;
  pov_solide_debut(o, f) ;

  if ( type_affichage == Facette )
    fprintf( f, "box{<0,0,0>,<1,1,1>}\n") ;
  else
    {
      points_de_cube(&points) ;
      t_grille_pov_general(&points, &infos_codec_grille_triplet
			   , globale_option[type_affichage]
			     | T_FACETTE_HAUT
			     | T_FACETTE_BAS
		             | T_RACCORDE_GD
			   ,f, NULL, NULL) ;
      codec_libere(codec_grille_triplet, &points) ;
    }
  pov_solide_fin(o, f) ;
}

static void ecrit_stl_cube(void *o, const Champ *c, const char *v)
{
  Grille points ;
  FILE *f ;

  stl_solide_debut(o) ;
  f = (FILE*)atol(v) ;
  points_de_cube(&points) ;
  t_grille_stl_general(&points, &infos_codec_grille_triplet
	       , globale_option[type_affichage]
	       | T_FACETTE_HAUT
	       | T_FACETTE_BAS
	       | T_RACCORDE_GD
	       | T_CULL_FACE
	       ,f) ;
  codec_libere(codec_grille_triplet, &points) ;
  stl_solide_fin(o) ;
}

static void ecrit_obj_cube(void *o, const Champ *c, const char *v)
{
  Grille points ;
  FILE *f ;

  stl_solide_debut(o) ;
  f = (FILE*)atol(v) ;
  points_de_cube(&points) ;
  t_grille_obj_general(&points, &infos_codec_grille_triplet
	       , globale_option[type_affichage]
	       | T_FACETTE_HAUT
	       | T_FACETTE_BAS
	       | T_RACCORDE_GD
	       | T_CULL_FACE
	       ,f) ;
  codec_libere(codec_grille_triplet, &points) ;
  stl_solide_fin(o) ;
}

CLASSE(cube, Primitive_Solide,
       SUR_CLASSE("primitive_solide")
       EVENEMENT("3c3u")
       BULLE_D_AIDE("Cube [0-1]x[0-1]x[0-1].\n")
       MENU("Figures/3d/Cube")
       CHAMP_VIRTUEL(L_affiche_gl(affiche_cube)
		     AFFICHAGE(
			       Different_suivant_type_affichage
			       )
		     )
       CHAMP_VIRTUEL(L_affiche_pov(ecrit_pov_cube))
       CHAMP_VIRTUEL(L_affiche_stl(ecrit_stl_cube))
       CHAMP_VIRTUEL(L_affiche_obj(ecrit_obj_cube))
       )

/*
 *****************************************************************************
 * Super sphere
 *****************************************************************************
 */

/*
 * super sphere
 */

void super_sphere_point(float lat, float lon
			, float a, float b, float c
			, Triplet *pt, Triplet *nor
			)
{
  float x, y, z ;

  a = fabs(a) ;
  b = fabs(b) ;
  c = fabs(c) ;
  if ( a < 0.0001 || b < 0.0001 || c < 0.0001 )
    {
      u_annule_triplet(pt) ;
      *nor = *pt ;
      nor->z = 1 ;
      return ;
    }

  x = cos(lon)*cos(lat) ;
  y = sin(lon)*cos(lat) ;
  z = sin(lat) ;

  if ( x > -1e-4 && x < 1e-4 )
    {
      x = 0 ;
    }
  if ( y > -1e-4 && y < 1e-4 )
    {
      y = 0 ;
    }
  if ( z > 1-1e-4 )
    {
      z = 1 ;
    }
  if ( z < -1+1e-4 )
    {
      z = -1 ;
    }

  if ( x > 0 )
    {
      pt->x = pow(x, 2./a) ;
      nor->x = a * pow(pt->x, a-1) ;
    }
  else
    {
      pt->x = -pow(-x, 2./a) ;
      nor->x = -a * pow(-pt->x, a-1) ;
    }

  if ( y > 0 )
    {
      pt->y = pow(y, 2./b) ;
      nor->y = b * pow(pt->y, b-1) ;
    }
  else
    {
      pt->y = -pow(-y, 2./b) ;
      nor->y = -b * pow(-pt->y, b-1) ;
    }
  if ( z > 0 )
    {
      pt->z = pow(z, 2./c) ;
      nor->z = c * pow(pt->z, c-1) ;
    }
  else
    {
      pt->z = -pow(-z, 2./c) ;
      nor->z = -c * pow(-pt->z, c-1) ;
    }
  u_normalise_triplet(nor) ;
}

void points_super_sphere(Grille_triplet *g, Grille_triplet *gn 
			 , float a, float b, float c
			 , int nb_lat, int nb_lon)
{
  float d_lat, d_lon ;
  int i, j ;
  struct informations_table it, itn ;

  nb_lat |= 1 ; /* Doit être impair */
  nb_lon &= ~3 ; /* Doit être multiple de 4 */

  d_lat = M_PI/nb_lat ;
  d_lon = 2*M_PI/nb_lon ;

  MET_A_0(g) ;
  it = codec_informations_table(&infos_codec_grille_triplet, g) ;
  codec_change_taille_table( &it, nb_lat+1, nb_lon+1) ;

  MET_A_0(gn) ;
  itn = codec_informations_table(&infos_codec_grille_triplet, gn) ;
  codec_change_taille_table( &itn, nb_lat+1, nb_lon+1) ;

  for( j=0; j<=nb_lat; j++)
    {
      for(i=0; i<=nb_lon; i++)
	{
	  super_sphere_point(-M_PI/2+j*d_lat
			     , i*d_lon + d_lon/2
			     , a, b, c
			     , &g->grille[j][i]
			     , &gn->grille[j][i]
			     ) ;
	}
    }
}


struct super_solide
{
  Primitive_Solide ps ;
  Triplet exposant ;
} ;

static void affiche_super_sphere(struct super_solide *o, const Champ *c, const char*v)
{
  int i ;
  Grille_triplet g, gn ;

  affichage_solide_debut(&o->ps) ;
  i = decoupage(v) ;

  points_super_sphere(&g, &gn, o->exposant.x, o->exposant.y, o->exposant.z
		      , i, i) ;
  t_grille_general(&g
		   , &infos_codec_grille_triplet
		   , globale_option_gl[type_affichage]
		   | 0
		   | T_CULL_FACE
		   | T_DEGENERE_HAUT
		   | T_DEGENERE_BAS
		   | T_RACCORDE_GD
		   | ( ACTIF(o) ? 0 : T_LISSAGE )
		   , "5"
		   , &gn
		   , &infos_codec_grille_triplet
		   ) ;
  codec_libere(codec_grille_triplet, &g) ;
  codec_libere(codec_grille_triplet, &gn) ;
  affichage_solide_fin(&o->ps) ;
}

static void ecrit_pov_super_sphere(struct super_solide  *o, const Champ *c, const char *v)
{
  Grille_triplet g, gn ;
  int i ;
  FILE *f ;

  f = (FILE*)atol(v) ;
  pov_solide_debut(&o->ps, f) ;

  i = decoupage(v) ;
  points_super_sphere(&g, &gn, o->exposant.x, o->exposant.y, o->exposant.z, i, i) ;
  t_grille_pov_general(&g, &infos_codec_grille_triplet
			   , globale_option[type_affichage]
			     | T_DEGENERE_HAUT
			     | T_DEGENERE_BAS
			     | T_RACCORDE_GD
			   , f, NULL, NULL) ;
  codec_libere(codec_grille_triplet, &g) ;
  codec_libere(codec_grille_triplet, &gn) ;
  pov_solide_fin(&o->ps, f) ;
}

static void ecrit_stl_super_sphere(struct super_solide *o, const Champ *c, const char *v)
{
  Grille_triplet g, gn ;
  int i ;
  FILE *f ;

  stl_solide_debut(&o->ps) ;
  f = (FILE*)atol(v) ;

  i = decoupage(v) ;
  points_super_sphere(&g, &gn, o->exposant.x, o->exposant.y, o->exposant.z, i, i) ;
  t_grille_stl_general(&g, &infos_codec_grille_triplet
	       , globale_option[type_affichage]
	       | T_DEGENERE_HAUT
	       | T_DEGENERE_BAS
	       | T_RACCORDE_GD
	       | T_CULL_FACE
	       , f) ;
  codec_libere(codec_grille_triplet, &g) ;
  codec_libere(codec_grille_triplet, &gn) ;
  stl_solide_fin(&o->ps) ;
}

static void ecrit_obj_super_sphere(struct super_solide *o, const Champ *c, const char *v)
{
  Grille_triplet g, gn ;
  int i ;
  FILE *f ;

  stl_solide_debut(&o->ps) ;
  f = (FILE*)atol(v) ;

  i = decoupage(v) ;
  points_super_sphere(&g, &gn, o->exposant.x, o->exposant.y, o->exposant.z, i, i) ;
  t_grille_obj_general(&g, &infos_codec_grille_triplet
	       , globale_option[type_affichage]
	       | T_DEGENERE_HAUT
	       | T_DEGENERE_BAS
	       | T_RACCORDE_GD
	       | T_CULL_FACE
	       , f) ;
  codec_libere(codec_grille_triplet, &g) ;
  codec_libere(codec_grille_triplet, &gn) ;
  stl_solide_fin(&o->ps) ;
}


CLASSE(super_sphere, struct super_solide,
       SUR_CLASSE("primitive_solide")
       CHAMP(exposant, L_triplet DEFAUT("3 4 5") Edite Extrait Sauve Init3d )
       CHAMP_VIRTUEL(L_affiche_gl(affiche_super_sphere)
		     AFFICHAGE(Different_suivant_type_affichage|
			       Different_suivant_qualite))
       CHAMP_VIRTUEL(L_affiche_pov(ecrit_pov_super_sphere))
       CHAMP_VIRTUEL(L_affiche_stl(ecrit_stl_super_sphere))
       CHAMP_VIRTUEL(L_affiche_obj(ecrit_obj_super_sphere))
       MENU("Figures/3d/Super Sphère")
       EVENEMENT("ShftCtrl+SUS")
       )
/*
 *****************************************************************************
 * BOOLEENNE
 *****************************************************************************
 */ 

static void affiche_union_solide(void *o, const Champ *c, const char*v)
{
  affichage_solide_debut(o) ;
  POUR_LES_FILS(o, f, u_affiche_objet(f, v) ; ) ; 
  affichage_solide_fin(o) ;
}


static void ecrit_pov_union_solide(void *o, const Champ *c, const char *v)
{
  FILE *f ;

  f = (FILE*)atol(v) ;
  pov_solide_debut(o, f) ;
  fprintf( f, "union {\n") ;
  POUR_LES_FILS(o,f, c_sauve_type(f, &infos_L_affiche_pov, v) ; ) ;
  fprintf( f, "}\n") ;
  pov_solide_fin(o, f) ;
}

static void ecrit_stl_union_solide(void *o, const Champ *c, const char *v)
{
  stl_solide_debut(o) ;
  POUR_LES_FILS(o,f, c_sauve_type(f, &infos_L_affiche_stl, v) ; ) ;
  stl_solide_fin(o) ;
}

static void ecrit_obj_union_solide(void *o, const Champ *c, const char *v)
{
  stl_solide_debut(o) ;
  POUR_LES_FILS(o,f, c_sauve_type(f, &infos_L_affiche_obj, v) ; ) ;
  stl_solide_fin(o) ;
}

CLASSE(union_solide, Primitive_Solide,
       SUR_CLASSE("booleenne_solide")
       EVENEMENT("3u3s")
       BULLE_D_AIDE("Union Solide\n")
       MENU("Figures/3d/Union")
       CHAMP_VIRTUEL(L_affiche_gl(affiche_union_solide)
		     AFFICHAGE(Toujours_different))
       CHAMP_VIRTUEL(L_affiche_pov(ecrit_pov_union_solide))
       CHAMP_VIRTUEL(L_affiche_stl(ecrit_stl_union_solide))
       CHAMP_VIRTUEL(L_affiche_obj(ecrit_obj_union_solide))
       )


static void ecrit_pov_intersection_solide(void *o,const Champ *c,const char *v)
{
  FILE *f ;

  f = (FILE*)atol(v) ;
  pov_solide_debut(o, f) ;
  fprintf( f, "intersection {\n") ;
  POUR_LES_FILS(o,f, c_sauve_type(f, &infos_L_affiche_pov, v) ; ) ;
  fprintf( f, "}\n") ;
  pov_solide_debut(o, f) ;
}
CLASSE(intersection_solide, Primitive_Solide,
       SUR_CLASSE("booleenne_solide")
       EVENEMENT("3i3s")
       BULLE_D_AIDE("Intersection Solide\n")
       MENU("Figures/3d/Intersection")
       CHAMP_VIRTUEL(L_affiche_gl(affiche_union_solide)
		     AFFICHAGE(Toujours_different))
       CHAMP_VIRTUEL(L_affiche_pov(ecrit_pov_intersection_solide))
       )


static void ecrit_pov_soustraction_solide(void *o,const Champ *c,const char *v)
{
  FILE *f ;

  f = (FILE*)atol(v) ;
  pov_solide_debut(o, f) ;
  fprintf( f, "difference {\n") ;
  POUR_LES_FILS(o,f, c_sauve_type(f, &infos_L_affiche_pov, v) ; ) ;
  fprintf( f, "}\n") ;
  pov_solide_fin(o, f) ;
}
CLASSE(soustraction_solide, Primitive_Solide,
       SUR_CLASSE("booleenne_solide")
       EVENEMENT("3s3s")
       BULLE_D_AIDE("Soustraction Solide\n")
       MENU("Figures/3d/Soustraction")
       CHAMP_VIRTUEL(L_affiche_gl(affiche_union_solide)
		     AFFICHAGE(Toujours_different))
       CHAMP_VIRTUEL(L_affiche_pov(ecrit_pov_soustraction_solide)
		     AFFICHAGE(Toujours_different))
       )
