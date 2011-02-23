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
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "u_base.h"
#include "u_dll.h"
#include "u_init_cls.h"
#include "l_liste.h"
#include "o_objet.h"
#include <locale.h>

char *directories_objets ;
void *globale_interface ; /* Pointeur sur la racine */

int ne_pas_debugger ;

int (*bug_glib)(void (*function)(void)) = atexit ;

typedef struct
{
  char   *nom ;
  time_t  date_modification ;
  DLL     dll ;
  int     changement ;
  Liste   *definitions ;
} Obj ;

/*
 *
 */

static void u_relecture_liste_objet(Liste **objs)
{
  Obj *nouveau ;
  Booleen trouve ;
  FILE *les_objets ;
  Buffer commande ;

  sprintf(commande,
	  "ls 2>/dev/null `eval \"echo '%s '  |"
	  "sed -e 's, ,/OBJ/'$ARCH'/o_*.o ,g'\"` |"
	  "fgrep -v '*'"
	  , directories_objets
	  ) ;
  les_objets = popen(commande, "r") ;
  
  while( fscanf(les_objets, "%[^\n]\n", commande) == 1 )
    {
      trouve = Faux ;
      POUR(Obj*, o, *objs,
	   if ( strcmp(o->nom, commande)==0 )
	   {
	     trouve = Vrai ;
	     break ;
	   }
	   ) ;
      if ( !trouve )
	{
	  ALLOUER(nouveau,1) ;
	  MET_A_0(nouveau) ;
	  nouveau->nom = strdup(commande) ;
	  l_ajoute(objs, nouveau) ;
	}
    }
  pclose(les_objets);
}

/*
 *
 */

static FILE* ouvre_source(char *nom)
{
  char *n, *c, *d ;
  FILE *f ;

  n = strdup(nom) ;
  n[strlen(n)-1] = 'c' ;

  c = strstr(n, "/OBJ/") ;
  d = strstr(c+strlen("/OBJ/"), "/") ;
  do
    *c++ = *d ;
  while(*d++) ;

  f = fopen(n, "r") ;
  if ( f == NULL )
    {
      strcat(n, "pp") ;
      f = fopen(n, "r") ;
      if ( f == NULL )
	{
	  perror(n) ;
	  exit(1) ;
	}
    }
  free(n) ;
  return(f) ;
}

static int  lit_nom_classe(FILE *f, char nom[2048])
{
  strcpy(nom, "u_classe_") ;
  nom += strlen(nom) ;

  while( fgets(nom, 2048-strlen("u_classe_"), f) )
    {
      if ( strncmp(nom, "CLASSE(", strlen("CLASSE(")) == 0 )
	{
	  strcpy_overlap(nom, nom+strlen("CLASSE(")) ;
	  nom[strcspn(nom, " \t,\n")] = '\0' ;
	  return(1) ;
	}
    }
  return(0) ;
}

int **definitions_classes = NULL ;
int globale_debug ;

static void u_recharge(Liste *objs, int ***definitions_classes)
{
  int j ;
  struct stat f ;
  char nom[2048], nom2[2048] ;
  char *nc ;
  int* (*fct)() ;
  FILE *source ;
  int symbol_manquant ;
  int **dc ;

  dc = *definitions_classes ;
  REALLOUER(dc, 10*l_longueur(objs)+1 ) ;

  j = 0 ;
  POUR(Obj*, ob, objs,
       {
	 if ( stat(ob->nom, &f) )
	   {
	     perror(ob->nom) ;
	     exit(1) ;
	   }
	 if ( f.st_mtime > ob->date_modification )
	   {
	     ob->changement = 1 ;
	     ob->date_modification = f.st_mtime ;
	     if ( ob->dll )
	    {
	      u_ferme_dll(ob->dll) ;
	      ob->dll = NULL ;
	      POUR(int*, def, ob->definitions, free(def) ; ) ;
	      l_detruit(&ob->definitions) ;
	    }
	   }
	 else
	   ob->changement = 0 ;
       }) ;

  do
    {
      symbol_manquant = 0 ;
      POUR(Obj*, ob, objs,
	   {
	     if ( ob->changement )
	       {
		 if ( ob->dll == NULL )
		   {
		     eprintf("%s\n", ob->nom) ;
		     ob->dll = u_ouvre_dll(ob->nom) ;
		     if ( ob->dll == NULL )
		       {
			 ICI ;
			 /* Il faudrait enlever le symbole suivant ? */
			 /* symbol_manquant = 1 ; */
		       }
		   }
	       }
	   }) ;
    }
  while(symbol_manquant) ;
  eprintf("\n") ;
  POUR(Obj*, ob, objs,
       {
	 if ( ob->changement )
	   {
	     nc = ob->nom ;
	     while ( strchr(nc, '/') ) nc = strchr(nc, '/')+1 ;
	     eprintf( "%16s :", nc) ;
	     if ( strncmp(nc, "o_", 2)==0 )
	       {
		 nc++ ;
		 strcpy(nom, nc) ;
		 nom[strlen(nc)-1] = '\0' ;
		 
		 source = ouvre_source(ob->nom) ;

		 while( lit_nom_classe(source, nom) )
		   {
		     eprintf( " %s", nom+strlen("u_classe_")) ;
		     fflush(stderr) ;
		     fct = (int* (*)())u_prend_fonction(ob->dll, nom) ;
		     if ( fct == NULL )
		       {
			 sprintf(nom2, "%s__Fv", nom) ;
			 fct = (int* (*)())u_prend_fonction(ob->dll, nom2) ;
		       }
		     dc[j] = (*fct)() ;
		     l_ajoute(&ob->definitions, dc[j]) ;
		     j++ ;
		   }
		 fclose(source) ;
	       }
	     eprintf("\n") ;
	   }
	 else
	      POUR(int*, def, ob->definitions, dc[j++] = def ; ) ;
       }) ;

  dc[j++] = NULL ;
  *definitions_classes = dc ;
}

/*
 *
 */

int main(int argc, char **argv)
{
  int i ;
  char      *nom_modele ;
  int        x11 ;
  int        ouvre_fenetres ;
  void      *modele ;
  Liste     *objs ;
  int      **definitions_classes = NULL ;
  int        dump ;
  int        http_ecrit ;
  int        demo ;
  int        netscape ;
  int        ouvre_fenetre_modele ;
  /*
   * Analyse des options
   */
  nom_modele           = strdup("4vues") ;
  x11                  = 1               ;
  globale_debug        = 0               ;
  dump                 = 0               ;
  http_ecrit           = 0               ;
  demo                 = 0               ;
  netscape             = 0               ;
  ouvre_fenetre_modele = 1               ;
  ouvre_fenetres       = 1               ;
  setlocale(LC_NUMERIC, "C");

  for(i=1;i<argc;i++)
    {
      if (strcmp(argv[i], "-x11"       )==0) { x11           = 0; continue; }
      if (strcmp(argv[i], "-debug"     )==0) { globale_debug = 1; continue; }
      if (strcmp(argv[i], "-dump"      )==0) { dump          = 1; continue; }
      if (strcmp(argv[i], "-http_ecrit")==0) { http_ecrit    = 1; continue; }
      if (strcmp(argv[i], "-demo"      )==0) { demo          = 1; continue; }
      if (strcmp(argv[i], "-netscape"  )==0) { netscape      = 1; continue; }
      if (strcmp(argv[i], "-ouvre_fenetres")==0) {ouvre_fenetres = 0;continue;}
      if (strcmp(argv[i], "-ouvre_fenetre_modele"  )==0) { ouvre_fenetre_modele      = 0; continue; }
      nom_modele = argv[i] ;
    }
  if ( demo )
    netscape = 1 ;

  directories_objets = getenv("DIRECTORIES") ;
  if ( directories_objets == NULL )
    {
      eprintf("Tapez 'make lance' pour lancer NanoÉdit\n") ;
      exit(1) ;
    }

  objs = NULL ;
  for(i=0;;i++)
    {
      eprintf( "Relecture liste des classes\n") ;
      u_relecture_liste_objet(&objs) ;
      eprintf( "Chargements dynamiques de NanoEdit\n") ;
      u_recharge(objs, &definitions_classes) ;
      /*
       *
       */
      eprintf( "Initialisations dynamiques de NanoEdit\n") ;
      u_init_classes(definitions_classes) ;
      eprintf( "Génération de l'interface\n") ;
      if ( x11 )
	{
	  globale_interface = o_objet( char_vers_classe("xt_interface") ) ;
	  eprintf( "Interface Xt\n") ;
	}
      else
	{
	  globale_interface = o_objet( char_vers_classe("interface") ) ;
	  eprintf( "Interface HTTP\n") ;
	}
      
      modele = o_objet( char_vers_classe("modele") ) ;
      o_il_devient_mon_fils(globale_interface, modele, Faux) ;
      
      c_sauve(globale_interface, "argc"      , int_vers_char(argc)) ;
      c_sauve(globale_interface, "argv"      , int_vers_char((long)argv)) ;
      c_sauve(globale_interface, "trace"     , globale_debug ? "1" : "0") ;
      c_sauve(globale_interface, "trace2"    , globale_debug ? "1" : "0") ;
      c_sauve(globale_interface, "dump"      , dump          ? "1" : "0") ;
      c_sauve(globale_interface, "http_ecrit", http_ecrit    ? "1" : "0") ;
      c_sauve(globale_interface, "demo"      , demo          ? "1" : "0") ;
      c_sauve(globale_interface, "netscape"  , netscape      ? "1" : "0") ;
      c_sauve(globale_interface, "ouvre_fenetre_modele"
	      , ouvre_fenetre_modele         ? "1" : "0") ;
      c_sauve(modele   , "trace"     , globale_debug ? "1" : "0") ;
      c_sauve(modele   , "trace2"    , globale_debug ? "1" : "0") ;
      c_sauve(modele   , "nom_modele", nom_modele) ;
      c_sauve(modele   , "ouvre_fenetres", ouvre_fenetres ? "1" : "0") ;
      if ( i!=0 )
	{
	  c_sauve(modele   , "recharge"  , "1") ;
	}
      
      eprintf("Exécution de l'interface\n") ;
      o_appel_changement(globale_interface) ; /* PAS EVALUATION() */
      o_destruction(globale_interface) ;
      /*
       * Dump memoire pour debugage
       */
      if ( globale_debug )
	u_dump_memoire() ;
      /*
       *
       */
      if ( dump )
	break ;
    }
  return(0) ;
}
