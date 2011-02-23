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
#include "gifs.h"

/**************************/
/* GENERALISATION DES IFS */
/**************************/

/* par defaut :
   - m = 2 ou 3,
   - plong(n x m) = pass_canon(n,m),
   - proj(m x n) = pass_canon(m,n),
   - Automate = Automate a 1 etat complet,
   CIFS(n) = plong(n x m) + IFS(n) + transfo(n) Boule(n,rayon 1) 
   PIFS(m,n) = proj(m x n) + IFS(n) + Automate complet(1 etat)
   LRIFS(n) = Idn + IFS(n) + Automate complet(1 etat) */

/*---------*/
/* Grilles */
/*---------*/

/* Operations informatiques */
/* creation d'une grille */
GRILLE gril_crea(int n)
 {
  GRILLE gr;
  gr.nb = n;
  gr.c = (SIMPLEXE *) malloc (n*sizeof(SIMPLEXE));
  return gr;
 }
/* destruction d'une grille */
void destru_gril(GRILLE g)
 {
  if(g.nb>0) free(g.c);
 }
/* duplication d'une grille */
GRILLE gril_dupli(GRILLE g)
 {
  int n,i;
  GRILLE gr;
  n = g.nb;
  gr = gril_crea(n);
  for(i=0;i<n;i++)
    gr.c[i] = g.c[i];
  return gr;
 }
/* ecriture d'une grille */
void ecrit_gril(FILE *fich, char *nom, GRILLE g)
 {
  int n,j;
  SIMPLEXE sj;
  n = g.nb;
  fprintf(fich,"%s = Grille ",nom);
  if(n>0)
    {
     n = n-1;
     sj = g.c[0];
     fprintf(fich,"S%d(%d,%d)",sj.dim,sj.deg_esp,sj.deg_ifs);
     for(j=1;j<n;j++)
       {
        sj = g.c[j];
        fprintf(fich,"x S%d(%d,%d)",sj.dim,sj.deg_esp,sj.deg_ifs);
       }
    }
  fprintf(fich,"\n");
 }

/* Operations n-aires */
/* dimension combinatoire */
int dim_gril(GRILLE g)
 {
  int dim,n,i;
  n = g.nb;
  dim = 0;
  for(i=0;i<n;i++)
    dim = dim + g.c[i].dim;
  return dim;
 }
/* nombre de points de controle */
/*
int nb_pc_simpl(SIMPLEXE s)
 {
  int dim,m;
  dim = s.dim;
  m = s.deg_esp;
  if(dim == 1) return m+1;
  else if(dim == 2) return ((m+1)*m)/2;
 }
int nb_pc_gril(GRILLE g)
 {
  int m,n,i;
  n = g.nb;
  nbr = 0;
  for(i=0;i<n;i++)
    nbr = nbr + nb_pc_simpl(g.c[i]);
  return nbr;
 }
*/
/* nombre d'operateurs dans l'IFS */
/*
int nb_ifs_simpl(SIMPLEXE s)
 {
 }
int nb_ifs_gril(GRILLE g)
*/
/* grille simplexe */
GRILLE gril_simpl(int n, int m, int nb)
 {
  GRILLE gr;
  SIMPLEXE sr;
  gr = gril_crea(1);
  sr = gr.c[0];
  sr.dim = n;
  sr.deg_esp = m;
  sr.deg_ifs = nb;
  return gr;
 }
/* produit cartesien de deux grilles */
GRILLE gril_pcar(GRILLE g1, GRILLE g2)
 {
  GRILLE gr;
  int i,m,m1;
  m1 = g1.nb;
  m = m1 + g2.nb;
  gr = gril_crea(m);
  for(i=0;i<=m1;i++)
    gr.c[i] = g1.c[i];
  for(i=m1+1;i<=m;i++)
    gr.c[i] = g2.c[i-m1];
  return gr;
 }

/* Bibliotheque de grilles */
/* Grilles 0D */
/* grille reduite a un sommet : S0(0,1) */
GRILLE gril_S0()
 {
  return gril_simpl(0,0,1);
 }
/* Grilles 1D */
/* ligne brisee : S1(m,N) avec m>=1 et N>=2 */
GRILLE gril_S1(int m, int nb)
 {
  return gril_simpl(1,m,nb);
 }
/* Grilles 2D */
/* quadrilatere : S1 x S1 */
GRILLE gril_S1xS1(int m1, int nb1, int m2, int nb2)
 {
  GRILLE g1,g2,gr;
  g1 = gril_S1(m1,nb1);
  g2 = gril_S1(m2,nb2);
  gr = gril_pcar(g1,g2);
  destru_gril(g1);
  destru_gril(g2);
  return gr;
 }
/* triangulaire : S2(m,N) */
GRILLE gril_S2(int m, int nb)
 {
  return gril_simpl(2,m,nb);
 }
/* Grilles 3D */
/* tetraedrique : S3 */
/*
GRILLE gril_S3(int m, int nb)
 {
  return gril_simpl(3,m,n);
 }
*/
/* prismatique : S2 x S1 */
/* prismatique : S1 x S2 */
/* parallelepipedique : S1 x S1 x S1 */


/*--------------------------*/
/* Operations informatiques */
/*--------------------------*/

/* creation de GIFS */
/*
GIFS gifs_crea(MATH proj, IFS ifs, MATH te, GRILLE gril)
 {
  GIFS gifsr;
  gifsr.proj = proj; 
  gifsr.ifs = ifs;
  gifsr.te = te; 
  gifsr.grille = gril; 
  return(gifsr);
 }
*/
/* destruction de gifs (PRECAUTIONS D'EMPLOI) */
void destru_gifs(GIFS gifs)
 {
  destru_math(gifs.te);
  destru_ifs(gifs.ifs);
  destru_math(gifs.proj);
 }
/* duplication d'un gifs */
GIFS gifs_dupli(GIFS gifs)
 {
  GIFS gifsr;
  gifsr.proj = math_dupli(gifs.proj);
  gifsr.ifs = ifs_dupli(gifs.ifs); 
  gifsr.te = math_dupli(gifs.te); 
  gifsr.extrem = gifs.extrem; 
  return(gifsr);
 }
/* Ecriture d'un GIFS */
void ecrit_gifs(FILE *f, char *nomgifs, GIFS gifs)
 {
  fprintf(f,nomgifs);
  fprintf(f," : IFS generalise\n");
  /*
  ecrit_math(f,"- Matrice de plongement",gifs.plong);
  */
  ecrit_math(f,"- Matrice de projection",gifs.proj);
  ecrit_ifs(f,"- IFS", gifs.ifs);
  ecrit_math(f,"- Transformation de la boule unite", gifs.te);
  ecrit_li(f,"- Indices des extremites de grille", gifs.extrem);
  fprintf(f," \n");
  return;
 }

/*--------------------*/
/* Operations 0-aires */
/*--------------------*/

/* Projection et englobant donnes */
/* coherence supposee : proj.m =< proj.n = dim(ifs) = dim(te) */
GIFS gifs_proj_ifse(MATH proj, IFS ifs, MATH te)
 {
  GIFS gifsr;
  gifsr.proj = proj;
  gifsr.ifs = ifs;
  gifsr.te = te;
  gifsr.extrem = li_id(dim_ifs(ifs));
  return(gifsr);
 }
/* Plongement et englobant donnes */
/* coherence supposee : plong.n =< plong.m = dim(ifs) = dim(te) */
GIFS gifs_plong_ifse(MATH plong, IFS ifs, MATH te)
 {
  return gifs_proj_ifse(math_inverse_g(plong),ifs,te);
 }
/* Projection et plongement canoniques, englobant donne */ 
/* coherence supposee : m =< dim(ifs) = te.m */
GIFS gifs_ifse(int m, IFS ifs, MATH te)
 {
  int n;
  n = dim_ifs(ifs);
  return gifs_proj_ifse(math_pass(m,n),ifs,te);
 }
/* Projection donnee, englobant standard */ 
/* coherence supposee : pass.n = dim(ifs) */
GIFS gifs_proj_ifs(MATH proj, IFS ifs)
 {
  int n;
  n = dim_ifs(ifs);
  return gifs_proj_ifse(proj,ifs,math_id(n));
 }
/* Projection et plongement canoniques, englobant standard */ 
GIFS gifs_ifs(int m, IFS ifs)
 {
  int n;
  n = dim_ifs(ifs);
  return gifs_proj_ifse(math_pass(m,n),ifs,math_id(n));
 }

/*--------------------*/
/* Operations unaires */
/*--------------------*/

/* Dimension de l'espace d'iteration de l'IFS */
int dim_gifs(GIFS gifs)
 {
  return dim_ifs(gifs.ifs);
 }
/* Nombre d'operateurs dans l'IFS */
int nb_gifs(GIFS gifs)
 {
  return (gifs.ifs).nb;
 }
/* Indices des extremites des grilles de points de controle */
L_INT lie_gifs(GIFS gifs)
 {
  return gifs.extrem;
 }
/*
L_INT lie_gifs(GIFS gifs)
 {
  GRILLE g;
  int dimg;
  SIMPLEXE s;
  g = gifs.grille;
  dimg = dim_gril(g);
  s1 = g.c[0]; 
  if(dimg == 1)
    {
     lie.c[0] = 0;
     lie.c[1] = s1.deg_esp; 
    }
  else if(dimg == 2)
    {
     s2 = g.c[1]; 
     if(s1
    }
  fprintf(stdout,"ERREUR : dim de grille >= 2\n");
 }
*/

/* Extractions */

/* Liste des projections des centres des operateurs */
/* A REVOIR */
L_POINT centre_gifs(GIFS gifs)
 {
  L_POINT lp;
  MATH proj;
  POINT ci;
  MATH *t;
  int nb,i;
  proj = gifs.proj;
  t = (gifs.ifs).tf;
  nb = (gifs.ifs).nb;
  lp = lpt_crea(nb);
  for(i=0;i<nb;i++)
    {
    ci = centre_math(t[i]);
    lp.pt[i] = pt_mul(proj,ci);
    destru_pt(ci);
    }
  return lp;
 }
/* Extraction des extremites des grilles de controle */  
/* (P,(T_i),Te,Je) -> P_Je liste de points */
L_VECH lvh_extrem_gifs(GIFS gifs)
 {
  return lvh_math_extr(gifs.proj,gifs.extrem); 
 }
/* (P,(T_i),Te,Je) -> P_Je matrice */
MATH extrem_gifs(GIFS gifs)
 {
  return math_extr(gifs.proj,gifs.extrem); 
 }
/* Extraction d'un sous-IFS a deux elements */ 
/* (P,(T_k)_k,Te) -> (P,(T_i,T_j),Te) */
GIFS gifs_2(GIFS gifs1, int i, int j)
 {
  GIFS gifsr;
  gifsr.proj = math_dupli(gifs1.proj); 
  gifsr.ifs = ifs_math2((gifs1.ifs).tf[i],(gifs1.ifs).tf[j]);
  gifsr.te = math_dupli(gifs1.te);
  gifsr.extrem = li2(0,1);  /* A VERIFIER */
  return(gifsr);
 }
/* Extraction d'un sous-IFS */ 
/* (P,(T_i)_i,Te) -> (P,(T_ij)_j,Te) */
GIFS gifs_extr(GIFS gifs1, L_INT ind)
 {
  GIFS gifsr;
  gifsr.proj = math_dupli(gifs1.proj); 
  gifsr.ifs = ifs_extr(gifs1.ifs,ind);
  gifsr.te = math_dupli(gifs1.te); 
  gifsr.extrem = li_extr(gifs1.extrem,ind); /* A VERIFIER */
  return(gifsr);
 }

/* Operations sur les GIFS */

/* Conjugaison (dans l'espace d'iteration) */
/* transformation attracteur et englobant */
/* (P,(T_i),Te) -> (P,(T*T_i*T^-1),T*Te) */
GIFS gifs_conj(MATH t, GIFS gifs1)
 {
  GIFS gifsr;
  gifsr.proj = math_dupli(gifs1.proj); 
  gifsr.ifs = ifs_conj(t,gifs1.ifs);
  gifsr.te = math_mul(t,gifs1.te);
  gifsr.extrem = li_dupli(gifs1.extrem);
  return(gifsr);
 }
/* Multiplication (dans l'espace de modelisation) */
/* transformation de la projection */
/* (P,(T_i),Te) -> (T*P,(T_i),Te) */
GIFS gifs_mul(MATH t, GIFS gifs1)
 {
  GIFS gifsr;
  gifsr.proj = math_mul(t,gifs1.proj);
  gifsr.ifs = ifs_dupli(gifs1.ifs);
  gifsr.te = math_dupli(gifs1.te);
  gifsr.extrem = li_dupli(gifs1.extrem);
  return(gifsr);
 }
/* Normalisation pour la visualisation */
/* - Algo deterministe :
     GIFS ramene a des coordonnees barycentriques
     si P Cartesien alors :
     (P,(T_i),Te) -> (P*Tcb,(Tcb^-1*T_i*Tcb),Tcb^-1*Te) 
   - Algo d'echappement :
     GIFS ramene a l'espace de l'englobant (ou motif) standard 
     (P,(T_i),Te) -> (P*Te,(Te^-1*T_i*Te),Id) */
GIFS gifs_normal(GIFS gifs1)
 {
  GIFS gifsr;
  MATH t,tinv;
  int m;
  if( BA_esp(espd_math(gifs1.proj)) ) 
     return gifs_dupli(gifs1);
  /* else (CA_esp) */
  m = dim_gifs(gifs1);
  t = math_CA_BA(m);
  tinv = math_BA_CA(m);
  gifsr.proj = math_mul(gifs1.proj,t); 
  gifsr.ifs = ifs_conj(tinv,gifs1.ifs);
  gifsr.te = math_mul(tinv,gifs1.te);
  gifsr.extrem = li_dupli(gifs1.extrem);
  destru_math(t);
  destru_math(tinv);
  return(gifsr);
 }
/*
GIFS gifs_normal(GIFS gifs1)
 {
  GIFS gifsr;
  MATH te,teinv;
  te = gifs1.te;
  teinv = math_inverse(te);
  gifsr.proj = math_mul(gifs1.proj,te); 
  gifsr.ifs = ifs_conj(teinv,gifs1.ifs);
  gifsr.te = math_id(dimd_math(te));
  gifsr.extrem = li_dupli(gifs1.extrem);
  destru_math(teinv);
  return(gifsr);
 }
*/
/* Iteration de GIFS */
/* (P,(T_i),Te) -> (P*T_j,(T_i),Te) */
GIFS gifs_it(int i, GIFS gifs1)
 {
  GIFS gifsr;
  gifsr.proj = math_mul(gifs1.proj,(gifs1.ifs).tf[i]);
  gifsr.ifs = gifs1.ifs;
  gifsr.te = gifs1.te;
  gifsr.extrem = li_dupli(gifs1.extrem); 
  /*gifsr.extrem = gifs1.extrem; */
  return(gifsr);
 }

/*---------------------*/
/* Operations binaires */
/*---------------------*/

/* Operations sur la projection */
/* Operations sur l'IFS */
/* Operations ensemblistes */
