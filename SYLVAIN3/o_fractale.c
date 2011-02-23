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
 * tout pour creer une fractale :-)
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
 * OBJET FRACTAL : AFFICHAGE
 *****************************************************************************
 */

struct fractale_commun
{
  int	niveau;			/* Nombre de récursions */
  matrice *p[10] ;		/* Matrices à différentes puissances */
  Triplet x, y ;		/* Pour éclatement affichage */
  Booleen seulement_les_actives ;
  Booleen affiche_transformations ;
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

static matrice* trouve_matrice(matrice *p[10],
			      int puissance,
			      const char *urgent)
{
  return( trouve_matrice_puissance(p, (atoi(urgent)-5) + puissance) ) ;
}

/*
 * crée l'affichage Pov correspondant
 */
static void liste_cree_pov(liste_transfo *l, int n, FILE *f)
{
  for(;l;l=l->suc)
    {
      fprintf(f,"object { o%x\n", n);
      transfo_affiche_pov(l->mat, f) ;
    }
}

static void ecrit_pov_fractale(struct fractale_commun *o, const Champ *c, const char *v)
{
  int i, j ;
  FILE *f ;
  Triplet *x, *y ;
  matrice *m ;

  if ( v[0]=='\0' ) return ;

  f = (FILE*)atoi(v) ;

  fprintf(f, "#declare o%x = ", (int)FILS(o,0)) ;
  c_sauve_type(FILS(o,0), &infos_L_affiche_pov, v) ;
  x = &o->x ;
  y = &o->y ;
  m = trouve_matrice_puissance(o->p, o->niveau) ;

  fprintf(f, "union {\n") ;
  for(i=0;i<m->dim;i++)
    for(j=0;j<m->dim;j++)
      {
	fprintf(f, "union {\n") ;
	liste_cree_pov(m->m[i][j], (int)FILS(o,0), f);
	fprintf(f, "translate <%g, %g, %g>\n}\n"
		, i*x->x+j*y->x, i*x->y+j*y->y, i*x->z+j*y->z
		) ;
      }
  fprintf(f, "}\n") ;
}

/*
 * Affichage GL
 */

static void liste_cree_gl(liste_transfo *l, GLuint list)
{
  Transfo tm ;

  while(l!=NULL)
    {
      glPushMatrix();
      (*transfo_extrait)(l->mat, &tm, 0);
      u_change_repere(&tm) ;
      glCallList(list);
      glPopMatrix();	
      l=l->suc;
    }
}

static void matrice_cree_gl(matrice *m, GLuint list,
		     const Triplet *x, const Triplet *y)
{
  int i,j;
	
  for(i=0;i<m->dim;i++)
    for(j=0;j<m->dim;j++)
      {
	glPushMatrix() ;
	glTranslatef(i*x->x+j*y->x, i*x->y+j*y->y, i*x->z+j*y->z) ;
	liste_cree_gl(m->m[i][j], list);
	glPopMatrix() ;
      }

}

/*
 * affichage STL
 */

static void liste_cree_stl(liste_transfo *l, void *obj, const char*v)
{
  Transfo t, t2 ;
  
  t = globale_etat_courant.transfo ;
  while(l!=NULL)
    {
      (*transfo_extrait)(l->mat, &t2, 0);
      globale_etat_courant.transfo = u_produit_transfo(&t, &t2) ;
      c_sauve_type(obj, &infos_L_affiche_stl, v) ;
      l=l->suc;
    }
}

static void matrice_cree_stl(matrice *m, void *obj,
		     const Triplet *x, const Triplet *y, const char*v)
{
  int i,j;
  Transfo t ;
	
  t = globale_etat_courant.transfo ;
  for(i=0;i<m->dim;i++)
    for(j=0;j<m->dim;j++)
      {
	globale_etat_courant.transfo.origine.x = t.origine.x + i*x->x+j*y->x ;
	globale_etat_courant.transfo.origine.y = t.origine.y + i*x->y+j*y->y ;
	globale_etat_courant.transfo.origine.z = t.origine.z + i*x->z+j*y->z ;
	liste_cree_stl(m->m[i][j], obj, v);
      }
  globale_etat_courant.transfo = t ;
}

static void affiche_fractale(struct fractale_commun *o,
			     const Champ *c, const char*v)
{
  void *obj;
  GLuint list;
  matrice *m ;
	
  transfo_4x4_utilise() ;
  if (o->affiche_transformations)
    {
      POUR_LES_FILS(o, f,
		    if ( i_f )
		    u_affiche_objet(f, "") ;
		    ) ;
    }
  m = trouve_matrice(o->p, o->niveau, v) ;
  if ( m == NULL )
    return ;
  /* le premier fils est l'objet de base de la fractale */
  obj=OBJET(o)->fils->objet;
  /*
   * Premier affichage invisible pour créer les Listes
   */
  glPushMatrix() ;
  glScalef(0,0,0) ;
  u_affiche_objet(obj, v) ;
  glPopMatrix() ;
  /*
   * Création de la liste
   */
  list=glGenLists(1);
  glNewList(list, GL_COMPILE);
  if (ACTIF(o))
    t_repere() ;
  else
    u_affiche_objet(obj, v) ;
  glEndList();

  matrice_cree_gl(m, list, &o->x, &o->y);
	
  glDeleteLists(list, 1);
}

static void affiche_fractale_stl(struct fractale_commun *o,
				 const Champ *c, const char*v)
{
  matrice *m ;
	
  m = trouve_matrice(o->p, o->niveau, strchr(v,' ')+1) ;
  if ( m == NULL )
    return ;
  /* le premier fils est l'objet de base de la fractale */

  matrice_cree_stl(m, OBJET(o)->fils->objet, &o->x, &o->y, v);
}

/*
 *
 */
static void detruit_fractale(struct fractale_commun *o)
{
  int i ;

  transfo_4x4_utilise() ;
  for(i=1;i<TAILLE_TABLE(o->p);i++)
    {
      matrice_libere(o->p[i]);
      o->p[i] = NULL ;
    }
}
/*
 *
 */
CLASSE(fractale_commun, struct fractale_commun,

       MENU("NON") /* Chemin dans le menu de création */
  
       CHAMP(niveau,
	     L_nombre_positif P_entier
	     Edite Sauve
	     LABEL("Niveau de details")
	     DEFAUT("1")
	     BULLE_D_AIDE("Nombre de recursions de la fractale")
	     )
       CHAMP(x, L_vecteur P_triplet DEFAUT("0 0 0") Init3d Edite Sauve)
       CHAMP(y, L_vecteur P_triplet DEFAUT("0 0 0") Init3d Edite Sauve)
       CHAMP(seulement_les_actives, L_booleen Edite Sauve
	     BULLE_D_AIDE("Si vrai alors seulement les transformations\n"
			  "active au sens de NanoÉdit sont incluses\n"
			  "dans l'IFS\n")
	     )
       CHAMP(affiche_transformations, L_booleen Edite Sauve
	     BULLE_D_AIDE("Si vrai alors seulement les transformations\n"
			  "utilisées sont affichées\n"
			  )
	     )

       CHAMP_VIRTUEL(L_solide Obligatoire)
       CHAMP(p[1], L_matrice_ifs) /* Pour pouvoir être extrait */
       DESTRUCTION(detruit_fractale)
       )
/*
 *****************************************************************************
 * FRACTALE DE BASE
 *****************************************************************************
 */

struct fractale
{
  struct fractale_commun fc ;
  int	taille ;		/* Largeur et Hauteur de la matrice */
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
static void rempli_matrice(struct fractale *o)
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
      if ( o->fc.seulement_les_actives && !ACTIF(lobj->objet) )
	continue ;

      if ( strcmp(OBJET(lobj->objet)->classe->nom, "fractale") == 0 )
	{
	  for( lobj2=OBJET(lobj->objet)->fils->suivant ; lobj2 ;
	       lobj2=lobj2->suivant)
	    if ( lobj2->objet )
	      matrice_affecte(o->fc.p[1],ligne,col,fait_transfo(lobj2->objet));
	  col++ ;
	  if ( col == o->taille )
	    {
	      ligne++ ;
	      col = 0 ;
	    }
	}
      else
	{
	  matrice_affecte(o->fc.p[1],ligne,col,fait_transfo(lobj->objet));
	}
    }
  if ( o->taille != 1 && ligne != o->taille )
    NOTE_CHAMP_INVALIDE(o, taille) ;
}

static void changement_fractale(struct fractale *o)
{
  transfo_4x4_utilise() ;
  detruit_fractale(&o->fc);
  o->fc.p[1] = matrice_nouvelle(o->taille);	
  rempli_matrice(o);
  /*
  printf("fait fichier Iventor %i\n",o->niveau);
  io_inc("fractale.iv", o->m);
  */
  /*io_mat("fractale.mat", m);*/
}


static char *info_fractale(struct fractale *o)
{
  static char info[100];
	
  sprintf(info,"Mat. %dx%d Prof. = %i",o->taille, o->taille, o->fc.niveau);
  return info;
}		
		

CLASSE(fractale, struct fractale,
       SUR_CLASSE("fractale_commun")
       BULLE_D_AIDE("Cet objet permet de créer une fractale\n")
	
       MENU("Figures/?d/Matrice IFS") /* Chemin dans le menu de création */
       EVENEMENT("mi")	 /* Raccourci clavier */
       PAQUET(NB_ACTIFS)
  
       CHAMP(taille,
	     L_nombre_positif P_entier
	     Edite Sauve
	     LABEL("Taille de la matrice")
	     DEFAUT("1")
	     BULLE_D_AIDE("Nombre de colonne et lignes de la matrice NxN")
	     )
       CHAMP_VIRTUEL(L_affiche_gl(affiche_fractale)
		     AFFICHAGE(Toujours_different))
       CHAMP_VIRTUEL(L_affiche_stl(affiche_fractale_stl))
       CHANGEMENT(changement_fractale)
       INFORMATIONS(info_fractale)
       )

/*
 *****************************************************************************
 * FRACTAL DEFINIE PAR DEUX FRACTALES
 *****************************************************************************
 */

struct fractale_binaire
{
  struct fractale_commun fc ;
  matrice **m1;			/* La matrice de la premiere fractale */
  matrice **m2;			/* La matrice de la deuxieme fractale */
} ;

CLASSE(fractale_binaire, struct fractale_binaire,
       SUR_CLASSE("fractale_commun")
       CHAMP(m1, L_pointeur_extrait TYPE_DANS_FILS(L_matrice_ifs) Extrait Obligatoire)
       CHAMP(m2, L_pointeur_extrait TYPE_DANS_FILS(L_matrice_ifs) Extrait Obligatoire)
       )

/*
 *****************************************************************************
 * COMPOSITION FRACTALE
 *****************************************************************************
 */

struct composition
{
  struct fractale_binaire fb ;
  Menu type;			/* type de composition */
} ;

static char *info_composition(struct composition *o)
{
	static char info[50];

	if ( o->fb.fc.p[1] == NULL )
	  return("?") ;
	
	sprintf(info,"%s %i %dx%d",o->type.selection, o->fb.fc.niveau,
		o->fb.fc.p[1]->dim, o->fb.fc.p[1]->dim);
	return info;
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
    }
}
		
static void changement_composition(struct composition *o)
{	
  if ( TRACE(o) )
    eprintf("o->m1 = %d o->m2 = %d\n", *o->fb.m1,*o->fb.m2) ;

  if ( o->fb.m1 == NULL || o->fb.m2 == NULL
       || *o->fb.m1 == NULL || *o->fb.m2 == NULL )
    return ;

  if ( FILS_CHANGE(o) || CHAMP_CHANGE(o, type) )
    {
      detruit_fractale(&o->fb.fc) ;
      o->fb.fc.p[1] = calcul_composition(o->type.index, *o->fb.m1,*o->fb.m2) ;
    }
  
  /*
    printf("fait fichier Iventor %i\n",o->niveau);
    io_inc("fractale.iv", o->m);
  */
}

CLASSE(composition, struct composition,

       SUR_CLASSE("fractale_binaire")
       BULLE_D_AIDE("Cet objet permet de composer deux fractales\n")
       
       MENU("Figures/?d/Compose") /* Chemin dans le menu de création */
       EVENEMENT("cf")	 /* Raccourci clavier */

       CHAMP(type,
	     LABEL("Type de composition")
	     P_option_menu E_option_menu
	     TE_TEXTE(
		      "Union\n"
		      "Concatenation\n"
		      "Melange\n"
		      "Addition\n"
		      "Multiplication\n"
		      )
	     DEFAUT("Union")
	     Edite Sauve
	     )
       CHAMP_VIRTUEL(L_affiche_gl(affiche_fractale)
		     AFFICHAGE(Toujours_different))
       CHAMP_VIRTUEL(L_affiche_stl(affiche_fractale_stl))
       CHAMP_VIRTUEL(L_affiche_pov(ecrit_pov_fractale))
       CHANGEMENT(changement_composition)
       INFORMATIONS(info_composition)
       )

/*
 *****************************************************************************
 * COLORIAGE FRACTALE
 *****************************************************************************
 */

struct coloriage_fractale
{
  struct fractale_binaire fb ;
  matrice *couleur[10];		/* matrices fractal couleur */
} ;

static void destruction_couleur(struct coloriage_fractale *o)
{
  int i ;

  for(i=1;i<TAILLE_TABLE(o->couleur);i++)
    {
      matrice_libere(o->couleur[i]);
      o->couleur[i] = NULL ;
    }
}

static void destruction_fractal_couleur(struct coloriage_fractale *o)
{
  detruit_fractale(&o->fb.fc) ;
  destruction_couleur(o) ;
}

static void liste_cree_gl_couleur(liste_transfo *l, liste_transfo *l2, GLuint list)
{
  Transfo tm ;
  Triplet t ;
  GLfloat m[4] ;

  while(l!=NULL)
    {
      if ( l2 )
	{
	  (*transfo_origine)(l2->mat, &t, 0) ;
	  glColor3dv((GLdouble*)&t) ;
	  m[0] = t.x ;
	  m[1] = t.y ;
	  m[2] = t.z ;
	  m[3] = 1 ;
	  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, m) ;

	}
      glPushMatrix();
      (*transfo_extrait)(l->mat, &tm, 0);
      u_change_repere(&tm) ;
      glCallList(list);
      glPopMatrix();	
      l=l->suc;
      if ( l2 )
	{
	  l2 = l2->suc ;
	  if ( l2 == NULL )
	    {
	      glColor3f(1,1,1) ;
	    }
	}
    }
}

static void matrice_cree_gl_couleur(matrice *m, matrice *m2,GLuint list,
				    const Triplet *x, const Triplet *y)
{
  int i,j;
	
  for(i=0;i<m->dim;i++)
    for(j=0;j<m->dim;j++)
      {
	glPushMatrix() ;
	glTranslatef(i*x->x+j*y->x, i*x->y+j*y->y, i*x->z+j*y->z) ;
	liste_cree_gl_couleur(m->m[i][j], m2->m[i][j], list);
	glPopMatrix() ;
      }
}

static void affiche_fractale_couleur(struct coloriage_fractale *o,
			     const Champ *c, const char*v)
{
  void *obj;
  GLuint list;

  if ( o->fb.fc.p[1]==NULL || o->couleur[1]==NULL )
    return ;

  /* le premier fils est l'objet de base de la fractale */
  obj=OBJET(o)->fils->objet;

  glPushMatrix() ;
  glScalef(0,0,0) ;
  u_affiche_objet(obj, v) ;
  glPopMatrix() ;
	
  list=glGenLists(1);
  glNewList(list, GL_COMPILE);
  u_affiche_objet(obj, v) ;
  glEndList();

  glPushAttrib(GL_CURRENT_BIT|GL_LIGHTING_BIT) ;
  matrice_cree_gl_couleur(trouve_matrice(o->fb.fc.p, o->fb.fc.niveau, v)
			  ,trouve_matrice(o->couleur, o->fb.fc.niveau, v)
			  , list, &o->fb.fc.x, &o->fb.fc.y);
  glPopAttrib() ;

  glDeleteLists(list, 1);
}

/*
 * crée l'affichage Pov correspondant
 */

static void liste_cree_pov_couleur(liste_transfo *l, liste_transfo *l2,
				   int n, FILE *f)
{
  Triplet pt ;

  for(;l;l=l->suc)
    {
      fprintf(f,"object { o%x\n", n);
      transfo_affiche_pov(l->mat, f) ;
      if ( l2 )
	{
	  (*transfo_origine)(l2->mat, &pt,0) ;
	  pigment(f, pt.x, pt.y, pt.z) ;
	  l2 = l2->suc ;
	}

      fprintf(f,"}\n");
    }
}

static void ecrit_pov_fractale_couleur(struct coloriage_fractale *o, const Champ *c, const char *v)
{
  int i, j ;
  FILE *f ;
  Triplet *x, *y ;
  matrice *couleur, *m ;

  if ( o->fb.fc.p[1]==NULL || o->couleur[1]==NULL )
    return ;

  if ( v[0]=='\0' ) return ;

  f = (FILE*)atoi(v) ;

  fprintf(f, "#declare o%x = ", (int)FILS(o,0)) ;

  globale_etat_courant.ne_pas_sauvegarder_les_attributs_pov++ ;
  c_sauve_type(FILS(o,0), &infos_L_affiche_pov, v) ;
  globale_etat_courant.ne_pas_sauvegarder_les_attributs_pov-- ;

  x = &o->fb.fc.x ;
  y = &o->fb.fc.y ;
  couleur = trouve_matrice_puissance(o->couleur, o->fb.fc.niveau) ;
  m = trouve_matrice_puissance(o->fb.fc.p, o->fb.fc.niveau) ;
  fprintf(f, "union {\n") ;
  for(i=0;i<couleur->dim;i++)
    for(j=0;j<couleur->dim;j++)
      {
	fprintf(f, "union {\n") ;
	liste_cree_pov_couleur(m->m[i][j], couleur->m[i][j],(int)FILS(o,0), f);
	fprintf(f, "translate <%g, %g, %g>\n}\n"
		, i*x->x+j*y->x, i*x->y+j*y->y, i*x->z+j*y->z
		) ;
      }
  fprintf(f, "}\n") ;
}
/*
 *
 */
static void changement_fractal_couleur(struct coloriage_fractale *o)
{
  if ( o->fb.m1==NULL || o->fb.m2==NULL || *o->fb.m1==NULL || *o->fb.m2==NULL
       || (*o->fb.m1)->dim != (*o->fb.m2)->dim )
    {
      /*
      NOTE_CHAMP_INVALIDE(o,fb.m1) ;
      NOTE_CHAMP_INVALIDE(o,fb.m2) ;
      */
      return ;
    } 

  detruit_fractale(&o->fb.fc) ;
  o->fb.fc.p[1] = matrice_duplique(o->fb.m1[0]) ;
  destruction_couleur(o) ;
  o->couleur[1] = matrice_duplique(o->fb.m2[0]) ;
}

CLASSE(coloriage_fractal, struct coloriage_fractale,
       SUR_CLASSE("fractale_binaire")
       BULLE_D_AIDE("Pour colorier un fractal\n")
       MENU("Figures/?d/Fractal Colorié")
       EVENEMENT("fc")	 /* Raccourci clavier */
  	
       CHAMP_VIRTUEL(L_affiche_gl(affiche_fractale_couleur)
		     AFFICHAGE(Toujours_different))
       CHAMP_VIRTUEL(L_affiche_pov(ecrit_pov_fractale_couleur))
       CHAMP_VIRTUEL(L_affiche_stl(affiche_fractale_stl))

       CHANGEMENT(changement_fractal_couleur)
       DESTRUCTION(destruction_fractal_couleur)
       
       )


/*
 *****************************************************************************
 * COMPOSITION DE FRACTAL COLORIE
 *****************************************************************************
 */

struct compose_fractale_couleur
{
  struct composition composition ;
  struct coloriage_fractale *a, *b ;
  matrice *couleur[10] ;
} ;

static void destruction_compose_couleur(struct compose_fractale_couleur *o)
{
  int i ;

  for(i=1;i<TAILLE_TABLE(o->couleur);i++)
    {
      matrice_libere(o->couleur[i]);
      o->couleur[i] = NULL ;
    }
}

static void destruction_compose_fractale_couleur(struct compose_fractale_couleur *o)
{
   detruit_fractale(&o->composition.fb.fc) ;
   destruction_compose_couleur(o) ;
}

static void affiche_compose_fractale_couleur(struct compose_fractale_couleur*o,
			     const Champ *c, const char*v)
{
  void *obj;
  GLuint list;
  matrice *m, *couleur ;

  if ( o->composition.fb.fc.p[1]==NULL || o->couleur[1]==NULL )
    return ;

  /* le premier fils est l'objet de base de la fractale */
  obj=OBJET(o)->fils->objet;

  glPushMatrix() ;
  glScalef(0,0,0) ;
  u_affiche_objet(obj, v) ;
  glPopMatrix() ;
	
  list=glGenLists(1);
  glNewList(list, GL_COMPILE);
  u_affiche_objet(obj, v) ;
  glEndList();

  m = trouve_matrice(o->composition.fb.fc.p, o->composition.fb.fc.niveau, v) ;
  couleur = trouve_matrice(o->couleur, o->composition.fb.fc.niveau, v) ;

  glPushAttrib(GL_CURRENT_BIT|GL_LIGHTING_BIT) ;
  matrice_cree_gl_couleur(m,couleur, list,
			  &o->composition.fb.fc.x, &o->composition.fb.fc.y);
  glPopAttrib() ;

  glDeleteLists(list, 1);
}

static void ecrit_pov_compose_fractale_couleur(struct compose_fractale_couleur *o, const Champ *c, const char *v)
{
  int i, j ;
  FILE *f ;
  Triplet *x, *y ;
  matrice *m, *couleur ;

  if ( o->composition.fb.fc.p[1]==NULL || o->couleur[1]==NULL )
    return ;

  if ( v[0]=='\0' ) return ;

  f = (FILE*)atoi(v) ;

  fprintf(f, "#declare o%x = ", (int)FILS(o,0)) ;

  globale_etat_courant.ne_pas_sauvegarder_les_attributs_pov++ ;
  c_sauve_type(FILS(o,0), &infos_L_affiche_pov, v) ;
  globale_etat_courant.ne_pas_sauvegarder_les_attributs_pov-- ;
	
  x = &o->composition.fb.fc.x ;
  y = &o->composition.fb.fc.y ;
  m = trouve_matrice_puissance(o->composition.fb.fc.p, o->composition.fb.fc.niveau) ;
  couleur = trouve_matrice_puissance(o->couleur, o->composition.fb.fc.niveau) ;
  fprintf(f, "union {\n") ;
  for(i=0;i<couleur->dim;i++)
    for(j=0;j<couleur->dim;j++)
      {
	fprintf(f, "union {\n") ;
	liste_cree_pov_couleur(m->m[i][j], couleur->m[i][j],(int)FILS(o,0), f);
	fprintf(f, "translate <%g, %g, %g>\n}\n"
		, i*x->x+j*y->x, i*x->y+j*y->y, i*x->z+j*y->z
		) ;
      }
  fprintf(f, "}\n") ;
}

static void changement_compose_fractale_couleur(struct compose_fractale_couleur *o)
{
  if ( o->a == NULL || o->b == NULL ||
       o->a->fb.fc.p[1] == NULL || o->b->fb.fc.p[1] == NULL ||
       o->a->fb.fc.p[1]->dim != o->b->fb.fc.p[1]->dim )
    return ;

  destruction_compose_fractale_couleur(o) ;
  /*
   * Composition des coloriages
   */
  o->composition.fb.fc.p[1]       = calcul_composition(o->composition.type.index,
					   *o->a->fb.m1,*o->b->fb.m1) ;
  o->couleur[1] = calcul_composition(o->composition.type.index,
					   *o->a->fb.m2,*o->b->fb.m2) ;

}

CLASSE(composition_coloriage_fractal, struct compose_fractale_couleur,
       SUR_CLASSE("fractale_commun")
       BULLE_D_AIDE("Composition de Fractales coloriées\n")
       MENU("Figures/?d/Compose Colorie")
       EVENEMENT("cc")	 /* Raccourci clavier */
  	
       CHAMP(composition.type,
	     LABEL("Type de composition")
	     P_option_menu E_option_menu
	     TE_TEXTE(
		      "Union\n"
		      "Concatenation\n"
		      "Melange\n"
		      "Addition\n"
		      "Multiplication\n"
		      )
	     DEFAUT("Union")
	     Edite Sauve
	     )
       
       CHAMP(a, P_entier CLASSE_PARAMETRE("coloriage_fractal") Extrait Obligatoire)
       CHAMP(b, P_entier CLASSE_PARAMETRE("coloriage_fractal") Extrait Obligatoire)
       
       CHAMP_VIRTUEL(L_affiche_gl(affiche_compose_fractale_couleur)
		     AFFICHAGE(Toujours_different))
       CHAMP_VIRTUEL(L_affiche_pov(ecrit_pov_compose_fractale_couleur))
       CHAMP_VIRTUEL(L_affiche_stl(affiche_fractale_stl))

       CHANGEMENT(changement_compose_fractale_couleur)
       DESTRUCTION(destruction_compose_fractale_couleur)
       INFORMATIONS(info_composition)
       PAQUET(3)
       
       )
