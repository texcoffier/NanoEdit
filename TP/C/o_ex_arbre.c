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
 * Vous devez faire la fonction "affiche_arbre_recursif".
 * Pour savoir comment accéder aux champs de "atd" regardez comment
 * la fonction "affiche_arbre" les affiche.
 */

#include <GL/gl.h>
#include <math.h>		/* Pour M_PI */
#include "o_objet.h"
#include "t_geometrie.h"
#include "u_vue.h"		/* Pour "u_affiche_objet" */

struct arbre
{
  Table_triplet atd ;		/* Triplet = (Angle, Torsion, Diminution) */
  int profondeur ;		/* Profondeur de récursion */
  unsigned int rand_seed ;	/* Graine de la série aléatoire */
} ;

static void affiche_tronc(const struct arbre *o, const char *v)
{
  if ( FILS(o,1) )
    u_affiche_objet(FILS(o,1), v) ;
  else
    {
      glBegin(GL_LINES) ;
      glVertex3f(0,0,0) ;
      glVertex3f(0,0,1) ;
      glEnd() ;
    }
}

static void affiche_feuille(const struct arbre *o, const char *v)
{
  if ( FILS(o,2) )
    u_affiche_objet(FILS(o,2), v) ;
  else
    {
      glBegin(GL_LINE_LOOP) ;
      glVertex3f(-.5,0,0) ;
      glVertex3f(.5,0,0) ;
      glVertex3f(0,0,1) ;
      glEnd() ;
    }
}

/*
 * Affichage d'un arbre de profondeur N.
 * Utilisez les fonctions "affiche_tronc" ou "affiche_feuille".
 *
 * Si la récursion n'est pas terminée
 * afficher "o->atd.nb" arbres de profondeur N-1
 * "atd" est une table de triplets : (Angle, Torsion, Diminution)
 */

static void affiche_arbre_recursif(int profondeur
				   , const struct arbre *o
				   , const char *v
				   )
{
}

/*
 * Affiche l'arbre (normalement rien à toucher dans cette fonction)
 */

static void affiche_arbre(struct arbre *o, const Champ *c, const char *v)
{
  int i ;

  if ( TRACE(o) )
    {
      eprintf("nb branches = %d\n", o->atd.nb) ;
      for(i=0;i<o->atd.nb;i++)
	eprintf("\tangle=%-8g torsion=%-8g diminution=%-8g\n",
		o->atd.table[i].x, o->atd.table[i].y, o->atd.table[i].z) ;
    }
  srand(o->rand_seed) ;
  affiche_arbre_recursif(o->profondeur, o, v) ;
}



CLASSE(tp_arbre, struct arbre,
       
       Edite Sauve		/* Tous les champs sont éditable/sauvable */

       MENU("TP Infographie/Arbre")

       BULLE_D_AIDE("Objet affichant un arbre\n")

       CHAMP(atd, L_table_triplet Extrait
	     DEFAUT("-.4 0 .8\n"
		    " .4 0 .7\n"
		    )
	     TE_HAUTEUR(100) /* taille du widget */
	     LABEL("Angle/Torsion/Diminution")
	     BULLE_D_AIDE("Il y a autant de triplet que de branche\n"
			  "Chaque triplet contient :\n"
			  "  - L'angle par rapport à l'axe des X\n"
			  "  - L'angle de torsion (axe Z)\n"
			  "  - Le facteur de réduction\n"
			  )
	     )
       CHAMP(profondeur, L_entier
	     BULLE_D_AIDE("Profondeur de la récursion\n")
	     )
       CHAMP(rand_seed, L_entier
	     BULLE_D_AIDE("Graine de la série aléatoire.\n"
			  "À chaque entier correspond un arbre différent\n"
			  "si le programme a été terminé.\n"
			  )
	     )

       CHAMP_VIRTUEL(L_solide Extrait NonAffiche NonSauve) /* Tronc */
       CHAMP_VIRTUEL(L_solide Extrait NonAffiche NonSauve) /* Feuille */

       CHAMP_VIRTUEL(L_affiche_gl(affiche_arbre)
		     AFFICHAGE(Toujours_different)
		     )
       )
