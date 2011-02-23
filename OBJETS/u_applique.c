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
#include "o_objet.h"
#include "u_applique.h"
#include "u_etat_courant.h"
#include "u_graphique.h"
#include "pov.h"

#define mm globale_etat_courant

static void ecrit_pov(FILE *f)
{
  fprintf(f, "/* nps = %d */\n", globale_etat_courant.ne_pas_sauvegarder_les_attributs_pov) ;

  if ( globale_etat_courant.ne_pas_sauvegarder_les_attributs_pov )
    return ;

  pigment(f, mm.couleur.x, mm.couleur.y, mm.couleur.z) ;
  fprintf( f, "finish { diffuse %g phong %g reflection %g ambient rgb <%g,%g,%g> phong_size %g }\n"
	   , mm.diffuse
	   , mm.speculaire
	   , mm.reflexion
	   , mm.ambiante.x, mm.ambiante.y, mm.ambiante.z
	   , mm.n
	   ) ;
  if ( mm.refraction )
    fprintf( f, "interior { refraction %g ior %g }\n"
	     , mm.refraction, mm.indice) ;
  else
    fprintf( f, "/* interior { ior 0 } */\n") ;
}

void stl_fait_union(FILE *f, const Liste *l_fils, const char *v)
{
  char *pile ;
  Booleen modif ;
  const Champ *c ;

  pile = u_sauve_etat() ;

  POUR
    (void*, f, l_fils,
     {
       if ( f == NULL )
	 continue ;
       if ( c_trouve_type((const void **)&f, &c, &infos_L_applique) )
	 {
	   modif = 0 ; /* (('T'*8 + 'S')*8 + 'T') * 8 ; */
	   c_sauve_type(f, &infos_L_applique, (char*)&modif) ;
	 }
       else
	 c_sauve_type(f, &infos_L_affiche_stl, v) ;
     }
     ) ;
  u_restaure_etat(pile) ;
}

void obj_fait_union(FILE *f, const Liste *l_fils, const char *v)
{
  char *pile ;
  Booleen modif ;
  const Champ *c ;

  pile = u_sauve_etat() ;

  POUR
    (void*, f, l_fils,
     {
       if ( f == NULL )
	 continue ;
       if ( c_trouve_type((const void **)&f, &c, &infos_L_applique) )
	 {
	   modif = 0 ; /* (('T'*8 + 'S')*8 + 'T') * 8 ; */
	   c_sauve_type(f, &infos_L_applique, (char*)&modif) ;
	 }
       c_sauve_type(f, &infos_L_affiche_obj, v) ;
     }
     ) ;
  u_restaure_etat(pile) ;
}



void pov_fait_union(FILE *f, const Liste *l_fils, const char *v)
{
  const Liste *l_i, *l_j ;
  Liste *transfo ;
  void *pile ;
  int nb_objets ;
  Booleen modif ;
  
  if ( l_fils == NULL ) return ;

  pile = u_sauve_etat() ;

  /*
   * Crée la liste des transformations
   */
  transfo = NULL ;
  for( l_i = l_fils  ;  l_i  ;  l_i = l_i->suivant )
    if ( l_i->objet )
      {
	if ( c_type_existe(l_i->objet, &infos_L_attribut_pov) )
	  {
	    c_sauve_type(l_i->objet,&infos_L_applique, (char*)&modif) ;
	    l_ajoute(&transfo, l_i->objet) ;
	  }
	else
	  break ;
      }
  /*
   * Calcul le nombre d'objets
   */
  nb_objets = 0 ;
  for( l_j=l_i ;  l_j  ;  l_j = l_j->suivant )
    if ( l_j->objet && !c_type_existe(l_j->objet, &infos_L_attribut_pov) )
      nb_objets++ ;
    else
      break ;

  if ( nb_objets )
    {
      fprintf(f, "union {\n") ;

      if ( nb_objets!=1 || l_j != NULL )
	fprintf(f, "union {\n") ;
      /*
       * Sauve les objets
       */
      for( l_j=l_i ;  l_j  ;  l_j = l_j->suivant )
	if ( l_j->objet && !c_type_existe(l_j->objet, &infos_L_attribut_pov) )
	  c_sauve_type(l_j->objet, &infos_L_affiche_pov, v) ;
	else
	  break ;
      /*
       * Écrit les attributs POV courant sur la feuille
       */
      ecrit_pov(f) ;
      if ( nb_objets!=1 || l_j != NULL )
	fprintf( f, "}\n") ;
      /*
       * Appel recursif
       */
      if ( l_j ) /* Sauve les objets suivants */
	pov_fait_union(f,l_j,v) ;
      /*
       * Sauve les transformations mémorisées (pas les couleurs)
       */
      POUR_LES_ELEMENTS(transfo, t,
			c_sauve_type(t, &infos_L_affiche_pov, v) ; ) ;
      fprintf( f, "}\n") ;
      
    }

  l_detruit(&transfo) ;
  u_restaure_etat(pile) ;
}
