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
#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "u_xt_interface.h"
#include "t_geometrie.h"
#include "o_objet.h"
#include "u_repartiteur.h"

struct dada
{
  int            pid ;		/* Pour pouvoir tuer date */
  int            pipe[2] ;	/* Pour lire la date */
  Booleen        onoff ;
  struct
  {
    Flottant courant ;
    Flottant depart ;
    Flottant final ;
  } temps ;
  Flottant increment ;
  Flottant dilatation ;
} ;
/*
 * Cette fonction est appelée chaque fois qu'il y a quelque chose
 * à lire dans le fichier
 */
static Parcour chose_a_lire(void* objet, int fildes)
{
  int buf ;
  struct dada *o ;

  o = objet ;
  read(o->pipe[0],&buf,1) ;

  if ( o->onoff && !u_quelque_chose_a_executer() )
    {
      o->temps.courant += o->increment * o->dilatation ;

      /*
       * D'habitude on indique pas que l'on change des champs de l'objets.
       * En effet, le changement est la conséquence d'un autre changement
       * d'un autre champ.
       * Ici, le champ se modifie de lui-même, il faut prévenir NanoEdit.
       */
      NOTE_UN_CHAMP_CHANGE(o) ;
      /*
       * Il faut ensuite propager le changement pour tout mettre à jour.
       */
      evaluation(o) ;

      if ( o->temps.courant >= o->temps.final )
	{
	  o->onoff = 0 ;
	  o->temps.courant = o->temps.final ;
	}
    }
  return(Continue) ;
}

static void destruction(struct dada *o)
{
  int status ;

  if ( o->pid )
    {
      kill(o->pid,SIGTERM) ;	/* On tue le processus */
      xt_enleve_fichier(o, chose_a_lire, o->pipe[0]) ;
      waitpid(o->pid,&status,0) ; /* On attend sa mort */
      close(o->pipe[0]) ;
      close(o->pipe[1]) ;
      o->pid = 0 ;
    }
}

static void changement(struct dada *o)
{
  if ( CHAMP_CHANGE(o, increment) )
    destruction(o) ;

  if ( o->pid )		/* Le processus est déjà lancé */
    return ;

  pipe(o->pipe) ;		/* Ouvre le fichier de communication */
  o->pid = u_fork(o->pipe[1]) ;	/* Création d'un nouveau processus */
  if ( o->pid )
    {
      /*
       * Pere :
       * On demande de nous prévenir en cas
       * d'arrivé de données dans le pipe.
       */
      xt_ajoute_fichier(o, chose_a_lire, o->pipe[0]) ;
    }
  else
    {
      /* fils */
      for(;;)			/* Le process est tué par un kill */
	{
	  write(o->pipe[1], "", 1) ;
	  usleep(1000000*o->increment) ;		/* Attend un peu */
	}
    }
}

static Booleen revient(const Interface_widget *iw)
{
  struct dada *o = iw->objet ;

  o->temps.courant = o->temps.depart ;
  NOTE_UN_CHAMP_CHANGE(o);

  return(Faux) ;
}

static Booleen onoff(const Interface_widget *iw)
{
  struct dada *o = iw->objet ;

  o->onoff ^= 1 ;

  return(Faux) ;
}

CLASSE(seconde, struct dada,
       CHAMP(temps, L_flottant_borne Edite Sauve
	     BULLE_D_AIDE("Le temps courant en seconde")
	     )
       CHAMP(increment, L_nombre P_flottant Edite Sauve DEFAUT("1")
	     BULLE_D_AIDE("L'interval de temps entre deux évaluations\n"
			  "de cette objet et donc la mise à jour\n"
			  "de la valeur du temps courant.\n"
			  "\n"
			  "Si cette valeur est trop petite alors l'affichage\n"
			  "ne se fera plus en temps réel, il sera plus lent.\n"
			  "\n"
			  "La valeur 0.04 représente 25 images/seconde.\n"
			  )
	     )
       CHAMP(dilatation, L_nombre P_flottant Edite Sauve DEFAUT("1")
	     BULLE_D_AIDE("La dilatation temporelle permet de ralentir\n"
			  "ou accélérer l'écoulement du temps.\n"
			  "La valeur 1 doit être utilisée pour\n"
			  "faire du temps réel.\n"
			  )
	     )
       CHAMP(onoff, L_booleen Sauve DEFAUT("0") NonCree)
       DEBUT_BOITE(HORIZONTALE)
       CHAMP_VIRTUEL(LABEL("Revient au début") L_bouton(revient))
       CHAMP_VIRTUEL(LABEL("On/Off") L_bouton(onoff))
       FIN_BOITE
       
       CHANGEMENT(changement)
       DESTRUCTION(destruction)
       MENU("Valeurs/Secondes")
       EVENEMENT("SEC")
       )
