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
 * Créé le Wed Feb  4 12:02:00 2004 par Thierry EXCOFFIER
 */

#ifndef _HOME_EXCO_NE_1_14_OBJETS_U_REPERE_H
#define _HOME_EXCO_NE_1_14_OBJETS_U_REPERE_H

#define OPTION_AFFICHE_X                (((long long)1)<<0)
#define OPTION_AFFICHE_Y                (((long long)1)<<1)
#define OPTION_AFFICHE_Z                (((long long)1)<<2)
#define OPTION_AFFICHE_TAG_POINT_X      (((long long)1)<<3)
#define OPTION_AFFICHE_TAG_POINT_Y      (((long long)1)<<4)
#define OPTION_AFFICHE_TAG_POINT_Z      (((long long)1)<<5)
#define OPTION_AFFICHE_TAG_DILATATION_X (((long long)1)<<6)
#define OPTION_AFFICHE_TAG_DILATATION_Y (((long long)1)<<7)
#define OPTION_AFFICHE_TAG_DILATATION_Z (((long long)1)<<8)
#define OPTION_AFFICHE_TAG_ROTATION_X   (((long long)1)<<9)
#define OPTION_AFFICHE_TAG_ROTATION_Y   (((long long)1)<<10)
#define OPTION_AFFICHE_TAG_ROTATION_Z   (((long long)1)<<11)
#define OPTION_AFFICHE_FLECHE_X   	(((long long)1)<<12)
#define OPTION_AFFICHE_FLECHE_Y   	(((long long)1)<<13)
#define OPTION_AFFICHE_FLECHE_Z   	(((long long)1)<<14)
#define OPTION_AFFICHE_TAG_INVERSION_X  (((long long)1)<<15)
#define OPTION_AFFICHE_TAG_INVERSION_Y  (((long long)1)<<16)
#define OPTION_AFFICHE_TAG_INVERSION_Z  (((long long)1)<<17)
#define OPTION_AFFICHE_TAG_SYMETRIE_X   (((long long)1)<<18)
#define OPTION_AFFICHE_TAG_SYMETRIE_Y   (((long long)1)<<19)
#define OPTION_AFFICHE_TAG_SYMETRIE_Z   (((long long)1)<<20)
#define OPTION_AFFICHE_TAG_ROTATION2_X  (((long long)1)<<21)
#define OPTION_AFFICHE_TAG_ROTATION2_Y  (((long long)1)<<22)
#define OPTION_AFFICHE_TAG_ROTATION2_Z  (((long long)1)<<23)
#define OPTION_AFFICHE_TAG_ECHANGE_X    (((long long)1)<<24)
#define OPTION_AFFICHE_TAG_ECHANGE_Y    (((long long)1)<<25)
#define OPTION_AFFICHE_TAG_ECHANGE_Z    (((long long)1)<<26)
#define OPTION_AFFICHE_X_NEGATIF        (((long long)1)<<27)
#define OPTION_AFFICHE_Y_NEGATIF        (((long long)1)<<28)
#define OPTION_AFFICHE_Z_NEGATIF        (((long long)1)<<29)
#define OPTION_AFFICHE_TAG_POINT   	(((long long)1)<<30)
#define OPTION_AFFICHE_TAG_ORIGINE   	(((long long)1)<<31)
#define OPTION_AFFICHE_TAG_VALEUR  	(((long long)1)<<32)
#define OPTION_AFFICHE_TAG_VECTEUR  	(((long long)1)<<32)
#define OPTION_AFFICHE_SEGMENT  	(((long long)1)<<34)


#define OPTION_AFFICHE_XYZ (OPTION_AFFICHE_X|OPTION_AFFICHE_Y|OPTION_AFFICHE_Z)
#define OPTION_AFFICHE_TAG_POINT_XYZ (OPTION_AFFICHE_TAG_POINT_X|OPTION_AFFICHE_TAG_POINT_Y|OPTION_AFFICHE_TAG_POINT_Z)
#define OPTION_AFFICHE_TAG_DILATATION_XYZ (OPTION_AFFICHE_TAG_DILATATION_X|OPTION_AFFICHE_TAG_DILATATION_Y|OPTION_AFFICHE_TAG_DILATATION_Z)
#define OPTION_AFFICHE_TAG_ROTATION_XYZ (OPTION_AFFICHE_TAG_ROTATION_X|OPTION_AFFICHE_TAG_ROTATION_Y|OPTION_AFFICHE_TAG_ROTATION_Z)
#define OPTION_AFFICHE_TAG_ROTATION2_XYZ (OPTION_AFFICHE_TAG_ROTATION2_X|OPTION_AFFICHE_TAG_ROTATION2_Y|OPTION_AFFICHE_TAG_ROTATION2_Z)
#define OPTION_AFFICHE_TAG_ECHANGE_XYZ (OPTION_AFFICHE_TAG_ECHANGE_X|OPTION_AFFICHE_TAG_ECHANGE_Y|OPTION_AFFICHE_TAG_ECHANGE_Z)
#define OPTION_AFFICHE_FLECHE_XYZ (OPTION_AFFICHE_FLECHE_X|OPTION_AFFICHE_FLECHE_Y|OPTION_AFFICHE_FLECHE_Z)
#define OPTION_AFFICHE_TAG_INVERSION_XYZ (OPTION_AFFICHE_TAG_INVERSION_X|OPTION_AFFICHE_TAG_INVERSION_Y|OPTION_AFFICHE_TAG_INVERSION_Z)
#define OPTION_AFFICHE_TAG_SYMETRIE_XYZ (OPTION_AFFICHE_TAG_SYMETRIE_X|OPTION_AFFICHE_TAG_SYMETRIE_Y|OPTION_AFFICHE_TAG_SYMETRIE_Z)
#define OPTION_AFFICHE_TOUT_Z (OPTION_AFFICHE_Z				\
			       | OPTION_AFFICHE_TAG_DILATATION_Z	\
			       | OPTION_AFFICHE_TAG_ROTATION_Z		\
			       | OPTION_AFFICHE_TAG_ROTATION2_Z		\
			       | OPTION_AFFICHE_TAG_ECHANGE_Z		\
			       | OPTION_AFFICHE_TAG_INVERSION_Z		\
			       | OPTION_AFFICHE_TAG_SYMETRIE_Z		\
			       | OPTION_AFFICHE_TAG_POINT_Z		\
			       )
typedef struct repere
{
  Transfo t ;		/* Le repere defini par origine et 3 vecteur */
  Triplet   px;		/* L_point au bout de l'axe de X */
  Triplet   py;		/* L_point au bout de l'axe de Y */
  Triplet   pz;		/* L_point au bout de l'axe de Z */
  t_angle angle ;	/* De rotation */
  Flottant homo ;	/* Coefficient */
  Triplet vecteur ;	/* Axe de rotation ou vecteur dilatation */
  Booleen fleche ;	/* Affiche des flèches */
  Booleen cache_tag ;   /* Rotation, extension */
  Booleen cache_tag_symetrie ;
  Booleen cache_tag_rotation2 ;
  Booleen cache_tag_echange ;
  Booleen inverse_z ;	/* Pour le repère axe */
  Booleen cache_repere ;/* Pour le repère axe */
  Booleen normalise ;	/* Pour le repère axe */
  char centre_deplacable ; /* 3 bits indiquent si cela déplace le centre */
  long long options ;
} Repere ;


#endif
