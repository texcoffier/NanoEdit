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
#include "s_string.h"
#include <GL/gl.h>

/*
 * set parametric
 * 
 */
/*
 * Un sommet sur la surface extrudée par translation
 * est défini par les paramètres (u,v)
 * u : varie entre 0 et contour.nb-1, c'est la position sur le contour.
 * v : varie entre 0 et 1 avec un pas de 1/nb_coupes
 *
 * Les coordonnées du point sont :
 *            contour[u] * ((1-v)*taille_debut + v*taille_fin) + v * vecteur
 *
 * C'est une extrusion par translation avec homothétie du contour.
 */

struct translation
{
  Table_triplet contour ;	/* Table des points en entrée */
  Triplet       vecteur ;	/* Vecteur de déplacement du contour */
  int           nb_coupes ;	/* Nombre de coupes selon le déplacement */
  Flottant      taille_debut ;	/* Homothétie initiale du contour */
  Flottant      taille_fin ;	/* Homothétie finale du contour */
} ;

/*
 * Affichage.
 * Pour le moment cette fonction affiche le contour et le vecteur.
 */

static void affiche_translation(struct translation *o)
{
  int i ;
  /*
   * Affiche le contour initial sans aucune transformation
   */
  glBegin(GL_LINE_LOOP) ;
  for( i = 0             ; /* Premier point du contour */
       i < o->contour.nb ; /* Pour tous les points du contour */
       i++
       )
    glVertex3f(o->contour.table[i].x,
	       o->contour.table[i].y,
	       o->contour.table[i].z
	       ) ;
  glEnd() ;
  /*
   * Affiche le vecteur de translation
   */
  glBegin(GL_LINES) ;
  glVertex3f(0.,0.,0.) ;
  glVertex3f(o->vecteur.x, o->vecteur.y, o->vecteur.z) ;
  glEnd() ;
}
/*
 * Normalement, vous n'avez pas besoin de toucher la fonction suivante.
 * Elle affiche dans la fenêtre graphe le petit texte à droite
 * du nom "extrusion_translation"
 */
static char *information_translation(struct translation *o)
{
  static Chaine s ;

  s_efface(&s) ;
  s_printf(&s,
	   "taille %g->%g",
	   o->taille_debut,
	   o->taille_fin
	   ) ;
  return( s_char(&s) ) ;
}
/*
 * Définition de la Classe
 */
CLASSE(tp_extrusion_translation, struct translation,
       Edite Extrait Sauve
       CHAMP(contour, L_table_point3
	     DEFAUT("1 1 0\n1 -1 0\n-1 -1 0\n-1 1 0")
	     BULLE_D_AIDE("Contour à déplacer")
	     )
       CHAMP(vecteur     , L_vecteur3
	     DEFAUT("0 0 2.5")
	     BULLE_D_AIDE("Direction dans laquelle le vecteur est translaté")
	     )
       CHAMP(taille_debut, L_flottant
	     DEFAUT("1")
	     BULLE_D_AIDE("Homothétie du contour à sa position initiale")
	     )
       CHAMP(taille_fin  , L_flottant
	     DEFAUT("1")
	     BULLE_D_AIDE("Homothétie du contour à sa position finale")
	     )
       CHAMP(nb_coupes   , L_entier
	     DEFAUT("10")
	     BULLE_D_AIDE("Nombre d'iso-paramétriques dans le sens\n"
			  "de la translation")
	     )

       MENU("TP Infographie/Extrusion Translation")
       CHAMP_VIRTUEL(L_affiche_gl(affiche_translation))
       INFORMATIONS(information_translation)
       )

