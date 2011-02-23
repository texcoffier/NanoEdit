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

/****************************************/
/*   MATRICES HOMOGENES (m+1) x (n+1)   */
/* REPRESENTANT DES OPERATEURS En -> Em */
/*     AFFINES OU PROJECTIFS            */
/****************************************/

/*--------------------------*/
/* Operations informatiques */
/*--------------------------*/

/* creation de matrice */
MATH math_crea_esp(t_espace espa, t_espace espd)
 {
  MATH tr;
  int m,n,i;
  tr.m = espa;
  tr.n = espd;
  m = dim_esp(espa);
  n = dim_esp(espd);
  tr.c = (Flottant **) malloc ((m+1)*sizeof(Flottant *));
  for(i=0;i<=m;i++)
    tr.c[i] = (Flottant *) malloc ((n+1)*sizeof(Flottant));
  return tr;
 }
/* par defaut matrice cartesienne */
MATH math_crea(int m, int n)
 {
  MATH tr;
  int i;
  tr.m = esp_CA(m);
  tr.n = esp_CA(n);
  /*
  tr.m.dim = m+1;
  tr.m.type_espace = TE_CA ;
  tr.n.dim = n+1;
  tr.n.type_espace = TE_CA ;
  */
  tr.c = (Flottant **) malloc ((m+1)*sizeof(Flottant *));
  for(i=0;i<=m;i++)
    tr.c[i] = (Flottant *) malloc ((n+1)*sizeof(Flottant));
  return tr;
 }
/* creation de matrice barycentrique */
MATH math_crea_BA(int m, int n)
 {
  return math_crea_esp(esp_BA(m),esp_BA(n));
 }
/* destruction de matrice */
void destru_math(MATH t)
 {
  int i;
  for(i=0;i<t.m.dim;i++)
    {
     free(t.c[i]);
    }
  free(t.c);
 }
/* duplication de matrice */
MATH math_dupli(MATH t)
 {
  MATH tr;
  int m,n,i,j;
  tr = math_crea_esp(espa_math(t),espd_math(t));
  m = dima_math(t);
  n = dimd_math(t);
  /*
  tr = math_crea(m,n);
  tr.n.type_espace = t.n.type_espace ;
  tr.m.type_espace = t.m.type_espace ;
  */
  for(i=0;i<=m;i++)
    {
     for(j=0;j<=n;j++)
       tr.c[i][j] = t.c[i][j];
    }
  return tr;
 }
/* modification des espaces */
void modif_math_esp(MATH math, t_espace espa, t_espace espd)
  {
   math.m = espa;
   math.n = espd;
  }

/* ecriture de semigroupe */
void ecrit_semig(FILE *f, char *nom, SEMIG sem)
 {
  char *nomsem;
  switch(sem)
   {
    case Sim :
      {
      nomsem = "des similitudes";
      break;
      }
    case Hyp :
      {
      nomsem = "hyperbolique";
      break;
      }
    case Dil :
      {
      nomsem = " des dilatations";
      break;
      }
    case Cis :
      {
      nomsem = "des cisaillements";
      break;
      }
    case Aff :
      {
      nomsem = "affine";
      break;
      }
    case Proj :
      nomsem = "projectif";
   }
  fprintf(f,"%s = semigroupe %s\n",nom,nomsem);
 } 

/* ecriture de matrice sur fichier quelconque */
void ecrit_math(FILE *f, char *nom, MATH t)
 {
  int m,n,i,j;
  m = dima_math(t);
  n = dimd_math(t);
  fprintf(f,"%s = matrice homogene %dD -> %dD\n",nom,n,m);
  for(i=0;i<=m;i++)
    {
     fprintf(f,"  ");
     for(j=0;j<=n;j++)
       fprintf(f,"%f  ",t.c[i][j]);
     fprintf(f,"\n");
    }
  ecrit_esp(f,"  - espace d'arrivee ",espa_math(t));
  ecrit_esp(f,"  - espace de depart ",espd_math(t));
 }
/* Lecture de matrices */
/*
MATH math_lit(FILE *f)
 {
  MATH tr;
  int m,n,i,j;
  Flottant cij;
  fscanf(f,"%d %d",&m,&n);
  fprintf(stdout,"  Lecture de matrice homogene %d x %d\n",m+1,n+1);
  tr = math_crea(m,n);
  for(i=0;i<=m;i++)
    {
     for(j=0;j<=n;j++)
       {
        fscanf(f,"%f ",&cij);
        tr.c[i][j] = cij;
       }
    }
  fprintf(stdout,"  fin de lecture de matrice");
  return tr;
 }
*/

/***************************/
/* Operations matricielles */ 
/***************************/

/*--------------------*/
/* Operations 0-aires */
/*--------------------*/

/* Matrice nulle */
/* par defaut cartesienne */
MATH math_nul(int m, int n)
 {
  MATH tr;
  int i,j;
  tr = math_crea(m,n);
  for(i=0;i<=m;i++)
    {
     for(j=0;j<=n;j++)
       tr.c[i][j]=0;
    }
  return tr;
 }
/* Matrice de passage canonique : CA(n) -> CA(m) */
/* 
   - Projection canonique si m<n :  P = |I_m 0_m-n| 
   - Identite si m=n : P = I_m  
   - Plongement canonique si m>n :  P = |I_n  |
                                        |0_n-m| 
*/
MATH math_pass(int m, int n)
 {
  MATH tr;
  int i;
  tr = math_nul(m,n);
  for(i=0;i<=m;i++)
    tr.c[i][i] = 1;
  return tr;
 }
/* Matrice identite */
MATH math_id(int m)
 {
  return math_pass(m,m);
 }

/* Matrices donnees par leurs colonnes (points de controle) */
/* Condition : pi points du meme espace X(m) 
   p0,..,pn  ==>  matrice de type X(m) x BA(n) */
/* Matrice donnee par un point */
MATH math_pt1(POINT p0)
 {
  MATH tr;
  int m,i;
  t_espace espa;
  espa = esp_vh(p0);
  tr = math_crea_esp(espa,esp_BA(0));
  m = dim_pt(p0);
  for(i=0;i<=m;i++)
    tr.c[i][0] = p0.c[i];
  return tr;
 }
/* Matrice donnee par deux points */
MATH math_pt2(POINT p0, POINT p1)
 {
  MATH tr;
  int m,i;
  t_espace espa;
  espa = esp_vh(p0);
  tr = math_crea_esp(espa,esp_BA(1));
  m = dim_pt(p0);
  for(i=0;i<=m;i++)
    {
     tr.c[i][0] = p0.c[i];
     tr.c[i][1] = p1.c[i];
    }
  return tr;
 }
/* Matrice donnee par trois points */
MATH math_pt3(POINT p0, POINT p1, POINT p2)
 {
  MATH tr;
  int m,i;
  t_espace espa;
  espa = esp_vh(p0);
  tr = math_crea_esp(espa,esp_BA(2));
  m = dim_pt(p0);
  for(i=0;i<=m;i++)
    {
     tr.c[i][0] = p0.c[i];
     tr.c[i][1] = p1.c[i];
     tr.c[i][2] = p2.c[i];
    }
  return tr;
 }
/* Matrice donnee par quatre points */
MATH math_pt4(POINT p0, POINT p1, POINT p2, POINT p3)
 {
  MATH tr;
  int m,i;
  t_espace espa;
  espa = esp_vh(p0);
  tr = math_crea_esp(espa,esp_BA(3));
  m = dim_pt(p0);
  for(i=0;i<=m;i++)
    {
     tr.c[i][0] = p0.c[i];
     tr.c[i][1] = p1.c[i];
     tr.c[i][2] = p2.c[i];
     tr.c[i][3] = p3.c[i];
    }
  return tr;
 }
/* Matrice donnee par cinq points */
MATH math_pt5(POINT p0, POINT p1, POINT p2, POINT p3, POINT p4)
 {
  MATH tr;
  int m,i;
  t_espace espa;
  espa = esp_vh(p0);
  tr = math_crea_esp(espa,esp_BA(4));
  m = dim_pt(p0);
  for(i=0;i<=m;i++)
    {
     tr.c[i][0] = p0.c[i];
     tr.c[i][1] = p1.c[i];
     tr.c[i][2] = p2.c[i];
     tr.c[i][3] = p3.c[i];
     tr.c[i][4] = p4.c[i];
    }
  return tr;
 }
/* Matrice donnee par six points */
MATH math_pt6(POINT p0, POINT p1, POINT p2, POINT p3, POINT p4, POINT p5)
 {
  MATH tr;
  int m,i;
  t_espace espa;
  espa = esp_vh(p0);
  tr = math_crea_esp(espa,esp_BA(5));
  m = dim_pt(p0);
  for(i=0;i<=m;i++)
    {
     tr.c[i][0] = p0.c[i];
     tr.c[i][1] = p1.c[i];
     tr.c[i][2] = p2.c[i];
     tr.c[i][3] = p3.c[i];
     tr.c[i][4] = p4.c[i];
     tr.c[i][5] = p5.c[i];
    }
  return tr;
 }

/*--------------------*/
/* Operations unaires */
/*--------------------*/

/* espaces de depart et d'arrivee */
t_espace espa_math(MATH t)
 {
  return t.m;
 }
t_espace espd_math(MATH t)
 {
  return t.n;
 }
/* dimensions des espaces de depart et d'arrivee */
int dima_math(MATH t)
 {
  return t.m.dim-1;
 }
int dimd_math(MATH t)
 {
  return t.n.dim-1;
 }
/* Rayon au carre associe a une matrice cartesienne vectorielle */
/*
   rayon = max_{j=1,n} ||v_j|| 
   avec v_j colonne supposee du type vectoriel (0,t1j,...,tmj) 
   utilisation : test de visualisation d'une iteration P_a = P T_a1...T_an 
*/ 
Flottant ray2_math(MATH t)
 {
  Flottant r,rj,tij;
  int m,n,i,j;
  m = dima_math(t);
  n = dimd_math(t);
  r = 0.;
  for(j=1;j<=n;j++)
    {
     rj = 0.;
     for(i=1;i<=m;i++)
       {
        tij = t.c[i][j];
        rj = rj + tij*tij; 
       }
     if(rj > r) r = rj;
    }
  return r;
 }

/* Operations avec plongement automatique dans l'espace le plus grand :
   m < n => x0,x1,...,xm,0,...,0 */

/* Multiplication d'un point par une matrice homogene : pr = T p */
/* fonction avec creation */
/* condition : espd(T) = esp(p)
   ==> esp(pr) = espa(T) */
VECH vh_mul(MATH t, VECH p)
 {  
  VECH pr;
  Flottant pri;
  int i,j,m,n;
  t_espace espa;
  espa = espa_math(t);
  pr = vh_crea_esp(espa);
  m = dima_math(t);
  n = int_min(dimd_math(t),dim_pt(p));
  for(i=0;i<=m;i++)
    {
     pri = 0; 
     for(j=0;j<=n;j++)
       pri = pri + t.c[i][j]*p.c[j];
     pr.c[i] = pri;
    }
  return(pr);
 }
/* equivalents */
POINT pt_mul(MATH t, POINT p)
 {  
  return vh_mul(t,p);
 }
POINT pt_math(MATH t, POINT p)
 {  
  return pt_mul(t, p);
 }
/* procedure avec modification */
/* condition : espd(T) = esp(p) et esp(pr) = espa(T) */
void mul_math_pt(POINT pr, MATH t, POINT p)
 {  
  Flottant pri;
  int i,j,m,n;
  m = dima_math(t);
  n = int_min(dimd_math(t),dim_pt(p));
  for(i=0;i<=m;i++)
    {
     pri = 0; 
     for(j=0;j<=n;j++)
       pri = pri + t.c[i][j]*p.c[j];
     pr.c[i] = pri;
    }
 }

/* Extraction de colonnes */
/* Extraction d'un vecteur colonne */
POINT pt_col(MATH t, int j)
 {
  POINT pr;
  int m,i;
  t_espace espa;
  espa = espa_math(t);
  pr = vh_crea_esp(espa);
  m = dima_math(t);
  for(i=0;i<=m;i++)
    pr.c[i] = t.c[i][j];
  return pr;
 }
/* Extraction d'une matrice composee de deux colonnes */
/* colonnes c0,c1 type point => matrice type X(m) x BA(1) */ 
MATH math_col2(MATH t, int j, int k)
 {
  MATH tr;
  int m,i;
  t_espace espa;
  espa = espa_math(t);
  tr = math_crea_esp(espa,esp_BA(1));
  m = dima_math(t);
  for(i=0;i<=m;i++)
   {
    tr.c[i][0] = t.c[i][j];
    tr.c[i][1] = t.c[i][k];
   }
  return tr;
 }
/* Extraction d'une matrice composee d'une suite de colonnes */ 
/* colonnes c0,...,cn-1 de type point => matrice type X(m) x BA(n-1) */ 
MATH math_extr(MATH t, L_INT js)
 {
  MATH tr;
  int m,n,i,l;
  t_espace espa;
  espa = espa_math(t);
  n = long_li(js);
  tr = math_crea_esp(espa,esp_BA(n-1));
  m = dima_math(t);
  for(l=0;l<n;l++)
   {
    for(i=0;i<=m;i++)
      tr.c[i][l] = t.c[i][js.c[l]];
   }
  return tr;
 }

/* Modification de colonnes */
/* utilisation : equations de raccord des attracteurs */
/* a partir de vecteurs homogenes 
   condition : espa(t) = esp(p1) = ... = esp(pn) */
/* T.j <- p */
void col_math_vh1(MATH tr, int j1, VECH p1)
 {
  int m,i;
  m = dim_pt(p1);
  for(i=0;i<=m;i++)
      tr.c[i][j1] = p1.c[i];
 }
/* a partir des colonnes d'une autre matrice 
   T.jl <- S.jsl  pour l = 0,..,n-1 
   condition : |jr| = |js| =< dimd(tr) = dima(ts) */
void col_math_li_math_li(MATH tr, L_INT jr, MATH ts, L_INT js)
 {
  int m,n,i,l;
  m = dima_math(tr);
  n = long_li(jr);
  for(l=0;l<n;l++)
   {
    for(i=0;i<=m;i++)
      tr.c[i][jr.c[l]] = ts.c[i][js.c[l]];
   }
 }
/* a partir d'une matrice carree de colonnes plus petites 
   T_i,jrl <- S_kl  si i = jrk 
           <- 0     sinon 
   condition : dima(tr) >= |jr| = dima(t) = dimd(t) */
void col_math_li_math(MATH tr, L_INT jr, MATH t)
 {
  int m,n,i,l;
  m = dima_math(t);
  n = long_li(jr);
  for(l=0;l<n;l++)
   {
    for(i=0;i<=m;i++)
      tr.c[i][jr.c[l]] = 0;
    for(i=0;i<n;i++)
      tr.c[jr.c[i]][jr.c[l]] = t.c[i][l];
   }
 }

/* Transposee d'une matrice */
/* X(m) x Y(n) -> Y(n) x X(m) */
MATH math_transp(MATH t)
 {
  MATH tr;
  int m,n,i,j;
  tr = math_crea_esp(espd_math(t),espa_math(t));
  m = dima_math(t);
  n = dimd_math(t);
  for(i=0;i<=n;i++)                     
    {
     for(j=0;j<=m;j++)
       tr.c[i][j] = t.c[j][i];
    }
  return tr;
 }
/* Matrice obtenue par reflexion des indices */
/* X(m) x BY(n) -> X(m) x BY(n) (car symetrie des coordonnees) */
MATH math_reflex(MATH t)
 {
  MATH tr;
  int m,n,i,j;
  tr = math_crea_esp(espa_math(t),espd_math(t));
  m = dima_math(t);
  n = dimd_math(t);
  tr = math_crea(m,n);
  for(i=0;i<=m;i++)                     
    {
     for(j=0;j<=n;j++)
       tr.c[i][j] = t.c[m-i][n-j];
    }
  return tr;
 }

/* Inverse d'une matrice homogene */
/* condition : carree et reguliere */
/* X(n) x Y(n) -> Y(n) x X(n) */

/* sous-matrice (m-1)D d'une matrice mD */
/* par defaut cartesienne (matrice tampon) */
MATH sous_math(int m, MATH t, int k, int l)
  {
   MATH tr;
   int i,j;
   tr = math_crea(m-1,m-1);
   for(i=0;i<k;i++)
     {
      for(j=0;j<l;j++) tr.c[i][j] = t.c[i][j];
      for(j=l;j<m;j++) tr.c[i][j] = t.c[i][j+1];
     }
   for(i=k;i<m;i++)
     {
      for(j=0;j<l;j++) tr.c[i][j] = t.c[i+1][j];
      for(j=l;j<m;j++) tr.c[i][j] = t.c[i+1][j+1];
     }
  return tr;
 }
/* determinant de matrice mD */
/* m >= 0 */
Flottant det_math(int m, MATH t)
 {
  Flottant d;
  int i;
  if(m == 0) return t.c[0][0];
  if(m == 1) return t.c[0][0]*t.c[1][1]-t.c[0][1]*t.c[1][0];
  d = 0;                                    
  for(i=0;i<=m;i++)
    d = d + t.c[0][i]*codet_math(m,t,0,i);
  return d;
 }
/* codeterminants de matrice mD */
/* m > 0 */
Flottant codet_math(int m, MATH t, int k, int l)
 {
  MATH s;
  Flottant d;
  s = sous_math(m,t,k,l);
  d = det_math(m-1,s);
  destru_math(s);
  if((k+l)&1) return -d; /* k+l impair */
  else return d;         /* k+l pair */
 }
/* inverse de matrice mD */
/* m >= 0 */
MATH math_inverse(MATH t)
 {
  MATH tr;
  double d;
  int m,n,i,j;
  tr = math_crea_esp(espd_math(t),espa_math(t));
  m = dima_math(t);
  n = dimd_math(t);
  /* 
  tr = math_crea(m,m);
  */
  if(m != n) 
    {
     fprintf(stdout,
     "ERREUR : matrice non inversible car non carree %d =/= %d\n",m,n);
     return tr;
    }
  if(m == 0)
    {
     d = t.c[0][0];
     if(d == 0)
       {
        fprintf(stdout,"ERREUR : matrice non inversible car determinant nul\n");
        return tr;
       }
     else
       {
        tr.c[0][0] = 1/d;
        return tr;
       }
    }
  for(i=0;i<=m;i++)                     /* matrice conjuguee */
    {
     for(j=0;j<=m;j++)
       tr.c[i][j] = codet_math(m,t,j,i);
    }
  d = 0;                                /* calcul determinant */
  for(i=0;i<=m;i++)
     d = d + t.c[0][i]*tr.c[i][0];
  if(d == 0)
    {
     fprintf(stdout,"ERREUR : matrice non inversible car determinant nul\n");
     return tr;
    }
  d = 1/d;                              /* matrice inversible d =/= 0 */
  for(i=0;i<=m;i++)
    {
     for(j=0;j<=m;j++)
        tr.c[i][j] = tr.c[i][j]*d;
    }
  return tr;
 }
/* Inverse a droite (projection) */ 
/* condition : m =< n et rang maximum 
    T T^-1 = Id_m et T^-1 T = P_n ; T^-1 = T^t (T T^t)^-1 */
MATH math_inverse_d(MATH t)
 {
  MATH tr,s,ts,tsinv;
  int m,n;
  m = dima_math(t);
  n = dimd_math(t);
  if(m > n) 
    {
     fprintf(stdout,
     "ERREUR : matrice non inversible a droite car %d > %d\n",m,n);
     return tr;
    }
  if(m == n)
     return math_inverse(t);  /* matrice carree */
  /* m > n */
  s = math_transp(t);
  ts = math_mul(t,s);
  tsinv = math_inverse(ts);
  tr = math_mul(s,tsinv);
  destru_math(tsinv);
  destru_math(ts);
  destru_math(s);
  return tr ;
 }
/* Inverse a gauche (plongement) */ 
/* condition : m >= n et rang maximum 
    T^-1 T = Id_n et T T^-1 = P_m ; T^-1 = (T^t T)^-1 T^t */
MATH math_inverse_g(MATH t)
 {
  MATH tr,s,st,stinv;
  int m,n;
  m = dima_math(t);
  n = dimd_math(t);
  if(m < n) 
    {
     fprintf(stdout,
     "ERREUR : matrice non inversible a gauche car %d < %d\n",m,n);
     return tr;
    }
  if(m == n)
     return math_inverse(t);  /* matrice carree */
  /* m > n */
  s = math_transp(t);
  st = math_mul(s,t);
  stinv = math_inverse(st);
  tr = math_mul(stinv,s);
  destru_math(stinv);
  destru_math(st);
  destru_math(s);
  return tr ;
 }

/*---------------------*/
/* Operations binaires */
/*---------------------*/

/* Produit de matrices */
/* plongement automatique si dimd_math(t1) =/= dima_math(t2) 
   M ~ M | 0 */
/* condition : espd(t1) = espa(t2)
   => espa(t1*t2) = espa(t1) et espd(t1*t2) = espd(t2) */
MATH math_mul(MATH t1, MATH t2)
 {  
  MATH tr;
  Flottant trij;
  int i,j,k,m1,n2,l;
  tr = math_crea_esp(espa_math(t1),espd_math(t2));
  m1 = dima_math(t1);
  n2 = dimd_math(t2);
  l = int_min(dimd_math(t1),dima_math(t2));
  /*
  tr = math_crea(m1,n2);
  */
  for(i=0;i<=m1;i++)
    {
     for(j=0;j<=n2;j++)
       {
        trij = 0; 
        for(k=0;k<=l;k++)
          trij = trij + t1.c[i][k]*t2.c[k][j];
        tr.c[i][j] = trij;
       }
    }
  return(tr);
 }
/* Division a droite */
/* T1/T2 = T1*T2^-1 */
MATH math_div_d(MATH t1, MATH t2)
 {
  MATH tr,t2inv;
   t2inv = math_inverse(t2);
   tr = math_mul(t1,t2inv);
   destru_math(t2inv);
   return tr;
 }
/* Division a gauche */
/* T1\T2 = T1^-1*T2 */
MATH math_div_g(MATH t1, MATH t2)
 {
  MATH tr,t1inv;
   t1inv = math_inverse(t1);
   tr = math_mul(t1inv,t2);
   destru_math(t1inv);
   return tr;
 }
/* Conjugaison (changement de repere) */
/* T1*T2*T1^-1 */
MATH math_conj(MATH t1, MATH t2)
 {
  MATH tr,t1t2,t1inv;
  t1t2 = math_mul(t1,t2);
  t1inv = math_inverse(t1);
  tr = math_mul(t1t2,t1inv);
  destru_math(t1inv);
  destru_math(t1t2);
  return tr;
 }

/**********************/
/* Operations affines */
/**********************/

/* matrices representant des operateurs affines en coordonnees cartesiennes */
/* T00 = 1 et T0i = 0 pour i=1,..,n */
/*     |1 0| 
   T = |u L| */

/*--------------------*/
/* Operations 0-aires */
/*--------------------*/

/* Homotheties */
/* -> CA(m) x CA(m) */
MATH math_ho(int m, Flottant r)
 {
  MATH tr;
  int i;
  tr = math_nul(m,m);
  tr.c[0][0] = 1;
  for(i=1;i<=m;i++)
    tr.c[i][i] = r;
  return tr;
 }
/* Passages : cartesien <-> barycentrique */
/* -> CA(m) x BA(m) */
MATH math_CA_BA(int m)
 {
  MATH tr;
  int i;
  tr = math_id(m);
  modif_math_esp(tr,esp_CA(m),esp_BA(m));
  for(i=1;i<=m;i++)
    tr.c[0][i] = 1;
  return tr;
 }
/* -> BA(m) x CA(m) */
MATH math_BA_CA(int m)
 {
  MATH tr;
  int i;
  tr = math_id(m);
  modif_math_esp(tr,esp_BA(m),esp_CA(m));
  for(i=1;i<=m;i++)
    tr.c[0][i] = -1;
  return tr;
 }

/*--------------------*/
/* Operations unaires */
/*--------------------*/

/* Partie lineaire (ou centree a l'origine) */
/* |1 0|    |1 0|
   |u L| -> |0 L| */
/* CA(m) x CA(n) -> CA(m) x CA(n) */
MATH lin_math(MATH t)
 {
  MATH tr;
  int m,n,i,j;
  m = dima_math(t);
  n = dimd_math(t);
  tr = math_crea(m,n);
  for(i=0;i<=n;i++)                     
    {
     tr.c[i][0] = 0;
     for(j=1;j<=m;j++)
       tr.c[i][j] = t.c[i][j];
    }
  tr.c[0][0] = 1;
  return tr;
 }
/* Origine du repere ou vecteur de translation */
/* |1 0|    |1|
   |u L| -> |u| */
/* XA(m) x YA(n) -> XA(m) */
POINT or_math(MATH t)
 {
  return pt_col(t,0);
 }
/* Centre (point fixe de l'operateur affine) */
/* T*c = c */
/* XA(m) x YA(n) -> ? */
POINT centre_math(MATH t)
 {
  POINT p;
  MATH il,ilinv;
  ecrit_math(stdout,"  t",t);
  il = math_diff(math_id(t.m.dim),lin_math(t));
  ecrit_math(stdout,"  il",il);
  ilinv = math_inverse(il);
  ecrit_math(stdout,"  ilinv",ilinv);
  p = pt_math(ilinv,or_math(t));
  ecrit_pt(stdout,"  centre",p);
  destru_math(ilinv);
  destru_math(il);
  return p;
 } 

/* Conversion en coordonnees cartesiennes 3D */
/* avec projection ou plongement */
/* condition : T telle que T_00 =/= 0 et T_0i = 0 pour 1<=i<=n */ 
/*
MATH car3_math(MATH t)
 {
  MATH tr;
  Flottant w;
  int m,n;
  m = dima_math(t);
  n = dimd_math(t);
  w = t[0][0];
  tr = math_crea(3,3);
  if(w == 0)
    {
     fprintf(stdout,"ERREUR : matrice non conforme t00 = 0 ou t0i =/= 0 \n");
     return tr;
    }
  tr[0][0] = 1;
  tr[0][1] = 0;
  tr[0][2] = 0;
  tr[0][3] = 0;
  if(m >= 3)
  return tr;
 }
*/

/*---------------------*/
/* Operations binaires */
/*---------------------*/

/* Difference */
/* |1     0...0 |
   |u1-u2 L1-L2 | */
/* condition : dima_math(t1) = dima_math(t2) , dimd_math(t1) = dimd_math(t2) 
   CA(m) x CA(n) ? */ 
MATH math_diff(MATH t1, MATH t2)
 {
  MATH tr;
  int i,j,m,n;
  m = dima_math(t1);
  n = dimd_math(t1);
  tr = math_crea(m,n);
  for(i=0;i<=m;i++)
    {
     for(j=0;j<=n;j++)
       tr.c[i][j] = t1.c[i][j]-t2.c[i][j];
    }
  tr.c[0][0] = 1;
  return tr;
 }

/* Produit cartesien : sommes des dimensions m1+m2, n1+n2 */  
/* |1  0  |
   |u1 L1 |
   |u2 L2 | */
/* condition : matrices affines cartesiennes
   (CA(m1)xCA(n1)) X (CA(m2)xCA(n2)) -> CA(m1+m2)xCA(n1+n2) */
/* generalisation : matrices mixtes
   (MA(m1)xNA(n1)) X (MA(m2)xNA(n2)) -> MNA(m1+m2)xMNA(n1+n2) */
MATH math_pcar(MATH t1, MATH t2)
 {  
  MATH tr;
  int i,j,m1,n1,m,n;
  fprintf(stdout,
  "ATTENTION : produit cartesien defini entre matrices affines cartesiennes\n");
  m1 = dima_math(t1);
  n1 = dimd_math(t1);
  m = m1 + dima_math(t2);
  n = n1 + dimd_math(t2); 
  tr = math_crea(m,n);
  for(i=0;i<=m1;i++)
    {
     for(j=0;j<=n1;j++)
       tr.c[i][j] = t1.c[i][j];
     for(j=n1+1;j<=n;j++)
       tr.c[i][j] = 0;
    }
  for(i=m1+1;i<=m;i++)
    {
     tr.c[i][0] = t2.c[i-m1][0];
     for(j=1;j<=n1;j++)
       tr.c[i][j] = 0;
     for(j=n1+1;j<=n;j++)
       tr.c[i][j] = t2.c[i-m1][j-n1];
    }
  return(tr);
 }

/****************************/
/* Bibliotheque de matrices */
/****************************/

/*------------*/
/* Espaces 1D */
/*------------*/

/* Matrices homogenes 2x2 */
/* 
   T = |a00 a01|
       |a10 a11|
*/
/* par defaut BA(1)xBA(1) */
MATH math2x2(Flottant a00,Flottant a01, 
	     Flottant a10,Flottant a11)
 {
  MATH tr;
  tr = math_crea_BA(1,1);
  tr.c[0][0] = a00;
  tr.c[0][1] = a01;
  tr.c[1][0] = a10;
  tr.c[1][1] = a11;
  return tr;
 }

/* Coordonnees cartesiennes */
/* -> CA(1)xCA(1) */

/* Matrice identite */
/* par defaut cartesienne */
MATH math_id1()
 {
  return math_id(1);
 }
/* Homothetie */
MATH ho1(Flottant r)
 {
  return math_ho(1,r);
 }
/* Translations */
MATH tr1(Flottant x)
 {
  MATH tr;
  tr = math_id(1);
  tr.c[1][0] = x;
  return tr;
 }
/* Transfo affine */
/* x -> x0 + r x */ 
MATH taf1(Flottant x0, Flottant r)
 {
  MATH tr;
  tr = math_id(1);
  tr.c[1][0] = x0;
  tr.c[1][1] = r;
  return tr;
 }

/*------------*/
/* Espaces 2D */
/*------------*/

/* Matrices homogenes 3x3 */
/* 
   T = |a00 a01 a02|
       |a10 a11 a12|
       |a20 a21 a22|
*/
/* par defaut BA(2)xBA(2) */
MATH math3x3(Flottant a00,Flottant a01, Flottant a02, 
	     Flottant a10,Flottant a11, Flottant a12,
	     Flottant a20,Flottant a21, Flottant a22)
 {
  MATH tr;
  tr = math_crea_BA(2,2);
  tr.c[0][0] = a00;
  tr.c[0][1] = a01;
  tr.c[0][2] = a02;
  tr.c[1][0] = a10;
  tr.c[1][1] = a11;
  tr.c[1][2] = a12;
  tr.c[2][0] = a20;
  tr.c[2][1] = a21;
  tr.c[2][2] = a22;
  return tr;
 }

/* Operateurs affines en coordonnees cartesiennes */
/* -> CA(2)xCA(2) */

/* Matrice identite */
MATH math_id2()
 {
  return math_id(2);
 }
/* Homothetie */
MATH ho2(Flottant r)
 {
  return math_ho(2,r);
 }
/*
MATH hoc2(POINT c, Flottant r)
 {
  return math_conj_pt(2,c,r);
 }
*/
/* Translations */
MATH tx2(Flottant x)
 {
  MATH tr;
  tr = math_id(2);
  tr.c[1][0] = x;
  return tr;
 }
MATH ty2(Flottant y)
 {
  MATH tr;
  tr = math_id(2);
  tr.c[2][0] = y;
  return tr;
 }
MATH tr2(Flottant x, Flottant y)
 {
  MATH tr;
  tr = math_id(2);
  tr.c[1][0] = x;
  tr.c[2][0] = y;
  return tr;
 }
/* Dilatations */
MATH dx2(Flottant x)
 {
  MATH tr;
  tr = math_id(2);
  tr.c[1][1] = x;
  return tr;
 }
MATH dy2(Flottant y)
 {
  MATH tr;
  tr = math_id(2);
  tr.c[2][2] = y;
  return tr;
 }
MATH dil2(Flottant x, Flottant y)
 {
  MATH tr;
  tr = math_id(2);
  tr.c[1][1] = x;
  tr.c[2][2] = y;
  return tr;
 }
/* Translation-dilatation */
/* x -> x0 + a x   
   y -> y0 + d y */ 
MATH tdil2(Flottant x0, Flottant y0, Flottant a, Flottant d)
 {
  MATH tr;
  tr = math_id(2);
  tr.c[1][0] = x0;
  tr.c[1][1] = a;
  tr.c[1][2] = 0;
  tr.c[2][0] = y0;
  tr.c[2][1] = 0;
  tr.c[2][2] = d;
  return tr;
 }
/* Similitude hyperbolique (conjugue de translation-dilatation) */
/* x -> x0 + a x + c y   
   y -> y0 + c x + a y  singuliere si a=c */ 
MATH simh2(Flottant x0, Flottant y0, Flottant a, Flottant c)
 {
  MATH tr;
  tr = math_id(2);
  tr.c[1][0] = x0;
  tr.c[1][1] = a;
  tr.c[1][2] = c;
  tr.c[2][0] = y0;
  tr.c[2][1] = c;
  tr.c[2][2] = a;
  return tr;
 }
/* Rotation */
MATH rot2(Flottant theta)
 {
  MATH tr;
  Flottant c,s;
  tr = math_id(2);
  c = cos(theta);
  s = sin(theta);
  tr.c[1][1] = c;
  tr.c[1][2] = -s;
  tr.c[2][1] = s;
  tr.c[2][2] = c;
  return tr;
 }
/* Similitude */
/* x -> x0 + a x - c y  
   y -> y0 + c x + a y */ 
MATH sim2(Flottant x0, Flottant y0, Flottant a, Flottant c)
 {
  MATH tr;
  tr = math_id(2);
  tr.c[1][0] = x0;
  tr.c[1][1] = a;
  tr.c[1][2] = -c;
  tr.c[2][0] = y0;
  tr.c[2][1] = c;
  tr.c[2][2] = a;
  return tr;
 }
/* Cisaillements */
MATH cxy2(Flottant c)
 {
  MATH tr;
  tr = math_id(2);
  tr.c[2][1] = c;
  return tr;
 }
MATH cyx2(Flottant b)
 {
  MATH tr;
  tr = math_id(2);
  tr.c[1][2] = b;
  return tr;
 }
/* Transfo affine */
/* x -> x0 + a x + b y  
   y -> y0 + c x + d y */ 
MATH taf2(Flottant x0, Flottant y0, Flottant a, Flottant b, Flottant c, Flottant d)
 {
  MATH tr;
  tr = math_id(2);
  tr.c[1][0] = x0;
  tr.c[1][1] = a;
  tr.c[1][2] = b;
  tr.c[2][0] = y0;
  tr.c[2][1] = c;
  tr.c[2][2] = d;
  return tr;
 }
/* Transfo affine centree a l'origine */
/* x -> a x + b y  
   y -> c x + d y */ 
MATH tafo2(Flottant a, Flottant b, Flottant c, Flottant d)
 {
  MATH tr;
  tr = math_id(2);
  tr.c[1][0] = 0;
  tr.c[1][1] = a;
  tr.c[1][2] = b;
  tr.c[2][0] = 0;
  tr.c[2][1] = c;
  tr.c[2][2] = d;
  return tr;
 }

/*
#define XY_PT(x,y,pt) xy_pt(&x,&y,pt)
*/

/* Transfo donnee par un couple de points */
MATH math_sem_pt2(SEMIG sem, POINT p, POINT q)
 {
  MATH tr;
  Flottant x0,y0,a,c;
  x0 = x_pt(p);
  y0 = y_pt(p);
  a = x_pt(q);
  c = y_pt(q);
  a = a-x0;
  c = c-y0;
  if(dim_pt(p) > 2)
    {
    fprintf(stdout,"ERREUR : deux points sont insuffisants\n");
    return math_id(dim_pt(p));
    }
  switch(sem)
    {
     case Sim :
       {
       tr = sim2(x0,y0,a,c);
       break;
       }
     case Hyp :
       {
       tr = simh2(x0,y0,a,c);
       break;
       }
     case Dil :
       {
       tr = tdil2(x0,y0,x0+a,y0+c);
       break;
       }
     case Cis :
       {
       tr = taf2(x0,y0,a,0.,c,0.5);
       break;
       }
     case Aff :
       {
       tr = taf2(x0,y0,a,0.,c,0.5);
       break;
       }
     case Proj :
       {
       tr = taf2(x0,y0,a,0.,c,0.5);
       break;
       }
    }
  return tr;
 }

/*------------*/
/* Espaces 3D */
/*------------*/

/* Matrices affines cartesiennes */
/* -> CA(3)xCA(3) */

/* Matrice identite 3D */
MATH math_id3()
 {
  return math_id(3);
 }
/* Homothetie 3D */
MATH ho3(Flottant r)
 {
  return math_ho(3,r);
 }
/* Translations */
MATH tx(Flottant x)
 {
  MATH tr;
  tr = math_id(3);
  tr.c[1][0] = x;
  return tr;
 }
MATH ty(Flottant y)
 {
  MATH tr;
  tr = math_id(3);
  tr.c[2][0] = y;
  return tr;
 }
MATH tz(Flottant z)
 {
  MATH tr;
  tr = math_id(3);
  tr.c[3][0] = z;
  return tr;
 }
MATH tr3(Flottant x, Flottant y, Flottant z)
 {
  MATH tr;
  tr = math_id(3);
  tr.c[1][0] = x;
  tr.c[2][0] = y;
  tr.c[3][0] = z;
  return tr;
 }
/* Rotations */
MATH rx(Flottant theta)
 {
  MATH tr;
  Flottant c,s;
  tr = math_id(3);
  c = cos(theta);
  s = sin(theta);
  tr.c[2][2] = c;
  tr.c[2][3] = -s;
  tr.c[3][2] = s;
  tr.c[3][3] = c;
  return tr;
 }
MATH ry(Flottant theta)
 {
  MATH tr;
  Flottant c,s;
  tr = math_id(3);
  c = cos(theta);
  s = sin(theta);
  tr.c[3][3] = c;
  tr.c[3][1] = -s;
  tr.c[1][3] = s;
  tr.c[1][1] = c;
  return tr;
 }
MATH rz(Flottant theta)
 {
  MATH tr;
  Flottant c,s;
  tr = math_id(3);
  c = cos(theta);
  s = sin(theta);
  tr.c[1][1] = c;
  tr.c[1][2] = -s;
  tr.c[2][1] = s;
  tr.c[2][2] = c;
  return tr;
 }
/* Plongement canonique : R^2 -> Oxy */
/* 
   |1 0 0|
   |0 1 0|
   |0 0 1|
   |0 0 0|
*/
MATH plong_xy()
 {
  return math_pass(3,2);
 }
/* Projection canonique : Oxy -> R^2 */
/* 
   |1 0 0 0|
   |0 1 0 0|
   |0 0 1 0|
*/
MATH proj_xy()
 {
  return math_pass(2,3);
 }
/* Plongement vertical : R^2 -> Oyz */
/* 
   |1 0 0|
   |0 0 0|
   |0 1 0| 
   |0 0 1|
*/
MATH plong_yz()
 {
  MATH tr;
  tr = math_pass(3,2);
  tr.c[1][1] = 0;
  tr.c[2][2] = 0;
  tr.c[2][1] = 1;
  tr.c[3][2] = 1;
  return tr; 
 }
/* Projection verticale : Oyz -> R^2 */
/* 
   |1 0 0 0|
   |0 0 1 0|
   |0 0 0 1|
*/
MATH proj_yz()
 {
  MATH tr;
  tr = math_pass(2,3);
  tr.c[1][1] = 0;
  tr.c[2][2] = 0;
  tr.c[1][2] = 1;
  tr.c[2][3] = 1;
  return tr; 
 }
/* Transfo donnee par un semigroupe et un triplet de points */
MATH math_sem_pt3(SEMIG sem, POINT p0, POINT p1, POINT p2)
 {
  MATH tr;
  VECTEUR v1,v2,v3;
  int m;
  m = dim_pt(p0);
  v1 = vec_pt2(p0,p1);
  v2 = vec_pt2(p0,p2);
  if(m == 2)
    {
     if((sem == Aff) || (sem == Proj))
       tr = math_pt3(p0,v1,v2);
     else
       {
       fprintf(stdout,"ERREUR : trop de points \n");
       tr = math_id(m);
       }
    }
  else if(m == 3)
    {
     if(sem == Cis)
       {
       v3 = vec(0,0,1);
       tr = math_pt4(p0,v1,v2,v3);
       }
     else
       {
       fprintf(stdout,"ERREUR : pas assez de points \n");
       tr = math_id(m);
       }
    }
  destru_pt(v1);
  destru_pt(v2);
  return tr;
 }
