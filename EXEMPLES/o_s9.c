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
struct segment { Triplet depart, arrivee ; } ;

static void ecrit_milieu(struct segment *s, const Champ *c, const char *v)
{
  Triplet ancien_milieu, nouveau_milieu, vecteur ;

  if ( CHAMP_CHANGE(s,depart) || CHAMP_CHANGE(s, arrivee) )
    return ; /* On change le milieu seulement s'il est le seul à changer */

  ancien_milieu  = u_milieu_2_triplet(&s->depart, &s->arrivee) ;
  nouveau_milieu = char_vers_triplet(v) ;
  vecteur        = u_soustrait_triplet(&nouveau_milieu, &ancien_milieu) ;
  s->depart      = u_ajoute_triplet(&s->depart, &vecteur) ;
  s->arrivee     = u_ajoute_triplet(&s->arrivee, &vecteur) ;
}

static const char* lit_milieu(struct segment *s, const Champ *c)
{
  Triplet milieu ;

  milieu = u_milieu_2_triplet(&s->depart, &s->arrivee) ;
  
  return( triplet_vers_char(&milieu) ) ;
}

CLASSE(segment9,
       struct segment,
       CHAMP(depart , L_point P_triplet    Edite   Sauve Init3d Extrait)
       CHAMP(arrivee, L_point P_triplet    Edite   Sauve Init3d Extrait)
       CHAMP_VIRTUEL(L_fonction2(ecrit_milieu, lit_milieu) Init3d) /**/
      )
CLASSE(segment9_,
       struct segment,
       CHAMP(depart , L_point P_triplet    Edite   Sauve Init3d Extrait)
       CHAMP(arrivee, L_point P_triplet    Edite   Sauve Init3d Extrait)
       CHAMP_VIRTUEL(LABEL("Milieu") Edite Init3d
		     L_point P_virtuel(ecrit_milieu, lit_milieu))
      )
