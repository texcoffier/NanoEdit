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
#include "u_etat_courant.h"
#include "u_vue.h"
#include "u_graphique.h"
#include "u_xt_interface.h"
#include "u_repartiteur.h"
#include "u_modele.h"
#include "s_string.h"

/*
 ******************************************************************************
 *
 ******************************************************************************
 */

struct active_groupe
{
  int inutile ;
} ;

static void changement_active_groupe(struct active_groupe *o)
{
  char *actif ;

  if ( FILS(o, 0) == NULL )
    return ;

  actif = ACTIF( FILS(o, 0) ) ? "1" : "0" ;

  POUR_LES_FILS(o, f,
		if ( i_f == 0 ) continue ;
		c_sauve(f, "actif", actif) ;
		) ;
		
		
}


/*
 * L_point3 est un raccourci pour P_triplet L_point
 */
CLASSE(active_groupe, struct active_groupe,
       EVENEMENT("AG")
       MENU("Divers/Active groupe")
       PAQUET(NB_ACTIFS)
       CHANGEMENT(changement_active_groupe)
       BULLE_D_AIDE("Cet objet force tous ces fils à être actif ou inactif\n"
		    "en même temps que le premier.\n"
		    "Ils sont donc tous actif ou inactif et le seul moyen\n"
		    "de changer leur état à tous en même temps est\n"
		    "d'activer ou inactiver le premier fils.\n"
		    )
       )

/*
 ******************************************************************************
 *
 ******************************************************************************
 */
struct commande
{
  char *nom_champ ;
  char *valeur ;
} ;

void* fils_1(struct commande *o)
{
  void *f ;
  const Champ *ch ;

  f = FILS(o,1) ;
  if ( f )
    {
      ch = c_trouve(f, o->nom_champ) ;
      if ( ch == NULL )
	{
	  ch = c_trouve(f, "nom_variable") ;
	  if ( ch )
	    f = Variable_contenu(f, ch) ;
	}
    }
  return f ;
}

static void affiche_commande(struct commande *o, const Champ *c, const char *v)
{
  GLint mode ;
  Booleen sauve ;

  if ( FILS(o,0) )
    {
      if ( TRACE(o) )
	eprintf("Début affiche commande\n") ;
      glGetIntegerv(GL_RENDER_MODE, &mode) ;
      if ( mode == GL_SELECT )
        {
          sauve = globale_etat_courant.designable ;
          globale_etat_courant.designable = Faux ;
          /*
           * Remplace le haut de pile nom car cela peut être un opérateur
           */
          glLoadName(OBJECT_ID(o)) ;
          glPushName(DEBUT_TAG_DEPLACABLE) ;
          u_affiche_objet(FILS(o,0),v) ;
          glPopName() ;
          globale_etat_courant.designable = sauve ;
        }
      else
	{
	  u_affiche_objet(FILS(o,0),v) ;
	}
      if ( TRACE(o) )
	eprintf("Fin affiche commande\n") ;
    }
}


static void o_lance_te(void *iw2)
{
  Interface_widget *iw = iw2 ;

  (*iw->ch->te->entree)(iw) ;
  free((char*)iw->texte) ;
  free(iw) ;
}

static void lance_une_fois(struct commande *o, void *f1)
{
  const Champ *ch ;
  Interface_widget *iw ;

  if ( TRACE(o) )
    eprintf("Lance commande %p.%s := %s sur %s\n", o, o->nom_champ, o->valeur,
	    o_informations(f1)) ;

  if ( TRACE(o) )
    o_dump(f1) ;
  c_sauve(f1, o->nom_champ, o->valeur) ;
  ch = c_trouve(f1, o->nom_champ) ;
  if ( ch )
    { 
      if ( ch->te->entree )
	{
	  ALLOUER(iw,1) ;
	  iw->objet = f1 ;
	  iw->ch = ch ;
	  iw->texte = strdup(o->valeur) ;
	  u_ajoute_action_unique(PRIORITE_OUVRIR_FENETRE, o_lance_te, iw) ;
	}
    }
  else
    {
      if ( TRACE(o) )
	eprintf("Ne trouve pas le champ dans l'objet\n") ;
    }
}

// Compliqué car on prend le 4ème argument puis les suivants.
static void lance_commande(struct commande *o,const Champ *c, const char *v)
{
  void *f1 ;
  int i ;

  if ( TRACE(o) )
    eprintf("Lance commande %p.%s := %s\n", o, o->nom_champ, o->valeur) ;

  f1 = fils_1(o) ;
  if ( f1 == NULL )
    POUR_LES_ELEMENTS(contenu_modele(u_modele(o)),f2,lance_une_fois(o, f2);)
  else
    lance_une_fois(o, f1) ;

  for (i=4 ; ; i++ )
    {
      f1 = FILS(o,i++) ;
      if ( f1 == NULL )
	break ;
      lance_une_fois(o, f1) ;
    }
}

static const char* rien(struct commande *o,const Champ *c)
{
  return("Cliquer sur le tag pour exécuter la commande") ;
}

static const char *information_commande(struct commande *o)
{
  static Chaine s ;

  s_efface(&s) ;
  s_printf(&s,"%s := %s", o->nom_champ, o->valeur) ;

  return( s_char(&s) ) ;
}


CLASSE(commande, struct commande,
       BULLE_D_AIDE("Cet objet permet de sauver une valeur dans un champ\n"
                    "quand il est cliqué.\n"
                    "Les arguments sont :\n"
                    "  - Son apparence sur l'écran.\n"
                    "  - L'objet dont on veut modifier le champ.\n"
                    "  - Le nom du champ\n"
                    "  - La valeur du champ\n"
                    "  - D'autres objets dont on veut modifier le même champ\n"
                    )
       CHAMP_VIRTUEL( Obligatoire TYPE_DANS_FILS(L_void))
       CHAMP_VIRTUEL( Obligatoire TYPE_DANS_FILS(L_void))
       CHAMP(nom_champ, L_chaine Edite Sauve Extrait) 
       CHAMP(valeur, L_chaine Edite Sauve Extrait TYPE_DANS_FILS(L_void))
       CHAMP_VIRTUEL(L_affiche_gl(affiche_commande)
                     AFFICHAGE(Toujours_different))
       CHAMP_VIRTUEL(L_tag(lance_commande,rien))
       INFORMATIONS(information_commande)
       MENU("Divers/Commande")
       EVENEMENT("CM")
       )


/*
 ******************************************************************************
 * Pas très propre....
 ******************************************************************************
 */

struct bulle
{
  const char *aide ;
} ;

static void affiche_bulle(struct bulle *o, const Champ *ch, const char *v)
{
  POUR_LES_FILS(o, f, u_affiche_objet(f, v) ; ) ; 
}
static void ecrit_pov_bulle(void *o, const Champ *c, const char *v)
{
  fprintf( (FILE*)atol(v), "union {\n") ;
  POUR_LES_FILS(o,f, c_sauve_type(f, &infos_L_affiche_pov, v) ; ) ;
  fprintf( (FILE*)atol(v), "}\n") ;
}

static void ecrit_stl_bulle(void *o, const Champ *c, const char *v)
{
  POUR_LES_FILS(o,f, c_sauve_type(f, &infos_L_affiche_stl, v) ; ) ;
}

static void ecrit_obj_bulle(void *o, const Champ *c, const char *v)
{
  POUR_LES_FILS(o,f, c_sauve_type(f, &infos_L_affiche_obj, v) ; ) ;
}

static const char* information_bulle(struct bulle *o)
{
  static Chaine s ;	/* STATICOK : car retournée */
  const char *pc ;

  pc = strchr(o->aide, '\n') ;
  if ( pc == NULL )
    return(o->aide) ;
  pc = o->aide ;
  s_efface(&s) ;
  while( *pc != '\n' )
    s_putchar(&s, *pc++) ;
  s_strcat(&s, "...") ;
  return(s_char(&s)) ;
}
/*
static void applique_bulle(void *o, const Champ *ch, Booleen *modif)
{
  if ( FILS(o, 0) )
    c_sauve_type(FILS(o, 0), &infos_L_applique, modif) ;
}

static void applique_gl_bulle(void *o, const Champ *ch, const char *urgent)
{
  if ( FILS(o, 0) )
    c_sauve_type(FILS(o, 0), &infos_L_applique_gl, urgent) ;
}

static const char * attributs_gl_bulle(void *o, const Champ *ch)
{
  if ( FILS(o, 0) )
    return( c_recupere_type_sans_copie(FILS(o, 0), &infos_L_applique_gl) ) ;
  else
    return("0") ;
}
*/


CLASSE(bulle_d_aide, struct bulle,
       BULLE_D_AIDE("Cet objet permet d'associer une bulle d'aide\n"
		    "à ses fils dans la fenêtre graphique\n"
		    )
       CHAMP(aide, L_chaine P_chaine_sans_echappement E_long_texte Edite Sauve DEFAUT("Entrer le texte de la bulle d'aide") InitChaine)
       MENU("Divers/Bulle d'aide")
       EVENEMENT("BA")
       PAQUET(NB_ACTIFS)
       CHAMP_VIRTUEL(L_affiche_gl(affiche_bulle)
		     AFFICHAGE(Toujours_different))
       CHAMP_VIRTUEL(L_affiche_pov(ecrit_pov_bulle))
       CHAMP_VIRTUEL(L_affiche_stl(ecrit_stl_bulle))
       CHAMP_VIRTUEL(L_affiche_obj(ecrit_obj_bulle))
       INFORMATIONS(information_bulle)
       /*
       CHAMP_VIRTUEL(L_applique(applique_bulle))
       CHAMP_VIRTUEL(L_applique_gl_rapide(applique_gl_bulle, attributs_gl_bulle))
       */
       )

/*
 ******************************************************************************
 *
 ******************************************************************************
 */

struct vrai_si_actif
{
  Booleen b ;
} ;

static void changement_vrai_si_actif(struct vrai_si_actif *o)
{
  o->b = !!ACTIF(o) ;
}


CLASSE(vrai_si_actif, struct vrai_si_actif,
       BULLE_D_AIDE("Valeur vrai si actif\n")
       CHAMP(b, L_booleen Affiche)
       MENU("Divers/Vrai Si Actif")
       EVENEMENT("VSA")
       CHANGEMENT(changement_vrai_si_actif)
       )


/*
 ******************************************************************************
 *
 ******************************************************************************
 */

struct prog_if
{
  void *fils_choisi ;
  Booleen v ;
} ;

static void changement_if(struct prog_if *o)
{
  o->fils_choisi = FILS(o, o->v ? 1 : 2 ) ;
}


CLASSE(prog_if, struct prog_if,
       SUR_CLASSE("prog")
       BULLE_D_AIDE("Affiche le deuxième fils si vrai\n"
		    "Le troisième si faux\n"
		    )
       CHAMP(v, L_booleen Edite Sauve Extrait DEFAUT("Critère de décision"))
       CHAMP(fils_choisi, L_prog)
       MENU("Divers/Programme IF")
       EVENEMENT("PIF")
       PAQUET(3)
       CHANGEMENT(changement_if)
       )


