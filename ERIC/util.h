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
/***************/
/* UTILITAIRES */  
/***************/

#include <stdlib.h>
#include "o_objet.h"
#include "t_base.h"
#include "t_geometrie.h"

/* Valeurs booleennes */

#define Vrai 1
#define Faux 0

/* Mots : chaines de caracteres et longueur (nombre de caracteres) */

typedef struct {int nb; char *c;} MOT;

/* Mots contenant l'adresse du repertoire Modeleur Fractal */

#define MOD_FRAC mot(28,"/home/et/LOGICIELS/MOD_FRAC/")
#define MOTEUR conca(MOD_FRAC,mot(7,"MOTEUR/"))

/* Longueur maximum des noms de productions (images, descriptions, ...) */

#define LMAX_NOM 20

/* Listes d'entiers */

typedef struct {int nb; int *c;} L_INT;

/*------------*/
/* Operations */
/*------------*/

/* Ecritures */
FILE *deb_essai(char *nom);
void fin_essai(FILE *f);
void ecrit_mess(FILE *f, char *mess);
void ecrit_int(FILE *f, char *nomi, int m);
void ecrit_float(FILE *f, char *nomr, float r);

/* Operations sur les entiers */
int int_min(int m1, int m2);
int int_max(int m1, int m2);
int impair(int m);

/* Operations sur les flottants */
float min(float r1, float r2);
float max(float r1, float r2);

/*------*/
/* Mots */
/*------*/

/* operations informatiques */
MOT mot_crea(int n);
void destru_mot(MOT mot);
void ecrit_mot(FILE *f, char *nom, MOT mot);

/* operations n-aires */
int long_mot(MOT mot);
MOT mot(int n, char *chaine);
MOT mot_int(int n, int m);
MOT mot_flot(int n, Flottant r);
MOT mot_nom(char *chaine);
MOT conca(MOT mot1, MOT mot2);
MOT conca3(MOT mot1, MOT mot2, MOT mot3);

/*------------------*/
/* Listes d'entiers */
/*------------------*/

/* operations informatiques */
L_INT li_crea(int n);
void destru_li(L_INT li);
L_INT li_dupli(L_INT li);
void ecrit_li(FILE *f, char *nom, L_INT li);

/* operations unaires */
int long_li(L_INT li);
int max_li(L_INT li);
int deb_li(L_INT li);
int fin_li(L_INT li);
int el_li(L_INT li, int j);
L_INT li_inv(L_INT li);

/* operations binaires */
L_INT li_extr(L_INT li1, L_INT li2);
L_INT li_conca(L_INT li1, L_INT li2);

/* bibliotheque */
L_INT li_id(int n);
L_INT li_vide();
L_INT li1(int j0);
L_INT li2(int j0, int j1);
L_INT li3(int j0, int j1, int j2);
L_INT li4(int j0, int j1, int j2, int j3);
L_INT li5(int j0, int j1, int j2, int j3, int j4);
