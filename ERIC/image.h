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
/**********/
/* IMAGES */
/**********/

/*-------*/
/* Image */
/*-------*/

/* Tableau-image (format exco) */

typedef struct t_image {
			char *nom_image; 
			Flottant *image; 
			int largeur; int hauteur;
		       } Image;

/* Tableau-image (format et) */

#define TIMA MATH

/* Type de formats de sortie */

typedef enum {Article, ImprA4, Diapo_essai, Diapo_h, Diapo} SORTIE;

#define RESOL 5   /* pixels/mm */


/*-------------------*/
/* Operations Images */
/*-------------------*/

/* Operations informatiques */
TIMA tima_crea(int larg, int haut);
void destru_tima(TIMA tima);
void ecrit_tima(FILE *f, char *nomtima, TIMA tima);

FILE *deb_pgm(MOT iprod, int larg, int haut);
void fin_pgm(FILE *f);
FILE *deb_ppm(MOT iprod, int larg, int haut);
void fin_ppm(FILE *f);
void xv_pgm(MOT iprod);
void xv_ppm(MOT iprod);
void xv(MOT ifima);

/* Operations sur les images */
int larg_tima(TIMA tima);
int haut_tima(TIMA tima);
void raz_tima(TIMA tima);
TIMA tima_nul(int larg, int haut);
void tima_xy(TIMA tima, Flottant x, Flottant y);
void tima_carre(TIMA tima, Flottant x, Flottant y, Flottant c);
void tima_xym(TIMA tima, Flottant x, Flottant y);
void tima_xy3p(TIMA tima, Flottant x, Flottant y);

/* Operations unaires */
void pgm_tima(MOT iprod, GAMME gam, TIMA tima);
void ppm_tima(MOT iprod, GAMME gam, TIMA tima);

/* Echelle : passage fenetre -> image */
MATH echel_larg(TIMA tima, Flottant lfen); 
MATH echel_inv_larg(TIMA tima, Flottant lfen);

/*---------------*/
/* Bibliotheques */
/*---------------*/

/* Bibliotheque de formats d'images */
TIMA tima_sortie(SORTIE sortie);
