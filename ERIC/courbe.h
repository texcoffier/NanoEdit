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
/*******************/
/* COURBES A POLES */
/*******************/

/*-----------*/
/* Courbe 1D */
/*-----------*/

IFS arc_b1(Flottant r01);

/*------------*/
/* Courbes 2D */
/*------------*/

IFS arc_b2(VECH q01, VECH qi, VECH q11);
IFS arc_b2_tg(Flottant r01, VECH qi, Flottant r12);
IFS arc_b2_c1(Flottant r01, Flottant r12, Flottant r012); 
IFS arc_b2_para(Flottant r);
IFS arc_b2_c1s(Flottant r01, Flottant r12); 
IFS arc_b2_bsp(Flottant r); 
IFS arc_b2_sym(VECH q01, Flottant ri);
IFS arc_b2_sym_c1(Flottant r);

/*------------*/
/* Courbes 3D */
/*------------*/

IFS arc_b3(VECH q01, VECH q02, VECH qi, VECH q11, VECH q12);
IFS arc_b3_tg(Flottant s01, VECH q02, VECH qi, VECH q11, Flottant s12);
IFS arc_b3_c1(Flottant s01, VECH q02, Flottant si, VECH q11, Flottant s12);
IFS arc_b3_c1b(Flottant r01, Flottant r12, Flottant r23,
               Flottant r012, Flottant r123, Flottant r0123);
IFS arc_b3_cub(Flottant r01);
/*
IFS arc_b3_c1s(Flottant r01, Flottant r12, Flottant r23,
               Flottant r112, Flottant r122);
IFS arc_b3_bsp(Flottant r01, Flottant r12, Flottant r23);
*/
IFS arc_b3_sym(VECH q01, VECH q02, Flottant ri);

/*------------*/
/* Courbes mD */ 
/*------------*/
/*
IFS arc_bm(int m,L_VECH lq) 
*/
