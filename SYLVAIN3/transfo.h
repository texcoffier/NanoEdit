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
#ifndef TRANSFODEF
#define TRANSFODEF

#include <stdio.h>
#include "u_geometrie.h"

#define NB_MAX_TRANSFO 4

typedef void *Transformation ;

#ifndef LISTEDE
#define LISTEDE extern
#endif

LISTEDE void (*transfo_libere)(Transformation*) ;
LISTEDE Transformation* (*transfo_alloue)() ;
LISTEDE Transformation* (*transfo_identite)() ;
LISTEDE Transformation* (*transfo_create)(const Transfo*) ;
LISTEDE Transformation* (*transfo_multiplie)(const Transformation*, const Transformation*);
LISTEDE Transformation* (*transfo_duplique)(const Transformation*) ;
LISTEDE void (*transfo_ajoute)(Transformation*, const Transfo*) ;
LISTEDE int (*transfo_extrait)(const Transformation*, Transfo *tab, int n) ;
LISTEDE int (*transfo_origine)(const Transformation*, Triplet *tab, int n) ;


void transfo_4x4_utilise() ;
void transfo_n_4x4_utilise() ;

double ** transfo_4x4_alloue() ;
double ** transfo_4x4_identite() ;
double ** transfo_4x4_create   (const Transfo *t) ;
double ** transfo_4x4_duplique (double **t) ;
double ** transfo_4x4_multiplie(double **tm1, double **tm2) ;
void      transfo_4x4_libere   (double **t) ;
void      transfo_4x4_ajoute   (double **t, const Transfo *u) ;
int       transfo_4x4_extrait  (double **t, Transfo *u, int n) ;
int       transfo_4x4_origine  (double **t, Triplet *o, int n) ;

/*
 * fonction génériques
 */

void transfo_affiche_pov(Transformation *t, FILE *f) ;
void transfo_affiche_gl(Transformation *t, FILE *f) ;
void transfo_affiche(Transformation *t, FILE *f) ;

#endif
