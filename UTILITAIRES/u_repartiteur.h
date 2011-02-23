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
 * Créé le Fri Feb 25 14:14:11 2000 par Thierry EXCOFFIER
 */

#ifndef _HOME_EXCO_NE_1_9_UTILITAIRES_U_REPARTITEUR_H
#define _HOME_EXCO_NE_1_9_UTILITAIRES_U_REPARTITEUR_H

void u_ajoute_action(int interval_temps,Booleen (*action)(void *donne),void *donnee) ;
void u_ajoute_action_unique(int interval_temps,void (*action)(void *donne),void *donnee) ;
void u_ajoute_action_unique_avec_test(int interval_temps,void (*action)(void *donne),void *donnee, Booleen (*test)(void *donnee1, void *donnee2)) ;
void u_enleve_action(Booleen (*action)(void *donne),void *donnee) ;
void u_enleve_action_par_test_de_donnee(Booleen (*action)(void *donne)
					, Booleen (*test)(void *donnee) );
void u_enleve_donnee(void *donnee) ;
Booleen u_execute_action() ;
Booleen u_quelque_chose_a_executer() ;
void u_enleve_toutes_les_actions() ;
int u_plus_petit_interval_temps() ;
void u_dump_toutes_les_actions(FILE *f) ;
#define u_enleve_action_unique(A,B) u_enleve_action((Booleen (*)(void *))A,B)
#define u_enleve_action_unique_par_test_de_donnee(A,B) u_enleve_action_par_test_de_donnee((Booleen (*)(void *))A,B)

struct une_action ;
struct une_action *trouve_action(Booleen (*action)(void *donne),void *donnee) ;

#define PRIORITE_BULLE_AIDE      10000000
#define PRIORITE_CHANGEMENT      1000000
#define PRIORITE_SAUVEGARDE      100000
#define PRIORITE_ARRIERE_PLAN    1000
#define PRIORITE_EXPOSE               120
#define PRIORITE_RECUPERE             110
#define PRIORITE_REGENERE_MENU        105 /* Avant récupère : Menu Detachab. */
#define PRIORITE_RECUPERE_URGENT      100
#define PRIORITE_UNMAPPER_FENETRE     95
#define PRIORITE_OUVRIR_FENETRE       90
#define PRIORITE_CALCUL_MAX_ACTIF     86
#define PRIORITE_ENLEVE_NULL          85
#define PRIORITE_DETRUIRE_OBJET       80
#define PRIORITE_EVALUATION       81
#define PRIORITE_FERMER_FENETRE       75
#define PRIORITE_APPLIQUE             65
#define PRIORITE_SAUVEGARDE_ANIMATION 60


#endif
