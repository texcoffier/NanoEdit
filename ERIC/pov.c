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
#include <GL/gl.h>
#include "util.h"
#include "point.h"
#include "matrice.h"
#include "controle.h"
#include "ifs.h"
#include "gifs.h"
#include "graphisme.h"
#include "figure.h"
#include "pov.h"
/*
#include "image.h"
#include "determ.h"
*/

/****************************/
/* GENERATION DE SCRIPT POV */
/****************************/

/*--------------------------*/
/* Operations informatiques */
/*--------------------------*/

/* Ouverture de scene.pov */
FILE *deb_pov(MOT iprod)
 {
  FILE *f;
  MOT ipov;
  ipov = conca(iprod,mot(4,".pov"));
    ecrit_mot(stdout,"  * Script",ipov);
  f = fopen(ipov.c,"w");
    fprintf(f,"camera{CameraOyz}\n"); 
    fprintf(f,"union{\n object{Repere}\n"); 
  return(f);
 }
/* 
    fprintf(f,"#include ""colors.inc""\n"); 
    fprintf(f,"#include ""repere.inc""\n");
    fprintf(f,"light_source{<2,2,2> color White}\n\n"); 
*/
/* Fermeture de scene.pov */
void fin_pov(FILE *f, CAMERA cam)
 {
  pov_math(f,cam.passm);
  fprintf(f,"}\n");
  fclose(f);
    fprintf(stdout,"    fin script.pov\n");
 }
/* Destruction de scene.pov */
void destru_pov(MOT iprod)
 {
  MOT icom;
  icom = conca3(mot(3,"rm "),iprod,mot(4,".pov"));
  system(icom.c);
 }
/* Calcul d'une image.tga a partir d'un script Pov */
void tga_pov(MOT iprod, CAMERA cam)
 {
  MOT icom;
  int larg,haut;
  larg = larg_cam(cam);
  haut = haut_cam(cam);
  icom = mot_crea(100);
  sprintf(icom.c,"cd %s\n dpovlh %s %d %d\n cd %s",
		  POV.c,iprod.c,larg,haut,MOTEUR.c); 
    ecrit_mot(stdout,"  * Commande",icom);
    ecrit_mot(stdout,"  * Debut calcul image.tga\n",iprod);
  system(icom.c);
    fprintf(stdout,"    fin calcul image.tga\n");
 }

/*--------------------------*/
/* Extraction de parametres */  
/*--------------------------*/

/* Extraction de point cartesien 3D */ 
void pov_pt(FILE *f, POINT p)
 {
  POINT q;
  q = car3_pt(p);
  fprintf(f,"<%g,%g,%g>",q.c[1],q.c[2],q.c[3]);
  destru_pt(q);
 }
/* Ecriture directe d'une matrice affine cartesienne vectorielle 3+1x4 */
void pov_math(FILE *f, MATH r)
 {
  fprintf(f,"  matrix <%g, %g, %g, %g, %g, %g, %g, %g, %g, %g, %g, %g>\n",
	  r.c[1][1],r.c[2][1],r.c[3][1],
	  r.c[1][2],r.c[2][2],r.c[3][2],
	  r.c[1][3],r.c[2][3],r.c[3][3],
	  r.c[1][0],r.c[2][0],r.c[3][0]);
 }
/* Couleur */
void pov_coul(FILE *f, RVB coul)
 {
  fprintf(f,"color red %f green %f blue %f",
  r_coul(coul),v_coul(coul),b_coul(coul));
 }

/*----------------------*/
/* Figures elementaires */
/*----------------------*/

/* Lampe (source de lumiere) */
void pov_lampe(FILE *f, POINT p, RVB coul)
 {
  fprintf(f,"  light_source{");
  pov_pt(f,p);
  fprintf(f," ");
  pov_coul(f,coul);
  fprintf(f,"}\n");
 }
/* Affichage d'une sphere */ 
void pov_sphere(FILE *f, POINT p, Flottant r)
 {
  fprintf(f,"   sphere{");
  pov_pt(f,p);
  fprintf(f,",%g}\n",r);
 }
/* Affichage d'un point avec marqueur */
void pov_ptm(FILE *f, POINT p)
 {
  pov_sphere(f,p,0.025);
 }
/* Affichage d'un point extrait avec marqueur */
void pov_ptm_li(FILE *f, MATH lp, L_INT je)
 {
  POINT p0;
  p0 = pt_col(lp,je.c[0]);
  pov_ptm(f,p0);
  destru_pt(p0);
 }
/* Affichage de suite de points marques a partir d'une matrice 
   cartesienne de points 3+1 x m+1 */ 
void pov_ptm_math(FILE *f, MATH lp)
 {
  int m,i;
  POINT pi;
  m = dimd_math(lp);
  for(i=0;i<=m;i++)
    {
     pi = pt_col(lp,i);
     pov_ptm(f,pi);
     destru_pt(pi);
    }
 }
/* Affichage d'un triangle */ 
void pov_triangle(FILE *f, POINT p0, POINT p1, POINT p2)
 {
  fprintf(f,"   triangle{");
  pov_pt(f,p0);
  pov_pt(f,p1);
  pov_pt(f,p2);
  fprintf(f,"}\n");
 }
/* Affichage des extremites d'une grille triangulaire */
/* je = (j0,j1,j2) */
void pov_triangle_li(FILE *f, MATH lp, L_INT je)
 {
  POINT p0,p1,p2;
  p0 = pt_col(lp,je.c[0]);
  p1 = pt_col(lp,je.c[1]);
  p2 = pt_col(lp,je.c[2]);
  pov_triangle(f,p0,p1,p2);
  destru_pt(p0);
  destru_pt(p1);
  destru_pt(p2);
 }
/* Affichage des extremites d'une grille quadrangulaire */
/* (j0,j1,j2,j3) -> tri(j0,j1,j3) U tri(j1,j2,j3) */
void pov_quadrilatere_li(FILE *f, MATH lp, L_INT je)
 {
  POINT p0,p1,p2,p3;
  p0 = pt_col(lp,je.c[0]);
  p1 = pt_col(lp,je.c[1]);
  p2 = pt_col(lp,je.c[2]);
  p3 = pt_col(lp,je.c[3]);
  pov_triangle(f,p0,p1,p3);
  pov_triangle(f,p1,p2,p3);
  destru_pt(p0);
  destru_pt(p1);
  destru_pt(p2);
  destru_pt(p3);
 }

/* Affichage d'un ellipsoide (sphere transformee) */ 
/*
void pov_ellipsoide(FILE *f, MATH t)
 {
  fprintf(f,"   object{");
  fprintf(f,"sphere{<0,0,0>,1}\n");
  pov_math(f,t);
  fprintf(f,"}\n");
 }
*/
/* Affichage d'un parallelepipede (cube transforme) */ 
/*
void pov_parallelepipede(FILE *f, MATH t)
 {
  fprintf(f,"   object{");
  fprintf(f,"box{<0,0,0>,<1,1,1>}\n");
  pov_math(f,lp);
  fprintf(f,"}\n");
 }
*/

/* Figure de base */
void pov_fig0(FILE *f, FIGIT fig)
 {
  MATH proj;
  L_INT je;
  proj = (fig.gifs).proj;
  je = lie_gifs(fig.gifs);
  switch(fig.motif)
    {
     case Point :
      {
       pov_ptm_li(f,proj,je);
       break;
      }
     case Triangle :
      {
       pov_triangle_li(f,proj,je);
       break;
      }
     case Quadrilatere :
      {
       pov_quadrilatere_li(f,proj,je);
       break;
      }
    default:
      fprintf(stderr, "Erreur %s:%d\n", __FILE__, __LINE__) ;
    }
  destru_li(je); 
 }
/*
     case Sphere :
      {
       pov_ellipsoide(f,proj);
       break;
      }
     case Cube :
      {
       pov_parallelepipede(f,proj);
       break;
      }
*/

/*-----------------*/ 
/* Figures iterees */ 
/*-----------------*/ 

/* Affichage des points de controle */
void pov_pc(FILE *f, FIGIT fig)
 {
  fprintf(f," object{\n  union{\n");
  pov_ptm_math(f,(fig.gifs).proj);
  fprintf(f,"  }\n");
  fprintf(f,"  pigment{");
  pov_coul(f,rouge());
  fprintf(f,"}\n }\n");
 }

/* Appel (avec normalisation) */
void pov_determ(FILE *f, FIGIT fig)
 {
  FIGIT figv;
  figv = fig_normal(fig); 
  /*
  figv.gifs = gifs_normal(fig.gifs); 
  figv.iter = fig.iter;
  figv.motif = fig.motif;
  figv.couleur = fig.couleur;
  */
  fprintf(f," object{\n  union{\n");
  pov_determ_iter(f,figv);
  fprintf(f,"  }\n");
  fprintf(f,"  pigment{");
  pov_coul(f,figv.couleur);
  fprintf(f,"}\n }\n");
 }

/* Noyau recursif */
void pov_determ_iter(FILE *f, FIGIT fig)
 {
  int n,i;
  FIGIT figi;
  MATH pass;
  ITER iter;
  pass = (fig.gifs).proj;
  iter = fig.iter;
  n = nb_gifs(fig.gifs);
  if(cond_iter(iter,pass))
    {
     iter = iter_decr(iter);
     for(i=0;i<n;i++)
       {
        figi.gifs = gifs_it(i,fig.gifs);
	figi.iter = iter;
	figi.motif = fig.motif;
        pov_determ_iter(f,figi);
       }
    }
  else pov_fig0(f,fig);
 }

/*--------------*/
/* Bibliotheque */
/*--------------*/
