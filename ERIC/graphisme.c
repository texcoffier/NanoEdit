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
#include "graphisme.h"

/**********************/
/* GESTION GRAPHISMES */
/**********************/

/* variables de rendu 
   char cari[] = " .:;-!+*%#" ;
   Flottant c; int ic;           
*/

/*------------------*/
/* Espace graphique */
/*------------------*/

/* creation d'un espace graphique */
ESP_GRAPH eg_crea(int dims, int dimc, int dimn)
 {
  ESP_GRAPH egr;
  egr.dims = dims;
  egr.dimc = dimc;
  egr.nc = dims + dimc;
  egr.dimn = dimn;
  return egr;
 }
/* ecriture d'un espace graphique */
void ecrit_eg(FILE *fich, char *nom, ESP_GRAPH eg)
 {
  fprintf(fich,"  %s = espace graphique : \n",nom);
  fprintf(fich,"   - espace support %d D",eg.dims);
  fprintf(fich,"   - espace des couleurs %d D : ",eg.dimc);
  if(eg.dimc == 3)
    fprintf(fich,"RVB \n");
  else if(eg.dimc == 1)
    fprintf(fich,"niveau de gris \n");
  else if(eg.dimc == 0)
    fprintf(fich,"Noir et Blanc \n");
  else   
    fprintf(fich,"INCONNU \n");
  fprintf(fich,"   - espace des normales : ");
  if(eg.dimc == 3)
    fprintf(fich,"normales definies \n");
  else if(eg.dimc == 0)
    fprintf(fich,"normales non definies \n");
  else   
    fprintf(fich,"INCONNU \n");
 }

/* dimensions */
int dims_eg(ESP_GRAPH eg)
 {
  return eg.dims;
 }
int dimc_eg(ESP_GRAPH eg)
 {
  return eg.dimc;
 }
int dimn_eg(ESP_GRAPH eg)
 {
  return eg.dimn;
 }
int nc_eg(ESP_GRAPH eg)
 {
  return eg.nc;
 }

/* Bilbiotheque */

ESP_GRAPH eg_2d_coul()
 { 
  return eg_crea(2,3,0);
 }
ESP_GRAPH eg_3d()
 { 
  return eg_crea(3,0,0);
 }
ESP_GRAPH eg_3d_gris()
 { 
  return eg_crea(3,1,0);
 }
ESP_GRAPH eg_3d_coul()
 { 
  return eg_crea(3,3,0);
 }
ESP_GRAPH eg_3d_coul_norm()
 { 
  return eg_crea(3,3,3);
 }

/*----------*/
/* Couleurs */
/*----------*/

/* Creation couleur */
/*
RVB coul_rvb(Flottant r, Flottant v, Flottant b)
 {
  RVB coulr;
  coulr.r = r;
  coulr.v = v;
  coulr.b = b;
  return coulr;
 }
*/
RVB coul_rvb(Flottant r, Flottant v, Flottant b)
 {
  return pt(r,v,b);
 }
/* Ecriture d'une couleur */
void ecrit_coul(FILE *fich, char *nom, RVB coul)
 {
  fprintf(fich,"%s = couleur RVB (%f,%f,%f)\n",nom,
  r_coul(coul),v_coul(coul),b_coul(coul));
 }
/* Duplication d'une couleur */
RVB coul_dupli(RVB coul)
  {
   return pt_dupli(coul);
  }
/* Composantes */
Flottant r_coul(RVB coul)
  {
   return coul.c[1];
  }
Flottant v_coul(RVB coul)
  {
   return coul.c[2];
  }
Flottant b_coul(RVB coul)
  {
   return coul.c[3];
  }

/* Combinaison convexe de couleurs */
/*
RVB coul_caf(RVB coul0, RVB coul1, Flottant t)
 {
  RVB coulr;
  Flottant t0;
  t0 = 1-t;
  coulr.r = t0*coul0.r + t*coul1.r; 
  coulr.v = t0*coul0.v + t*coul1.v; 
  coulr.b = t0*coul0.b + t*coul1.b; 
  return coulr;
 }
*/
RVB coul_caf(RVB coul0, RVB coul1, Flottant t)
 {
  return pt_caf(coul0,coul1,t);
 }

/* Bibliotheque de couleurs */
RVB noir()
 {
  return coul_rvb(0,0,0);
 }
RVB blanc()
 {
  return coul_rvb(CMAX,CMAX,CMAX);
 }
RVB rouge()
 {
  return coul_rvb(CMAX,0,0);
 }
RVB vert()
 {
  return coul_rvb(0,CMAX,0);
 }
RVB bleu()
 {
  return coul_rvb(0,0,CMAX);
 }
RVB jaune()
 {
  return coul_rvb(CMAX,CMAX,0);
 }
RVB violet()
 {
  return coul_rvb(CMAX,0,CMAX);
 }

/*--------*/
/* Gammes */
/*--------*/

/* Operations informatiques */
/* creation d'une gamme */
GAMME gam_crea(DEGRADE degr, RVB coul0, RVB coul1, Flottant r0, Flottant r1)
 {
  GAMME gam;
  gam.degrade = degr;
  gam.coul0 = coul0;
  gam.coul1 = coul1;
  gam.r0 = r0;
  gam.r1 = r1;
  gam.c = 255./(gam.r1-gam.r0);
  gam.ic = (int) gam.c;
  return gam;
 }
/* destruction d'une gamme */
void destru_gam(GAMME gam)
 {
   /*
  free(gam.degrade);
  free(gam.coul0);
  free(gam.coul1);
  free(gam.r0);
  free(gam.r1);
  free(gam.c);
  free(gam.ic);
   */
 }
/* ecriture d'un degrade */
void ecrit_degrad(FILE *fich, char *nom, DEGRADE degrad)
 {
  char *nomd;
  switch(degrad)
    {
     case Continu :
      {
       nomd = "Continu";
       break;
      }
     case Discret : 
      {
       nomd = "Discret";
       break;
      }
     case Alterne : 
      {
       nomd = "Alterne";
       break;
      }
     case Moire : 
      {
       nomd = "Moire";
       break;
      }
     case Sans : 
      {
       nomd = "Sans";
       break;
      }
    }
  fprintf(fich,"%s = %s\n",nom,nomd);
 }
/* ecriture d'une gamme */
void ecrit_gam(FILE *fich, char *nomgam, GAMME gam)
 {
  fprintf(fich,nomgam);
  fprintf(fich," = gamme de couleurs :\n");
  ecrit_degrad(fich,"    - type de degrade", gam.degrade);
  ecrit_coul(fich,"    - couleur initiale", gam.coul0);
  ecrit_coul(fich,"    - couleur finale", gam.coul1);
  fprintf(fich,"    - valeur initiale %f,", gam.r0);
  fprintf(fich," valeur finale %f\n", gam.r1);
  fprintf(fich,"    - coefficient de degrade : %f, %d", gam.c, gam.ic);
  fprintf(fich,"\n");
 }

/* Gamme par defaut */
GAMME gam_nit(Flottant n)
 {
  return gam_crea(Continu,blanc(),noir(),0,n);
 }

/* Fonction de degrade de gris : 
   valeur reelle -> niveau de gris (entre 0 et 255) */
int gris_degr(GAMME gam, Flottant vr)
 {
  int k,ic;
  Flottant c;
  c = gam.c;
  ic = gam.ic;
	switch(gam.degrade)
	  {
	   case Continu :
	     {
	      k = (int)(c*vr);
	      break;
	     }
           case Discret :
	     {
	      k = (int)(vr+0.99);
	      k = ic*k;
	      break;
	     }
	   case Moire :
	     {
	      k = (int)(c*vr+0.99);
	      k = ic*k;
	      break;
	     }
           case Alterne :
	     {
	      k = (int)(vr+0.99);
	      k = ic*k;
	      if(impair(k)) k = 0;
	      else k = 255;
	      break;
	     }
           case Sans :
	     {
	      k = (int)(c*vr);
	      if(k<255) k = 0; 
	      break;
	     }
	  }
  return (255-k);
 }

/*---------*/
/* Cameras */
/*---------*/

/* Creation d'une camera */
/* tm : R^3 -> R^3 et tvm : R^3 -> R^2 */ 
CAMERA cam_crea(int larg, int haut, Flottant lfen, 
		MATH tvm, MATH tm, GAMME gamme)
 {
  CAMERA camr;
  MATH tech,tim;
  camr.larg = larg;
  camr.haut = haut;
  camr.lfen = lfen;
  camr.passm = math_inverse(tm);
  tim = math_mul(tvm,camr.passm);
  tech = math_mul(tr2(0.5*larg,0.5*haut),ho2(larg/lfen));
  camr.passv = math_mul(tech,tim);
  camr.gamme = gamme;
  destru_math(tim);
  destru_math(tech);
  return camr;
 }
/* Ecriture d'une camera */
void ecrit_cam(FILE *f, char *nom, CAMERA cam)
 {
  fprintf(f,"%s : Camera\n", nom);
  fprintf(f,"  - Format d'image : %d x %d\n",cam.larg, cam.haut);
  fprintf(f,"  - Fenetre : %f\n",cam.lfen);
  ecrit_math(f,"  - Passage scene -> image",cam.passv);
  ecrit_math(f,"  - Passage scene -> camera",cam.passm);
  ecrit_gam(f,"  - Gamme",cam.gamme);
  fprintf(f,"\n");
 }
/* Formats associes */
int larg_cam(CAMERA cam)
 {
  return cam.larg;
 }
int haut_cam(CAMERA cam)
 {
  return cam.haut;
 }
Flottant lfen_cam(CAMERA cam)
 {
  return cam.lfen;
 }
/* camera : modele 2d -> image 2d */
CAMERA cam_2d(int larg, int haut, Flottant lfen)
  {
   return cam_crea(larg,haut,lfen,math_id(2),math_id(2),gam_nit(10));
  }
/* camera : modele 3d vertical yOz -> image 2d xOy */
CAMERA cam_3d(int larg, int haut, Flottant lfen)
  {
   return cam_crea(larg,haut,lfen,proj_yz(),math_id(3),gam_nit(10));
  }
