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
#include <math.h>
#include "o_objet.h"
#include "u_vue.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include "u_etat_courant.h"

/*
 * Les fonctions vides sont à compléter
 */

static Booleen en_train_d_etre_affiche = Faux ;

struct plan
{
  Booleen projection_orthogonale ;
  Booleen projection_orthogonale_en_noir ;
  Booleen affiche_ombres ;
  Booleen affiche_plan ;
  Booleen affiche_miroir ;
  Booleen affiche_quart_plan ;
  Flottant taille ;
} ;
/*
 ******************************************************************************
 * UTILITAIRES QUE VOUS N'AVEZ PAS A MODIFIER
 ******************************************************************************
 */
/*
 * Applique une transformation GL à un point
 */
static void transforme(GLdouble m[4][4],
		       GLfloat in[4], GLfloat out[4])
{
  int i, j ;
  GLdouble s ;

  for(i=0;i<4;i++)
    {
      s = 0 ;
      for(j=0;j<4;j++)
	{
	  s += m[j][i]*in[j] ;
	}
      out[i] = s ;
    }
}
/*
 *
 */
static void ecrit_pov_plan(struct plan *o, const Champ *c, const char *v)
{
  fprintf( (FILE*)atol(v), "plane { <0,0,1>, 0 }\n") ;
}
/*EXCOIN*/
/*
 *
 */
#include "u_graphique.h"
#include "t_geometrie.h"

struct plan_de_decoupe
{
  Transfo t ;
} ;

static void plan_de_decoupage(struct plan_de_decoupe *o,
			      const Champ *c, const char *qualite)
{
  GLdouble plan[4] = { 0, 0, 1, 0} ;
  int cp, premier ;

  glPushMatrix() ;
  u_change_repere(&o->t) ;
  cp = alloue_plan_decoupage_gl(plan) ;
  glPopMatrix() ;
  premier = 1 ;
  POUR_LES_FILS(o, f,
		{
		  if ( !premier )
		    u_affiche_objet(f, qualite) ;
		  premier = 0 ;
		}
		) ;
  libere_plan_decoupage_gl(cp) ;
}

static void ecrit_pov_plan_decoupage(struct plan_de_decoupe *o, const Champ *c, const char *v)
{
  FILE *f ;

  f = (FILE*)atol(v) ;
  fprintf( f,
	   "intersection {\n"
	   "      object { plane { <0,0,-1>, 0 }\n"
	   "    matrix <%g, %g, %g,  %g, %g, %g,  %g, %g, %g,  %g, %g, %g>\n"
	   "}\n"
	   ,
	   o->t.v1.x     , o->t.v1.y     , o->t.v1.z,
	   o->t.v2.x     , o->t.v2.y     , o->t.v2.z,
	   o->t.v3.x     , o->t.v3.y     , o->t.v3.z,
	   o->t.origine.x, o->t.origine.y, o->t.origine.z
	   ) ;
  POUR_LES_FILS(o,f, if ( i_f ) c_sauve_type(f, &infos_L_affiche_pov, v) ; ) ;
  fprintf(f, "}\n") ;
}

CLASSE(plan_de_decoupage, struct plan_de_decoupe,
       CHAMP(t, L_transfo Extrait Edite Sauve)
       CHAMP_VIRTUEL(L_affiche_gl(plan_de_decoupage)
		     AFFICHAGE(Toujours_different))
       CHAMP_VIRTUEL(L_affiche_pov(ecrit_pov_plan_decoupage))
       PAQUET(NB_ACTIFS)
       MENU("Divers/Plan de découpage")
       EVENEMENT("Ctrl+PDD")
       )       
/*EXCOOUT*/

/*
 ******************************************************************************
 * LES FONCTIONS QUE VOUS DEVEZ CREER
 ******************************************************************************
 */
/*
 * Affichage du plan Z=0.
 * Il faut afficher une facette infinie (une seule?)
 */
static void affiche_plan(struct plan *o)
{
  /*
   * Astuce NanoÉdit pour que le plan ne soit pas désignable (activable)
   * en cliquant dessus.
   */
  glLoadName(Faux) ;
  /* EXCOIN */
  glEnable(GL_LIGHTING) ;
  glBegin(GL_QUADS) ;

  if ( o->affiche_quart_plan )
    {
      glNormal3f(0 ,0 ,1) ;
      glVertex3f(0         ,0         ,0) ;
      glVertex3f(o->taille ,0         ,0) ;
      glVertex3f(o->taille ,o->taille ,0) ;
      glVertex3f(0         ,o->taille ,0) ;
    }
  else
    {
      glNormal3f(0 ,0 ,1) ;

      glVertex3f(0         ,0         ,0) ;
      glVertex3f(o->taille ,0         ,0) ;
      glVertex3f(o->taille ,o->taille ,0) ;
      glVertex3f(0         ,o->taille ,0) ;
      glVertex3f(0         ,0         ,0) ;
      glVertex3f(-o->taille,0         ,0) ;
      glVertex3f(-o->taille,-o->taille,0) ;
      glVertex3f(0         ,-o->taille,0) ;
      glVertex3f(0         ,0         ,0) ;
      glVertex3f(0         ,-o->taille,0) ;
      glVertex3f(o->taille ,-o->taille,0) ;
      glVertex3f(o->taille ,0         ,0) ;
      glVertex3f(0         ,0         ,0) ;
      glVertex3f(0         ,o->taille ,0) ;
      glVertex3f(-o->taille,o->taille ,0) ;
      glVertex3f(-o->taille,0         ,0) ;
    }
  glEnd() ;
  glDisable(GL_LIGHTING) ;
  /* EXCOOUT */
}
/*
 * Affiche le sol de manière à mettre les pixels stencil
 * à la valeur 1 aux endroits ou le sol est visible,
 * sans toucher à la couleur ni à la profondeur des pixels.
 * (glStencilOp, glStencilFunc, glDepthMask, glColorMask, ...)
 */
static void affiche_plan_dans_stencil(struct plan *o)
{
  /* EXCOIN */
  glClear(GL_STENCIL_BUFFER_BIT) ;
  glEnable(GL_STENCIL_TEST) ;
  glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE) ;
  glStencilFunc(GL_ALWAYS, 1, 1) ;
  glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE) ;
  glDepthMask(GL_FALSE) ;
  affiche_plan(o) ;
  glDepthMask(GL_TRUE) ;
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE) ;
  glDisable(GL_STENCIL_TEST) ;
  /* EXCOOUT */
}
/*
 * Change le mode GL pour n'afficher que si la valeur du stencil
 * est à 1 pour le pixel. Ceci ne doit pas changer la valeur du stencil.
 * Vous devez utiliser glEnable, glStencilFunc, glStencilOp
 */
static void affiche_seulement_si_stencil_est_a_1()
{
  /* EXCOIN */
  glEnable(GL_STENCIL_TEST) ;
  glStencilFunc(GL_EQUAL, 1, 1) ; /* Affiche si = 1 */
  glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP) ;
  /* EXCOOUT */
}
/*
 * Change le mode GL pour annuler la fonction précédente :
 * "affiche_seulement_si_stencil_est_a_1"
 */
static void affiche_partout()
{
  /* EXCOIN */
  glDisable(GL_STENCIL_TEST) ;
  /* EXCOOUT */
}
/*
 * Affiche la couleur du sol sans toucher à la profondeur.
 * (On peut désactiver le Z-Buffer)
 */
static void affiche_couleur_plan(struct plan *o)
{
  /* EXCOIN */
  glDisable(GL_DEPTH_TEST) ;
  affiche_plan(o) ;
  glEnable(GL_DEPTH_TEST) ;
  /* EXCOOUT */
}
/*
 * Affiche la profondeur du sol sans toucher la couleur.
 */
static void affiche_profondeur_plan(struct plan *o)
{
  /* EXCOIN */
  glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE) ;
  glDepthFunc(GL_ALWAYS) ;
  affiche_plan(o) ;
  glDepthFunc(GL_LESS) ;
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE) ;
  /* EXCOOUT */
}
/*
 * Met la profondeur du sol à l'infini ("glDepthRange")
 * ATTENTION : ne pas faire "glDisable(GL_DEPTH_TEST)"
 *             mais utiliser "glDepthFunc" et "glDepthRange"
 */
static void affiche_profondeur_infini_plan(struct plan *o)
{
  /* EXCOIN */
  glDepthFunc(GL_ALWAYS) ;
  glDepthRange(1,1) ;
  glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE) ;
  affiche_plan(o) ;
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE) ;
  glDepthRange(-1,1) ;
  glDepthFunc(GL_LESS) ;
  /* EXCOOUT */
}
/*
 * Empile les matrices de projection et de modélisation
 */
static void empile_matrices_courantes()
{
  /* EXCOIN */
  /*
  GLint i ;
  glGetIntegerv(GL_MODELVIEW_STACK_DEPTH, &i) ;
  eprintf("GL_MODELVIEW_STACK_DEPTH = %d\n", i) ;
  glGetIntegerv(GL_PROJECTION_STACK_DEPTH, &i) ;
  eprintf("GL_PROJECTION_STACK_DEPTH = %d\n", i) ;
  glGetIntegerv(GL_MAX_MODELVIEW_STACK_DEPTH, &i) ;
  eprintf("GL_MAX_MODELVIEW_STACK_DEPTH = %d\n", i) ;
  glGetIntegerv(GL_MAX_PROJECTION_STACK_DEPTH, &i) ;
  eprintf("GL_MAX_PROJECTION_STACK_DEPTH = %d\n", i) ;
  */
  glMatrixMode(GL_PROJECTION) ;
  glPushMatrix() ;
  if ( 0 && glGetError() != GL_NO_ERROR )
    EXIT ;
  glMatrixMode(GL_MODELVIEW) ;
  glPushMatrix() ;
  if ( 0 && glGetError() != GL_NO_ERROR )
    EXIT ;
  /* EXCOOUT */
}
/*
 * Dépile les matrices de projection et de modélisation
 */
static void depile_matrices_courantes()
{
  /* EXCOIN */
  glMatrixMode(GL_PROJECTION) ;
  glPopMatrix() ;
  glMatrixMode(GL_MODELVIEW) ;
  glPopMatrix() ;
  /* EXCOOUT */
}
/*
 * Initialise et met en action le plan de découpage Z>0.
 * L'entier retourné est le numéro du plan de découpage.
 */
static int initialise_plan_decoupage_z0()
{
  /* EXCOIN */
  GLdouble plan[4] = { 0, 0, 1, 0} ;
  return( alloue_plan_decoupage_gl(plan) ) ;
  /* EXCOOUT */
}
/*
 * Initialise et met en action le plan de découpage Z<d.
 * L'entier retourné est le numéro du plan de découpage.
 */
static int initialise_plan_decoupage_z(double d)
{
  /* EXCOIN */
  GLdouble plan[4] = { 0, 0, -1, 0} ;
  plan[3] = d ;
  return( alloue_plan_decoupage_gl(plan) ) ;
  /* EXCOOUT */
}
/*
 * Arête de découper suivant le plan de découpage indiqué
 */
static void arrete_de_decouper(int plan)
{
  /* EXCOIN */
  libere_plan_decoupage_gl(plan) ;
  /* EXCOOUT */
}
/*
 * Applique à la transformation GL courante de modélisation
 * la projection orthogonale sur le plan Z=0
 */
static void projection_orthogonale()
{
  /* EXCOIN */
  glScalef(1,1,0) ;
  /* EXCOOUT */
}
/*
 * Applique à la transformation GL courante de modélisation
 * la projection miroir sur le plan Z=0
 */
static void projection_miroir()
{
  /* EXCOIN */
  glScalef(1,1,-1) ;
  /* EXCOOUT */
}
/*
 * Applique à la transformation GL courante de modélisation
 * la projection perspective des ombres portées
 */
static void projection_ombres(GLfloat position_source_lumineuse[4])
{
  /* EXCOIN */
  GLdouble m[16] = { 0 } ;

  glTranslatef(position_source_lumineuse[0], position_source_lumineuse[1],0) ;
  m[0 ] = 1 ;
  m[5 ] = 1 ;
  m[11] = -1/position_source_lumineuse[2]  ;
  m[15] = 1 ;      
  glMultMatrixd(m) ;
  glTranslatef(-position_source_lumineuse[0],-position_source_lumineuse[1],0.);
  /* EXCOOUT */
}
/*
 * Ajout de la matrice de modélisation dans la matrice de projection
 * afin que les plans de découpages fonctionnent ainsi que le
 * calcul de l'éclairement,
 */
static void transfert_matrice_de_modelisation_dans_perspective()
{
  /* EXCOIN */
  GLdouble m[16] ;

  glGetDoublev(GL_MODELVIEW_MATRIX,m) ;
  glLoadIdentity() ;
  glMatrixMode(GL_PROJECTION) ;
  glMultMatrixd(m) ;
  glMatrixMode(GL_MODELVIEW) ;
  /* EXCOOUT */
}
/*
 * Change l'état courant de manière à afficher en noir
 * quelque soit la couleur d'affichage, et avec ou sans matériaux.
 * (On utilise glBlendFunc et glEnable).
 */
static void affiche_en_noir()
{
  /* EXCOIN */
  glBlendFunc(GL_ZERO, GL_ZERO) ;
  glEnable(GL_BLEND) ;
  /* EXCOOUT */
}
/*
 * Affiche en tenant compte du ALPHA (transparence) dans la couleur.
 * (On utilise glBlendFunc et glEnable).
 */
static void affiche_en_tenant_compte_du_alpha()
{
  /* EXCOIN */
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA) ;
  glEnable(GL_BLEND) ;
  /* EXCOOUT */
}
/*
 * Revient à un affichage normal (annule les 2 fonctions précédentes)
 */
static void affiche_normal()
{
  /* EXCOIN */
  glDisable(GL_BLEND) ;
  /* EXCOOUT */
}
/*
 * Fonction que je vous donne et que vous devez comprendre.
 */
static void affiche(struct plan *o, const Champ *c, const char *qualite)
{
  GLdouble repere_plan[16], repere_inverse_plan[16] ;
  GLfloat p[4], pp[4] ;
  int plan_z0, plan_z ;
  int i ;
  GLint max ;
  GLfloat couleur_courante[4], ambiante_courante[4], couleur_alpha[4] ;

  if ( TRACE(o) )
    eprintf("Début affichage plan en_train_d_etre_affiche=%d\n"
	    , en_train_d_etre_affiche) ;
  /*
   * Pour éviter la récursion.
   * C'est une variable globale car on ne veut pas faire les ombres des ombres.
   */
  if ( en_train_d_etre_affiche )
    return ;
  en_train_d_etre_affiche = Vrai ;
  /*
   * Pour simplifier le programme, on affiche systématiquement
   * le plan dans le stencil
   */
  affiche_plan_dans_stencil(o) ;
  /*
   * Pour la suite, on teste systématiquement le stencil
   * avant d'afficher.
   */
  affiche_seulement_si_stencil_est_a_1() ;
  /*
   * On récupère le repère courant et on calcule son inverse.
   * Ceci est nécessaire car on ne veut pas que ce que l'on
   * affiche sur le plan subisse la transformation du plan.
   */
  glGetDoublev(GL_MODELVIEW_MATRIX, repere_plan) ;
  inversion4x4(repere_plan, repere_inverse_plan) ;
  /*
   * Pour retracer le plan avec un autre ALPHA
   * afin que les ombres ne soient pas complètement noires
   * et pour atténuer les reflets.
   */
  glGetMaterialfv(GL_FRONT, GL_DIFFUSE, couleur_courante) ;
  glGetMaterialfv(GL_FRONT, GL_DIFFUSE, couleur_alpha) ;
  glGetMaterialfv(GL_FRONT, GL_AMBIENT, ambiante_courante) ;
  couleur_alpha[3] = ambiante_courante[0] ;
  /*
   * Ce qui nous intéresse est toujours au dessus du plan.
   * On coupe donc ce qui est en dessous du plan pour ne pas l'afficher.
   */
  plan_z0 = initialise_plan_decoupage_z0() ;
  /*
   * Affichage du plan de projection.
   */
  if ( o->affiche_plan && globale_etat_courant.type_affichage == Facette )
    affiche_couleur_plan(o) ;
  /*
   * Affichage des ombres portées de chaque lampe
   * Si la qualité d'affichage est meilleur que 'mauvaise'
   */
  if ( o->affiche_ombres && qualite[0]>'4')
    {
      if ( TRACE(o) )
	eprintf("Affichage des ombres\n") ;
      affiche_en_noir() ;
      glGetIntegerv(GL_MAX_LIGHTS, &max) ;

      for(i=GL_LIGHT0;i<GL_LIGHT0+max;i++)
	if ( glIsEnabled(i) )
	  {
	    if ( TRACE(o) )
	      eprintf("Affichage des ombres lampe %d\n", i-GL_LIGHT0) ;

	    glGetLightfv(i, GL_POSITION, p) ;
	    /*
	     * Pourquoi la ligne suivante ?
	     */
	    transforme((GLdouble(*)[4])repere_inverse_plan, p, pp) ;
	    plan_z = initialise_plan_decoupage_z(pp[2]) ; /* POURQUOI ? */
	    
	    empile_matrices_courantes() ;
	    projection_ombres(pp) ;
	    glMultMatrixd(repere_inverse_plan) ; /* POURQUOI ? */
	    transfert_matrice_de_modelisation_dans_perspective() ;
	    /* affiche_modele(o, qualite) ; 26/7/2000 */
	    /*
	     * Réaffiche le modèle complet (pas propre)
	     */
	    affiche_gl( OBJET(u_modele(o))->fils, qualite) ;

	    depile_matrices_courantes() ;
	    
	    arrete_de_decouper(plan_z) ;
	  }
      /*
       * Pour atténuer les ombres
       */
      glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, couleur_alpha) ;
      affiche_en_tenant_compte_du_alpha() ;
      affiche_couleur_plan(o) ;
      /*
       * Remet dans l'état normal.
       */
      glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, couleur_courante) ;
      affiche_normal() ;
    }
  /*
   * Affichage du miroir
   * Si la qualité d'affichage est meilleur que 'tres mauvaise'
   */
  if ( o->affiche_miroir && qualite[0]>'3')
    {
      affiche_profondeur_infini_plan(o) ; /* POURQUOI ? */
      empile_matrices_courantes() ;
      projection_miroir() ;
      glMultMatrixd(repere_inverse_plan) ;
      transfert_matrice_de_modelisation_dans_perspective() ;
      glCullFace(GL_FRONT) ;	/* Car miroir inverse interieur/extérieur */
      if ( TRACE(o) )
	eprintf("Affiche miroir sur le plan\n") ;
      /* affiche_modele(o, qualite) ; */
      affiche_gl( OBJET(u_modele(o))->fils, qualite) ;
      glCullFace(GL_BACK) ;
      depile_matrices_courantes() ;
    }
  /*
   * Affichage de la projection orthogonale
   * Si la qualité d'affichage est meilleur que 'tres mauvaise'
   */
  if ( o->projection_orthogonale && qualite[0]>'3')
    {
      empile_matrices_courantes() ;
      projection_orthogonale() ;
      glMultMatrixd(repere_inverse_plan) ;
      transfert_matrice_de_modelisation_dans_perspective() ;
      if ( o->projection_orthogonale_en_noir )
	affiche_en_noir() ;
      if ( TRACE(o) )
	eprintf("Affiche projection orthogonales sur le plan\n") ;
      /* affiche_modele(o, qualite) ; */
      affiche_gl( OBJET(u_modele(o))->fils, qualite) ;
      if ( o->projection_orthogonale_en_noir )
	affiche_normal() ;
      depile_matrices_courantes() ;
    }
  /*
   * On met la profondeur du plan, pas sa couleur.
   * Comme cela les faces cachées vont fonctionner correctement
   * grace à la bonne profondeur.
   */
  if ( o->affiche_plan && globale_etat_courant.type_affichage == Facette )
    {
      affiche_profondeur_plan(o) ;
      /*
       * On n'affiche le filtre masquant que s'il y a une bonne
       * qualité d'affichage (supérieure à 'normale')
       */
      if ( o->affiche_miroir && qualite[0]>'5' )
	{
	  affiche_en_tenant_compte_du_alpha() ;
	  affiche_couleur_plan(o) ;
	  affiche_normal() ;
	}
    }
  /*
   * On sort proprement
   */    
  arrete_de_decouper(plan_z0) ;
  affiche_partout() ;
  en_train_d_etre_affiche = Faux ;
  if ( TRACE(o) )
    eprintf("Fin affichage plan\n") ;
}




CLASSE(plan, struct plan,
       Edite Sauve

       BULLE_D_AIDE("Objet plan horizontal\n"
		    "\n"
		    "Cet objet permet de nombreux effets visuels....\n"
		    "ATTENTION : tous les effets visuels ne sont pas\n"
		    "actif quelque soit la qualité d'affichage.\n"
		    "Il faut au moins que la qualité soit `bonne'.\n"
		    )

       MENU("Figures/2d/Plan")
       EVENEMENT("Ctrl+PL")

       L_booleen

       CHAMP(projection_orthogonale,
	     BULLE_D_AIDE("Si vrai, alors une projection orthogonale\n"
			  "est faite sur le plan de projection.\n"
			  )
	     Extrait
	     )
       CHAMP(affiche_ombres,
	     BULLE_D_AIDE("Si vrai, alors affiche les ombres portées\n"
			  "sur le plan.\n"
			  )
	     Extrait
	     DEFAUT("1")
	     )
       CHAMP(projection_orthogonale_en_noir,
	     BULLE_D_AIDE("Si vrai, alors le projection orthogonale\n"
			  "est dessinée en noir au lieu des\n"
			  "couleurs originales.\n"
			  )
	     )
       CHAMP(affiche_plan,
	     BULLE_D_AIDE("Si vrai, le plan de projection et d'ombrage\n"
			  "est affiché. Sinon, les ombres et projections\n"
			  "aparaissent dans le vide.\n"
			  )
	     DEFAUT("1")
	     )
       CHAMP(affiche_quart_plan,
	     BULLE_D_AIDE("Si vrai, seulement le quart du plan est affiché")
	     DEFAUT("0")
	     )
       CHAMP(affiche_miroir,
	     BULLE_D_AIDE("Si vrai, affiche la reflexion sur le plan\n"
			  "Attention, il faut changer la valeur du\n"
			  "coefficient de réflexion du plan pour\n"
			  "que l'effet soit visible.\n")
	     DEFAUT("0")
	     )
       CHAMP(taille,
	     BULLE_D_AIDE("Taille du ``plan''")
	     L_nombre P_flottant Init3d
	     DEFAUT("5")
	     )

       CHAMP_VIRTUEL(L_affiche_gl(affiche) AFFICHAGE(Toujours_different))
       CHAMP_VIRTUEL(L_affiche_pov(ecrit_pov_plan))
       )

/*
 ******************************************************************************
 *
 ******************************************************************************
 */

struct plan_lie
{
  Quadruplet equation ;		/* Équation plan ax+by+cz+d = 0 */
  Triplet point ;
  Triplet normale ;
} ;

static void affiche_plan_lie(struct plan_lie *o)
{
  int i ;
  Triplet p, q ;

  if ( ACTIF(o) )
    t_tag_origine(1, &o->point) ;

  p = u_ajoute_triplet(&o->point, &o->normale) ;
  t_fleche_proportionnelle(&o->point, &p, 0, 0.1) ;

  glBegin(GL_LINES) ;
  t_triplet(&o->point) ;
  t_triplet(&p) ;

  for(i=0; i<8; i++)
    {
      p.x = o->point.x + ((i&1) ? 1 : -1) ;
      p.y = o->point.y + ((i&2) ? 1 : -1) ;
      p.z = o->point.z + ((i&4) ? 1 : -1) ;

      q = p ;

      if ( fabs(o->equation.x) > 1e-4 )
	{
	  q.x = (-o->equation.h - o->equation.z*p.z - o->equation.y*p.y)
	    / o->equation.x ;
	  if ( u_distance_triplet(&q, &o->point) < 3 )
	    {
	      t_triplet(&o->point) ;
	      t_triplet(&q) ;
	    }
	  q.x = p.x ;
	}
      if ( fabs(o->equation.y) > 1e-4 )
	{
	  q.y = (-o->equation.h - o->equation.z*p.z - o->equation.x*p.x)
	    / o->equation.y ;
	  if ( u_distance_triplet(&q, &o->point) < 3 )
	    {
	      t_triplet(&o->point) ;
	      t_triplet(&q) ;
	    }
	  q.y = p.y ;
	}
      if ( fabs(o->equation.z) > 1e-4 )
	{
	  q.z = (-o->equation.h - o->equation.x*p.x - o->equation.y*p.y)
	    / o->equation.z ;
	  if ( u_distance_triplet(&q, &o->point) < 3 )
	    {
	      t_triplet(&o->point) ;
	      t_triplet(&q) ;
	    }
	  q.z = p.z ;
	}
    }

  glEnd() ;

}

static void changement_plan_lie(struct plan_lie *o)
{
  o->equation = u_creation_plan_lie(&o->normale, &o->point) ;
}


CLASSE(plan_lie, struct plan_lie,
       CHAMP(equation, L_plan P_quadruplet Affiche)
       CHAMP(normale, L_vecteur P_triplet Extrait Edite Sauve Init3d DEFAUT("0 0 1"))
       CHAMP(point, L_point P_triplet Extrait Edite Sauve Init3d DEFAUT("0 0 0"))
       CHAMP_VIRTUEL(L_affiche_gl(affiche_plan_lie))
       CHANGEMENT(changement_plan_lie)
       EVENEMENT("Ctrl+EP")
       MENU("Figures/2d/Plan Lié")
       )

/*
 * Intersection 3 plans
 */

struct intersection_3_plans
{
  Triplet p ;
  Quadruplet p1, p2, p3 ;
} ;

static void changement_intersection_3_plans(struct intersection_3_plans *o)
{
  o->p = u_intersection_3_plans(&o->p1, &o->p2, &o->p3) ;
}

CLASSE(intersection_3_plans, struct intersection_3_plans,
       CHAMP(p, L_point P_triplet Affiche)
       L_plan P_quadruplet Extrait Sauve Edite
       CHAMP(p1, DEFAUT("0 0 1 0"))
       CHAMP(p2, DEFAUT("0 1 0 0"))
       CHAMP(p3, DEFAUT("1 0 0 0"))
       EVENEMENT("I3P")
       MENU("Figures/0d/Intersection 3 plans")
       CHANGEMENT(changement_intersection_3_plans)
       )

       

