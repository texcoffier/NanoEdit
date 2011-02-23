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
  fprintf( (FILE*)atoi(v), "plane { <0,0,1>, 0 }\n") ;
}

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
}
/*
 * Affiche le sol de manière à mettre les pixels stencil
 * à la valeur 1 aux endroits ou le sol est visible,
 * sans toucher à la couleur ni à la profondeur des pixels.
 * (glStencilOp, glStencilFunc, glDepthMask, glColorMask, ...)
 */
static void affiche_plan_dans_stencil(struct plan *o)
{
}
/*
 * Change le mode GL pour n'afficher que si la valeur du stencil
 * est à 1 pour le pixel. Ceci ne doit pas changer la valeur du stencil.
 * Vous devez utiliser glEnable, glStencilFunc, glStencilOp
 */
static void affiche_seulement_si_stencil_est_a_1()
{
}
/*
 * Change le mode GL pour annuler la fonction précédente :
 * "affiche_seulement_si_stencil_est_a_1"
 */
static void affiche_partout()
{
}
/*
 * Affiche la couleur du sol sans toucher à la profondeur.
 * (On peut désactiver le Z-Buffer)
 */
static void affiche_couleur_plan(struct plan *o)
{
}
/*
 * Affiche la profondeur du sol sans toucher la couleur.
 */
static void affiche_profondeur_plan(struct plan *o)
{
}
/*
 * Met la profondeur du sol à l'infini ("glDepthRange")
 * ATTENTION : ne pas faire "glDisable(GL_DEPTH_TEST)"
 *             mais utiliser "glDepthFunc" et "glDepthRange"
 */
static void affiche_profondeur_infini_plan(struct plan *o)
{
}
/*
 * Empile les matrices de projection et de modélisation
 */
static void empile_matrices_courantes()
{
}
/*
 * Dépile les matrices de projection et de modélisation
 */
static void depile_matrices_courantes()
{
}
/*
 * Initialise et met en action le plan de découpage Z>0.
 * L'entier retourné est le numéro du plan de découpage.
 */
static int initialise_plan_decoupage_z0()
{
}
/*
 * Initialise et met en action le plan de découpage Z<d.
 * L'entier retourné est le numéro du plan de découpage.
 */
static int initialise_plan_decoupage_z(double d)
{
}
/*
 * Arête de découper suivant le plan de découpage indiqué
 */
static void arrete_de_decouper(int plan)
{
}
/*
 * Applique à la transformation GL courante de modélisation
 * la projection orthogonale sur le plan Z=0
 */
static void projection_orthogonale()
{
}
/*
 * Applique à la transformation GL courante de modélisation
 * la projection miroir sur le plan Z=0
 */
static void projection_miroir()
{
}
/*
 * Applique à la transformation GL courante de modélisation
 * la projection perspective des ombres portées
 */
static void projection_ombres(GLfloat position_source_lumineuse[4])
{
}
/*
 * Ajout de la matrice de modélisation dans la matrice de projection
 * afin que les plans de découpages fonctionnent ainsi que le
 * calcul de l'éclairement,
 */
static void transfert_matrice_de_modelisation_dans_perspective()
{
}
/*
 * Change l'état courant de manière à afficher en noir
 * quelque soit la couleur d'affichage, et avec ou sans matériaux.
 * (On utilise glBlendFunc et glEnable).
 */
static void affiche_en_noir()
{
}
/*
 * Affiche en tenant compte du ALPHA (transparence) dans la couleur.
 * (On utilise glBlendFunc et glEnable).
 */
static void affiche_en_tenant_compte_du_alpha()
{
}
/*
 * Revient à un affichage normal (annule les 2 fonctions précédentes)
 */
static void affiche_normal()
{
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
      glCullFace(GL_FRONT) ;	/* POURQUOI ? */
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




CLASSE(tp_plan, struct plan,
       Edite Sauve

       BULLE_D_AIDE("Objet plan horizontal\n"
		    "\n"
		    "Cet objet permet de nombreux effets visuels....\n"
		    "ATTENTION : tous les effets visuels ne sont pas\n"
		    "actif quelque soit la qualité d'affichage.\n"
		    "Il faut au moins que la qualité soit `bonne'.\n"
		    )

       MENU("TP Infographie/Plan")

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

       CHAMP_VIRTUEL(L_affiche_gl(affiche) AFFICHAGE(Toujours_different))
       CHAMP_VIRTUEL(L_affiche_pov(ecrit_pov_plan))
       )




