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
#include <math.h>
#include <GL/gl.h>
#include <ctype.h>
#include "o_objet.h"            /* Car il faut pouvoir definir la classe */
#include "t_geometrie.h"
#include "u_graphique.h"
#include "u_etat_courant.h"
#include "stl.h"
#include <arpa/inet.h>
#include <assert.h>

typedef struct
{
  char *nom_fichier ;
  Table_triplet tt ;
  int nb_triangles ;
  Booleen prend_les_normales_du_fichier ;
  Menu affichage ;
} Objet_stl ;

void met_en_majuscule(char *chaine)
{
  while ( *chaine )
    {
      *chaine = toupper(*chaine) ;
      chaine++ ;
    }
}

int recupere_triplet(FILE *f, const char *mot_clef, Triplet *p)
{
  char ligne[999] ;
  char *pc ;

  do
    {
      if ( fgets(ligne, sizeof(ligne), f) == NULL )
	return(1) ;
      met_en_majuscule(ligne) ;
      pc = strstr(ligne, mot_clef) ;
    }
  while(!pc) ;

  *p = char_vers_triplet(pc + strlen(mot_clef)) ;
  return(0) ;
}

int lire_facette(FILE *f, Triplet valeurs[4], Booleen normale_du_fichier,
		 int version_ascii)
{
  Triplet normale, u, v ;
  float triangle[3*4] ; // +1 padding
  int i ;
  assert(sizeof(triangle) == 48) ;

  if ( version_ascii )
    {
      if ( recupere_triplet(f, "NORMAL", &normale) )
	return(1) ;
      for(i=0;i<3;i++)
	if ( recupere_triplet(f, "VERTEX", &valeurs[i]) )
	  return(1) ;
    }
  else
    {
      if ( fread(triangle, sizeof(triangle)+2, 1, f) <= 0 )
	return 1 ;
      for(i=0;i<3*4;i++)
	(&valeurs[0].x)[i] = triangle[i] ;
      normale = valeurs[0] ;
      valeurs[0] = valeurs[3] ;
      valeurs[3] = normale ;
    }

  u = u_soustrait_triplet(&valeurs[1], &valeurs[0]) ;
  v = u_soustrait_triplet(&valeurs[2], &valeurs[0]) ;
  valeurs[3] = u_pv_triplet(&u, &v) ;

  if ( normale_du_fichier )
    {
      if ( u_ps_triplet(&normale, &valeurs[3]) < 0 )
	{
	  /*
	   * Changer l'ordre des points de la facette
	   */
	  u = valeurs[0] ;
	  valeurs[0] = valeurs[2] ;
	  valeurs[2] = u ;
	  valeurs[3] = u_pv_triplet(&u, &v) ;
	}
    }

  valeurs[3] = u_normalise_triplet(&valeurs[3]) ;

  return(0) ;
}

static void changement_stl(Objet_stl *o)
{
  FILE *f ;
  int i, version_ascii ;
  int nb_facettes_entete ;
  struct informations_table it ;
  char entete_binaire[81] ;

  assert(sizeof(nb_facettes_entete) == 4) ;

  if ( ! CHAMP_CHANGE(o, nom_fichier) )
    return ;

  it = codec_informations_table(&infos_codec_table_triplet, &o->tt) ;
  i = 0 ;
  f = fopen(o->nom_fichier, "r") ;
  if ( f == NULL )
    {
      perror(o->nom_fichier) ;
      NOTE_CHAMP_INVALIDE(o, nom_fichier) ;
      return ;
    }
  version_ascii = 0 ;
  fread(entete_binaire, 80, 1, f) ;
  entete_binaire[80] = '\0' ;
  if ( strstr(entete_binaire, "solid") )
    {
      version_ascii = 1 ;
      rewind(f) ;
    }
  else
    {
       fread(&nb_facettes_entete, 4, 1, f) ;
       // nb_facettes_entete = ntohl(nb_facettes_entete) ;
    }

  for(;;)
    {
      if ( i >= o->tt.nb - 4 )
	{
	  codec_change_taille_table(&it, (int)(16 + o->tt.nb*1.5)) ;
	}
      if ( lire_facette(f, &o->tt.table[i], o->prend_les_normales_du_fichier,
			version_ascii) )
	break ;
      i += 4 ;
    }
  codec_change_taille_table(&it, i) ;
  o->nb_triangles = i/4 ;
  if ( ! version_ascii && o->nb_triangles != nb_facettes_entete )
    {
      NOTE_CHAMP_INVALIDE(o, nom_fichier) ;
      eprintf("Nombre de triangle pas bon dans %s : %d != %d\n",
	      o->nom_fichier,  o->nb_triangles, nb_facettes_entete) ;
    }
}  

static void detruit_stl(Objet_stl *o)
{
  CODEC_FREE(o->tt.table) ;
}

static void affiche_stl(Objet_stl *o
			, const Champ *c
			, const char*v)
{
  int i, di ;

  if ( o->affichage.index == 2 )
    di = 8 ;
  else
    di = 4 ;

  switch(globale_etat_courant.type_affichage)
    {
    case Facette:
      glEnable(GL_LIGHTING) ;
      glBegin(GL_TRIANGLES) ;
      for(i=0; i<o->tt.nb; i+=di)
	{
	  glNormal3dv((GLdouble*)&o->tt.table[i+3]) ;
	  t_triplet(&o->tt.table[i+0]) ;
	  t_triplet(&o->tt.table[i+1]) ;
	  t_triplet(&o->tt.table[i+2]) ;
	}
      glEnd() ;
      glDisable(GL_LIGHTING) ;
      break ;
    case Ponctuel:
      glBegin(GL_POINTS) ;
      for(i=0; i<o->tt.nb; i+=di)
	{
	  t_triplet(&o->tt.table[i+0]) ;
	  t_triplet(&o->tt.table[i+1]) ;
	  t_triplet(&o->tt.table[i+2]) ;
	}
      glEnd() ;
      break ;
    case Filaire:
      if ( o->affichage.index == 0 )
	di = 4 ;
      else
	di = 8 ;
      glBegin(GL_LINES) ;
      for(i=0; i<o->tt.nb; i+=di)
	{
	      t_triplet(&o->tt.table[i+0]) ;
	      t_triplet(&o->tt.table[i+1]) ;

	      t_triplet(&o->tt.table[i+1]) ;
	      t_triplet(&o->tt.table[i+2]) ;

	      t_triplet(&o->tt.table[i+2]) ;
	      t_triplet(&o->tt.table[i+0]) ;
	}
      glEnd() ;
      /* Copier/coller */
      if ( o->affichage.index == 1 )
	{
	  glEnable(GL_LINE_STIPPLE) ;
	  glLineStipple(1,0x1111) ;

	  glBegin(GL_LINES) ;
	  for(i=4; i<o->tt.nb; i+=8)
	    {
	      t_triplet(&o->tt.table[i+0]) ;
	      t_triplet(&o->tt.table[i+1]) ;
	      
	      t_triplet(&o->tt.table[i+1]) ;
	      t_triplet(&o->tt.table[i+2]) ;

	      t_triplet(&o->tt.table[i+2]) ;
	      t_triplet(&o->tt.table[i+0]) ;
	    }
	  glEnd() ;

	  glDisable(GL_LINE_STIPPLE) ;
	}
      break ;
    case Invisible:
      break ;
    case AfficheDebug:
      break ;
    }
}

static void ecrit_stl_pov( Objet_stl *o, const Champ *c,
                                     const char *v)
{ 
  int i,di;
  FILE *f ;  

  if ( v[0]=='\0' ) return; 
  f = (FILE*)atol(v) ;

  if ( o->affichage.index == 2 )
    di = 8 ;
  else
    di = 4 ;

 for(i=0; i<o->tt.nb; i+=di)
	{
          fprintf(f,
		"triangle {<%g,%g,%g>,<%g,%g,%g>,<%g,%g,%g> }\n",
		o->tt.table[i+0].x,o->tt.table[i+0].y,o->tt.table[i+0].z,
               
                o->tt.table[i+1].x,o->tt.table[i+1].y,o->tt.table[i+1].z,
                
		  o->tt.table[i+2].x,o->tt.table[i+2].y,o->tt.table[i+2].z);
		
	}
     
  
}

static void ecrit_stl_stl(Objet_stl *o, const Champ *c, const char *v)
{
  FILE *f ;
  int i, di ;
  
  f = (FILE*)atol(v) ;
  if ( o->affichage.index == 2 )
    di = 8 ;
  else
    di = 4 ;

   for(i=0; i<o->tt.nb; i+=di)
       t_triangle_stl(f, &o->tt.table[i],&o->tt.table[i+1],&o->tt.table[i+2]);
}		     



CLASSE(objet_stl, Objet_stl,
       CHAMP(nom_fichier, L_chaine Edite Sauve InitChaine DEFAUT("pas_de_nom.stl"))
       CHAMP(nb_triangles, L_nombre P_entier Affiche)
       CHAMP(prend_les_normales_du_fichier, L_booleen Edite Sauve DEFAUT("1"))
       CHAMP(affichage,
	     L_option_menu
	     TE_TEXTE(
		      "Affiche toutes les facettes\n"
		      "Affiche les facettes impaires en pointillés\n"
		      "Affiche seulement les facettes paires\n"
		      )
	     DEFAUT("Affiche toutes les facettes")
	     Sauve Edite
	     BULLE_D_AIDE
	     ("Ceci est utile si le fichier STL contient UNIQUEMENT\n"
	      "des objets avec offset pour creuser l'intérieur.\n"
	      "La première facette (paire) est à l'extérieur et\n"
	      "la deuxième (impaire) est son offset à l'intérieur.\n"
	      )
	     )
       CHANGEMENT(changement_stl)
       DESTRUCTION(detruit_stl)
       CHAMP_VIRTUEL(L_affiche_gl(affiche_stl)
		     AFFICHAGE(Different_suivant_type_affichage)
		     )
       CHAMP_VIRTUEL(L_affiche_pov(ecrit_stl_pov)) 
       CHAMP_VIRTUEL(L_affiche_stl(ecrit_stl_stl))
       MENU("Afficheur fichier STL")
       EVENEMENT("ASTL")
       )


