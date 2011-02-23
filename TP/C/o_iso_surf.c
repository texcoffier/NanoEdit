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
 * Vous devez modifier la fonction "affiche_iso_surface" de ce fichier
 */
#include <math.h>		/* Pour sin et cos */
#include <GL/gl.h>		/* Pour OpenGL */
#include "o_objet.h"
#include "t_geometrie.h"	/* Pour Triplet */
#include "u_graphique.h"	/* Pour t_chaine */
#include "u_etat_courant.h"	/* Pour savoir si affichage filaire/facette */

struct iso_surface
{
  Flottant umin, umax ;		/* indique les bornes de calcul */
  Flottant vmin, vmax ;		/* indique les bornes de calcul */
  int      nb_isos ;		/* Nombre d'iso-parametriques */
  Booleen  facette_triangulaire ; /* Vrai: Triangle Faux: Quadrangle */
} ;

static Triplet fonction(Flottant u, Flottant v)
{
  Triplet p ;

  p.x = u ;
  p.y = v ;
  p.z = cos(u) * cos(v) ;
  return(p) ;
}

static void affiche_iso_surface(struct iso_surface *o)
{
  Flottant u, v ;
  Triplet p ;

  /*
   * Ces quelques lignes ci-dessous affichent le point en
   *      u=(umin+umax)/2 v=(vmin+vmax)/2
   */
  u = (o->umin + o->umax)/2 ;
  v = (o->vmin + o->vmax)/2 ;
  p = fonction(u, v) ;
  glBegin(GL_POINTS) ;
  glVertex3f(p.x, p.y, p.z) ;
  glEnd() ;
  /*
   * L'affichage général
   */
  switch(globale_etat_courant.type_affichage)
    {
    case Ponctuel:
      glRasterPos3f(0.,0.,0.) ;
      t_chaine( "Il faut ecrire l'affichage ponctuel !" ) ;
      break ;
    case Filaire:
      glRasterPos3f(0.,0.,0.) ;
      t_chaine( "Il faut ecrire l'affichage filaire !" ) ;
      break ;
    case Facette:
      if ( o->facette_triangulaire )
	{
	  glRasterPos3f(0.,0.,0.) ;
	  t_chaine( "Il faut ecrire l'affichage facette triangulaire!" ) ;
	}
      else
	{
	  glRasterPos3f(0.,0.,0.) ;
	  t_chaine( "Il faut ecrire l'affichage facette quadrangulaire !" ) ;
	}
      break ;
    case Invisible:
      break ;
    }
  /*
   * Affichage des normales à la surface aux sommets des facettes
   */
  if ( ACTIF(o) )
    {
      glRasterPos3f(0.,0.,0.) ;
      t_chaine( "Il faut ecrire l'affichage des normales !" ) ;
    }
  /*
   * Le printf suivant n'est fait que si vous avez appuyé
   * sur le bouton "T" (pour trace) dans la fenetre d'édition
   * de l'objet.
   */
  if ( TRACE(o) )
    eprintf("Affichage surface terminé\n") ;
}

CLASSE(iso_surface, struct iso_surface,

       BULLE_D_AIDE("Cet objet permet d'afficher une surface\n"
		    "à l'aide d'iso-paramétriques.\n")

       MENU("TP Infographie/Iso-surface") /* Chemin dans le menu de création */
       EVENEMENT("Ctrl+IS")	/* Raccourci clavier */

       /*
	* Les indications suivantes sont valables pour tous les
	* champs suivants car elles sont définies à l'extérieur de toute
	* définition de champ.
	*/
       Edite			/* La valeur est éditable par l'utilisateur */
       Sauve			/* La valeur est sauvée dans les fichiers */
       Extrait			/* La valeur peut-être paramétrée (fils) */
       Init3d			/* Les valeurs sont modifiables à la souris */
       L_nombre			/* Les paramètres sont des nombres */
       P_flottant		/* Ils sont stockés dans des Flottant */

       CHAMP(umin, LABEL("U Min") DEFAUT("-7")
	     BULLE_D_AIDE("Valeur minimale pour le paramètre u"))
       CHAMP(umax, LABEL("U Max") DEFAUT("6")
	     BULLE_D_AIDE("Valeur maximale pour le paramètre u"))
       CHAMP(vmin, LABEL("V Min") DEFAUT("-7")
	     BULLE_D_AIDE("Valeur minimale pour le paramètre v"))
       CHAMP(vmax, LABEL("V Max") DEFAUT("6")
	     BULLE_D_AIDE("Valeur maximale pour le paramètre v"))

       CHAMP(nb_isos, L_entier LABEL("Nombre de courbes") DEFAUT("10"))
       CHAMP(facette_triangulaire, L_booleen DEFAUT("0"))

       CHAMP_VIRTUEL(L_affiche_gl(affiche_iso_surface)
		     AFFICHAGE(Different_suivant_type_affichage)
		     )
       )
