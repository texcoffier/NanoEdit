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
#include "util.h" 

/**********************/
/* Utilitaires divers */
/**********************/

/*----------*/
/* Fichiers */
/*----------*/

FILE *deb_essai(char *nom)
 { 
  FILE *f;
  f = fopen(nom,"a");
  if (f == NULL)
    {
    perror(nom) ;
    exit(0);
    }
  fprintf(f,"DEBUT ESSAI **********\n");
  return f;
 }

void fin_essai(FILE *f)
 {
  fprintf(f,"FIN ESSAI ************\n");
  fclose(f);
 }

/* ecriture de message dans un fichier */
void ecrit_mess(FILE *f, char *mess)
 {
  fprintf(f,"%s\n", mess);
 }

/* ecriture d'entier dans un fichier */
void ecrit_int(FILE *f, char *nomi, int m)
 {
  fprintf(f,nomi);
  fprintf(f," = %d \n",m);
 }

/* ecriture de flottant dans un fichier */
void ecrit_float(FILE *f, char *nomr, float r)
 {
  fprintf(f,nomr);
  fprintf(f," = %f \n",r);
 }

/*----------------------------*/
/* Operations sur les entiers */
/*----------------------------*/

int int_min(int m1, int m2)
 {
  if(m1 <= m2) return m1;
  return m2;
 }

int int_max(int m1, int m2)
 {
  if(m1 >= m2) return m1;
  return m2;
 }

int impair(int m)
 {
  return m&1;
 }

/*------------------------------*/
/* Operations sur les flottants */
/*------------------------------*/

float min(float r1, float r2)
 {
  if(r1 <= r2) return r1;
  return r2;
 }

float max(float r1, float r2)
 {
  if(r1 >= r2) return r1;
  return r2;
 }

/*-------------------------*/
/* Operations sur les mots */
/*-------------------------*/

/* Operations informatiques */

/* Creation de mot */
MOT mot_crea(int n)
 {
  MOT motr;
  motr.nb = n;
  motr.c = (char *) malloc (n*sizeof(char));
  return motr;
 }
/* Destruction de mot */
void destru_mot(MOT mot)
 {
  free(mot.c);
 }
/* Impression de mot */
void ecrit_mot(FILE *fich, char *nom, MOT mot)
 {
  fprintf(fich,"%s = %s\n",nom,mot.c); 
 }

/* Operations n-aires */

/* Longueur de mot */
int long_mot(MOT mot)
 {
  return mot.nb;
 }
/* Mot defini a partir d'une chaine */
MOT mot(int n, char *chaine)
 {
  MOT motr;
  motr = mot_crea(n);
  motr.c = chaine;
  return motr;
 }
/* Mot defini a partir d'un entier */
MOT mot_int(int n, int m)
 {
  MOT motr;
  motr = mot_crea(n);
  sprintf(motr.c,"%d",m);
  return motr;
 }
/* Mot defini a partir d'un flottant */
MOT mot_flot(int n, Flottant r)
 {
  MOT motr;
  motr = mot_crea(n);
  sprintf(motr.c,"%f",r);
  return motr;
 }
/* Mot d'identification */
MOT mot_nom(char *chaine)
 {
  return mot(LMAX_NOM,chaine);
 }
/* Concatenation de deux chaines */
MOT conca(MOT mot1, MOT mot2)
 {
  MOT motr;
  motr = mot_crea(mot1.nb+mot2.nb);
  sprintf(motr.c,"%s%s",mot1.c,mot2.c);
  return motr;
 }
/* Concatenation de trois chaines */
MOT conca3(MOT mot1, MOT mot2, MOT mot3)
 {
  MOT motr;
  motr = mot_crea(mot1.nb+mot2.nb+mot3.nb);
  sprintf(motr.c,"%s%s%s",mot1.c,mot2.c,mot3.c);
  return motr;
 }

/*******************/
/* Suite d'entiers */
/*******************/

/* Operations informatiques */

/* Creation d'une liste d'entiers */
L_INT li_crea(n)
 {
  L_INT lir;
  lir.nb = n;
  lir.c = (int *) malloc (n*sizeof(int));
  return lir;
 }
/* Destruction d'une liste d'entiers */
void destru_li(L_INT li)
 {
  if(li.nb>0) free(li.c);
 }
/* Duplication d'une liste d'entiers */
L_INT li_dupli(L_INT li)
 {
  L_INT lir;
  int n,i;
  n = li.nb;
  lir = li_crea(n);
  for(i=0;i<n;i++)
    lir.c[i] = li.c[i];
  return lir;
 }
/* Impression d'une liste d'entiers */
void ecrit_li(FILE *fich, char *nom, L_INT li)
 {
  int n,j;
  n = li.nb; 
  fprintf(fich,"%s = (",nom);
  if(n>0)
    {
     n = n-1; 
     for(j=0;j<n;j++)
       fprintf(fich,"%d,",li.c[j]);
     fprintf(fich,"%d",li.c[n]);
    }
  fprintf(fich,")\n");
 }

/* Operations unaires */

/* Longueur d'une liste */
int long_li(L_INT li)
 {
  return li.nb;
 }
/* Element maximum d'une liste d'entiers non vide */
int max_li(L_INT li)
 {
  int jr,n,i;
  n = li.nb;
  if(n==0)
    {
     fprintf(stdout,"ERREUR : LISTE VIDE\n");
     return 0;
    }
  jr = li.c[0];
  for(i=1;i<n;i++)
    jr = int_max(jr,li.c[i]);
  return jr;
 }
/* Premier element */
int deb_li(L_INT li)
 {
  if(li.nb==0) fprintf(stdout,"ERREUR : LISTE VIDE\n");
  return(li.c[0]);
 }
/* Dernier element */
int fin_li(L_INT li)
 {
  if(li.nb==0) fprintf(stdout,"ERREUR : LISTE VIDE\n");
  return(li.c[li.nb-1]);
 }
/* jeme element (numerotation de 1 a nb) */
int el_li(L_INT li, int j)
 {
  if(li.nb<j-1) fprintf(stdout,"ERREUR : LISTE TROP PETITE\n");
  return(li.c[j-1]);
 }
/* Suite inverse */
L_INT li_inv(L_INT li)
 {
  L_INT lir;
  int n,i;
  n = li.nb;
  lir = li_crea(n);
  n = n-1;
  for(i=0;i<=n;i++)
    lir.c[i] = li.c[n-i];
  return lir;
 }

/* Operations binaires */

/* Extraction d'une suite d'entier ou composition de deux suites d'entier */
/* condition : elements de li2 dans [0,li1.nb] */  
L_INT li_extr(L_INT li1, L_INT li2)
 {
  L_INT lir;
  int n,i;
  n = li2.nb; 
  lir = li_crea(n);
  for(i=0;i<n;i++)
    lir.c[i] = li1.c[li2.c[i]];
  return lir;
 }
L_INT li_conca(L_INT li1, L_INT li2)
 {
  L_INT lir;
  int n1,n,i;
  n1 = long_li(li1);
  n = n1 + long_li(li2); 
  lir = li_crea(n);
  for(i=0;i<n1;i++)
    lir.c[i] = li1.c[i];
  for(i=n1;i<n;i++)
    lir.c[i] = li2.c[i-n1];
  return lir;
 }

/* Bibliotheque */

/* Liste identite : 0,1,..,n-1 */
L_INT li_id(int n)
 {
  L_INT lir;
  int i;
  lir = li_crea(n);
  for(i=0;i<n;i++)
    lir.c[i] = i;
  return lir;
 }
/* Liste vide */ 
L_INT li_vide()
 {
  return li_crea(0);
 }
L_INT li_0()
 {
  return li_crea(0);
 }
/* Liste a un element */ 
L_INT li1(int j0)
 {
  L_INT lir;
  lir = li_crea(1);
  lir.c[0] = j0;
  return lir;
 }
/* Creation d'un couple d'entiers */ 
L_INT li2(int j0, int j1)
 {
  L_INT lir;
  lir = li_crea(2);
  lir.c[0] = j0;
  lir.c[1] = j1;
  return lir;
 }
/* Creation d'un triplet d'entiers */ 
L_INT li3(int j0, int j1, int j2)
 {
  L_INT lir;
  lir = li_crea(3);
  lir.c[0] = j0;
  lir.c[1] = j1;
  lir.c[2] = j2;
  return lir;
 }
/* Creation d'un quadruplet d'entiers */ 
L_INT li4(int j0, int j1, int j2, int j3)
 {
  L_INT lir;
  lir = li_crea(4);
  lir.c[0] = j0;
  lir.c[1] = j1;
  lir.c[2] = j2;
  lir.c[3] = j3;
  return lir;
 }
/* Creation d'un quintuplet d'entiers */ 
L_INT li5(int j0, int j1, int j2, int j3, int j4)
 {
  L_INT lir;
  lir = li_crea(5);
  lir.c[0] = j0;
  lir.c[1] = j1;
  lir.c[2] = j2;
  lir.c[3] = j3;
  lir.c[4] = j4;
  return lir;
 }

