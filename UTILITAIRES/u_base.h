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
#ifndef U_BASE_H
#define U_BASE_H

#include <stdio.h>		/* Pour le type FILE */

#if 0
void *mon_malloc(int n) ;
void mon_free(void *) ;
void *mon_realloc(void *, int) ;
char *mon_strdup(const char *s) ;
void test_bloc_memoire() ;

#define malloc(S) mon_malloc(S)
#define free(S) mon_free(S)
#define realloc(S,N) mon_realloc(S,N)
#define strdup(S) mon_strdup(S)
#endif


#ifndef ICI
#define ICI eprintf("%s:%d:\n",__FILE__,__LINE__)
#endif
#define EXIT { ICI ; abort() ; }


#if 0
#define popen(X,Y) (eprintf("%s:%d:popen\n",__FILE__,__LINE__),popen(X,Y))
#define pclose(F) (eprintf("%s:%d:pclose\n",__FILE__,__LINE__),pclose(F))
#define system(F) (eprintf("%s:%d:system\n",__FILE__,__LINE__),system(F),ICI,0)
#endif


/*
 * Le + 1 est la au cas ou le malloc de 0 octets pose des problemes
 */

#define ALLOUER(X,NB) if ( (X = malloc(1+sizeof(*(X)) * (NB) )) == 0 )\
                             { eprintf( "Plus de memoire\n") ; EXIT ; }
#define REALLOUER(X,NB) if ( (X = (X) ? realloc(X,1+sizeof(*(X)) * (NB)) : malloc(1+sizeof(*(X)) * (NB)))  == 0 )\
                             { eprintf( "Plus de memoire\n") ; EXIT ; }
#define CONCATENE(A,B) A ## B
#define TAILLE_TABLE(T) ( sizeof(T) / sizeof(T[0]) )
#define GUILLEMET(A) #A
#define MET_A_0(A) memset(A, 0, sizeof(*A))

#ifndef ABS
#define ABS(X) ( (X)>0 ? (X) : -(X) )
#endif
#ifndef MAX
#define MAX(X,Y) ( (X)>(Y) ? (X) : (Y) )
#endif
#ifndef MIN
#define MIN(X,Y) ( (X)<(Y) ? (X) : (Y) )
#endif
#define MAX3(x,y,z) ( (x)>(y) ? ((x)>(z)?(x):(z)) : ((y)>(z)?(y):(z)) )
#define MIN3(x,y,z) ( (x)>(y) ? ((y)>(z)?(z):(y)) : ((x)>(z)?(z):(x)) )

#ifndef Faux
enum { Faux, Vrai } ;
#endif
typedef char Booleen ;
typedef char Buffer      [1024] ;	/* Les tailles differentes servent */
typedef char Nombre      [1021] ;
typedef char NomFichier  [1020] ;

const char * u_ligne(FILE *f) ;
int u_memoire() ;
int u_memoire_augmentee(const char *message) ;
void reaffecte_chaine_avec_copie(char **ancienne, const char *nouvelle) ;
void reaffecte_chaine_sans_copie(char **ancienne, char *nouvelle) ;
int u_fork(int files_a_garder_ouvert) ;
#define strcpy_overlap(A,B) memmove(A, B, strlen(B)+1)

Booleen fichier_existe(const char *s) ;
int taille_fichier(const char *nom) ;
void lit_fichier_complet(const char *nom, char **data, int *size) ;

#include "eprintf.h"

#ifndef M_PI
#define M_PI 3.141592653589793238462643383279
#endif

#endif


