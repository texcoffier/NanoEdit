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

struct disposition
{
  Booleen a, b, c, d, e, f, g, h, i, j, k, l ;
} ;

CLASSE(disposition, struct disposition,
       Edite L_booleen
       DEBUT_BOITE(HORIZONTALE)
       		CHAMP(a,)
       		CHAMP(b,)
       		CHAMP(c,)
       FIN_BOITE
       DEBUT_BOITE(HORIZONTALE)
       		CHAMP(d,)
       		DEBUT_BOITE(VERTICALE)
                      DEBUT_BOITE(HORIZONTALE)
                            CHAMP(e,)
       		            CHAMP(f,)
                      FIN_BOITE
       		      CHAMP(g,)
       		      CHAMP(h,)
                FIN_BOITE
       		CHAMP(i,)
       FIN_BOITE
       DEBUT_BOITE(HORIZONTALE BoiteHomogene)
       CHAMP(j,)
       CHAMP(k,)
       CHAMP(l,)
       FIN_BOITE
)

