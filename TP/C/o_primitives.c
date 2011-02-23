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
 * Vous devez faire les fonctions d'affichage.
 */

#include <GL/gl.h>
#include <math.h>
#include "o_objet.h"
#include "u_etat_courant.h"

struct vide
{
  int inutile ;			/* Pour éviter un Warning du compilateur */
} ;

/*
 ******************************************************************************
 * Affichage du carré [0,0]x[1,1] avec Z=0
 ******************************************************************************
 */

static void affiche_carre(struct vide *o)
{
  switch(globale_etat_courant.type_affichage)
    {
    case Ponctuel:
      break ;
    case Facette:
      break ;
    case Filaire:
      break ;
    case Invisible:
      break ;
    }
}

CLASSE(tp_carre, struct vide,
       MENU("TP Infographie/Carre") /* Chemin dans le menu de création */
       BULLE_D_AIDE("Carre [0,0]x[1,1] en Z=0\n")
       CHAMP_VIRTUEL(L_affiche_gl(affiche_carre)
		     AFFICHAGE(Different_suivant_type_affichage))
       )
/*
 ******************************************************************************
 * Affichage du cerle de centre (0,0,0) de rayon 1.
 * On prendra 100 segment pour faire le tour du cercle
 ******************************************************************************
 */

static void affiche_cercle(struct vide *o)
{
  switch(globale_etat_courant.type_affichage)
    {
    case Ponctuel:
      break ;
    case Facette:
      break ;
    case Filaire:
      break ;
    case Invisible:
      break ;
    }
}

CLASSE(tp_cercle, struct vide,
       MENU("TP Infographie/Cercle") /* Chemin dans le menu de création */
       BULLE_D_AIDE("Cercle centre [0,0] rayon 1 en Z=0\n")
       CHAMP_VIRTUEL(L_affiche_gl(affiche_cercle)
		     AFFICHAGE(Different_suivant_type_affichage))
       )
/*
 ******************************************************************************
 * Affichage du cube [0,0,0]x[1,1,1]
 ******************************************************************************
 */

static void affiche_cube(struct vide *o)
{
  switch(globale_etat_courant.type_affichage)
    {
    case Ponctuel:
      break ;
    case Facette:
      break ;
    case Filaire:
      break ;
    case Invisible:
      break ;
    }
}

CLASSE(tp_cube, struct vide,
       MENU("TP Infographie/Cube") /* Chemin dans le menu de création */
       BULLE_D_AIDE("Cube [0,0,0]x[1,1,1]\n")
       CHAMP_VIRTUEL(L_affiche_gl(affiche_cube)
		     AFFICHAGE(Different_suivant_type_affichage))
       )
/*
 ******************************************************************************
 * Affichage du Cylindre d'équation x^2+y^2<1 0<z<1
 * C'est un cylindre de rayon 1, de hauteur 1 posé sur le sol.
 ******************************************************************************
 */

static void affiche_cylindre(struct vide *o)
{
  switch(globale_etat_courant.type_affichage)
    {
    case Ponctuel:
      break ;
    case Facette:
      break ;
    case Filaire:
      break ;
    case Invisible:
      break ;
    }
}

CLASSE(tp_cylindre, struct vide,
       MENU("TP Infographie/Cylindre") /* Chemin dans le menu de création */
       BULLE_D_AIDE("Cylindre x^2+y^2<1 0<z<1\n")
       CHAMP_VIRTUEL(L_affiche_gl(affiche_cylindre)
		     AFFICHAGE(Different_suivant_type_affichage))
       )
/*
 ******************************************************************************
 * Affichage de la sphère d'équation x^2+y^2+z^2 < 1
 * C'est une sphere de centre origine et de rayon 1
 ******************************************************************************
 */

static void affiche_sphere(struct vide *o)
{
  switch(globale_etat_courant.type_affichage)
    {
    case Ponctuel:
      break ;
    case Facette:
      break ;
    case Filaire:
      break ;
    case Invisible:
      break ;
    }
}

CLASSE(tp_sphere, struct vide,
       MENU("TP Infographie/Sphere") /* Chemin dans le menu de création */
       BULLE_D_AIDE("Sphère de centre origine et de rayon 1\n")
       CHAMP_VIRTUEL(L_affiche_gl(affiche_sphere)
		     AFFICHAGE(Different_suivant_type_affichage))
       )
