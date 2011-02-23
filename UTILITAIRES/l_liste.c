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
#include "l_liste.h"
#include <stdlib.h>


void *l_enleve_tete(Liste **liste)
{
  Liste *suivant ;
  void *objet ;

  if ( *liste==NULL )
    return(NULL) ;
  suivant = (*liste)->suivant ;
  objet   = (*liste)->objet ;
#ifdef DEBUG_CODEC
  libere_zone_memoire(&globale_gm_codec, *liste) ;
#else
  free( *liste ) ;
#endif
  *liste = suivant ;
  return(objet) ;
}

void l_ajoute(Liste **liste, void *objet)
{
  Liste *nl ;

#ifdef DEBUG_CODEC
  if ( *liste && !MEMOIRE_VALIDE(globale_gm_codec, *liste) )
    abort() ;
  nl = alloue_zone_memoire(&globale_gm_codec, sizeof(*nl)) ;
#else
  ALLOUER( nl, 1 ) ;
#endif
  nl->suivant = *liste ;
  nl->objet   = objet ;
  *liste = nl ;
}

void l_ajoute_fin(Liste **liste, void *objet)
{
  if ( *liste == NULL )
    l_ajoute(liste, objet) ;
  else
    {
#ifdef DEBUG_CODEC
      if ( !MEMOIRE_VALIDE(globale_gm_codec, *liste) )
	abort() ;
#endif
      l_ajoute_fin( &(*liste)->suivant, objet) ;
    }
}

int l_enleve(Liste **liste, void *objet)
{
  int nb ;

  if ( *liste )
    {
#ifdef DEBUG_CODEC
      if ( !MEMOIRE_VALIDE(globale_gm_codec, *liste) )
	abort() ;
#endif
      nb = l_enleve(&(*liste)->suivant, objet) ;
      if ( (*liste)->objet == objet )
	{
	  l_enleve_tete(liste) ;
	  nb++ ;
	}
    }
  else
    nb = 0 ;
  return(nb) ;
}

void l_enleve_indice(Liste **liste, int position)
{
  Liste *nl ;

  while(position--)
    liste = &(*liste)->suivant ;

  nl = (*liste)->suivant ;

  free( *liste ) ;
  *liste = nl ;
}

void l_insere(Liste **liste, void *objet, int position)
{
  Liste *nl ;

  while(position--)
    liste = &(*liste)->suivant ;

  ALLOUER( nl, 1 ) ;
  nl->suivant = *liste ;
  nl->objet   = objet ;
  *liste = nl ;
}

Liste* l_duplique(const Liste *liste)
{
  Liste *nl ;

  if ( liste == NULL )
    return( NULL ) ;
#ifdef DEBUG_CODEC
  if ( !MEMOIRE_VALIDE(globale_gm_codec, liste) )
    abort() ;
#endif
  nl = l_duplique( liste->suivant ) ;
  l_ajoute( &nl, liste->objet ) ;
  return(nl) ;
}

Liste* l_inverse(const Liste *liste)
{
  Liste *l ;

  l = NULL ;
  while(liste)
    {
#ifdef DEBUG_CODEC
      if ( !MEMOIRE_VALIDE(globale_gm_codec, liste) )
	abort() ;
#endif
      l_ajoute(&l, liste->objet) ;
      liste = liste->suivant ;
    }
  return(l) ;
}

void l_echange(Liste *liste, void *a, void *b)
{
  if ( liste == NULL )
    return ;
#ifdef DEBUG_CODEC
  if ( !MEMOIRE_VALIDE(globale_gm_codec, liste) )
    abort() ;
#endif
  if ( liste->objet == a )
    liste->objet = b ;
  else
    if ( liste->objet == b )
      liste->objet = a ;

  l_echange(liste->suivant, a, b) ;
}

void l_remplace(Liste *liste, void *a, void *b)
{
  if ( liste == NULL )
    return ;
#ifdef DEBUG_CODEC
  if ( !MEMOIRE_VALIDE(globale_gm_codec, liste) )
    abort() ;
#endif
  if ( liste->objet == a )
    liste->objet = b ;

  l_remplace(liste->suivant, a, b) ;
}


void l_detruit(Liste** liste)
{
  while( *liste )
    l_enleve_tete(liste) ;
}

void l_relier(Liste** liste, Liste *a)
{
  if ( *liste == NULL )
    {
      *liste = a ;
      return ;
    }
#ifdef DEBUG_CODEC
  if ( !MEMOIRE_VALIDE(globale_gm_codec, *liste) )
    abort() ;
#endif
  l_relier(&(*liste)->suivant, a) ;
}

int l_longueur(const Liste *liste)
{ 
  if ( liste == NULL ) return( 0 ) ;
#ifdef DEBUG_CODEC
  if ( !MEMOIRE_VALIDE(globale_gm_codec, liste) )
    abort() ;
#endif
 return( l_longueur(liste->suivant) + 1 ) ;
}

void *l_element(const Liste *liste, int num)
{
  if ( liste == NULL ) return(NULL) ;
#ifdef DEBUG_CODEC
  if ( !MEMOIRE_VALIDE(globale_gm_codec, liste) )
    abort() ;
#endif
  if ( num == 0 ) return(liste->objet) ;
  return( l_element( liste->suivant, num-1 ) ) ;
}

Liste **l_queue(Liste **liste)
{
  Liste **l ;
  
  if ( *liste == NULL )
    return(NULL) ;
#ifdef DEBUG_CODEC
  if ( !MEMOIRE_VALIDE(globale_gm_codec, *liste) )
    abort() ;
#endif
  l = liste ;
  while( (*l)->suivant )
    {
#ifdef DEBUG_CODEC
  if ( !MEMOIRE_VALIDE(globale_gm_codec, (*l)->suivant) )
    abort() ;
#endif
      l = &(*l)->suivant ;
    }
  return(l) ;
}

void *l_enleve_queue(Liste **liste)
{
  return( l_enleve_tete(l_queue(liste)) ) ;
}

Booleen l_trouve(const Liste *liste, const void *objet)
{
  if ( liste == NULL ) return(Faux) ;
#ifdef DEBUG_CODEC
  if ( !MEMOIRE_VALIDE(globale_gm_codec, liste) )
    abort() ;
#endif
  if ( liste->objet == objet ) return(Vrai) ;
  return( l_trouve(liste->suivant, objet) ) ;
}

int l_trouve_indice(const Liste *liste, const void *objet)
{
  int i ;
  if ( liste == NULL ) abort() ;
  i = 0 ;
  while( liste->objet != objet )
    {
      liste = liste->suivant ;
      i++ ;
    }
  return i ;
}


Booleen l_ajoute_si_pas_deja(Liste **liste, void *objet)
{
  if ( l_trouve(*liste,objet) ) return(Faux) ;
  l_ajoute(liste,objet) ;
  return(Vrai) ;
}

void **l_liste_vers_table(const Liste *l, int *nb)
{
  void **table, **courant ;

  *nb = l_longueur(l) ;
  ALLOUER(table, *nb ) ;
  courant = table ;
  POUR(void*, o, l, *courant++ = o ; ) ;
  return(table) ;
}


void l_sort(Liste *liste, int (*cmp)(const void*,const void*))
{
  int l ;
  void **t, **u ;

  t = l_liste_vers_table(liste, &l) ;
  qsort(t, l, sizeof(void*), cmp) ;
  u = t ;
  POUR(void*, i, liste, ((Liste*)l__i)->objet = *u++ ; ) ;
  free(t) ;
}

int cmp_int(const void* a,const void* b)
{
return( *(int*)b-*(int*)a ) ;
}

int cmp_reverse_int(const void* a,const void* b)
{
return( *(int*)a-*(int*)b ) ;
}

void l_int_sort(Liste *liste)
{
l_sort(liste, cmp_int) ;
}

void l_reverse_int_sort(Liste *liste)
{
l_sort(liste, cmp_reverse_int) ;
}

