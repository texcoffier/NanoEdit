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
#include "u_vue.h"
#include "u_etat_courant.h"

typedef struct
{
  char *nom_fichier ;
  int qualite_vue ;
  Booleen couleur ;
} Stl ;

static void genere2(Stl *o)
{
  FILE *f ;
  NomFichier file ;
  
  f = fopen(o->nom_fichier, "w") ;
  if ( f == NULL )
    {
      perror(o->nom_fichier) ;
      return ;
    }
  if ( TRACE(o) )
    eprintf("Génère fichier STL : (%s)\n", o->nom_fichier) ;

  sprintf(file, "%ld %d",(long)f, o->qualite_vue) ;

  fprintf(f, "solid %s\n",
	  (char*)c_recupere_sans_copie(u_modele(o),"nom_modele")) ;
  globale_etat_courant.stl_couleur = o->couleur ;
  POUR_LES_ELEMENTS(fils_a_afficher(o), f,
		    c_sauve_type(f, &infos_L_affiche_stl, file) ;
		    ) ;
  fprintf(f, "endsolid\n") ;

  if ( fclose(f) )
    perror(o->nom_fichier) ;
}

static Booleen genere(const Interface_widget *iw)
{
  genere2((Stl*)iw->objet) ;

  return(Faux) ;
}



CLASSE(stl, Stl,
       CHAMP(nom_fichier, L_chaine Edite Sauve InitChaine DEFAUT("pas_de_nom.stl"))
       CHAMP(qualite_vue, L_nombre P_entier Edite Sauve DEFAUT("5"))
       CHAMP(couleur, L_booleen Edite Sauve DEFAUT("0")
	     BULLE_D_AIDE("Activer pour générer un fichier STL couleur")
	     )

       CHAMP_VIRTUEL(LABEL("Génère fichier STL") L_bouton(genere))
       CHAMP_VIRTUEL(NOM("genere") L_fonction(genere2))

       PAQUET(NB_ACTIFS)
       EVENEMENT("STL") ;
       MENU("Divers/Stl")
       )



