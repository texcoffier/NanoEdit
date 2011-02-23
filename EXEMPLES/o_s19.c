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
#include "t_geometrie.h"
#include <GL/gl.h>
struct ligne_brisee { int inutile ; } ;	   
static void affiche_lb(struct ligne_brisee *s)
{
  const char *texte ;
  Triplet point ;
  
  glBegin(GL_LINE_STRIP) ;
  POUR_LES_FILS			/* Boucle sur tous les fils non détruit */
    (s,				/* De "s" */
     fils,			/* Nom de l'indice de boucle */
     {
       /* Récupère un champ de type L_point dans le fils */
       texte = c_recupere_type_sans_copie(fils, &infos_L_point) ;
       /* Teste si la chaine de caractère n'est pas vide */
       if ( texte[0] )
	 {
	   /* Conversion de la chaine en triplet */
	   point = char_vers_triplet(texte) ;
	   /* Affichage */
	   glVertex3f(point.x, point.y, point.z) ;
	 }
     }
     ) ;
  glEnd();
}
CLASSE(segment19,
       struct ligne_brisee,
       CHAMP_VIRTUEL(L_affiche_gl(affiche_lb))
       /* Lors de la création de l'objet, il prend TOUS
	* les actifs comme paramètre. */
       PAQUET(NB_ACTIFS)
      )
