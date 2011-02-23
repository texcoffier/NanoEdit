/*
    NanoÉdit: Un modeleur algébrique interactif.
    Copyright (C) 2010 Thierry EXCOFFIER, Université Claude Bernard, LIRIS

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

/* Need package : libsdl-dev */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "u_xt_interface.h"
#include "o_objet.h"
#include "t_geometrie.h"
#include <SDL.h>


struct joystick
{
  char *joystickv ;
  SDL_Joystick *sdl_joy;
  Table_entier axis ;
  int idle ;
} ;

static int chose_a_lire(void* objet)
{
  struct joystick *o ;
  int i ;

  o = objet ;

  if ( ! ACTIF(o) )
     return 1 ;

  SDL_JoystickUpdate() ;
  for(i=0; i<o->axis.nb; i++)
    {
      o->axis.table[i] = SDL_JoystickGetAxis(o->sdl_joy, i) ;
    }

  NOTE_CHAMP_CHANGE(o, axis) ;

  /*
   * Il faut ensuite propager le changement pour tout mettre à jour.
   */
  evaluation(o) ;
  return 1 ;
}

static void destruction(struct joystick *o)
{
  if ( o->sdl_joy )
    SDL_JoystickClose(o->sdl_joy) ;

  if ( o->idle )
    widget_timeout_remove((void*)o->idle) ;
}

static void changement(struct joystick *o)
{
  static int initiated = 0 ;

  if ( initiated == 0 )
    {
      int i ;
      initiated = 1 ;
      SDL_Init(SDL_INIT_VIDEO |SDL_INIT_JOYSTICK) ;
      printf("%i joysticks were found:\n", SDL_NumJoysticks() );
      for( i=0; i < SDL_NumJoysticks(); i++ ) 
	{
	  printf("    %s\n", SDL_JoystickName(i));
	}
    }


  if ( CHAMP_CHANGE(o, joystickv) )
    {
      o->sdl_joy = SDL_JoystickOpen(o->joystickv[strlen(o->joystickv)-1]-'0') ;
      if ( o->sdl_joy )
	{
	  o->idle = widget_timeout_add(chose_a_lire, 1000/25, o) ;
	  CHANGE_TAILLE(o, axis,  SDL_JoystickNumAxes(o->sdl_joy)) ;
	}
    }
}

CLASSE(joystick, struct joystick,
       CHAMP(joystickv, L_chaine Edite DEFAUT("/dev/input/js0")
	     Sauve InitChaine)
       CHAMP(axis, L_table_int Affiche) 
       CHANGEMENT(changement)
       DESTRUCTION(destruction)
       MENU("Valeurs/Joystick")
       EVENEMENT("JS")
       )

struct joy_transfo
{
  Transfo t ;
  Table_entier axis ;
  char *quoi ;
} ;

static void changement_joy_transfo(struct joy_transfo *o)
{
  int i, v ;
  Triplet vec ;

  if ( ! FILS_CHANGE(o) )
    return ;

  for(i=0; i < o->axis.nb && o->quoi[i]; i++)
    {
      v = o->axis.table[i] ;
      switch(o->quoi[i])
	{
	case 'D':
	  vec = u_triplet_fois_flottant(&o->t.v3, v / 60000.) ;
	  o->t.origine = u_ajoute_triplet(&o->t.origine, &vec) ;
	  break ;
	case 'G':
	  vec = u_triplet_fois_flottant(&o->t.v2, v / 60000.) ;
	  o->t.origine = u_ajoute_triplet(&o->t.origine, &vec) ;
	  break ;
	case 'A':
	  vec = u_triplet_fois_flottant(&o->t.v1, v / -60000.) ;
	  o->t.origine = u_ajoute_triplet(&o->t.origine, &vec) ;
	  break ;
	case 'g':
	  o->t.v1 = u_rotation_triplet(&o->t.v1, &o->t.v3, v / 300000.) ;
	  o->t.v2 = u_rotation_triplet(&o->t.v2, &o->t.v3, v / 300000.) ;
	  break ;
	case 'h':
	  o->t.v1 = u_rotation_triplet(&o->t.v1, &o->t.v2, v / 300000.) ;
	  o->t.v3 = u_rotation_triplet(&o->t.v3, &o->t.v2, v / 300000.) ;
	  break ;
	case 't':
	  o->t.v2 = u_rotation_triplet(&o->t.v2, &o->t.v1, v / 300000.) ;
	  o->t.v3 = u_rotation_triplet(&o->t.v3, &o->t.v1, v / 300000.) ;
	  break ;
	}
    }

}


CLASSE(joy_transfo, struct joy_transfo,
       CHANGEMENT(changement_joy_transfo)
       CHAMP(t, L_transfo Edite Sauve)
       CHAMP(quoi, L_chaine Edite Sauve DEFAUT("gAGDth")
	     BULLE_D_AIDE("A : Avance/Recule\n"
			  "D : Descend/Monte\n"
			  "G : Va à gauche/droite\n"
			  "g : Tourne à gauche/droite\n"
			  "h : Tourne en haut/bas\n"
			  "t : Tourne l'écran\n"
			  )
	     )
       CHAMP(axis,L_table_int Extrait Affiche Obligatoire)

       MENU("Attributs/Géométrique 3D/Joystick Transformation")
       EVENEMENT("JT")
       
       )
