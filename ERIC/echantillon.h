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
/************************************/
/* GENERATION DE FICHIERS DE POINTS */
/* PAR ECHANTILLONNAGE DE FIGURES   */
/* version 19/5/99                  */
/************************************/

/* (projection d'attracteur d'IFS)  */
/* Algorithme deterministe          */

/*--------------------------*/
/* Operations informatiques */
/*--------------------------*/

FILE *deb_echan(MOT iprod, int m);
void fin_echan(FILE *f);
void destru_echan(MOT iprod);

/*----------------------*/
/* Figures elementaires */
/*----------------------*/
/* definies dans l'espace de visualisation */ 

/* point (en coordonnees cartesiennes) */ 
void echan_pt(FILE *echan, POINT p);
/*
void echan_pt_math(FILE *echan, MATH lp);
void echan_segm(FILE *echan, POINT p, POINT q);
void echan_triangle(FILE *echan, POINT p0, POINT p1, POINT p2);
void echan_quadrilatere(FILE *echan, POINT p0, POINT p1, POINT p2, POINT p3);
*/

/*-----------------*/ 
/* Figures iterees */ 
/*-----------------*/ 
/*
void echan_pc(FILE *echan, FIGIT fig);
*/
void echan_figv0(FILE *echan, FIGIT fig);
void echan_determ(FILE *echan, FIGIT fig);
void echan_determ_iter(FILE *echan, FIGIT fig);
