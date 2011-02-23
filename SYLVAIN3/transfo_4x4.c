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
#include <string.h>
#include <math.h>
#include "transfo.h"

void transfo_4x4_libere(double **t)
{
  free(t);
}
double ** transfo_4x4_alloue()
{
  double **t, *p ;
  int i ;

  t = (double **)malloc(4*sizeof(double *) + 4*4*sizeof(double));
  p = (double*) &t[4] ; /* Début des données */
  for(i=0;i<4;i++)
    t[i] = &p[4*i] ;

  return(t) ;
}
double** transfo_4x4_create(const Transfo *tt)
{
  double **tr ;
  int i, j ;
  const double *t = &tt->v1.x ;

  tr = transfo_4x4_alloue() ;
  for(i=0;i<4;i++)
    for(j=0;j<3;j++)
      tr[j][i] = *t++ ;
  
  for(i=0;i<3;i++)
    tr[3][i] = 0. ;
  tr[3][3] = 1. ;

  return(tr) ;
}
int transfo_4x4_extrait(double **tr, Transfo *tt, int n)
{
  int i, j ;
  double *t = &tt->v1.x ;

  if ( n!=0 )
    return(0) ;

  for(i=0;i<4;i++)
    for(j=0;j<3;j++)
      *t++ = tr[j][i] ;

  return(1) ;
}
void transfo_4x4_ajoute(double **t, const Transfo *u)
{
  fprintf(stderr, "transfo_4x4_ajoute\n") ;
  exit(1) ;
}
int transfo_4x4_origine(double **t, Triplet *o, int n)
{
  int i ;

  if ( n!=0 )
    return(0) ;

  for(i=0; i<3; i++)
    (&o->x)[i] = t[i][3] ;
  return(1) ;
}
double ** transfo_4x4_duplique(double **t)
{
  int i ;
  double **nt;
  
  nt = transfo_4x4_alloue() ;
  
  for(i=0;i<4;i++)
    memcpy( nt[i], t[i], 4*sizeof(t[i][0]) ) ;
  
  return(nt) ;
}
double **transfo_4x4_identite()
{
  double **t;
  int i,j;
  
  t = transfo_4x4_alloue() ;
  /* tout d'abord on initialise la matrice */
  for(i=0;i<4;i++)
    {
      for(j=0;j<4;j++)
	{
	  if (i==j)
	    t[i][j]=1.0;
	  else
	    t[i][j]=0.0;
	}
    }
  return t;
}
double **transfo_4x4_multiplie(double **tm1, double **tm2)
{
  double **tm;
  int i,j,k;
  double acc;
		
  tm=transfo_4x4_alloue();
  for(i=0;i<4;i++)
    {
      for(j=0;j<4;j++)
	{
	  acc=0;
	  for(k=0;k<4;k++)
	    {
	      acc+=tm1[i][k] * tm2[k][j];
	    }
	  tm[i][j]=acc;
	}
    }
  return tm;
}

void transfo_4x4_utilise()
{
  transfo_alloue    = (Transformation* (*)())
    transfo_4x4_alloue ;
  transfo_create    = (Transformation* (*)(const Transfo*))
    transfo_4x4_create ;
  transfo_ajoute    = (void (*)(Transformation*,const Transfo*))
    transfo_4x4_ajoute ;
  transfo_extrait   = (int (*)(const Transformation*,Transfo*, int n))
    transfo_4x4_extrait ;
  transfo_identite  = (Transformation* (*)())
    transfo_4x4_identite ;
  transfo_duplique  = (Transformation* (*)(const Transformation*))
    transfo_4x4_duplique ;
  transfo_libere    = (void (*)(Transformation*))
    transfo_4x4_libere ;
  transfo_origine   = (int (*)(const Transformation*,Triplet*, int n))
    transfo_4x4_origine ;
  transfo_multiplie = (Transformation* (*)(const Transformation*
					   ,const Transformation*))
    transfo_4x4_multiplie ;

}

