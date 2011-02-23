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
#include <GL/gl.h>
#include "o_objet.h"            /* Car il faut pouvoir definir la classe */
#include "t_geometrie.h"
#include "u_graphique.h"

/*
 * A mettre dans un t_toto.h
 */
PROTOTYPE_TYPE_LOGIQUE(L_parallelipipede)
#define L_parallelipipede            TL(L_parallelipipede)

/*
 * A mettre dans un t_toto.c
 */
DEFINITION_TYPE_LOGIQUE(L_parallelipipede, L_valeur, "Parallelipipede", )


/*
 * Objet extrayant un plan d'un parallelipipede 
 */
typedef struct
{
  Transfo parallelipipede ;
  Quadruplet equation ;		/* Équation plan ax+by+cz+d = 0 */
  int face ;
} Plan_de_parallelipipede ;

CLASSE(plan_de_parallelipipede, Plan_de_parallelipipede,
       CHAMP(equation, L_plan P_quadruplet DEFAUT("0 0 1 0") Affiche)
       CHAMP(parallelipipede, L_parallelipipede P_transfo
	     Edite Extrait Sauve)
       CHAMP(face, L_nombre P_entier DEFAUT("0") Edite Sauve)
       MENU("Reconstruction Mono Image/Plan de Parallélipipède")
       EVENEMENT("RMIPP")
       )



/*
 * Opération sur le plan :
 * TRANSFORME_PLAN(PLAN, TRANSFO)
 * Comme cela on peut réutiliser toutes les transfos existantes.
 */




/*
 * On peut tirer sur les Tag de l'objet pour le déplacer
 * Si cet objet est paramétré par des plans/droite/point : contraintes
 */

typedef struct
{
  Transfo t ;		/* repère du cube unitaire */
} Parallelipipede ;

static void affiche_parallelipipede(Parallelipipede *o
				    , const Champ *c
				    , const char*v)
{
  glPushMatrix() ;
  glTranslatef(-3,-2,1) ;
  t_cube() ;
  glPopMatrix() ;

}

CLASSE(parallelipipede, Parallelipipede,
       CHAMP(t,
	     L_parallelipipede P_transfo
	     DEFAUT("[(0 0 0)\n(1 0 0)\n(0 1 0)\n(0 0 1)]\n")
	     Affiche
	     BULLE_D_AIDE("Repère du cube unitaire\n")
	     )
       CHAMP_VIRTUEL(L_affiche_gl(affiche_parallelipipede)
		     AFFICHAGE(Different_si_actif)
		     )
       PAQUET(NB_ACTIFS)
       MENU("Reconstruction Mono Image/Parallélipipède")
       EVENEMENT("RMIPA")
       )



/*
 * Calcul observateur
 */

typedef struct
{
  Parallelipipede parallelipipede ;
  t_observateur observateur ;
  Table_triplet pts ;
} Calcul_observateur ;


static void changement_calcul_observateur(Calcul_observateur *o)
{
  int i ;

  if ( o->pts.nb < 2 )
    return ;
  /*
   * Annule le Z des points saisis.
   * Ils ne sont pas nuls pour des raisons d'arrondis
   */
  for(i=0; i<o->pts.nb; i++)
    o->pts.table[i].z = 0 ;
  /*
   * Calcul observateur
   */
  o->observateur.point_de_vue.x = 0 ;
  o->observateur.point_de_vue.y = 0 ;

  o->observateur.point_de_vue.z = u_distance_triplet(&o->pts.table[0]
						     , &o->pts.table[1]
						     ) + 1.1 ;
  o->observateur.focale = tan(atan(4./o->observateur.point_de_vue.z)*2) ;

  u_annule_triplet(&o->observateur.point_vise) ;
  u_annule_triplet(&o->observateur.haut) ;
  o->observateur.haut.y = 1 ;
}



CLASSE(calcul_observateur, Calcul_observateur,
       SUR_CLASSE("parallelipipede")
       CHAMP(observateur, L_observateur Affiche
	     BULLE_D_AIDE("Observateur calculé\n")
	     )

       CHAMP(pts, L_table_point3 Edite Extrait Sauve Obligatoire)

       CHANGEMENT(changement_calcul_observateur)
       MENU("Reconstruction Mono Image/Calcul Observateur")
       EVENEMENT("RMICO")
       )

