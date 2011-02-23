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
#include "u_graphique.h"
#include "s_string.h"
#include "t_geometrie.h"
#include <GL/gl.h>

struct piece ;
/*
 *
 */
struct reserve
{
  int 	  type_de_piece ;
  int 	  nombre_de_pieces_maximum ;
  int 	  nombre_de_pieces ;
  Booleen pleine ;		/* Champ calculé automatiquement */
  Triplet position ;		/* Position de la réserve dans l'espace */
} ;
/*
 * Affichage de la réserve de pièces.
 */
static void affiche_reserve(struct reserve *o)
{
  struct piece *p ;
  int i, j ;
  /*
   * Teste si l'objet réserve à bien un fils (paramètre)
   * pour lui indiquer le type de pièce qu'il stocke.
   */
  p = FILS(o, 0) ;
  if ( p == NULL )
    {
      glRasterPos3f(o->position.x, o->position.y, o->position.z) ;
      t_chaine( "C'est une réserve de quoi ?" ) ;
      return ;
    }
  /*
   * Affichage des pièce.
   */
  glPushMatrix() ;
  glTranslatef(o->position.x, o->position.y, o->position.z) ;
  /*
   * Affiche le nombre de pieces en réserve
   */
  glRasterPos3f(0.5, 0.5, 0.5) ;
  t_chaine( (char*)int_vers_char(o->nombre_de_pieces) ) ;  
  t_cube() ;
  /*
   * Comme exemple, je fais l'affichage d'une grille de pièces
   * au dixieme de sa taille normale.
   */
  glScalef(.1,.1,.1) ;
  for(i=0;i<5;i++)
    for(j=0;j<5;j++)
      {
	glPushMatrix() ;
	glTranslatef(i*2+.5, j*2+.5, .1) ;
	c_sauve_type(p, &infos_L_affiche_gl, "") ;
	glPopMatrix() ;
      }
  glPopMatrix() ;
}
/*
 * Normalement, vous n'avez pas besoin de toucher la fonction suivante.
 * Elle sert à mettre à jour la structure en cas de changement.
 */
static void changement_reserve(struct reserve *o)
{
  if ( o->nombre_de_pieces < 0 )
     o->nombre_de_pieces = 0 ;
  if ( o->nombre_de_pieces > o->nombre_de_pieces_maximum )
     o->nombre_de_pieces = o->nombre_de_pieces_maximum ;
  o->pleine = ( o->nombre_de_pieces >= o->nombre_de_pieces_maximum ) ;
}
/*
 * Normalement, vous n'avez pas besoin de toucher la fonction suivante.
 * Elle demande à la réserve d'ajouter ou d'enlever une pièce.
 */
static void modification(struct reserve *o, const Champ *ch, const char *v)
{
  if ( *v == '+' && !o->pleine )
    o->nombre_de_pieces++ ;
  if ( *v == '-' && o->nombre_de_pieces )
    o->nombre_de_pieces-- ;
  /*
  NOTE_CHAMP_CHANGE(o, nombre_de_pieces) ;
  */
  changement_reserve(o) ;      
}
/*
 * Normalement, vous n'avez pas besoin de toucher la fonction suivante.
 * Elle affiche dans la fenêtre graphe le petit texte à droite
 * du nom "reserve"
 */
static char *information_reserve(struct reserve *o)
{
  static Chaine s ;

  s_efface(&s) ;
  s_printf(&s,
	   "%d pièces, maximum %d",
	   o->nombre_de_pieces,
	   o->nombre_de_pieces_maximum
	   ) ;
  return( s_char(&s) ) ;
}
/*
 * Définition de la Classe
 */
CLASSE(gpao_reserve, struct reserve,
       Edite Extrait Sauve L_entier
       CHAMP(type_de_piece           , Obligatoire DEFAUT("0"))
       CHAMP(nombre_de_pieces        , DEFAUT("2"))
       CHAMP(nombre_de_pieces_maximum, DEFAUT("4"))
       CHAMP(position                , L_point3 Edite Sauve Extrait Init3d)
       CHAMP(pleine                  , NonEdite NonExtrait NonSauve L_booleen)

       MENU("TP Infographie/GPAO Réserve")
       CHANGEMENT(changement_reserve)
       INFORMATIONS(information_reserve)
       CHAMP_VIRTUEL(L_affiche_gl(affiche_reserve))
       CHAMP_VIRTUEL(NOM("modification") L_fonction(modification))
       )

