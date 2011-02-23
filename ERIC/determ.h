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
/* Visualisation d'attracteur d'IFS */
/*     Algorithme deterministe      */
/************************************/

/* Figures elementaires */
void tima_pt(TIMA tima,POINT p);
void tima_ptm(TIMA tima,POINT p);
/*
void determ0(TIMA tima,MATH proj);
*/
void tima_ptm_math(TIMA tima,MATH lp);

/* Figures filaires */
void tima_segm(TIMA tima, POINT p, POINT q);
void tima_triangle(TIMA tima, POINT p0, POINT p1, POINT p2);
void tima_quadrilatere(TIMA tima, POINT p0, POINT p1, POINT p2, POINT p3);
void tima_lb_math(TIMA tima,MATH lp);
void tima_repere(TIMA tima,MATH tc);

/* Figures iterees */
void tima_repere_cam(TIMA tima, CAMERA cam);
void tima_pc(TIMA tima, FIGIT fig);
void tima_lbc(TIMA tima, FIGIT fig);
/*
void determ(TIMA tima, MATH proj, IFS ifs, int n);
void tima_determ_v(TIMA tima, GIFS gifs, int n);
*/
void tima_figv0(TIMA tima, FIGIT fig);
void tima_figv(TIMA tima, FIGIT fig);
void tima_determ_iter(TIMA tima, FIGIT fig);

/* Espace de modelisation */
void tima_pt_pass(TIMA tima, MATH pass, POINT p);
void tima_lpt_pass(TIMA tima, MATH pass, L_POINT lp);
void tima_determ_pass(TIMA tima, MATH pass, IFS gifs, int n);
void tima_determ(TIMA tima, Flottant lfen, GIFS gifs, int n);
void tima_cam_fig(TIMA tima, CAMERA cam, FIGIT fig);
void tima_determ_pass(TIMA tima, MATH pass, IFS ifs, int n);
void tima_determ(TIMA tima, Flottant lfen, GIFS gifs, int n);
