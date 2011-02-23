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
#include "u_graphique.h"
#include <string.h>
#include <GL/gl.h>

struct reserve ;
/*
 *
 */
struct transporteur
{
  Triplet 	  depart, arrivee ;
  int     	  nb_pieces_depart ;
  struct reserve *reserve_depart ;
  struct reserve *reserve_arrivee ;
  int longueur ;		/* En nombre de pièces transportables */
  int tempo ;			/* Nombre de secondes pour avancer d'1 pièce */
  /*
   * La chaine de caractère à une longueur qui est celle du transporteur.
   * Un ' ' indique qu'il n'y a rien
   * Une '*' indique qu'il y a une pièce
   * Les pièces entrent à gauche et sortent à droite.
   */
  char *contenu ;
  /*
   * Pour gérer la simulation
   */
  int seconde, seconde2 ;
} ;
/*
 * Affichage.
 * N'oubliez pas d'utiliser les points de départ et d'arrivée.
 */
static void affiche_transporteur(struct transporteur *o)
{
  Triplet milieu ;

  glBegin(GL_LINES) ;
  glVertex3f(o->depart.x, o->depart.y, o->depart.z) ;
  glVertex3f(o->arrivee.x, o->arrivee.y, o->arrivee.z) ;
  glEnd() ;
  /*
   * Positionne le repère courant au milieu du transporteur
   */
  glPushMatrix() ;
  milieu = u_ajoute_triplet(&o->depart, &o->arrivee) ;
  milieu = u_triplet_fois_flottant(&milieu, 0.5) ;
  glTranslatef(milieu.x, milieu.y, milieu.z) ;
  /*
   * Affiche la chaine de caractères représentant le transporteur
   */
  glRasterPos3f(0.,0.,0.) ;
  t_chaine(o->contenu) ;
  /*
   * Affiche une pièce de la première réserve au milieu
   * du transporteur (en petit).
   */
  glScalef(.1,.1,.1) ;
  c_sauve_type(FILS(o->reserve_depart,0), &infos_L_affiche_gl, "") ;
  /*
   * Remet le repère courant
   */
  glPopMatrix() ;
}
/*
 * Ne pas lire la suite.
 * Elle concerne la simulation et non le graphique.
 */
static void changement_transporteur(struct transporteur *o)
{
  /*
   * Initialisation du contenu en fonction de la longueur
   */
  if ( CHAMP_CHANGE(o, longueur) )
    if ( strlen(o->contenu) != o->longueur )
      {
	REALLOUER(o->contenu, o->longueur+1) ;
	sprintf(o->contenu, "%*s", o->longueur, "") ;
      }
  /*
   * On ne fait rien si l'on ne peut pas décharger la pièce
   */
  if ( c_recupere_sans_copie(o->reserve_arrivee, "pleine")[0] != '0' )
    return ;
  /*
   * Cette fonction doit s'exécuter qu'une fois par "tempo"
   */
  if ( o->seconde < o->seconde2 + o->tempo )
    return ;
  o->seconde2 = o->seconde ;
  /*
   * Décharge la pièce
   */
  if ( o->contenu[strlen(o->contenu)-1] == '*' )
    {
      c_sauve(o->reserve_arrivee, "modification", "+") ;
    }
  memmove(o->contenu+1, o->contenu, o->longueur-1) ;
  /*
   * Prend une pièce si possible
   */
  if ( c_recupere_sans_copie(o->reserve_depart, "nombre_de_pieces")[0] != '0' )
    {
      c_sauve(o->reserve_depart, "modification", "-") ;
      o->contenu[0] = '*' ;
    }
  else
      o->contenu[0] = ' ' ;
  /*  NOTE_CHAMP_CHANGE(o, contenu) ; */
}
/*
 * Définition de la Classe
 */
CLASSE(gpao_transporteur, struct transporteur,
       CHAMP(reserve_depart , L_entier Extrait CLASSE_PARAMETRE("gpao_reserve"))
       CHAMP(reserve_arrivee, L_entier Extrait CLASSE_PARAMETRE("gpao_reserve"))
       CHAMP(seconde        , L_entier  Extrait Obligatoire Affiche)
       CHAMP(depart         , L_point3  Init3d Edite Sauve DEFAUT("0 0 0"))
       CHAMP(arrivee        , L_point3  Init3d Edite Sauve DEFAUT("2 0 0"))
       CHAMP(longueur       , L_entier  Edite Sauve DEFAUT("4"))
       CHAMP(tempo          , L_entier  Edite Sauve DEFAUT("2"))
       CHAMP(contenu        , L_chaine  Affiche Sauve InitChaine)

       MENU("TP Infographie/GPAO Transporteur")
       CHANGEMENT(changement_transporteur)
       CHAMP_VIRTUEL(L_affiche_gl(affiche_transporteur))
       )

