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
#include <GL/gl.h>

struct exemple_raccourci
{
  int valeur ;
  char *texte ;
  Triplet pt ;
  Booleen passe_au_suivant ;
  Booleen seulement_si_le_curseur_pres_de_l_objet ;
} ;

static void raccourci(struct exemple_raccourci *o, const Champ *ch, char *v)
{
  NOTE_UN_CHAMP_CHANGE(o) ;  
  reaffecte_chaine_avec_copie(&o->texte, v) ;
  /*
   * On n'accepte l'événement clavier que si le curseur est sur l'objet
   */
  if ( o->seulement_si_le_curseur_pres_de_l_objet
       && lit_numero_tag(ch) == 0
       )
    return ;

  o->pt = char_vers_triplet(strchr(strchr(strchr(strchr(v,' ')+1,' ')+1,' ')+1,' ')+1);

  if ( strncmp(v, "0p", 2) == 0 )
    {
      o->valeur++ ;
      if ( !o->passe_au_suivant )
	v[0] = '\0' ;
    }
  if ( strncmp(v, "0m", 2) == 0 )
    {
      o->valeur-- ;
      if ( !o->passe_au_suivant )
	v[0] = '\0' ;
    }
}

static void decremente(struct exemple_raccourci *o, const Champ *ch, char *v)
{
  reaffecte_chaine_avec_copie(&o->texte, v) ;
  NOTE_UN_CHAMP_CHANGE(o) ;  
}

static void affiche(struct exemple_raccourci *o, const Champ *ch, const char *urgent)
{
  t_tag(0, &o->pt) ;
  glRasterPos3f(o->pt.x,o->pt.y,o->pt.z) ;
  t_chaine( flottant_vers_char(o->valeur) ) ;
}


CLASSE(exemple_raccourci, struct exemple_raccourci,
       (void)decremente ; /* Unused */
       CHAMP(pt, L_point P_triplet Edite Sauve DEFAUT("0.5 0.5"))
       CHAMP(valeur, L_nombre P_entier Edite Sauve Extrait DEFAUT("10"))
       CHAMP(texte, L_chaine Affiche)
       CHAMP(passe_au_suivant, L_booleen Edite Sauve)
       CHAMP(seulement_si_le_curseur_pres_de_l_objet, L_booleen Edite Sauve)
       CHAMP_VIRTUEL(L_raccourci(raccourci))
       CHAMP_VIRTUEL(L_affiche_gl(affiche))
       )
