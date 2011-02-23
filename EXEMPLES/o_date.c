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
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "u_xt_interface.h"
#include "o_objet.h"

struct dada
{
  int secondes ;		/* Nombre de secondes depuis la création */
  int start ;			/* Date de création */
  int            pid ;		/* Pour pouvoir tuer date */
  int            pipe[2] ;	/* Pour lire la date */
} ;
/* Cette fonction est appelée chaque fois qu'il y a quelque chose
 * à lire dans le fichier */
static Parcour chose_a_lire(void* objet, int fildes)
{
  time_t date ;
  struct dada *o ;

  o = objet ;
  read(o->pipe[0],&date,sizeof(date)) ; /* Lit la date */
  o->secondes = time(NULL) - o->start ;
  /* D'habitude on indique pas que l'on change des champs de l'objets.
   * En effet, le changement est la conséquence d'un autre changement
   * d'un autre champ.
   * Ici, le champ se modifie de lui-même, il faut prévenir NanoÉdit.
   */
  NOTE_UN_CHAMP_CHANGE(o) ;
  /*
   * Il faut ensuite propager le changement pour tout mettre à jour.
   */
  if ( ACTIF(o) ) /* Pas tout le temps sinon c'est trop consommateur CPU */
    evaluation(o) ;
  return(Continue) ;
}

static void changement(struct dada *o)
{
  if ( o->pid || CREATION(o) )		/* Le processus est déjà lancé */
    return ;

  pipe(o->pipe) ;		/* Ouvre le fichier de communication */
  /* Création d'un nouveau processus, en gardant le pipe ouvert */
  o->pid = u_fork(o->pipe[1]) ;
  if ( o->pid )
    {
      /* Pere :
       * On demande de nous prévenir en cas
       * d'arrivé de données dans le pipe. */
      xt_ajoute_fichier(o, chose_a_lire, o->pipe[0]) ;
      o->start = time(NULL) ;
    }
  else
    {
      time_t date ;
      /* fils */
      for(;;)			/* Le process est tué par un kill */
	{
	  time(&date) ;		/* Récupere le temps */
	  write(o->pipe[1], &date,sizeof(date)) ; /* Envoit le temps */
	  sleep(1) ;		/* Attend un peu */
	}
    }
}

static void destruction(struct dada *o)
{
  int status ;

  if ( o->pid )
    {
      kill(o->pid,SIGTERM) ;	/* On tue le processus */
      waitpid(o->pid,&status,0) ; /* On attend sa mort */
      close(o->pipe[0]) ;
      close(o->pipe[1]) ;
      xt_enleve_fichier(o, chose_a_lire, o->pipe[0]) ;
      o->pid = 0 ;
    }
}

CLASSE(date, struct dada,
       CHAMP(secondes, L_nombre P_entier Affiche)
       CHANGEMENT(changement)
       DESTRUCTION(destruction)
       )
