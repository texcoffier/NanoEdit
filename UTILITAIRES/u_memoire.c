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
#include <stdlib.h>
#include "u_base.h"
#include "u_memoire.h"

void *alloue_zone_memoire(Gestion_memoire *gm, unsigned int taille)
{
  int libre ;
  char *objet ;

  if ( gm->nb_emplacements_vides == 0 )
    {
      if ( gm->nb_objets_alloues == gm->nb_max_objets_alloues )
	{
	  gm->nb_max_objets_alloues = 2*gm->nb_max_objets_alloues + 16 ;
	  REALLOUER(gm->objets_alloues, gm->nb_max_objets_alloues) ;
	}
      libre = gm->nb_objets_alloues++ ;
    }
  else
    libre = gm->emplacements_vides[--gm->nb_emplacements_vides] ;

  objet = malloc(taille + sizeof(double)) ;
  if ( objet == NULL )
    {
      ICI ;
      abort() ;
    }
  
  gm->objets_alloues[libre] = objet + sizeof(double) ;
  INDEX_OBJET(gm->objets_alloues[libre]) = libre ;

  return(gm->objets_alloues[libre]) ;
}

void *realloue_zone_memoire(Gestion_memoire *gm, void *objet
			    , unsigned int taille)
{
  char *o ;
  unsigned int index ;

  if ( objet == NULL )
    return( alloue_zone_memoire(gm, taille) ) ;

  if ( !MEMOIRE_VALIDE(*gm, objet) )
    abort() ;

  index = INDEX_OBJET(objet) ;
  o = realloc( (char*)objet - sizeof(double), taille + sizeof(double)) ;
  gm->objets_alloues[index] = o + sizeof(double) ;
  return(gm->objets_alloues[index]) ;
}

void libere_zone_memoire(Gestion_memoire *gm, void *objet)
{
  if ( objet == NULL )
    return ;


  if ( !MEMOIRE_VALIDE(*gm, objet) )
    abort() ;

  if ( gm->nb_emplacements_vides == gm->nb_max_emplacements_vides )
    {
      gm->nb_max_emplacements_vides = 2*gm->nb_max_emplacements_vides + 16 ;
      REALLOUER(gm->emplacements_vides, gm->nb_max_emplacements_vides) ;
    }

  gm->objets_alloues[INDEX_OBJET(objet)] = NULL ; /* BEURK */
  gm->emplacements_vides[gm->nb_emplacements_vides++] = INDEX_OBJET(objet) ;

  free((char*)objet - sizeof(double)) ;
}

int nb_zones_memoires(Gestion_memoire *gm)
{
  int i, nb ;

  nb = 0 ;
  for(i=0; i<gm->nb_objets_alloues; i++)
    if ( gm->objets_alloues[i] )
      nb++ ;
  return(nb) ;
}




