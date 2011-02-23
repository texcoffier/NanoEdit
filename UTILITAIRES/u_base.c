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
#include "u_memoire.h"
#include "u_base.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include "s_string.h"
/*
 *
 */

const char *u_ligne(FILE *f)
{
  int longueur ;
  Buffer buf ;
  int c ; 
  static Chaine s ;

  s_efface(&s) ;
  for(;;)
    {
      if ( fgets(buf, sizeof(Buffer), f) == NULL )
	{
	  /*
	   * fin de fichier.
	   * Cela peut etre après une ligne suite
	   * quand il manque le \n à la fin du fichier
	   */
	  if ( s_longueur(&s) == 0 )
	    return(NULL) ;
	  else
	    return(s_char(&s)) ;
	}
      longueur = strlen(buf) ;
      s_strcat(&s, buf) ;	/* Concatène dans tous les cas */
      if ( buf[longueur-1] == '\n' )
	{
	  /*
	   * On a lu une ligne entière car on a lu le \n
	   */
	  c = fgetc(f) ;
	  if ( c != '\t' && c != ' ' )
	    {
	      /*
	       * Il n'y a pas de ligne suite.
	       * On enlève le dernier \n
	       */
	      ungetc(c,f) ;
	      s_putchar_precedent(&s, '\0') ;
	      return(s_char(&s)) ;
	    }
	}
    }
}

void lit_fichier_complet(const char *nom, char **data, int *size)
{
  FILE *f ;

  *size = taille_fichier(nom) ;
  if ( *size )
    {
      f = fopen(nom, "r") ;
      if ( f == NULL )
	{
	  *size = 0 ;
	  perror(nom) ;
	  return ;
	}
      *data = malloc(*size) ;
      fread(*data, 1, *size, f) ;
      fclose(f) ;
    }
  
}

void reaffecte_chaine_avec_copie(char **ancienne, const char *nouvelle)
{
  int longueur ;

  if ( *ancienne == NULL )
    {
      *ancienne = strdup(nouvelle) ;
      return ;
    }

  longueur = strlen(nouvelle)+1 ;
  REALLOUER(*ancienne, longueur) ;
  memcpy(*ancienne, nouvelle, longueur) ;
}

void reaffecte_chaine_sans_copie(char **ancienne, char *nouvelle)
{
  if ( *ancienne )
    free(*ancienne) ;
  *ancienne = nouvelle ;
}
/*
 * Mémoire virtuelle utilisee par le processus
 */
int u_memoire()
{
  FILE * f ;
  char nom[99] ;
  int m ;

  m = 0 ;
  sprintf(nom, "/proc/%d/status", (int)getpid()) ;
  f = fopen(nom, "r") ;
  if ( f )
    {
      while( fgets(nom, sizeof(nom), f) )
	{
	  if ( strncmp(nom, "VmSize", 6) == 0 )
	    {
	      sscanf(nom+7, "%d", &m) ;
	      break ;
	    }
	}
      fclose(f) ;
    }
  return(m) ;
}

int u_memoire_augmentee(const char *message)
{
  static int mem = 0 ;
  int m ;

  m = u_memoire() ;
  if ( m > mem )
    {
      eprintf("%s (+%dk)\n", message, m-mem) ;
      mem = m ;
      return 1 ;
    }
  return 0 ;
}

static void fin_fils()
{
  _exit(0) ;			/* Ne pas réveiller les widgets et fichiers */
}

int u_fork(int fildes_a_garder_ouvert)
{
  int i, f ;

  fflush(stderr) ;
  fflush(stdout) ;
  f = fork() ;
  if ( f == 0 )
    {
      /* Fils */
      atexit(fin_fils) ;
      /* Ferme les fildes pour éviter le problème de socket ne se fermant
       * pas dans le père à  cause d'un fils
       */
      for(i=3;i<100;i++)
	if ( i != fildes_a_garder_ouvert )
	  close(i) ;
    }
  return(f) ;
}

/*
 *
 */

#undef malloc
#undef free
#undef realloc

struct bloc_memoire
{
  int *p ;
  int taille_en_entier ;
  struct bloc_memoire *suivant ;
} ;

static struct bloc_memoire *globale_bloc_memoire = NULL ;

#define PATERN 9875432

void test_bloc_memoire()
{
  struct bloc_memoire *b ;
return ;
  for(b=globale_bloc_memoire; b; b = b->suivant)
    {
      if ( b->p[0] != PATERN  ||  b->p[b->taille_en_entier-1] != PATERN )
	EXIT ;
    }
}

#define TEST ((void*)0x827180c)

void *mon_malloc(int n)
{
  struct bloc_memoire *b ;

  test_bloc_memoire() ;

  ALLOUER(b,1) ;
  b->taille_en_entier = (n+sizeof(int)-1)/sizeof(int)+2 ;
  b->p = malloc( b->taille_en_entier * sizeof(int) ) ;
  b->p[0] = PATERN ;
  b->p[b->taille_en_entier-1] = PATERN ;
  b->suivant = globale_bloc_memoire ;
  globale_bloc_memoire = b ;

  if ( TEST ==  b->p + 1 )
    eprintf("Malloced\n") ;

  return( b->p + 1 ) ;
}


void mon_free(void *p)
{
  struct bloc_memoire **b, *bb ;
  int *pp ;

  test_bloc_memoire() ;

  pp = (int*)p - 1 ;
  for(b=&globale_bloc_memoire; *b; b = &(*b)->suivant)
    if ( (*b)->p == pp )
      {
	bb = *b ;
	free(bb->p) ;
	*b = bb->suivant ;
	free(bb) ;

  if ( TEST == p )
    eprintf("Freed\n") ;
	return ;
      }
  EXIT ;
}

void *mon_realloc(void *p, int n)
{
  struct bloc_memoire **b ;
  int *pp ;

  test_bloc_memoire() ;

  pp = (int*)p - 1 ;
  for(b=&globale_bloc_memoire; *b; b = &(*b)->suivant)
    if ( (*b)->p == pp )
      {
	(*b)->taille_en_entier = (n+sizeof(int)-1)/sizeof(int)+2 ;
	(*b)->p = realloc( (*b)->p, (*b)->taille_en_entier * sizeof(int) ) ;
	(*b)->p[0] = PATERN ;
	(*b)->p[(*b)->taille_en_entier-1] = PATERN ;
	
  if ( TEST == p )
    eprintf("Reallocated in\n") ;
  if ( TEST == (*b)->p + 1 )
    eprintf("Reallocated out\n") ;
	return( (*b)->p + 1 ) ;
      }
  EXIT ;
  return(NULL) ;
}

char *mon_strdup(const char *s)
{
  char *t ;

  t = mon_malloc( strlen(s) + 1 ) ;
  strcpy(t, s) ;
  return(t) ;
}


Booleen fichier_existe(const char *s)
{
  struct stat st ;

  return( stat(s, &st) == 0 ) ;
}

int taille_fichier(const char *nom)
{
  struct stat st ;

  if ( stat(nom, &st) == 0 )
    return(st.st_size) ;
  else
    return(-1) ;
}
