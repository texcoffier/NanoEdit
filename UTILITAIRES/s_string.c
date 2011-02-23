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
#include "u_base.h"
#include "s_string.h"
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>


/*
 * gestion des printf en memoire
 */

Chaine s_ouvre_ecriture()
{
  Chaine s ;
  s.courant = 0 ;
  s.fin     = 0 ;
  s.taille  = 80 ;
  ALLOUER(s.debut,s.taille) ;
  return(s) ;
}

Chaine s_ouvre_lecture(char * ini)
{
  Chaine s ;
  s.courant = 0 ;
  s.fin     = strlen(ini) ;
  s.taille  = s.fin + 1 ;
  if ( ini )
    s.debut = ini ;
  else
    s.debut = "" ;
  return(s) ;
}

int s_longueur(const Chaine *s)
{
  return( s->fin ) ;
}

void s_ferme(Chaine *s)
{
  free(s->debut) ;
}

char* s_char(Chaine *s)
{
  if ( s->debut==NULL )
    {
      s->debut  = strdup("") ;
      s->taille = 1 ;
      s->fin    = 0 ;
    }
  return(s->debut) ;
}

char* s_dup_courant_fin(Chaine *s)
{
  char *str ;

  ALLOUER(str, s->fin - s->courant + 1 ) ;
  strncpy(str, &s->debut[s->courant], s->fin - s->courant + 1 ) ;
  str[s->fin - s->courant] = '\0' ;
  return(str) ;
}


char s_current(Chaine* s)
{
  if ( s->courant > s->fin )
    s->courant = s->fin ;
  if ( s->courant == s->fin )
    return('\0') ;
  else
    return(s->debut[s->courant]) ;
}

char s_get_char(Chaine *s)
{
  if ( s->courant < s->fin )
    return( s->debut[s->courant++] ) ;
  else
    return( '\0' ) ;
}

char s_saute(Chaine *s, const char *liste)
{
  s->courant += strspn(&s->debut[s->courant], liste ) ;
  return( s_current(s) ) ;
}

char s_saute_sauf(Chaine *s, const char *liste)
{
  s->courant += strcspn(&s->debut[s->courant], liste ) ;
  return( s_current(s) ) ;
}

int s_strstr(Chaine *s, const char *mot)
{
  const char *st ;

  st = strstr(&s->debut[s->courant], mot ) ;
  if ( st==NULL ) return(0) ;
  if ( st - s->debut >= s->fin )
    return(0) ;
  s->courant = st - s->debut ;
  return(1) ;
}

void s_fin(Chaine *s)
{
  s->courant = s->fin ;
}

double s_get_float(Chaine *s)
{
  char *p ;
  double f ; 
  f = strtod(&s->debut[s->courant], &p);
  s->courant += p - &s->debut[s->courant] ;
  if ( s->courant >= s->fin )
    s->courant = s->fin ;
  return(f) ;
}

int s_get_int(Chaine *s)
{
  char *p ;
  int f ; 
  f = strtol(&s->debut[s->courant], &p, 10);
  s->courant += p - &s->debut[s->courant] ;
  if ( s->courant >= s->fin )
    s->courant = s->fin ;
  return(f) ;
}

Chaine s_get_mot(Chaine *s, const char *liste)
{
  Chaine t ;
  t = *s ;
  s_saute_sauf(s,liste) ;
  t = s_extrait(&t,s) ;
  s_deplace(s,1) ;
  return( t ) ;
}

void s_debut(Chaine *s)
{
  s->courant = 0 ;
}

void s_efface(Chaine *s)
{
  s->courant = 0 ;
  s->fin     = 0 ;
  if ( s->debut ) s->debut[0] =  '\0' ;
}

void s_deplace(Chaine *s, int nb)
{
  s->courant += nb ;
  if ( s->courant >= s->fin )
    s->courant = s->fin ;
}

Chaine s_extrait(Chaine *s, Chaine *t)
{
  Chaine r ;

  if ( s->debut != t->debut ||
       s->courant > t->courant )
    {
      eprintf("e_extrait\n") ;
      exit(1) ;
    }
  r = *s ;
  r.fin = t->courant ;
  return(r) ;
}


unsigned int s_nombre_de_mots(Chaine *in,
			      const char *separateur,
			      Booleen accepte_mot_vide)
{
  Chaine s ;
  unsigned int nb, i ;

  s = *in ;
  nb = 0 ;
  while( s.courant != s.fin )
    {
      i = strspn( &s.debut[s.courant], separateur) ;
      if ( s.courant + i > s.fin )
	i = s.fin - s.courant ;	
      if ( accepte_mot_vide && i!=0 )
	nb += i - 1 ;
      s.courant += i ;
      if ( s.courant == s.fin )
	break ;
      
      i = strcspn( &s.debut[s.courant], separateur) ;
      if ( s.courant + i > s.fin )
	i = s.fin - s.courant ;	
      s.courant += i ;
      nb++ ;
    }
  return(nb) ;
}


void s_putchar(Chaine *s, char a)
{
    if ( s->courant+1 >= s->taille )
      {
	s->taille = 2*s->taille + 1 ;
	REALLOUER(s->debut, s->taille) ;
      }
    s->debut[s->courant++] = a ;
    s->debut[s->courant] = '\0' ;
    s->fin      = s->courant ;
}

void s_putchar_precedent(Chaine *s, char a)
{
  if ( s->courant > 0 )
    s->debut[s->courant-1] = a ;
  else
    fprintf(stderr,"s_putchar_precedent sur chaine vide\n") ;
}


void s_strcat(Chaine *s, const char *a)
{
    unsigned int lo ;

    lo = strlen(a) ;
    if ( s->courant+lo >= s->taille )
      {
	s->taille = 2*s->taille + lo + 1 ;
	REALLOUER(s->debut, s->taille) ;
      }
    strcpy( &s->debut[s->courant], a) ;
    s->courant += lo ;
    s->fin      = s->courant ;
}


int s_printf(Chaine *s, const char *format, ...)
{
    va_list ap;
    int i ;
    char buffer[10000] ;

    va_start(ap,format);
#ifdef HAVE_VSNPRINTF
    i = vsnprintf(buffer, sizeof(buffer), format, ap) ;
#else
    i = vsprintf(buffer, format, ap) ;
    if ( strlen(buffer) >= sizeof(buffer) )
      {
	ICI ;
	exit(1) ;
      }
#endif
    va_end(ap);
    s_strcat(s, buffer) ;

    return(i) ;
}
