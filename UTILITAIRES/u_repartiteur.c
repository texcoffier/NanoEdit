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
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include "u_base.h"
#include "l_liste.h"
#include "eprintf.h"
#include "u_repartiteur.h"
#include "s_string.h"

#if 0
#define EPRINTF eprintf
#else
#define EPRINTF while(0) eprintf
#endif

static int globale_profondeur = 0 ;

struct une_action
{
  int interval_temps ;		/* Nombre d'appel entre deux évaluations */
  int temps ;			/* Temps du dernier appel */
  void *donnee ;		/* Donnée utilisateur */
  Booleen une_fois ;
  Booleen (*action)(void *donnee) ; /* Retourne Vrai si continue */
} ;

static Liste *globale_repartiteur = NULL ;
static int globale_temps = 0 ;
static Liste *globale_a_enlever = NULL ;

void enlever_actions_detruites()
{
  POUR(struct une_action*, a, globale_a_enlever,
       if ( l_enleve(&globale_repartiteur, a) )
       free(a) ;
       ) ;
  l_detruit(&globale_a_enlever) ;
}


struct une_action *trouve_action_fine(int interval_temps,
				  Booleen (*action)(void *donne),
				  void *donnee)
{
  POUR(struct une_action*, a, globale_repartiteur,
       {
	 if ( a->action == action
	      && a->donnee == donnee
	      && a->interval_temps == interval_temps
	      )
      {
	return(a) ;
      }
  } ) ;
  return(NULL) ;
}

struct une_action *trouve_action(Booleen (*action)(void *donne),void *donnee)
{
  POUR(struct une_action*, a, globale_repartiteur,
       {
	 if ( a->action == action && a->donnee == donnee )
      {
	return(a) ;
      }
  } ) ;
  return(NULL) ;
}

#define AAAA(X,Y) ((a->interval_temps==X) ? #X : Y)

static const char * u_dump_action(const struct une_action *a)
{
  static Chaine s ;
  
  s_efface(&s) ;
  s_printf(&s
	  ,"%d DT=%-28s(%5d) T=%8d Uniq=%d Data=%p"
	  , globale_profondeur
,
AAAA(PRIORITE_BULLE_AIDE,
AAAA(PRIORITE_CHANGEMENT,
AAAA(PRIORITE_SAUVEGARDE,
AAAA(PRIORITE_ARRIERE_PLAN,
AAAA(PRIORITE_EXPOSE,
AAAA(PRIORITE_RECUPERE,
AAAA(PRIORITE_RECUPERE_URGENT,
AAAA(PRIORITE_UNMAPPER_FENETRE,
AAAA(PRIORITE_OUVRIR_FENETRE,
AAAA(PRIORITE_EVALUATION,
AAAA(PRIORITE_REGENERE_MENU,
AAAA(PRIORITE_DETRUIRE_OBJET,
AAAA(PRIORITE_FERMER_FENETRE,
AAAA(PRIORITE_CALCUL_MAX_ACTIF,
AAAA(PRIORITE_APPLIQUE,
AAAA(PRIORITE_ENLEVE_NULL,
AAAA(PRIORITE_SAUVEGARDE_ANIMATION,
     0)))))))))))))))))
	  , a->interval_temps, a->temps, a->une_fois, a->donnee) ;
  return(s_char(&s)) ;
}


void u_ajoute_action_(int interval_temps,Booleen (*action)(void *donne),void *donnee, Booleen une_fois)
{
  struct une_action *a ;

  a = trouve_action_fine(interval_temps, action, donnee) ;
  if ( a )
    {
      l_enleve(&globale_a_enlever, a) ;
      return ;
    }

  ALLOUER(a,1) ;
  l_ajoute(&globale_repartiteur, a) ;

  a->interval_temps    = interval_temps ;
  a->action            = action         ;
  a->donnee            = donnee         ;
  a->temps             = globale_temps-1;
  a->une_fois          = une_fois ;
  EPRINTF("+ACTION : %s\n", u_dump_action(a)) ;  
}

void u_ajoute_action(int interval_temps,Booleen (*action)(void *donne),void *donnee)
{
  POUR(struct une_action*, a, globale_repartiteur,
       {
	 if ( a->action == action && a->donnee == donnee )
      {
	return ;
      }
  } ) ;
  u_ajoute_action_(interval_temps, (Booleen (*)(void *))action, donnee, Faux) ;
}

void u_ajoute_action_unique_avec_test(int interval_temps,void (*action)(void *donne),void *donnee, Booleen (*test)(void *donnee1, void *donnee2))
{
  POUR(struct une_action*, a, globale_repartiteur,
       {
	 if ( a->action == (Booleen (*)(void *))action && (*test)(a->donnee, donnee) )
      {
	return ;
      }
  } ) ;

  u_ajoute_action_(interval_temps, (Booleen (*)(void *))action, donnee, Vrai) ;
}

void u_ajoute_action_unique(int interval_temps,void (*action)(void *donne),void *donnee)
{
  u_ajoute_action_(interval_temps, (Booleen (*)(void *))action, donnee, Vrai) ;
}

void u_enleve_action(Booleen (*action)(void *donne),void *donnee)
{
  struct une_action *a ;

  a = trouve_action(action, donnee) ;
  if ( a )
    {
      EPRINTF("-ACTION : %s\n", u_dump_action(a)) ; 
      l_ajoute(&globale_a_enlever, a) ;
    }
}

void u_enleve_action_par_test_de_donnee(Booleen (*action)(void *donne)
					, Booleen (*test)(void *donnee) )
{
  POUR(struct une_action*, a, globale_repartiteur,
       {
	 if ( a->action == action)
	 if ( (*test)(a->donnee) )
      {
	EPRINTF("-ACTION^: %s\n", u_dump_action(a)) ; 
	l_ajoute(&globale_a_enlever, a) ;
      }
  } ) ;
}

void u_enleve_donnee(void *donnee)
{
  POUR(struct une_action*, a, globale_repartiteur,
       {
	 if ( a->donnee == donnee )
      {
	EPRINTF("-ACTION*: %s\n", u_dump_action(a)) ;  
	l_ajoute(&globale_a_enlever, a) ;
      }
  }
       ) ;
}

/*
 * Pourquoi le rand() pose des problèmes ?
 */
Booleen u_execute_action()
{
  struct une_action *action ;
  float p, p_min ;

  enlever_actions_detruites() ;
  p_min = 0 ;
  action = NULL ; /* Pour éviter un Warning */
  POUR(struct une_action*, a, globale_repartiteur,
       {
	 p = ( globale_temps - a->temps /* +(rand()%1000)/10000. */) / (float)a->interval_temps ;
	 if ( p > p_min )
      {
	p_min = p ;
	action = a ;
      }
  }) ;


  if ( p_min != 0 )
    {
      EPRINTF("EXECUTE : %s\n", u_dump_action(action)) ;
      globale_profondeur++ ;
      if ( action->une_fois )
	{
	  (*action->action)(action->donnee) ;
	  l_ajoute(&globale_a_enlever, action) ;
	}
      else
	{
	  if ( (*action->action)(action->donnee) )
	    {
	      if ( l_trouve(globale_repartiteur, action ) )
		action->temps = globale_temps ;
	      globale_temps++ ;
	    }
	  else
	    {
	      l_ajoute(&globale_a_enlever, action) ;
	    }
	}
      globale_profondeur-- ;
    }
  enlever_actions_detruites() ;
  return( p_min != 0 ) ;
}

Booleen u_quelque_chose_a_executer()
{
  enlever_actions_detruites() ;
  return(globale_repartiteur!=NULL) ;
}

int u_plus_petit_interval_temps()
{
  int p ;

  enlever_actions_detruites() ;
  p = INT_MAX ;
  POUR(struct une_action*, action, globale_repartiteur,
       {
	 if ( p > action->interval_temps )
	 p = action->interval_temps ;
  }
       ) ;
  return(p) ;
}

void u_enleve_toutes_les_actions()
{
  POUR(struct une_action*, a, globale_repartiteur, free(a) ; ) ;
  l_detruit(&globale_repartiteur) ;
}


void u_dump_toutes_les_actions(FILE *f)
{
  fprintf(f, "Liste des actions, Temps=%d\n",globale_temps ) ;
  POUR(struct une_action*, a, globale_repartiteur,
       fprintf(f, "%s\n", u_dump_action(a)) ; ) ;
}
