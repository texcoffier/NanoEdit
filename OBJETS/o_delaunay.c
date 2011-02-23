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
/* #include "u_simplex.h" */
#define NB_PLANS 100

#include "u_delaunay.h"
#include "t_geometrie.h"
#include "u_graphique.h"
#include "u_table.h"
#include "stl.h"

int inter_espace(double a[100][100], double b[100] ,int mm,int nn) ;
int inter2_espace(double a[100][100], double b[100] ,int mm,int nn) ;

typedef struct
{
  Table_triplet t ;
  t_graphe_N_l3_float g ; /* Le graphe de points */
  Delaunay d ;
  Flottant rayon ;
} delaunay ;

/*
 * Table globale nécessaire car la bibliothèque Delaunay
 * utilise des indices de tableaux plutôt que des pointeurs
 */
static Triplet *globale_points ;
/*
 * Détermination si une région est vide ou non
 */
static int empty_region2(const Region* r)
{
  Triplet *in, *out ;
  int i ;
  double t[NB_PLANS][NB_PLANS], v[NB_PLANS] ;

  for(i=0;i<r->nb_pairs;i++)
    {
      in = &globale_points[r->pairs[i].in] ;
      out = &globale_points[r->pairs[i].out] ;
      t[i][0] = out->x - in->x ;
      t[i][1] = out->y - in->y ;
      t[i][2] = out->z - in->z ;
      v[i] = t[i][0]*(in->x+out->x)/2
	+ t[i][1]*(in->y+out->y)/2
	+ t[i][2]*(in->z+out->z)/2
	;
    }
  i = !inter_espace(t, v ,r->nb_pairs, 3) ;

  return(i) ;
}
/*
 * Copie de la structure de donnée "DNode" contenant
 * le Delaunay dans l'objet NanoÉdit.
 */
static void copie_delaunay(delaunay *o, DNode *g)
{
  int i ;

  if ( flagged(g) )
    return ;

  if ( g->value >= o->g.nb_noeuds )
    EXIT ;

  o->g.noeuds[g->value].donnee.table[0] = o->t.table[g->value].x ;
  o->g.noeuds[g->value].donnee.table[1] = o->t.table[g->value].y ;
  o->g.noeuds[g->value].donnee.table[2] = o->t.table[g->value].z ;

  CODEC_REALLOC(o->g.noeuds[g->value].aretes, g->nb_connected) ;
  o->g.noeuds[g->value].nb_aretes = g->nb_connected ;
  
  for(i=0;i<g->nb_connected;i++)
    {
      o->g.noeuds[g->value].aretes[i].destination = g->germs[i]->value ;
      copie_delaunay(o, g->germs[i]) ;
    }
}
/*
 * Évalutation de l'objet Delaunay.
 * Fonction complexe car elle passe les coordonnées
 * des points d'un graphe dans une table et inversement.
 */
static void changement_delaunay(delaunay *o)
{
  int i ;

  if ( TRACE(o) )
    eprintf("Calcul Delaunay de %d point\n", o->t.nb) ;

  if ( CHAMP_CHANGE(o,g) && !FILS_CHANGE(o) && !CREATION(o) )
    {
      /*
       * Prend les points du graphe et les met dans la table
       */
      CODEC_REALLOC(o->t.table, o->g.nb_noeuds) ;
      for(i=0;i<o->g.nb_noeuds;i++)
	{
	  o->t.table[i].x = o->g.noeuds[i].donnee.table[0] ;
	  o->t.table[i].y = o->g.noeuds[i].donnee.table[1] ;
	  o->t.table[i].z = o->g.noeuds[i].donnee.table[2] ;
	}
      o->t.nb = o->g.nb_noeuds ;
    }
  /*
   * Calcul du Delaunay de manière incrémentale
   */  
  free_delaunay(&o->d) ;
  initialise_delaunay(&o->d, empty_region2, NULL) ;
  if ( o->t.nb == 0 )
    return ;
  globale_points = o->t.table ;
  for(i=0;i<o->t.nb;i++)
    add_a_new_germ(&o->d, i) ;
  /*
   * Allocation/Libération du Graphe de l'objet pour qu'il puisse
   * recevoir les données du Delaunay
   */	
  if ( o->t.nb < o->g.nb_noeuds )
    {
      for(i=o->t.nb;i<o->g.nb_noeuds;i++)
	codec_libere(codec_graphe_N_l3_float_noeud, &o->g.noeuds[i]) ;
      o->g.nb_noeuds = o->t.nb ;
    }
  else
    if ( o->t.nb > o->g.nb_noeuds )
      {
	CODEC_REALLOC(o->g.noeuds, o->t.nb) ;
	for(i=o->g.nb_noeuds;i<o->t.nb;i++)
	  {
	    o->g.noeuds[i].nb_aretes = 0 ;
	    CODEC_MALLOC(o->g.noeuds[i].aretes, 1) ;
	  }
	    
      }
  o->g.nb_noeuds = o->t.nb ;
  /*
   * Recopie de  Delaunay dans le graphe de l'objet.
   */
  reset_flag(&o->d) ;
  copie_delaunay(o, a_region(&o->d)) ;
}

/*
 * Draw the Delaunay arcs.
 */
static void stroke_delaunay(Triplet *pt, DNode *g)
{
  int i ;

  if ( flagged(g) )
    return ;

  for(i=0;i<g->nb_connected;i++)
    {
      if ( g->value > g->germs[i]->value )
	{
	  t_triplet(&pt[g->value]) ;
	  t_triplet(&pt[g->germs[i]->value]) ;
	}
      stroke_delaunay(pt, g->germs[i]) ;
    }
}


#define NBF 20

static void stroke_delaunay_stl(FILE *f, float rayon, Triplet *pt, DNode *g)
{
  int i ;

  if ( flagged(g) )
    return ;

  for(i=0;i<g->nb_connected;i++)
    {
      if ( g->value > g->germs[i]->value )
	{
	  t_cylindre_stl(f, &pt[g->germs[i]->value], &pt[g->value], rayon, NBF);
	}
      stroke_delaunay_stl(f, rayon, pt, g->germs[i]) ;
    }
}

static void affiche_delaunay(delaunay *o)
{
  if ( a_region(&o->d)==NULL )
    return ;

  reset_flag(&o->d) ;
  glBegin(GL_LINES) ;
  stroke_delaunay(o->t.table, a_region(&o->d)) ;
  glEnd();
  
  if ( ACTIF(o) )
    t_table(o, c_trouve(o, "t"), T_TAG) ;
}

static Booleen affiche_delaunay_stl(const Interface_widget *iw)
{
  FILE *f ;
  delaunay *o ;

  o = iw->objet ;
  if ( a_region(&o->d)==NULL )
    return(Vrai) ;

  reset_flag(&o->d) ;
  
  f = fopen("delaunay.stl", "w") ;
  fprintf(f, "solid delaunay.tri\n") ;
  stroke_delaunay_stl(f, o->rayon, o->t.table, a_region(&o->d)) ;
  fprintf(f, "endsolid delaunay.tri\n") ;
  fclose(f) ;
  return(Vrai) ;
}

CLASSE(delaunay, delaunay,
       CHAMP(g, LABEL("Graphe") L_graphe_de_points P_graphe_N_l3_float Edite
	     TE_HAUTEUR(200)
	     BULLE_D_AIDE("Graphe de Delaunay des points\n"
			  "Pour chaque sommet :\n"
			  "  - Liste des indices des sommets connexes\n"
			  "  - Coordonnées du sommet\n"
			  )
	     )
       CHAMP(t, LABEL("Germes") L_table_point P_table_triplet Extrait Edite Sauve
	     BULLE_D_AIDE("Table de points germes du Delaunay\n")
	     )
       CHAMP(rayon, L_nombre P_flottant Edite Sauve DEFAUT("0.1")
	     BULLE_D_AIDE("Rayon des cylindre pour sortie STL")
	     ) ;
       CHAMP_VIRTUEL(L_affiche_gl(affiche_delaunay)
		     AFFICHAGE(Different_si_actif))
       CHAMP_VIRTUEL(L_tag(ecrit_tag_table,lit_tag_table))
       CHANGEMENT(changement_delaunay)
       CHAMP_VIRTUEL(LABEL("Sortie STL dans `delaunay.stl'")
		     L_bouton(affiche_delaunay_stl))
       MENU("Figures/0d/Delaunay")
       EVENEMENT("DEL")
       )
