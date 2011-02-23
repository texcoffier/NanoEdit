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
/*
 * Créé le Thu May 28 14:51:04 1998 par Thierry EXCOFFIER
 */

#ifndef U_HTTPD_H
#define U_HTTPD_H

typedef char Url[32768] ;	/* Changer ici la taille maxi d'URL */
#define NOMBRE_DE_VALEUR 1000	/* Limite sur les "?nom=valeur" dans une URL */

/*
 * Pour l'utilisation des fonctions suivantes, 
 * Regardez exemple.c
 */
void http_init(int *port, int *socket) ;
void http_stop(int socket) ;
void http(int socket) ;
void goto_racine(int port, const char *url) ;

void enregistre_url(const char *url,
		    void (*fonction)(FILE *, char*, void *, char**, int),
		    const char *mime_type,
		    void *donnee,
		    int forked,
		    int ferme_fichier) ;
/*
 * Remplace dans une chaine de caractere normal les codes
 * gênant pour HTML : & < >
 * Elle retourne une chaine allouee statiquement.
 * IL NE FAUT PAS FAIRE :
 *       fprintf(c,"%s et %s", remplace_code(a), remplace_code(b))
 * Cela imprimerai deux fois la meme chose....
 */
char *remplace_code(const char* old) ;
/*
 * Demande à Netscape de charger une URL
 */
void goto_url(const char *url) ;

#endif
