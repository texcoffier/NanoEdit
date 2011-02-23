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
#include <GL/gl.h>
#include "o_objet.h"
#include "u_etat.h"
#include "u_applique.h"
#include "u_etat_courant.h"
#include "u_vue.h"
#include "t_geometrie.h"

/*
 * Variable locale pour memorise les etats courants
 */

struct applique
{
  Booleen applique_toujours_visible ;
  int attributs_gl_a_sauver ;
  char *commentaire ;
} ;


static void affiche_applique(struct applique*o, const Champ *ch, const char *v)
{
  char *pile ;
  Booleen modif ;
  const Champ *c ;

  Gl_Affiche_Erreur(o, NULL, "Début applique") ;

  if ( TRACE(o) )
    eprintf("DEBUT Affiche_Applique %p\n", o) ;

  glPushAttrib( o->attributs_gl_a_sauver & ~NANOEDIT_MATRIX_BIT ) ;
  if ( o->attributs_gl_a_sauver & NANOEDIT_MATRIX_BIT )
    glPushMatrix() ;
  pile = u_sauve_etat() ;

  POUR_LES_FILS
    (o, f,
     {
       if ( TRACE(o) )
	 eprintf("Applique[%d] affiche %p:%s\n", i_f, f,
		 OBJET(f)->classe->nom) ;
       if ( c_trouve_type((const void **)&f, &c, &infos_L_applique) )
	 {
	   /*
	    * C'est un attribut
	    */
	   if ( o->applique_toujours_visible || ACTIF(f) )
	     u_affiche_objet(f, v) ;
	   /*
	    * Cette affectation est là pour que le debuggage affiche
	    * "TST" comme valeur stockée dans le champ.
	    * Ceci n'est pas propre, il faudrait plutôt faire
	    * c_recupere_type mais c'est un peu bizard que le récupère
	    * fasse une action alors que c'est plutôt le "sauve"
	    * qui l'a fait d'habitude....
	    */
	   modif = 0 ; /* (('T'*8 + 'S')*8 + 'T') * 8 ; */
	   if ( TRACE(o) )
	     eprintf("Applique[%d] modif=%p\n", i_f, &modif) ;
	   c_sauve_type(f, &infos_L_applique, (char*)&modif) ;
	   if ( TRACE(o) )
	     eprintf("Applique[%d] modif=%d\n", i_f, modif) ;
	   if ( modif )
	     {
	       c_sauve_type(f, &infos_L_applique_gl, v) ;
	       Gl_Affiche_Erreur(f, NULL, "Dans le applique_gl") ;
	     }
	 }
       else
	 u_affiche_objet(f, v) ;
     }
     ) ;
  u_restaure_etat(pile) ;
  if ( o->attributs_gl_a_sauver & NANOEDIT_MATRIX_BIT )
    glPopMatrix() ;
  glPopAttrib() ;

  if ( TRACE(o) )
    eprintf("FIN Affiche_Applique %p\n", o) ;
}

static void ecrit_pov_applique(void *o, const Champ *c, const char *v)
{
  if ( v[0]=='\0' ) return ;
  pov_fait_union((FILE*)atol(v), OBJET(o)->fils, v) ;
}

static void ecrit_stl_applique(void *o, const Champ *c, const char *v)
{
  if ( TRACE(o) )
    eprintf("Sortie STL dans (%s)\n", v) ;
  if ( v[0]=='\0' ) return ;
  stl_fait_union((FILE*)atol(v), OBJET(o)->fils, v) ;
}

static void ecrit_obj_applique(void *o, const Champ *c, const char *v)
{
  if ( TRACE(o) )
    eprintf("Sortie POV dans (%s)\n", v) ;
  if ( v[0]=='\0' ) return ;
  obj_fait_union((FILE*)atol(v), OBJET(o)->fils, v) ;
}

static void changement_applique(struct applique *o)
{
  const char *v ;

  o->attributs_gl_a_sauver = 0 ;
  POUR_LES_FILS
    (o, f,
     {
       if ( c_type_existe(f, &infos_L_applique) )
	 {
	   v = c_recupere_type_sans_copie(f, &infos_L_applique_gl) ;
	   if ( v[0] == '\0' )
	     {
	       o->attributs_gl_a_sauver = GL_ALL_ATTRIB_BITS
		 | NANOEDIT_MATRIX_BIT ;
	       return ;
	     }
	   o->attributs_gl_a_sauver |= atol(v) ;
	 }
     }
     ) ;
}

/*
 *
 */

CLASSE(applique, struct applique,
       CHAMP(commentaire, L_chaine Edite Sauve InitChaine
	     BULLE_D_AIDE("Commentaire pour nommer le applique")
	     )
       CHAMP(applique_toujours_visible,
	     L_booleen Edite Sauve DEFAUT("0")
	     BULLE_D_AIDE("D'habitude seules les transformations actives\n"
			  "s'affichent graphiquement.\n"
			  "\n"
			  "Si cet indicateur est vrai, elle s'afficheront\n"
			  "toujours qu'elles soient actives ou non.\n"
			  )
	     )
       PAQUET(NB_ACTIFS)
       CHAMP_VIRTUEL(L_affiche_gl(affiche_applique)
		     AFFICHAGE(Toujours_different))
       CHAMP_VIRTUEL(L_affiche_pov(ecrit_pov_applique))
       CHAMP_VIRTUEL(L_affiche_stl(ecrit_stl_applique))
       CHAMP_VIRTUEL(L_affiche_obj(ecrit_obj_applique))
       CHAMP_VIRTUEL(L_solide)
       CHANGEMENT(changement_applique)
       MENU("Attributs/Applique")
       EVENEMENT("0a0p")
       )

