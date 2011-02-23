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
/*
 * Créé le Wed Sep 16 15:06:36 1998 par Thierry EXCOFFIER
 */

#ifndef T_IMAGE_H
#define T_IMAGE_H

#include "t_gl.h"

typedef struct t_image
{
  /*
   * Si l'image doit être affichée, il faut un éditeur d'image
   * qui a besoin d'un Widget GL.
   * CE CHAMP DOIT ETRE EN PREMIER CAR ON REUTILISE E_GL
   */
  Gl gl ;
  /*
   * Dans le cas ou l'image est lu à partir d'un fichier
   * il faut indiquer le nom du fichier
   * Le type physique est alors P_codec_fichier_image_...
   * La lecture donne le nom du fichier et l'écriture fait relire le fichier.
   */
  char *nom_image ;
  /*
   * L'image elle même, elle peut être de type différents
   * suivant la façon de stocker les pixels
   */
  union
  {
    t_HL_l3_float l3_float ;
    t_HL_l3_unsigned_char l3_unsigned_char ;
    t_HL_unsigned_char unsigned_char ;
    t_HL_l4_unsigned_char l4_unsigned_char ;
  } image ;
  /*
   * Ratio de l'image lors du chargement
   * Cette information est ici car l'objet "image"
   * perd le ratio. Et ne sait pas quand le calculer
   */
  Flottant ratio ;
  /*
   * Le codec du tableau représentant l'image
   */
  Codec codec ;
} Image ;

#define LAR(p) (p)->image.l3_float.lar
#define HAU(p) (p)->image.l3_float.hau
#define         L3_FLOAT(p) (p)->image.l3_float.table
#define L3_UNSIGNED_CHAR(p) (p)->image.l3_unsigned_char.table
#define L4_UNSIGNED_CHAR(p) (p)->image.l4_unsigned_char.table
#define    UNSIGNED_CHAR(p) (p)->image.unsigned_char.table



PROTOTYPE_TYPE_PHYSIQUE(codec_fichier_image_l3_float        , Image)
PROTOTYPE_TYPE_PHYSIQUE(codec_fichier_image_l3_unsigned_char, Image)
PROTOTYPE_TYPE_PHYSIQUE(codec_fichier_image_l4_unsigned_char, Image)
PROTOTYPE_TYPE_PHYSIQUE(codec_fichier_image_unsigned_char , Image)
PROTOTYPE_TYPE_PHYSIQUE(codec_image_l3_unsigned_char        , Image)
PROTOTYPE_TYPE_PHYSIQUE(codec_image_l4_unsigned_char        , Image)
PROTOTYPE_TYPE_PHYSIQUE(codec_image_unsigned_char         , Image)
PROTOTYPE_TYPE_PHYSIQUE(codec_image_l3_float                , Image)

PROTOTYPE_TYPE_LOGIQUE(L_fichier_image                 )
PROTOTYPE_TYPE_LOGIQUE(L_fichier_image_l3_unsigned_char)
PROTOTYPE_TYPE_LOGIQUE(L_fichier_image_l4_unsigned_char)
PROTOTYPE_TYPE_LOGIQUE(L_fichier_image_unsigned_char )
PROTOTYPE_TYPE_LOGIQUE(L_fichier_image_l3_float        )
PROTOTYPE_TYPE_LOGIQUE(L_image                         )
PROTOTYPE_TYPE_LOGIQUE(L_image_l3_unsigned_char        )
PROTOTYPE_TYPE_LOGIQUE(L_image_l4_unsigned_char        )
PROTOTYPE_TYPE_LOGIQUE(L_image_unsigned_char         )
PROTOTYPE_TYPE_LOGIQUE(L_image_l3_float                )

#define P_fichier_image_l3_float         TP(codec_fichier_image_l3_float        )
#define P_fichier_image_l3_unsigned_char TP(codec_fichier_image_l3_unsigned_char)
#define P_fichier_image_l4_unsigned_char TP(codec_fichier_image_l4_unsigned_char)
#define P_fichier_image_unsigned_char  TP(codec_fichier_image_unsigned_char)
#define P_image                          TP(codec_image                         )
#define P_image_l3_float                 TP(codec_image_l3_float                )
#define P_image_l3_unsigned_char         TP(codec_image_l3_unsigned_char        )
#define P_image_l4_unsigned_char         TP(codec_image_l4_unsigned_char        )
#define P_image_unsigned_char          TP(codec_image_unsigned_char        )

#define L_fichier_image                  TL(L_fichier_image                 )
#define L_fichier_image_unsigned_char  TL(L_fichier_image_unsigned_char)
#define L_fichier_image_l3_unsigned_char TL(L_fichier_image_l3_unsigned_char)
#define L_fichier_image_l4_unsigned_char TL(L_fichier_image_l4_unsigned_char)
#define L_fichier_image_l3_float         TL(L_fichier_image_l3_float        )
#define L_image                          TL(L_image                         )
#define L_image_l3_unsigned_char         TL(L_image_l3_unsigned_char        )
#define L_image_l4_unsigned_char         TL(L_image_l4_unsigned_char        )
#define L_image_unsigned_char          TL(L_image_unsigned_char        )
#define L_image_l3_float                 TL(L_image_l3_float                )

PROTOTYPE_TYPE_EDITEUR(E_image)
#define E_image      TE(E_image)




int recharge_image(Image *o) ;

#endif
