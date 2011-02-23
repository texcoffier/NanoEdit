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
#ifndef MATRICEDEF
#define MATRICEDEF


#include "liste.h"


/* on definie une matrice  d'element */
typedef struct {
	liste_transfo ***m;	/* une matrice de pointeurs sur des listes de transformations */
	int dim;	/* les matrices sont carres */
} matrice;


/* alloue la memoire pour une nouvelle matrice */
matrice *matrice_nouvelle(int dim);

/* desalloue la memoire de la matrice*/
void matrice_libere(matrice *m);

/* addition de 2 matrice */
matrice *matrice_additionne(matrice *mat1,matrice *mat2);

/* multiplication de 2 matrices */
matrice *matrice_multiplie(matrice *mat1,matrice *mat2);

/* puissance n d'une matrice */
matrice *matrice_puissance(matrice *mat, int n);

/* union de 2 matrices */
matrice *matrice_union(matrice *mat1,matrice *mat2);

/* melange de 2 matrices */
matrice *matrice_melange(matrice *mat1,matrice *mat2);

/* concatenation de 2 matrices */
matrice *matrice_concatenation(matrice *mat1,matrice *mat2);

/* intersection de 2 matrices */
matrice *matrice_intersection(matrice *mat1,matrice *mat2);

/* affiche une matrice */
void matrice_affiche(FILE *f, matrice *mat);

/* affecte une case d'une matrice */
void matrice_affecte(matrice *mat, int ligne, int col, Transformation *x);

/* duplique une matrice */
matrice *matrice_duplique(matrice *mat);

#endif
