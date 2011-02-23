#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netdb.h>

int ouvre_connexion(char *machine, int port)
{
  int s ;				     /* Le socket que l'on ouvre     */
  struct sockaddr_in name ;		     /* Nom du socket                */
  int err ;				     /* Numero d'erreur de commande  */
  struct hostent *mmserv ;		     /* Le host du serveur           */

  s = socket(AF_INET ,			     /* Le socket est sur le reseau  */
	     SOCK_STREAM,		     /* C'est analog a un pipe       */
	     0				     /* On creer le socket           */
	     ) ;

  if ( s<0 )
    {
      perror(__FILE__ "/ouvre_connexion") ;
      fprintf(stderr, "Ne peut pas ouvrir le socket de connexion\n") ;
      exit(1) ;
    }
  /*
   * On dit a qui l'on veut connecter le socket
   */

  mmserv = gethostbyname(machine) ;
  if ( mmserv==0 )
    {
      perror(__FILE__) ;
      fprintf(stderr, "Ne trouve pas la machine serveur\n" );
      fprintf(stderr, "Verifier le nom de la machine (%s)\n", machine );
      exit(1) ;
    }
  memset(&name, 0, sizeof(name)) ;
  memcpy( 
	 (char*)(&(name.sin_addr.s_addr)) ,
	 mmserv->h_addr ,
	 mmserv->h_length
	 ) ;
  name.sin_family      = AF_INET;	     /* Sur le reseau                */
  name.sin_port = htons(port) ;

  err = connect(s,
		(struct sockaddr*)&name,
		sizeof(name)
		) ;

  if ( err<0 )
    {
      perror(__FILE__) ;
      fprintf(stderr, "La connexion ne se fait pas\n" );
      fprintf(stderr, "Le serveur ne doit pas marcher\n" );
      fprintf(stderr, "Machine = %s Port = %d\n", machine, port) ;
      exit(1) ;
    }
  return(s) ;
}



int main(int argc, char **argv)
{
  int s ;
  char buffer[1024] ;			     /* Buffer de lecture */
  int nb ;				     /* Nombre de caracteres lues */

  s = ouvre_connexion( argv[1], atoi(argv[2]) ) ;
  nb = write(s, argv[3], strlen(argv[3])) ;
  while( (nb = read(s,buffer,sizeof(buffer))) > 0 )
    {
      fwrite(buffer, nb, 1, stdout) ;
    }
  return(0) ;
}
