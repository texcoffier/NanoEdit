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
#include "codec.h"
#include "t_eric.h"

/*
 * Codage des coordonnée :
 *
 * Pt Cartésien     Affine    : x y z            	 x y z 1 CA
 * Ve Cartésien     Affine    : x y z V          	 x y z 0 CA
 * Pt Cartésien     Projectif : x/w y/w z/w (w)  	 x y z w CP
 * Pt Cartésien     Projectif : x y z I          	 x y z 0 CP
 * Pt Barycentrique Affine    : l m n o B         	 l m n o BA (l+m+n+o=1)
 * Ve Barycentrique Affine    : l m n o BV        	 l m n o BA (l+m+n+o=0)
 * Pt Barycentrique Projectif : l m n o (w) B            l m n o BP (l+m+n+o=w)
 * Pt Barycentrique Projectif : l m n o BI               l m n o BP (l+m+n+o=0)
 */

static Flottant somme_coordonnees(const VECH *a)
{
  int i ;
  Flottant somme ;
  somme = 0 ;
  for(i=0; i<a->espace.dim; i++)
    somme += a->c[i] ;
  if ( ABS(somme) < FLOTTANT_EPSILON )
    somme = 0 ;
  return(somme) ;
}

int codec_coordonnees_recupere_texte(const VECH *a)
{
  int i ;
  Flottant w, x ;

  w = CARTESIEN(a) ? HOMOGENE(a) :somme_coordonnees(a) ;

  if ( BARYCENTRIQUE(a) )
    codec_l(a->c, a->espace.dim, codec_flottant) ;

  if ( PROJECTIF(a) && w != 0 )
    {
      if ( CARTESIEN(a) )
	{
	  for(i=PREMIERE_COORDONNEE; i < DERNIERE_COORDONNEE(a); i++)
	    {
	      x = a->c[i]/w ;
	      codec_flottant(&x) ;
	    }
	}

      PUTCHAR('(') ;
      codec_flottant(&w) ;
      G->sortie-- ;
      PUTSTR(") ") ;
    }
  else
    if ( CARTESIEN(a) ) 
      codec_l(&a->c[PREMIERE_COORDONNEE],
	      a->espace.dim-1, codec_flottant) ;
  
  if ( BARYCENTRIQUE(a) )
    PUTCHAR('B') ;

  if ( w == 0 )
    PUTCHAR(AFFINE(a) ? 'V' : 'I') ;


  if ( G->sortie[-1] == ' ' )
    G->sortie-- ;
  PUTCHAR('\n') ;
  return(0) ;
}


int codec_coordonnees_sauve_texte(VECH *a)
{
  const char *debut, *pc, *tmp ;
  Flottant w, somme ;
  int i ;

  debut = G->entree ;
  codec_L((void*)&a->c, &a->espace.dim, codec_flottant) ;

  a->espace.type_espace = TE_CA ; /* Cartesien Affine */
  w = 1 ;
  for(pc=G->entree-1; pc>=debut;pc--)
    {
      switch(*pc)
	{
	case 'B':
	case 'b':
	  a->espace.type_espace |= TE_Barycentrique ;
	  break ;
	case 'I':
	case 'i':
	  a->espace.type_espace |= TE_Projectif ;
	case 'V':
	case 'v':
	  w = 0 ;
	  break ;
	case ')':
	  tmp = pc ;
	  while( pc>debut && *pc!='(' )
	    pc-- ;
	  if ( pc == debut )
	    {
	      /*
	       * Cas ou la ')' ne correspond pas au poids
	       * mais a autre chose
	       */
	      pc = tmp ;
	      break ;
	    }
	case '(':
	  if ( *pc == '(' )
	    {
	      a->espace.type_espace |= TE_Projectif ;
	      sscanf(pc+1, FF, &w) ;
	    }
	  break ;
	case ' ':
	case '\t':
	case '\n':
	  break ;
	default:
	  pc = debut ; /* Arrete boucle */
	}
    }


  if ( CARTESIEN(a) )
    {
      a->espace.dim++ ;
      CODEC_REALLOC(a->c, a->espace.dim) ;
      if ( PREMIERE_COORDONNEE != 0 )
	{
	  memmove(a->c+1, a->c,
		  sizeof(*a->c)*(a->espace.dim-1)) ;
	}
      HOMOGENE(a) = w ;
      if ( w !=0 && w != 1 )
	for(i=PREMIERE_COORDONNEE; i<DERNIERE_COORDONNEE(a); i++)
	  a->c[i] *= w ;
    }
  else
    {
      somme = somme_coordonnees(a) ;
      if ( somme != 0 && somme != w )
	{
	  if ( w != 0 )
	    {
	      if ( PROJECTIF(a) )
		for(i=0; i<a->espace.dim; i++)
		  a->c[i] *= w/somme ;
	      else
		for(i=0; i<a->espace.dim; i++)
		  a->c[i] /= somme ;
	    }
	  else
	    {
	      for(i=0; i<a->espace.dim; i++)
		a->c[i] -= somme/a->espace.dim ;
	    }	  
	}
    }
  return(0) ;
}


int codec_coordonnees_libere(VECH *a)
{
  CODEC_FREE(a->c) ;
  return(0) ;
}

int codec_coordonnees_recupere_float(const VECH *a)
{
  int i ;
  Flottant w ;

  w = CARTESIEN(a) ? HOMOGENE(a) : somme_coordonnees(a) ;

  if ( BARYCENTRIQUE(a) )
    {
      for(i=0; i < DERNIERE_COORDONNEE(a); i++)
	PUTFLOAT(a->c[i]) ;
      if ( PROJECTIF(a) && w!=0 )
	PUTFLOAT(w) ;
    }
  else
    {
      if ( PROJECTIF(a) && w != 0 )
	{
	  for(i=PREMIERE_COORDONNEE; i < DERNIERE_COORDONNEE(a); i++)
	    PUTFLOAT(a->c[i]/w) ;
	  PUTFLOAT(w) ;
	}
      else
	{
	  for(i=PREMIERE_COORDONNEE; i < DERNIERE_COORDONNEE(a); i++)
	    PUTFLOAT(a->c[i]) ;
	}
    }
  return(0) ;
}


TYPE_DE_BASE(codec_coordonnees, VECH,
             codec_coordonnees_recupere_texte,
             codec_coordonnees_sauve_texte,
             codec_coordonnees_libere,
	     codec_coordonnees_recupere_float
             )

DEFINITION_TYPE_LOGIQUE(L_coord, L_valeur,
			"Coordonnées génériques de point ou vecteur",
			c->tp = &infos_codec_coordonnees ;
			)

TYPE_PHYSIQUE(codec_L_coordonnees, L_VECH,
	      codec_L((void**)&a->pt, &a->nb, codec_coordonnees) ;
	      )

DEFINITION_TYPE_LOGIQUE(L_table_coord, L_table,
			"Table de Coordonnées génériques de point ou vecteur",
			c->tp = &infos_codec_L_coordonnees ;
			)



static Booleen coord_est_un_vecteur(const VECH *c)
{
  if ( !AFFINE(c) )
    return(Faux) ;
  
  if ( CARTESIEN(c) )
    return( HOMOGENE(c) == 0. ) ;
  else
    return( somme_coordonnees(c) == 0. ) ;
}

static Booleen coord_vecteur(const void *o, const Champ *c)
{
  if ( tl_inclus(c->tl, &infos_L_vecteur) )
    return(Vrai) ;

  if ( c->tp!=&infos_codec_coordonnees )
    return(Faux) ;

  return( coord_est_un_vecteur(&T(VECH, o, c)) ) ;
}

DEFINITION_TYPE_LOGIQUE(L_coord_vecteur, L_coord,
			"Vecteur de coordonnées générique",
			PREDICAT(coord_vecteur)
			)


static Booleen coord_point(const void *o, const Champ *c)
{
  if ( tl_inclus(c->tl, &infos_L_point) )
    return(Vrai) ;

  if ( c->tp!=&infos_codec_coordonnees )
    return(Faux) ;

  return( !coord_est_un_vecteur(&T(VECH, o, c)) ) ;
}

DEFINITION_TYPE_LOGIQUE(L_coord_point, L_coord,
			"Point de coordonnées générique",
			PREDICAT(coord_point)
			)

/*
 * Attention : Bien tenir l'ordre à jour
 */

static char* globale_enum_type_espace[] = { "CA", "BA", "CP", "BP" } ;

TYPE_PHYSIQUE(codec_enum_type_espace, t_type_espace,
	      return( codec_enum(a, 4, globale_enum_type_espace) ) ;
	      )

DEFINITION_TYPE_LOGIQUE(L_type_espace, L_entier,
			"Type d'espace",
			c->tp = &infos_codec_enum_type_espace ;
			)

static char* globale_enum_semig[] = {
  "similitude", "hyperbolique", "dilatation",
  "cisaillement", "affine", "projectif" } ;

TYPE_PHYSIQUE(codec_enum_semig, SEMIG,
	      return( codec_enum((int*)a, 6, globale_enum_semig) ) ;
	      )

DEFINITION_TYPE_LOGIQUE(L_semig, L_valeur,
			"Type de semi-groupe",
			)

/*
 *
 */

static Flottant somme_colonne(MATH *a, int colonne)
{
  int ligne ;
  Flottant s ;

  s = 0 ;
  for(ligne=0;ligne<a->m.dim;ligne++)
    s += a->c[ligne][colonne] ;

  return(s) ;
}

static void norme_colonne_a_0(MATH *a, int colonne)
{
  int ligne ;
  Flottant s ;

  s = somme_colonne(a, colonne) ;
  if ( ABS(s) < FLOTTANT_EPSILON )
    return ;
  s /= a->m.dim ;
  for(ligne=0;ligne<a->m.dim;ligne++)
    a->c[ligne][colonne] -= s ;
}

static void norme_colonne_a_1(MATH *a, int colonne)
{
  int ligne ;
  Flottant s ;

  s = somme_colonne(a, colonne) ;
  if ( ABS(s-1) < FLOTTANT_EPSILON )
    return ;
  if ( ABS(s) <  FLOTTANT_EPSILON )
    {
      a->c[0][colonne] = 1 ;
    }
  else
    {
      for(ligne=0;ligne<a->m.dim;ligne++)
	a->c[ligne][colonne] /= s ;
    }
}


static void force_w_a_0(MATH *a, int colonne)
{
  if ( TE_CARTESIEN(a->m.type_espace) )
    a->c[0][colonne] = 0 ;
  else
    norme_colonne_a_0(a, colonne) ;
}

static void force_w_a_1(MATH *a, int colonne)
{
  if ( TE_CARTESIEN(a->m.type_espace) )
    a->c[0][colonne] = 1 ;
  else
    norme_colonne_a_1(a, colonne) ;
}

static void force_w_a_different_de_0(MATH *a, int colonne)
{
  if ( TE_CARTESIEN(a->m.type_espace) )
    {
      if ( a->c[0][colonne] == 0 )
	a->c[0][colonne] = 1 ;
    }
  else
    {
      if ( ABS(somme_colonne(a, colonne)) < FLOTTANT_EPSILON)
	a->c[0][colonne]++ ;
    }
}

static void normalise_math(MATH *a)
{
  int colonne ;

  if ( a->n.dim == 0 || a->m.dim == 0 )
    return ;

  switch(a->n.type_espace)
    {
    case TE_CA:
      for(colonne=1;colonne<a->n.dim;colonne++)
	force_w_a_0(a, colonne) ;
      if ( TE_AFFINE(a->m.type_espace) )
	force_w_a_1(a,0) ;
      else
	force_w_a_different_de_0(a,0) ;
      break ;
    case TE_BA:
      if ( TE_AFFINE(a->m.type_espace) )
	for(colonne=0;colonne<a->n.dim;colonne++)
	  force_w_a_1(a, colonne) ;
      else
	for(colonne=0;colonne<a->n.dim;colonne++)
	  force_w_a_different_de_0(a, colonne) ;
      break ;
    case TE_CP:
    case TE_BP:
      break ;
    }
}

TYPE_PHYSIQUE(codec_math, MATH,
	      codec_H_L((void***)&a->c,
			&a->m.dim, &a->n.dim,
			codec_flottant) ;
	      codec_enum_type_espace(&a->n.type_espace) ;
	      codec_linefeed() ;
	      codec_enum_type_espace(&a->m.type_espace) ;
	      codec_linefeed() ;

	      if ( globale_traduction->action == Sauve_texte )
	         normalise_math(a) ;
	      )

DEFINITION_TYPE_LOGIQUE(L_math, L_grille_nombre,
			"Matrice générique",
			init_E_long_texte(c) ;
			c->te_hauteur = 200 ;
			c->tp = &infos_codec_math ;			
			)

/* ajout -----> */
TYPE_PHYSIQUE(codec_controle, L_VECH,
	      return(codec_L((void**)&a->pt, &a->nb, codec_coordonnees)))

DEFINITION_TYPE_LOGIQUE(L_controle, L_valeur,
			"Points de controle",
			init_E_long_texte(c) ;
			c->te_hauteur = 400 ;
			c->tp = &infos_codec_controle ;			
			)
/* /ajout <----- */

TYPE_PHYSIQUE(codec_ifs, L_MATH,
	      return(codec_L((void**)&a->tf, &a->nb, codec_math)))

DEFINITION_TYPE_LOGIQUE(L_ifs, L_valeur,
			"IFS",
			init_E_long_texte(c) ;
			c->te_hauteur = 400 ;
			c->tp = &infos_codec_ifs ;			
			)

TYPE_PHYSIQUE(codec_gifs, GIFS,
	      codec_math(&a->proj) ;		      		 
	      codec_ifs(&a->ifs) ;		      		 
	      codec_math(&a->te) ;		      		 
	      codec_L_int((t_L_int*)&a->extrem) ;     		 
	      )

DEFINITION_TYPE_LOGIQUE(L_gifs, L_valeur,
			"GIFS",
			init_E_long_texte(c) ;
			c->te_hauteur = 700 ;
			c->tp = &infos_codec_gifs ;			
			)

TYPE_PHYSIQUE(codec_iter, ITER,
	      codec_char_decimal(&a->fini) ;
	      codec_flottant(&a->val) ;
	      codec_linefeed() ;
	      ) ;

DEFINITION_TYPE_LOGIQUE(L_iter, L_valeur,
			"ITER",
			c->tp = &infos_codec_iter ;
			)


