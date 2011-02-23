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
#include "u_dll.h"

#include <stdio.h>

#define SYMBOLE_DANS_PROGRAMME_PRIORITAIRE

#ifdef HAVE_DLFCN_H
#include <dlfcn.h>
#else
#include <dl.h>
#endif

DLL u_ouvre_dll(const char *nom)
{
  DLL r ;

#ifdef HAVE_DLFCN_H
  r = dlopen(nom, RTLD_NOW) ;
  if ( r == NULL )
    fprintf(stderr,"dlopen %s : %s\n", nom, dlerror()) ;
#else
  r = (void*)shl_load(nom, BIND_DEFERRED, 0) ;
  if ( r == NULL )
    perror(nom) ;
#endif

  return(r) ;
}

void (*u_prend_fonction(DLL r, const char *nom))()
{
  void *fct, *fct_main ;

#ifdef HAVE_DLFCN_H
#ifdef SYMBOLE_DANS_PROGRAMME_PRIORITAIRE
  static DLL main_program = NULL ;

  if ( main_program == NULL )
    main_program = dlopen(NULL, 0) ;

  fct_main = dlsym((void*)main_program, nom) ;
#endif

  fct = dlsym((void*)r, nom) ;


#ifdef SYMBOLE_DANS_PROGRAMME_PRIORITAIRE
  if ( fct && fct_main && r!=NULL )
    {
      fprintf(stderr
	      , "%s:%d Même nom (%s) de fonction dans le main et le .so\n"
	      , __FILE__, __LINE__, nom
	      ) ;
    }
  if ( fct_main )
    fct = fct_main ;
#endif

  if ( fct==NULL && r!=NULL )
    {
      fprintf(stderr,"dlsym %s :\n", nom) ;
      fprintf(stderr,"dlsym %s : %s\n", nom, dlerror()) ;
    }
#else
  if ( shl_findsym((shl_t*)&r, nom, TYPE_PROCEDURE, &fct) )
    {
      fct = NULL ;
      perror(nom) ;
    }
#endif

  return( (void (*)())fct) ;
}

void (*u_prend_fonction_(DLL r, const char *nom))()
{
  void *fct ;

#ifdef HAVE_DLFCN_H
  fct = dlsym((void*)r, nom) ;
#else
  if ( shl_findsym((shl_t*)&r, nom, TYPE_PROCEDURE, &fct) )
    fct = NULL ;
#endif

  return( (void (*)())fct) ;
}

void u_ferme_dll(DLL r)
{
#ifdef HAVE_DLFCN_H
    dlclose((void*)r) ;
#else
    if ( shl_unload((DLL)(r)) )
      perror("shl_unload") ;
#endif
}
