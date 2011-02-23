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
/*************/
/* GRAPHISME */
/*************/

/*------------------*/
/* Espace graphique */
/*------------------*/

/* espace graphique =
   espace support x espace couleurs x espace des normales x ...   
   type = format de lecture d'un vecteur homogene
     <point support><couleur><vecteur normale> ... 
   sorties standard : points graphiques cartesiens du type :
   - (pt) 2D ou 3D
   - (pt,couleur) avec couleur 0D(N&B),1D(gris),3D(RVB)
   - (pt,couleur,normale) avec normale 0D(pas de normale),3D(normale au sommet)
*/

typedef struct {int dims; int dimc; int dimn; int nc;} ESP_GRAPH;

/* vecteur homogene graphique */
 
typedef struct {ESP_GRAPH eg; VECH vh;} VECG; 

/*----------*/
/* Couleurs */
/*----------*/

/* Espace RVB = CA(3) */
/*
typedef struct {Flottant r; Flottant v; Flottant b;} RVB;
*/
#define RVB POINT

/*
#define CMAX 495
*/
#define CMAX 1 
#define VMAX 7 

/* Type de degrade */

typedef enum {Continu, Discret, Alterne, Moire, Sans} DEGRADE;

/* Gamme de couleur ou de gris */

typedef struct {DEGRADE degrade;
		RVB coul0; RVB coul1; 
		Flottant r0; Flottant r1;
                Flottant c; int ic;} 
		GAMME;

/*--------*/
/* Camera */
/*--------*/
/* - passv matrice de passage : modele 2 ou 3d -> image 2d (visu tima) 
   - passm matrice de passage : modele 2 ou 3d -> camera 2 ou 3d (mod)
*/

typedef struct {int larg; int haut; 
		Flottant lfen;
		MATH passv;
		MATH passm;
		GAMME gamme;} 
		CAMERA;

/*------------------*/
/* Espace graphique */
/*------------------*/

ESP_GRAPH eg_crea(int ns, int nc, int nn);
void ecrit_eg(FILE *fich, char *nom, ESP_GRAPH eg);
int dims_eg(ESP_GRAPH eg);
int dimc_eg(ESP_GRAPH eg);
int dimn_eg(ESP_GRAPH eg);
int nc_eg(ESP_GRAPH eg);
ESP_GRAPH eg_2d_coul();
ESP_GRAPH eg_3d();
ESP_GRAPH eg_3d_gris();
ESP_GRAPH eg_3d_coul();
ESP_GRAPH eg_3d_coul_norm();

/*----------*/
/* Couleurs */
/*----------*/

RVB coul_rvb(Flottant r, Flottant v, Flottant b);
void ecrit_coul(FILE *f, char *nomcoul, RVB coul);
RVB coul_dupli(RVB coul);
Flottant r_coul(RVB coul);
Flottant v_coul(RVB coul);
Flottant b_coul(RVB coul);
RVB coul_caf(RVB coul0, RVB coul1, Flottant t);

RVB noir();
RVB blanc();
RVB rouge();
RVB vert();
RVB bleu();
RVB jaune();
RVB violet();

/* Gammes */ 

GAMME gam_crea(DEGRADE degr, RVB coul0, RVB coul1, Flottant r0, Flottant r1);
void destru_gam(GAMME gam);
void ecrit_gam(FILE *f, char *nomgam, GAMME gam);

GAMME gam_nit(Flottant n);
int gris_degr(GAMME gam, Flottant t);

/*---------*/
/* Cameras */
/*---------*/

CAMERA cam_crea(int larg, int haut, Flottant lfen, MATH tvm, MATH tm, 
		GAMME gamme);
void ecrit_cam(FILE *f, char *nom, CAMERA cam);
int larg_cam(CAMERA cam);
int haut_cam(CAMERA cam);
Flottant lfen_cam(CAMERA cam);
CAMERA cam_2d(int larg, int haut, Flottant lfen);
CAMERA cam_3d(int larg, int haut, Flottant lfen);
