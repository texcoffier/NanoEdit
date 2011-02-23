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
/* MATRICES HOMOGENES (m+1) x (n+1) */  
/************************************/

/* Type des operateurs */
/*
  Cas de repere :
   2 cas correspondant aux systemes de coordonnees 
   - Affines : RB = (p0,p1,...,pm) ou RC = (p0,v1,...,vm)
	       avec pi et vi en coordonnees C ou B
   - Interpretation projective : 
       RB = (p0,p1,...,pm) ou RC = (p0,v1,..,vm) avec vi points a l'infini
   - par defaut  Af 
   - plongement :
       X Af RY -> X Pr RY
  => 8 cas d'operateur (passage d'un espace a un autre) :
      (C ou B)(Af ou Pr)(RC ou RB)
  regle de multiplication des operateurs :
      X G RY * Y H RZ = X Max(G,H) Z  
  Cas utilises :
  * 4 cas de transformation d'un espace dans lui-meme :
      C(Af ou Pr)RC ou B(Af ou Pr)RB
  * 2 cas utilises de passage : scene -> image : 
      - projection parallele : C Af RC
      - projection perspective : C Pr RC
  * 4 cas utilises de GIFS :
      - classiques :
        C(Af ou Pr)RC, C(Af ou Pr)RC
      - formes a poles :
        C(Af ou Pr)RB, B(Af ou Pr)RB
*/
/* Types logiques :
   * types geometriques :
     - applications affines R^n -> R^m
       Af -> Af 
     - applications projectives P^n R -> P^m R
       Proj -> Proj  
     - plongements : T Af*pt Proj = pt Proj et T Proj*pt Af = pt Proj 
   * types de coordonnees 
     C RC = applic : C^nR -> C^mR ou reperes (p0,v1,..,vn);
     C RB = applic : B^nR -> C^mR ou reperes (p0,p1,..,pn); 
     B RC = applic : C^nR -> B^mR ou reperes (p0,v1,..,vn);
     B RB  = applic : B^nR -> B^mR ou reperes (p0,p1,..,pn);
   * types algebriques :
     - semigroupe d'operateurs internes (matrices carrees)
     - groupe de transformations (matrices inversibles)
     - categorie
       si m=/=n => passage d'un espace a un autre
       (si rang max alors : m > n => plongement et m < n projection)
*/

/* Type physique : matrices homogenes (m+1) x (n+1) */

typedef struct
{
  t_espace m, n ;
  Flottant **c ;
} MATH ;

/* Semigroupe d'operateurs */

typedef enum {Sim, Hyp, Dil, Cis, Aff, Proj} SEMIG;

/*--------------------------*/
/* Operations informatiques */
/*--------------------------*/

MATH math_crea_esp(t_espace espa, t_espace espd);
MATH math_crea(int m, int n);
MATH math_crea_BA(int m, int n);
void destru_math(MATH t);
MATH math_dupli(MATH t);
void modif_math_esp(MATH t, t_espace espa, t_espace espd);
void ecrit_semig(FILE *f, char *nom, SEMIG sem);
void ecrit_math(FILE *f, char *nomt, MATH t);

/*-------------------------*/
/* Operations matricielles */
/*-------------------------*/

/* Operations 0-aires */
MATH math_nul(int m,int n);
MATH math_pass(int m,int n); 
MATH math_id(int m);
/* (p0,..,pm) -> BA(m) */
MATH math_pt1(POINT p0);
MATH math_pt2(POINT p0, POINT p1);
MATH math_pt3(POINT p0, POINT p1, POINT p2);
MATH math_pt4(POINT p0, POINT p1, POINT p2, POINT p3);
MATH math_pt5(POINT p0, POINT p1, POINT p2, POINT p3, POINT p4);
MATH math_pt6(POINT p0, POINT p1, POINT p2, POINT p3, POINT p4, POINT p5);

/* Operations unaires */

/* espaces */
t_espace espa_math(MATH t);
t_espace espd_math(MATH t);
int dima_math(MATH t);
int dimd_math(MATH t);
Flottant ray2_math(MATH t);

/* action sur les vecteurs homogenes */
VECH vh_math(MATH t, VECH p);
POINT pt_math(MATH t, POINT p);  /* equivalent */
POINT pt_mul(MATH t, POINT p);   /* equivalent */
void mul_math_pt(POINT pr, MATH t, POINT p);

/* extraction de colonnes */
POINT pt_col(MATH t, int j);
/*
VECH vh_col(MATH t, int j);
*/
MATH math_col2(MATH t1, int j, int k);
MATH math_extr(MATH t1, L_INT li);

/* modification de colonnes */
void col_math_vh1(MATH tr, int j1, VECH p1);
void col_math_li_math_li(MATH tr, L_INT jr, MATH ts, L_INT js);
void col_math_li_math(MATH tr, L_INT jr, MATH ts);

/* operations unaires sur les matrices */
MATH math_transp(MATH t1);
MATH math_reflex(MATH t1);
MATH sous_math(int m, MATH t1, int i, int j);
Flottant det_math(int m, MATH t1);
Flottant codet_math(int m, MATH t1, int i, int j);
MATH math_inverse(MATH t1);
MATH math_inverse_d(MATH t1);
MATH math_inverse_g(MATH t1);

/* Operations binaires */

MATH math_mul(MATH t1, MATH t2);
MATH math_div_d(MATH t1, MATH t2);
MATH math_div_g(MATH t1, MATH t2);
MATH math_conj(MATH t1, MATH t2);

/*--------------------*/
/* Operations affines */
/*--------------------*/
/* operateurs affines en coordonnees cartesiennes */

/* Operations 0-aires */
MATH math_ho(int m, Flottant r);
MATH math_CA_BA(int m);
MATH math_BA_CA(int m);

/* Operations unaires */
MATH math_lin(MATH t1);
POINT or_math(MATH t1);
POINT centre_math(MATH t1);
/*
MATH lin_math(MATH t1);
*/

/* Operations binaires */
MATH math_diff(MATH t1, MATH t2);
MATH math_pcar(MATH t1, MATH t2);

/*--------------*/
/* Bibliotheque */
/*--------------*/

/* Espaces 1D */
/* -> BA(1) */
MATH math2x2(Flottant a00,Flottant a01,
	     Flottant a10,Flottant a11);
/* -> CA(1) */
MATH math_id1();
MATH ho1(Flottant r);
MATH tr1(Flottant x);
MATH taf1(Flottant x0, Flottant r);

/* Espaces 2D */
/* -> BA(2) */
MATH math3x3(Flottant a00,Flottant a01, Flottant a02,
	     Flottant a10,Flottant a11, Flottant a12,
             Flottant a20,Flottant a21, Flottant a22);
/* -> CA(2) */
MATH math_id2();
MATH ho2(Flottant r);
MATH tx2(Flottant x);
MATH ty2(Flottant y);
MATH tr2(Flottant x, Flottant y);
MATH dx2(Flottant x);
MATH dy2(Flottant y);
MATH dil2(Flottant x, Flottant y);
MATH tdil2(Flottant x0, Flottant y0, Flottant a, Flottant d);
MATH simh2(Flottant x0, Flottant y0, Flottant a, Flottant c);
MATH rot2(Flottant theta);
MATH sim2(Flottant x0, Flottant y0, Flottant a, Flottant c);
MATH cxy2(Flottant c);
MATH cyx2(Flottant b);
MATH taf2(Flottant x0, Flottant y0, Flottant a, Flottant b, Flottant c, Flottant d);
MATH tafo2(Flottant a, Flottant b, Flottant c, Flottant d);
MATH math_sem_pt2(SEMIG sem, POINT p, POINT q);

/* Espaces 3D */
/* -> CA(3) */
MATH math_id3();
MATH ho3(Flottant r);
MATH tx(Flottant x);
MATH ty(Flottant y);
MATH tz(Flottant z);
MATH tr3(Flottant x, Flottant y, Flottant z);
MATH rx(Flottant theta);
MATH ry(Flottant theta);
MATH rz(Flottant theta);
/* CA(2) <-> CA(3) */ 
MATH plong_xy();
MATH proj_xy();
MATH plong_yz();
MATH proj_yz();
/* Matrices dans des semigroupes */ 
MATH math_sem_pt3(SEMIG sem, POINT p0, POINT p1, POINT p2);
