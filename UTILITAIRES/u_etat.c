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
#include <string.h>
#include "u_base.h"
#include "u_etat.h"

/*
 * Variable locale pour memorise les etats courants
 */

static int o_applique_nb = 0 ;
static int o_applique_taille = 0 ;

static struct
{
  void *position ;
  int taille ;
} *o_applique_variables = NULL ;

static void dump_etat()
{
  int i ;
  eprintf("-------------------- ETAT\n") ;
  for(i=0;i<o_applique_nb;i++)
    {
      eprintf("%p : %x\n"
	      , o_applique_variables[i].position
	      , o_applique_variables[i].taille
	      ) ;
    }
}

static void fusionne(int i)
{
  if ( i >= o_applique_nb-1 || i<0 )
    return ;

  if ( (char*)o_applique_variables[i].position
       + o_applique_variables[i].taille
       == (char*)o_applique_variables[i+1].position
       )
    {
      o_applique_variables[i].taille += o_applique_variables[i+1].taille ;

      memmove(&o_applique_variables[i+1], &o_applique_variables[i+2],
	  sizeof(o_applique_variables[0])*(o_applique_nb-i-2)) ;
      o_applique_nb-- ;
    }
}

void o_applique_a_empiler(void *position, int taille)
{
  int i ;

  for(i=0;i<o_applique_nb;i++)
    if ( o_applique_variables[i].position > position )
      break ;

  if ( i != 0 )
    if ( (char*)position < (char*)o_applique_variables[i-1].position + o_applique_variables[i-1].taille )
      {
	ICI ;
	dump_etat() ;
	return ;
      }
  if ( i != o_applique_nb )
    if ( (char*)position+taille > (char*)o_applique_variables[i].position )
      {
	ICI ;
	dump_etat() ;
	return ;
      }


  o_applique_nb++ ;
  REALLOUER(o_applique_variables, o_applique_nb) ;
  if ( i != o_applique_nb-1 )
    {
      memmove(&o_applique_variables[i+1], &o_applique_variables[i],
	      sizeof(o_applique_variables[0])*(o_applique_nb-i-1)) ;
    }
  o_applique_variables[i].position = position ;
  o_applique_variables[i].taille   = taille ;
  o_applique_taille += taille ;

  /*
   * Ne pas changer l'ordre
   */
  fusionne(i) ;
  fusionne(i-1) ;
}


char *u_sauve_etat()
{
  int i ;
  char *pile, *pointeur ;

  ALLOUER(pile, o_applique_taille) ;
  pointeur = pile ;
  
  for(i=0;i<o_applique_nb;i++)
    {
      memcpy( pointeur,
	      o_applique_variables[i].position,
	      o_applique_variables[i].taille) ;
      pointeur += o_applique_variables[i].taille ;
    }
  return(pile) ;
}

void u_restaure_etat(char *p)
{
  char *pointeur ;
  int i ;

  pointeur = p ;
  for(i=0;i<o_applique_nb;i++)
    {
      memcpy( o_applique_variables[i].position,
	      pointeur,
	      o_applique_variables[i].taille) ;
      pointeur += o_applique_variables[i].taille ;
    }
  free(p) ;
}
