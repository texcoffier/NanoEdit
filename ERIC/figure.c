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
#include "util.h"
#include "point.h"
#include "matrice.h"
#include "controle.h"
#include "ifs.h"
#include "gifs.h"
#include "graphisme.h"
#include "figure.h"

/*******************/
/* FIGURES ITEREES */
/*******************/

/*------------*/
/* Iterations */
/*------------*/

/* Ecriture iteration */
void ecrit_iter(FILE *f, char *nom, ITER iter)
 {
  fprintf(f,nom);
  fprintf(f," : nombre d'iterations "); 
  if(iter.fini)
    fprintf(f,"fini = %f\n",iter.val); 
  else
    fprintf(f,"infini, distance minimum = %f\n",iter.val); 
  return;
 }    
/* Iteration finie (discrete) */
ITER iter_n(int n)
 {
  ITER iter;
  iter.fini = Vrai;
  iter.val = n;
  return iter;
 }
/* Iteration infinie : distance minimum de vision */
ITER iter_infini_d(Flottant d)
 {
  ITER iter;
  iter.fini = Faux;
  iter.val = d;
  return iter;
 }
/* Iteration infinie : distance minimum de vision fixee par defaut */
ITER iter_infini()
 {
  ITER iter;
  iter.fini = Faux;
  iter.val = DMIN;
  return iter;
 }
/* Iteration nombre reel : equipotentielles */
ITER iter_r(Flottant r)
 {
  ITER iter;
  iter.fini = Vrai;
  iter.val = r;
  return iter;
 }
/* Condition d'iteration */
/* distance en cours d'iteration elevee au carre (acceleration calcul) */
Booleen cond_iter(ITER iter, MATH pass)
 {
  if(iter.fini)
    return (iter.val >= 1);
  else
    return (ray2_math(pass) > iter.val);
 }
/* Decrementation profondeur d'iteration */
ITER iter_decr(ITER iter)
 {
  ITER iterr;
  iterr.fini = iter.fini;
  if(iter.fini) iterr.val = iter.val - 1;
  else iterr.val = iter.val;
  return iterr;
 }

/*--------*/
/* Motifs */
/*--------*/

/* Ecriture de motifs */
void ecrit_motif(FILE *f, char *nom, MOTIF motif)
 {
  char *nomotif;
  switch(motif)
     {
      case Point :
        {
         nomotif = "Point";
         break;
        }
      case Segment :
        {
         nomotif = "Segment";
         break;
        }
      case Triangle :
        {
         nomotif = "Triangle";
         break;
        }
      case Quadrilatere :
        {
         nomotif = "Quadrilatere";
         break;
        }
      case Sphere :
        {
         nomotif = "Sphere";
         break;
        }
      case Cube :
        {
         nomotif = "Cube";
         break;
        }
     }
  fprintf(f,"%s = %s\n",nom,nomotif);
 }

/* Extraction des caracteristiques d'un motif 
   MODIFICATION du 4-4-2000 (=/= version MOTEUR)
   -entree : matrice homogene et suite d'indices
   -sortie : liste de points graphiques cartesiens du type :
     (pt) 2D ou 3D
     (pt,couleur) avec couleur 0D(N&B),1D(gris),3D(RVB)  
     (pt,couleur,normale) avec normale 0D(pas de normale),3D(normale au sommet)
*/
L_VECH lpt_math_fig(MATH proj, FIGIT fig)
 {
   GIFS gifs;
   L_VECH lv;
   gifs = fig.gifs;
   lv = lvh_math_extr(proj,gifs.extrem);
   return lv;
 }
/* Cas du point (hypotheses : proj CAxBA, p0 point fini) */
L_POINT lpt_point(ESP_GRAPH eg, MATH proj, L_INT je)
 {
   L_POINT lp;
   lp = lpt_crea(1);
   lp.pt[0] = car_pt(pt_col(proj,je.c[0]));
   return lp;
 }
/* Cas du segment (hypotheses : proj CAxBA, p0 point fini) */
L_POINT lpt_segment(ESP_GRAPH eg, MATH proj, L_INT je)
 {
   L_POINT lp;
   lp = lpt_crea(2);
   lp.pt[0] = car_pt(pt_col(proj,je.c[0]));
   lp.pt[1] = car_pt(pt_col(proj,je.c[1]));
   return lp;
 }
/* Cas du triangle (hypotheses : proj CAxBA, p0 point fini) */
L_POINT lpt_triangle(ESP_GRAPH eg, MATH proj, L_INT je)
 {
   L_POINT lp; 
   POINT pc0,pc1,pc2;
   VECTEUR v01,v02,vn;
   lp = lpt_crea(3);
   pc0 = car_pt(pt_col(proj,je.c[0]));
   pc1 = car_pt(pt_col(proj,je.c[1]));
   pc2 = car_pt(pt_col(proj,je.c[2]));
   if(dimn_eg(eg) == 0)
     {
      lp.pt[0] = pc0;
      lp.pt[1] = pc1;
      lp.pt[2] = pc2;
     }
   else
     {
      v01 = vec_pt2(pc0,pc1);
      v02 = vec_pt2(pc0,pc2);
      vn = vec_pvec(v01,v02); 
      lp.pt[0] = pt_pcar(pc0,vn);
      lp.pt[1] = pt_pcar(pc1,vn);
      lp.pt[2] = pt_pcar(pc2,vn);
      destru_pt(v01);
      destru_pt(v02);
      destru_pt(vn);
     }
  destru_pt(pc0);
  destru_pt(pc1);
  destru_pt(pc2);
  return lp;
 }
/* Cas du quadrilatere */
L_POINT lpt_quadrilatere(ESP_GRAPH eg, MATH proj, L_INT je)
 {
   L_POINT lp; 
   POINT pc0,pc1,pc2,pc3;
   VECTEUR v01,v12,v23,v30;
   lp = lpt_crea(4);
   pc0 = car_pt(pt_col(proj,je.c[0]));
   pc1 = car_pt(pt_col(proj,je.c[1]));
   pc2 = car_pt(pt_col(proj,je.c[2]));
   pc3 = car_pt(pt_col(proj,je.c[3]));
   if(dimn_eg(eg) == 0)
     {
      lp.pt[0] = pc0;
      lp.pt[1] = pc1;
      lp.pt[2] = pc2;
      lp.pt[3] = pc3;
     }
   else
     {
      v01 = vec_pt2(pc0,pc1);
      v12 = vec_pt2(pc1,pc2);
      v23 = vec_pt2(pc2,pc3);
      v30 = vec_pt2(pc3,pc0);
      lp.pt[0] = pt_coin_face(pc0,v30,v01);
      lp.pt[1] = pt_coin_face(pc1,v01,v12);
      lp.pt[2] = pt_coin_face(pc2,v12,v23);
      lp.pt[3] = pt_coin_face(pc3,v23,v30);
      destru_pt(v01);
      destru_pt(v12);
      destru_pt(v23);
      destru_pt(v30);
     }
  destru_pt(pc0);
  destru_pt(pc1);
  destru_pt(pc2);
  destru_pt(pc3);
  return lp;
 }
/* calcul auxiliaire */
POINT pt_coin_face(POINT p, VECTEUR u, VECTEUR v)
 {
  POINT pn;
  VECTEUR w,wn;
  w = vec_pvec(u,v);
  wn = vec_scal(-1.,w);
  pn = pt_pcar(p,wn);
  destru_pt(w);
  destru_pt(wn);
  return pn; 
 }

/*---------*/
/* Figures */
/*---------*/

/*..........................*/
/* Operations informatiques */
/*..........................*/

/* Creation de figure iteree */ 
FIGIT fig_crea(GIFS gifs, ITER iter, MOTIF motif)
 {
  FIGIT figr;
  figr.gifs = gifs;
  figr.iter = iter;
  figr.motif = motif;
  figr.couleur = jaune();
  figr.eg = eg_3d();
  return figr;
 }
/* Ecriture de figure */
void ecrit_fig(FILE *f, char *nom, FIGIT fig)
 {
  fprintf(f,nom);
  fprintf(f," : Figure Iteree\n");
  ecrit_gifs(f," * GIFS",fig.gifs);
  ecrit_iter(f," * Iteration",fig.iter);
  ecrit_motif(f," * Motif",fig.motif);
  ecrit_coul(f," * Couleur",fig.couleur);
  fprintf(f,"\n");
 }

/*.....................*/
/* Operations uniaires */
/*.....................*/

/* Figure iteree finie */ 
FIGIT fig_n(GIFS gifs, int n, MOTIF motif)
 {
  return fig_crea(gifs,iter_n(n),motif);
 }
/* Approximation test de distance */
FIGIT fig_d(GIFS gifs, Flottant d, MOTIF motif)
 {
  return fig_crea(gifs,iter_infini_d(d),motif);
 }
/* Distance minimum par defaut */ 
FIGIT fig_dmin(GIFS gifs, MOTIF motif)
 {
  return fig_crea(gifs,iter_infini(),motif);
 }

/*....................*/
/* Operations unaires */
/*....................*/

/* Caracteristiques graphiques d'une figure */
ESP_GRAPH eg_fig(FIGIT fig)
 {
  return fig.eg;
 }
RVB coul_fig(FIGIT fig)
 {
  return fig.couleur;
 }

/* Transformation */
FIGIT fig_mul(MATH t, FIGIT fig)
 {
  FIGIT figr;
  figr.gifs = gifs_mul(t,fig.gifs);
  figr.iter = fig.iter;
  figr.motif = fig.motif;
  figr.couleur = coul_dupli(fig.couleur);
  return figr;
 }
/* Projection de figure dans l'espace de visualisation */
FIGIT fig_visu(CAMERA cam, FIGIT fig)
 {
  FIGIT figv;
  figv = fig_mul(cam.passv,fig);
  return figv;
 }
/* Normalisation */
FIGIT fig_normal(FIGIT fig)
 {
  FIGIT figr;
  figr.gifs = gifs_normal(fig.gifs);
  figr.iter = fig.iter;
  figr.motif = fig.motif;
  figr.couleur = coul_dupli(fig.couleur);
  return figr;
 }

/* Test d'iteration */
/*
Booleen cond_iter_fig(FIGIT fig)
 {
  L_VECH lv;
  if(iter.fini) return (iter.val >= 1);
  else
    {
     lv = lvh_extrem_gifs(fig.gifs);
     return (ray2_math(lv) > iter.val);
    }
 }
*/
/* Iteration */
/*
FIGIT fig_iter(int i, FIGIT fig)
 {
  FIGIT figr;
  figr.gifs = gifs_it(i,gifs);
  figr.iter = iter_decr(iter);
  figr.motif = fig.motif;
  figr.couleur = fig.couleur;
  return figr;
 }
*/

/*--------------*/
/* Bibliotheque */
/*--------------*/

/* Motif et iteration fixes par le type topologique */
/* figures ponctuelles (nuages) */
/*
FIGIT fig_ponc(GIFS gifs, ITER iter);
 {
  return fig_crea(gifs, iter, Point);
 }
*/
/* figures filaires */
/*
FIGIT fig_fil(GIFS gifs, Flottant d);
 {
  return fig_d(gifs,d,Segment);
 }
*/
/* figures surfaciques */
/*
FIGIT fig_surf(GIFS gifs, int n);
 {
  return fig_n(gifs,n,Triangle);
 }
*/


