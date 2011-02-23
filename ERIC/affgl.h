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
/***********************/
/* Affichage en openGL */
/***********************/

void glVertex3f_pt(POINT q);
/*
void glNormal3f_vec(VECTEUR v);
*/
void gl_couleur(RVB coul);

void gl_point(ESP_GRAPH eg, POINT p0);
void gl_segment(ESP_GRAPH eg, POINT p0, POINT p1);
void gl_coin_face(ESP_GRAPH eg, POINT p);
void gl_triangle(ESP_GRAPH eg, POINT p0, POINT p1, POINT p2);
void gl_quadrilatere(ESP_GRAPH eg, POINT p0, POINT p1, POINT p2, POINT p3);

void gl_determ(FIGIT fig);
void gl_fig0(FIGIT fig);
void gl_determ_iter(FIGIT fig);
