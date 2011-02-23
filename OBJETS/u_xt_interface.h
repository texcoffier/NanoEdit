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
 * Créé le Fri Apr 23 11:07:18 1999 par Thierry EXCOFFIER
 */

#ifndef U_XT_INTERFACE_H
#define U_XT_INTERFACE_H

#include "o_objet.h"
#include "o_interface.h"	/* Racine de tous les interfaces */

typedef struct xt_interface
{
  Interface interface ;
  /*
   * Dépendant de Xt
   */
  NanoAppContext app_context ;
  NanoWidget       sommet ;		/* La racine de l'application */
  NanoColor       table[8] ;	/* Les 8 couleurs classique */
  NanoInputId    http ;		/* Socket */
  NanoWidget   help_shell ;
  char         *historique ;
  int init ;
  int y ;			/* Position de creation de fenêtre */

  NanoEvent       *event ;	/* L'evenement X11 lui-meme */
  NanoWidget       widget ;	/* Widget ayant recu l'evenement */
  Liste        *attentes_fichier;/* (objet/function/Xid) */
} Xt_interface ;

struct fichier
{
  void *objet ;
  Parcour (*fonction)(void *objet, int fildes) ;
  NanoInputId id ;
  int fildes ;
} ;




void xt_ajoute_fichier(void *objet,
		       Parcour (*fonction)(void *objet, int fildes),
		       int fildes
		       ) ;
void xt_enleve_fichier(void *objet,
		      Parcour (*fonction)(void *objet, int fildes),
		      int fildes
		      ) ;

NanoWidget       xt_champ                      (NanoWidget w, const char *nom) ;
const Champ* xt_objet_et_widget_vers_champ (void *o, NanoWidget w        ) ;
void*        xt_widget_vers_objet          (NanoWidget w                 ) ;
const Champ* xt_widget_vers_champ          (NanoWidget w                 ) ;

void                  w_applique(void *) ;
void              ouvrir_fenetre(void *) ;
void              fermer_fenetre(void *) ;
void            recupere_fenetre(void *) ;
void            applique_fenetre(void *) ;
void              detruire_objet(void *) ;
void detruire_objet_remonte_fils(void *) ;
void            unmapper_fenetre(void *) ;
void                  evaluation(void *) ;
void           u_popup_help_reel(const Interface_widget *iw) ;




#endif
