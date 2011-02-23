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

/***********/
/* Espaces */
/***********/

/* Ecriture d'espace */
void ecrit_esp(FILE *f, char *nom, t_espace esp)
 {
  fprintf(f,"  %s = Espace ", nom);
  if(C_esp(esp))
    fprintf(f,"C");
  else 
    fprintf(f,"B");
  if(A_esp(esp))
    fprintf(f,"A");
  else 
    fprintf(f,"P");
  fprintf(f,"(%d)\n",dim_esp(esp));
 }

/* Creations d'espaces */
/* Affines */
t_espace esp_CA(int m)
 {
  t_espace espr;
  espr.dim = m+1;
  espr.type_espace = TE_CA;
  return espr;
 }
t_espace esp_BA(int m)
 {
  t_espace espr;
  espr.dim = m+1;
  espr.type_espace = TE_BA;
  return espr;
 }
/* Projectifs */
t_espace esp_CP(int m)
 {
  t_espace espr;
  espr.dim = m+1;
  espr.type_espace = TE_CP;
  return espr;
 }
t_espace esp_BP(int m)
 {
  t_espace espr;
  espr.dim = m+1;
  espr.type_espace = TE_BP;
  return espr;
 }
/* Espace vide */
t_espace esp_vide()
 {
  return esp_CA(-1); 
 }

/* Caracteristiques d'espace */
/* dimension de l'espace represente */
int dim_esp(t_espace esp)
  {
   return esp.dim-1;
  }
/* dimension de l'espace homogene */
int dim_esph(t_espace esp)
  {
   return esp.dim;
  }
/* types d'espace */
Booleen C_esp(t_espace esp)
 {
  return(SYSTEME_DE_COORDONNEE(esp.type_espace) == TE_Cartesien);
 }
Booleen B_esp(t_espace esp)
 {
  return(SYSTEME_DE_COORDONNEE(esp.type_espace) == TE_Barycentrique);
 }
Booleen A_esp(t_espace esp)
 {
  return(GEOMETRIE(esp.type_espace) == TE_Affine);
 }
Booleen P_esp(t_espace esp)
 {
  return(GEOMETRIE(esp.type_espace) == TE_Projectif);
 }
Booleen CA_esp(t_espace esp)
 {
  return(esp.type_espace == TE_CA);
 }
Booleen BA_esp(t_espace esp)
 {
  return(esp.type_espace == TE_BA);
 }
Booleen CP_esp(t_espace esp)
 {
  return(esp.type_espace == TE_CP);
 }
Booleen BP_esp(t_espace esp)
 {
  return(esp.type_espace == TE_BP);
 }

/* Egalite d'espaces */
Booleen egal_esp(t_espace esp1, t_espace esp2)
 {
  return ((esp1.type_espace == esp2.type_espace) & (esp1.dim == esp2.dim));
 }
/* Lois de composition interne */
/* XY(m) x XY(m) -> XY(m) */ 
t_espace esp_comp_interne(t_espace esp1, t_espace esp2)
 {
   t_espace espr;
   if(egal_esp(esp1,esp2))
      espr = esp1;
   else
     {
      espr = esp_vide();
      fprintf(stdout," ERREUR : espaces differents \n");
     }
   return espr;
 }

/**********************/
/* Points et vecteurs */
/**********************/

/*--------------------------*/
/* Operations informatiques */
/*--------------------------*/

/* creation de vecteur homogene dans un espace */ 
VECH vh_crea_esp(t_espace esp)
 {
  POINT pr;
  pr.espace = esp;
  pr.c = (Flottant *) malloc ((esp.dim) * sizeof(Flottant));
  return pr;
 }
/* dans BA(m) */
VECH vh_crea_BA(int m)
 {
  return vh_crea_esp(esp_BA(m));
 }
/* dans CA(m) */
VECH vh_crea_CA(int m)
 {
  return vh_crea_esp(esp_CA(m));
 }
/* creation de point, par defaut dans CA(m) */
POINT pt_crea(int m)
 {
  return vh_crea_esp(esp_CA(m));
 }
/*
POINT pt_crea(int m)
 {
  POINT pr;
  pr.espace = esp_CA(m);
  pr.c = (Flottant *) malloc ((m+1) * sizeof(Flottant));
  return pr;
 }
*/
/* destruction de point */
void destru_pt(POINT p)
 {
  free(p.c);
 }
/* duplication de point */
VECH vh_dupli(VECH p)
 {
  VECH pr;
  int i;
  pr = vh_crea_esp(p.espace);
  for(i=0;i<p.espace.dim;i++)
    pr.c[i] = p.c[i];
  return pr;
 }
POINT pt_dupli(POINT p)
 {
  return vh_dupli(p);
 }
/* ecriture de vecteur homogene dans un fichier */
void ecrit_vh(FILE *f, char *nomp, VECH p)
 {
  int m,i;
  t_espace esp;
  esp = esp_vh(p);
  m = dim_esp(esp);
  fprintf(f,nomp);
  fprintf(f," = vecteur homogene (",m);
  for(i=0;i<=m;i++)
    fprintf(f,"%f  ",p.c[i]);
  fprintf(f,"%f)",p.c[m]);
  ecrit_esp(f," dans",esp);
 }
/* ecriture de point dans un fichier */
void ecrit_pt(FILE *f, char *nomp, POINT p)
 {
  int m,i;
  m = p.espace.dim-1;
  fprintf(f,nomp);
  fprintf(f," = point %dD (",m);
  for(i=0;i<m;i++)
    fprintf(f,"%f  ",p.c[i]);
  fprintf(f,"%f)\n",p.c[m]);
 }

/*-----------------------------------*/
/* Operations vectorielles homogenes */
/*-----------------------------------*/

/* Operations 0-aires */ 
/*....................*/

/* Vecteur homogene canonique de base */
VECTEUR vh_e(int m, int i)
 {
  VECTEUR vr;
  int j;
  vr = pt_crea(m);
  for(j=0;j<=m;j++) vr.c[j] = 0;
  vr.c[i] = 1;
  return vr;
 }
  
/* Operations unaires  */
/*....................*/

/* espace */
t_espace esp_vh(VECH p)
  {
   return p.espace;
  }
/* dimension du vecteur homogene */
int dim_vh(VECH p)
  {
   return dim_esph(p.espace);
  }
/* poids (indicateur) */
/* si A w=1 => point, w=0 => vecteur 
   si P w=/=0 => point fini p/w, 
        w=0 => point a l'infini dans la direction du vecteur p */
Flottant w_vh(VECH vh)
  {
   t_espace esp;
   Flottant w;
   int i;
   esp = esp_vh(vh); 
   w = vh.c[0];
   if(B_esp(esp))
    {
     for(i=1;i<esp.dim;i++)
        w = w + vh.c[i];
    }
   return w;
  }

/* multiplication par un scalaire */
/* R x A(m) -> P(m) */
/*
VECH vh_scal(Flottant r, VECH p)
 {
  VECH pr;
  int i ;
  pr = pt_crea(p.espace.dim-1);
  for(i=0;i<p.espace.dim;i++)
    pr.c[i] = r*p.c[i];
  pr.espace.type_espace = p.type_espace;
  return pr;
 }
*/

/* Operations binaires  */
/*......................*/

/* combinaison affine de vecteurs homogenes */
/* A(m) x A(m) -> A(m) */ 
/* P(m) x A(n) -> P(max(m,n)) ? */
VECH vh_caf(VECH p0, VECH p1, Flottant r)
 {
  VECH pr;
  Flottant r0;
  int m,i;
  t_espace espr;
  espr = esp_comp_interne(esp_vh(p0),esp_vh(p1));
  pr = vh_crea_esp(espr);
  m = dim_esp(espr); 
  r0 = 1-r;
  for(i=0;i<=m;i++)
    pr.c[i] = r0*p0.c[i] + r*p1.c[i];
  return pr;
 }
/* milieu */
VECH vh_mil(VECH p0, VECH p1)
 {
  return vh_caf(p0,p1,0.5);
 }

/*******************/
/* Espaces affines */
/*******************/

/*--------------------*/
/* Operations 0-aires */ 
/*--------------------*/

/* Point origine */
/* -> CA(m) */
POINT pt_or(int m)
 {
  POINT pr;
  int i;
  pr = pt_crea(m);
  pr.c[0] = 1;
  for(i=1;i<pr.espace.dim;i++)
    pr.c[i] = 0;
  return pr;
 }

/*----------------------------------------*/
/* Caracteristiques d'un vecteur homogene */
/*----------------------------------------*/

/* dimension du point (ou vecteur) represente */
int dim_pt(POINT p)
  {
   return dim_esp(p.espace);
  }
/* distance du point a l'origine au carre */
/* intervient dans algo echappement */
Flottant norme2_pt(POINT p)
  {
   Flottant r;
   int i;
   r = 0;
   for(i=1;i<p.espace.dim;i++)
     r = r + p.c[i]*p.c[i]; 
   return r/(p.c[0]*p.c[0]);
  }
/* multiplication par un scalaire */
/* R x vecA(m) -> vecA(m) */
VECTEUR vec_scal(Flottant r, VECTEUR p)
 {
  VECH pr;
  int i ;
  pr = pt_crea(p.espace.dim-1);
  pr.espace = p.espace;
  for(i=0;i<p.espace.dim;i++)
    pr.c[i] = r*p.c[i];
  return pr;
 }
/* conversion en coordonnees cartesiennes classiques */
/* CP(m) -> CA(m) */
POINT car_pt(POINT p)
  {
   POINT pr;
   Flottant w;
   int m,i;
   m = dim_pt(p);
   pr = pt_crea(3);
   w = p.c[0];
   if(w == 0.)
     {
      fprintf(stdout,"ERREUR : point a l'infini\n");
      return pr;
     }
   pr.c[0] = 1;
   for(i=1;i<=m;i++) 
      pr.c[i] = p.c[i]/w;
   return pr;
  }
/* conversion en point 3D avec projection ou plongement */
/* CP(m) -> CA(3) */
POINT car3_pt(POINT p)
  {
   POINT pr;
   Flottant w;
   int m;
   m = dim_pt(p);
   pr = pt_crea(3);
   w = p.c[0];
   if(w == 0.)
     {
      fprintf(stdout,"ERREUR : vecteur ou point a l'infini\n");
      return pr;
     }
   pr.c[0] = 1;
   if(m >= 3)
     {
      pr.c[1] = p.c[1]/w;
      pr.c[2] = p.c[2]/w;
      pr.c[3] = p.c[3]/w;
     }
   else  
     {
      pr.c[3] = 0;
      if(m >= 2)    
	{
         pr.c[1] = p.c[1]/w;
         pr.c[2] = p.c[2]/w;
	}
      else
	{
         pr.c[2] = 0;
	 if(m >= 1) pr.c[1] = p.c[1]/w;
	 else pr.c[1] = 0;
        }
     }
   return pr;
  }

/*---------------------*/
/* Operations binaires */
/*---------------------*/

/* combinaison affine de points affines */
/* A(m) x A(m) -> A(m) */ 
/* P(m)fini x P(m)fini -> A(m) */ 
POINT pt_caf(POINT p0, POINT p1, Flottant r)
 {
  POINT pr;
  Flottant r0,r1;
  int m,i;
  m = dim_pt(p0);
  pr = pt_crea(m);
  r0 = (1-r)/p0.c[0];
  r1 = r/p1.c[0];
  pr.c[0] = 1.;
  for(i=1;i<=m;i++)
    pr.c[i] = r0*p0.c[i] + r1*p1.c[i];
  return pr;
 }
/* Point milieu */
POINT pt_mil(POINT p0, POINT p1)
 {
  return pt_caf(p0,p1,0.5);
 }
/* Point translate */
/* A(m) x vecA(m) -> A(m) */ 
POINT pt_add(POINT p0, VECTEUR v1)
 {
  POINT pr;
  int i;
  pr = pt_crea(p0.espace.dim-1);
  pr.c[0] = 1.;
  for(i=1;i<p0.espace.dim;i++)
    pr.c[i] = p0.c[i] + v1.c[i];
  return pr;
 }
/* Vecteur libre */
/* A(m) x A(m) -> vecA(m) */ 
POINT vec_pt2(POINT p0, POINT p1)
 {
  VECTEUR vr;
  int i;
  vr = pt_crea(p0.espace.dim-1);
  vr.c[0] = 0;
  for(i=1;i<p0.espace.dim;i++)
    vr.c[i] = p1.c[i] - p0.c[i];
  return vr;
 }

/* Produit cartesien d'espaces */
/* CA(m1) x CA(m2) = CA(m1+m2) */
/* cas mixte : BA(m1) x CA(m2) = MA(m1+m2) ? */
/* concatenation : (p0,p1,...,pm) || (q0,q1,...,qn) = (p0,p1,..,pm,q1,..,qn) 
   en supposant : p0=q0=1 (points) ou p0=q0=0 (vecteurs) */
POINT pt_pcar(POINT p1, POINT p2)
 {
  POINT pr;
  int i,m,m1;
  m1 = dim_pt(p1);
  m = m1 + dim_pt(p2);
  pr = pt_crea(m);
  for(i=0;i<=m1;i++)
    pr.c[i] = p1.c[i];
  for(i=m1+1;i<=m;i++)
    pr.c[i] = p2.c[i-m1];
  return pr;
 }

/* Produit barycentrique */
/* BA(m1) x_B BA(m2) = BA(m1+m2+1) */
/* (1-r)p || rq = ((1-r)p0,..,(1-r)pm,r q0,..,r qn) */

/* Produit tensoriel d'espaces barycentriques */
/* R^(m1+1) (X) R^(m2+1) <_ R^((m1+1)x(m2+1)) */
/* BA(m1) (X) BA(m2) <_ BA((m1+1)x(m2+1)-1) */
/* p (X) q = (pi qj) */

/*------------------------------------*/
/* Bibliotheque de points et vecteurs */ 
/*------------------------------------*/
/* Convention pour m = 3 : x=p[1], y=p[2], z=p[3], w=p[0] */
#define iw 0 
#define ix 1 
#define iy 2
#define iz 3

/*------------*/
/* Espaces 1D */ 
/*------------*/

/* Representant : vecteur homogene 2D */
/* par defaut dans BA(1) */
VECH vh2(Flottant w, Flottant x)
  {
   POINT pr;
   pr = vh_crea_BA(2);
   pr.c[iw] = w;
   pr.c[ix] = x;
   return pr;
  }
/* Representant : vecteur homogene 2D */
VECH vh2_CA(Flottant w, Flottant x)
  {
   POINT pr;
   pr = vh_crea_CA(2);
   pr.c[iw] = w;
   pr.c[ix] = x;
   return pr;
  }
/* point de l'espace affine cartesien 1D */
POINT pt1(Flottant x)
  {
   return vh2_CA(1,x); 
  }
/* vecteur cartesien 1D */
VECTEUR vec1(Flottant x)
  {
   return vh2_CA(0,x); 
  }

/*------------*/
/* Espaces 2D */ 
/*------------*/

/* Representant : vecteur homogene 3D */
/* par defaut dans BA(2) */
VECH vh3(Flottant w, Flottant x, Flottant y)
  {
   POINT pr;
   pr = vh_crea_BA(2);
   pr.c[iw] = w;
   pr.c[ix] = x;
   pr.c[iy] = y;
   return pr;
  }
/* Espace affine cartesien 2D */
VECH vh3_CA(Flottant w, Flottant x, Flottant y)
  {
   POINT pr;
   pr = vh_crea_CA(2);
   pr.c[iw] = w;
   pr.c[ix] = x;
   pr.c[iy] = y;
   return pr;
  }
/* point de l'espace affine cartesien 2D */
POINT pt2(Flottant x, Flottant y)
  {
   return vh3_CA(1,x,y);
  }
/* vecteur cartesien 2D */
VECTEUR vec2(Flottant x, Flottant y)
  {
   return vh3_CA(0,x,y);
  }

/*------------*/
/* Espaces 3D */ 
/*------------*/

/* Representant : vecteur homogene 4D */
/* par defaut dans BA(3) */
VECH vh4(Flottant w, Flottant x, Flottant y, Flottant z)
  {
   VECH pr;
   pr = vh_crea_BA(3);
   pr.c[iw] = w;
   pr.c[ix] = x;
   pr.c[iy] = y;
   pr.c[iz] = z;
   return pr;
  }
/* espace projectif cartesien 3D */
/* R^4 -> CP(3) */
POINT pt_pj(Flottant x, Flottant y, Flottant z, Flottant w)
  {
   POINT pr;
   pr = vh_crea_esp(esp_CP(3));
   pr.c[iw] = w;
   pr.c[ix] = x;
   pr.c[iy] = y;
   pr.c[iz] = z;
   return pr;
  }
/* espace affine cartesien 3D */
/* R^3 -> CA(3) */
VECH vh4_CA(Flottant w, Flottant x, Flottant y, Flottant z)
  {
   POINT pr;
   pr = vh_crea_CA(3);
   pr.c[iw] = w;
   pr.c[ix] = x;
   pr.c[iy] = y;
   pr.c[iz] = z;
   return pr;
  }
/* point cartesien 3D */
POINT pt(Flottant x, Flottant y, Flottant z)
  {
   return vh4_CA(1,x,y,z);
  }
/* vecteur cartesien 3D */
VECTEUR vec(Flottant x, Flottant y, Flottant z)
  {
   return vh4_CA(0,x,y,z);
  }
/* point du plan xOy */
/* R^2 -> CA(3) */
POINT pt_xy(Flottant x, Flottant y)
  {
   return vh4_CA(1,x,y,0);
  }
/* vecteur du plan xOy */
VECTEUR vec_xy(Flottant x, Flottant y)
  {
   return vh4_CA(0,x,y,0);
  }
/* point origine 3D */
POINT pt_or3()
  {
   return vh4_CA(1,0,0,0);
  }

/* Extraction des coordonnees cartesiennes */
/* CA(m) -> R */ 
Flottant x_pt(POINT p)
  {
   Flottant w;
   w = p.c[0];
   if(w == 0.)
     {
      fprintf(stdout,"ERREUR : point a l'infini\n");
      return p.c[1];
     }
   return (p.c[1])/w;
  }
Flottant y_pt(POINT p)
  {
   Flottant w;
   int m;
   m = dim_pt(p);
   w = p.c[0];
   if(m<2)
     {
      fprintf(stdout,"ERREUR : dim=%d < 2",m);
      return 0.;
     }
   if(w == 0.)
     {
      fprintf(stdout,"ERREUR : point a l'infini\n");
      return p.c[2];
     }
   return (p.c[2])/w;
  }
Flottant z_pt(POINT p)
  {
   Flottant w;
   int m;
   m = dim_pt(p);
   w = p.c[0];
   if(m<3)
    {
     fprintf(stdout,"ERREUR : dim=%d < 3",m);
     return 0.;
    }
   if(w == 0.)
     {
      fprintf(stdout,"ERREUR : point a l'infini\n");
      return p.c[3];
     }
   return (p.c[3])/w;
  }

/* produit vectoriel */
/* vecCA(3) x vecCA(3) -> vecCA(3) */
VECTEUR vec_pvec(VECTEUR u, VECTEUR v)
 {
  VECTEUR w;
  w = pt_crea(3);
  w.c[0] = 0;
  w.c[1] = u.c[2]*v.c[3]-u.c[3]*v.c[2];
  w.c[2] = u.c[3]*v.c[1]-u.c[1]*v.c[3];
  w.c[3] = u.c[1]*v.c[2]-u.c[2]*v.c[1];
  return w;
 }

#undef iw 
#undef ix 
#undef iy 
#undef iz
