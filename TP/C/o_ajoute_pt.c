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
/*
 * Vous devez modifier la fonction "changement" de ce fichier.
 * Elle est appelee lorsque le parametre est modifie.
 * La fonction crée un nouveau tableau de points
 * à partir d'un tableau passé en paramètre.
 */
#include "o_objet.h"
#include "t_geometrie.h"

struct ajoute_pt
{
  Table_triplet resultat ;	/* Table des points en sortie */
  Table_triplet points ;	/* Table des points en entree */
} ;

/*
 * La fonction suivante va dedoubler tous les points du tableau
 * en ajoutant des points milieux.
 */
static void changement(struct ajoute_pt *o)
{
  int lit, ecrit ;
  /*
   * Reallocation du precedent tableau qui n'etait peut-etre pas
   * de taille suffisante
   */
  if ( TRACE(o) )
    eprintf("points.nb = %d\n", o->points.nb) ;

  if ( o->points.nb == 0 )
    return ;

  o->resultat.nb = 2*o->points.nb - 1 ;
  REALLOUER(o->resultat.table, o->resultat.nb) ;
  /*
   * Rentrer les points
   */
  o->resultat.table[0] = o->points.table[0] ;
  ecrit = 1 ;
  for(lit=1; lit<o->points.nb; lit++)
    {
      o->resultat.table[ecrit] =
	u_ajoute_triplet(&o->points.table[lit-1], &o->points.table[lit]) ;
      o->resultat.table[ecrit] =
	u_triplet_fois_flottant(&o->resultat.table[ecrit], 0.5) ;
      ecrit++ ;
      o->resultat.table[ecrit] = o->points.table[lit] ;
      ecrit++ ;
    }
  if ( TRACE(o) )
    eprintf("resultat.nb=%d lit=%d ecrit=%d\n", o->resultat.nb, lit, ecrit) ;
}
/*
 * Définition Classe
 */
CLASSE(ajoute_pt, struct ajoute_pt,
       CHAMP(resultat, L_table_point3 Affiche)
       CHAMP(points, L_table_point3 Edite Extrait Sauve
	     DEFAUT("0 0 0\n1 0 0\n1 1 0\n0 1 0")
	     )

       CHANGEMENT(changement)
       MENU("Squelettes/Points milieux")
       )
