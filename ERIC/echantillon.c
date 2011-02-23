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
#include "gifs.h"
#include "graphisme.h"
#include "figure.h"
#include "echantillon.h"

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

/* Ouverture de fichier d'echantillon */
FILE *deb_echan(MOT iprod, int m)
 {
  FILE *f;
  MOT ipov;
  ipov = conca(iprod,mot(4,".ech"));
  ecrit_mot(stdout,"  * Fichier echantillon",ipov);
  f = fopen(ipov.c,"w");
  fprintf(f,"%d\n",m);
  return(f);
 }
/* Fermeture de fichier d'echantillon */
void fin_echan(FILE *f)
 {
  fclose(f);
  fprintf(stdout,"    fin fichier.ech\n");
 }
/* Destruction de fichier d'echantillon */
void destru_echan(MOT iprod)
 {
  MOT icom;
  icom = conca3(mot(3,"rm "),iprod,mot(4,".ech"));
  system(icom.c);
 }

/*----------------------*/
/* Figures elementaires */
/*----------------------*/
/* definies dans l'espace de visualisation */ 

/* point (en coordonnees cartesiennes) */ 
void echan_pt(FILE *echan, POINT p)
 {
  POINT q;
  int m, i ;

  q = car_pt(p);
  m = dim_pt(p);
  for(i=0;i<=m;i++)
    {
     fprintf(echan,"%f ",q.c[i]);
    }
  destru_pt(q);
 }
/* suite de points donnee par une matrice de points cartesiens */
/*
   Fig(P) = P*{e0,...,em}
*/
/*
void echan_pt_math(FILE *echan, MATH lp)
 {
  int m,i;
  POINT pi;
  m = dimd_math(lp);
  for(i=0;i<=m;i++)
    {
     pi = pt_col(lp,i);
     echan_pt(echan,pi);
     destru_pt(pi);
    }
 }
*/
/* Lignes brisees */
/* segment (couple de points) */
/*
void echan_segm(FILE *echan, POINT p, POINT q);
*/
/* triangle (triplet de points) */
/*
void echan_triangle(FILE *echan, POINT p0, POINT p1, POINT p2)
*/
/* quadrilatere (quadruplet de points) */
/*
void echan_quadrilatere(FILE *echan, POINT p0, POINT p1, POINT p2, POINT p3)
*/

/*-----------------*/ 
/* Figures iterees */ 
/*-----------------*/ 
/* definies dans espace de visualisation */

/* points de controle */ 
/*
void echan_pc(FILE *echan, FIGIT fig)
 {
  echan_pt_math(echan,(fig.gifs).proj);
 }
*/
/* Figure de base */
void echan_figv0(FILE *echan, FIGIT fig)
 {
  L_VECH lp;
  POINT p0;
  lp = lvh_extrem_gifs(fig.gifs);
  switch(fig.motif)
    {
     case Point :
      {
       p0 = pt_moyen(lp);
       echan_pt(echan,p0);
       destru_pt(p0);
       break;
      }
    default:
      fprintf(stderr,"bug %s:%d\n", __FILE__, __LINE__) ;
    }
 }
/*
     case Segment :
      {
       echan_segm(echan,pe0,pe1);
       break;
      }
     case Triangle :
      {
       echan_triangle(echan,pe0,pe1,pe2);
       break;
      }
     case Quadrilatere :
      {
       echan_quadrilatere(echan,pe0,pe1,pe2,pe3);
       break;
      }
     case Grille :
      {
       lpe = (fig.gifs).proj;
       echan_grille(echan,lpe);
       break;
      }
*/
/* Appel (sans normalisation) */
void echan_determ(FILE *echan, FIGIT fig)
 {
  FIGIT figv;
  GIFS gifsv;
  /*
  gifsv = gifs_normal(fig.gifs); 
  figv.gifs = gifsv; 
  figv.iter = fig.iter;
  figv.motif = fig.motif;
  */
  echan_determ_iter(echan,figv);
  destru_gifs(gifsv);
 }
/* Noyau recursif */
void echan_determ_iter(FILE *echan, FIGIT fig)
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
        echan_determ_iter(echan,figi);
       }
    }
  else echan_figv0(echan,fig);
 }
