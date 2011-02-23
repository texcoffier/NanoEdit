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
#include "u_xt_interface.h"
#include "o_objet.h"
#include "u_repartiteur.h"

static Booleen annule_ferme(const Interface_widget *iw)
{
  fermer_fenetre(iw->objet) ;
  return(0) ;
}

static Booleen applique_ferme(const Interface_widget *iw)
{
  applique_fenetre(iw->objet) ;
  fermer_fenetre(iw->objet) ;
  return(0) ;
}

Booleen applique(const Interface_widget *iw)
{
  applique_fenetre(iw->objet) ;
  return(0) ;
}
/*
static Booleen annule(const Interface_widget *iw)
{
  OBJET(iw->objet)->mettre_a_jour_widget = Vrai ;
  return(0) ;
}
*/
CLASSE(objet, Objet,
       Edite Sauve
       MENU("NON")
       BULLE_D_AIDE
       ("Ceci est le nom de la classe de\n"
	"l'objet que vous éditez.")
       DEBUT_BOITE(VERTICALE)
       DEBUT_BOITE(HORIZONTALE)
       CHAMP_VIRTUEL(
	     NOM("applique_ferme")
	     BULLE_D_AIDE
	     ("Les valeurs contenues dans la fenetre sont validées.\n"
	      "Elles sont recopiées dans la structure C.\n"
	      "Après quoi la fenêtre est fermée")
	     LABEL("Applique & Ferme")
	     L_bouton(applique_ferme)
	     )
       /* On le laisse a cause de o_xt_interface : setupcloseback */
       CHAMP_VIRTUEL(
	     BULLE_D_AIDE
	     ("La fenêtre est fermée sans que rien\n"
	      "ne soit validé")
	     NOM(CHAMP_FERME)
	     LABEL("Annule & Ferme")
	     L_bouton(annule_ferme)
	     NonEdite
	     )
       /*
       CHAMP(change,
	     BULLE_D_AIDE
	     ("Les valeurs contenues dans la fenetre sont annulées.\n"
	      "Les valeurs courantes de la structure C sont\n"
	      "recopiées dans la fenêtre")
	     NOM("annule") 
	     LABEL("Annule")
	     L_bouton(annule)
	     )
       */
       CHAMP_VIRTUEL(
	     NOM("ok")
	     BULLE_D_AIDE
	     ("Les valeurs contenues dans la fenêtre sont validées.\n"
	      "Elles sont recopiées dans la structure C.")
	     LABEL("Applique")
	     L_bouton(applique)
	     )
       FIN_BOITE
       DEBUT_BOITE(HORIZONTALE)
       CHAMP(classe,
	     LABEL("Classe")
	     L_classe Affiche NonEdite NonInitialiser
	     )
       CHAMP(trace,
	     BULLE_D_AIDE
	     ("Si cette bascule est activée alors\n"
	      "TRACE(o) est vrai.")
	     LABEL("T")
	     L_booleen
	     )
       CHAMP(trace2,
	     LABEL("T2")
	     BULLE_D_AIDE
	     ("Si cette bascule est activée alors\n"
	      "TRACE2(o) est vrai, c'est une trace avec des détails.")
	     L_booleen
	     )
       CHAMP(visible,
	     LABEL("V")
	     BULLE_D_AIDE
	     ("Si cette bascule est activée alors\n"
	      "les parents du noeuds l'affichent")
	     L_booleen
	     )
       CHAMP(actif,
	     LABEL("Actif")
	     BULLE_D_AIDE
	     ("Si elle est activée, l'objet fait partie des objets actifs.\n"
	      "Il sera utilisé pour créer de nouveaux objets ou\n"
	      "bien subira les actions faites sur l'ensemble des\n"
	      "objet actifs : édition, destruction, ...")
	     L_booleen P_actif
	     )
       FIN_BOITE
       CHAMP(nom  , L_nombre P_adresse_objet  NonEdite)
       CHAMP(edite, L_long             NonEdite DEFAUT("0") )
       CHAMP(fils , L_liste_objet       NonEdite)
       CHAMP(ferme, L_booleen NonEdite Sauve DEFAUT("0"))
       )
