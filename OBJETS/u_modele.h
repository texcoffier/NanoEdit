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
 * Créé le Wed Feb 26 11:59:41 2003 par Thierry EXCOFFIER
 */

#ifndef _HOME_EXCO_NE_1_13_OBJETS_O_MODELE_H
#define _HOME_EXCO_NE_1_13_OBJETS_O_MODELE_H

typedef struct modele
{
  char *nom_modele ;
  int numero_sauvegarde ;
  int numero_actif ;
  int paquet, decalage ;
  Booleen recharge ;
  char *message ;
  Liste **graphe ; /* contient la liste des fils du modèle : */
				/* C'est une répétition à cause widget graf */
  char *historique ;
  char *historique_interne ;
  char *titre ;
  int init ;
  Booleen ne_pas_sauver ;
  Booleen animation ;
  Operateur oper ;
  char *info ;
  Booleen designe_fils_au_lieu_operateur ;

  Menu fichier ;
  Menu charge ;
  Menu les_actifs ;
  Menu menu_contextuel ;
  Menu liste_actif ;
  Menu creation ;
  Menu creation_plausible ;
  Menu undo ;
  Liste *actifs ;
  char *geometrie ;
  int ouvre_fenetres ;

  int timout_id_sauve_temporaire ;
} Modele ;

void modele_ajoute_fils(Modele *m, void *fils) ;
void *duplique_objet(void *o) ; /* devrait etre dans u_objet.c */
void *duplique_objet_parmi_d_autres(void *o) ;
Parcour variable_change(void *o, void *operateur) ;
Liste *contenu_modele(void *modele) ;
Liste **contenu_modele_ptr(void *modele);
void operateur_debut_eval(void *o) ;
void operateur_fin_eval(void *o) ;



#endif
