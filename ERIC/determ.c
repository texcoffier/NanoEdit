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
#include "ifs.h"
#include "courbe.h"
#include "gifs.h"
#include "graphisme.h"
#include "figure.h"
#include "image.h"
#include "determ.h"

/***********************/
/* GENERATION D'IMAGES */
/* A PARTIR DE FIGURES */
/***********************/

/* Projection d'attracteur d'IFS */
/* Algorithme deterministe       */

/*----------------------*/
/* Figures elementaires */
/*----------------------*/
/* definies dans l'espace de visualisation */ 

/* point : visualisation de sa projection 2d */ 
void tima_pt(TIMA tima, POINT p)
 {
  Flottant x,y;
  POINT q;
  q = car3_pt(p);
  x = q.c[1];
  y = q.c[2];
  tima_xy(tima,x,y); 
  destru_pt(q);
 }
/* point avec marqueur carre */ 
void tima_ptm(TIMA tima, POINT p)
 {
  Flottant x,y;
  POINT q;
  q = car3_pt(p);
  x = q.c[1];
  y = q.c[2];
  tima_xym(tima,x,y); 
  destru_pt(q);
 }
/* point avec marqueur carre 3 pixels */ 
void tima_pt3p(TIMA tima, POINT p)
 {
  Flottant x,y;
  POINT q;
  q = car3_pt(p);
  x = q.c[1];
  y = q.c[2];
  tima_xy3p(tima,x,y); 
  destru_pt(q);
 }
/* premiere colonne d'une matrice avec un point */
/* 
   Fig(P) = P*{e0}
*/
/*
void determ0(TIMA tima, MATH proj)
 {
  VECH p0;
  p0 = pt_col(lp,0);
  tima_pt(tima,p0); 
  destru_pt(p0);
 }
*/
/* suite de points marques donnes par une matrice */
/*
   Fig(P) = P*{e0,...,em}
*/
void tima_ptm_math(TIMA tima, MATH lp)
 {
  int m,i;
  POINT pi;
  m = dimd_math(lp);
  for(i=0;i<=m;i++)
    {
     pi = pt_col(lp,i);
     tima_ptm(tima,pi);
     destru_pt(pi);
    }
 }

/*------------------*/
/* Figures filaires */
/*------------------*/

/* segment */
void tima_segm(TIMA tima, POINT p, POINT q)
 {
  MATH pass;
  IFS ifs;
  GIFS gifs;
  FIGIT fig;
  pass = math_pt2(p,q);
  ifs = arc_b1(0.5);
  gifs = gifs_proj_ifs(pass,ifs);
  /*
  fig = fig_d(gifs,2.,Point);
  */
  fig = fig_n(gifs,6,Point);
  tima_figv(tima,fig);
  destru_math(pass);
  destru_ifs(ifs);
 }
/* triangle */
void tima_triangle(TIMA tima, POINT p0, POINT p1, POINT p2)
 {
  tima_segm(tima,p0,p1);
  tima_segm(tima,p1,p2);
  tima_segm(tima,p2,p0);
 }
/* quadrilatere (eventuellement gauche) */
void tima_quadrilatere(TIMA tima, POINT p0, POINT p1, POINT p2, POINT p3)
 {
  tima_segm(tima,p0,p1);
  tima_segm(tima,p1,p2);
  tima_segm(tima,p2,p3);
  tima_segm(tima,p3,p0);
 }
/* ligne brisee definie par une matrice */
/*
   Fig(P) = P*[e0,...,em]
*/
void tima_lb_math(TIMA tima, MATH proj)
 {
  POINT p,q;
  int m,i;
  m = dimd_math(proj);
  p = pt_col(proj,0);
  for(i=1;i<=m;i++)
    {
     q = pt_col(proj,i);
     tima_segm(tima,p,q);
     p = q;
    }
  destru_pt(p);
  destru_pt(q);
 }
/* repere */
void tima_repere(TIMA tima, MATH t)
 {
  int m,i;
  POINT p0;
  L_POINT lp;
  m = dimd_math(t);
  lp = lpt_vec_math(t);
  p0 = lp.pt[0];
  tima_ptm(tima,p0);
  for(i=1;i<=m;i++)
    tima_segm(tima,p0,lp.pt[i]);
  destru_lpt(lp);
 }

/*-----------------*/ 
/* Figures iterees */ 
/*-----------------*/ 
/* definies dans espace de visualisation */

/* Repere camera */
void tima_repere_cam(TIMA tima, CAMERA cam)
 {
  tima_repere(tima,cam.passv);
 }
/* Points de controle avec marqueurs */ 
void tima_pc(TIMA tima, FIGIT fig)
 {
  tima_ptm_math(tima,(fig.gifs).proj);
 }
/* Ligne brisee de controle */ 
void tima_lbc(TIMA tima, FIGIT fig)
 {
  tima_lb_math(tima,(fig.gifs).proj);
 }

/* Appel (normalisation) */
void tima_figv(TIMA tima, FIGIT fig)
 {
  FIGIT figv;
  GIFS gifsv;
  gifsv = gifs_normal(fig.gifs); 
  figv.gifs = gifsv; 
  figv.iter = fig.iter;
  figv.motif = fig.motif;
  tima_determ_iter(tima,figv);
  destru_gifs(gifsv);
 }
/* Figure de base (sans iteration et dans espace de visualisation) */
void tima_figv0(TIMA tima, FIGIT fig)
 {
  L_VECH lv;
  lv = lvh_extrem_gifs(fig.gifs);
  switch(fig.motif)
    {
     case Point :
      {
      /*
       tima_pt3p(tima,lv.pt[0]);
      */
       tima_pt(tima,lv.pt[0]);
       break;
      }
     case Segment :
      {
       tima_segm(tima,lv.pt[0],lv.pt[1]);
       break;
      }
     case Triangle :
      {
       tima_triangle(tima,lv.pt[0],lv.pt[1],lv.pt[2]);
       break;
      }
     case Quadrilatere :
      {
       tima_quadrilatere(tima,lv.pt[0],lv.pt[1],lv.pt[2],lv.pt[3]);
       break;
      }
    default:
      fprintf(stderr,"Erreur %s:%d\n", __FILE__, __LINE__) ;
    }
  destru_lpt(lv);
 }
/*
     case Grille :
      {
       lpe = (fig.gifs).proj;
       tima_grille(tima,lpe);
       break;
      }
*/
/* Noyau recursif */
void tima_determ_iter(TIMA tima, FIGIT fig)
 {
  int n,i;
  FIGIT figi;
  GIFS gifs;
  MATH pass;
  ITER iter;
  gifs = fig.gifs;
  pass = (fig.gifs).proj;
  iter = fig.iter;
  n = nb_gifs(fig.gifs);
  if(cond_iter(iter,pass))
    {
     iter = iter_decr(iter);
     for(i=0;i<n;i++)
       {
	figi.gifs = gifs_it(i,gifs);
	figi.iter = iter;
	figi.motif = fig.motif;
        tima_determ_iter(tima,figi);
       }
    }
  else tima_figv0(tima,fig);
 }

/*------------------------------------------------*/
/* Figures definie dans un espace de modelisation */
/*------------------------------------------------*/
/* definies dans un espace de modelisation */

/* point dans un espace de modelisation */ 
void tima_ptm_pass(TIMA tima, MATH pass, POINT p)
 {
  POINT q;
  q = pt_mul(pass,p);
  tima_ptm(tima,q);
  destru_pt(q);
 }
/* liste de points dans un espace de modelisation */ 
void tima_lpt_pass(TIMA tima, MATH pass, L_POINT lp)
 {
  int i;
  for(i=0;i<lp.nb;i++)
    tima_ptm_pass(tima,pass,lp.pt[i]);
 }
/* Vue de figure definie dans un espace de modelisation */
void tima_cam_fig(TIMA tima, CAMERA cam, FIGIT fig)
 {
  FIGIT figv;
  figv = fig_visu(cam,fig);
  tima_figv(tima,figv);
 }

/* visualisation d'IFS defini dans espace d'iteration */
/* ancienne version */
/*
void tima_determ_pass(TIMA tima, MATH pass, IFS ifs, int n)
 {
  GIFS gifs,gifsv;
  gifs = gifs_proj_ifs(pass,ifs);
  gifsv = gifs_normal(gifs);            
  determ(tima,gifsv.proj,gifsv.ifs,n);
  destru_gifs(gifsv);
 }
*/
void tima_determ_pass(TIMA tima, MATH pass, IFS ifs, int n)
 {
  GIFS gifsv;
  FIGIT figv;
  gifsv = gifs_proj_ifs(pass,ifs);
  figv = fig_n(gifsv,n,Point);
  tima_figv(tima,figv);
  /*
  destru_gifs(gifsv);
  */
 }
/* visualisation de GIFS defini dans espace de modelisation */
/* ancienne version */
/*
void tima_determ(TIMA tima, Flottant lfen, GIFS gifs, int n)
 {
  MATH echel;
  GIFS gifsv;
  echel = echel_larg(tima,lfen);
  gifsv = gifs_mul(echel,gifs);
  tima_determ_v(tima, gifsv, n);
  destru_gifs(gifsv);
  destru_math(echel);
 }
*/
void tima_determ(TIMA tima, Flottant lfen, GIFS gifs, int n)
 {
  MATH echel;
  GIFS gifsv;
  FIGIT figv;
  echel = echel_larg(tima,lfen);
  gifsv = gifs_mul(echel,gifs);
  figv = fig_n(gifsv,n,Point);
  tima_figv(tima,figv);
  destru_gifs(gifsv);
  destru_math(echel);
 }
