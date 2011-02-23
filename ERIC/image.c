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
#include "image.h"

/***************************/
/* GESTION TABLEAUX IMAGES */
/***************************/

/* Constantes globales
   larg, haut : largeur et hauteur image,
   c rapport 255/me, ic valeur entiere de c;
*/
/* variables de rendu 
   char cari[] = " .:;-!+*%#" ;
   Flottant c; int ic;           
*/
   
/*--------------------------*/
/* Operations informatiques */
/*--------------------------*/

/* Creation d'un tableau image (voir matrice.c) */
TIMA tima_crea(int larg, int haut)
 {
  return math_crea(larg-1,haut-1);
 }
/* destruction d'un tableau image (voir matrice.c) */
void destru_tima(TIMA tima)
 {
  destru_math(tima);
 }
/* Ecriture du tableau (format) */
void ecrit_tima(FILE *fich, char *nomtima, TIMA tima)
 {
  fprintf(fich,nomtima);
  fprintf(fich," = tableau-image [0,%d] x [0,%d]",tima.m.dim,tima.n.dim);
  fprintf(fich,"\n");
 }

/* Ouverture de fichier.pgm */
FILE *deb_pgm(MOT iprod, int larg, int haut)
 {
  FILE *f;
  MOT idf;
  idf = conca(iprod,mot(4,".pgm"));
  ecrit_mot(stdout,"  * Image.pgm",idf);
  f = fopen(idf.c,"w");
  fprintf(f, "P5\n%d %d\n255\n", larg, haut);
  return(f);
 }
/* Fermeture de fichier.pgm */
void fin_pgm(FILE *f)
 {
  fclose(f);
  fprintf(stdout,"    image.pgm terminee\n");
 }
/* Ouverture de fichier.ppm */
FILE *deb_ppm(MOT iprod, int larg, int haut)
 {
  FILE *f;
  MOT idf;
  idf = conca(iprod,mot(4,".ppm"));
  ecrit_mot(stdout,"  * Image.ppm",idf);
  f = fopen(idf.c, "w") ;
  fprintf(f, "P6\n%d %d\n255\n",larg,haut);
  return(f);
 }
/* Fermeture de fichier.ppm */
void fin_ppm(FILE *f)
 {
  fclose(f);
  fprintf(stdout,"    image.ppm terminee\n");
 }

/* Affichage de l'image.pgm d'une production */
void xv_pgm(MOT iprod)
 {
  system(conca3(mot(3,"xv "),iprod,mot(6,".pgm &")).c);
 }
/* Affichage de l'image.ppm d'une production */
void xv_ppm(MOT iprod)
 {
  system(conca3(mot(3,"xv "),iprod,mot(6,".ppm &")).c);
 }
/* Affichage d'une image */
void xv(MOT ifima)
 {
  system(conca3(mot(3,"xv "),ifima,mot(2," &")).c);
 }

/*------------*/
/* Operations */ 
/*------------*/

/*----------------*/
/* Tableaux-image */
/*----------------*/

/* Dimension */
/* 
   Matrice correspondant a : 
           hauteur 
          0------>y
          |
  largeur |
          |
          V
          x
*/
int larg_tima(TIMA tima)
 {
  return tima.m.dim;
 }
int haut_tima(TIMA tima)
 {
  return tima.n.dim;
 }

/* Remise a zero */
void raz_tima(TIMA tima)
 {
  int larg,haut,i,j;
  larg = larg_tima(tima);
  haut = haut_tima(tima);
  for(j=0;j<haut;j++)
    {
     for(i=0;i<larg;i++)
        tima.c[i][j] = 0.;
    }
 }

/* Tableau-image nul */
TIMA tima_nul(int larg, int haut)
 {
  TIMA tima;
  tima = tima_crea(larg,haut);
  raz_tima(tima);
  return tima;
 }  

/* Ajout d'un point dans le tableau-image */
void tima_xy(TIMA tima, Flottant x, Flottant y)
 {
  int larg,haut,i,j;
  larg = larg_tima(tima);
  haut =  haut_tima(tima);
  i = (int) x;
  j = (int) y;
  if((0<=i)&&(i<larg)&&(0<=j)&&(j<haut))
     tima.c[i][j] = VMAX;
  return;
 }

/* Affichage d'un carre de centre (x,y) et de cote c */
void tima_carre(TIMA tima, Flottant x, Flottant y, Flottant c)
 {
  Flottant d;
  int larg,haut,i0,j0,i1,j1,i,j;
  larg = larg_tima(tima);
  haut =  haut_tima(tima);
  d = 0.5*c;
  i0 = int_max((int)x-d,0);
  i1 = int_min((int)x+d,larg-1);
  j0 = int_max((int)y-d,0);
  j1 = int_min((int)y+d,haut-1);
  if((i0<=i1)&&(j0<=j1))
    {
     for(i=i0;i<=i1;i++)
       {
        for(j=j0;j<=j1;j++)
          tima.c[i][j] = VMAX;
       }
    }
  return;
 }
/* Ajout d'un point avec marqueur dans le tableau-image */
void tima_xym(TIMA tima, Flottant x, Flottant y)
 {
  tima_carre(tima,x,y,tima.m.dim/50.);
 }
/* Ajout d'un point avec marqueur 3 pixels */
void tima_xy3p(TIMA tima, Flottant x, Flottant y)
 {
  tima_carre(tima,x,y,1.4);
 }

/*----------------*/
/* Fichiers-image */
/*----------------*/

/* Conversion d'un tableau image {0,...,m-1} x {0,..,n-1} de reels 
   en un fichier image.pgm */
void pgm_tima(MOT iprod, GAMME gam, TIMA tima)
 {
  FILE *fichima;
  int larg,haut;
  int i,j;
  larg = larg_tima(tima);
  haut = haut_tima(tima);
 /*
  gam.c = 255./(gam.r1-gam.r0);
  gam.ic = (int) gam.c;
 */
  fichima = deb_pgm(iprod, larg, haut);
    for(j=haut-1;j>=0;j--)
      {
       for(i=0;i<larg;i++)
          fprintf(fichima,"%c",gris_degr(gam,tima.c[i][j]));
      }
  fin_pgm(fichima);
 }

/* Conversion d'un tableau image {0,...,m-1} x {0,..,n-1} de reels 
   en un fichier image.ppm */
void ppm_tima(MOT iprod, GAMME gam, TIMA tima)
 {
  FILE *fichima;
  int larg,haut,i,j;
  int ir,iv,ib;
  RVB coul,coul0,coul1;
  Flottant a,b;
  larg = larg_tima(tima);
  haut = haut_tima(tima);
  coul0 = gam.coul0;
  coul1 = gam.coul1;
  a = 1/(gam.r1-gam.r0);
  b = -a*gam.r0;
  fichima = deb_ppm(iprod, larg, haut);
  /*
  ifich = conca(iprod,mot(4,".ppm"));
   ecrit_mot(stdout,"  Image",ifich);
  fichima = fopen(ifich.c, "w") ;
  fprintf(fichima, "P6\n%d %d\n255\n",larg,haut);
  */
  for(j=0;j<haut;j++)
    {
     for(i=0;i<larg;i++)
       {
        coul = coul_caf(coul0,coul1,a*tima.c[i][j]+b);
	ir = (int) r_coul(coul);
	iv = (int) v_coul(coul);
	ib = (int) b_coul(coul);
        fprintf(fichima,"%c%c%c",ir,iv,ib);
       }
    }
  fin_ppm(fichima);
  /*
  fclose(fichima);
   ecrit_mess(stdout,"  image.ppm terminee");
  */
 }

/*--------------------------------------*/
/* Relation avec espace de modelisation */
/*--------------------------------------*/

/* Echelle : largeurs image et fenetre --> projection de fenetre centree */
MATH echel_larg(TIMA tima, Flottant lfen)
 {
  int larg,haut;
  larg = larg_tima(tima);
  haut = haut_tima(tima);
  return math_mul(tr2(0.5*larg,0.5*haut),ho2(larg/lfen));
 }
/* Echelle inverse :
   largeurs image et fenetre --> plongement de fenetre centree */
MATH echel_inv_larg(TIMA tima, Flottant lfen)
 {
  int larg,haut;
  larg = larg_tima(tima);
  haut = haut_tima(tima);
  return math_mul(ho2(lfen/larg),tr2(-0.5*larg,-0.5*haut));
 }

/*-----------------------*/
/* Bibliotheque d'images */
/*-----------------------*/

/* Formats de sortie */
TIMA tima_sortie(SORTIE type)
 {
  TIMA tima;
  switch(type)
    {
     case Article :
       {
        tima = tima_crea(300,250);
	break;
       }
     case ImprA4 :
       {
        tima = tima_crea(210*RESOL,297*RESOL);
	break;
       }
     case Diapo_essai :
       {
        tima = tima_crea(2040/2,1354/2);
	break;
       }
     case Diapo_h :
       {
        tima = tima_crea(4060,2708);
	break;
       }
     case Diapo :
       {
        tima = tima_crea(2040,1354);
	break;
       }
    }
  return tima;
 }
