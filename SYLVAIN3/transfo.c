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
#define LISTEDE /* Pour déclarer en global */
#include "transfo.h"
#include "u_graphique.h"


void transfo_affiche_pov(Transformation *t, FILE *f)
{
  Transfo tr ;

  (*transfo_extrait)(t, &tr, 0) ;

  fprintf( f,
	   "matrix <%g, %g, %g,  %g, %g, %g,  %g, %g, %g,  %g, %g, %g>\n",
	   tr.v1.x     , tr.v1.y     , tr.v1.z,
	   tr.v2.x     , tr.v2.y     , tr.v2.z,
	   tr.v3.x     , tr.v3.y     , tr.v3.z,
	   tr.origine.x, tr.origine.y, tr.origine.z
	   ) ;

}

void transfo_affiche(Transformation *t, FILE *f)
{
  Transfo tr ;
  int i ;

  for(i=0; (*transfo_extrait)(t, &tr, i); i++)
    {
      fprintf( f,
	       "matrix <%g, %g, %g,  %g, %g, %g,  %g, %g, %g,  %g, %g, %g>\n",
	       tr.v1.x     , tr.v1.y     , tr.v1.z,
	       tr.v2.x     , tr.v2.y     , tr.v2.z,
	       tr.v3.x     , tr.v3.y     , tr.v3.z,
	       tr.origine.x, tr.origine.y, tr.origine.z
	       ) ;
    }
}

void transfo_extrait_gl(Transformation *t, double tab[16])
{
  Transfo tr ;

  (*transfo_extrait)(t, &tr, 0) ;
  u_transfo_vers_gl(&tr, tab) ;
}
