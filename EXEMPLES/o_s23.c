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

struct segment { Image image ; Flottant l,h ;  Triplet depart, arrivee ; } ;  

static void changement_segment(struct segment *o)
{
  int x, y ;
  unsigned char *pc ;

  if ( !UN_CHAMP_CHANGE(o) )
    return ;
  if ( L3_UNSIGNED_CHAR(&o->image) == NULL
       || LAR(&o->image) != o->l
       || HAU(&o->image) != o->h
       ) /* Allocation */
    {
      o->image.codec = codec_HL_l3_unsigned_char ;
      LAR(&o->image) = o->l ; /* Pourrait être une variable éditable */
      HAU(&o->image) = o->h ;
      CODEC_REALLOC(L3_UNSIGNED_CHAR(&o->image),o->l*o->h) ;
    }
  /* Calcul de l'image */
  pc = (unsigned char*) L3_UNSIGNED_CHAR(&o->image) ;
  for(y=0;y<o->h;y++)
    for(x=0;x<o->l;x++)
      {
	*pc++ = x ;
	*pc++ = y ;
	*pc++ = 128 + (sqrt(((x - o->depart.x)  *  (x - o->depart.x)
		 + (y - o->depart.y)  *  (y - o->depart.y))) -
	        sqrt((x - o->arrivee.x)  *  (x - o->arrivee.x)
		 + (y - o->arrivee.y)  *  (y - o->arrivee.y)))*2 ;
      }
}
/* En cliquant dans la fenêtre image, on modifie l'un des points */
static Booleen segment_entree(const Interface_widget *iw)
{
  struct segment *o = iw->objet ;
  if ( iw->b == 0 ) /* 0 représente le bouton de gauche */
    { o->depart.x  = iw->x ; o->depart.y  = o->h - iw->y ; }
  else
    { o->arrivee.x = iw->x ; o->arrivee.y = o->h - iw->y ; }
  return(1) ; /* 0 Si l'on ne veut pas faire de suivie de mouvement */
}
static void segment_entree_mouvement(const Interface_widget *iw)
{
  if ( iw->b >= 0 )		/* Si bouton appuyé */
    segment_entree(iw) ;
}
CLASSE(segment23, struct segment,
       CHAMP(depart , L_point  P_triplet      Edite Sauve Init3d Extrait)
       CHAMP(arrivee, L_point  P_triplet      Edite Sauve Init3d Extrait)
       CHAMP(l      , L_nombre P_flottant     Edite Sauve Init3d DEFAUT("64"))
       CHAMP(h      , L_nombre P_flottant     Edite Sauve Init3d DEFAUT("64"))
       CHAMP(image  , L_grille_point E_image P_image_l3_unsigned_char
	     Affiche TE_HAUTEUR(512) TE_LARGEUR(512)
	     TE_ENTREE(segment_entree) TE_MOUVEMENT(segment_entree_mouvement)
	     )
       CHANGEMENT(changement_segment)
      )
