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
#include "t_geometrie.h"

enum { UnPoint, UnVecteur, UnFlottant, UnDernier } ;

typedef struct
{
  Menu type ;
  Triplet point ;
  Flottant flottant ;
} Polymorphe ;

/*
 * Tout d'abord on définit un type logique qui va dire
 * a NanoÉdit si l'objet est bien de la classe demandée.
 * Ce type logique s'applique SEULEMENT a un type d'objet.
 * Il n'est pas réutilisable (sauf avec des sous-classes).
 */


/*
 * Cette fonction demande à l'objet si il contient un champs
 * de type "t", elle peut modifier l'objet ou le champs dans
 * le cas ou elle redirige vers un autre objets
 * (c'est seulement utile pour variable, operateur, if, )
 */
static Booleen suis_je(const void **oo
		       , const Champ **c, const Type_logique *tl)
{
  struct
  {
    Type_logique *type_logique ; /* Type logique de l'objet */
    const char *nom_champ ;      /* Nom du champs ou on trouve la valeur */
  } t[UnDernier] = 
    {
      {  &infos_L_point, "point" },
      {  &infos_L_vecteur, "point" },
      {  &infos_L_nombre, "flottant" },
   } ;
  const Polymorphe *o ;

  o = *oo ;

  if ( ! tl_inclus( t[o->type.index].type_logique, tl ) )
    return Faux ;

  *c = c_trouve(o, t[o->type.index].nom_champ) ;

  return *c != NULL ;
}

/*
 * Définition du type logique
 */

DEFINITION_TYPE_LOGIQUE(L_mon_polymorphe, L_metamorphe,
			"Ce champ pioche dans un autre en fonction "
			"du type stocké"
			,
			SOUS_CLASSE(suis_je)
			)

#define L_mon_polymorphe TL(L_mon_polymorphe)

/*
 * Définition de l'objet.
 * Pour tester, il suffit de charger
 *         EXEMPLES/MODELES/EXEMPLES/mon_polymorphe.ne
 */

/* Noté le NonModifiable, on ne veut pas que les parents
 * s'amusent à modifier ces valeurs si elles sont extraites.
 * Malheureusement, je viens de m'appercevoir qu'ils peuvent
 * quand même, ya un bug....
 */

CLASSE(mon_polymorphe_a_moi, Polymorphe,
       CHAMP_VIRTUEL(L_mon_polymorphe NonModifiable)
       CHAMP(type, L_option_menu Edite Sauve DEFAUT("Point")
	     LABEL("Type du résultat")
             TE_TEXTE(
                      "Point\n"
                      "Vecteur\n"
		      "Flottant\n"
		      )
	     )
       CHAMP(point, P_triplet DEFAUT("1 2 3") NonModifiable)
       CHAMP(flottant, P_flottant DEFAUT("3.14199999") NonModifiable)
       )

/*
 * Points faibles de cet exemple :
 * - La valeur n'est pas affichée dans l'objet.
 *   Pour ce faire, ou on affiche tous les champs et on en marque invalide
 *   ou on affiche un champ texte libre.
 * - On ne peut être de plusieurs type en même temps, c'est facile
 *   à modifier.
 */
