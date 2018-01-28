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
#include "u_base.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

#define CODEC_BASE
#include "codec.h"

#ifdef DEBUG_CODEC
#include "u_memoire.h"
Gestion_memoire globale_gm_codec = { 0 } ;
#else
int globale_codec_malloc = 0 ;
#endif

/*
 *---------------------
 * Utilitaires chaine
 *---------------------
 * Ses utilitaires sont adaptés précisemment au Codec
 * et utilisent une variable globale afin d'optimiser
 * le temps de calcul.
 */
void codec_reallouer()
{
  int l, p ;

  l = G->sortie_fin - G->sortie_debut ;
  p = G->sortie - G->sortie_debut ;
  l = INCREMENTE(l) ;
  G->sortie_debut = realloc(G->sortie_debut, l) ;
  G->sortie       = G->sortie_debut + p ;
  G->sortie_fin   = G->sortie_debut + l ;
}

void codec_efface()
{
  if ( G->sortie==NULL )
    {
      G->sortie_debut = malloc(1) ;
      G->sortie_fin   = G->sortie_debut + 1 ;
    }
  G->sortie = G->sortie_debut ;
  /*  G->sortie[0] = '\0' ; */
}

void putstr(const char *s)
{					
  int lon ;				

  lon = strlen(s) ;			
  while( G->sortie_fin - G->sortie <= lon )
    codec_reallouer() ;			
  memcpy(G->sortie, s, lon) ;		
  G->sortie += lon ;			
}


/*
 *----------------------
 * Fonctions utilitaires
 *----------------------
 *
 * Ces fonctions permettent d'utiliser simplement un Codec.
 * Ce sont celles qui sont utilisée par l'utilisateur final.
 *
 */

struct globale_traduction globale_pile_traduction[100] ;
int globale_change_environnement_traduction_n = 0 ;


void codec_debut_libere()
{
  change_environnement_traduction(+1) ;
  G->action            = Libere ;
}

void  codec_fin_libere()
{
  change_environnement_traduction(-1) ;

#ifdef DEBUG_CODEC
  eprintf("nb zones codec = %d\n", nb_zones_memoires(&globale_gm_codec)) ;
#endif
}

void codec_debut_recupere_texte()
{
  change_environnement_traduction(+1) ;

  G->action = Recupere_texte ;
  codec_efface() ;
}

const char* codec_fin_recupere_texte_avec_lf()
{
  const char *sortie ;

  sortie = G->sortie_debut ;
  change_environnement_traduction(-1) ;

  return( sortie ) ;
}


const char* codec_fin_recupere_texte()
{
  const char *sortie ;

  while ( G->sortie != G->sortie_debut
          && ( G->sortie[-1] == ' ' || G->sortie[-1] == '\n' )
          )
    G->sortie-- ;
  G->sortie[0] = '\0' ;
  sortie = G->sortie_debut ;

  change_environnement_traduction(-1) ;

  return( sortie ) ;
}

void codec_debut_recupere_float()
{
  change_environnement_traduction(+1) ;

  G->action = Recupere_float ;
  codec_efface() ;
}

const float* codec_fin_recupere_float(int *nb)
{
  float *sortie ;

  sortie = (float*)G->sortie_debut ;
  *nb = (G->sortie - G->sortie_debut)/sizeof(float) ;
  change_environnement_traduction(-1) ;

  return( sortie ) ;
}

const float *codec_recupere_float(Codec tp, const void *a, int *nb)
{
  codec_debut_recupere_float() ;
  (*tp)(a) ;
  return(codec_fin_recupere_float(nb)) ;
}

int codec_libere(Codec tp, void *a)
{
  int err ;

  change_environnement_traduction(+1) ;
  G->action = Libere ;
  err = (*tp)(a) ;
  change_environnement_traduction(-1) ;
  return(err) ;
}

const char *codec_recupere_texte(Codec tp, const void *a)
{
  codec_debut_recupere_texte() ;
  (*tp)(a) ;
  return(codec_fin_recupere_texte()) ;
}

const char *codec_recupere_texte_avec_lf(Codec tp, const void *a)
{
  codec_debut_recupere_texte() ;
  (*tp)(a) ;
  return(codec_fin_recupere_texte_avec_lf()) ;
}

int codec_sauve_texte(Codec tp, void *a, const char *v)
{
  int r ;

  codec_debut_sauve_texte(v) ;
  r = (*tp)(a) ;
  codec_fin_sauve_texte() ;

  return(r) ;
}


const Codec_informations* codec_informations(Codec tp)
{
  const Codec_informations *ci ;
  change_environnement_traduction(+1) ;
  G->action = Informations ;
  (*tp)(NULL) ;
  ci = G->informations ;
  change_environnement_traduction(-1) ;
  return(ci) ;
}

int codec_taille(Codec tp)
{
  return( codec_informations(tp)->taille ) ;
}

/*
 * Passage au suivant générique pour toutes les tables (pas pour les listes)
 * On fait varier le dernier indice le plus rapidement comme en C
 * Retourne 0 si pas de suivant.
 * Après le dernier, remet au premier
 */

int suivant_generique(struct informations_table *it)
{
  int d ;

  for(d=it->dimension-1; d>=0; d--)
    {
      it->courant[d]++ ;
      if ( it->courant[d] < it->taille[d] )
	break ;
      it->courant[d] = 0 ;
    }
  (*it->positionne_element)(it) ;
  return(1) ;
}

struct informations_table codec_informations_table(const Codec_informations *ci
						   , const void *a)
{
  struct informations_table it ;
  int i ;

  change_environnement_traduction(+1) ;

  G->action = Informations_table ;
  G->informations_table = &it ;
  MET_A_0(G->informations_table) ;
  G->informations_table->suivant = suivant_generique ;

  (*ci->generique)(a) ;

  if ( G->informations_table->dimension == 0 )
    EXIT ;
  for(i=0; i<G->informations_table->dimension; i++)
    if ( G->informations_table->pointeur_taille[i] )
      G->informations_table->taille[i]
	= *G->informations_table->pointeur_taille[i] ;
  if ( G->informations_table->pointeur_table )
    G->informations_table->table = *G->informations_table->pointeur_table ;
  if ( G->informations_table->pointeur_table2 )
    G->informations_table->table2 = *G->informations_table->pointeur_table2 ;

  (*it.positionne_element)(&it) ;

  change_environnement_traduction(-1) ;

  return(it) ;
}

int codec_positionne_element_table(struct informations_table *it, ...)
{
  va_list li ;
  int i ;

  va_start(li, it) ;
  for(i=0;i<it->dimension;i++)
      it->courant[i] = va_arg(li, int) ;
  va_end(li) ;

  for(i=0;i<it->dimension;i++)
      if ( it->courant[i]<0 || it->courant[i]>=it->taille[i] )
	{
	  eprintf("Débordement tableau dimension=%d %d>=%d\n",
		  i, it->courant[i], it->taille[i]) ;
	  it->courant[i] = 0 ;
	}

  (*it->positionne_element)(it) ;
  return(1) ;
}

int codec_change_taille_table(struct informations_table *it, ...)
{
  va_list li ;
  int i ;

  if ( it->change_taille == NULL )
    EXIT ;

  va_start(li, it) ;
  for(i=0;i<it->dimension;i++)
    {
      it->nouvelle_taille[i] = va_arg(li, int) ;
      it->courant[i] = 0 ;
    }
  va_end(li) ;
  (*it->change_taille)(it) ;
  (*it->positionne_element)(it) ;
  return(1) ;
}

void codec_recupere_element_courant_(struct informations_table *it)
{
  codec_debut_recupere_texte() ;
  (*it->type->recupere_texte)(it->element_courant) ;
  if ( it->type2 )
    (*it->type2->recupere_texte)(it->element_courant2) ;

  (*it->suivant)(it) ;
}

const char *codec_recupere_element_courant(struct informations_table *it)
{
  codec_recupere_element_courant_(it) ;
  return(codec_fin_recupere_texte()) ;
}

const char *codec_recupere_element_courant_avec_lf(struct informations_table *it)
{
  codec_recupere_element_courant_(it) ;
  return(codec_fin_recupere_texte_avec_lf()) ;
}

const float *codec_recupere_element_courant_float(struct informations_table *it, int *nb)
{
  codec_debut_recupere_float() ;
  (*it->type->recupere_float)(it->element_courant) ;
  if ( it->type2 )
    (*it->type2->recupere_float)(it->element_courant2) ;

  (*it->suivant)(it) ;

  return(codec_fin_recupere_float(nb)) ;
}

int codec_sauve_element_courant(struct informations_table *it, const char *v)
{
  int r ;

  codec_debut_sauve_texte(v) ;
  r = (*it->type->sauve_texte)(it->element_courant) ;
  if ( it->type2 )
    r |= (*it->type2->sauve_texte)(it->element_courant2) ;

  codec_fin_sauve_texte() ;
  (*it->suivant)(it) ;

  return(r) ;
}

/*
 * fonction sur TABLE pas grille (detruire_ligne, detruire_colonne)
 * fonction lente car générique, elle marchera aussi sur les listes.
 */
void codec_detruit_element_courant(struct informations_table *it)
{
  int i ;
  struct informations_table it2 ;
  
  it2 = *it ;
  (*it->suivant)(it) ;
  for(i=it->courant[0]; i<it->taille[0]; i++)
    codec_sauve_element_courant(&it2, codec_recupere_element_courant(it)) ;
  codec_change_taille_table(it, it->taille[0]-1) ;
}

void codec_duplique_element_courant(struct informations_table *it)
{
  int i ;
  char *t, *t2 ;
  struct informations_table it2 ;
  
  it2 = *it ;
  t = strdup(codec_recupere_element_courant(&it2)) ;
  (*it->suivant)(it) ;
  if ( it->courant[0] ) /* Bouclage droite/gauche */
    for(i=it->courant[0]; i<it->taille[0]; i++)
      {
	t2 = strdup(codec_recupere_element_courant(it)) ;
	codec_sauve_element_courant(&it2, t) ;
	free(t) ;
	t = t2 ;
      }
  codec_change_taille_table(it, it->taille[0]+1) ;
  codec_positionne_element_table(it, it->taille[0]-1) ;
  codec_sauve_element_courant(it, t) ;
  free(t) ;
}

/*
 * Debuggage
 */

void codec_print_context(char *t)
{
  int j ;
  for(j=0; G->entree[j] != '\n'; j--)
    {
    }
  while(++j < 0 )
    eprintf("%c", G->entree[j]) ;

  eprintf(" attend=%s trouve=%c  ", t, G->entree[0]) ;
	  
  while( G->entree[j] != '\0' && G->entree[j] != '\n' )
    eprintf("%c", G->entree[j++]) ;

  eprintf("\n") ;
}

/*
 * Utilitaire : ne rien faire.
 */
int codec_rien()
{
  return(0) ;
}
/*
 * Le Linefeed et autres : Ne sert qu'a simplifier les programmes
 */

#define codec_CARACTERE(NOM, CAR, OBLIGATOIRE, POST,SAUTEPAS)		\
int codec_##NOM##_recupere_texte()					\
{									\
  if ( OBLIGATOIRE )							\
     {									\
        while( G->sortie>G->sortie_debut					\
               && ( G->sortie[-1] == '\n' || G->sortie[-1] == ' ' ) )	\
          G->sortie-- ;							\
     }									\
  PUTCHAR(CAR) ;							\
  POST									\
  return(0) ;								\
}									\
int codec_##NOM##_sauve_texte()					\
{									\
  if (0) { char t[9] ; sprintf(t,"%c", CAR); codec_print_context(t) ; } \
  if ( OBLIGATOIRE )							\
     {									\
       while( *G->entree )						\
         {								\
           if ( CAR == *G->entree )					\
             {								\
               G->entree++ ;						\
               if ( CAR != '\n' )					\
                    while( *G->entree == '\n' )				\
                          G->entree++ ;					\
               return(0) ;						\
             }								\
if ( *G->entree == '\n' ) { G->entree++ ; return(0) ; }			\
           if ( SAUTEPAS )						\
             return(0) ;						\
           G->entree++ ;							\
         }								\
     }									\
  else									\
     {									\
       if ( *G->entree == CAR )						\
          G->entree++ ;							\
     }									\
   return(0) ;								\
}									\
TYPE_DE_BASE_VOID(codec_##NOM,						\
                  codec_##NOM##_recupere_texte,				\
                  codec_##NOM##_sauve_texte,				\
                  codec_rien,						\
                  codec_rien						\
                 )
codec_CARACTERE(linefeed        , '\n',1,,
            *G->entree == ')' || *G->entree == '}' || *G->entree == ']')
codec_CARACTERE(ouvre_parenthese, '(' ,0,,0 )
codec_CARACTERE(ferme_parenthese, ')' ,1, PUTCHAR('\n'),
         *G->entree == '}' || *G->entree == ']')
codec_CARACTERE(ouvre_crochet   , '[' ,0,,0 )
codec_CARACTERE(ferme_crochet   , ']' ,1, PUTCHAR('\n'),
         *G->entree == ']')
codec_CARACTERE(ouvre_accolade  , '{' ,0,,0 )
codec_CARACTERE(ferme_accolade  , '}' ,1, PUTCHAR('\n'),0)

TYPE_DE_BASE_VOID(codec_vide,
		  codec_rien,
		  codec_rien,
		  codec_rien,
		  codec_rien
		  )

/*
 * Type traduisible par un simple printf
 */

#define UPF(TYPE, TYPE_, FORMAT, FONCTION)		\
int codec_##TYPE_##_recupere_texte(const TYPE *a)	\
{							\
  if ( G->sortie_fin - G->sortie < 100 )		\
     codec_reallouer() ;				\
  G->sortie += sprintf(G->sortie, FORMAT " ", *a) ;	\
  return(0) ;						\
}							\
int codec_##TYPE_##_recupere_float(const TYPE *a)	\
{							\
  PUTFLOAT(*a) ;					\
  return(0) ;						\
}							\
int codec_##TYPE_##_sauve_texte_ni(TYPE *a)		\
{							\
  const char *tmp ;					\
  tmp = G->entree ;					\
  if ( *tmp == '\n' || *tmp == '\0' )			\
    return(1) ;						\
  *a = FONCTION ;					\
  if ( tmp == G->entree )				\
    return(1) ;						\
  if ( *G->entree == '.' || *G->entree == 'e' )		\
     while( *G->entree!='\0' && *G->entree!=' '		\
	    && *G->entree!='\t' && *G->entree!='\n' )	\
       G->entree++ ;					\
  SAUTE_BLANC ;						\
  return(0) ;						\
}							\
int codec_##TYPE_##_sauve_texte(TYPE *a)		\
{							\
  *a = 0. ;						\
  return(codec_##TYPE_##_sauve_texte_ni(a)) ;		\
}							\
TYPE_DE_BASE(codec_##TYPE_, TYPE,			\
             codec_##TYPE_##_recupere_texte,		\
             codec_##TYPE_##_sauve_texte,		\
             codec_rien,				\
             codec_##TYPE_##_recupere_float		\
             )						\
TYPE_DE_BASE(codec_##TYPE_##_ni, TYPE,			\
             codec_##TYPE_##_recupere_texte,		\
             codec_##TYPE_##_sauve_texte_ni,		\
             codec_rien,				\
             codec_##TYPE_##_recupere_float		\
             )

UPF(float        ,float        ,"%g" , strtod(G->entree,(char**)&G->entree    ))
UPF(double       ,double       ,"%g" , strtod(G->entree,(char**)&G->entree    ))
UPF(int          ,int          ,"%d" , strtol(G->entree,(char**)&G->entree, 10))
UPF(char         ,char_decimal ,"%d" , strtol(G->entree,(char**)&G->entree, 10))
UPF(unsigned char,unsigned_char,"%u" ,strtoul(G->entree,(char**)&G->entree, 10))
UPF(long         ,long         ,"%ld", strtol(G->entree,(char**)&G->entree, 10))
UPF(unsigned int ,unsigned_int ,"%u" ,strtoul(G->entree,(char**)&G->entree, 10))
UPF(unsigned long,unsigned_long,"%lu",strtoul(G->entree,(char**)&G->entree, 10))


/*
 * Char
 */

int codec_char_recupere_texte(const char *a)
{
  if ( *a == '\n' || *a == '\\' )
    {
      PUTCHAR('\\') ;
      PUTCHAR(*a) ;
    }
  else
    {
      if ( *a == '\0' )
	{
	  PUTCHAR('\\') ;
	  PUTCHAR('0') ;
	}
      else
	PUTCHAR(*a) ;
    }
  return(0) ;
}

int codec_char_recupere_float(const char *a)
{
  PUTFLOAT(*a) ;
  return(0) ;
}

int codec_char_sauve_texte(char *a)
{
  if ( *G->entree == '\\' )
    {
      switch(G->entree[1])
	{
	case '0'  : *a = '\0' ; break ;
	case '\\' : *a = '\\' ; break ;
	case 'n'  : *a = '\n' ; break ;
	default   : *a = '\0' ; G->entree++ ; return(1) ;
	}
      G->entree += 2 ;
    }
  else
    if ( *G->entree == '\n' || *G->entree == '\0' )
      {
        *a = *G->entree ;
        return(1) ;
      }
    else
      *a = *G->entree++ ;
  return(0) ;
}

TYPE_DE_BASE(codec_char, char,
             codec_char_recupere_texte,
             codec_char_sauve_texte,
             codec_rien,
             codec_char_recupere_float
             )

/*
 * Char*
 */
int codec_char_etoile_recupere_texte(const char **a)
{
  const char *p ;

  for( p = *a ; *p ; p++ )
    {
      if ( *p == '\n' || *p == '\\' )
        PUTCHAR('\\') ;
      PUTCHAR(*p) ;
    }
  PUTCHAR('\n') ;
  return(0) ;
}

int codec_chaine_recupere_texte(const char **a)
{
  const char *p ;

  PUTCHAR('"') ;
  for( p = *a ; *p ; p++ )
    {
      if ( *p == '\\' || *p == '"' )
	PUTCHAR('\\') ;
      PUTCHAR(*p) ;
    }
  PUTCHAR('"') ;
  PUTCHAR(' ') ;
  return(0) ;
}

int codec_char_etoile_recupere_float(const char **a)
{
  PUTFLOAT( atof(*a) ) ;
  return(0) ;
}

int codec_char_etoile_recupere_texte_sans_echappement(const char **a)
{
  putstr(*a) ;
  return(0) ;
}

int codec_char_etoile_sauve_texte_sans_echappement(char **a)
{
  int lon ;

  lon = strlen(G->entree) ;
  CODEC_REALLOC(*a, lon+1) ;
  strcpy(*a, G->entree) ;
  G->entree += lon ;
  return(0) ;
}

int codec_char_etoile_sauve_texte(char **a)
{
  const char *r, *p ;
  char *w ;

  if (*a)
   CODEC_FREE(*a) ;

  for( p = G->entree ; *p != '\n' && *p != '\0' ; p = r )
    {
      r = p + strcspn(p, "\\\n") ;
      if ( *r == '\\' )
        {
          if ( r[1] )
            r += 2 ;
          else
            r++ ;
        }
    }
  CODEC_MALLOC(*a, (p - G->entree) + 1) ;
  r = G->entree ;
  w = *a ;
  while( r != p )
    {
      if ( *r == '\\' )
        r++ ;
      *w++ = *r++ ;
    }
  *w = '\0' ;
  G->entree = p + 1 ;
  return(0) ;
}

int codec_chaine_sauve_texte(char **a)
{
  const char *r, *p ;
  char *w ;

  while( *G->entree != '"' && *G->entree != '\0' )
    G->entree++ ;

  if ( *G->entree == '"' )
    G->entree++ ;
  
  p = G->entree ;
  while( *p != '"' && *p != '\0' )
    {
      if ( *p == '\\' )
	{
	  p++ ;
	  if ( *p == '\0' )
	    break ;
	}
      p++ ;
    }
  CODEC_REALLOC(*a, (p - G->entree) + 1) ;
  r = G->entree ;
  w = *a ;
  while( r != p )
    {
      if ( *r == '\\' )
        r++ ;
      *w++ = *r++ ;
    }
  *w = '\0' ;
  G->entree = *p == '\0' ? p : p + 1 ;
  return(0) ;
}

int codec_free(char **a)
{
  CODEC_FREE(*a) ;
  return(0) ;
}
TYPE_DE_BASE(codec_char_etoile, char*,
	     codec_char_etoile_recupere_texte,
	     codec_char_etoile_sauve_texte,
	     codec_free,
	     codec_char_etoile_recupere_float
	     )
TYPE_DE_BASE(codec_char_etoile_sans_echappement, char*,
	     codec_char_etoile_recupere_texte_sans_echappement,
	     codec_char_etoile_sauve_texte_sans_echappement,
	     codec_free,
	     codec_char_etoile_recupere_float
	     )
TYPE_DE_BASE(codec_chaine, char*,
	     codec_chaine_recupere_texte,
	     codec_chaine_sauve_texte,
	     codec_free,
	     codec_char_etoile_recupere_float
	     )
/*
 * L'ordre des indices est le même que pour les tableaux C.
 *
 * La structure de tableau de taille variable contient
 * un entier par indice dans le même ordre
 * (si le tableau est de taille variable)
 *
 * Les indices sont nommés dans l'ordre : Profondeur, Hauteur, Largeur
 *
 * Une lettre minuscule indique que l'indice max est une constante.
 * Dans ce cas, l'indice max n'est pas stocké dans la structure.
 * Le nom de l'instance contient la taille constante après
 * la lettre minuscule : "float_Hl5" est donc "float (*t)[5] ;"
 *
 * Le souligné indique un pointeur vers la dimension suivante.
 *
 *
 ******************************************************************************
 * Tableau à une dimension
 ******************************************************************************
 * L    : void *t ;                                          *** IMPLEMENTE ***
 * l    : void t[MaxL] ;                                     *** IMPLEMENTE ***
 ******************************************************************************
 * Tableau à deux dimensions
 ******************************************************************************
 * HL   : void *t ;                                          *** IMPLEMENTE ***
 * hl   : void t[MaxH][MaxL] ;                               *** IMPLEMENTE ***
 * Hl   : void (*t)[MaxL] ;     Ou :   void t[][Maxl]
 * hL   : void t[MaxH][] ;      Déclaration impossible en C
 *
 * H_L  : void **t ;                                         *** IMPLEMENTE ***
 * H_l  : void (**t)[MaxL] ;
 * h_l  : void (*t[MaxH])[MaxL] ;
 * h_L  : void *t[MaxH] ;
 ******************************************************************************
 * Tableau à trois dimensions
 ******************************************************************************
 * PHL  : void *t ;                                          *** IMPLEMENTE ***
 * phl  : void t[MaxP][MaxH][MaxL] ;
 * Phl  : void (*t)[MaxH][MaxL] ;
 * PHl  : void (*t)[MaxL] ;
 * P_HL : void **t ;
 * p_HL : void *t[MaxP] ;
 * p_hl : void (*t[MaxP])[MaxH][MaxL] ;
 * PH_L : void **t ;
 * PH_l : void (**t)[MaxL] ;
 * ph_l : void (*t[MaxP][Maxh])[MaxL] ;
 * ph_L : void *t[MaxP][Maxh] ;
 * p_h_l: void (*(*t[MaxP])[MaxH])[MaxL] ;
 * P_h_l: void (*(**t)[MaxH])[MaxL] ;
 * P_H_l: void (***t)[MaxL] ;
 * P_H_L: void ***t ;
 ******************************************************************************
 * Combinaisons :
 ******************************************************************************
 * L_L  : Table de table
 * HL_L : Matrice de table de tailles différentes
 * L_HL : Table de matrices différentes
 * ...
 */

/*
 *
 */

int positionne_element_l(struct informations_table *it)
{
  if ( it->table )
    it->element_courant = (char*)it->table + it->courant[0]*it->type->taille ;
  return(1) ;
}

int codec_l(void *a, int nb, Codec tp)
{
  int i, err ;
  int taille ;
  Codec fct ;
  const void *sauve ;
  const Codec_informations *ci ;

  ci = codec_informations(tp) ;
  fct    = POINTEUR_METHODE(ci) ;
  taille = ci->taille ;
  err = 0 ;

  switch(G->action)
    {
    case Recupere_texte :
    case Recupere_float :
      for(i=0 ; i<nb ; i++)
        {
          err |= (*fct)(a) ;
          a = (char*)a + taille ;
        }
      break ;

    case Sauve_texte    :
      sauve = G->entree ;
      for(i=0 ; i<nb ; i++)
        {
          if ( *G->entree == '\n' )
            {
              sauve = G->entree ;
              while( i++<nb )
                {
                  (*fct)(a) ;
		  a = (char*)a + taille ;
                  G->entree = sauve ;
                }
              return(1) ;
            }
          err |= (*fct)(a) ;
	  a = (char*)a + taille ;
        }
      if ( G->entree!=sauve && G->entree[-1]=='\n' ) 
	codec_linefeed_sauve_texte() ;
      break ;

    case Libere         :
      for(i=0 ; i<nb ; i++)
        {
          err |= (*fct)(a) ;
	  a = (char*)a + taille ;
        }
      break ;

    case Informations   : abort() ; break ;

    case Informations_table :
      if ( G->informations_table->dimension )
	EXIT ;
      G->informations_table->dimension = 1 ;
      G->informations_table->taille[0] = nb ;
      G->informations_table->type = ci ;
      G->informations_table->table = a ;
      G->informations_table->positionne_element = positionne_element_l ;
      break ;
    }
  return(err) ;
}
/*
 *
 */

int change_taille_L_(void **table, int *taille, int nouvelle
		     , const Codec_informations *ci)
{
  int i ;
  
  if ( nouvelle < *taille )
    {
      codec_debut_libere() ;
      for(i=nouvelle; i<*taille; i++)
	(*ci->libere)((char*)*table + i*ci->taille) ;
      codec_fin_libere() ;
      CODEC_REALLOC(*(char**)table, nouvelle * ci->taille) ;
    }
  else
    if ( nouvelle > *taille )
      {
	CODEC_REALLOC(*(char**)table, nouvelle * ci->taille) ;
	memset((char*)*table + *taille * ci->taille
	       , '\0', (nouvelle - *taille)*ci->taille ) ;
      }
  *taille = nouvelle ;
  return(1) ;
}

int change_taille_L(struct informations_table *it)
{
  change_taille_L_(it->pointeur_table
		   , it->pointeur_taille[0], it->nouvelle_taille[0]
		   , it->type) ;
  it->table = *it->pointeur_table ;
  it->taille[0] = it->nouvelle_taille[0] ;
  return(1) ;
}


int codec_L(void **table, int *lar, Codec tp)
{
  int i ;
  int taille ;
  void *a ;
  int max ;
  Codec fct ;
  int err ;
  const Codec_informations *ci ;

  ci = codec_informations(tp) ;
  fct = POINTEUR_METHODE(ci) ;
  taille = ci->taille ;
  err = 0 ;

  codec_ouvre_parenthese() ;

  switch( G->action )
    {
    case Informations :
      fprintf(stderr,"codec_L_taille_variable\n") ;
      abort() ;

    case Recupere_texte :
    case Libere :
    case Recupere_float :
      a = *table ;
      for(i=0 ; i<*lar ; i++)
        {
          err |= (*fct)(a) ;
          a = (char*)a + taille ;
        }
      if ( G->action == Libere )
        {
	  CODEC_FREE(*table) ;
          *lar = 0 ;
        }
      break ;

    case Sauve_texte :
      max = *lar ;
      if ( max && *table==NULL )
	{
	  fprintf(stderr, "Vous avez mis un tableau (codec_L) à NULL sans mettre\n"
		  "à 0 le nombre d'élément, ce que je fais\n") ;
	  max = 0 ;
	}
      for(i=0 ; *G->entree!=')' && *G->entree!='\n' && *G->entree!='\0' ; i++)
        {
          if ( i >= max )
	    change_taille_L_(table, &max, INCREMENTE(max), ci) ;
          if ( (*fct)((char*)*table + i*taille) )
	    break ;
        }
      *lar = i ;
      if ( G->economise_memoire==0 )
	change_taille_L_(table, &max, *lar, ci) ;
      break ;
      
    case Informations_table :
      if ( G->informations_table->dimension )
	EXIT ;
      G->informations_table->dimension = 1 ;
      G->informations_table->pointeur_taille[0] = lar ;
      G->informations_table->type = ci ;
      G->informations_table->pointeur_table = table ;
      G->informations_table->positionne_element = positionne_element_l ;
      G->informations_table->change_taille = change_taille_L ;
      break ;      
    }
  codec_ferme_parenthese() ;
  return(err) ;
}

/*
 * Premier indice : No Ligne, maximum : hau
 */

int positionne_element_H_L(struct informations_table *it)
{
  if ( it->table && ((char**)it->table)[it->courant[0]] )
    it->element_courant = ((char**)it->table)[it->courant[0]]
      + it->courant[1] * it->type->taille ;
  return(1) ;
}

/*
 * Le contenu de la table est incohérent
 * car il reste tassé au début de la table
 */

int change_taille_H_L_(void ***table, int *hau, int n_hau
		       , int *lar, int n_lar
		       , const Codec_informations *ci)
{
  int i ;
  
  if ( n_hau < *hau )
    {
      codec_debut_libere() ;
      for(i=n_hau; i < *hau ; i++)
	codec_l((*table)[i], *lar, ci->generique) ;
      codec_fin_libere() ;
      CODEC_REALLOC((*table), n_hau) ;
    }
  else
    if ( n_hau > *hau )
      {
	CODEC_REALLOC((*table), n_hau) ;
	for(i = *hau ; i < n_hau ; i++)
	  {
	    CODEC_MALLOC( (*(char***)table)[i], ci->taille * n_lar) ;
	    memset((*table)[i], '\0', ci->taille * n_lar) ;
	  }
      }

  if ( n_lar != *lar )
    for(i = 0 ; i < *hau ; i++)
      {
	int t ;

	t = *lar ;
	change_taille_L_(&(*table)[i], &t, n_lar, ci) ;
      }

  *lar = n_lar ;
  *hau = n_hau ;
  return(1) ;
}


int change_taille_H_L(struct informations_table *it)
{
  change_taille_H_L_((void***)it->pointeur_table
		     , it->pointeur_taille[0], it->nouvelle_taille[0]
		     , it->pointeur_taille[1], it->nouvelle_taille[1]
		     , it->type
		     ) ;

  it->taille[0] = it->nouvelle_taille[0] ;
  it->taille[1] = it->nouvelle_taille[1] ;
  *it->pointeur_taille[0] = it->nouvelle_taille[0] ;
  *it->pointeur_taille[1] = it->nouvelle_taille[1] ;
  return(1) ;
}

int change_taille_H_L_old(struct informations_table *it)
{
  int i ;
  
  if ( it->nouvelle_taille[0] < it->taille[0] )
    {
      codec_debut_libere() ;
      for(i=it->nouvelle_taille[0]; i <  it->taille[0] ; i++)
	codec_l(((void**)it->table)[i], it->taille[1], it->type->generique) ;
      codec_fin_libere() ;
      CODEC_REALLOC(*(void****)&it->pointeur_table, it->nouvelle_taille[0]) ;
      it->table = *it->pointeur_table ;
    }
  else
    if ( it->nouvelle_taille[0] > it->taille[0] )
      {
	CODEC_REALLOC(*(void***)it->pointeur_table, it->nouvelle_taille[0]) ;
	it->table = *it->pointeur_table ;
	for(i = it->taille[0] ; i < it->nouvelle_taille[0] ; i++)
	  {
	    CODEC_MALLOC( ((char**)it->table)[i]
			 , it->type->taille*it->nouvelle_taille[1]
			 ) ;
	    memset(((void**)it->table)[i]
		   , '\0',  it->type->taille*it->nouvelle_taille[1]) ;
	  }
      }

  if ( it->nouvelle_taille[1] != it->taille[1] )
    for(i = 0 ; i < it->taille[0] ; i++)
      {
	int t ;

	t = it->taille[1] ;
	change_taille_L_(&((void**)it->table)[i], &t, it->nouvelle_taille[1]
			 , it->type) ;
      }


  it->taille[0] = it->nouvelle_taille[0] ;
  it->taille[1] = it->nouvelle_taille[1] ;
  *it->pointeur_taille[0] = it->nouvelle_taille[0] ;
  *it->pointeur_taille[1] = it->nouvelle_taille[1] ;
  return(1) ;
}


int codec_H_L(void ***table, int *hau, int *lar, Codec tp)
{
  int i, j ;
  int old_lar, old_lar2 ;
  int max ;
  int err ;
  const Codec_informations *ci ;

  codec_ouvre_crochet() ;
  err = 0 ;

  /*  eprintf("action=%d hau=%p(%d) lar=%p(%d) table=%p(%p)\n",G->action,  hau,*hau, lar,*lar, table, *table) ; */

  switch( G->action )
    {
    case Informations :
      fprintf(stderr,"codec_H_L\n") ;
      abort() ;

    case Recupere_texte :
    case Recupere_float :
      for(i=0 ; i<*hau ; i++)
        {
          codec_ouvre_parenthese() ;
          err |= codec_l((*table)[i], *lar, tp) ;
          codec_ferme_parenthese() ;
        }
      break ;

    case Libere :
      for(i=0 ; i<*hau ; i++)
        {
          err |= codec_l((*table)[i], *lar, tp) ;
          CODEC_FREE((*table)[i]) ;
        }
      CODEC_FREE(*table) ;
      *hau = 0 ;
      *lar = 0 ;
      break ;

    case Sauve_texte :
      max = *hau ;
      old_lar = *lar ;
      ci = codec_informations(tp) ;
      if ( max && *table==NULL )
	{
	  fprintf(stderr, "Vous avez mis un tableau (codec_H_L) à NULL sans mettre\n"
		  "à 0 le nombre d'élément, ce que je fais\n") ;
	  max = 0 ;
	}
      for(i=0 ; *G->entree!=']' && *G->entree!='\n' && *G->entree!='\0' ; i++)
        {
          if ( i >= max )
            {
              max = INCREMENTE(max) ;
              CODEC_REALLOC(*table, max) ;
              memset(&(*table)[i], '\0', (max-i)*sizeof(void*)) ;
            }
          if ( i==0 )
            {
              err |= codec_L(&(*table)[0], lar, tp) ;
            }
          else
            {
	      old_lar2 = old_lar ;
	      change_taille_L_(&(*(void ***)table)[i], &old_lar2, *lar, ci) ;

              codec_ouvre_parenthese() ;
              err |= codec_l((*table)[i], *lar, tp) ;
              codec_ferme_parenthese() ;
            }
        }
      if ( i < *hau )
	{
	  codec_debut_libere() ;
	  for( j=i; j<*hau; j++)
	    {
	      codec_l((*table)[j], *lar, tp) ;
	      CODEC_FREE((*table)[j]) ;
	    }
	  codec_fin_libere() ;
	}
      *hau = i ;
      CODEC_REALLOC(*table, *hau) ;
      break ;

    case Informations_table :
      if ( G->informations_table->dimension )
	EXIT ;
      G->informations_table->dimension = 2 ;
      G->informations_table->taille[0] = *hau ;
      G->informations_table->taille[1] = *lar ;
      G->informations_table->type = codec_informations(tp) ;
      G->informations_table->table = *table ;
      G->informations_table->positionne_element = positionne_element_H_L ;
      G->informations_table->change_taille = change_taille_H_L ;
      G->informations_table->pointeur_table = (void**)table ;
      G->informations_table->pointeur_taille[0] = hau ;
      G->informations_table->pointeur_taille[1] = lar ;
      break ;      
    }
  codec_ferme_crochet() ;
  return(err) ;
}

/*
****************************************************************************
GRILLE
****************************************************************************
*/

/*
 * Premier indice : No Ligne, maximum : hau
 */

int positionne_element_P_H_L(struct informations_table *it)
{
  if ( it->table && ((char***)it->table)[it->courant[0]] )
    if ( it->table && ((char***)it->table)[it->courant[0]][it->courant[1]] )
      it->element_courant = ((char***)it->table)[it->courant[0]][it->courant[1]]
	+ it->courant[2] * it->type->taille ;
  return(1) ;
}

int change_taille_P_H_L(struct informations_table *it)
{
  int i, hau, lar ;
  
  if ( it->nouvelle_taille[0] < it->taille[0] )
    {
      codec_debut_libere() ;
      for(i=it->nouvelle_taille[0]; i <  it->taille[0] ; i++)
	{
	  hau = it->taille[1] ;
	  lar = it->taille[2] ;
	  codec_H_L(((void**)it->table)[i], &hau, &lar, it->type->generique) ;
	}
      codec_fin_libere() ;
      CODEC_REALLOC(*(void****)&it->pointeur_table, it->nouvelle_taille[0]) ;
      it->table = *it->pointeur_table ;
    }
  else
    if ( it->nouvelle_taille[0] > it->taille[0] )
      {
	CODEC_REALLOC(*(void***)it->pointeur_table, it->nouvelle_taille[0]) ;
	it->table = *it->pointeur_table ;
	for(i = it->taille[0] ; i < it->nouvelle_taille[0] ; i++)
	  {
	    hau = 0 ;
	    lar = 0 ;
	    change_taille_H_L_( &((void***)it->table)[i]
				, &hau, it->nouvelle_taille[0]
				, &lar, it->nouvelle_taille[1]
				, it->type
				) ;
	  }
      }

  if ( it->nouvelle_taille[1] != it->taille[1] 
       || it->nouvelle_taille[2] != it->taille[2] 
       )
    for(i = 0 ; i < it->taille[0] ; i++)
    {
      hau = it->taille[1] ;
      lar = it->taille[2] ;
      change_taille_H_L_( &((void***)it->table)[i]
			  , &hau, it->nouvelle_taille[0]
			  , &lar, it->nouvelle_taille[1]
			  , it->type
			  ) ;
    }
  
  for(i=0; i<3; i++)
    {
      it->taille[i] = it->nouvelle_taille[i] ;
      *it->pointeur_taille[i] = it->nouvelle_taille[i] ;
    }
  return(1) ;
}




int codec_P_H_L(void ****table, int *pro, int *hau, int *lar, Codec tp)
{
  int i, j, h, l, h2, l2 ;
  int max ;
  int err ;
  const Codec_informations *ci ;

  codec_ouvre_accolade() ;
  err = 0 ;

  switch( G->action )
    {
    case Informations :
      fprintf(stderr,"codec_P_H_L\n") ;
      abort() ;

    case Recupere_texte :
    case Recupere_float :
      for(i=0 ; i<*pro ; i++)
        {
	  codec_ouvre_crochet() ;
	  for(j=0; j<*hau; j++)
	    {
	      codec_ouvre_parenthese() ;
	      err |= codec_l((*table)[i][j], *lar, tp) ;
	      codec_ferme_parenthese() ;
	    }
	  codec_ferme_crochet() ;
        }
      break ;

    case Libere :
      for(i=0 ; i<*pro ; i++)
	{
	  for(j=0; j<*hau; j++)
	    {
	      err |= codec_l((*table)[i][j], *lar, tp) ;
	      CODEC_FREE((*table)[i][j]) ;
	    }
          CODEC_FREE((*table)[i]) ;
	}
      CODEC_FREE(*table) ;

      *pro = 0 ;
      *hau = 0 ;
      *lar = 0 ;
      break ;

    case Sauve_texte :
      max = *pro ;
      h = *hau ;
      l = *lar ;
      ci = codec_informations(tp) ;
      if ( max && *table==NULL )
	{
	  fprintf(stderr, "Vous avez mis un tableau (codec_P_H_L) à NULL sans mettre\n"
		  "à 0 le nombre d'élément, ce que je fais\n") ;
	  max = 0 ;
	}
      for(i=0 ; *G->entree!='}' && *G->entree!='\n' && *G->entree!='\0' ; i++)
        {
          if ( i >= max )
            {
              max = INCREMENTE(max) ;
              CODEC_REALLOC(*table, max) ;
              memset(&(*table)[i], '\0', (max-i)*sizeof(void*)) ;
            }
	  if ( i == 0 )
	    {
	      codec_H_L(&(*table)[0], hau, lar, tp) ;
	    }
	  else
	    {
	      if ( (*table)[i] )
		{
		  h2 = h ;
		  l2 = l ;
		}
	      else
		{
		  h2 = 0 ;
		  l2 = 0 ;
		}
	      codec_H_L(&(*table)[i], &h2, &l2, tp) ;
	      change_taille_H_L_(&(*table)[i], &h2, *hau, &l2, *lar, ci) ;
	    }
        }
      if ( i < *pro )
	{
	  codec_debut_libere() ;
	  for(j=i; j<*pro; j++ )
	    {
	      h2 = h ;
	      l2 = l ;
	      codec_H_L(&(*table)[j], &h2, &l2, tp) ;
	    }
	  codec_fin_libere() ;
	}

      *pro = i ;
      CODEC_REALLOC(*table, *pro) ;
      break ;

    case Informations_table :
      if ( G->informations_table->dimension )
	EXIT ;
      G->informations_table->dimension = 3 ;
      G->informations_table->taille[0] = *pro ;
      G->informations_table->taille[1] = *hau ;
      G->informations_table->taille[2] = *lar ;
      G->informations_table->type = codec_informations(tp) ;
      G->informations_table->table = *table ;
      G->informations_table->positionne_element = positionne_element_P_H_L ;
      G->informations_table->change_taille = change_taille_P_H_L ;
      G->informations_table->pointeur_table = (void**)table ;
      G->informations_table->pointeur_taille[0] = pro ;
      G->informations_table->pointeur_taille[1] = hau ;
      G->informations_table->pointeur_taille[2] = lar ;
      break ;      
    }
  codec_ferme_accolade() ;
  return(err) ;
}




/*
 * Premier indice : No Ligne, maximum : hau
 */

int positionne_element_HL(struct informations_table *it)
{
  if ( it->table )
    it->element_courant = (char*)it->table
      + it->type->taille * ( it->courant[0]*it->taille[1] + it->courant[1] ) ;
  return(1) ;
}
/*
 * Le contenu de la table est incohérent
 * car il reste tassé au début de la table
 */

int change_taille_HL(struct informations_table *it)
{
  int taille, nouvelle ;

  taille = *it->pointeur_taille[0] * *it->pointeur_taille[1] ;
  nouvelle = it->nouvelle_taille[0] * it->nouvelle_taille[1] ;

  change_taille_L_(it->pointeur_table, &taille, nouvelle, it->type) ;

  it->table = *it->pointeur_table ;

  it->taille[0] = it->nouvelle_taille[0] ;
  it->taille[1] = it->nouvelle_taille[1] ;
  *it->pointeur_taille[0] = it->nouvelle_taille[0] ;
  *it->pointeur_taille[1] = it->nouvelle_taille[1] ;
  return(1) ;
}

int codec_HL(void **table, int *hau, int *lar, Codec tp)
{
  int i, j ;
  int taille ;
  int lart ;
  int oldmax, newmax ;
  int err ;

  codec_ouvre_crochet() ;
  taille = codec_taille(tp) ;
  err = 0 ;

  switch( G->action )
    {
    case Informations :
      fprintf(stderr,"codec_HL\n") ;
      abort() ;

    case Recupere_float :
    case Recupere_texte :
      for(i=0 ; i<*hau ; i++)
        {
          codec_ouvre_parenthese() ;
          err |= codec_l((char*)*table + i * *lar * taille, *lar, tp) ;
          codec_ferme_parenthese() ;
        }
      if ( G->action == Recupere_texte )
	PUTCHAR('\n') ;
      break ;

    case Libere :
      for(i=0 ; i<*hau ; i++)
        {
          err |= codec_l((char*)*table + i * *lar * taille, *lar, tp) ;
        }
     CODEC_FREE(*table) ;
      *lar = 0 ;
      *hau = 0 ;
      break ;

    case Sauve_texte :
      oldmax = *hau * *lar * taille ;
      if ( oldmax && *table==NULL )
	{
	  fprintf(stderr
		  , "Vous avez mis un tableau (codec_HL) à NULL sans mettre\n"
		  "à 0 le nombre d'éléments, ce que je fais\n") ;
	  oldmax = 0 ;
	}
      lart = 0 ;
      for(i=0, j=0;
          *G->entree != ']' && *G->entree != '\n' && *G->entree != '\0' ;
          i++, j+=lart
          )
        {
          if ( i==0 )
            {
              G->economise_memoire++ ;
              err |= codec_L(table, lar, tp) ;
              G->economise_memoire-- ;
              lart = *lar * taille ;
	      if ( oldmax < lart )
		oldmax = lart ;
            }
          else
            {
              while ( j + lart > oldmax )
                {
                  newmax = INCREMENTE(oldmax) ;
                  CODEC_REALLOC(*(char**)table, newmax) ;
                  memset((char*)*table + oldmax, '\0', newmax - oldmax ) ;
		  oldmax = newmax ;
                }
              codec_ouvre_parenthese() ;
              err |= codec_l((char*)*table + j, *lar, tp) ;
              codec_ferme_parenthese() ;
            }
        }
      *hau = i ;
      if ( G->economise_memoire==0 )
          CODEC_REALLOC(*(char**)table, *hau * lart) ;
      break ;

    case Informations_table :
      if ( G->informations_table->dimension )
	EXIT ;
      G->informations_table->dimension = 2 ;
      G->informations_table->taille[0] = *hau ;
      G->informations_table->taille[1] = *lar ;
      G->informations_table->type = codec_informations(tp) ;
      G->informations_table->table = *table ;
      G->informations_table->positionne_element = positionne_element_HL ;
      G->informations_table->change_taille = change_taille_HL ;
      G->informations_table->pointeur_taille[0] = hau ;
      G->informations_table->pointeur_taille[1] = lar ;
      G->informations_table->pointeur_table = table ;
      break ;      
    }
  codec_ferme_crochet() ;
  return(err) ;
}

/*
 *
 */

int codec_hl(void *a, int hau, int lar, Codec tp)
{
  int i ;
  int taille ;
  const void *sauve ;
  int err ;

  if ( G->action == Informations_table )
    {
      if ( G->informations_table->dimension )
	EXIT ;
      G->informations_table->dimension = 2 ;
      G->informations_table->taille[0] = hau ;
      G->informations_table->taille[1] = lar ;
      G->informations_table->type = codec_informations(tp) ;
      G->informations_table->table = a ;
      G->informations_table->positionne_element = positionne_element_HL ;
      return(0) ;
    }

  err = 0 ;
  taille = codec_informations(tp)->taille*lar ;

  for(i=0 ; i<hau ; i++)
    {
      if ( G->action==Sauve_texte && *G->entree == '\n' )
        {
          sauve = G->entree ;
          while( i++<hau )
            {
              codec_ouvre_parenthese() ;
              codec_l(a, lar, tp) ;
              codec_ferme_parenthese() ;
	      a = (char*)a + taille ;
              G->entree = sauve ;
            }
          return(1) ;
        }
      codec_ouvre_parenthese() ;
      err |= codec_l(a, lar, tp) ;
      codec_ferme_parenthese() ;
      a = (char*)a + taille ;
    }
  codec_linefeed() ;
  return(err) ;
}
/*
 *
 */
int positionne_element_PHL(struct informations_table *it)
{
  if ( it->table )
    it->element_courant = (char*)it->table
      + it->type->taille * (
			    it->courant[2]
			    + it->taille[2] * (
					       it->courant[1]
					       + it->taille[1] * (
								  it->courant[0]
								  )
					       )
			    )
      ;
  return(1) ;
}

int codec_PHL(void **table, int *pro, int *hau, int *lar, Codec tp)
{
  int i, j ;
  int taille ;
  int lart ;
  int oldmax, newmax ;
  int err ;

  codec_ouvre_accolade() ;
  taille = codec_taille(tp) ;
  lart = *hau * *lar * taille ;
  err = 0 ;

  switch( G->action )
    {
    case Informations :
      fprintf(stderr,"PHL\n") ;
      abort() ;

    case Recupere_float :
    case Recupere_texte :
      for(i=0 ; i<*pro ; i++)
        {
          codec_ouvre_crochet() ;
          err |= codec_hl((char*)*table + i * lart, *hau, *lar, tp) ;
          codec_ferme_crochet() ;
        }
      if ( G->action == Recupere_texte )
	PUTCHAR('\n') ;
      break ;

    case Libere :
      for(i=0 ; i<*pro ; i++)
        {
          err |= codec_hl((char*)*table + i * lart, *hau, *lar, tp) ;
        }
      CODEC_FREE(*table) ;
      *lar = 0 ;
      *hau = 0 ;
      *pro = 0 ;
      break ;

    case Sauve_texte :
      oldmax = *pro * *hau * *lar * taille ;
      if ( oldmax && *table==NULL )
	{
	  fprintf(stderr, "Vous avez mis un tableau (codec_PHL) à NULL sans mettre\n"
		  "à 0 le nombre d'élément, ce que je fais\n") ;
	  oldmax = 0 ;
	}
      lart = 0 ;
      for(i=0, j=0;
          *G->entree != '}' && *G->entree != '\n' && *G->entree != '\0' ;
          i++, j+=lart
          )
        {
          if ( i==0 )
            {
              G->economise_memoire++ ;
              err |= codec_HL(table, hau, lar, tp) ;
              G->economise_memoire-- ;
              lart = *lar * *hau * taille ;
	      if ( oldmax < lart )
		oldmax = lart ;
            }
          else
            {
              while ( j + lart > oldmax )
                {
                  newmax = INCREMENTE(oldmax) ;
                  CODEC_REALLOC(*(char**)table, newmax) ;
                  memset((char*)*table + oldmax, '\0', newmax - oldmax ) ;
		  oldmax = newmax ;
                }
              codec_ouvre_crochet() ;
              err |= codec_hl((char*)*table + j, *hau, *lar, tp) ;
              codec_ferme_crochet() ;
            }
        }
      *pro = i ;
      if ( G->economise_memoire == 0 )
        CODEC_REALLOC(*(char**)table, *pro * lart) ;
      break ;
      
    case Informations_table :
      if ( G->informations_table->dimension )
	EXIT ;
      G->informations_table->dimension = 3 ;
      G->informations_table->taille[0] = *pro ;
      G->informations_table->taille[1] = *hau ;
      G->informations_table->taille[2] = *lar ;
      G->informations_table->type = codec_informations(tp) ;
      G->informations_table->table = *table ;
      G->informations_table->positionne_element = positionne_element_PHL ;
      break ;      
    }
  codec_ferme_accolade() ;
  return(err) ;
}

/*
 * Premier indice : No Ligne, maximum : hau
 */
int codec_HH(void **table, int *hau, Codec tp)
{
  int i, j ;
  int taille ;
  int lart ;
  int oldmax, newmax ;
  int err ;
  const char *tmp ;

  codec_ouvre_crochet() ;
  taille = codec_taille(tp) ;
  err = 0 ;

  switch( G->action )
    {
    case Informations :
      fprintf(stderr,"codec_HH\n") ;
      abort() ;

    case Recupere_float :
    case Recupere_texte :
      for(i=0 ; i<*hau ; i++)
        {
          codec_ouvre_parenthese() ;
          err |= codec_l((char*)*table + i * *hau * taille, *hau, tp) ;
          codec_ferme_parenthese() ;
        }
      if ( G->action == Recupere_texte )
	PUTCHAR('\n') ;
      break ;

    case Libere :
      for(i=0 ; i<*hau ; i++)
        {
          err |= codec_l((char*)*table + i * *hau * taille, *hau, tp) ;
        }
      CODEC_FREE(*table) ;
      *hau = 0 ;
      break ;

    case Sauve_texte :
      lart = 0 ;
      for(i=0, j=0;
          *G->entree != ']' && *G->entree != '\n' && *G->entree != '\0' ;
          j+=lart
          )
        {
          if ( i==0 )
            {
	      oldmax = *hau * *hau * taille ;
	      if ( oldmax && *table==NULL )
		{
		  fprintf(stderr, "Vous avez mis un tableau (codec_HH) à NULL sans mettre\n"
			  "à 0 le nombre d'élément, ce que je fais\n") ;
		  oldmax = 0 ;
		}
              G->economise_memoire++ ;
              err |= codec_L(table, hau, tp) ;
              G->economise_memoire-- ;
	      newmax = *hau * *hau * taille ;
              lart = *hau * taille ;
	      if ( oldmax < lart )
		oldmax = lart ;
	      if ( newmax > oldmax )
                {
                  CODEC_REALLOC(*(char**)table, newmax) ;
                  memset((char*)*table + oldmax, '\0', newmax - oldmax ) ;
                }
            }
          else
            {
              codec_ouvre_parenthese() ;
              err |= codec_l((char*)*table + j, *hau, tp) ;
              codec_ferme_parenthese() ;
            }
	  i++ ;
	  if ( i >= *hau )
	    break ;
        }
      if ( i != *hau )
	{
	  /*
	   * Lecture de champs vides
	   */
	  while( i<*hau )
	    {
              tmp = G->entree ;
              codec_ouvre_parenthese() ;
              codec_l((char*)*table + j, *hau, tp) ;
              codec_ferme_parenthese() ;
	      G->entree = tmp ;
	      i++ ;
	      j += lart ;
	    }
	  err = 1 ;
	}
      if ( G->economise_memoire==0 )
          CODEC_REALLOC(*(char**)table, *hau * lart) ;
      break ;

    case Informations_table :
      if ( G->informations_table->dimension )
	EXIT ;
      G->informations_table->dimension = 2 ;
      G->informations_table->taille[0] = *hau ;
      G->informations_table->taille[1] = *hau ;
      G->informations_table->type = codec_informations(tp) ;
      G->informations_table->table = *table ;
      G->informations_table->positionne_element = positionne_element_HL ;
      break ;      
    }
  codec_ferme_crochet() ;
  return(err) ;
}

/*
 * Par exemple, pour
 * struct
 *   {
 *      int lar ;
 *      float *a[2] ; // ou :   float *a, *b ;
 *   } S ;
 * Ou "a" et "b" contiennent le meme nombre d'élément.
 * codec_h_L(S.a, 2, &S.lar, codec_Float) ;
 */
int codec_h_L(void **table, int hau, int *lar, Codec tp)
{
  int i ;
  int taille ;
  int err ;
  const char *tmp ;

  codec_ouvre_crochet() ;
  taille = codec_taille(tp) ;
  err = 0 ;

  switch( G->action )
    {
    case Informations :
      fprintf(stderr,"codec_h_L\n") ;
      abort() ;

    case Recupere_float :
    case Recupere_texte :
      for(i=0 ; i<hau ; i++)
        {
          codec_ouvre_parenthese() ;
          err |= codec_l(table[i], *lar, tp) ;
          codec_ferme_parenthese() ;
        }
      break ;

    case Libere :
      for(i=0 ; i<hau ; i++)
        {
          err |= codec_l(table[i], *lar, tp) ;
	  CODEC_FREE(table[i]) ;
        }
      *lar = 0 ;
      break ;

    case Sauve_texte :
      for(i=0 ;
	  i<hau
	    && *G->entree!=']' && *G->entree!='\n' && *G->entree!='\0' ;
	  i++)
        {
          if ( i==0 )
            {
              err |= codec_L(&table[0], lar, tp) ;
            }
          else
            {
              CODEC_REALLOC(((char**)table)[i], taille * *lar) ;
              codec_ouvre_parenthese() ;
              err |= codec_l(table[i], *lar, tp) ;
              codec_ferme_parenthese() ;
            }
        }
      if ( i != hau )
	{
	  /*
	   * Lecture de champs vides
	   */
	  while( i<hau )
	    {
              tmp = G->entree ;
              codec_ouvre_parenthese() ;
              CODEC_REALLOC(((char**)table)[i], taille * *lar) ;
              codec_l(table[i], *lar, tp) ;
              codec_ferme_parenthese() ;
	      G->entree = tmp ;
	      i++ ;
	    }
	  err = 1 ;
	}
      break ;

    case Informations_table :
      if ( G->informations_table->dimension )
	EXIT ;
      G->informations_table->dimension = 2 ;
      G->informations_table->taille[0] = hau ;
      G->informations_table->taille[1] = *lar ;
      G->informations_table->type = codec_informations(tp) ;
      G->informations_table->table = table ;
      G->informations_table->positionne_element = positionne_element_H_L ;
      break ;      
    }
  codec_ferme_crochet() ;
  return(err) ;
}
/*
 * Comme codec_h_L mais transposé à la lecture/écriture.
 * Par exemple pour lire XYZ XYZ XYZ ... et stocker en mémoire XX... YY... ZZ..
 * (hau, lar) est la taille du tableau en mémoire et non comme il est affiché
 */

int codec_L_h(void **table, int hau, int *lar, Codec tp)
{
  int i, j ;
  int taille ;
  int max ;
  int err ;

  codec_ouvre_parenthese() ;
  taille = codec_taille(tp) ;
  err = 0 ;

  switch( G->action )
    {
    case Informations :
      fprintf(stderr,"codec_L_h\n") ;
      abort() ;

    case Recupere_float :
    case Recupere_texte :
      for(i=0 ; i<*lar ; i++)
        {
	  for(j=0;j<hau;j++)
	    err |= (*tp)((char*)table[j] + i*taille) ;
	  codec_linefeed() ;
        }
      break ;

    case Libere :
      for(i=0 ; i<hau ; i++)
        {
          err |= codec_l(table[i], *lar, tp) ;
	  CODEC_FREE(table[i]) ;
        }
      *lar = 0 ;
      break ;

    case Sauve_texte :
      max = *lar ;
      if ( max && *table==NULL )
	{
	  fprintf(stderr, "Vous avez mis un tableau (codec_L_h) à NULL sans mettre\n"
		  "à 0 le nombre d'élément, ce que je fais\n") ;
	  max = 0 ;
	}
      for(i=0;
          *G->entree != ')' && *G->entree != '\n' && *G->entree != '\0' ;
          i++
          )
        {
          if ( i >= max )
            {
              max = INCREMENTE(max) ;
	      for(j=0; j<hau; j++)
		{
		  CODEC_REALLOC(((char**)table)[j], max*taille) ;
		  memset((char*)table[j] + i*taille, '\0', (max-i)*taille) ;
		}
            }
	  for(j=0; j<hau; j++)
	    {
	      err |= (*tp)((char*)table[j] + i*taille) ;
	    }
	  codec_linefeed() ;
	}
      *lar = i ;
      if ( G->economise_memoire==0 )
	{
	  for(j=0; j<hau; j++)
	    CODEC_REALLOC(((char**)table)[j], *lar*taille) ;
	}
      break ;

    case Informations_table :
      if ( G->informations_table->dimension )
	EXIT ;
      G->informations_table->dimension = 2 ;
      G->informations_table->taille[0] = hau ;
      G->informations_table->taille[1] = *lar ;
      G->informations_table->type = codec_informations(tp) ;
      G->informations_table->table = table ;
      G->informations_table->positionne_element = positionne_element_H_L ;
      break ;      
    }
  codec_ferme_parenthese() ;
  return(err) ;
}
/*
 * codec_L_h mais avec deux tableaux séparés
 */
int positionne_element_L_hh(struct informations_table *it)
{
  if ( it->table && it->table2 )
    {
      it->element_courant = (char*)it->table + it->courant[0] * it->type->taille ;
      it->element_courant2= (char*)it->table2+ it->courant[0] * it->type2->taille ;
    }
  return(1) ;
}

int codec_L_hh(void **table1, Codec tp1, void **table2, Codec tp2, int *lar)
{
  int i ;
  int taille1, taille2 ;
  int max ;
  int err ;

  codec_ouvre_parenthese() ;
  taille1 = codec_taille(tp1) ;
  taille2 = codec_taille(tp2) ;
  err = 0 ;

  switch( G->action )
    {
    case Informations :
      fprintf(stderr,"codec_L_hh\n") ;
      abort() ;

    case Recupere_float :
    case Recupere_texte :
      for(i=0 ; i<*lar ; i++)
        {
	  err |= (*tp1)((char*)*table1 + i*taille1) ;
	  err |= (*tp2)((char*)*table2 + i*taille2) ;
	  /* 2/5/2000 codec_linefeed_recupere_texte() ; */
        }
      break ;

    case Libere :
      err |= codec_l(*table1, *lar, tp1) ;
      CODEC_FREE(*table1) ;
      err |= codec_l(*table2, *lar, tp2) ;
      CODEC_FREE(*table2) ;
      *lar = 0 ;
      break ;

    case Sauve_texte :
      max = *lar ;
      if ( max && (*table1==NULL||*table2==NULL) )
	{
	  fprintf(stderr, "Vous avez mis un tableau (codec_L_hh) à NULL sans mettre\n"
		  "à 0 le nombre d'élément, ce que je fais\n") ;
	  max = 0 ;
	}
      for(i=0;
          *G->entree != ')' && *G->entree != '\n' && *G->entree != '\0' ;
          i++
          )
        {
          if ( i >= max )
            {
              max = INCREMENTE(max) ;
	      CODEC_REALLOC(*((char**)table1), max*taille1) ;
	      memset((char*)*table1 + i*taille1, '\0', (max-i)*taille1) ;
	      CODEC_REALLOC(*((char**)table2), max*taille2) ;
	      memset((char*)*table2 + i*taille2, '\0', (max-i)*taille2) ;
            }
	  err |= (*tp1)((char*)*table1 + i*taille1) ;
	  err |= (*tp2)((char*)*table2 + i*taille2) ;
	  if ( err )
	    break ;
	}
      *lar = i ;
      if ( G->economise_memoire==0 )
	{
	  CODEC_REALLOC(*((char**)table1), *lar*taille1) ;
	  CODEC_REALLOC(*((char**)table2), *lar*taille2) ;
	}
      break ;

    case Informations_table :
      if ( G->informations_table->dimension )
	EXIT ;
      G->informations_table->dimension = 1 ;
      G->informations_table->taille[0] = *lar ;
      G->informations_table->type = codec_informations(tp1) ;
      G->informations_table->type2 = codec_informations(tp2) ;
      G->informations_table->table = *table1 ;
      G->informations_table->table2 = *table2 ;
      G->informations_table->positionne_element = positionne_element_L_hh ;
      break ;      
    }
  codec_ferme_parenthese() ;
  return(err) ;
}
/*
 *
 */
int codec_l_hh(void *table1, Codec tp1, void *table2, Codec tp2, int lar)
{
  int i ;
  int taille1, taille2 ;
  int err ;
  const char *sauve ;

  taille1 = codec_taille(tp1) ;
  taille2 = codec_taille(tp2) ;
  err = 0 ;

  switch( G->action )
    {
    case Informations :
      fprintf(stderr,"codec_l_hh\n") ;
      abort() ;

    case Recupere_float :
    case Recupere_texte :
      for(i=0 ; i<lar ; i++)
        {
	  err |= (*tp1)((char*)table1 + i*taille1) ;
	  err |= (*tp2)((char*)table2 + i*taille2) ;
	  /* 2/5/2000	  codec_linefeed_recupere_texte() ; */
        }
      break ;

    case Libere :
      err |= codec_l(table1, lar, tp1) ;
      err |= codec_l(table2, lar, tp2) ;
      break ;

    case Sauve_texte :
      for(i=0 ; i<lar ; i++)
        {
          if ( *G->entree == '\n' )
            {
              sauve = G->entree ;
              while( i++<lar )
                {
                  (*tp1)(table1) ;
		  table1 = (char*)table1 + taille1 ;
                  (*tp2)(table2) ;
		  table2 = (char*)table2 + taille2 ;
                  G->entree = sauve ;
                }
              return(1) ;
            }
          err |= (*tp1)(table1) ;
	  table1 = (char*)table1 + taille1 ;
          err |= (*tp2)(table2) ;
	  table2 = (char*)table2 + taille2 ;
	  if ( err )
	    break ;
        }
      if ( G->entree[-1]=='\n' ) 
        codec_linefeed_sauve_texte() ;
      break ;

    case Informations_table :
      if ( G->informations_table->dimension )
	EXIT ;
      G->informations_table->dimension = 1 ;
      G->informations_table->taille[0] = lar ;
      G->informations_table->type = codec_informations(tp1) ;
      G->informations_table->type2 = codec_informations(tp2) ;
      G->informations_table->table = table1 ;
      G->informations_table->table2 = table2 ;
      G->informations_table->positionne_element = positionne_element_L_hh ;
      break ;      
    }
  return(err) ;
}
/*
 * Fusion de Deux matrices
 * Utilisation possible :
 *    int hau, lar ;
 *    Triplet **points ;
 *    float **poids ;
 */
int positionne_element_H_L_pp(struct informations_table *it)
{
  if ( it->table && ((char**)it->table)[it->courant[0]]
       && it->table2 && ((char**)it->table2)[it->courant[0]] )
    {
      it->element_courant = ((char**)it->table)[it->courant[0]]
	+ it->courant[1] * it->type->taille ;
      it->element_courant2 = ((char**)it->table2)[it->courant[0]]
	+ it->courant[1] * it->type2->taille ;
    }
  return(1) ;
}

int codec_H_L_pp(void ***table1, Codec tp1,
		 void ***table2, Codec tp2,
		 int *hau, int *lar)
{
  int i ;
  int taille1, taille2 ;
  int max ;
  int err ;

  codec_ouvre_crochet() ;

  taille1 = codec_taille(tp1) ;
  taille2 = codec_taille(tp2) ;

  err = 0 ;

  switch( G->action )
    {
    case Informations :
      fprintf(stderr,"codec_H_L_pp\n") ;
      abort() ;

    case Recupere_float :
    case Recupere_texte :
      for(i=0 ; i<*hau ; i++)
        {
          codec_ouvre_parenthese() ;
	  err |= codec_l_hh((*table1)[i], tp1, (*table2)[i], tp2, *lar) ;
          codec_ferme_parenthese() ;
        }
      break ;

    case Libere :
      for(i=0 ; i<*hau ; i++)
        {
	  err |= codec_l_hh((*table1)[i], tp1, (*table2)[i], tp2, *lar) ;
          CODEC_FREE((*table1)[i]) ;
          CODEC_FREE((*table2)[i]) ;
        }
      CODEC_FREE(*table1) ;
      CODEC_FREE(*table2) ;
      *hau = 0 ;
      *lar = 0 ;
      break ;

    case Sauve_texte :
      max = *hau ;
      if ( max && (*table1==NULL||*table2==NULL) )
	{
	  fprintf(stderr, "Vous avez mis un tableau (codec_H_L_pp) à NULL sans mettre\n"
		  "à 0 le nombre d'élément, ce que je fais\n") ;
	  max = 0 ;
	}
      for(i=0 ; *G->entree!=']' && *G->entree!='\n' && *G->entree!='\0' ; i++)
        {
          if ( i >= max )
            {
              max = INCREMENTE(max) ;
              CODEC_REALLOC(*table1, max) ;
              CODEC_REALLOC(*table2, max) ;
              memset(&(*table1)[i], '\0', (max-i)*sizeof(void*)) ;
              memset(&(*table2)[i], '\0', (max-i)*sizeof(void*)) ;
            }
          if ( i==0 )
            {
	      if ( *lar && (*table1)[0]==NULL )
		CODEC_MALLOC((*(char***)table1)[0], taille1 * *lar) ;
	      if ( *lar && (*table2)[0]==NULL )
		CODEC_MALLOC((*(char***)table2)[0], taille2 * *lar) ;

              err |= codec_L_hh(&(*table1)[0], tp1,
				&(*table2)[0], tp2,
				lar) ;
            }
          else
            {
              CODEC_REALLOC((*(char***)table1)[i], taille1 * *lar) ;
              CODEC_REALLOC((*(char***)table2)[i], taille2 * *lar) ;
              codec_ouvre_parenthese() ;
              err |= codec_l_hh((*table1)[i], tp1,
				(*table2)[i], tp2,
				*lar) ;
              codec_ferme_parenthese() ;
            }
        }
      *hau = i ;
      CODEC_REALLOC(*table1, *hau) ;
      CODEC_REALLOC(*table2, *hau) ;
      break ;

    case Informations_table :
      if ( G->informations_table->dimension )
	EXIT ;
      G->informations_table->dimension = 2 ;
      G->informations_table->taille[0] = *hau ;
      G->informations_table->taille[1] = *lar ;
      G->informations_table->type = codec_informations(tp1) ;
      G->informations_table->type2 = codec_informations(tp2) ;
      G->informations_table->table = *table1 ;
      G->informations_table->table2 = *table2 ;
      G->informations_table->positionne_element = positionne_element_H_L_pp ;
      break ;      
    }
  codec_ferme_crochet() ;
  return(err) ;
}

/*
 *
 */
int codec_enum_general(int *a, int nb, char **noms, int *indice, int case_sensitive)
{
  int i, colonne, numero_ok ;
  int err ;

  err = 0 ;

  switch( G->action )
    {
    case Informations :
      fprintf(stderr,"codec_enum\n") ;
      abort() ;

    case Recupere_float :
      PUTFLOAT(*a) ;
      break ;

    case Recupere_texte :
      if ( *a >=0 && *a < nb )
	putstr(noms[*a]) ;
      else
	err = 1 ;
      break ;

    case Informations_table :
    case Libere :
      break ;

    case Sauve_texte :
      SAUTE_BLANC ;
      for(colonne=0;;colonne++)
	{
	  numero_ok = -1 ;
	  if ( case_sensitive )
	    {
	      for(i=0;i<nb;i++)
		if ( strncmp(G->entree-colonne, noms[i], colonne+1) == 0 )
		  {
		    if ( numero_ok == -1 )
		      numero_ok = i ;
		    else
		      break ; /* au moins 2 bons */
		  }
	    }
	  else
	    for(i=0;i<nb;i++)
	      if ( strncasecmp(G->entree-colonne, noms[i], colonne+1) == 0 )
		{
		  if ( numero_ok == -1 )
		    numero_ok = i ;
		  else
		    break ; /* au moins 2 bons */
		}
	  if ( i == nb )
	    {
	      if ( numero_ok == -1 )
		{
		  for(i=0;i<nb;i++)
		    if ( noms[i][0] == '\0' ) /* FAUX a corriger */
		      {
			numero_ok = i ;
			break ;
		      }
		  if ( i == nb )
		    {
		      err = 1 ;
		      break ;
		    }
		}
	      if ( indice )
		*a = indice[numero_ok] ;
	      else
		*a = numero_ok ;
	      if ( noms[numero_ok][colonne+1] == '\0' )
		{
		  G->entree++ ;
		  break ;
		}
	    }
	  G->entree++ ;
	  if ( *G->entree == '\0' )
	    {
	      err = 1 ;
	      break ;
	    }
	}
    }
  return(err) ;
}

int codec_enum(int *a, int nb, char **noms)
{
  return(codec_enum_general(a, nb, noms, NULL, 1)) ;
}


int codec_union(void *a, int *num, int nb, Codec type, Codec *tp, int parentheses)
{
  int err ;

  if ( (*type)(num) )
    return(1) ;
  if ( *num<0 || *num>=nb )
    return(1) ;

  if ( parentheses )
    codec_ouvre_parenthese() ;
  err = (tp[*num])(a) ;
  if ( parentheses )
    codec_ferme_parenthese() ;

  return(err) ;
}
int codec_pointeur(void **a, Codec tp)
{
  int taille ;
  int err ;

  err = 0 ;

  switch( G->action )
    {
    case Informations :
      fprintf(stderr,"codec_pointeur\n") ;
      abort() ;

    case Recupere_texte :
    case Recupere_float :
      codec_ouvre_parenthese() ;
      if (*a)
	err = (*tp)(*a) ;
      codec_ferme_parenthese() ;
      break ;

    case Libere :
      if (*a)
	{
	  err = (*tp)(*a) ;
	  CODEC_FREE(*a) ;
	}
      break ;

    case Sauve_texte :
      codec_ouvre_parenthese() ;
      if (*G->entree==')')
	{
	  if (*a)
	    err = codec_libere(tp, a) ;
	}
      else
	{
	  if (*a==NULL)
	    {	  
	      taille = codec_taille(tp) ;
	      CODEC_MALLOC(*(char**)a, taille) ;
	      memset(*a, '\0', taille) ;
	    }
	  err = (*tp)(*a) ;
	}
      codec_ferme_parenthese() ;
      break ;

    case Informations_table :
      break ;
    }
  return(err) ;
}
int codec_pointeur_non_null(void **a, Codec tp)
{
  int taille ;
  int err ;

  err = 0 ;
  switch( G->action )
    {
    case Informations :
      fprintf(stderr,"codec_pointeur_non_null\n") ;
      abort() ;

    case Recupere_float :
    case Recupere_texte :
      if (*a)
	err = (*tp)(*a) ;
      else
	err = 1 ;
      break ;

    case Libere :
      if (*a)
	{
	  err = (*tp)(*a) ;
	  CODEC_FREE(*a) ;
	}
      else
	err = 1 ;
      break ;

    case Sauve_texte :
      if (*a==NULL)
	{	  
	  taille = codec_taille(tp) ;
	  CODEC_MALLOC(*(char**)a, taille) ;
	  memset(*a, '\0', taille) ;
	}
      err = (*tp)(*a) ;
      break ;

    case Informations_table :
      break ;
    }
  return(err) ;
}

