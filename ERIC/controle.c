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
#include <stdio.h>
#include <math.h>
#include "util.h"
#include "point.h"
#include "matrice.h"
#include "controle.h"

/**********************************/
/* POINTS ET VECTEURS DE CONTROLE */  
/**********************************/

/*--------------------------*/
/* Operations informatiques */
/*--------------------------*/
/* representants : vecteurs homogenes */

/* creation de liste de points */
L_POINT lpt_crea(int n)
 {
  L_POINT lpr;
  lpr.nb = n;
  lpr.pt = (POINT *) malloc (n * sizeof(POINT));
  return lpr;
 }
/* destruction de liste de points */
void destru_lpt(L_POINT lp)
 {
  if(lp.nb>0) free(lp.pt);
 }
/* duplication de liste de points */
L_POINT lpt_dupli(L_POINT lp)
 {
  L_POINT lpr;
  int n,i;
  n = lp.nb;
  lpr = lpt_crea(n);
  for(i=0;i<n;i++)
    lpr.pt[i] = pt_dupli(lp.pt[i]);
  return lpr;
 }
/* ecriture liste de points */
void ecrit_lpt(FILE *f, char *nom, L_POINT lp)
 {
  int n,i;
  n = lp.nb;
  fprintf(f,"%s = Liste de %d points\n {\n",nom,lp.nb);
  for(i=0;i<n;i++)
    {
     fprintf(f,"  p%d",i);
     ecrit_pt(f,"",lp.pt[i]); 
    }
  fprintf(f," }\n");
 }

/*--------------------*/
/* Operations 0-aires */
/*--------------------*/

/* Conversion d'une matrice en liste de vecteurs homogenes */
/* (M.0 M.1 ... M.n) -> (p0,p1,..,pn) */
L_POINT lpt_math(MATH t)
 {
  L_POINT lp;
  POINT pj;
  int m,n,i,j;
  m = t.m.dim ;
  n = t.n.dim ;
  lp = lpt_crea(n+1);
  for(j=0;j<=n;j++)
    {
     pj = pt_crea(m);
     for(i=0;i<=m;i++)
        pj.c[i] = t.c[i][j] ;
     lp.pt[j] = pj;
    }
  return lp;
 }
/* Extraction d'une liste de vecteurs homogenes a partir d'une matrice */
/* p_j = M_sj ; condition |s| < dimd(M) */
L_VECH lvh_math_extr(MATH t, L_INT js)
 {
  L_VECH lp;
  POINT pj;
  int m,n,i,j;
  m = dima_math(t);
  n = long_li(js);
  lp = lpt_crea(n);
  for(j=0;j<n;j++)
    {
     pj = pt_crea(m);
     for(i=0;i<=m;i++)
        pj.c[i] = t.c[i][js.c[j]] ;
     lp.pt[j] = pj;
    }
  return lp;
 }

/* Conversion d'une matrice XV en liste de points affines XA */
/* (M.0 M.1 ... M.n) = (p0,v1,..,vn) -> (p0,p0+v1,...,p0+vn) 
   condition : p0 point affine et vi vecteurs */
L_POINT lpt_vec_math(MATH t)
 {
  L_POINT lp;
  Flottant pi0;
  int m,n,i,j;
  m = dima_math(t);
  n = dimd_math(t);
  lp = lpt_crea(n+1);
  for(j=0;j<=n;j++)
     lp.pt[j] = pt_crea(m);
  for(i=0;i<=m;i++)
    {
     pi0 = t.c[i][0] ;
     (lp.pt[0]).c[i] = pi0 ;
     for(j=1;j<=n;j++)
	(lp.pt[j]).c[i] = pi0 + t.c[i][j] ;
    }
  return lp;
 }

/*--------------------*/
/* Operations unaires */
/*--------------------*/

/* Dimension */
/* condition : pt[0].dim = ... pt[n].dim */
int dim_lpt(L_POINT lp)
 {
  return dim_pt(lp.pt[0]);
 }
/* Nombre de points */
int nb_lpt(L_POINT lp)
 {
  return lp.nb;
 }

/* Moyenne (centre de gravite) d'une liste de points */
POINT pt_moyen(L_POINT lp)
 {
  POINT pr;
  Flottant w,xi;
  int m,nb,i,j;
  m = dim_lpt(lp);
  nb = nb_lpt(lp);
  w = 0;
  for(j=0;j<nb;j++)
     w = w + lp.pt[j].c[0];  
  if(w == 0.)
    {
     fprintf(stdout,"ERREUR : Poids nul : point moyen non defini\n");
     return pr;
    }
  for(i=1;i<=m;i++)
    {
     xi = 0;
     for(j=0;j<nb;j++)
       xi = xi + lp.pt[j].c[i];  
     pr.c[i] = xi/w;
    }
  return pr;
 }
/* Extraction d'un point */
/* condition : 0 <= i < lp.nb */ 
POINT pt_extr(L_POINT lp, int i)
 {
  return pt_dupli(lp.pt[i]);
 }
/* Extraction d'une suite de points */
/* condition : ind.nb <= lp.nb */ 
L_POINT lpt_extr(L_POINT lp, L_INT js)
 {
  L_POINT lpr;
  int n,i;
  n = long_li(js);
  lpr = lpt_crea(n);
  for(i=0;i<n;i++)
     lpr.pt[i] = pt_dupli(lp.pt[js.c[i]]);
  return lpr;
 }

/* Conversion directe en une matrice */
/* (p0,p1,..,pn) -> (M.0 M.1 ... M.n) */
MATH math_lpt(L_POINT lp)
 {
  MATH tr;
  int m,n,i,j;
  m = dim_lpt(lp);
  n = lp.nb-1;
  if(n<0) fprintf(stdout,"ERREUR : LISTE DE POINTS VIDE");
  tr = math_crea(m,n);
  for(i=0;i<=m;i++)
    {
     for(j=0;j<=n;j++)
        tr.c[i][j] = (lp.pt[j]).c[i] ;
    }
  return tr;
 }
/* Conversion vectorielle en une matrice */
/* (p0,p1,..,pn) -> (p0,p1-p0,...,pn-p0)
   condition : pi points affines */
MATH math_vec_lpt(L_POINT lp)
 {
  MATH tr;
  float pi0;
  int m,n,i,j;
  m = dim_lpt(lp);
  n = nb_lpt(lp)-1;
  if(n<0) fprintf(stdout,"ERREUR : LISTE DE POINTS VIDE");
  tr = math_crea(m,n);
  for(i=0;i<=m;i++)
    {
     pi0 = (lp.pt[i]).c[0] ;
     tr.c[i][0] = pi0 ;
     for(j=1;j<=n;j++)
        tr.c[i][j] = (lp.pt[j]).c[i] - pi0 ;
    }
  return tr;
 }
/* Modification de colonnes de matrice */
/* T_.jrl <- p_jsl 
   condition : |jr| = |js| */
void col_math_lvh_li(MATH tr, L_INT jr, L_VECH lp, L_INT js)
 {
  int m,n,i,l;
  m = dim_lpt(lp);
  n = long_li(js);
  for(l=0;l<n;l++)
    {
     for(i=0;i<=m;i++)
        tr.c[i][jr.c[l]] = (lp.pt[js.c[l]]).c[i] ;
    }
 }
/* Extraction d'une matrice dans un semigroupe */
/* condition : liste composee de points affines */
MATH math_sem_lpt(SEMIG sem, L_POINT lp)
 {
  int m;
  m = dim_lpt(lp);
  if(lp.nb == 2)
    return math_sem_pt2(sem,lp.pt[0],lp.pt[1]);
  if(lp.nb == 3)
    return math_sem_pt3(sem,lp.pt[0],lp.pt[1],lp.pt[2]);
  if((lp.nb == m+1) && (sem == Aff))
    return math_vec_lpt(lp);
  else
       {
        fprintf(stdout,"ERREUR : projectif en cours  de definition \n");
        return math_id(m);
       }
 }
/* Multiplication d'une liste de points par une matrice */
L_POINT lpt_mul(MATH t, L_POINT lp)
 {
  L_POINT lpr;
  int i;
  lpr = lpt_crea(lp.nb);
  for(i=0;i<lp.nb;i++)
    lpr.pt[i] = pt_mul(t,lp.pt[i]);
  return lpr;
 }

/*---------------------*/
/* Operations binaires */
/*---------------------*/

/* Operations sur les suites */
/*
L_POINT lpt_conca(L_POINT lp1, L_POINT lp2);
*/
/* Extension des operations sur les points */ 
/*
L_POINT lpt_caf(L_POINT lp0, L_POINT lp1, float r);
L_POINT lpt_pcar(L_POINT lp1, L_POINT lp2);
*/

/*--------------*/
/* Bibliotheque */
/*--------------*/

/* hypercubes, simplexes */
