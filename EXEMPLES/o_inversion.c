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
#include <math.h>
#include "t_image.h"                                 /**/
#include "t_geometrie.h"

struct inversion { Image image ; Image *entree ; } ;  

static void changement_image(struct inversion *o)
{
  int taille, i ;
  unsigned char *in, *out ;

  if ( o->entree==NULL || L3_UNSIGNED_CHAR(o->entree)==NULL )
    return ;
  if ( LAR(&o->image) != LAR(o->entree) ||
       HAU(&o->image) != HAU(o->entree) )
    {
      o->image.codec = codec_HL_l3_unsigned_char ;
      LAR(&o->image) = LAR(o->entree) ;
      HAU(&o->image) = HAU(o->entree) ;
      CODEC_REALLOC(L3_UNSIGNED_CHAR(&o->image),LAR(&o->image)*HAU(&o->image)) ;
    }
  out = (unsigned char*) L3_UNSIGNED_CHAR(&o->image) ;
  in = (unsigned char*) L3_UNSIGNED_CHAR(o->entree) ;
  taille = LAR(&o->image) * HAU(&o->image) * 3 ;

  for(i=0;i<taille;i++)
     *out++ = 255 - *in++ ;
}
CLASSE(inversion, struct inversion,
       CHAMP(image, L_grille_point P_image_l3_unsigned_char E_image
	     Affiche TE_HAUTEUR(256))
       CHAMP(entree, L_pointeur_extrait               /**/
	     TYPE_DANS_FILS(L_image_l3_unsigned_char) /**/
	     Extrait)                                 /**/
       CHANGEMENT(changement_image)
      )
