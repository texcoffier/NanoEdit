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
/********************/
/* SURFACES A POLES */
/********************/

/* Utilitaires */

MATH coin_tq(L_INT ju, IFS ifsu, int iu,
	     L_INT kv, MATH lq, L_INT ki,
             L_INT j, IFS ifs, int iw);

/*------------------------*/
/* Carreaux triangulaires */
/*------------------------*/

/* Carreaux subdivises en 4 */

/* Carreau grille degre 1 */
IFS surf_qt1_abc();

/* Carreau grille degre 2 */
IFS surf_qt2_abc(IFS ifsa, IFS ifsb, IFS ifsc, POINT q0, POINT q1, POINT q2);

/*--------------------------*/
/* Carreaux quadrangulaires */
/*--------------------------*/

/* Carreaux subdivises en 4 */

/* Carreau grille 2 x 2 */
IFS surf_qq1x1(IFS ifsa, IFS ifsb, POINT qm);
/* Carreau grille 3 x 2 */
/*
IFS surf_qq2x1(IFS ifsa, IFS ifsb, POINT q0, POINT q1, POINT q2);
*/
/* Carreau grille 3 x 3 */
/*
IFS surf_qq3x3(IFS ifsa, IFS ifsb, MATH lq);
*/
