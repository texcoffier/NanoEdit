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
/* #include <math.h> */
#include "util.h" 
#include "point.h"
#include "matrice.h"
#include "controle.h"
#include "ifs.h"

/**************************/
/* OPERATIONS SUR LES IFS */
/**************************/

/*--------------------------*/
/* Operations informatiques */
/*--------------------------*/

/* creation d'ifs */
IFS ifs_crea(int n)
 {
  IFS ifsr;
  ifsr.nb = n;
  ifsr.tf = (MATH *) malloc (n * sizeof(MATH));
  return(ifsr);
 }
/* destruction d'ifs (PRECAUTIONS D'EMPLOI) */
void destru_ifs(IFS ifs)
 {
  free (ifs.tf);
 }
/* duplication d'ifs */
IFS ifs_dupli(IFS ifs)
 {
  IFS ifsr;
  int i;
  ifsr = ifs_crea(ifs.nb); 
  for(i=0;i<ifsr.nb;i++)
    ifsr.tf[i] = math_dupli(ifs.tf[i]);
  return(ifsr);
 }
/* Lecture d'un IFS */
/*
IFS ifs_lect(char *nom)
 {
  IFS ifsr;
  FILE *f;
  int nr,i;
  f = fopen(nom,"r");
  nr = int_lect(f);
  ifsr = ifs_crea(nr);
  for(i=0;i<nr;i++)
    ifsr.tf[i] = math_lect(f);
  fclose(f);
  return(ifsr);
 }
*/
/* Ecriture d'un IFS */
void ecrit_ifs(FILE *f, char *nomifs, IFS ifs)
 {
  int n,i;
  char *ti;
  ti = (char *) malloc (6*sizeof(char)); 
  n = ifs.nb;
  fprintf(f,nomifs);
  fprintf(f," = IFS a %d elements\n {\n",n);
  for(i=0;i<n;i++)
    {
     sprintf(ti,"%s%d"," T",i);
     ecrit_math(f,ti,ifs.tf[i]);
    }
  fprintf(f," }\n");
  free(ti);
 }

/*--------------------*/
/* Operations 0-aires */
/*--------------------*/

/* les dimensions sont censees etre coherentes :
   m0 = n0 = m1 = n1 = ... */

IFS ifs_math1(MATH t0)
 {
  IFS ifsr;
  ifsr = ifs_crea(1);
  ifsr.tf[0] = math_dupli(t0);
  return(ifsr);
 }

IFS ifs_math2(MATH t0, MATH t1)
 {
  IFS ifsr;
  ifsr = ifs_crea(2);
  ifsr.tf[0] = math_dupli(t0);
  ifsr.tf[1] = math_dupli(t1);
  return(ifsr);
 }

IFS ifs_math3(MATH t0, MATH t1, MATH t2)
 {
  IFS ifsr;
  ifsr = ifs_crea(3);
  ifsr.tf[0] = math_dupli(t0);
  ifsr.tf[1] = math_dupli(t1);
  ifsr.tf[2] = math_dupli(t2);
  return(ifsr);
 }

IFS ifs_math4(MATH t0, MATH t1, MATH t2, MATH t3)
 {
  IFS ifsr;
  ifsr = ifs_crea(4);
  ifsr.tf[0] = math_dupli(t0);
  ifsr.tf[1] = math_dupli(t1);
  ifsr.tf[2] = math_dupli(t2);
  ifsr.tf[3] = math_dupli(t3);
  return(ifsr);
 }

IFS ifs_math5(MATH t0, MATH t1, MATH t2, MATH t3, MATH t4)
 {
  IFS ifsr;
  ifsr = ifs_crea(5);
  ifsr.tf[0] = math_dupli(t0);
  ifsr.tf[1] = math_dupli(t1);
  ifsr.tf[2] = math_dupli(t2);
  ifsr.tf[3] = math_dupli(t3);
  ifsr.tf[4] = math_dupli(t4);
  return(ifsr);
 }

/*--------------------*/
/* Operations unaires */
/*--------------------*/

/* Espace d'iteration de l'IFS */
/* condition : matrices supposees carrees et de meme espace */
t_espace esp_ifs(IFS ifs)
 {
  return espa_math(ifs.tf[0]);
 }
int dim_ifs(IFS ifs)
 {
  return int_max(dima_math(ifs.tf[0]),dimd_math(ifs.tf[0]));
 }
/* Nombre d'elements */
int nb_ifs(IFS ifs)
 {
  return ifs.nb;
 }
/* Liste des centres (points fixes) des operateurs */
L_POINT centre_ifs(IFS ifs)
 {
  L_POINT lp;
  int nb,i;
  nb = ifs.nb;
  lp = lpt_crea(nb);
  for(i=0;i<nb;i++)
    lp.pt[i] = centre_math(ifs.tf[i]);
  return lp;
 }

/* Extension : ajout d'une math */
IFS ifs_ajout(IFS ifs1, MATH t)
 {
  IFS ifsr;
  int nr,i;
  nr = ifs1.nb + 1;
  ifsr = ifs_crea(nr);
  for(i=0;i<nr-1;i++)
    ifsr.tf[i] = math_dupli(ifs1.tf[i]);
  ifsr.tf[nr-1] = t;
  return(ifsr);
 }
/* Restriction : enlevement de la transformation numero k */
IFS ifs_enlev(IFS ifs1, int k)
 {
  IFS ifsr;
  int nr,i;
  nr = ifs1.nb - 1;
  ifsr = ifs_crea(nr);
  for(i=0;i<k;i++)
    ifsr.tf[i] = math_dupli(ifs1.tf[i]);
  for(i=k;i<nr;i++)
    ifsr.tf[i] = math_dupli(ifs1.tf[i+1]);
  return(ifsr);
 }
/* Extraction d'un sous-IFS */
/* condition : ind.nb <= ifs1.nb */
IFS ifs_extr(IFS ifs1, L_INT ind)
 {
  IFS ifsr;
  int nr,i;
  nr = ind.nb;
  ifsr = ifs_crea(nr);
  for(i=0;i<nr;i++)
    ifsr.tf[i] = math_dupli(ifs1.tf[ind.c[i]]);
  return(ifsr);
 }

/* Inversion des transformations d'un IFS */
IFS ifs_inverse(IFS ifs1)
 {
  IFS ifsr;
  int nr,i;
  nr = ifs1.nb;
  ifsr = ifs_crea(nr);
  for(i=0;i<nr;i++)
    ifsr.tf[i] = math_inverse(ifs1.tf[i]);
  return(ifsr);
 }
/* Conjugaison d'IFS : transformation d'attracteur */
IFS ifs_conj(MATH t, IFS ifs1)
 {
  IFS ifsr;
  int nr,i;
  nr = ifs1.nb;
  ifsr = ifs_crea(nr);
  for(i=0;i<nr;i++)
    ifsr.tf[i] = math_conj(t,ifs1.tf[i]);
  return(ifsr);
 }
/* Multiplication a droite par une transfo */
IFS ifs_mul_math(IFS ifs1, MATH t2)
 {
  IFS ifsr;
  int n1,i;
  n1 = ifs1.nb;
  ifsr = ifs_crea(n1);
  for(i=0;i<n1;i++)
    {
     ifsr.tf[i] = math_mul(ifs1.tf[i],t2);
    }
  return(ifsr);
 }

/*---------------------*/
/* Operations binaires */
/*---------------------*/

/* Melange : concatenation des IFS et melange des langages */
IFS ifs_mel(IFS ifs1, IFS ifs2)
 {
  IFS ifsr;
  int n1,nr,i;
  n1 = ifs1.nb;
  nr = n1 + ifs2.nb;
  ifsr = ifs_crea(nr);
  for(i=0;i<n1;i++)
    ifsr.tf[i] = math_dupli(ifs1.tf[i]);
  for(i=n1;i<nr;i++)
    ifsr.tf[i] = math_dupli(ifs2.tf[i-n1]);
  return(ifsr);
 }
/* Combinaisons tensorielles */
/* Produit cartesien : A(I1 (X)_x I2) = A(I1) x A(I2) */
IFS ifs_tens_pcar(IFS ifs1, IFS ifs2)
 {
  IFS ifsr;
  int n1,n2,i,j,ij;
  n1 = ifs1.nb;
  n2 = ifs2.nb;
  ifsr = ifs_crea(n1*n2);
  ij = 0;
  for(i=0;i<n1;i++)
    {
    for(j=0;j<n2;j++)
      {
       ifsr.tf[ij] = math_pcar(ifs1.tf[i],ifs2.tf[j]);
       ij = ij+1;
      }
    }
  return(ifsr);
 }
/* Combinaisons tensorielles */
/* A(I1 (X)_# I2) = A(I1) # A(I2)
   avec # : 
   - produit cartesien S||T 
   - produit tensoriel S(X)T 
   - conjugaison       R*T*R^-1 (action de groupe) 
*/
/*
IFS ifs_tens_pcar(OPERATION op, IFS ifs1, IFS ifs2)
 {
  IFS ifsr;
  int n1,n2,i,j,ij;
  n1 = ifs1.nb;
  n2 = ifs2.nb;
  ifsr = ifs_crea(n1*n2);
  ij = 0;
  for(i=0;i<n1;i++)
    {
    for(j=0;j<n2;j++)
      {
       ifsr.tf[ij] = math_pcar(ifs1.tf[i],ifs2.tf[j]);
       ij = ij+1;
      }
    }
  return(ifsr);
 }
*/
/* Combinaisons fonctionnelles */
/* ifs1.nb = ifs2.nb suppose vrai */
/*
IFS ifs_fonc_op(OPERATION op, IFS ifs1, IFS ifs2)
 {
  IFS ifsr;
  int nbr,i;
  nbr = ifs1.nb;
  ifsr = ifs_crea(nbr);
  for(i=0;i<nbr;i++)
    ifsr.tf[i] = math_op2(op,ifs1.tf[i],ifs2.tf[i]);
  return(ifsr);
 }
*/

/*--------*/
/* IFS 1D */
/*--------*/

/* Singleton {0} */
/*
IFS singleton1()
 {
  IFS ifsr;
  MATH ho;
  ho = ho1(0.5);
  ifsr = ifs_math1(ho);
  return ifsr;
 }
*/
/* Intervalle [0,1] -> [0,1/2] U [1/2,1] */
IFS intervalle()
 {
  IFS ifsr;
  MATH ho;
  ho = ho1(0.5);
  ifsr = ifs_math2(ho,math_mul(tr1(0.5),ho));
  return ifsr;
 }
/* Cantor [0,1] -> [0,1/3] U [2/3,1] */
IFS cantor()
 {
  IFS ifsr;
  MATH ho;
  ho = ho1(1./3);
  ifsr = ifs_math2(ho,math_mul(tr1(2./3),ho));
  return ifsr;
 }

/*--------*/
/* IFS 2D */
/*--------*/

/* COURBES */
/* Definition diagonale : [O,O+e1+e2] -> [O,p1] U [p1,O+e1+e2] */
/* Courbes hyperboliques de Dubuc (dilatations) */
IFS dubuc(Flottant x, Flottant y)
 {
  IFS ifsr;
  MATH ho;
  ho = ho2(0.5);
  ifsr = ifs_math2(tdil2(0,0,x,y),tdil2(x,y,1-x,1-y));
  return ifsr;
 }
/* Definition horizontale : [O,O+e1] -> [0,p1] U {p1,O+e1] */
/* Courbes hyperboliques de Dubuc (similitudes hyperboliques) */
IFS dubuch(Flottant x, Flottant y)
 {
  IFS ifsr;
  MATH ho;
  ho = ho2(0.5);
  ifsr = ifs_math2(simh2(0,0,x,y),simh2(x,y,1-x,-y));
  return ifsr;
 }
/* Courbes de Von Koch-Mandelbrot */
IFS vonkoch(Flottant x, Flottant y)
 {
  IFS ifsr;
  MATH ho;
  ho = ho2(0.5);
  ifsr = ifs_math2(sim2(0,0,x,y),sim2(x,y,1-x,-y));
  return ifsr;
 }
/* Courbes de Takagi */
IFS takagi(Flottant x, Flottant y, Flottant r, Flottant s)
 {
  IFS ifsr;
  MATH ho;
  ho = ho2(0.5);
  ifsr = ifs_math2(tafo2(x,0,y,r),taf2(x,y,1-x,0,-y,s));
  return ifsr;
 }
/* Courbes de Takagi symetriques */
IFS takagi_sym(Flottant y, Flottant r)
 {
  return takagi(0.5,y,r,r);
 }
/* IFS a 3 elements */
/* Triangle de Sierpinski (rectangle) */
IFS sierpinski()
 {
  IFS ifsr;
  MATH ho;
  ho = ho2(0.5);
  ifsr = ifs_math3(ho,math_mul(tx2(0.5),ho),math_mul(ty2(0.5),ho));
  return ifsr;
 }
/* Arbre de Sierpinski (rectangle) */
IFS arbre_sierpinski()
 {
  IFS ifsr;
  MATH ho;
  Flottant r;
  r = 2./3;
  ho = ho2(1-r);
  ifsr = ifs_math3(ho2(r),math_mul(tx2(r),ho),math_mul(ty2(r),ho));
  return ifsr;
 }
/* Subdivision reguliere 2 x 2 du carre unitaire [0,1]^2 */
IFS carre_un()
 {
  IFS ifsr;
  MATH ho;
  ho = ho2(0.5);
  ifsr = ifs_math4(ho,math_mul(tx2(0.5),ho),math_mul(ty2(0.5),ho),
	 math_mul(tr2(0.5,0.5),ho));
  return ifsr;
 }
/* Croix en X */
IFS croix()
 {
  IFS ifsr;
  MATH ho;
  Flottant r,s;
  r = 1./3;
  s = 1-r;
  ho = ho2(r);
  ifsr = ifs_math5(ho,math_mul(tr2(r,r),ho),math_mul(tr2(s,s),ho),
		   math_mul(tx2(s),ho),math_mul(ty2(s),ho));
  /*
  ifsr = ifs_math5(ho,math_mul(tx2(s),ho),math_mul(ty2(s),ho),math_mul(tr2(r,r),ho),math_mul(tr2(s,s),ho));
  */
  return ifsr;
 }

/*--------*/
/* IFS 3D */
/*--------*/

/* Segment unitaire [O,O+e1] dans R^3 */
IFS segment_un_ox()
 {
  IFS ifsr;
  MATH ho;
  ho = ho3(0.5);
  ifsr = ifs_math2(ho,math_mul(tr1(0.5),ho));
  return ifsr;
 }
/* Carre unitaire du plan Oxy [0,1]^2 x {0} */
IFS carre_un_oxy()
 {
  IFS ifsr;
  MATH ho;
  ho = ho3(0.5);
  ifsr = ifs_math4(ho,math_mul(tx(0.5),ho),math_mul(ty(0.5),ho),
         math_mul(tr3(0.5,0.5,0),ho));
  return ifsr;
 }
/* Tetraedre de Sierpinski (rectangle) */
IFS tetra_sierpinski()
 {
  IFS ifsr;
  MATH ho;
  ho = ho3(0.5);
  ifsr = ifs_math4(ho,math_mul(tx(0.5),ho),math_mul(ty(0.5),ho),math_mul(tz(0.5),ho));
  return ifsr;
 }
/* Pyramide de Sierpinski */
IFS pyramide_sierpinski()
 {
  IFS ifsr;
  MATH ho;
  ho = ho3(0.5);
  ifsr = ifs_math5(ho,math_mul(tx(0.5),ho),math_mul(ty(0.5),ho),
		   math_mul(tr3(0.5,0.5,0),ho),math_mul(tr3(0.5,0.5,1),ho));
  return ifsr;
 }
