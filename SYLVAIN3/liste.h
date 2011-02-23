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
#ifndef LISTEDEF
#define LISTEDEF

#include "transfo.h"

/* on defini les elements */
typedef struct lt {
	struct lt *suc;	/* Liste chainee de transformations */
	Transformation *mat; /* pointe sur la transformation */
} liste_transfo;

/* libere la memoire d'une liste */
void liste_libere(liste_transfo *lt);

/* ajoute la transformation t au debut de la liste liste et renvoie la liste transformee */
liste_transfo *liste_ajoute(liste_transfo *liste, Transformation *t);

/* duplique l'element */
liste_transfo *liste_duplique(liste_transfo *l);

/* ajoute 2 elements */
/* (a,b)+(c,d) = (a,b,c,d) */
liste_transfo *liste_additionne(liste_transfo *l1, liste_transfo *l2);
void liste_concatene(liste_transfo **e1, liste_transfo *e2) ;

/* multiplie 2 elements */
/* (a,b)*(c,d) = (ac,ad,bc,bd) */
liste_transfo *liste_multiplie(liste_transfo *l1, liste_transfo *l2);

/* affiche la liste des transformation de l'element */
void liste_affiche(FILE *f, liste_transfo *l);

/* Inverse ordre de la liste */
liste_transfo *liste_inverse(liste_transfo *l) ;

#endif
