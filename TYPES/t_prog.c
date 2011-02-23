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
#include "o_objet.h"

static Booleen sous_classe_prog(const void **o,
				const Champ **c,
				const Type_logique *t)
{
  const void *f ;
  const Champ *cc ;

  f = T(void*, *o, *c) ;

  if ( f && OBJET_OU_NULL(f) )
    {
      if ( c_trouve_type(&f,&cc,t) )
	{
	  if ( TRACE2(*o) )
	    eprintf("Trouve dans les fils\n") ;
	  *o = f ;
	  *c = cc ;
	  return(Vrai) ;
	}
    }
  /*
  else
    NOTE_CHAMP_INVALIDE_(*o, *c) ;
  */

  if ( TRACE2(*o) )
    eprintf("NE Trouve PAS dans les fils\n") ;
  return(Faux) ;
}

DEFINITION_TYPE_LOGIQUE(L_prog, L_metamorphe,
			"Type variable",
			SOUS_CLASSE(sous_classe_prog)
			)


