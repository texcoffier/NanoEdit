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
#ifndef CODEC_BASE_H
#define CODEC_BASE_H


/* #define DEBUG_CODEC */


typedef int (*Codec)() ;

#define DIMMAX 3

/*
 * Les champs avec "pointeur_" devant sont non-nul si la valeur
 * pointée existe en mémoire.
 * Par exemple :
 *    float p[5] ; // L'adresse de "p" n'existe pas
 *    float *p ;   // L'adresse de "p" existe
 * Les "pointeur_" sont tous nul ou tous non-null en même temps.
 */
struct informations_table
{
  int dimension ;
  int taille[DIMMAX] ;		/* PHL au maximum */
  int *pointeur_taille[DIMMAX] ;		/* PHL au maximum */
  void *table, *table2 ;	/* Début de l'objet table/grille */
  void **pointeur_table, **pointeur_table2 ;
  const struct type_physique *type, *type2 ; /* Type des éléments  */
  int courant[DIMMAX] ;		/* Position */
  /*
   * À partir de la position dans la table, calcule la position en mémoire
   */
  int (*positionne_element)(struct informations_table*) ;
  void *element_courant, *element_courant2 ;
  /*
   * Passe à l'élément suivant.
   * Met à jour TOUS les élements de cette structure
   */
  int (*suivant)(struct informations_table*) ;
  /*
   * Change la taille de la table
   */
  int (*change_taille)(struct informations_table*) ;
  int nouvelle_taille[DIMMAX] ;
} ;


#ifndef CODEC_BASE
extern
#endif
struct globale_traduction
{
  enum action { Recupere_texte,
		Sauve_texte,
		Libere,
		Recupere_float,
		Informations,
		Informations_table
  } action ;
  const char *entree ;

  char *sortie ;
  char *sortie_debut ;
  char *sortie_fin ;

  const struct type_physique *informations ;
  struct informations_table *informations_table ;

  int economise_memoire ;

  void *objet, *champ ;		/* Pour NanoEdit */
} *globale_traduction ;

typedef struct type_physique
{
  int taille ;
  char *nom ;
  char *nom_structure ;
  /* Ordre important car accédé comme un tableau */
  Codec recupere_texte, sauve_texte, libere, recupere_float ;
  const char *recupere_texte_nom, *sauve_texte_nom, *libere_nom, *recupere_float_nom ;
  int (*generique)() ;
} Codec_informations ;

#define G globale_traduction

#define INCREMENTE(n) (16*sizeof(double) + n*1.5)
#define POINTEUR_METHODE(CI) (&CI->recupere_texte)[G->action]

const Codec_informations* codec_informations(Codec tp) ;

/*
 *
 */

#ifdef DEBUG_CODEC


#include "u_memoire.h"
extern Gestion_memoire globale_gm_codec ;


#define CODEC_MALLOC(X,NB) X = alloue_zone_memoire(&globale_gm_codec, (NB)*sizeof(*(X)))

#define CODEC_REALLOC(X,NB) X = realloue_zone_memoire(&globale_gm_codec, X, (NB)*sizeof(*(X)))

#define CODEC_FREE(X) do { if ( X ) { libere_zone_memoire(&globale_gm_codec, X) ; X=NULL ; } } while(0)

#else

extern int globale_codec_malloc ;

#define CODEC_MALLOC(X,NB) do { globale_codec_malloc++ ; \
                           if ( (X = malloc(1+sizeof(*(X)) * (NB) )) == 0 )\
                                 { eprintf( "Plus de memoire\n") ; EXIT ; }} \
                            while(0)

#define CODEC_REALLOC(X,NB) do { if ( (X = (X) \
                                 ? realloc(X,1+sizeof(*(X)) * (NB)) \
                                 : ( globale_codec_malloc++, \
                                      malloc(1+sizeof(*(X)) * (NB))))  == 0 )\
                                    { eprintf( "Plus de memoire\n") ; EXIT ; }\
} while(0)

#define CODEC_FREE(X) do { if ( X ) { globale_codec_malloc-- ; free(X) ; X=NULL ; } } while(0)


#endif

/*
 *
 *
 * Macro de déclaration de type de base
 *
 *
 */
#define TYPE_DE_BASE_GENERIQUE(NOM,					\
                               ARGUMENT_IN, ARGUMENT_OUT,		\
                               TYPE, TAILLE_TYPE,			\
                               RECUPERE, SAUVE, LIBERE, RECUPERE_FLOAT)	\
int NOM ARGUMENT_IN ;							\
const Codec_informations infos_##NOM =					\
{									\
  TAILLE_TYPE,								\
  #NOM,									\
  #TYPE,								\
  RECUPERE,								\
  SAUVE,								\
  LIBERE,								\
  RECUPERE_FLOAT,							\
  #RECUPERE,								\
  #SAUVE,								\
  #LIBERE,								\
  #RECUPERE_FLOAT,							\
  NOM									\
} ;									\
int NOM ARGUMENT_IN							\
{									\
  switch(G->action)							\
    {									\
    case Recupere_texte :						\
      return((*infos_##NOM.recupere_texte)ARGUMENT_OUT) ;		\
    case Sauve_texte    :						\
      return((*infos_##NOM.sauve_texte   )ARGUMENT_OUT) ;		\
    case Libere         :						\
      return((*infos_##NOM.libere        )ARGUMENT_OUT) ;		\
    case Recupere_float :						\
      return((*infos_##NOM.recupere_float)ARGUMENT_OUT) ;		\
    case Informations   :						\
      G->informations = &infos_##NOM  ; return(0) ;			\
    case Informations_table :						\
      return(0) ;							\
    }									\
  return(1) ;								\
}

#define TYPE_DE_BASE_VOID(NOM, RECUPERE, SAUVE, LIBERE, RECUPERE_FLOAT)  \
 TYPE_DE_BASE_GENERIQUE(NOM,(),(),void, 0, RECUPERE, SAUVE, LIBERE, RECUPERE_FLOAT)

#define TYPE_DE_BASE_NON_VOID(NOM, ARGUMENT_IN, ARGUMENT_OUT,           \
                              TYPE, RECUPERE, SAUVE, LIBERE, RECUPERE_FLOAT)            \
 TYPE_DE_BASE_GENERIQUE(NOM, ARGUMENT_IN, ARGUMENT_OUT,                 \
                        TYPE, sizeof(TYPE) , RECUPERE, SAUVE, LIBERE, RECUPERE_FLOAT)

#define TYPE_DE_BASE(NOM, TYPE, RECUPERE, SAUVE, LIBERE, RECUPERE_FLOAT) \
 TYPE_DE_BASE_NON_VOID(NOM,(TYPE* a),(a),TYPE, RECUPERE, SAUVE, LIBERE, RECUPERE_FLOAT)

void codec_reallouer() ;


#define PUTCHAR(C)				\
{						\
  *G->sortie++ = (C) ;				\
  if ( G->sortie == G->sortie_fin )		\
    codec_reallouer() ;				\
}

#define PUTFLOAT(C)					\
{							\
  if ( G->sortie_fin - G->sortie <= sizeof(float) )	\
    codec_reallouer() ;					\
  *((float*)G->sortie) = (C) ;				\
  G->sortie += sizeof(float) ;				\
}

#define PUTSTR(S) { const char *_s ; _s = (S) ; while(*_s) PUTCHAR(*_s++) ; }

#define SAUTE_BLANC while( *G->entree == ' ' || *G->entree == '\t' ) G->entree++ ;

/*
 * Fonctions utilitaire
 */

extern struct globale_traduction globale_pile_traduction[100] ;
extern int globale_change_environnement_traduction_n ;

#define change_environnement_traduction(i)				    \
{									    \
  globale_change_environnement_traduction_n += i ;			    \
  if ( globale_change_environnement_traduction_n			    \
       >= TAILLE_TABLE(globale_pile_traduction)				    \
       || globale_change_environnement_traduction_n<0 )			    \
    EXIT ;								    \
  G = &globale_pile_traduction[globale_change_environnement_traduction_n] ; \
}

#define codec_debut_sauve_texte(v)		\
{						\
  change_environnement_traduction(+1) ;		\
  G->action            = Sauve_texte ;		\
  G->entree            = v           ;		\
  G->economise_memoire = 0           ;		\
}

#define codec_fin_sauve_texte()			\
{						\
  change_environnement_traduction(-1) ;		\
}


#endif
