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
/**********************************************/
/* FORMES GEOMETRIQUES GENEREES PAR ITERATION */
/**********************************************/

/* Formes definies a partir d'un generateur (gifs),
   caracteristiques :
   * type mathematique du modele : 
     - figure (compact K de E) 
       Figures iterees : K_n = U |a|=n T_a K_0
       (champ discret)
     - ou champ (fonction F : E -> R+)
       Figures isopotentielles : K_r = {p/ F(p)>=r }  
   * type d'iteration : 
     - avec condensation (U n K_n) ou champ F : E -> [0,r] 
       ou sans (K_n) ou (K_r), 
     - nombre d'iterations  
       + nombre fini d'iterations  : K_n
         n = 0 => figure de base : K_0
       + nombre reel positif 
         isopotentielle : K_r = {p/ F(p)>=r}  
       + nombre infini : A = lim K_n
         visualisation controlee par une distance minimum d
   * type de motif (figure repetee) (K_0), 
       ily a deux types de transformation des motifs : 
       a partir de P_u = P_e T_u1...T_un  operateur de passage : E_I -> E_M
     - projection exacte :
       contraction dans E_I et projection dans E_M :
       P_u K_I = P_e T_u1...T_un K_I = K_M(P_u) (formule pour sphere)
     - projection approximee : 
       contraction dans E_M : 
       P_u K_I ~ R(P_u) K_M 
       ou P_u K_I <_ R(P_u) K_M 
*/

/* Caracteristiques de forme iteree */

/*
typedef enum {Compact, Champ} TYPE_MODELE;
*/
/*
typedef struct {TYPE_MODELE type; 
		Booleen condens; 
		Booleen fini; int nit; Flottant d; 
		Booleen proj; MOTIF motif;} 
		FORME;
*/

/* Type d'iterations (test de profondeur) */

typedef struct {Booleen fini; Flottant val;} ITER; 

#define DMIN 0.1 

/* Motif */

typedef enum {Point, Segment, Triangle, Quadrilatere, Sphere, Cube} MOTIF;

/* Figure iteree */

typedef struct {GIFS gifs;
		ITER iter; 
		MOTIF motif;
		RVB couleur;
		ESP_GRAPH eg;} FIGIT;

/*------------*/
/* Iterations */
/*------------*/

void ecrit_iter(FILE *f, char *nom, ITER iter);
ITER iter_n(int n);
ITER iter_infini_d(Flottant d);
ITER iter_infini();
ITER iter_r(Flottant r);
Booleen cond_iter(ITER iter, MATH math);
ITER iter_decr(ITER iter);

/*--------*/
/* Motifs */
/*--------*/

void ecrit_motif(FILE *f, char *nom, MOTIF motif);
L_POINT lpt_math_fig(MATH proj,FIGIT fig);
L_POINT lpt_point(ESP_GRAPH eg, MATH proj, L_INT je);
L_POINT lpt_segment(ESP_GRAPH eg, MATH proj, L_INT je);
L_POINT lpt_triangle(ESP_GRAPH eg, MATH proj, L_INT je);
L_POINT lpt_quadrilatere(ESP_GRAPH eg, MATH proj, L_INT je);
POINT pt_coin_face(POINT p, VECTEUR u, VECTEUR v);

/*---------*/
/* Figures */
/*---------*/

/* Operations 0-aires */
FIGIT fig_crea(GIFS gifs, ITER iter, MOTIF motif); 
void ecrit_fig(FILE *f, char *nom, FIGIT fig);
FIGIT fig_n(GIFS gifs, int n, MOTIF motif); 
FIGIT fig_d(GIFS gifs, Flottant d, MOTIF motif);
FIGIT fig_dmin(GIFS gifs, MOTIF motif);

/* Operations unaires */
ESP_GRAPH eg_fig(FIGIT fig);
RVB coul_fig(FIGIT fig);
FIGIT fig_mul(MATH t, FIGIT fig);
FIGIT fig_visu(CAMERA cam, FIGIT fig);
FIGIT fig_normal(FIGIT fig);

/* champ -> figure */

/*--------*/
/* Champs */
/*--------*/

/* Conversions */
/* figure -> champ */
/*
FIGIT echap(GIFS gifs, int n);
*/
