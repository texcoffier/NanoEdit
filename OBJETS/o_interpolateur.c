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
#include <GL/gl.h>		/* Car il affiche du GL */
#include "o_objet.h"
#include "t_geometrie.h"
#include "u_graphique.h"
#include "u_vue.h"

/*

Les temps clefs sont en relatif

------------|-----------------|-----------------|---------------
      temps_depart      +temps_clef[0]    +temps_clef[1]

*/

/*
 * Interpolateur doit être le premier champs de l'objet
 */

typedef struct interpolateur
{
  Flottant temps_de_depart ;
  Table_flottant temps_clefs ;	/* Les N-1 moments clefs de l'interpolation */
  Flottant temps ;		/* Le temps courant */
  Menu chemin ;
  Booleen utilise_poids ;
} Interpolateur ;

/*
 * Retourne !=0 si erreur.
 *
 * Retourne dans ses paramètres :
 *   - valeur entre 0 et 1
 *   - index entre 0 et nb_point, si 0 ou nb_point, valeur n'a pas d'importance
 */

static int evalu_interpolateur(Interpolateur *o, Flottant *valeur, int *index)
{
  int i ;
  Flottant t, temps, temps_total ;

  /* Les moments clefs doivent être dans un ordre croissant */

  for(i = 0 ; i < o->temps_clefs.nb; i++)
    if ( o->temps_clefs.table[i] <= 0 )
      {
	NOTE_CHAMP_INVALIDE(o, temps_clefs) ;
	break ;
      }

  /* Si une erreur ou rien à calculer on sort */

  if ( UN_CHAMP_INVALIDE(o) )
    return(1) ;
  if ( 0 && !UN_CHAMP_CHANGE(o) ) /* Sinon l'objet swtich marche pas */
    return(1) ;

  /* Calcule la longueur du trajet pour pouvoir faire boucle et pingpong */

  temps_total = 0 ;
  for(i=0; i < o->temps_clefs.nb; i++)
    temps_total +=  o->temps_clefs.table[i] ;

  if ( TRACE(o) )
    eprintf("temps=%g temps depart=%g\n", o->temps, o->temps_de_depart) ;

  temps = o->temps - o->temps_de_depart ;


  if ( temps > 0 )
    {
      switch( o->chemin.index )
	{
	case 0: /* unique */
	  break ;
	case 1: /* boucle */
	  temps = fmod(temps, temps_total) ;
	  break ;
	case 2: /* pingpong */
	  temps = fmod(temps, temps_total) ;
	  
	  if ( (int)(temps / temps_total) & 1 )
	    temps = temps_total - temps ;
	  break ;
	}
    }
  else
    {
      *index = 0 ;
      return 0 ;
    }

  /* Recherche de l'interval dans lequel on est */

  i = 0 ;
  t = 0 ;
  while ( i < o->temps_clefs.nb  &&  temps >= t )
    {
      t += o->temps_clefs.table[i] ;
      i++ ;
    }

  if ( TRACE(o) )
    eprintf("t=%g temps=%g i=%d\n", t, temps, i) ;

  /* On est entre i-1 et i */

  if ( i == 0 )
    *index = 0 ;
  else
    if ( temps > t )
      *index = o->temps_clefs.nb + 1 ;
    else
      {
	*valeur = (temps - t + o->temps_clefs.table[i-1])
	  / o->temps_clefs.table[i-1]
	  ;
	*index = i ;
      }

  return(0) ;
}

CLASSE(interpolateur, Interpolateur,
       BULLE_D_AIDE
       ("Interpolation linéaire par morceaux")
       CHAMP(temps_de_depart, L_nombre P_flottant Extrait Edite Sauve
	     DEFAUT("0")
	     BULLE_D_AIDE("C'est le temps ou commence le changement.\n"
			  "Les autres temps sont en relatif\n"
			  "par rapport à celui-ci.\n"
			  )
	     )
       CHAMP(utilise_poids, L_booleen Edite Sauve
	     BULLE_D_AIDE("Si vrai alors on prend le temps sur la quatrième\n"
			  "coordonnée des quadruplets\n"))
       CHAMP(temps_clefs, L_table_flottant Extrait Edite Sauve
	     DEFAUT("1 1 1 1")
	     BULLE_D_AIDE("La durée passée dans entre chaque moment clef.\n"
			  "On interpole linéairement entre chaque moment.\n"
			  )
	     )
       CHAMP(temps, L_nombre P_flottant Extrait Edite Sauve DEFAUT("0"))
       CHAMP(chemin,
	     L_option_menu
	     TE_TEXTE(
		      "unique\n"
		      "boucle\n"
		      "pingpong\n"
		      )
	     DEFAUT("unique")
	     Sauve Edite InitChaine
	     BULLE_D_AIDE
	     ("Chemin parcouru par l'interpolation.\n"
	      )
	     )
       MENU("NON")
       )
/*
*****************************************************************************
 */

typedef struct interpolateur_quadruplet
{
  Interpolateur ip ;
  Table_quadruplet table ;	/* Les N coordonnees de point */
  Quadruplet resultat ;		/* Le résultat de l'interpolation */
} Interpolateur_quadruplet ;

static void changement_interpolateur_quadruplet(Interpolateur_quadruplet *o)
{
  int i, old_nb ;
  Flottant temps ;

  /* Le nombre de moments clefs doit être égal aux nombre de points et >0 */

  if ( o->table.nb == 0 )
    {
      NOTE_CHAMP_INVALIDE(o, table) ;
      return ;
    }
  if ( o->table.nb - 1 != o->ip.temps_clefs.nb || o->ip.utilise_poids )
    {
      old_nb = o->ip.temps_clefs.nb ;
      CHANGE_TAILLE(o, ip.temps_clefs,  o->table.nb - 1) ;
      if ( o->ip.utilise_poids )
	{
	  for(i=0; i<o->ip.temps_clefs.nb; i++)
	    o->ip.temps_clefs.table[i] = o->table.table[i].h ;
	}
      else
	{
	  for(i=old_nb; i<o->ip.temps_clefs.nb; i++)
	    if ( old_nb )
	      o->ip.temps_clefs.table[i] = o->ip.temps_clefs.table[old_nb-1] ;
	    else
	      o->ip.temps_clefs.table[i] = 1 ;
	}
    }

  if ( evalu_interpolateur(&o->ip, &temps, &i) )
    return ;

  if ( i == 0 )
    o->resultat = o->table.table[0] ;
  else
    if ( i == o->table.nb )
      o->resultat = o->table.table[o->table.nb - 1] ;
    else
      {
	o->resultat = u_barycentre_2_quadruplet(&o->table.table[i-1]
						, temps
						, &o->table.table[i]
						) ;
      }
}

static Booleen harmonise_vitesse(const Interface_widget *iw)
{
  Interpolateur_quadruplet *o = iw->objet ;
  int i ;
  float longueur ;

  CHANGE_TAILLE(o, ip.temps_clefs,  o->table.nb - 1) ;
  longueur = 0 ;
  for(i=0;i<o->table.nb-1;i++)
    longueur += u_distance_triplet((Triplet*)&o->table.table[i],
				   (Triplet*)&o->table.table[i+1]) ;

  for(i=0;i<o->table.nb-1;i++)
    {
      o->ip.temps_clefs.table[i] = u_distance_triplet
	((Triplet*)&o->table.table[i], (Triplet*)&o->table.table[i+1])
	/ longueur;
    }

  return(Faux) ;
}





CLASSE(interpolateur_quadruplet, Interpolateur_quadruplet,
       SUR_CLASSE("interpolateur")
       CHAMP(table, L_table P_table_quadruplet Extrait Edite Sauve
	     DEFAUT("0 0 0\n1 0 0\n1 1 0\n0 1 0\n0 0 0\n")
	     )
       CHAMP(resultat, L_quadruplet Affiche Init3d)
       CHANGEMENT(changement_interpolateur_quadruplet)
       CHAMP_VIRTUEL(LABEL("Harmonise vitesse") L_bouton(harmonise_vitesse))
       EVENEMENT("IQ")
       MENU("Figures/0d/Interpolateur Quadruplet")
       )



/*
*****************************************************************************
 */

typedef struct interpolateur_table_quadruplet
{
  Interpolateur ip ;
  Grille_quadruplet table ;	/* Les N coordonnees de point */
  Table_quadruplet resultat ;		/* Le résultat de l'interpolation */
} Interpolateur_table_quadruplet ;

static void changement_interpolateur_table_quadruplet(Interpolateur_table_quadruplet *o)
{
  int i, j, old_nb ;
  Flottant temps ;

  /* Le nombre de moments clefs doit être égal aux nombre de points et >0 */

  if ( o->table.nb_lignes == 0 )
    {
      NOTE_CHAMP_INVALIDE(o, table) ;
      return ;
    }
  if ( o->table.nb_lignes - 1 != o->ip.temps_clefs.nb)
    {
      old_nb = o->ip.temps_clefs.nb ;
      CHANGE_TAILLE(o, ip.temps_clefs,  o->table.nb_lignes - 1) ;
      for(i=old_nb; i<o->ip.temps_clefs.nb; i++)
	if ( old_nb )
	  o->ip.temps_clefs.table[i] = o->ip.temps_clefs.table[old_nb-1] ;
	else
	  o->ip.temps_clefs.table[i] = 1 ;
    }

  if ( evalu_interpolateur(&o->ip, &temps, &i) )
    return ;


  CHANGE_TAILLE(o, resultat, o->table.nb_colonnes) ;

  for(j=0; j<o->table.nb_colonnes; j++)
    {
      if ( i == 0 )
	o->resultat.table[j] = o->table.grille[0][j] ;
      else
	if ( i == o->table.nb_lignes )
	  o->resultat.table[j] = o->table.grille[o->table.nb_lignes - 1][j] ;
	else
	  {
	    o->resultat.table[j]
	      = u_barycentre_2_quadruplet(&o->table.grille[i-1][j]
					  , temps
					  , &o->table.grille[i][j]
					  ) ;
	  }
    }
}




CLASSE(interpolateur_table_quadruplet, Interpolateur_table_quadruplet,
       SUR_CLASSE("interpolateur")
       CHAMP(table, L_grille P_grille_quadruplet Extrait Edite Sauve
	     DEFAUT("0 0 0\n1 0 0\n1 1 0\n0 1 0\n0 0 0\n")
	     )
       CHAMP(resultat, L_table_point P_table_quadruplet Affiche Init3d)
       CHANGEMENT(changement_interpolateur_table_quadruplet)
       EVENEMENT("ITQ")
       MENU("Figures/0d/Interpolateur Table Quadruplet")
       )


/*
*****************************************************************************
 */

typedef struct
{
  Interpolateur ip ;
  void *fils_choisi ;
} Switch ;


static void changement_switch(Switch *o)
{
  int i ;
  Flottant temps ;

  if ( evalu_interpolateur(&o->ip, &temps, &i) )
    {
      o->fils_choisi = NULL ;
      return ;
    }
  o->fils_choisi = FILS(o, i+2) ;
}

CLASSE(switch, Switch,
       SUR_CLASSE("interpolateur")
       BULLE_D_AIDE("Choix du fils en fonction d'une valeur.\n"
		    "Ceci peut-être utilisé par des objets à afficher\n"
		    "ou bien des attributs GL.\n"
		    )
       CHAMP(fils_choisi, L_prog)
       CHANGEMENT(changement_switch)
       EVENEMENT("SW")
       MENU("Divers/Switch")
       PAQUET(NB_ACTIFS)
       )



