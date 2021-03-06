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
#include "pov.h"

/*
 *
 */

void pigment(FILE *f, float r, float v, float b)
{
  fprintf(f
	  , "pigment {red %g green %g blue %g}\n"
	  , r<0 ? 0 : ( r>1 ? 1 : r )
	  , v<0 ? 0 : ( v>1 ? 1 : v )
	  , b<0 ? 0 : ( b>1 ? 1 : b )
	  ) ;
}

