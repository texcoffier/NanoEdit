/*
    NanoÉdit: Un modeleur algébrique interactif.
    Copyright (C) 2006 Thierry EXCOFFIER, Université Claude Bernard, LIRIS

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

#include "o_objet.h"		/* Car il faut definir un objet */
#include "t_geometrie.h"
#include "u_table.h"
#include "u_etat_courant.h"
#include "u_graphique.h"
#include <GL/glu.h>
#include <math.h>

typedef struct
{
  t_L_l4_int t ;
  int min[3], max[3], taille[3] ; /* min et max sont inclusif */
  t_L_int requise ; /* Table indique pour chaque boule si elle est requise */
  t_L_int voxels ; /* Les voxels x varie le plus vite */
  Booleen cache_voxels_bord ;
  Booleen cache_filaire ;
} Boulier ;

  static int dx[] = {-1,1,0,0,0,0} ;
  static int dy[] = {0,0,-1,1,0,0} ;
  static int dz[] = {0,0,0,0,-1,1} ;

Booleen voxel_dans_boule(t_l4_int *b, int v[3])
{
  return ( (b->table[0] - v[0]) * (b->table[0] - v[0]) + 
	   (b->table[1] - v[1]) * (b->table[1] - v[1]) + 
	   (b->table[2] - v[2]) * (b->table[2] - v[2]) ) < b->table[3] ;
}

void changement_boulier_calcule_boite(Boulier *o)
{
  int i, j, v, rayon ;

  for(i=0;i<3;i++)
    o->min[i] = o->max[i] = o->t.table[0].table[i] ;

  for(i=0; i<o->t.lar; i++)
    {
      rayon = sqrt(o->t.table[i].table[3]) + 1 ;
      for(j=0; j<3; j++)
	{
	  v =  o->t.table[i].table[j] - rayon ;
	  if ( v < o->min[j] )
	    o->min[j] = v ;
	  
	  v =  o->t.table[i].table[j] + rayon ;
	  if ( v > o->max[j] )
	    o->max[j] = v ;
	}
    }

  for(i=0; i<3; i++)
    o->taille[i] = o->max[i] - o->min[i] + 1 ;
}

#define FOR_VOXELS_BOULE(BOULE, INDICE, CORPS)	\
{						\
  int INDICE[3] ;				\
  int rayon = sqrt((BOULE)->table[3]) + 1 ;	\
  for(INDICE[2] = (BOULE)->table[2] - rayon;	\
      INDICE[2] <= (BOULE)->table[2] + rayon;	\
	  INDICE[2]++)				\
    for(INDICE[1] = (BOULE)->table[1] - rayon;	\
	INDICE[1] <= (BOULE)->table[1] + rayon;	\
	INDICE[1]++)				\
      for(INDICE[0] = (BOULE)->table[0] - rayon;	\
	  INDICE[0] <= (BOULE)->table[0] + rayon;	\
      INDICE[0]++)				\
	if ( voxel_dans_boule(BOULE, INDICE) ) CORPS \
}

#define FOR_VOXELS_BOULIER(O, INDICE, CORPS)		\
{							\
  int INDICE[3] ;					\
  for(INDICE[2] = o->min[2];				\
      INDICE[2] <= o->max[2];				\
      INDICE[2]++)					\
    {							\
      eprintf("for voxel %d/%d\r", INDICE[2], o->max[2]) ;	\
      for(INDICE[1] = o->min[1];			\
	  INDICE[1] <= o->max[1];			\
	  INDICE[1]++)					\
	for(INDICE[0] = o->min[0];			\
	    INDICE[0] <= o->max[0];			\
	    INDICE[0]++)				\
	  CORPS	;					\
    }							\
}

Booleen sphere_incluse(t_l4_int *b1, t_l4_int *b2)
{
  FOR_VOXELS_BOULE(b1, v, 
	     {
	       if ( ! voxel_dans_boule(b2, v) )
		 return 0 ;
	     }
	     )
  return 1 ;
}

void changement_boulier_cherche_spheres_incluses(Boulier *o)
{
  int i, j ;

  if ( o->t.lar > 100 )
    return ;

  for(i=0;i<o->t.lar;i++)
    for(j=0;j<o->t.lar;j++)
      if ( i != j )
	if ( sphere_incluse(&o->t.table[i], &o->t.table[j]) )
	  {
	    NOTE_CHAMP_INVALIDE(o, t) ;
	    return ;
	  }
}

typedef struct
{
  unsigned int bord:1 ;
  unsigned int dans_requise:1 ;
  unsigned int numero_boule:14 ;
  unsigned int nb_boules:16 ;
} Voxel ;

#define VOXEL(B, V) ((Voxel*)&(B)->voxels.table[V[0]-(B)->min[0] +	\
				      (B)->taille[0]*(V[1] -(B)->min[1]	+	\
						      (B)->taille[1]*(V[2]-(B)->min[2]))])



void changement_boulier_voxels(Boulier *o)
{
  int i, j, k, vv[3] ;
  Voxel *voxel ;

  o->voxels.lar = o->taille[0] * o->taille[1] * o->taille[2] ;
  REALLOUER(o->voxels.table, o->voxels.lar) ;
  for(i=0; i<o->voxels.lar; i++)
    o->voxels.table[i] = 0 ;

  o->requise.lar = o->t.lar ;
  REALLOUER(o->requise.table, o->t.lar) ;
  for(i=0; i<o->requise.lar; i++)
    o->requise.table[i] = 0 ;

  for(i=0; i<o->t.lar; i++)
    {
      eprintf("projete boule %d/%d\r", i, o->t.lar) ;
      FOR_VOXELS_BOULE(&o->t.table[i], v,
		       {
			 voxel = VOXEL(o, v) ;
			 voxel->nb_boules++ ;
			 voxel->numero_boule = i ;
		       }
		       ) ;
    }

  
  FOR_VOXELS_BOULIER(o, v,
		     {
		       voxel = VOXEL(o, v) ;
		       if ( voxel->nb_boules == 1 )
			 {
			   o->requise.table[voxel->numero_boule] = 1 ;
			 }
		     }
		     ) ;

  // Pour toutes les boules requises, note les voxels recouverts
  j = 0 ;
  k = -1 ;
  for(i=0; i<o->t.lar; i++)
    {
      eprintf("pixel dans boule requise %d/%d\r", i, o->t.lar) ;
      if ( o->requise.table[i] )
	FOR_VOXELS_BOULE(&o->t.table[i], v,
			 {
			   VOXEL(o, v)->dans_requise = 1 ;
			 }    
			 );
    }

  // Détection bord
  FOR_VOXELS_BOULIER(o, v,
		     {
		       voxel = VOXEL(o, v) ;
		       if ( v[0] == o->min[0] || v[0] == o->max[0] ||
			    v[1] == o->min[1] || v[1] == o->max[1] ||
			    v[2] == o->min[2] || v[2] == o->max[2] )
			 {
			   voxel->bord = 1 ;
			   continue ;
			 }
		       j = !!voxel->nb_boules ;
		       for(i=0; i<6; i++)
			 {
			   vv[0] = v[0] + dx[i] ;
			   vv[1] = v[1] + dy[i] ;
			   vv[2] = v[2] + dz[i] ;
			   if ( !!VOXEL(o, vv)->nb_boules != j )
			     {
			       voxel->bord = 1 ;
			       break ;
			     }
			 }
		     }
		     ) ;
}


void changement_boulier(Boulier *o)
{
  if ( CHAMP_CHANGE(o, t) )
    {
      changement_boulier_calcule_boite(o) ;
      changement_boulier_cherche_spheres_incluses(o) ;
      changement_boulier_voxels(o) ;
    }
}

Booleen coords_dans_boulier(Boulier *o, int v[3])
{
  return v[0] >= o->min[0] && v[0] <= o->max[0]
    && v[1] >= o->min[1] && v[1] <= o->max[1]
    && v[2] >= o->min[2] && v[2] <= o->max[2] ;
}

void affiche_gl_voxels(Boulier *o)
{
  int i, j, nb, valeurs[4] ;
  Voxel *voxel, *voxel2 ;
  int voisin[3] ;
  float c[3] ;
  static int normale[][3]={{-1,0,0},{1,0,0},{0,-1,0},{0,1,0},{0,0,-1},{0,0,1}};
  static int f[][4][3] =
    {
      { {0,0,0}, {0,0,1}, {0,1,1}, {0,1,0} },
      { {1,0,0}, {1,1,0}, {1,1,1}, {1,0,1} },
      { {0,0,0}, {1,0,0}, {1,0,1}, {0,0,1} },
      { {0,1,0}, {0,1,1}, {1,1,1}, {1,1,0} },
      { {0,0,0}, {1,0,0}, {1,1,0}, {0,1,0} },
      { {0,0,1}, {0,1,1}, {1,1,1}, {1,0,1} },
    } ;
  static int carre[][3][3] = {
    {{0,1,0},{0,1,1},{0,0,1}},
    {{1,0,0},{1,0,1},{0,0,1}},
    {{1,0,0},{1,1,0},{0,1,0}},
  } ;

  /* Voxel plein */
  glEnable(GL_LIGHTING) ;
  glBegin(GL_QUADS) ;
  FOR_VOXELS_BOULIER(o, v,
		     {		       
		       voxel = VOXEL(o, v) ;
		       /* Si vide ou voxel de boule requise : affiche rien */
		       if ( voxel->nb_boules == 0 || voxel->dans_requise)
			 continue ;

		       if ( o->cache_voxels_bord && voxel->bord )
			 continue ;

		       nb = voxel->nb_boules ;
		       if ( 1 ) // Colorise
			 {
			   nb-- ;
			   if ( nb > 7 )
			     nb = 7 ;
			   c[0] = nb&1 ;
			   c[1] = (nb&2)/2 ;
			   c[2] = (nb&4)/4 ;
			   glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, c);
			 }

		       for(i=0; i<6; i++)
			 {
			   voisin[0] = v[0] + dx[i] ;
			   voisin[1] = v[1] + dy[i] ;
			   voisin[2] = v[2] + dz[i] ;
			   if ( coords_dans_boulier(o, voisin) )
			     {
			       voxel2 = VOXEL(o, voisin) ;
			       if ( voxel2->nb_boules && !voxel2->dans_requise)
				 continue ;
			     }
			   glNormal3iv(normale[i]) ;
			   for(j=0; j<4; j++)
			     glVertex3f(v[0]+f[i][j][0],
					v[1]+f[i][j][1],
					v[2]+f[i][j][2]) ;
			   
			 }
		     }
		     ) ;
  glEnd() ;

  if ( o->cache_filaire )
    return ;

  glPushAttrib(GL_LINE_BIT) ;
  glDisable(GL_LIGHTING) ;
  glLineWidth(1) ;
  glBegin(GL_LINES) ;
  FOR_VOXELS_BOULIER(o, v,
		     {		       
		       valeurs[0] = !!VOXEL(o, v)->nb_boules ;

		       for(i=0; i<3; i++) // Axes 1,2,4
			 {
			   nb = valeurs[0] ;
			   for(j=0; j<3; j++)
			     {
			       voisin[0] = v[0] + carre[i][j][0] ;
			       voisin[1] = v[1] + carre[i][j][1] ;
			       voisin[2] = v[2] + carre[i][j][2] ;
			       if ( ! coords_dans_boulier(o, voisin)
				    || VOXEL(o, voisin)->nb_boules == 0 )
				 valeurs[j+1] = 0 ;
			       else
				 {
				   valeurs[j+1] = 1 ;
				   nb++ ;
				 }
			     }
			   if ( nb == 0 || nb == 4 )
			     continue ;
			   if ( nb == 2 &&
				valeurs[0] != valeurs[2] )
			     continue ;

			   glVertex3f(v[0]+carre[i][1][0],
				      v[1]+carre[i][1][1],
				      v[2]+carre[i][1][2]) ;
			   glVertex3f(v[0]+carre[i][1][0]+(i==0),
				      v[1]+carre[i][1][1]+(i==1),
				      v[2]+carre[i][1][2]+(i==2)
				      ) ;
			 }
		     }
		     )
    glEnd() ;
    glPopAttrib() ;

}

void affiche_gl_spheres(Boulier *o)
{
  int i ;
  
  GLUquadricObj* q = gluNewQuadric  ( );
  gluQuadricDrawStyle ( q, GLU_FILL   );
  gluQuadricNormals   ( q, GLU_SMOOTH );
  glEnable(GL_LIGHTING) ;
  
  for(i=0; i<o->t.lar; i++)
    {
      glPushMatrix() ;
      glTranslatef(o->t.table[i].table[0],
		   o->t.table[i].table[1],
		   o->t.table[i].table[2]);
      gluSphere ( q, sqrt(o->t.table[i].table[3]), 10, 10 );
      glPopMatrix() ;
    }
  glDisable(GL_LIGHTING) ;
  
  gluDeleteQuadric ( q );
}


static void affiche_table(Boulier *o)
{
  if ( globale_etat_courant.type_affichage == T_PONCTUEL )
    t_table(o, c_trouve(o,"t"), T_PONCTUEL) ;
  else
    if ( ACTIF(o) )
      affiche_gl_voxels(o) ;
    else
      affiche_gl_spheres(o) ;
}


CLASSE(boulier, Boulier,
       CHAMP(t, L_table_point4 P_table_quadruplet_int
	     Extrait Edite Sauve)
       CHAMP(min, L_point P_triplet_int Affiche)
       CHAMP(max, L_point P_triplet_int Affiche)
       CHAMP(requise, L_table_nombre P_table_int Affiche)
       CHAMP(cache_voxels_bord, L_booleen Edite Sauve)
       CHAMP(cache_filaire, L_booleen Edite Sauve)
       CHAMP_VIRTUEL(L_affiche_gl(affiche_table)
		     AFFICHAGE(Different_suivant_qualite))
       CHAMP_VIRTUEL(L_tag(ecrit_tag_table,lit_tag_table))
       CHANGEMENT(changement_boulier)
       MENU("Figures/3d/Boulier")
       EVENEMENT("Shft+Ctrl+BO")
       )


