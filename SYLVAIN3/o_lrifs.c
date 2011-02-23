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
#include <math.h>		/* Pour sin et cos */
#include <GL/gl.h>		/* Pour OpenGL */
#include "o_objet.h"
#include "t_geometrie.h"	/* Pour Triplet */
#include "u_graphique.h"	/* Pour t_texte */
#include "u_etat.h"
#include "matrice.h"
#include "io.h"
#include "t_ifs.h"
#include "u_graphique.h"
#include "u_vue.h"
#include "u_etat_courant.h"
#include "pov.h"

/*
 *****************************************************************************
 *****************************************************************************
 * OBJET LRIFS : STRUCTURES ET OUTILS COMMUNS
 *****************************************************************************
 *****************************************************************************
 */

struct lrifs
{
  int	niveau;			/* Nombre de récursions */
  matrice *p[10] ;		/* Matrices à différentes puissances */
  Triplet x, y ;		/* Pour éclatement affichage */
  GLuint list;			/* La primitive à afficher */
} ;

static matrice* trouve_matrice_puissance(matrice *p[10], int n)
{
  if ( n < 1 )
    n = 1 ;
  if ( n > 9 )
    n = 9 ;
  if ( n==1 )
    return(p[1]) ;
  if ( !p[n] )
    p[n] = matrice_multiplie(trouve_matrice_puissance(p, n/2),
			     trouve_matrice_puissance(p, (n+1)/2)) ;
  return( p[n] ) ;
}

static matrice* trouve_matrice(matrice *p[10],int puissance,const char *urgent)
{
  return( trouve_matrice_puissance(p, (atoi(urgent)-5) + puissance) ) ;
}

static void detruit_lrifs(struct lrifs *o)
{
  int i ;

  for(i=1;i<TAILLE_TABLE(o->p);i++)
    {
      matrice_libere(o->p[i]);
      o->p[i] = NULL ;
    }
}

/*
 *****************************************************************************
 * crée l'affichage POV correspondant
 *****************************************************************************
 */

static void liste_cree_pov(liste_transfo *l, struct lrifs *o, FILE *f)
{
  Triplet pt ;
  Quadruplet q ;

  for(;l;l=l->suc)
    {
      if ( (*transfo_origine)(l->mat, &pt, 2) )
	{
	  q.x = pt.x ;
	  q.y = pt.y ;
	  q.z = pt.y ;
	  q.h = 0 ;
	  o_positionne(FILS(o,0), &q, 0) ;
	  fprintf(f, "union {\n") ;
	  c_sauve_type(FILS(o,0), &infos_L_affiche_pov, int_vers_char((int)f)) ;
	  if ( (*transfo_origine)(l->mat, &pt, 1) )
	    pigment(f, pt.x, pt.y, pt.z) ;
	  fprintf(f,"}\n");
	}
      else
	{
	  fprintf(f,"object { o%x\n", (int)o);
	  transfo_affiche_pov(l->mat, f) ;
	  if ( (*transfo_origine)(l->mat, &pt, 1) )
	    pigment(f, pt.x, pt.y, pt.z) ;
	  fprintf(f,"}\n");
	}
    }
}

static void matrice_cree_pov(matrice *m
			     , FILE *f
			     , struct lrifs *o
			     )
{
  int i,j;

  if ( m == NULL )
    return ;
  for(i=0;i<m->dim;i++)
    for(j=0;j<m->dim;j++)
      {
	fprintf(f, "union {\n") ;
	liste_cree_pov(m->m[i][j], o, f);
	fprintf(f, "translate <%g, %g, %g>\n}\n"
		, i*o->x.x+j*o->y.x, i*o->x.y+j*o->y.y, i*o->x.z+j*o->y.z
		) ;
      }	
}

static void affiche_lrifs_pov(struct lrifs *o, const Champ *c, const char*v)
{
  FILE *f ;

  if ( v[0]=='\0' ) return ;
  if ( OBJET(o)->fils == NULL )
    return ;

  f = (FILE*)atoi(v) ;

  fprintf(f, "#declare o%x = ", (int)o) ;
  c_sauve_type(FILS(o,0), &infos_L_affiche_pov, v) ;

  fprintf(f, "union {\n") ;
  matrice_cree_pov(trouve_matrice_puissance(o->p, o->niveau), f, o) ;
  fprintf(f, "}\n") ;
}

/*
 *****************************************************************************
 * Affichage GL
 *****************************************************************************
 */

static void liste_cree_gl(liste_transfo *l, GLuint list, struct lrifs *o
			  , const char *v)
{
  Transfo tm ;
  Triplet t ;
  Quadruplet q ;
  GLfloat m[4] ;
  int affiche_liste ;

  while(l!=NULL)
    {
      affiche_liste = 1 ;
      if ( (*transfo_origine)(l->mat, &t, 1) )
	{
	  m[0] = t.x ;
	  m[1] = t.y ;
	  m[2] = t.z ;
	  m[3] = 1 ;
	  glColor3fv(m) ;
	  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, m) ;

	  if ( (*transfo_origine)(l->mat, &t, 2) )
	    {
	      q.x = t.x ;
	      q.y = t.y ;
	      q.z = t.y ;
	      q.h = 0 ;
	      o_positionne(FILS(o,0), &q, 0) ;
	      affiche_liste = 0 ;
	    }
	}
      glPushMatrix();
      (*transfo_extrait)(l->mat, &tm, 0);
      u_change_repere(&tm) ;
      if ( affiche_liste && list != 0 )
	glCallList(list);
      else
	{
	  destruction_liste_objet(FILS(o,0)) ;
	  u_affiche_objet(FILS(o,0), v) ;
	}
      glPopMatrix();	
      l=l->suc;
    }
}

static void matrice_cree_gl(matrice *m, GLuint list, struct lrifs *o
			    , const char *v)
{
  int i,j;

  if ( m == NULL )
    return ;

  glPushAttrib( GL_CURRENT_BIT|GL_LIGHTING_BIT ) ;
  for(i=0;i<m->dim;i++)
    for(j=0;j<m->dim;j++)
      {
	glPushMatrix() ;
	glTranslatef(i*o->x.x+j*o->y.x, i*o->x.y+j*o->y.y, i*o->x.z+j*o->y.z) ;
	liste_cree_gl(m->m[i][j], list, o, v);
	glPopMatrix() ;
      }
  glPopAttrib() ;
}

static void affiche_lrifs_gl(struct lrifs *o, const Champ *c, const char*v)
{
  void *obj;
  GLuint list ;
  GLint param;

  if ( OBJET(o)->fils == NULL )
    return ;
  if ( o->p[1] == NULL )
    return ;

  Gl_Affiche_Erreur(o, NULL, "Début affiche_lrifs_gl") ;

  transfo_n_4x4_utilise() ;
  /* le premier fils est l'objet de base de la lrifs */
  obj=OBJET(o)->fils->objet;
  /*
   * Premier affichage invisible pour créer les Listes
   */
  /*
  glPushMatrix() ;
  glScalef(0,0,0) ;
  u_affiche_objet(obj, v) ;
  glPopMatrix() ;
  Gl_Affiche_Erreur(o, NULL, "Fin affichage primitive") ;
  */

  // Création d'une list si c'est possible
  glGetIntegerv(GL_LIST_INDEX, &param) ;

  if ( param )
    {
      matrice_cree_gl(trouve_matrice(o->p, o->niveau, v), 0, o, v) ;
    }
  else
    {
      /*
       * Création de la liste
       * Pour éviter d'appele u_affiche_objet tout le temps
       * car il est lent
       */
      list = glGenLists(1);
      glNewList(list, GL_COMPILE);
      u_affiche_objet(obj, v) ;
      glEndList();

      Gl_Affiche_Erreur(o, NULL, "Fin création liste") ;

      matrice_cree_gl(trouve_matrice(o->p, o->niveau, v), list, o, v) ;
      glDeleteLists(list, 1);
    }



  Gl_Affiche_Erreur(o, NULL, "Fin affiche_lrifs_gl") ;
}

/*
 *****************************************************************************
 * affichage STL
 *****************************************************************************
 */

static void liste_cree_stl(liste_transfo *l, void *obj, const char*v)
{
  Transfo t, t2 ;
  Quadruplet q ;
  Triplet pt ;
  
  t = globale_etat_courant.transfo ;
  while(l!=NULL)
    {
      (*transfo_extrait)(l->mat, &t2, 0);
      globale_etat_courant.transfo = u_produit_transfo(&t, &t2) ;

      if ( (*transfo_origine)(l->mat, &pt, 1) )
	{
	  globale_etat_courant.couleur.x = pt.x ;
	  globale_etat_courant.couleur.y = pt.y ;
	  globale_etat_courant.couleur.z = pt.z ;
	}
      if ( (*transfo_origine)(l->mat, &pt, 2) )
	{
	  q.x = pt.x ;
	  q.y = pt.y ;
	  q.z = pt.y ;
	  q.h = 0 ;
	  o_positionne(obj, &q, 0) ;
	}
      c_sauve_type(obj, &infos_L_affiche_stl, v) ;
      l=l->suc;
    }
}

static void matrice_cree_stl(matrice *m, void *obj,
			     const Triplet *x, const Triplet *y, const char*v)
{
  int i,j;
  Transfo t ;
  Triplet c ;

  if ( m == NULL )
    return ;
	
  t = globale_etat_courant.transfo ;
  c = globale_etat_courant.couleur ;
  for(i=0;i<m->dim;i++)
    for(j=0;j<m->dim;j++)
      {
	globale_etat_courant.transfo = t ;
	globale_etat_courant.transfo.origine.x = t.origine.x + i*x->x+j*y->x ;
	globale_etat_courant.transfo.origine.y = t.origine.y + i*x->y+j*y->y ;
	globale_etat_courant.transfo.origine.z = t.origine.z + i*x->z+j*y->z ;
	liste_cree_stl(m->m[i][j], obj, v);
      }
  globale_etat_courant.transfo = t ;
  globale_etat_courant.couleur = c ;
}


static void affiche_lrifs_stl(struct lrifs *o,
				 const Champ *c, const char*v)
{
  /* le premier fils est l'objet de base de la lrifs */
  fprintf(stderr,"v=%s\n", v);
  if ( FILS(o,0) == NULL )
    return ;
  matrice_cree_stl(trouve_matrice(o->p, o->niveau, strchr(v,' ')+1)
		   , FILS(o,0), &o->x, &o->y, v);
}

/*
 *****************************************************************************
 * affichage OBJ
 *****************************************************************************
 */

static void liste_cree_obj(liste_transfo *l, void *obj, const char*v)
{
  Transfo t, t2 ;
  Quadruplet q ;
  Triplet pt ;
  
  t = globale_etat_courant.transfo ;
  while(l!=NULL)
    {
      (*transfo_extrait)(l->mat, &t2, 0);
      globale_etat_courant.transfo = u_produit_transfo(&t, &t2) ;

      if ( (*transfo_origine)(l->mat, &pt, 1) )
	{
	  globale_etat_courant.couleur.x = pt.x ;
	  globale_etat_courant.couleur.y = pt.y ;
	  globale_etat_courant.couleur.z = pt.z ;
	}
      if ( (*transfo_origine)(l->mat, &pt, 2) )
	{
	  q.x = pt.x ;
	  q.y = pt.y ;
	  q.z = pt.y ;
	  q.h = 0 ;
	  o_positionne(obj, &q, 0) ;
	}
      c_sauve_type(obj, &infos_L_affiche_obj, v) ;
      l=l->suc;
    }
}

static void matrice_cree_obj(matrice *m, void *obj,
			     const Triplet *x, const Triplet *y, const char*v)
{
  int i,j;
  Transfo t ;
  Triplet c ;

  if ( m == NULL )
    return ;
	
  t = globale_etat_courant.transfo ;
  c = globale_etat_courant.couleur ;
  for(i=0;i<m->dim;i++)
    for(j=0;j<m->dim;j++)
      {
	globale_etat_courant.transfo = t ;
	globale_etat_courant.transfo.origine.x = t.origine.x + i*x->x+j*y->x ;
	globale_etat_courant.transfo.origine.y = t.origine.y + i*x->y+j*y->y ;
	globale_etat_courant.transfo.origine.z = t.origine.z + i*x->z+j*y->z ;
	liste_cree_obj(m->m[i][j], obj, v);
      }
  globale_etat_courant.transfo = t ;
  globale_etat_courant.couleur = c ;
}


static void affiche_lrifs_obj(struct lrifs *o,
				 const Champ *c, const char*v)
{
  /* le premier fils est l'objet de base de la lrifs */
  fprintf(stderr,"v=%s\n", v);
  if ( FILS(o,0) == NULL )
    return ;
  matrice_cree_obj(trouve_matrice(o->p, o->niveau, strchr(v,' ')+1)
		   , FILS(o,0), &o->x, &o->y, v);
}

CLASSE(lrifs_commun, struct lrifs,

       BULLE_D_AIDE("Cet objet permet de créer un IFS ou LR-IFS\n")
       PAQUET(NB_ACTIFS)
       MENU("NON")
  
       CHAMP(niveau,
	     L_nombre_positif P_entier
	     Edite Sauve
	     LABEL("Niveau de details")
	     DEFAUT("1")
	     BULLE_D_AIDE("Nombre de recursions de la lrifs")
	     )
       CHAMP(x, L_vecteur P_triplet DEFAUT("0 0 0") Init3d Edite Sauve)
       CHAMP(y, L_vecteur P_triplet DEFAUT("0 0 0") Init3d Edite Sauve)
       CHAMP(p[1], L_matrice_ifs) /* Pour pouvoir être extrait */

       CHAMP_VIRTUEL(L_affiche_gl(affiche_lrifs_gl)
		     AFFICHAGE(Toujours_different))
       CHAMP_VIRTUEL(L_affiche_stl(affiche_lrifs_stl))
       CHAMP_VIRTUEL(L_affiche_obj(affiche_lrifs_obj))
       CHAMP_VIRTUEL(L_valeur Obligatoire)
       CHAMP_VIRTUEL(L_affiche_pov(affiche_lrifs_pov))

       DESTRUCTION(detruit_lrifs)
       )

/*
 *****************************************************************************
 *****************************************************************************
 * OBJET LRIFS : FEUILLE
 *****************************************************************************
 *****************************************************************************
 */

struct lrifs_feuille
{
  struct lrifs lrifs ;
  int	taille ;		/* Largeur et Hauteur de la matrice */
  Booleen seulement_les_actives ;
  Booleen affiche_transformations ;
} ;

/*
 * Évalue la transformation
 */
static Transformation *fait_transfo(const void *tr)
{
  Transformation *trm, *a, *b ;
  const char *nom_fils;
  const Champ *c ;
	
  nom_fils=OBJET(tr)->classe->nom;

  if (strcmp(nom_fils,"applique")==0)
    {
      /*
       * On fait le produit des transformations sous le applique
       */
      trm = (*transfo_identite)() ;
      POUR_LES_FILS(tr, o,
		    {
		      a = trm ;
		      b = fait_transfo(o) ;
		      trm = (*transfo_multiplie)(a,b) ;
		      (*transfo_libere)(a) ;
		      (*transfo_libere)(b) ;
		    }
		    );
    }
  else
    {
      /*
       * Transformation de base
       */
      if ( !c_trouve_type(&tr, &c, &infos_L_transfo) )
	{
	  /*
	   * Transformation inconnue, on retourne l'identité
	   */
	  return( (*transfo_identite)() ) ;
	}
      /*
       * On retourne la transformation de base
       */
      trm = (*transfo_create)(&T(Transfo, tr, c)) ;
    }

  return trm;
}
/*
 * Rempli la matrice de transformation.
 * En créant les listes de transformations.
 */
static void rempli_matrice(struct lrifs_feuille *o)
{
  Liste *lobj, *lobj2;
  int ligne, col ;

  ligne = 0 ;
  col = 0 ;

  for( lobj=OBJET(o)->fils->suivant ; lobj ; lobj=lobj->suivant)
    {
      if ( ligne == o->taille )
	{
	  NOTE_CHAMP_INVALIDE(o, taille) ;
	  return ;
	}
      if ( lobj->objet==NULL )
	continue ;
      if ( o->seulement_les_actives && !ACTIF(lobj->objet) )
	continue ;

      if ( strcmp(OBJET(lobj->objet)->classe->nom, "lrifs") == 0 )
	{
	  for( lobj2=OBJET(lobj->objet)->fils->suivant ; lobj2 ;
	       lobj2=lobj2->suivant)
	    if ( lobj2->objet )
	      matrice_affecte(o->lrifs.p[1],ligne,col,fait_transfo(lobj2->objet));
	  col++ ;
	  if ( col == o->taille )
	    {
	      ligne++ ;
	      col = 0 ;
	    }
	}
      else
	{
	  matrice_affecte(o->lrifs.p[1],ligne,col,fait_transfo(lobj->objet));
	}
    }
  if ( o->taille != 1 && ligne != o->taille )
    NOTE_CHAMP_INVALIDE(o, taille) ;
}

static void changement_lrifs(struct lrifs_feuille *o)
{
  transfo_n_4x4_utilise() ;
  detruit_lrifs(&o->lrifs) ;
  o->lrifs.p[1] = matrice_nouvelle(o->taille);	
  rempli_matrice(o);
}


static char *info_lrifs(struct lrifs_feuille *o)
{
  static char info[100];
	
  sprintf(info,"Mat. %dx%d Prof. = %i",o->taille, o->taille, o->lrifs.niveau);
  return info;
}

static void affiche_lrifs_feuille_gl(struct lrifs_feuille *o
				     , const Champ *c, const char*v)
{
  if (o->affiche_transformations)
    {
      POUR_LES_FILS(o, f,
                    if ( i_f )
                    u_affiche_objet(f, "") ;
                    ) ;
    }
  affiche_lrifs_gl(&o->lrifs, c, v) ;
}
	     
CLASSE(lrifs, struct lrifs_feuille,
       CHAMP_VIRTUEL(L_affiche_gl(affiche_lrifs_feuille_gl)
		     AFFICHAGE(Toujours_different))
       SUR_CLASSE("lrifs_commun")
       EVENEMENT("LRIFS")	 /* Raccourci clavier */
       MENU("Figures/?d/LR-IFS")
       CHAMP(taille,
	     L_nombre_positif P_entier
	     Edite Sauve
	     LABEL("Taille de la matrice")
	     DEFAUT("1")
	     BULLE_D_AIDE("Nombre de colonne et lignes de la matrice NxN")
	     )
       CHAMP(affiche_transformations, L_booleen Edite Sauve DEFAUT("0")
	     BULLE_D_AIDE("Si vrai alors seulement les transformations\n"
			  "utilisées sont affichées\n"
			  )
	     )
       CHAMP(seulement_les_actives, L_booleen Edite Sauve DEFAUT("0")
	     BULLE_D_AIDE("Si vrai alors seulement les transformations\n"
			  "active au sens de NanoÉdit sont incluses\n"
			  "dans l'LRIFS\n")
	     )
       CHANGEMENT(changement_lrifs)
       )

/*
 *****************************************************************************
 *****************************************************************************
 * OBJET LRIFS : NOEUD
 *****************************************************************************
 *****************************************************************************
 */

struct lrifs_compose
{
  struct lrifs lrifs ;
  matrice **m1;			/* La matrice de la premiere lrifs */
  matrice **m2;			/* La matrice de la deuxieme lrifs */
  Menu type;			/* type de composition */
} ;
  


static char *info_composition(struct lrifs_compose *o)
{
  static char info[50];
  
  if ( o->lrifs.p[1] == NULL )
    return("?") ;
  
  sprintf(info,"%s %d %dx%d",o->type.selection, o->lrifs.niveau,
	  o->lrifs.p[1]->dim, o->lrifs.p[1]->dim);
  return info;
}	
	
static matrice* matrice_compose(matrice *m1, matrice *m2, int couleur)
{
  liste_transfo *l1, *l2 ;
  Transformation *tr ;
  Transfo m ;
  int i, j ;
  matrice *mat ;

  if ( m1->dim != m2->dim )
    return(NULL) ;

  transfo_n_4x4_utilise() ;
  mat = matrice_nouvelle(m1->dim) ;

  for(j=0; j<m1->dim; j++)
    for(i=0; i<m1->dim; i++)
      {
	l1 = m1->m[j][i] ;
	l2 = m2->m[j][i] ;
	while( l1 && l2 )
	  {
	    tr = (*transfo_alloue)() ;
	    mat->m[j][i] = liste_ajoute(mat->m[j][i], tr) ;

	    /*
	     * La géometrie du premier
	     */
	    (*transfo_extrait)(l1->mat, &m, 0) ;
	    (*transfo_ajoute)(tr, &m) ;

	    if ( couleur )
	      {
		/*
		 * La couleur du deuxième
		 */  
		(*transfo_extrait)(l2->mat, &m, 0) ;
		(*transfo_ajoute)(tr, &m) ;
		/*
		 * La forme du premier
		 */
		if ( (*transfo_extrait)(l1->mat, &m, 2) )
		  (*transfo_ajoute)(tr, &m) ;
	      }
	    else
	      {
		/*
		 * La couleur du premier
		 */
		if ( ! (*transfo_extrait)(l1->mat, &m, 1) )
		  {
		    u_identite_transfo(&m) ;
		    m.origine.x = 0.5 ;
		    m.origine.y = 0.5 ;
		    m.origine.z = 0.5 ;
		  }
		(*transfo_ajoute)(tr, &m) ;
		/*
		 * La forme du deuxième
		 */
		(*transfo_extrait)(l2->mat, &m, 0) ;
		(*transfo_ajoute)(tr, &m) ;
	      }
	    /*
	     *
	     */
	    l1 = l1->suc ;
	    l2 = l2->suc ;
	  }
	mat->m[j][i] = liste_inverse(mat->m[j][i]) ;
      }

  return(mat) ;
}

static matrice* calcul_composition(int type, matrice *m1, matrice *m2)
{	
  if ( m1 == NULL || m2 == NULL )
    return(NULL) ;
  
  switch(type)
    {
    default: return(                        NULL) ;
    case 0 : return(        matrice_union(m1,m2)) ;
    case 1 : return(matrice_concatenation(m1,m2)) ;
    case 2 : return(      matrice_melange(m1,m2)) ;
    case 3 : return(   matrice_additionne(m1,m2)) ;
    case 4 : return(    matrice_multiplie(m1,m2)) ;
    case 5 : return(      matrice_compose(m1,m2,1)) ;
    case 6 : return(      matrice_compose(m1,m2,0)) ;
    }
}
		
static void changement_composition(struct lrifs_compose *o)
{	
  if ( o->m1 == NULL || o->m2 == NULL || *o->m1 == NULL || *o->m2 == NULL )
    return ;

  if ( TRACE(o) )
    eprintf("o->m1 = %d o->m2 = %d\n", *o->m1,*o->m2) ;

  if ( FILS_CHANGE(o) || CHAMP_CHANGE(o, type) )
    {
      detruit_lrifs(&o->lrifs) ;
      o->lrifs.p[1] = calcul_composition(o->type.index, *o->m1,*o->m2) ;
    }
}


CLASSE(lrifs_compose, struct lrifs_compose,
       SUR_CLASSE("lrifs_commun")
       MENU("Figures/?d/LR-IFS")
       EVENEMENT("LRIFS")	 /* Raccourci clavier */
       CHAMP(m1, L_pointeur_extrait TYPE_DANS_FILS(L_matrice_ifs) Extrait Obligatoire)
       CHAMP(m2, L_pointeur_extrait TYPE_DANS_FILS(L_matrice_ifs) Extrait Obligatoire)
       CHAMP(type,
	     LABEL("Type de composition")
	     P_option_menu E_option_menu
	     TE_TEXTE(
		      "Union\n"
		      "Concatenation\n"
		      "Melange\n"
		      "Addition\n"
		      "Multiplication\n"
		      "Colorié : géométrie du deuxième\n"
		      "Forme   : géométrie du deuxième\n"
		      )
	     DEFAUT("Union")
	     Edite Sauve
	     )
       BULLE_D_AIDE("Cet objet permet de composer deux lrifs\n")
       CHANGEMENT(changement_composition)
       INFORMATIONS(info_composition)
       )

