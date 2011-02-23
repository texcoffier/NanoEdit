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
#include <stdlib.h>
#include <math.h>
#include "util.h"
#include "point.h"
#include "matrice.h"
#include "controle.h"
#include "ifs.h"
#include "gifs.h"
#include "regle.h"
#include "graphisme.h"
#include "figure.h"
#include "image.h"
#include "echap.h"
#include "determ.h"
#include "pov.h"
#include "prod.h"

/******************************************/
/* PRODUCTION D'IMAGES ET DE DESCRIPTIONS */
/******************************************/

/*--------------------------*/
/* Operations informatiques */
/*--------------------------*/

/* Debut de production */
MOT deb_prod(MOT ir, char *nom)
 {
  fprintf(stdout,"\nProduction = %s \n",nom);
  return conca(ir,mot_nom(nom));
 }
/* Fin de production */
void fin_prod(MOT iprod)
 {
  char *cprod;
  cprod = iprod.c;
  fprintf(stdout,"Fin de production : %s\n",iprod.c);
 }
/* Ouverture de fichier.desc */
FILE *deb_desc(MOT iprod)
 {
  FILE *f;
  MOT idf;
  idf = conca(iprod,mot(5,".desc"));
  ecrit_mot(stdout,"  * Description",idf);
  f = fopen(idf.c,"w");
  return(f);
 }
/* Fermeture de fichier.desc */
void fin_desc(FILE *f)
 {
  fclose(f);
  fprintf(stdout,"    fin de description\n");
 } 
/* Ouverture de fichier.tex */
FILE *deb_tex(MOT iprod)
 {
  FILE *f;
  MOT idf;
  idf = conca(iprod,mot(4,".tex"));
  ecrit_mot(stdout,"  * Commentaire TeX",idf);
  f = fopen(idf.c,"w");
  return(f);
 }
/* Fermeture de fichier.tex */
void fin_tex(FILE *f)
 {
  fclose(f);
  fprintf(stdout,"    fin de commentaire TeX\n");
 } 

/*-----------------------------*/
/* Bibliotheque de Productions */
/*-----------------------------*/

/*------*/
/* Vues */
/*------*/

/* Image.pgm */
void vue_pgm(MOT iprod, CAMERA cam, FIGIT fig)
 {
  TIMA tima;
  FIGIT figv;
  tima = tima_nul(larg_cam(cam),haut_cam(cam));
    figv = fig_mul(cam.passv,fig);
  /*
    tima_repere_cam(tima,cam);
    tima_pc(tima,figv);
  */
    /*
    tima_lbc(tima,figv);
    */
    tima_figv(tima,figv);
  pgm_tima(iprod,cam.gamme,tima);
  xv_pgm(iprod);
  destru_tima(tima);
 }
/* Scene.pov */
void vue_pov(MOT iprod, CAMERA cam, FIGIT fig, POINT pl)
 {
  FILE *fpov;
  fpov = deb_pov(iprod);
  /*
    pov_pc(fpov,fig);
  */
    pov_determ(fpov,fig);
    pov_lampe(fpov,pl,blanc());
  fin_pov(fpov,cam);
 }
/* Image.tga */
void vue_tga(MOT iprod, CAMERA cam, FIGIT fig, POINT pl)
 {
  vue_pov(iprod,cam,fig,pl);
  tga_pov(iprod,cam);
  destru_pov(iprod);
 }

/*-------------*/
/* Productions */
/*-------------*/

/*--------------------*/
/* Images 2D directes */
/*--------------------*/

/* Image.pgm d'un attracteur */
/* Hypothese : modele 2d ? */
/*
void pgm_attrac_gifs(MOT iprod, float lfen, GAMME gam, GIFS gifs)
 {
  TIMA tima;
  int nit;
   ecrit_mess(stdout," Attracteur");
  nit = 8;
  tima = tima_sortie(Article);
  raz_tima(tima);
    tima_determ(tima,lfen,gifs,nit);
  pgm_tima(iprod,gam,tima);
   xv_pgm(iprod);
  destru_tima(tima);
 }
*/
  /*
  nit = 15;
  nit = 10;
  */
void pgm_attrac_gifs(MOT iprod, float lfen, GAMME gam, GIFS gifs)
 {
  FIGIT fig;
  CAMERA cam;
  fig = fig_n(gifs,11,Point);
  cam = cam_crea(500,500,lfen,math_id(2),math_id(2),gam);
  vue_pgm(iprod,cam,fig);
 }
/* Productions 2D */
void prod_2d(MOT ir, char *nom, IFS ifs2)
 {
  MOT iprod;
  FILE *fd;           
  IFS ifs;
  GIFS gifs;
  float lfen;
  int nit;
  GAMME gam;
  iprod = deb_prod(ir,nom);
   /* description */
    fd = deb_desc(iprod);
   /* modele */
      ecrit_ifs(fd,"IFS 2D",ifs2);
      ifs = ifs_conj(tr2(-0.5,-0.3),ifs2);
      gifs = gifs_ifs(2,ifs);
   /* parametres de visualisation */
      lfen = 1.2;
      ecrit_float(fd,"Largeur fenetre",lfen);
      nit = 7;
      gam.degrade = Discret;
      gam.r0 = 0;
      gam.r1 = nit;
    fin_desc(fd); 
   /* image.pgm */
    pgm_attrac_gifs(iprod, lfen, gam, gifs);
  fin_prod(iprod); 
 }

/*----------------------*/
/* Productions filaires */
/*----------------------*/
void prod_fil(char *nom, SEMIG sem, PC pc, REGLE reg)
 {
  MOT iprod;
  FILE *fd;           
  IFS ifs;
  GIFS gifs;
  float lfen;
  int nit;
  GAMME gam;
  iprod = deb_prod(DIR_FILAIRE,nom);
 /* description */
    fd = deb_desc(iprod);
      fprintf(fd,"PRODUCTION FILAIRE %s",nom);
      ecrit_semig(fd,"Semigroupe d'iteration",sem);
      ecrit_lpt(fd,"Points de controle (internes)",pc);
      ecrit_reg(fd,"Regle filaire",reg);
      ifs = ifs_pc_regle(sem, pc, reg);
      ecrit_ifs(fd,"IFS filaire",ifs);
      /*
      gifs = gifs_proj_ifs(tr2(-0.5,0.1),ifs);
      gifs = gifs_proj_ifs(tr2(-0.489,-0.1),ifs); 
      */
      gifs = gifs_proj_ifs(tr2(-0.5,-0.3),ifs);
   /* parametres de visualisation */
      lfen = 1.2;
      lfen = 1.1;
      ecrit_float(fd,"Largeur fenetre",lfen);
      nit = 10;
      nit = 11;
      nit = 12;
      nit = 13;
      gam = gam_nit(nit);
    fin_desc(fd);
   /* commentaire */
    fd = deb_tex(iprod);
      tex_reg(fd,"R\\`egle filaire",reg);
    fin_tex(fd);
   /* images.pgm */
    pgm_graph_gifs(iprod, lfen, gam, gifs, pc, reg);
    /*
    pgm_attrac_gifs(iprod, lfen, gam, gifs);
    */
  fin_prod(iprod);
 }
/* Visualisation des graphes */
void pgm_graph_gifs(MOT iprod, float lfen, GAMME gam, GIFS gifs, PC pc, REGLE reg)
 {
  MOT iprodg;
  TIMA tima;
  MATH echel,pass,ti,pass_ti;
  MATH *tf;
  IFS ifs;
  IFS *ag;
  L_POINT sg,sdi;
  int nb,nba,nit,i,j;
 /* constantes */
  /*
  tima = tima_sortie(Article);
  */
  tima = tima_crea(500,500);
  echel = echel_larg(tima,lfen);
  pass = math_mul(echel,gifs.proj);
  ifs = gifs.ifs;
  nb = (gifs.ifs).nb;
  tf = (gifs.ifs).tf;
  nba = reg.nba;   
  ag = (IFS *) malloc (nba*sizeof(IFS));
  nit = 10;
 /* graphe gauche */ 
  iprodg = conca(iprod,mot(2,"_g"));
  ecrit_mess(stdout," Graphe gauche");
  raz_tima(tima);
 /* arcs gauche */
  fprintf(stdout,"  - Arcs gauche :");
  for(j=0;j<nba;j++)
    {
     fprintf(stdout," A%d = A",j); 
     ecrit_ind(stdout,"T",reg.iarc[j]); 
     fprintf(stdout,";"); 
     ag[j] = ifs_extr(ifs,reg.iarc[j]);
     tima_determ_pass(tima,pass,ag[j],nit);
    }
 /* sommets gauche */
  fprintf(stdout,"\n  - Sommets gauche :"); 
   fprintf(stdout," Sg = %d",(reg.ipg).nb); 
   ecrit_ind(stdout,"q",reg.ipg); 
   fprintf(stdout,";\n");
   sg = lpt_extr(pc,reg.ipg);
   tima_lpt_pass(tima,pass,sg);
  pgm_tima(iprodg,gam,tima);
  xv_pgm(iprodg);
 /* graphe droite */ 
  iprodg = conca(iprod,mot(2,"_d"));
   ecrit_mess(stdout," Graphe droite");
  raz_tima(tima);
  for(i=0;i<nb;i++)
    {
     ti = tf[i];
     pass_ti = math_mul(pass,ti);
     fprintf(stdout,"  - T%d*Ag =",i); 
   /* arcs droite */
     for(j=0;j<nba;j++)                
       {
	fprintf(stdout," T%d*A%d U",i,j); 
        tima_determ_pass(tima,pass_ti,ag[j],nit);
       }
   /* sommets droite */
     fprintf(stdout,"\n"); 
     fprintf(stdout,"    Sd%d = ",i); 
     ecrit_ind(stdout,"q",reg.ipd[i]); 
     fprintf(stdout,";\n");
     sdi = lpt_extr(pc,reg.ipd[i]);    
     tima_lpt_pass(tima,pass,sdi);  
    }
  pgm_tima(iprodg,gam,tima);
  xv_pgm(iprodg);
  destru_tima(tima);
 }

/*------------------------*/
/* Production de surfaces */
/*------------------------*/

/*
void prod_qt2_abc(MOT inom,ifsa,ifsb,ifsc,q0,q1,q2)
 {
  POINT p0,p1,p2;
  Flottant h;
  p0 = pt2(0,0);
  p1 = pt2(0.5,h);
  p2 = pt2(1,0);
  proj2 = math_pt3(p0,p1,p2);
  iprod = deb_prod(DIR_SURFACE,inom.c);
    vue_pgm(conca(iprod,mot(1,"a")),cam2,figa);
    vue_pgm(conca(iprod,mot(1,"b")),cam2,figb);
    vue_pgm(conca(iprod,mot(1,"c")),cam2,figc);
    vue_pgm(iprod,cam,fig);
  fin_prod(iprod);
 }
*/
