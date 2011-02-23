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
/* LIB: -lXm -lXt -lXext -lSM -lICE -lX11 */


/*
 * Enlever le passage par une liste : a_manager
 */

#include <unistd.h>
#include "u_xt_interface.h"
#include <ctype.h>
#include "s_string.h"
#include "u_httpd.h"
#include "t_geometrie.h"
#include "u_repartiteur.h"

/*
 *
 */



/*
 *
 */
static void x_init_color(Xt_interface *o)
{
  char color[11] ;
  int i ;

  for(i=0;i<8;i++)
    {
      color[0] = '#' ;
      color[2] = color[1] = (i&1) ? 'F' : '0' ;
      color[3] = color[4] = (i&2) ? 'F' : '0' ;
      color[5] = color[6] = (i&4) ? 'F' : '0' ;
      color[7] = '\0' ;
      widget_allocation_couleur(o->sommet, color, &o->table[i]) ;
    }
}
/*
 *
 */
static Parcour connexion_http(void *objet, int fildes)
{
  http( fildes ) ;
  return(Continue) ;
}
static NanoIdle recupere_en_attente = 0 ; /* STATICOK */

static PROTOTYPE_CALLBACK_IDLE(u_rien_a_faire)
{
  if ( globale_debug )
    eprintf("rien a faire pour l'interface : faire action arrière plan\n") ;

  /* Si de nouvelles choses urgentes sont apparues, alors on essaye
     pas de les exécuter en arrière plan, on fini d'abord
     la prise en compte des événements.
  */
  if ( u_plus_petit_interval_temps() <= PRIORITE_RECUPERE_URGENT
       && recupere_en_attente )
    {
      widget_enleve_idle(recupere_en_attente) ;
      recupere_en_attente = 0 ;
      FIN_CALLBACK_IDLE_ARRETE ;
    }
  /*
   * On exécute une action
   */
  if ( !u_execute_action() )
    {
      if ( globale_debug )
	eprintf("Plus d'action arrière plan recupere_en_attente=%d\n",
		recupere_en_attente) ;
      if ( recupere_en_attente )
	{
	  widget_enleve_idle(recupere_en_attente) ;
	  recupere_en_attente = 0 ;
	  FIN_CALLBACK_IDLE_ARRETE ;
	}
    }
  FIN_CALLBACK_IDLE ;
}


/*
 *
 */
static void changement_xt_interface(Xt_interface *o)
{
  static int premiere_fois = 1 ;
  static Xt_interface sauvegarde ;
  Interface_widget iw ;

  if ( o->init == 0 )
    {
      o->init = 1 ;

      (*OBJET(o)->classe->sc->changement)(o) ;	/* Appel changement de la surclasse */

      if ( premiere_fois )
	{
	  putenv( "XENVIRONMENT=.NanoEdit.ad" ) ;
	  o->sommet = widget_initialise_application(&o->interface.argc
						    , &o->interface.argv
						    , &o->app_context
						    ) ;
	  x_init_color(o) ;
	  if ( TRACE(o) )
	    eprintf("socket = %d\n", o->interface.socket) ;

          sauvegarde.app_context = o->app_context ;
          sauvegarde.sommet      = o->sommet      ;
	  memcpy(sauvegarde.table, o->table, sizeof(o->table)) ;
	  premiere_fois = 0 ;

	}
      else
	{
          o->app_context = sauvegarde.app_context ;
          o->sommet      = sauvegarde.sommet      ;
	  memcpy(o->table, sauvegarde.table, sizeof(o->table)) ;
	  
	}
      xt_ajoute_fichier(o, connexion_http, o->interface.socket) ;

      iw.objet = o ;
      iw.w = o->sommet ;
      POUR_LES_CLASSES
	(c, POUR_LES_CHAMPS___CLASSE
	 (c, ch, (*ch->te->start_classe)(&iw););) ;

      if ( o->interface.ouvre_fenetre_modele )
	{
	  ouvrir_fenetre(FILS(o,0)) ;
	  recupere_fenetre(FILS(o,0)) ;
	}
      o->historique = strdup("") ;
      do
	{
	  /* Exécute tout ce qui est urgent */
	  while( u_plus_petit_interval_temps() <= PRIORITE_RECUPERE_URGENT )
	    {
	      u_execute_action() ;
	    }
	  /* Analyse tous les événements */
	  while (widget_process_event(o->app_context) )
	    {
	    }
	  /* S'il reste des choses à exécuter, alors en arriere plan */
	  if ( recupere_en_attente==0 && u_quelque_chose_a_executer())
	    recupere_en_attente = widget_ajoute_idle(o->app_context
						     , u_rien_a_faire) ;
	}
      while( !o->interface.stop ) ;
      o_appel_changement(o) ;	/* POURQUOI ? */
      return ;
    }

  /*  (*OBJET(o)->classe->sc->changement)(o) ;  Appel changement de la surclasse */

}
/*
 *
 */

static Parcour u_detruit(void *objet, void *interface)
{
  detruire_objet(objet) ;
  return(Continue) ;
}

static void destruction_xt_interface(Xt_interface *o)
{
  Interface_widget iw ;


  c_sauve(FILS(o,0), "u_sauve_modele", NOM_FICHIER_SAUVEGARDE) ;
  u_enleve_toutes_les_actions() ;
  o_parcour_sauf_racine( o, NULL, u_detruit, NULL) ;

  while ( u_execute_action() ) /* vide */ ;

  if ( recupere_en_attente )
    {
      widget_enleve_idle(recupere_en_attente) ;
      recupere_en_attente = 0 ;
    }
  xt_enleve_fichier(o, connexion_http, o->interface.socket) ;

  POUR(struct fichier*, f, o->attentes_fichier,
       xt_enleve_fichier(f->objet, f->fonction, f->fildes) ;
       );
  
  iw.objet = o ;
  iw.w = o->sommet ;
  POUR_LES_CLASSES
    (c, POUR_LES_CHAMPS___CLASSE
     (c, ch, (*ch->te->stop_classe)(&iw););) ;

  POUR_LES_CLASSES
    (
     cl,
     {
       POUR(NanoWidget, w, cl->fenetres_unmappees,
	    {
	      widget_detruit(w) ;
	    }) ;
       l_detruit(&cl->fenetres_unmappees) ;
     }) ;	   

  (*OBJET(o)->classe->sc->destruction)(o) ;

}
/*
 *
 */
static void widget_applique(void *objet, const Champ *ch, char *v)
{
  applique_fenetre((void*)atol(v)) ;
}

CLASSE(xt_interface, Xt_interface,
       SUR_CLASSE("interface")
       CHANGEMENT(changement_xt_interface)
       DESTRUCTION(destruction_xt_interface)
       CHAMP_VIRTUEL(NOM("widget_applique") L_fonction(widget_applique))
       )
