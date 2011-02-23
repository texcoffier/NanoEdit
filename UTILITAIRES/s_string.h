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
#ifndef S_STRING_H
#define S_STRING_H

#include "u_base.h"

typedef struct s_string
{
  char *debut ;			/* Debut de la zone memoire */
  unsigned int fin ;		/* Dernier caractere normalement le '\0' */
  unsigned int courant ;	/* indice courant */
  unsigned int taille ;	       	/* Taille allouee */
} Chaine ;

/*
 * gestion des printf en memoire
 */

Chaine s_ouvre_ecriture() ;
Chaine s_ouvre_lecture(char * ini) ;
void s_ferme(Chaine *s) ;

char*           s_char           (Chaine *s) ;
char*           s_dup_courant_fin(Chaine *s) ;
char            s_current        (Chaine *s) ;
 
void            s_debut          (Chaine *s) ;
void            s_fin            (Chaine *s) ;
void            s_efface         (Chaine *s) ;
void            s_deplace        (Chaine *s, int nb) ;
char            s_saute          (Chaine *s, const char *liste) ;
char            s_saute_sauf     (Chaine *s, const char *liste) ;
int             s_strstr         (Chaine *s, const char *mot) ;
int             s_longueur       (const Chaine *s) ;

unsigned int    s_nombre_de_mots (Chaine *s,
				  const char *separateur,
				  Booleen accepte_mot_vide) ;
      
void            s_putchar        (Chaine *s, char a) ;
void            s_putchar_precedent(Chaine *s, char a) ;
void            s_strcat         (Chaine *s, const char *a) ;
int             s_printf         (Chaine *s, const char *format, ...);
 
double          s_get_float      (Chaine *s) ;
int             s_get_int        (Chaine *s) ;
char            s_get_char       (Chaine *s) ;
Chaine 		s_get_mot        (Chaine *s, const char *liste) ;
Chaine 		s_extrait        (Chaine *s, Chaine *t) ;


#endif
