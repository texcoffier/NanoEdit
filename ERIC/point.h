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
/************************/
/* ESPACES HOMOGENES mD */  
/************************/

/* Types des espaces */

/* Types geometriques :
   - entites : P = point; V = vecteur; par defaut P
   - geometrie : Af = affine; Pr = projective; par defaut Af
   => 4 cas d'espaces (theoriques) : 
   - cas affine Af : points (w=1) espace affine EAm 
		     vecteurs (w=0) espace vectoriel EVm
   - cas projectif Pr : 
	     + points a distance finie (w=/=0) ~ espace affine,
	     + points a l'infini (w=0) 
	       hyperplan projectif, ~ espace projectif ~ EVm/R 
   plongements et projections : 
	    + Af P -> Pr P et Pr P -> Af P (xi/w) si w=/=0
	    + Af V -> Pr V (direction a l'infini)  
*/  
/* Systemes de coordonnees : 
   coordonnees homogenes (x0,...,xm), espace R^{m+1} = {(x0,...,xm)^T};
   les systemes de coordonnees correspondent aux types de reperes : 
   - C = coordonnees cartesiennes :
     reperes (p0,v1,...,vm), hyperplan (w(x) = x0)
   - B = coordonnees barycentriques :
     reperes (p0,p1,..,pm), hyperplan (w(x) = x0+...+xm)
   coordonnees par defaut : C
   - mais il peut en exister d'autres :
     repere (p0,p1,v2) => coordonnees (x0,x1,x2) avec :
		- w = x0 + x1 
		- x0 + x1 = 1 pour un point
		- x0 + x1 = 0 pour un vecteur
   - M = coordonnees mixtes
     definies par (w0,...,wm) dans {0,1}^m+1
     reperes (pv0,...,pvm), 
     w(x) = w0 x0 + ... + wm xm avec wi = 1 si pvi point et 0 si pvi vecteur 
       - w(x) = 1 pour un point 
       - w(x) = 0 pour un vecteur 
*/
/* Espaces (numeriques) :
   => 8 cas d'espace : 
   CA = espace affine cartesien :
         CAp = espace affine C^m(R) = {x0 = 1};
         CAv = espace vectoriel associe CV^m(R) = {x0 = 0};
   BA = espace affine barycentrique : 
         BAp = espace affine B^m(R) = {x0+x1+...+xm = 1};
         BAv = espace vectoriel associe BV^m(R) = {x0+x1+..+xm = 0};
   CP = espace projectif cartesien :
	 CPp = points affines {x0 =/= 0}, 
	 CPv = points a l'infini {x0 = 0} 
   BP = espace projectif barycentrique : 
	 BPp = points affines {x0+...+xm =/= 0}, 
	 BPv = points a l'infini {x0+...+xm = 0} 
*/ 
/* Types de structuration de l'espace de controle */
/* Coordonnees homogenes R^J avec J ensemble d'indices 
   indices i correspondant a des elements de controle 
     (sommets ou aretes) de grilles ou de graphes.
   Traduction : i -> point pi ou vecteurs vi de controle 
		grille J -> P = (p_i) passage
		i -> coordonnee xi d'un point de l'espace d'iteration
		grille J -> x = (x_i)   
   Cas :
	- reperes ordinaires : 
	  arbres de racine 0 et de feuilles 1,..,m
	  sommets = points de controle, arcs = vecteurs de controle
	  + droite Ox : R_1 = 0-->1  repere (p0,p1) ou (p0,v01)
				     coordonnees (b0,b1) ou (1,c01)
	  + plan xOy :  R_2 = 2
			      ^
			      |
			      0-->1  repere (p0,p1,p2) ou (p0,v01,v02)
          + espace Oxyz : R_3 
	- grilles regulieres : produits de simplexes
	  caracterises par une suite de couple d'entiers :
	  (n1,m1),...,(mk,mk) -> S_n1(m1) X ... X S_nk(mk)
	  + S_n1 x ... x S_nk caracterise les topologies
	    et les formes des figures : 
	    S_1 = arcs, S_2 = carreaux triangulaires,
	    S_1 x S_1 = quadrilateres, S_3 = tetraedriques, ...
	    (relation avec les structures de subdivision (IFS) ?)
	  + m1, ... ,mk caracterise les degres (nombre de cotes) 
        - arbres, graphes
	    (relation avec les automates ?)
	- cartes
	    (relation avec les structures de subdivision (IFS) ?)
        - conversions ?
*/

/* Type physique : vecteur homogene (m+1)D */

#define Msq_systeme_de_coordonnee 1
#define TE_Cartesien              0
#define TE_Barycentrique          1

#define Msq_geometrie             2
#define TE_Affine                 0
#define TE_Projectif              2

typedef int t_type_espace ;

#define TE_CA (TE_Cartesien    |TE_Affine   )
#define TE_CP (TE_Cartesien    |TE_Projectif)
#define TE_BA (TE_Barycentrique|TE_Affine   )
#define TE_BP (TE_Barycentrique|TE_Projectif)

typedef struct
{
  int dim ;
  t_type_espace type_espace ;
} t_espace ;

typedef struct
{
  t_espace espace ;
  Flottant *c ;
} VECH ;
/*
typedef struct {int dim; Flottant *c;} VECH; 
*/
#define POINT VECH
#define VECTEUR VECH 

#define TYPE_ESPACE(X) ((X)->espace.type_espace)
#define SYSTEME_DE_COORDONNEE(X) ((X) & Msq_systeme_de_coordonnee)
#define             GEOMETRIE(X) ((X) & Msq_geometrie            )

#define     TE_CARTESIEN(X) ( SYSTEME_DE_COORDONNEE(X) == TE_Cartesien    )
#define TE_BARYCENTRIQUE(X) ( SYSTEME_DE_COORDONNEE(X) == TE_Barycentrique)
#define        TE_AFFINE(X) (             GEOMETRIE(X) == TE_Affine       )
#define     TE_PROJECTIF(X) (             GEOMETRIE(X) == TE_Projectif    )

#define     CARTESIEN(X)     TE_CARTESIEN(TYPE_ESPACE(X))
#define BARYCENTRIQUE(X) TE_BARYCENTRIQUE(TYPE_ESPACE(X))
#define        AFFINE(X)        TE_AFFINE(TYPE_ESPACE(X))
#define     PROJECTIF(X)     TE_PROJECTIF(TYPE_ESPACE(X))

/***********/
/* Espaces */
/***********/

void ecrit_esp(FILE *f, char *nom, t_espace esp);
t_espace esp_CA(int m);
t_espace esp_BA(int m);
t_espace esp_CP(int m);
t_espace esp_BP(int m);
int dim_esp(t_espace esp);
int dim_esph(t_espace esp);
Booleen C_esp(t_espace esp);
Booleen B_esp(t_espace esp);
Booleen A_esp(t_espace esp);
Booleen P_esp(t_espace esp);
Booleen CA_esp(t_espace esp);
Booleen BA_esp(t_espace esp);
Booleen CP_esp(t_espace esp);
Booleen BP_esp(t_espace esp);

Booleen egal_esp(t_espace esp1, t_espace esp2);
t_espace esp_comp_interne(t_espace esp1, t_espace esp2);

/**********/
/* Points */
/**********/

/*------------------------------------*/
/* Representants : vecteurs homogenes */
/*------------------------------------*/

/* Operations informatiques */

VECH vh_crea_esp(t_espace esp);
VECH vh_crea_BA(int m);
VECH vh_crea_CA(int m);
POINT pt_crea(int m);
void destru_pt(POINT p);
void ecrit_vh(FILE *f, char *nom, VECH p);
void ecrit_pt(FILE *f, char *nom, POINT p);
VECH vh_dupli(VECH p);
POINT pt_dupli(POINT p);

/* Operations vectorielles homogenes */
VECH vh_e(int m, int i);
t_espace esp_vh(VECH vh);
Flottant w_vh(VECH vh);
/*
VECH vh_scal(Flottant r, VECH vh);
*/
VECH vh_caf(VECH p0, VECH p1, Flottant r);
VECH vh_mil(VECH p0, VECH p1);

/*-----------------*/
/* Espaces affines */
/*-----------------*/

/* Operations 0-aires */
POINT pt_or(int m);

/* Operations unaires */
int dim_pt(POINT p);
Flottant norme2_pt(POINT p);
VECTEUR vec_scal(Flottant r, VECTEUR v);
POINT car_pt(POINT p);
POINT car3_pt(POINT p);

/* Operations binaires */
/*
Flottant dist_pt(POINT p, POINT q);
Flottant prod_scal(VECTEUR v1, VECTEUR v2);
Flottant norme_vec(VECTEUR v);
*/
POINT pt_caf(POINT p0, POINT p1, Flottant r);
POINT pt_mil(POINT p0, POINT p1);
POINT pt_add(POINT p0, VECTEUR v1);
POINT pt_pcar(POINT p1, POINT p2);
VECTEUR vec_pt2(POINT p0, POINT p1);
/*
VECTEUR vec_add(VECTEUR v1, VECTEUR v2);
*/

/*--------------------*/
/* Espaces projectifs */
/*--------------------*/



/*--------------*/
/* Bibliotheque */
/*--------------*/

/* Points 1D */
VECH vh2(Flottant w, Flottant x);
VECH vh2_CA(Flottant w, Flottant x);
POINT pt1(Flottant x);
VECTEUR vec1(Flottant x);

/* Points et vecteurs 2D */
VECH vh3(Flottant w, Flottant x, Flottant y);
VECH vh3_CA(Flottant w, Flottant x, Flottant y);
POINT pt2(Flottant x, Flottant y);
VECTEUR vec2(Flottant x, Flottant y);

/* Points et vecteurs 3D */
VECH vh4(Flottant w, Flottant x, Flottant y, Flottant z);
POINT pt_pj(Flottant x, Flottant y, Flottant z, Flottant w);
VECH vh4_CA(Flottant w, Flottant x, Flottant y, Flottant z);
POINT pt(Flottant x, Flottant y, Flottant z);
VECTEUR vec(Flottant x, Flottant y, Flottant z);
POINT pt_xy(Flottant x, Flottant y);
VECTEUR vec_xy(Flottant x, Flottant y);
POINT pt_or3();
Flottant x_pt(POINT p);
Flottant y_pt(POINT p);
Flottant z_pt(POINT p);
VECTEUR vec_pvec(VECTEUR u, VECTEUR v);
