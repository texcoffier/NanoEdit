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
 * Créé le Mon Apr 10 10:47:18 2000 par Thierry EXCOFFIER
 */

#ifndef _HOME_EXCO_NE_1_10_OBJETS_U_ETAT_COURANT_H
#define _HOME_EXCO_NE_1_10_OBJETS_U_ETAT_COURANT_H

#define PLUS_PRIORITAIRE(NOM)						   \
(									   \
  *modif = 								   \
  ( globale_etat_courant.CONCATENE(NOM,_priorite) <= o->ec.priorite &&	   \
    (globale_etat_courant.CONCATENE(NOM,_priorite) = o->ec.priorite, Vrai) \
  )									   \
)

typedef enum { Ponctuel, Filaire, Facette, AfficheDebug, Invisible } Type_affichage  ;
typedef enum { Point_de_R3, TeinteSaturation, TeinteLuminosite, Palette_Invisible } Type_palette ;

/*
 * Ce define est là car il n'existe pas d'attributs Matrix
 * pour PushAttrib, j'en cré donc 1
 */
#define NANOEDIT_MATRIX_BIT (1<<30)

#define MAX_VARIABLE 100

extern struct globale_etat_courant
{
  /*
   * Ne pas changer l'ordre des champs suivant : voir "u_vue.c"
   */
  Triplet        couleur        ;
  Flottant       diffuse        ;
  Flottant       speculaire     ;
  Triplet        ambiante       ;
  Flottant       reflexion      ;
  Flottant       refraction     ;
  Flottant       indice         ;
  Flottant       n              ;
  Type_affichage type_affichage ;
  Transfo        transfo        ; /* Transformation courante */
  int            lampe          ;
  Flottant       epaisseur_trait; /* Pour sortie POV des traits */
  void          *operateur_racine;
  void (*detourne_affichage)(void *o, unsigned int pos) ;
  int decoupage_hls[3] ; /* Pour la palette de couleur */
  Booleen        affiche_ligne_de_rappel ; /* Ligne brisée vers l'origine */
  Booleen        designable     ; /* Les objets affichés sont désignable */
  Type_palette   affichage_palette_couleur ; /* couleur & choix */
  Flottant       stl_offset ;
  /*
   * Vous pouvez changer l'ordre de la suite
   */

  int couleur_priorite                  ;
  int diffuse_priorite                  ;
  int speculaire_priorite               ;
  int ambiante_priorite                 ;
  int reflexion_priorite                ;
  int refraction_priorite               ;
  int indice_priorite                   ;
  int n_priorite                        ;
  int type_affichage_priorite           ;
  int epaisseur_trait_priorite          ;
  int affiche_ligne_de_rappel_priorite  ;
  int designable_priorite               ;
  int decoupage_hls_priorite            ;
  int affichage_palette_couleur_priorite;
  int stl_offset_priorite               ;


  /*
   * Ce qui suit n'est pas empilé dans la pile d'état courant
   */
  int ne_pas_sauvegarder_les_attributs_pov ;

  void *variables[MAX_VARIABLE] ;	/* Pour Opérateur/Variable */

  Booleen stl_couleur ;

  long gl ; /* ID Fenetre GL en cours */

} globale_etat_courant ;


#endif
