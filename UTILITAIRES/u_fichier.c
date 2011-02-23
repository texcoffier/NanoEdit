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
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "l_liste.h"
#include "s_string.h"

#if 0
#define EPRINTF eprintf
#else
#define EPRINTF if (0) eprintf
#endif


static Liste *directory = NULL ;

void u_empile_directory(const char* dir)
{
  EPRINTF("empile %s\n", dir) ;
  l_ajoute(&directory, strdup(dir)) ;
}

const char *u_directory()
{
  EPRINTF("accede %s\n",l_element(directory,0) ? l_element(directory,0) : "" ) ;
  return( l_element(directory,0) ? l_element(directory,0) : "." ) ;
}

void u_depile_directory()
{
  char *v ;
  v = l_enleve_tete(&directory) ;
  EPRINTF("depile %s\n", v==NULL ? "":v) ;
  if ( v )
    free(v) ;
}

char *my_basename(char *s)
{
  while( strchr(s, '/') )
    s = strchr(s, '/')+1 ;
  return(s) ;
}


/*
 * Trouve un fichier dans une liste de directory, avec prefixe et suffixe
 */
const char *u_trouve_fichier(const char *liste,
			     const char *nom,
			     const char *prefixe,
			     const char *suffixe)
{ 
  static NomFichier f ;
  static Chaine liste_des_noms ;
  static int date_mise_a_jour = 0 ;
  const char *debut, *fin ;
  FILE *fi ;

  EPRINTF("u_trouve_fichier nom=(%s) prefixe=(%s) suffixe=(%s) liste=(%s)\n"
	  , nom, prefixe, suffixe, liste) ;

  if ( nom[0] == '\0' )
    {
      EPRINTF("Nom de fichier vide !\n") ;
      return(NULL) ;
    }
  for(debut=nom; *debut; debut++)
    {
      if ( !isgraph(*debut) || *debut == '`' || *debut == '\'' )
	{
	  EPRINTF("Nom fichier invalide : %s\n", nom) ;
	  return(NULL) ;
	}
    }
  /* Test bêtement le nom passé */
      fi = fopen(nom, "r") ;	/* Copier/colle.... */
      if ( fi )
	{
	  EPRINTF("Ça marche !\n") ;
	  fclose(fi) ;
	  return(nom) ;
	}

  if ( strncmp(nom, prefixe+1, strlen(prefixe+1)) == 0 )
    nom += strlen(prefixe+1) ;
  /*
   * Ouvre le fichier directement si absolu
   */
  if ( nom[0]=='/' )
    {
      sprintf(f, "%s%s", nom, suffixe) ;
      EPRINTF("Test Ouverture directe de (%s)\n", f) ;
      fi = fopen(f, "r") ;	/* Copier/colle.... */
      if ( fi )
	{
	  EPRINTF("Ça marche !\n") ;
	  fclose(fi) ;
	  return(f) ;
	}
      EPRINTF("Introuvable\n") ;
      return(NULL) ;
    }
  /*
   * Relatif au répertoire courant
   */
  if ( nom[0]!='/' )
    {
      sprintf(f, "%s/%s%s", u_directory(), nom, suffixe) ;
      EPRINTF("Test Ouverture dans le repertoire courant de (%s)\n", f) ;
      fi = fopen(f, "r") ;	/* Copier/colle.... */
      if ( fi )
	{
	  EPRINTF("Ça marche !\n") ;
	  fclose(fi) ;
	  return(f) ;
	}
    }
  /*
   * Génère la liste des fichiers
   */
  if ( time(0) - date_mise_a_jour > 600 )
    {
      sprintf(f, "find `echo \"%s \" | sed 's,[ |$],%s ,g'` -print 2>/dev/null",
	      liste, prefixe
	      ) ;
      fi = popen(f, "r") ;
      s_efface(&liste_des_noms) ;
      for(;;)
	{
	  debut = u_ligne(fi) ;
	  if ( debut )
	    {
	      s_strcat(&liste_des_noms, debut) ;
	      s_putchar(&liste_des_noms, '\n') ;
	    }
	  else
	    break ;
	}
      pclose(fi) ;
      EPRINTF("[[[%s]]]\n", s_char(&liste_des_noms)) ;
      EPRINTF("LISTE=(%s) prefixe=(%s)\n", liste, prefixe) ;
      date_mise_a_jour = time(0) ;
    }
  /*
   *
   */
  sprintf(f, "/%s%s\n", nom, suffixe) ;

  debut = strstr(s_char(&liste_des_noms), f) ;
  if ( debut )
    {
      fin = strchr(debut, '\n') ;
      while( debut > s_char(&liste_des_noms) && *debut != '\n' )
	debut-- ;

      if ( *debut == '\n' )
	debut++ ;
      strncpy(f, debut, fin-debut) ;
      f[fin-debut] = '\0' ;

      EPRINTF("RETOURNE (%s)\n", f) ;

      return(f) ;
    }


  return(NULL) ;
}
