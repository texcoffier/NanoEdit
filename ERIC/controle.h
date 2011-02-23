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
/**********************/
/* POINTS DE CONTROLE */  
/**********************/

/* Type physique : m+1-uplets de vecteurs homogenes */

typedef struct {int nb; VECH *pt;} L_VECH;

/* Types logiques :
   * m+1-uplets de points : P = (p_j)_{j=0,..,m}
    - repere de dimension m
    - m-simplexe
    - polygone de controle a m cotes
    - grille de controle a m+1 x n+1 points
    - grille de controle de dimension combinatoire n 
    - produits de grilles
    - structure de controle = liste de points et sommets=numeros
    - centres d'IFS : nb=2 (T0,T1) -> (c0,c1)
   * m+1-uplets point et vecteurs : (p_0,v_1,...,vm)
*/

#define L_POINT L_VECH

/* #define PC MATH (voir regle.c) */

/* Structure de controle */
/*
typedef struct {GRAPHE gr; L_POINT lpt;} STRUC;
*/

/*--------------------------*/
/* Operations informatiques */
/*--------------------------*/
/* representants : vecteurs homogenes */

L_POINT lpt_crea(int m);
void destru_lpt(L_POINT lp);
L_POINT lpt_dupli(L_POINT lp);
void ecrit_lpt(FILE *f, char *nom, L_POINT lp);

/* Operations 0-aires */
L_POINT lpt_math(MATH t);
L_VECH lvh_math_extr(MATH t, L_INT js);
L_POINT lpt_vec_math(MATH t);

/* Operations unaires */
int dim_lpt(L_POINT lp);
int nb_lpt(L_POINT lp);
POINT pt_moyen(L_POINT lp);
POINT pt_extr(L_POINT lp1, int i);
L_POINT lpt_extr(L_POINT lp1, L_INT ind);
MATH math_lpt(L_POINT lp);
MATH math_vec_lpt(L_POINT lp);
void col_math_lvh_li(MATH tr, L_INT jr, L_POINT lp, L_INT js);
MATH math_sem_lpt(SEMIG sem, L_POINT lp);

L_POINT lpt_mul(MATH t, L_POINT lp);

/* Operations binaires */
/*
L_POINT lpt_conca(L_POINT lp1, L_POINT lp2);
L_POINT lpt_pcar(L_POINT lp1, L_POINT lp2);
L_POINT lpt_caf(L_POINT lp0, L_POINT lp1, Flottant r);
L_POINT lpt_tens_op(L_POINT lp0, L_POINT lp1, Flottant r);
*/

/*--------------*/
/* Bibliotheque */
/*--------------*/

/* hypercubes, simplexes */
