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
#include <ctype.h>
#include <GL/gl.h>
#include "t_gl.h"
#include "u_graphique.h"		/* t_chaine */
#include "t_geometrie.h"		/* P_triplet */

struct flottant
{
  Flottant valeur ;
} ;

CLASSE(flottant, struct flottant,
       MENU("Valeurs/Flottant")
       EVENEMENT("0f0l")
       BULLE_D_AIDE("Objet nombre flottant")
       CHAMP(valeur,
	     Edite Sauve L_nombre P_flottant Init3d Extrait
	     BULLE_D_AIDE("Valeur du nombre flottant."
			  "Modifiable interactivement en activant l'objet\n"
			  "et en déplacement le curseur sur l'axe des X\n"
			  "d'une vue 2D tout en appuyant sur '1'\n"
			  )
	     )
       )

struct valeur_bornee
{
  Triplet valeur ;
} ;

CLASSE(valeur_bornee, struct valeur_bornee,
       MENU("Valeurs/Valeur Bornée")
       EVENEMENT("0v0b")
       BULLE_D_AIDE("Valeur bornée modifiable par un ascenseur")
       CHAMP(valeur , L_flottant_borne Edite Sauve Extrait
	     LABEL("Valeur bornée")             
	     BULLE_D_AIDE("Le triplet représente :\n"
			  "(valeur courante, valeur min, valeur max)\n"
			  "La valeur peut atteindre les bornes.\n"
			  )
	     )   
      )


struct entier
{
  int valeur ;
} ;

CLASSE(entier, struct entier,
       MENU("Valeurs/Entier")
       EVENEMENT("0e0n")
       BULLE_D_AIDE("Objet nombre entier")
       CHAMP(valeur,
	     Edite Sauve L_entier Init3d Extrait TYPE_DANS_FILS(L_nombre)
	     BULLE_D_AIDE("Valeur du nombre entier."
			  "Modifiable interactivement en activant l'objet\n"
			  "et en déplacement le curseur sur l'axe des X\n"
			  "d'une vue 2D tout en appuyant sur '1'\n"
			  )
	     )
       )

struct booleen
{
  Booleen valeur ;
} ;

CLASSE(booleen, struct booleen,
       MENU("Valeurs/Booléenne")
       EVENEMENT("0b0o")
       BULLE_D_AIDE("Objet booleen")
       CHAMP(valeur,
	     Edite Sauve L_booleen Init3d Extrait TYPE_DANS_FILS(L_nombre)
	     BULLE_D_AIDE("Valeur booléenne.\n")
	     )
       )

/*
 *
 */

struct texte
{
  char *valeur ;
  Flottant hauteur ;
} ;

static void affiche_texte(struct texte *o)
{
  glRasterPos3f(0.,0.,0.) ;
  t_chaine( o->valeur ) ;
}

static void ecrit_pov_texte(struct texte *o, const Champ *c, const char *v)
{
  FILE *f ;
  int i ;
  char buf[99] ;
  char *p ;

  if ( v[0]=='\0' ) return ;
  f = (FILE*)atol(v) ;

  fprintf(f, "union {\n") ;
  for(i=0; o->valeur[i]; i++)
    {
      p = buf ;
      if ( isalpha(o->valeur[i]) || isdigit(o->valeur[i]) )
	sprintf(buf, "%c", toupper(o->valeur[i])) ;
      else
	switch(o->valeur[i])
	  {
	  case '-' :
	    p = "Dash" ;
	    break ;
	  case '(' :
	    p = "LPar" ;
	    break ;
	  case ')' :
	    p = "RPar" ;
	    break ;
	  case '#' :
	    p = "Num" ;
	    break ;
	  default:
	    continue ;
	  }
      fprintf(f, "object { char_%s\n translate <%d,0,0>\n}\n", p, i*5+2) ;
    }
  fprintf(f,
	  "scale <%g,%g,%g>\n"
	  "}\n"
	  , o->hauteur/5,  o->hauteur/5,  o->hauteur/5
	  ) ;
}


CLASSE(texte, struct texte,
       MENU("Valeurs/Texte")
       EVENEMENT("0t0e")
       BULLE_D_AIDE("Objet contenant un texte.\n"
		    "Il s'affiche à l'origine par défaut.\n")
       CHAMP(valeur, L_chaine Edite Sauve InitChaine
	     Extrait TYPE_DANS_FILS(L_valeur))

       CHAMP(hauteur, Edite Sauve L_nombre P_flottant Init3d Extrait
	     DEFAUT("0.3")
	     BULLE_D_AIDE("Hauteur du texte.\n"
			  "Utilisé seulement pour la sortie POVRAY.\n"
			  )
	     )

       CHAMP_VIRTUEL(L_affiche_gl(affiche_texte)
		     AFFICHAGE(Toujours_different))
       CHAMP_VIRTUEL(L_affiche_pov(ecrit_pov_texte))
       )
