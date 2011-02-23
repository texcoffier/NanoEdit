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
#include <stdlib.h>
#include <math.h>
#include "transfo.h"

typedef struct tr
{
  int nb ;
  double **t[NB_MAX_TRANSFO] ;
} Tr ;

void transfo_n_4x4_libere(Tr *t)
{
  int i ;

  for(i=0;i<t->nb;i++)
    free(t->t[i]) ;
  free(t);
}
Tr* transfo_n_4x4_alloue()
{
  Tr *t ;
  t = malloc(sizeof(*t)) ;
  t->nb = 0 ;
  return(t) ;
}
void transfo_n_4x4_ajoute(Tr *t, Transfo *v)
{
  if ( t->nb == NB_MAX_TRANSFO )
    {
      fprintf(stderr, "transfo_n_4x4_ajoute\n") ;
      exit(1) ;
    }

  t->t[t->nb++] = transfo_4x4_create(v) ;
}
Tr* transfo_n_4x4_create(Transfo *t)
{
  Tr *tr ;

  tr = transfo_n_4x4_alloue() ;
  transfo_n_4x4_ajoute(tr, t) ;
  return(tr) ;
}
int transfo_n_4x4_extrait(Tr *tr, Transfo *t, int n)
{
  if ( n >= tr->nb || n < 0 )
    return(0) ;

  transfo_4x4_extrait(tr->t[n], t, 0) ;
  return(1) ;
}
int transfo_n_4x4_origine(Tr *t, Triplet *o, int n)
{
  if ( n<0 || n>=t->nb )
    return(0) ;

  transfo_4x4_origine(t->t[n], o, 0) ;
  return(1) ;
}
Tr* transfo_n_4x4_duplique(Tr *t)
{
  int i ;
  Tr *nt;
  Transfo tr ;
  
  nt = transfo_n_4x4_alloue() ;
  for(i=0; i<t->nb; i++)
    {
      transfo_n_4x4_extrait(t, &tr, i) ;
      transfo_n_4x4_ajoute(nt, &tr) ;  
    }
  return(nt) ;
}
Tr *transfo_n_4x4_multiplie(Tr *tm1, Tr *tm2)
{
  int i ;
  Tr *tm ;
  Transfo t ;

  tm = transfo_n_4x4_alloue() ;
		
  for(i=0; i<tm1->nb || i<tm2->nb; i++)
    {
      if ( i < tm1->nb )
	{
	  if ( i < tm2->nb )
	    {
	      tm->t[i] = transfo_4x4_multiplie(tm1->t[i], tm2->t[i]) ;
	      tm->nb++ ;
	    }
	  else
	    {
	      transfo_n_4x4_extrait(tm1, &t, i) ;
	      transfo_n_4x4_ajoute(tm, &t) ;
	    }
	}
      else
	{
	  if ( i < tm2->nb )
	    {
	      transfo_n_4x4_extrait(tm2, &t, i) ;
	      transfo_n_4x4_ajoute(tm, &t) ;
	    }
	}
    }

  return tm;
}

void transfo_n_4x4_utilise()
{
  transfo_alloue    = (Transformation* (*)())
    transfo_n_4x4_alloue ;
  transfo_create    = (Transformation* (*)(const Transfo*))
    transfo_n_4x4_create ;
  transfo_ajoute    = (void (*)(Transformation*, const Transfo*))
    transfo_n_4x4_ajoute ;
  transfo_extrait   = (int (*)(const Transformation*, Transfo*, int n))
    transfo_n_4x4_extrait ;
  transfo_identite  = (Transformation* (*)())
    transfo_n_4x4_alloue ;
  transfo_duplique  = (Transformation* (*)(const Transformation*))
    transfo_n_4x4_duplique ;
  transfo_multiplie = (Transformation* (*)(const Transformation*, const Transformation*))
    transfo_n_4x4_multiplie ;
  transfo_libere    = (void (*)(Transformation*))
    transfo_n_4x4_libere ;

  transfo_origine   = (int (*)(const Transformation*, Triplet*, int n))
    transfo_n_4x4_origine ;
}

