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
 * Créé le Thu Mar 11 16:15:22 1999 par Thierry EXCOFFIER
 */

#ifndef CODEC_H
#define CODEC_H

#include <stdlib.h>
#include <string.h>
#include "l_liste.h"
#include "codec_base.h"

/*
 * Utilitaire pour définir des types construits
 */

#define TYPE_PHYSIQUE(NOM, TYPE, CORPS)					\
int NOM(TYPE *a) ;							\
const Codec_informations infos_##NOM =					\
{									\
  sizeof(TYPE),								\
  #NOM,									\
  #TYPE,								\
  NOM,									\
  NOM,									\
  NOM,									\
  NOM,									\
  #NOM,									\
  #NOM,									\
  #NOM,									\
  #NOM,									\
  NOM									\
} ;									\
									\
int NOM(TYPE *a)							\
{									\
  if( G->action == Informations )					\
    {									\
      G->informations = &infos_##NOM ;					\
      return(0) ;							\
    }									\
  { /* { } Pour permettre la declaration de variables locales */	\
    CORPS ;								\
  }									\
  return(0) ;								\
}

#define CODEC_PROTOTYPE_(NOM, ARG,CONST)	\
       extern const Codec_informations infos_##NOM ;	\
       int NOM##_sauve_texte(ARG*) ;		\
       int NOM##_recupere_texte(CONST ARG*) ;	\
       int NOM (ARG*) ;

#define CODEC_PROTOTYPE(NOM, ARG) CODEC_PROTOTYPE_(NOM, ARG, const)

#define CODEC_PROTOTYPE_VOID(NOM)		\
       extern const Codec_informations infos_##NOM ;	\
       int NOM##_sauve_texte() ;		\
       int NOM##_recupere_texte() ;		\
       int NOM() ;



#define CODEC_LISTE_PROTOTYPE(NOM, DONNEE, T_DONNEE)		\
typedef struct t_liste##NOM					\
{								\
  struct t_liste##NOM *suivant ;				\
  T_DONNEE donnee ;						\
} t_liste##NOM ;						\
CODEC_PROTOTYPE(codec_liste##NOM, t_liste##NOM* )


#define CODEC_GRAPHE_PROTOTYPE(NOM,GRAPHE,T_GRAPHE,NOEUD,T_NOEUD,ARETE,T_ARETE) \
typedef struct								    \
{									    \
  int destination ;							    \
  T_ARETE donnee ;							    \
} t_arete##NOM ;							    \
typedef struct								    \
{									    \
  int nb_aretes ;							    \
  t_arete##NOM *aretes ;						    \
  T_NOEUD donnee ;							    \
} t_noeud##NOM ;							    \
typedef struct								    \
{									    \
  int nb_noeuds ;							    \
  t_noeud##NOM *noeuds ;						    \
  T_GRAPHE donnee ;							    \
} t_graphe##NOM ;							    \
CODEC_PROTOTYPE(codec_graphe##NOM, t_graphe##NOM ) \
CODEC_PROTOTYPE(codec_graphe##NOM##_arete, t_arete##NOM ) \
CODEC_PROTOTYPE(codec_graphe##NOM##_noeud, t_noeud##NOM )

/*
 * Définis dans "codec_base.c"
 */
int          codec_taille                   (Codec                        ) ;

void         codec_debut_libere             () ;
void         codec_fin_libere               () ;
int          codec_libere                   (Codec, void *a               ) ;

/*
void         codec_debut_sauve_texte        (const char *v                ) ;
void         codec_fin_sauve_texte          () ;
*/
int          codec_sauve_texte              (Codec, void *a, const char *v) ;

const char*  codec_fin_recupere_texte       (                             ) ;
const char*  codec_fin_recupere_texte_avec_lf(                             ) ;
void         codec_debut_recupere_texte     (                             ) ;
const char * codec_recupere_texte           (Codec, const void *a         ) ;
const char * codec_recupere_texte_avec_lf   (Codec, const void *a         ) ;

void         codec_debut_recupere_float     () ;
const float* codec_fin_recupere_float       (int *nb) ;
const float* codec_recupere_float           (Codec, const void *a, int *nb) ;

void putstr(const char*) ;

struct informations_table codec_informations_table(const Codec_informations*, const void *a) ;
int codec_positionne_element_table(struct informations_table *it, ...) ;
const char *codec_recupere_element_courant(struct informations_table *it) ;
const char *codec_recupere_element_courant_avec_lf(struct informations_table *it) ;
const float *codec_recupere_element_courant_float(struct informations_table *it, int *nb) ;
int codec_sauve_element_courant(struct informations_table *it, const char *v);
int codec_change_taille_table(struct informations_table *it, ...) ;
void codec_detruit_element_courant(struct informations_table *it) ;
void codec_duplique_element_courant(struct informations_table *it) ;

/* CODEC_PROTOTYPE_VO ID(codec_rien) */
int codec_rien() ;

CODEC_PROTOTYPE_VOID(codec_vide)
CODEC_PROTOTYPE_VOID(codec_linefeed        )
CODEC_PROTOTYPE_VOID(codec_ouvre_parenthese)
CODEC_PROTOTYPE_VOID(codec_ferme_parenthese)
CODEC_PROTOTYPE_VOID(codec_ouvre_crochet   )
CODEC_PROTOTYPE_VOID(codec_ferme_crochet   )
CODEC_PROTOTYPE_VOID(codec_ouvre_accolade  )
CODEC_PROTOTYPE_VOID(codec_ferme_accolade  )
int codec_free(char **a) ;
CODEC_PROTOTYPE(codec_float                       , float        ) /* 0    */
CODEC_PROTOTYPE(codec_float_ni                    , float        ) /* NON  */
CODEC_PROTOTYPE(codec_double                      , double       ) /* 0    */
CODEC_PROTOTYPE(codec_double_ni                   , double       ) /* NON  */
CODEC_PROTOTYPE(codec_int                         , int          ) /* 0    */
CODEC_PROTOTYPE(codec_int_ni                      , int          ) /* NON  */
CODEC_PROTOTYPE(codec_long                        , long         ) /* 0    */
CODEC_PROTOTYPE(codec_long_ni                     , long         ) /* NON  */
CODEC_PROTOTYPE(codec_unsigned_char               , unsigned char) /* 0    */
CODEC_PROTOTYPE(codec_unsigned_char_ni            , unsigned char) /* NON  */
CODEC_PROTOTYPE(codec_unsigned_int                , unsigned int ) /* 0    */
CODEC_PROTOTYPE(codec_unsigned_int_ni             , unsigned int ) /* NON  */
CODEC_PROTOTYPE(codec_unsigned_long               , unsigned long) /* 0    */
CODEC_PROTOTYPE(codec_unsigned_long_ni            , unsigned long) /* NON  */
CODEC_PROTOTYPE(codec_char_decimal                , char         ) /* 0    */
CODEC_PROTOTYPE(codec_char_decimal_ni             , char         ) /* NON */
CODEC_PROTOTYPE(codec_char                        , char         ) /* '\0' */
CODEC_PROTOTYPE(codec_char_etoile                 , char*        ) /* ""   */
CODEC_PROTOTYPE(codec_chaine                 , char*        ) /* ""   */
CODEC_PROTOTYPE(codec_char_etoile_sans_echappement, char*        ) /* ""   */
int codec_char_etoile_recupere_texte_sans_echappement(const char **a) ;


int codec_l            (void  *a,                     int  lar, Codec tp) ;
int codec_L            (void **a,                     int *lar, Codec tp) ;
int codec_H_L          (void***a,           int *hau, int *lar, Codec tp) ;
int codec_h_L          (void **a,           int  hau, int *lar, Codec tp) ;
int codec_L_h          (void **a,           int  hau, int *lar, Codec tp) ;
int codec_HL           (void **a,           int *hau, int *lar, Codec tp) ;
int codec_HH           (void **a,           int *hau,           Codec tp) ;
int codec_hl           (void  *a,           int  hau, int  lar, Codec tp) ;
int codec_PHL          (void **a, int *pro, int *hau, int *lar, Codec tp) ;
int codec_P_H_L        (void****a,int *pro, int *hau, int *lar, Codec tp) ;

int codec_l_hh  (void   *t1,Codec tp1,void   *t2,Codec tp2,int  lar) ;
int codec_L_hh  (void  **t1,Codec tp1,void  **t2,Codec tp2,int *lar);
int codec_H_L_pp(void ***t1,Codec tp1,void ***t2,Codec tp2,int *hau,int *lar) ;


int codec_enum_general (int   *a, int nb, char **noms,
			int *indices,
			int sensible_a_la_casse) ;
int codec_enum         (int   *a, int nb, char **noms) ;
/*
 * Pour l'union, l'entier peut être codé par "codec_int" ou "codec_enum"
 */
int codec_union            (void  *a, int *num, int nb, Codec type, Codec *tp,
			int parenthese) ;
int codec_pointeur         (void **a, Codec tp) ;
int codec_pointeur_non_null(void **a, Codec tp) ;


/*
 * Définis dans "codec.c"
 */

typedef struct{                   int              table[3];} t_l3_int        ;
typedef struct{                   int              table[4];} t_l4_int        ;
typedef struct{                   unsigned char table[3];} t_l3_unsigned_char ;
typedef struct{                   unsigned char table[4];} t_l4_unsigned_char ;
typedef struct{                   float            table[2];} t_l2_float      ;
typedef struct{                   double           table[2];} t_l2_double     ;
typedef struct{                   float            table[3];} t_l3_float      ;
typedef struct{                   double           table[3];} t_l3_double     ;
typedef struct{                   float            table[4];} t_l4_float      ;
typedef struct{                   float            table[6];} t_l6_float      ;
typedef struct{                   double           table[4];} t_l4_double     ;
typedef struct{                   t_l3_float       table[2];} t_l2_l3_float   ;
typedef struct{                   t_l3_double      table[2];} t_l2_l3_double  ;
typedef struct{                   t_l3_double      table[3];} t_l3_l3_double  ;
typedef struct{                   t_l3_double      table[4];} t_l4_l3_double  ;
typedef struct{                   double        table[6][4];} t_l6_l4_double  ;
typedef struct{int           lar; double          *table   ;} t_L_double      ;
typedef struct{int           lar; float           *table   ;} t_L_float       ;
typedef struct{int           lar; int             *table   ;} t_L_int         ;
typedef struct{int           lar; t_l3_double     *table   ;} t_L_l3_double   ;
typedef struct{int           lar; t_l4_double     *table   ;} t_L_l4_double   ;
typedef struct{int           lar; t_l4_int        *table   ;} t_L_l4_int      ;
typedef struct{int           lar; t_l2_double     *table   ;} t_L_l2_double   ;
typedef struct{int           lar; t_l2_float      *table   ;} t_L_l2_float    ;
typedef struct{int           lar; t_l4_l3_double  *table   ;} t_L_l4_l3_double;
typedef struct{int           lar; t_L_l3_double   *table   ;} t_L_L_l3_double ;
typedef struct{int           lar; t_L_l4_double   *table   ;} t_L_L_l4_double ;
typedef struct{int      hau, lar; t_l3_float      *table   ;} t_HL_l3_float   ;
typedef struct{int      hau, lar; t_l3_double     *table   ;} t_HL_l3_double  ;
typedef struct{int      hau, lar; t_l4_double     *table   ;} t_HL_l4_double  ;
typedef struct{int      hau, lar; t_l3_unsigned_char *table;} t_HL_l3_unsigned_char  ;
typedef struct{int      hau, lar; t_l4_unsigned_char *table;} t_HL_l4_unsigned_char  ;
typedef struct{int      hau, lar; unsigned char *table;} t_HL_unsigned_char  ;
typedef struct{int      hau     ; float           *table   ;} t_HH_float      ;
typedef struct{int      hau, lar; float          **table   ;} t_H_L_float     ;
typedef struct{int      hau, lar; double         **table   ;} t_H_L_double    ;
typedef struct{int      hau, lar; t_l3_double    **table   ;} t_H_L_l3_double ;
typedef struct{int      hau, lar; t_l4_double    **table   ;} t_H_L_l4_double ;
typedef struct{int           lar; float           *table[3];} t_h3_L_float    ;
typedef struct{int pro, hau, lar; t_l3_float      *table   ;} t_PHL_l3_float  ;
typedef struct{int pro, hau, lar; float           *table   ;} t_PHL_float     ;
typedef struct{int           lar; float         *x, *y, *z ;} t_L_h3_float    ;
typedef struct{int pro, hau, lar; t_l3_float    ***table   ;} t_P_H_L_l3_float;
typedef struct{int pro, hau, lar; t_l3_double   ***table   ;}t_P_H_L_l3_double;
typedef struct{int pro, hau, lar; t_l4_float    ***table   ;} t_P_H_L_l4_float;
typedef struct{int pro, hau, lar; t_l4_double   ***table   ;}t_P_H_L_l4_double;

typedef struct{ float table[3]; char *chaine ;} t_l3_float_chaine      ;
typedef struct{ float position[3], couleur[3]; char *chaine ;} t_pcc ;

typedef struct{ double angle; char unite ;} t_angle ;

CODEC_PROTOTYPE(codec_float1        , float          ) /* 1 */
CODEC_PROTOTYPE(codec_float1_lf     , float          ) /* 1 */
CODEC_PROTOTYPE(codec_float_lf      , float          ) /* 0 */
CODEC_PROTOTYPE(codec_double1       , double         ) /* 1 */
CODEC_PROTOTYPE(codec_double1_lf    , double         ) /* 1 */
CODEC_PROTOTYPE(codec_double_lf     , double         ) /* 0 */
CODEC_PROTOTYPE(codec_l3_int        , t_l3_int       ) /* 0 0 0 */
CODEC_PROTOTYPE(codec_l4_int        , t_l4_int       ) /* 0 0 0 0 */
CODEC_PROTOTYPE(codec_l2_float      , t_l2_float     ) /* 0 0 0 */
CODEC_PROTOTYPE(codec_l3_float      , t_l3_float     ) /* 0 0 0 */
CODEC_PROTOTYPE(codec_l3_float_chaine, t_l3_float_chaine ) /* 0 0 0 "" */
CODEC_PROTOTYPE(codec_l6_float      , t_l6_float     ) /* 0 0 0 0 0 0 */
CODEC_PROTOTYPE(codec_l3_float1     , t_l3_float     ) /* 0 0 1 */
CODEC_PROTOTYPE(codec_l2_double     , t_l2_double    ) /* 0 0 0 */
CODEC_PROTOTYPE(codec_l3_double     , t_l3_double    ) /* 0 0 0 */
CODEC_PROTOTYPE(codec_l3_double_nolf, t_l3_double    ) /* 0 0 0 */
CODEC_PROTOTYPE(codec_l3_double1    , t_l3_double    ) /* 0 0 1 */
CODEC_PROTOTYPE(codec_double_l2_double_ni, t_l3_double) /* 0 ni ni */
CODEC_PROTOTYPE(codec_l2_l3_float   , t_l2_l3_float  ) /* 0 0 0  0 0 0 */
CODEC_PROTOTYPE(codec_l2_l3_double  , t_l2_l3_double ) /* 0 0 0  0 0 0 */
CODEC_PROTOTYPE(codec_l3_l3_double  , t_l3_l3_double ) /* 0 0 0  0 0 0 0 0 0 */
CODEC_PROTOTYPE(codec_l4_l3_double  , t_l4_l3_double ) /* Matrice nulle */
CODEC_PROTOTYPE(codec_l6_l4_double  , t_l6_l4_double ) /* 0 */
CODEC_PROTOTYPE(codec_h4l3_double   , t_l4_l3_double ) /* Matrice nulle */
CODEC_PROTOTYPE(codec_l4_float      , t_l4_float     ) /* 0 0 0 0 */
CODEC_PROTOTYPE(codec_l4_double     , t_l4_double    ) /* 0 0 0 0 */
CODEC_PROTOTYPE(codec_l4_float1     , t_l4_float     ) /* 0 0 0 1 */
CODEC_PROTOTYPE(codec_l4_double1    , t_l4_double    ) /* 0 0 0 1 */
CODEC_PROTOTYPE(codec_L_double      , t_L_double     )
CODEC_PROTOTYPE(codec_L_float       , t_L_float      )
CODEC_PROTOTYPE(codec_L_int         , t_L_int        )
CODEC_PROTOTYPE(codec_L_l2_float    , t_L_l2_float   )
CODEC_PROTOTYPE(codec_L_l2_double   , t_L_l2_double  )
CODEC_PROTOTYPE(codec_L_l3_double   , t_L_l3_double  )
CODEC_PROTOTYPE(codec_L_l4_int      , t_L_l4_int     )
CODEC_PROTOTYPE(codec_L_l4_double   , t_L_l4_double  )
CODEC_PROTOTYPE(codec_L_l4_double1  , t_L_l4_double  )
CODEC_PROTOTYPE(codec_L_h4l3_double , t_L_l4_l3_double)
CODEC_PROTOTYPE(codec_L_L_l3_double , t_L_L_l3_double)
CODEC_PROTOTYPE(codec_L_L_l4_double , t_L_L_l4_double)
CODEC_PROTOTYPE(codec_HL_l3_double  , t_HL_l3_double )
CODEC_PROTOTYPE(codec_HL_l3_float   , t_HL_l3_float  )
CODEC_PROTOTYPE(codec_HL_l3_unsigned_char, t_HL_l3_unsigned_char)
CODEC_PROTOTYPE(codec_HL_l4_unsigned_char, t_HL_l4_unsigned_char)
CODEC_PROTOTYPE(codec_HL_unsigned_char, t_HL_unsigned_char)
CODEC_PROTOTYPE(codec_HL_l4_double  , t_HL_l4_double )
CODEC_PROTOTYPE(codec_HH_float      , t_HH_float     )
CODEC_PROTOTYPE(codec_H_L_l3_double , t_H_L_l3_double)
CODEC_PROTOTYPE(codec_H_L_l4_double , t_H_L_l4_double)
CODEC_PROTOTYPE(codec_H_L_float     , t_H_L_float    )
CODEC_PROTOTYPE(codec_H_L_double    , t_H_L_double   )
CODEC_PROTOTYPE(codec_h3_L_float    , t_h3_L_float   )
CODEC_PROTOTYPE(codec_L_h3_float    , t_h3_L_float   )
CODEC_PROTOTYPE(codec_PHL_l3_float  , t_PHL_l3_float )
CODEC_PROTOTYPE(codec_PHL_float     , t_PHL_float    )
CODEC_PROTOTYPE(codec_P_H_L_l3_float, t_P_H_L_l3_float )
CODEC_PROTOTYPE(codec_P_H_L_l3_double, t_P_H_L_l3_double )
CODEC_PROTOTYPE(codec_P_H_L_l4_float1, t_P_H_L_l4_float )
CODEC_PROTOTYPE(codec_P_H_L_l4_double1, t_P_H_L_l4_double )

CODEC_LISTE_PROTOTYPE(_int, codec_int, int) /*codec_liste_int(t_liste_int**) */
CODEC_LISTE_PROTOTYPE(_unsigned_long, codec_unsigned_long, unsigned long)

CODEC_PROTOTYPE(codec_angle, t_angle )

CODEC_GRAPHE_PROTOTYPE(					\
                   ,codec_rien, void*			\
                   ,codec_rien, void*			\
                   ,codec_rien, void*			\
                   ) /* Graphe sans information */
CODEC_GRAPHE_PROTOTYPE(_N_l3_float		\
                   ,codec_rien    , void*	\
                   ,codec_l3_float, t_l3_float	\
                   ,codec_rien    , void*
		   ) /* Graphe avec des l3_floats sur les noeuds */
CODEC_GRAPHE_PROTOTYPE(_N_l2_l3_float					\
                   ,codec_rien    , void*				\
                   ,codec_l2_l3_float, t_l2_l3_float			\
                   ,codec_rien    , void*				\
		   ) /* Graphe avec des l2_l3_floats sur les noeuds */
CODEC_GRAPHE_PROTOTYPE(_N_l6_float					\
                   ,codec_rien    , void*				\
                   ,codec_l6_float, t_l6_float			\
                   ,codec_rien    , void*				\
		   ) /* Graphe avec des l6_floats sur les noeuds */
CODEC_GRAPHE_PROTOTYPE(_N_l3_float_chaine_A_chaine		\
                   ,codec_rien    , void*				\
                   ,codec_l3_float_chaine, t_l3_float_chaine		\
                   ,codec_chaine, char*				\
		   )
CODEC_GRAPHE_PROTOTYPE(_N_pcc_A_chaine		\
                   ,codec_rien    , void*				\
                   ,codec_pcc, t_pcc		\
                   ,codec_chaine, char*				\
		   )

/*
*****************************************************************************
 * GRAPHE
 * La macro suivante permet de définir les données
 * que l'on met dans le graphe au graphe/noeud/aretes
*****************************************************************************
 */
#define CODEC_GRAPHE(NOM, GRAPHE, T_GRAPHE, NOEUD, T_NOEUD, ARETE, T_ARETE) \
TYPE_PHYSIQUE(codec_graphe##NOM##_arete, t_arete##NOM,			    \
{									    \
   if ( codec_int(&a->destination) ) return(1) ;			    \
   if ( (*ARETE)(&a->donnee) ) return(1) ;				    \
})									    \
TYPE_PHYSIQUE(codec_graphe##NOM##_noeud, t_noeud##NOM,			    \
{									    \
  codec_L((void**)&a->aretes,&a->nb_aretes,codec_graphe##NOM##_arete) ;	    \
  (*NOEUD)(&a->donnee) ;						    \
})									    \
TYPE_PHYSIQUE(codec_graphe##NOM, t_graphe##NOM,				    \
{									    \
  codec_L((void**)&a->noeuds,&a->nb_noeuds,codec_graphe##NOM##_noeud) ;	    \
  (*GRAPHE)(&a->donnee) ;						    \
})
/*
*****************************************************************************
 * LISTE
*****************************************************************************
 */
#define CODEC_LISTE(NOM, DONNEE, T_DONNEE)				\
int codec_liste##NOM##_recupere_texte(const t_liste##NOM **a)		\
{									\
  int err ;								\
  err = 0 ;								\
  codec_ouvre_parenthese() ;						\
  for( ; *a ; a = (const t_liste##NOM **)&(*a)->suivant )		\
    err |= (*infos_##DONNEE.recupere_texte)( &(*a)->donnee ) ;		\
  codec_ferme_parenthese() ;						\
  return(err) ;								\
}									\
int codec_liste##NOM##_recupere_float(const t_liste##NOM **a)		\
{									\
  int err ;								\
  err = 0 ;								\
  for( ; *a ; a = (const t_liste##NOM **)&(*a)->suivant )		\
    err |= (*infos_##DONNEE.recupere_float)( &(*a)->donnee ) ;		\
  return(err) ;								\
}									\
int codec_liste##NOM##_libere(t_liste##NOM **a)				\
{									\
  t_liste##NOM *x, *y ;							\
  int err ;								\
  err = 0 ;								\
  for( x=*a ; x ; x = y )						\
    {									\
      err |= (*infos_##DONNEE.libere)( &x->donnee ) ;			\
      y = x->suivant ;							\
      CODEC_FREE(x) ;							\
    }									\
  *a = NULL ;								\
  return(err) ;								\
}									\
int codec_liste##NOM##_sauve_texte(t_liste##NOM **a)			\
{									\
  int i ;								\
  T_DONNEE valeur ;							\
									\
  codec_ouvre_parenthese() ;						\
  for(i=0 ; *G->entree != '\n' && *G->entree != '\0' ; i++)		\
    {									\
      memset(&valeur, '\0', sizeof(valeur) ) ;				\
      if ( (*infos_##DONNEE.sauve_texte)(&valeur) )			\
        break ;								\
      if ( *a == NULL )							\
        {								\
          CODEC_MALLOC(*a, 1) ;						\
          (*a)->suivant = NULL ;					\
        }								\
      (*a)->donnee = valeur ;						\
      a = &(*a)->suivant ;						\
    }									\
  if (*a)								\
    codec_liste##NOM##_libere(a) ;         /* Enleve la fin de liste */	\
  codec_ferme_parenthese() ;						\
  return(0) ;								\
}									\
TYPE_DE_BASE(codec_liste##NOM, t_liste##NOM*,				\
             codec_liste##NOM##_recupere_texte,				\
             codec_liste##NOM##_sauve_texte,				\
             codec_liste##NOM##_libere,					\
             codec_liste##NOM##_recupere_float  			\
             )

/*
 * Pour l'exemple d'arbre
 */
typedef struct t_arbre_N_int_F_int
{
  int type ;
  union
  {
    int donnee_feuille ;
    struct
    {
      int donnee_noeud ;
      struct t_arbre_N_int_F_int *g, *d ;
    } noeud ;
  } t ;
} t_arbre_N_int_F_int ;

typedef struct t_arbre_N_int_F_int * t_pointeur_arbre_N_int_F_int ;

CODEC_PROTOTYPE(codec_arbre_N_int_F_int         , t_arbre_N_int_F_int) 
CODEC_PROTOTYPE(codec_pointeur_arbre_N_int_F_int, t_arbre_N_int_F_int* ) 

/*
 * Pour l'exemple d'octree
 */

typedef struct t_octree
{
  int type ;
  struct t_octree *fils[8] ;
} t_octree ;

typedef struct t_octree * t_pointeur_octree ;

CODEC_PROTOTYPE(codec_octree, t_octree)
CODEC_PROTOTYPE(codec_pointeur_octree, t_pointeur_octree)

/*
 * Pour tester
 */

typedef struct
{
  int lar ;
  t_l3_double *point ;
  double *poids ;
} t_L_hh_l3_double_double ;
CODEC_PROTOTYPE(codec_L_hh_l3_double_double, t_L_hh_l3_double_double)

#endif

