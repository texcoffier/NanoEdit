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
/***************/
/* IFS INDICES */ 
/***************/

/* Type physique : N-uplet de matrices homogenes */
/* IFS : (T_i)_{i=0,..,nb-1} 
   nb : nombre de maths,
   tf : tableau des transformations;
   */

typedef struct {int nb; MATH *tf;} L_MATH;

/* Types logiques :
   - IFS si les matrices sont carrees, de meme dimension et contractantes
   - groupe fini si les matrices sont inversibles et de meme dimension
*/

#define IFS L_MATH

/* Operations informatiques */
IFS ifs_crea(int n);
void destru_ifs(IFS ifs);
IFS ifs_dupli(IFS ifs);
void ecrit_ifs(FILE *f, char *nomifs, IFS ifs);

/* Operations 0-aires */
IFS ifs_math1(MATH t0);
IFS ifs_math2(MATH t0, MATH t1);
IFS ifs_math3(MATH t0, MATH t1, MATH t2);
IFS ifs_math4(MATH t0, MATH t1, MATH t2, MATH t3);
IFS ifs_math5(MATH t0, MATH t1, MATH t2, MATH t3, MATH t4);
/*
IFS ifs_centre(L_POINT lp);
*/

/* Operations unaires */
t_espace esp_ifs(IFS ifs1);
int dim_ifs(IFS ifs1);
int nb_ifs(IFS ifs1);
L_POINT centre_ifs(IFS ifs);
IFS ifs_ajout(IFS ifs1, MATH t);
IFS ifs_enlev(IFS ifs1, int k);
IFS ifs_extr(IFS ifs1, L_INT ind);
IFS ifs_inverse(IFS ifs1);
IFS ifs_conj(MATH t,IFS ifs1);
IFS ifs_mul_math(IFS ifs1, MATH t2);

/* Operations binaires */
IFS ifs_mel(IFS ifs1, IFS ifs2);
IFS ifs_tens_pcar(IFS ifs1, IFS ifs2);
/*
IFS ifs_tens_ptens(IFS ifs1, IFS ifs2);
IFS ifs_caf(IFS ifs1, IFS ifs2);
IFS ifs_pcar(IFS ifs1, IFS ifs2);
*/

/*--------------*/
/* Bibliotheque */
/*--------------*/

/* IFS 1D */
IFS intervalle();
IFS cantor();

/* IFS 2D */
IFS dubuc(Flottant x, Flottant y);
IFS dubuch(Flottant x, Flottant y);
IFS vonkoch(Flottant x, Flottant y);
IFS takagi(Flottant x, Flottant y, Flottant r, Flottant s);
IFS takagi_sym(Flottant y, Flottant r);
IFS sierpinski();
IFS carre_un();
IFS arbre_sierpinski();
IFS croix();

/* IFS 3D */
IFS segment_un_ox();
IFS carre_un_oxy();
IFS tetra_sierpinski();
IFS pyramide_sierpinski();
