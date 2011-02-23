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
/**************************/
/* VISUALISATION AVEC POV */
/**************************/

/* distance de resolution minimum dans l'espace modelisation R^3 */

#define DMIN 0.1 

/* adresse du directoire de calcul Pov */

#define POV conca(MOD_FRAC,mot(4,"POV/"))

/*--------------------------*/
/* Operations informatiques */
/*--------------------------*/

FILE *deb_pov(MOT iprod);
void fin_pov(FILE *f, CAMERA cam);
void destru_pov(MOT iprod);
void tga_pov(MOT iprod, CAMERA cam);

/*------------*/
/* Parametres */  
/*------------*/

void pov_pt(FILE *f, POINT p);
void pov_math(FILE *f, MATH r);
void pov_couleur(FILE *f, RVB coul);

/*----------------------*/
/* Figures elementaires */
/*----------------------*/

void pov_lampe(FILE *f, POINT p, RVB coul);
void pov_sphere(FILE *f, POINT p, Flottant r);
void pov_ptm(FILE *f, POINT p);
void pov_ptm_li(FILE *f, MATH lp, L_INT je);
void pov_ptm_math(FILE *f, MATH lp);

void pov_triangle(FILE *f, POINT p0, POINT p1, POINT p2);
void pov_triangle_li(FILE *f, MATH lp, L_INT je);
void pov_quadrilatere_li(FILE *f, MATH lp, L_INT je);

/*-----------------*/ 
/* Figures iterees */ 
/*-----------------*/ 

void pov_figv0(FILE *f, FIGIT fig);
void pov_pc(FILE *f, FIGIT fig);
void pov_determ(FILE *f, FIGIT fig);
void pov_determ_iter(FILE *f, FIGIT fig);

/* pour gl */
void gl_determ_iter(FIGIT fig);
