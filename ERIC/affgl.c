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
#include "affgl.h"

/****************************/
/* Affichage en GL          */
/****************************/

/*------------*/
/* Parametres */
/*------------*/

/* sommet donne par un point (cartesien) */
void glVertex3f_pt(POINT q) 
 {
  return glVertex3f(q.c[1],q.c[2],q.c[3]); 
 }
/* couleur */
void gl_couleur(RVB coul) 
 {
  return glColor3f(r_coul(coul),v_coul(coul),b_coul(coul)); 
 }
/* normale donnee par un vecteur (cartesien) */
/*
void glNormal3f_vec(VECTEUR v)
 {
  return glNormal3f(v.c[1],v.c[1],v.c[2]);
 }
*/

/*----------------------*/
/* Figures elementaires */
/*----------------------*/

/* affichage d'un point en gl */
void gl_point(ESP_GRAPH eg, POINT p0)
 {
  int ns,nc;
  ns = dims_eg(eg);
  nc = nc_eg(eg);
  if (p0.c[0] == 0) /* point a l'infini */
     return;
  else
    {
     if(nc > ns)
        glColor3f(p0.c[ns+1],p0.c[ns+2],p0.c[ns+3]); 
     glVertex3f(p0.c[1],p0.c[2],p0.c[3]);
     return;
    }
 }
/* affichage d'un segment en gl */
void gl_segment(ESP_GRAPH eg, POINT p0, POINT p1)
 {
  gl_point(eg,p0);
  gl_point(eg,p1);
 }
/* affichage d'un coin de face dans un espace graphique */
void gl_coin_face(ESP_GRAPH eg, POINT p)
 {
  int ns,nc;
  ns = dims_eg(eg);
  nc = nc_eg(eg);
  if(nc > ns)
     glColor3f(p.c[ns+1],p.c[ns+2],p.c[ns+3]); 
  if(dimn_eg(eg) == 3)
     glNormal3f(p.c[nc+1],p.c[nc+2],p.c[nc+3]);
  glVertex3f(p.c[1],p.c[2],p.c[3]); 
 }
/* affichage d'un triangle */
void gl_triangle(ESP_GRAPH eg, POINT p0, POINT p1, POINT p2)
 {
  gl_coin_face(eg,p0);
  gl_coin_face(eg,p1);
  gl_coin_face(eg,p2);
 }
/* affichage d'un quadrilatere */
void gl_quadrilatere(ESP_GRAPH eg, POINT p0, POINT p1, POINT p2, POINT p3)
 {
  gl_coin_face(eg,p0);
  gl_coin_face(eg,p1);
  gl_coin_face(eg,p2);
  gl_coin_face(eg,p3);
 }

/*-----------------*/
/* Figures iterees */
/*-----------------*/

/* Affichage d'une figure iteree */
void gl_determ(FIGIT fig)
 {
  FIGIT figv;
  figv = fig_normal(fig);
  gl_couleur(coul_fig(figv));
  ecrit_fig(stdout," figv",figv);
  switch (figv.motif)
	 {
          case Point :
	    {
	     glBegin(GL_POINTS);
             gl_couleur(coul_fig(figv)); 
	     break;
	    }
	  case Segment :
	    {
             glBegin(GL_LINES);
             gl_couleur(coul_fig(figv)); 
	     break;
	    }
	  case Triangle :
	    {
             glEnable(GL_LIGHTING);
             glEnable(GL_CULL_FACE);
             glCullFace(GL_BACK);
	     glBegin(GL_TRIANGLES);
             gl_couleur(coul_fig(figv)); 
	     break;
	    }
	  case Quadrilatere :
	    {
             glEnable(GL_LIGHTING);
             glEnable(GL_CULL_FACE);
             glCullFace(GL_BACK);
	     glBegin(GL_QUADS);
	     break;
	    }
         }
  gl_determ_iter(figv);
  switch (figv.motif)
	 {
          case Point :
          case Segment :
	    {
	     glEnd();
             break; 
	    }
	  case Triangle :
	  case Quadrilatere :
	    {
	     glEnd();
	     glDisable(GL_CULL_FACE);
	     glDisable(GL_LIGHTING);
             break; 
	    }
         }
 }
/* Figure de base pour GL */
void gl_fig0(FIGIT fig)
 {
  MATH proj;
  L_POINT lp;
  ESP_GRAPH eg;
  L_INT je;
  je = lie_gifs(fig.gifs);
  eg = eg_fig(fig);
  proj = (fig.gifs).proj;
  /*
  lp = lpt_math_fig(proj,fig);
  */
  switch(fig.motif)
    {
     case Point :
      {
       lp = lpt_point(eg,proj,je);
       gl_point(eg,lp.pt[0]);
       break;
      }
     case Segment :
      {
       lp = lpt_segment(eg,proj,je);
       /*
       ecrit_lpt(stdout," lpt_segment",lp);
       */
       gl_segment(eg,lp.pt[0],lp.pt[1]);
       fprintf(stdout,"  gl_segment"); 
       break;
      }
     case Triangle :
      {
       lp = lpt_triangle(eg,proj,je);
       gl_triangle(eg,lp.pt[0],lp.pt[1],lp.pt[2]);
       break;
      }
     case Quadrilatere :
      {
       lp = lpt_quadrilatere(eg,proj,je);
       gl_quadrilatere(eg,lp.pt[0],lp.pt[1],lp.pt[2],lp.pt[3]);
       break;
      }
    default:
      fprintf(stderr, "Erreur %s:%d\n", __FILE__, __LINE__) ;
    }
  /*destru_li(je); */
  destru_lpt(lp); 
 }
/* noyau recursif */
void gl_determ_iter(FIGIT fig)
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
        gl_determ_iter(figi);
        destru_math(figi.gifs.proj);
        destru_li(figi.gifs.extrem);
       }
    }
  else gl_fig0(fig);
 }
