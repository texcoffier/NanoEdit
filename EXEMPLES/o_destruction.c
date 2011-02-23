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
#include <GL/gl.h>
#include "o_objet.h"
#include "s_string.h"
#include "u_graphique.h"
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/wait.h>
struct delet
{
  int pid ;			/* PID du processus lancé */
  int pipe[2] ;			/* Pipe de communication */
  FILE *entree ;		/* Fichier lisant dans le pipe */
} ;
/* Cette fonction d'affichage va afficher a chaque fois
 * une chaine de caractere differentes.
 * Cette chaine de caractere vient d'une commande UNIX. (ls -lsR /) */
static void affichage_ls(struct delet *o)
{
  Buffer ligne ;

  if ( o->pid==0 )
    {
      /* PREMIER AFFICHAGE DE L'OBJET SEULEMENT
       * Ouvre un pipe pour communiquer et lance le processus */
      pipe(o->pipe) ;
      /* Lance le processus */
      o->pid = u_fork(o->pipe[1]) ;
      if ( o->pid == 0 )
	{
	  /* Dans le fils */
	  dup2(o->pipe[1],1) ; /* La sortie standard de la suite = pipe */
	  execlp("ls","ls","-R","/",NULL) ;
	  exit(1) ;
	}
      /* Dans le pere */
      o->entree = fdopen(o->pipe[0],"r") ;
    }
  /* On affiche un texte différent à chaque affichage! */
  fgets(ligne,sizeof(ligne),o->entree) ;
  glRasterPos3f(0.,0.,0.) ;
  t_chaine( ligne ) ;
}
/* Destruction de l'objet */
static void destruction(struct delet *o)
{
  int status ;

  if ( o->pid )		/* + Fiable */
    {
      kill(o->pid, SIGTERM) ;
      waitpid(o->pid, &status, 0) ; /* Evite le Zombi */
      fclose(o->entree) ;
      close(o->pipe[0]) ;
      close(o->pipe[1]) ;
    }
}
/*
 *
 */
CLASSE(destruction, struct delet,
       DESTRUCTION(destruction)
       CHAMP_VIRTUEL(L_affiche_gl(affichage_ls) AFFICHAGE(Toujours_different))
       )
