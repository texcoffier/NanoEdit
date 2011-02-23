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
#ifndef U_LISTE_H
#define U_LISTE_H

#include "u_base.h"		/* Pour CONCATENE */
#include "codec_base.h"

typedef struct l_liste
{
  struct l_liste *suivant ;
  void *objet ;
} Liste ;

int           l_enleve      (	  Liste **, void *objet) ;
void         *l_enleve_tete (	  Liste **) ;
void         *l_enleve_queue(	  Liste **) ;
void          l_ajoute      (	  Liste **, void *objet) ;
Booleen l_ajoute_si_pas_deja(	  Liste **, void *objet) ;
void          l_ajoute_fin  (	  Liste **, void *objet) ;
Liste*        l_duplique    (const Liste *) ;
Liste*        l_inverse     (const Liste *) ;
void          l_echange     (     Liste *liste, void *a, void *b) ;
void          l_remplace    (     Liste *liste, void *a, void *b) ;
Liste**       l_queue       (	  Liste **) ;
void          l_detruit     (	  Liste **) ;
int           l_longueur    (const Liste *) ;
void*         l_element     (const Liste *, int num) ;
Booleen       l_trouve      (const Liste *, const void *objet) ;
int           l_trouve_indice(const Liste *, const void *objet) ;
void          l_sort        (      Liste *, int (*)(const void*,const void*));
void          l_int_sort    (      Liste *);
void          l_reverse_int_sort(  Liste *);
void          l_relier      (      Liste**, Liste *) ;
void **       l_liste_vers_table(const Liste *l, int *nb) ;
void          l_insere      (      Liste **, void *objet, int position);
void          l_enleve_indice(     Liste **, int position);


#define l_vide(L) (L==NULL)

#ifdef DEBUG_CODEC
#define XXXX(A) if(!MEMOIRE_VALIDE(globale_gm_codec, A)) abort() ;
#else
#define XXXX(A)
#endif

#define POUR(TYPE, INDICE, LISTE, CORPS)			\
{								\
const Liste *CONCATENE(l__,INDICE) ;				\
const Liste *CONCATENE(n__,INDICE) ;				\
int CONCATENE(i_,INDICE) ;					\
TYPE INDICE ;							\
								\
CONCATENE(i_,INDICE) = 0 ;					\
for( CONCATENE(l__,INDICE) = LISTE ;				\
     CONCATENE(l__,INDICE) ;					\
     CONCATENE(l__,INDICE) = CONCATENE(n__,INDICE),		\
        CONCATENE(i_,INDICE)++					\
      )								\
{								\
 XXXX(CONCATENE(l__,INDICE)) ; \
 CONCATENE(n__,INDICE) = CONCATENE(l__,INDICE) -> suivant ;	\
 INDICE = (TYPE) CONCATENE(l__,INDICE) -> objet ;		\
 CORPS ;							\
}								\
}

#endif
