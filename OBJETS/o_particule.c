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
#include "u_xt_interface.h"
#include "t_geometrie.h"
#include "u_graphique.h"
#include "u_repartiteur.h"
#include "t_gl.h"

#define BASETAG 1000		/* Numéro d'identification du premier atome */
#define D 3			/* Dimension de l'espace */
#define POS(i)  (o->g.noeuds[i].donnee.table)
#define VIT(i)   (o->g.noeuds[i].donnee.table+D)
#define DIM(PROG) do { int dim ; for(dim=0; dim<D; dim++) { PROG ; } } while(0)

struct systeme_particule
{
  t_graphe_N_l6_float g ;		/* Le graphe de particules */
  Flottant de, dt, k, f ;
  Booleen repulsion ;
  Booleen onoff, old_onoff ;
} ;
/*
 * Affichage du système de particules
 */
static void affiche_particules(struct systeme_particule *o)
{
  int i,j,k ;
  Triplet p ; 

  /* Affiche les arêtes */
  glBegin( GL_LINES ) ;
  for(i=0;i<o->g.nb_noeuds;i++)
    {
      for(j=0;j<o->g.noeuds[i].nb_aretes;j++)
	{
	  k = o->g.noeuds[i].aretes[j].destination ;
	  if ( k < i )
	    {
	      glVertex3fv(POS(i)) ;
	      glVertex3fv(POS(k)) ;
	    }
	}
    }
  glEnd() ;
  /* Affiche les sommets */
  if ( ACTIF(o) )
    {
      /* Si le système est actif, on trace des tag sur les points
       * pour permettre à l'utilisateur de les déplacer */
      for(i=0;i<o->g.nb_noeuds;i++)
	{
	  DIM( (&p.x)[dim] = POS(i)[dim] ) ;
	  t_tag(BASETAG+i, &p) ;
	}
    }
  else
    {
      glBegin(GL_POINTS) ;
      for(i=0;i<o->g.nb_noeuds;i++)
	glVertex3fv(POS(i)) ;
      glEnd() ;
    }
}
/* L'utilisateur déplace un tag */
static void ecrit_tag(struct systeme_particule *o,const Champ *c,const char *v)
{
  int i ;

  i = lit_numero_tag(c) - BASETAG ;
  if ( i >= 0 && i < o->g.nb_noeuds )
    sscanf(v, "%g%g%g", POS(i), POS(i)+1, POS(i)+2) ;
}
/* Lecture de la position d'un tag */
static const char * lit_tag(struct systeme_particule *o, const Champ *c)
{
  static char buf[99] ;
  int i ;

  i = c->parametre - BASETAG ;
  if ( i >= 0 && i < o->g.nb_noeuds )
    sprintf(buf, "%g %g %g", POS(i)[0], POS(i)[1], POS(i)[2]) ;
  else
    strcpy(buf, "") ;
  return(buf) ;
}
/* Déplacement des particules */
static Booleen u_calcul(void *objet)
{
  struct systeme_particule *o = objet ;
  float d, n, force[D], somme[D] ;
  int i, j, k, r ;

  for(i=0;i<o->g.nb_noeuds;i++)
    {
      DIM( somme[dim] = 0. ) ;
      for(j=0;j<o->g.noeuds[i].nb_aretes;j++)
	{
	  k = o->g.noeuds[i].aretes[j].destination ;
	  if ( k >= o->g.nb_noeuds )
	    EXIT ;
	  n = 0. ;
	  DIM( force[dim] = POS(k)[dim] - POS(i)[dim] ;
	       n += force[dim] * force[dim] ; ) ;
	  d  = o->k*(sqrt(n) - o->de) ;	  
	  DIM( somme[dim] += force[dim] * d ) ;

#define NB 1

	  if ( o->repulsion )
	    for(r=0; r<NB; r++)
	      {
		k = rand() % o->g.nb_noeuds ;
		if ( k == i )
		  continue ;
		n = 0 ;
		DIM( force[dim] = POS(k)[dim] - POS(i)[dim] ;
		     n += force[dim] * force[dim] ; ) ;
		d  = (o->g.nb_noeuds/NB)*0.1/n ;	  
		DIM( somme[dim] -= force[dim] * d ) ;
	      }
	}
      DIM( VIT(i)[dim] += (somme[dim] - VIT(i)[dim]*o->f) * o->dt ) ;
    }
  for(i=0;i<o->g.nb_noeuds;i++)
    DIM( POS(i)[dim] += VIT(i)[dim]*o->dt ) ;
  /* Demande à l'éditeur de mettre tout à jour (notamment afficher) */
  evaluation(o) ;

  return(Vrai) ; /* Continuer en arrière plan */
}

static void destruction_systeme(struct systeme_particule *o)
{
  u_enleve_action(u_calcul, o) ;
}

static void lance_arrete(struct systeme_particule *o)
{
  if ( o->onoff == o->old_onoff )
    return ;

  if ( o->onoff )
    {
      u_ajoute_action(PRIORITE_ARRIERE_PLAN, u_calcul, o) ;      
    }
  else
    {
      u_enleve_action(u_calcul, o) ;
    }

  o->old_onoff = o->onoff ;
}

static Booleen onoff(const Interface_widget *iw)
{
  struct systeme_particule *o = iw->objet ;

  o->onoff ^= 1 ;
  lance_arrete(o) ;

  return(Faux) ;
}

static void changement(struct systeme_particule *o)
{
  if ( CHAMP_CHANGE(o, onoff) )
    lance_arrete(o) ;
}

CLASSE(systeme_de_particule, struct systeme_particule,
       Edite Sauve
       CHAMP(g, L_graphe_de_points P_graphe_N_l6_float Extrait)
       CHAMP(de, LABEL("Distance"  ) L_nombre P_flottant DEFAUT("1"   ))
       CHAMP(dt, LABEL("Delta t"   ) L_nombre P_flottant DEFAUT("0.01"))
       CHAMP(k , LABEL("Raideur"   ) L_nombre P_flottant DEFAUT("1"   ))
       CHAMP(f , LABEL("Frottement") L_nombre P_flottant DEFAUT("0.1" ))
       CHAMP(repulsion, L_booleen Edite Sauve DEFAUT("0")
	     LABEL("Répulsion entre tous"))
       CHAMP_VIRTUEL(LABEL("On/Off") L_bouton(onoff))
       CHAMP_VIRTUEL(L_affiche_gl(affiche_particules)
		     AFFICHAGE(Toujours_different))
       CHAMP_VIRTUEL(L_tag(ecrit_tag,lit_tag))
       CHAMP(onoff, L_booleen Sauve DEFAUT("0") NonCree)
       DESTRUCTION(destruction_systeme)
       CHANGEMENT(changement)
       MENU("Simulateurs/Système de particules")
       EVENEMENT("SDP")
       )
