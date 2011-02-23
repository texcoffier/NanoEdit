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
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>

#include <netdb.h>

#include "u_httpd.h"

static struct
{
  const char *url ;
  void (*fonction)(FILE *c, char* url, void *donnee, char** args, int fildes) ;
  const char *mime_type ;
  void *donnee ;
  int forked ;
  int ferme_fichier ;
} *urls = NULL ;

static int nb_urls ;

/*
 *
 */
void enregistre_url(const char *url,
		    void (*fonction)(FILE *, char*, void *, char**, int),
		    const char *mime_type,
		    void *donnee,
		    int forked,
		    int ferme_fichier)
{
  urls = realloc(urls, (nb_urls+1)*sizeof(*urls)) ;
  urls[nb_urls].url           = url ;
  urls[nb_urls].fonction      = fonction ;
  urls[nb_urls].mime_type     = mime_type ;
  urls[nb_urls].donnee        = donnee ;
  urls[nb_urls].forked        = forked ;
  urls[nb_urls].ferme_fichier = ferme_fichier ;
  nb_urls++ ;
}
/*
 *
 */
char *decode_url(const char *url, char **args)
{
  static char *decodee = NULL ;
  static int taille_decodee = 0 ;
  int code ;
  int nb_args ;
  int i ;

  nb_args = 0 ;
  for( i=0 ; *url!=' ' && *url!='\0' ; url++)
    {
      if ( i>taille_decodee-10 )
	{
	  taille_decodee = 1.5*taille_decodee + 1000 ;
	  decodee = realloc(decodee, taille_decodee) ;
	}

      if ( *url!='%' ) 
	{
	  if ( *url=='?' || *url=='=' || *url=='&' )
	    {
	      decodee[i++] = '\0' ;
	      if ( nb_args < NOMBRE_DE_VALEUR - 1 )
		args[nb_args++] = &decodee[i] ;
	    }
	  else
	    {
	      if ( *url=='+' )
		decodee[i++] = ' ' ;
	      else
		decodee[i++] = *url ;
	    }
	}
      else
	{
	  if ( url[1] && url[2] )
	    {
	      sscanf(url+1, "%2x", &code) ;
	      decodee[i++] = code ;
	      url += 2 ;
	    }
	}
    }
  args[nb_args] = NULL ;
  if ( i==0 )
    return("") ;
  else
    decodee[i++] = '\0' ;
  return(decodee) ;
}
/*
 *
 */
char *encode_url(const char *url)
{
  static Url encodee ;
  char *e ;
  int jumped_columns ;

  e = encodee ;
  jumped_columns = 0 ;
  for( ; *url && (e-encodee)<sizeof(Url)-4 ; url++)
    {
      if ( *url==':' && jumped_columns != 2 )
	{
	  jumped_columns++ ;
	  *e++ = ':' ;
	}
      else
      if ( isalnum(*url) || *url=='/' || *url=='.' || *url=='_' || *url=='-')
	*e++ = *url ;
      else
	{
	  sprintf(e, "%%%02X", *(unsigned char*)url) ;
	  e += 3 ;
	}
    }
  *e = '\0' ;
  return(encodee) ;
}
/*
 *
 */
char *remplace_code(const char* old)
{
  static Url c ;
  char *pc ;

  pc = c ;
  for( ; *old && (pc-c)<sizeof(Url)-6 ; old++)
    {
      if (*old=='>')		/* A votre avis : pourquoi pas un switch ? */
	sprintf(pc, "&gt;") ;
      else
	{
	  if(*old=='<')
	    sprintf(pc, "&lt;") ;
	  else
	    {
	      if(*old=='&')
		sprintf(pc, "&amp;") ;
	      else
		if(*old=='\n')
		  sprintf(pc, "<br>") ;
		else
		  {
		    *pc++ = *old ;
		    continue ;
		  }
	    }
	}
      pc += strlen(pc) ;
    }
  *pc = '\0' ;
  return(c) ;
}
/*
 * Attend un connexion et fourni le document
 */
void http(int socket)
{
  Url buf ;
  int fildes ;
  FILE *client_ecriture ;
  int status ;
  int i ;
#ifdef linux
  unsigned int ui ;
#else
  int ui ;
#endif
  int fait_le_boulot ;
  char *args[NOMBRE_DE_VALEUR] ;
  struct sockaddr s ;
  char *da ;


  waitpid((pid_t)-1, &status, WNOHANG) ; /* Détruit les zombis */

  ui = sizeof(s) ;
  sleep(1);
  fildes = accept(socket,&s,&ui) ;
  i = read(fildes, buf, sizeof(buf)-1) ;
  if ( i <= 0 )
    return ;
  buf[i+1] = '\0' ;
  for(i=0;i<nb_urls;i++)
    {
      if ( strncmp(buf+4, urls[i].url, strlen(urls[i].url)) == 0 )
	{
	  if ( urls[i].forked )
	    {
	      fait_le_boulot = (fork()==0) ; /* Si je suis le fils */
	      if ( !fait_le_boulot )
		signal(SIGPIPE, SIG_IGN) ;
	    }
	  else
	    fait_le_boulot = 1 ;
	  
	  if ( fait_le_boulot )
	    {
	      client_ecriture = fdopen(fildes, "w") ;
	      fprintf(client_ecriture,
		      "HTTP/1.0 200 Document follows\n"
		      "MIME-Version: 1.0\n"
		      "Server: u_httpd.c\n"
		      "Pragma-Type: no-cache\n"
		      "Content-Type: %s\n"
		      "\n", urls[i].mime_type) ;
	      da = decode_url(buf+4+strlen(urls[i].url),args) ;
	      (*urls[i].fonction)(client_ecriture,
				  da,
				  urls[i].donnee,
				  args,
				  fildes
				  ) ;
	      
	      if ( urls[i].ferme_fichier )
		{
		  fclose(client_ecriture) ; /* Fait aussi le close */
		}
	    }
	  else
	    if ( urls[i].ferme_fichier )
	      {
		close(fildes) ;
	      }
	  
	  if ( urls[i].forked && fait_le_boulot )
	      _exit(0) ;
	  else
	    return ;
	}
    }
}
/*
 *
 */
void goto_url(const char *url)
{
  Url commande ;
  char *eurl ;

  eurl = encode_url(url) ;

  sprintf(commande, "firefox '%s' || mozilla '%s' &", eurl, eurl ) ;
  printf("Lance %s\n", eurl) ;
  system(commande) ;
}
/*
 *
 */
void goto_racine(int port, const char *urle)
{
  Url url ;
  char nom[256] ;
  struct hostent *he ;

  gethostname(nom, sizeof(nom)) ;
  he = gethostbyname(nom);
  sprintf(url, "http://%s:%d%s", he->h_name, port, urle) ;
  goto_url(url) ;
}
/*
 *
 */



int ouvre_socket(int port)
{
  int s ;				     /* Le socket que l'on ouvre     */
  struct sockaddr_in name ;		     /* Nom du socket                */
  int err ;				     /* Numero d'erreur de commande  */

  s = socket(AF_INET ,			     /* Le socket est sur le reseau  */
	     SOCK_STREAM,		     /* C'est analog a un pipe       */
	     0				     /* On creer le socket           */
	     ) ;
  if ( s<0 )
    {
      perror(__FILE__ "/ouvre_socket_anonyme") ;
      fprintf(stderr, "Ne peut pas ouvrir le socket de connexion\n") ;
      exit(1) ;
    }
  /*
   * Maintenant on nomme le socket, et on dit avec qui il communique
   */
  memset(&name, 0, sizeof(name)) ;
  name.sin_family      = AF_INET;	     /* Sur le reseau                */
  name.sin_addr.s_addr = INADDR_ANY ;	     /* N'importe qu'elle machine    */
  name.sin_port        = htons(port) ;	     /* Mon numero de port           */
					     /* (Suis pas sur du htons.....) */
  err = bind(s,				     /* Le socket en question        */
	     (struct sockaddr*)&name,	     /* Son nouveau nom              */
	     sizeof(name)
	     ) ;
  if ( err != 0 )
    {
      close(s) ;
      return(-1) ;
    }
  return(s) ;
}
/*
 *
 */
void http_init(int *port, int *socket)
{
  /*
   * Dans le cas ou le navigateur a fermé la connexion.
   * On ne veut pas tuer l'application,
   * il faut donc annuler le signal.
   */
  signal(SIGPIPE, SIG_IGN) ;

  for(;;)
    {
      *socket = ouvre_socket(*port) ;
      if ( *socket >= 0 )
	break ;
      (*port)++  ;
    }
  if ( listen(*socket, 10) )
    {
      perror("Listen") ;
      exit(1) ;
    }
}
/*
 *
 */
void http_stop(int socket)
{
  close(socket) ;
  free(urls) ;
  urls = NULL ;
  nb_urls = 0 ;
}
