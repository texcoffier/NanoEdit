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

/*
 * Un mini éditeur de graphes.
 * Noeuds : Position + Nom
 * Arête  : Nom
 */

#define BASETAG 1000		/* Numéro d'identification du premier atome */
#define BASETAG2 DEBUT_TAG_DEPLACABLE	/* Identification de lettre */

#define D 3			/* Dimension de l'espace */
#define POS(i)  (o->g.noeuds[i].donnee.position)
#define COUL(i)  (o->g.noeuds[i].donnee.couleur)
#define NOM_NOEUD(i)  (o->g.noeuds[i].donnee.chaine)
#define NOM_ARETE(i,j)  (o->g.noeuds[i].aretes[j].donnee)
#define DIM(PROG) do { int dim ; for(dim=0; dim<D; dim++) { PROG ; } } while(0)

typedef struct
{
  t_graphe_N_pcc_A_chaine g ;
  Booleen affiche_cercle ;
  Flottant rayon_cercle ;
  Booleen lettres_separees ;
  Booleen lettres_penchees ;
  Booleen editeur ;
  Booleen colorier_noeuds ;
  char *alphabet ;
  Flottant ecart_lettres ;
  Flottant ecart_ligne ;
  Flottant taille_lettres ;
} Graphe ;

typedef union
{
  struct
  {
    unsigned int padding:2 ;
    unsigned int depart:10 ;
    unsigned int arrivee:10 ;
    unsigned int lettre:10 ;
  } bits ;
  int entier ;
} CodeLettre ;
    
  
/*
 *
 */
static void vertex_carre(Triplet *centre, float cote, int plein)
{
  glPushMatrix() ;
  glPushAttrib(GL_CURRENT_BIT) ;

  glTranslatef(centre->x, centre->y, centre->z-0.1) ;
  glScalef(cote, cote, 1) ;

  if ( plein )
    glColor3f(1,1,0) ;
  else
    glColor3f(.5,.5,.5) ;

  glBegin(GL_QUADS) ;
  glVertex2i(-1,-1) ;
  glVertex2i(1,-1) ;
  glVertex2i(1,1) ;
  glVertex2i(-1,1) ;
  glEnd() ;

  glTranslatef(0,0,.05) ;
  glPopAttrib() ;
  glBegin(GL_LINE_LOOP) ;
  glVertex2i(-1,-1) ;
  glVertex2i(1,-1) ;
  glVertex2i(1,1) ;
  glVertex2i(-1,1) ;
  glEnd() ;

  glPopMatrix() ;
}
/*
 * Affiche ligne de lettres
 */
void affiche_ligne_de_lettres(Graphe *o, int i, int j, Triplet *p, Triplet *q)
{
  int k ;
  const char *s ;
  float longueur ;
  Triplet v, milieu, pos, debut ;
  CodeLettre cl ;
  char lettre[2] ;

  if ( o->editeur || (o->alphabet[0] && ACTIF(o)) )
    s = o->alphabet ;
  else
    s = NOM_ARETE(i,j) ;
  
  longueur = (strlen(s)-1) * 1.5*o->ecart_lettres ;
  v = u_soustrait_triplet(q, p) ;
  v = u_normalise_triplet(&v) ;
  milieu = u_milieu_2_triplet(p, q) ;
  pos = u_triplet_fois_flottant(&v, longueur/2) ;
  debut = u_soustrait_triplet(&milieu, &pos) ;

  for(k=strlen(s)-1; k>=0 ; k--)
    {
      pos = u_triplet_fois_flottant(&v, k*1.5*o->ecart_lettres) ;
      pos = u_ajoute_triplet(&debut, &pos) ;
      if ( o->editeur || (o->alphabet[0] && ACTIF(o)) )
	{
	  cl.bits.padding = 0 ;
	  cl.bits.depart = i ;
	  cl.bits.arrivee = j ;
	  cl.bits.lettre = k ;
	  glPushName(BASETAG2 + cl.entier) ;
	  if ( strchr( NOM_ARETE(i,j), o->alphabet[k] ) )
	    vertex_carre(&pos, o->taille_lettres, 1) ;
	  else
	    vertex_carre(&pos, o->taille_lettres, 0) ;
	  glPopName() ;
	}
      glPushMatrix() ;
      glTranslatef(pos.x, pos.y, pos.z) ;
      glScalef(o->taille_lettres, o->taille_lettres, 1) ;
      lettre[0] = s[k] ;
      lettre[1] = '\0' ;
      t_chaine_centree( lettre ) ;
      glPopMatrix() ;
    }
}

/*
 * Arète de soit vers soit
 * *-----/P
 * |    /
 * |   /
 * |  /
 * | /
 * |/
 * /
 *Q
 */
static void affiche_arete_sur_soit(Graphe *o, int n)
{
  int i, nb ;
  Triplet centre, p, p2, q2, q, v, z, direction ;

  z.x = 0 ;
  z.y = 0 ;
  z.z = 1 ;
  DIM( (&centre.x)[dim] = POS(n)[dim] ) ;
  u_annule_triplet(&direction) ;
  nb = 0 ;
  for(i=0; i<o->g.noeuds[n].nb_aretes; i++)
    {
      if ( i == o->g.noeuds[n].aretes[i].destination )
	continue ;
      nb++ ;
      DIM( (&v.x)[dim] = POS(o->g.noeuds[n].aretes[i].destination)[dim] ) ;
      v = u_soustrait_triplet(&v, &centre) ;
      v = u_normalise_triplet(&v) ;
      direction = u_soustrait_triplet(&direction, &v) ;
    }
  if ( nb == 0 )
    {
      direction.x = 2*o->rayon_cercle ;
    }
  else
    {
      direction = u_normalise_triplet(&direction) ;
      direction = u_triplet_fois_flottant(&direction, 2*o->rayon_cercle) ;
    }


  v = u_rotation_triplet(&direction, &z, M_PI/4 ) ;
  p = u_ajoute_triplet(&centre, &v) ;
  p2 = u_ajoute_triplet(&p, &v) ;

  v = u_rotation_triplet(&direction, &z, -M_PI/4 ) ;
  q = u_ajoute_triplet(&centre, &v) ;
  q2 = u_ajoute_triplet(&q, &v) ;


  glBegin(GL_LINE_STRIP) ;

  if ( o->affiche_cercle)
    v = u_barycentre_2_triplet(&centre
			       , o->rayon_cercle
			       /u_distance_triplet(&centre, &p)
			       , &p) ;
  else
    v = centre ;
  t_triplet(&v) ;
  t_triplet(&p) ;
  t_triplet(&q) ;
  if ( o->affiche_cercle)
    v = u_barycentre_2_triplet(&centre
			       , o->rayon_cercle
			       /u_distance_triplet(&centre, &q)
			       , &q) ;
  else
    v = centre ;
  t_triplet(&v) ;
  glEnd() ;
  t_fleche_proportionnelle(&q, &v, 0, 0.3) ;

  if ( o->editeur || o->lettres_separees || ( o->alphabet[0] && ACTIF(o) ) ) 
    {
      for(i=0;i<o->g.noeuds[n].nb_aretes; i++)
	if ( o->g.noeuds[n].aretes[i].destination == n )
	  {
	    affiche_ligne_de_lettres(o, n, i, &p2, &q2) ;
	    break ;
	  }
    }

}

/*
 *
 */
static void affiche_arete(Graphe *o, int i, int j)
{
  float d ;
  Triplet a, b, p, q ; 
  Transfo t ;

  if ( j == i )
    {
      affiche_arete_sur_soit(o, i) ;
    }
  else
    {
      DIM( (&a.x)[dim] = POS(i)[dim] ) ;
      DIM( (&b.x)[dim] = POS(j)[dim] ) ;
      
      t.v1 = u_soustrait_triplet(&a, &b) ;
      t.v1 = u_normalise_triplet(&t.v1) ;
      t.v1 = u_triplet_fois_flottant(&t.v1, o->ecart_ligne) ;

      t.v2.x = -t.v1.y ;
      t.v2.y = t.v1.x ;
      t.v2.z = t.v1.z ;
      
      if ( o->affiche_cercle )
	{
	  d = u_distance_triplet(&a, &b) ;
	  p = u_barycentre_2_triplet(&a, o->rayon_cercle/d,&b) ;
	  q = u_barycentre_2_triplet(&a, 1-o->rayon_cercle/d,&b) ;
	}
      else
	{
	  p = a ;
	  q = b ;
	}
      p = u_ajoute_triplet(&p, &t.v2) ;
      q = u_ajoute_triplet(&q, &t.v2) ;
      t_fleche_proportionnelle(&p, &q, 0, 0.03) ;
      glBegin(GL_LINES) ;
      t_triplet(&p) ;
      t_triplet(&q) ;
      glEnd() ;

      p = u_ajoute_triplet(&p, &t.v2) ;
      p = u_ajoute_triplet(&p, &t.v2) ;
      p = u_ajoute_triplet(&p, &t.v2) ;
      q = u_ajoute_triplet(&q, &t.v2) ;
      q = u_ajoute_triplet(&q, &t.v2) ;
      q = u_ajoute_triplet(&q, &t.v2) ;

      if ( o->editeur || o->lettres_separees || ( o->alphabet[0] && ACTIF(o) ) )
	{
	  affiche_ligne_de_lettres(o, i, j, &p, &q) ;
	  return ;
	}

      if ( o->lettres_penchees )
	{
	  t.origine = u_milieu_2_triplet(&p, &q) ;
	  t.v3.x = 0 ;
	  t.v3.y = 0 ;
	  t.v3.z = 1 ;
	  glPushMatrix() ;
	  u_change_repere(&t) ;
	  t_chaine_centree( NOM_ARETE(i,j) ) ;
	  glPopMatrix() ;
	  return ;
	}


      a = u_milieu_2_triplet(&p, &q) ;
      glRasterPos3f(a.x, a.y, a.z) ;
      t_chaine( NOM_ARETE(i,j) ) ;
    }
}

/*
 *
 */
static void affiche_sommet(Graphe *o, int i)
{
  Triplet p ;
  float d ;

  DIM( (&p.x)[dim] = POS(i)[dim] ) ;

  if ( o->affiche_cercle )
    {
      glPushMatrix() ;
      glTranslatef(p.x, p.y, p.z) ;
      d = o->rayon_cercle/strlen(NOM_NOEUD(i)) ;
      glScalef(d, d, 1) ;
      t_chaine_centree( NOM_NOEUD(i) ) ;
      glPopMatrix() ;
      if ( o->colorier_noeuds )
	{
	  glPushAttrib(GL_CURRENT_BIT) ;
	  glColor3f(COUL(i)[0], COUL(i)[1], COUL(i)[2]) ;
	  t_disque_centre_rayon(&p, o->rayon_cercle) ;
	  glPopAttrib() ;
	  t_cercle_centre_rayon(&p, o->rayon_cercle) ;
	}
      else
	t_cercle_centre_rayon(&p, o->rayon_cercle) ;
    }
  else
    {
      if ( !ACTIF(o) )
	t_triplet(&p) ;
    }

  if ( ACTIF(o) )
    t_tag(BASETAG+i, &p) ;
}
/*
 *
 */
static void affiche_graphe(Graphe *o)
{
  int i,j,k ;

  /* Affiche les arêtes */
  for(i=0;i<o->g.nb_noeuds;i++)
    {
      for(j=0;j<o->g.noeuds[i].nb_aretes;j++)
	{
	  k = o->g.noeuds[i].aretes[j].destination ;
	  affiche_arete(o, i, k) ;
	}
    }

  for(i=0;i<o->g.nb_noeuds;i++)
    affiche_sommet(o, i) ;
}

/*
 * L'utilisateur déplace un tag
 */
static void ecrit_tag(Graphe *o,const Champ *c,const char *v)
{
  int i ;
  CodeLettre cl ;
  char *s, **na ;

  i = lit_numero_tag(c) - BASETAG ;
  if ( i >= 0 && i < o->g.nb_noeuds )
    sscanf(v, "%g%g%g", POS(i), POS(i)+1, POS(i)+2) ;
  else
    {
      cl.entier = lit_numero_tag(c) - BASETAG2 ;
      s = strchr(NOM_ARETE(cl.bits.depart,cl.bits.arrivee)
		 ,o->alphabet[cl.bits.lettre]) ; 
      if ( s )
	{
	  strcpy_overlap(s, s+1) ;
	}
      else
	{
	  na = &NOM_ARETE(cl.bits.depart,cl.bits.arrivee) ;

	  CODEC_REALLOC(*na, strlen(*na)+2) ;
	  sprintf(*na+strlen(*na), "%c", o->alphabet[cl.bits.lettre]) ;
	}
    }
}
/*
 * Lecture de la position d'un tag
 */
static const char * lit_tag(Graphe *o, const Champ *c)
{
  static char buf[99] ;
  int i ;

  i = c->parametre - BASETAG ;
  if ( i >= 0 && i < o->g.nb_noeuds )
    sprintf(buf, "%g %g %g", POS(i)[0], POS(i)[1], POS(i)[2]) ;
  else
    strcpy(buf,
	   "Cliquez sur cette lettre pour l'ajouter/l'enlever\n"
	   "de la chaine de caractère associé à la lettre\n"
	   ) ;
  return(buf) ;
}

CLASSE(graphe, Graphe,
       Edite Sauve
       CHAMP(g, L_graphe_de_points P_graphe_N_pcc_A_chaine Extrait)
       DEBUT_BOITE(HORIZONTALE)
       CHAMP(affiche_cercle, L_booleen           DEFAUT("1"))
       CHAMP(rayon_cercle  , L_nombre P_flottant DEFAUT("0.1"))
       FIN_BOITE
       DEBUT_BOITE(HORIZONTALE)
       CHAMP(lettres_separees, L_booleen  DEFAUT("1"))
       CHAMP(lettres_penchees, L_booleen  DEFAUT("1"))
       CHAMP(alphabet, L_chaine  DEFAUT("abcd"))
       FIN_BOITE
       DEBUT_BOITE(HORIZONTALE)
       CHAMP(editeur, L_booleen  DEFAUT("0"))
       CHAMP(colorier_noeuds, L_booleen  DEFAUT("0"))
       FIN_BOITE
       CHAMP(ecart_lettres, L_nombre P_flottant Edite Sauve DEFAUT("0.1"))
       CHAMP(ecart_ligne, L_nombre P_flottant Edite Sauve DEFAUT("0.1"))
       CHAMP(taille_lettres, L_nombre P_flottant Edite Sauve DEFAUT("0.1"))
       CHAMP_VIRTUEL(L_affiche_gl(affiche_graphe)
		     AFFICHAGE(Toujours_different))
       CHAMP_VIRTUEL(L_tag(ecrit_tag,lit_tag))
       MENU("Divers/Graphe")
       EVENEMENT("GR")
       )
