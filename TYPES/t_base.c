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
/* #include <string.h> */
#include "o_objet.h"

const char *int_vers_char(long f)
{
  static Nombre buf ;
  sprintf( buf, "%ld", f ) ;
  return( buf ) ;
}

const char *flottant_vers_char(Flottant f)
{
  static Nombre buf ;
  sprintf( buf, "%g", f ) ;
  return( buf ) ;
}

const char *ptr_flottant_vers_char(Flottant *f)
{
  static Nombre buf ;
  sprintf( buf, "%g", *f ) ;
  return( buf ) ;
}

Triplet char_vers_triplet(const char* t)
{
  Triplet p ;
  u_annule_triplet(&p) ;
  sscanf( t, FF3, &p.x, &p.y, &p.z) ;
  return( p ) ;
}

Quadruplet char_vers_quadruplet(const char* t)
{
  Quadruplet p ;
  u_annule_quadruplet(&p) ;
  sscanf( t, FF4, &p.x, &p.y, &p.z, &p.h) ;
  return( p ) ;
}

const char *triplet_vers_char(const Triplet* p)
{
  static Nombre buf ;
  sprintf( buf, "%g %g %g", p->x, p->y, p->z ) ;
  return( buf ) ;
}

const char *quadruplet_vers_char(const Quadruplet* p)
{
  static Nombre buf ;
  sprintf( buf, "%g %g %g %g", p->x, p->y, p->z, p->h ) ;
  return( buf ) ;
}



static Booleen nombre_positif(const void *o, const Champ *c)
{
  return(    ( c->tp==&infos_codec_entier     && T(int,o,c)>=0 )
	  || ( c->tp==&infos_codec_flottant   && T(Flottant,o,c)>=0 )
	  ) ;
}

static Booleen nombre_entier(const void *o, const Champ *c)
{
  return( c->tp == &infos_codec_entier || c->tp == &infos_codec_long ) ;
}

static Booleen nombre_flottant(const void *o, const Champ *c)
{
  return( c->tp == &infos_codec_flottant ) ;
}

static Booleen valeur_dans_les_bornes(const void *o, const Champ *c)
{
  Triplet *t ;

  if ( c->tp != &infos_codec_triplet && c->tp != &infos_codec_flottant_borne )
    return(Faux) ;
  t = &T(Triplet,o,c) ;
  return( t->x >= t->y  &&  t->x <= t->z ) ;
}

/*
 *
 */
TYPE_DE_BASE(codec_pointeur_extrait, int,
	     codec_int_recupere_texte,
	     codec_int_sauve_texte,
	     codec_rien,
	     codec_rien
	     )
/*
 * Type physique retournant le champ "te_texte" du champ
 */
int codec_champ_texte_recupere_texte()
{
  putstr( CODEC_CHAMP->te_texte ) ;
  return(0) ;
}

TYPE_DE_BASE_VOID(codec_champ_texte,
                  codec_champ_texte_recupere_texte,
                  codec_rien,
                  codec_rien,
                  codec_rien
                 )

/*
 ******************************************************************************
 * Le champ actif des objets est très particulier.
 * ACTIF : Quand on écrit une valeur, on prend celle
 * du numéro actif le plus grand +1 dans le modèle et on l'incrémente.
 ******************************************************************************
 */
int codec_actif_sauve_texte(int *a)
{
  void *modele ;
  int numero_actif, v_int ;

  codec_int_sauve_texte(&v_int) ;

  if ( *a==0 && v_int==0 )
    return(0) ;
  if ( *a!=0 && v_int!=0 )
    return(0) ;
  if ( v_int )
    {
      modele = FILS(u_racine(CODEC_OBJET),0) ;
      if ( modele )
	numero_actif = atol(c_recupere_sans_copie(modele,CHAMP_NUMERO_ACTIF)) ;
      else
	numero_actif = 0 ;
      if ( numero_actif )
	{
	  v_int = numero_actif++ ;
	  c_sauve(modele,CHAMP_NUMERO_ACTIF, int_vers_char(numero_actif)) ;
	}
    }
  *a = v_int ;
  return(0) ;
}
TYPE_DE_BASE(codec_actif, int,
	     codec_int_recupere_texte,
	     codec_actif_sauve_texte,
	     codec_rien,
	     codec_rien
	     )
/*
 *
 */
int codec_adresse_objet_recupere_texte(const void **a)
{
 if ( OBJET(CODEC_OBJET)->parent )
   {
     putstr(int_vers_char((long)CODEC_OBJET)) ;
   }
 else
   {
     PUTCHAR('0') ;
   }
 return(0) ;
}
TYPE_DE_BASE(codec_adresse_objet, void*,
	     codec_adresse_objet_recupere_texte,
	     codec_long_sauve_texte,
	     codec_rien,
	     codec_rien
	     )
/*
 ******************************************************************************
 * VOID
 ******************************************************************************
 */
int codec_void_recupere_texte()
{
  static int n=-1 ;
  codec_int_recupere_texte(&n) ;
  n-- ;/* Jamais la même valeur */
  return(0) ;
}
TYPE_DE_BASE_VOID(codec_void,
		  codec_void_recupere_texte,
		  codec_rien,
		  codec_rien,
		  codec_rien
		  )
/*
 ******************************************************************************
 * CLASSE
 ******************************************************************************
 */
const Classe* char_vers_classe(const char *nom)
{
  POUR_LES_CLASSES(classe,
		   {
		     if ( strcmp( classe->nom, nom ) == 0 )
		       return(classe) ;
		   }
		   ) ;
  return(NULL) ;
}

int codec_classe_sauve_texte(Classe **a)
{
  char *nom ;

  codec_char_etoile_sauve_texte(&nom) ;
  *a = (Classe*)char_vers_classe(nom) ;
  codec_free(&nom) ;
  return ( *a == NULL ) ;
}

int codec_classe_recupere_texte(const Classe **a)
{
  /* On enlève le const : problème du double pointeur */
  codec_char_etoile_recupere_texte((const char**)&(*a)->nom) ;
  return(0) ;
}

TYPE_DE_BASE(codec_classe, Classe*,
	     codec_classe_recupere_texte,
	     codec_classe_sauve_texte,
	     codec_rien,
	     codec_rien
	     )
/*
 ******************************************************************************
 * MODELE NanoÉdit pour le Widget Graphe
 * Pour chaque objet : adresse(liste fils)
 ******************************************************************************
 */
int codec_modele_sauve_texte(Liste_Referenced **a)
{
  return(1) ;
}

int codec_modele_recupere_texte_(const Liste **a)
{
  POUR(void*, f, *a,
       codec_unsigned_long_recupere_texte((unsigned long*)&f) ;
       globale_traduction->sortie-- ;
       if ( f )
       {
	 PUTCHAR('(') ;
	 if ( OBJET(f)->flag2 == 0 )
	   {
	     OBJET(f)->flag2 = 1 ;
	     codec_modele_recupere_texte_((const Liste **)&OBJET(f)->fils) ;
	   }
	 else
	   PUTCHAR('*') ;
	 PUTCHAR(')') ;
       }
       else
       {
	 PUTCHAR(' ') ;
       }
       ) ;
       
  return(0) ;
}

int codec_modele_recupere_texte(const Liste_Referenced **a)
{
  /*
  if ( CONTENU(a) == NULL )
    {
      if ( !globale_debug )
	EXIT ;
      return(0) ;
    }
  */
  if ( *a == NULL )
    {
      eprintf("ici\n") ;
      return 0 ;
    }

  POUR(void*, o, (*a)->contenu, if ( o ) o_annule_flag2(o) ; ) ;
  codec_modele_recupere_texte_((const Liste **)& (*a)->contenu) ;
      
  return(0) ;
}

TYPE_DE_BASE(codec_modele, Liste_Referenced*,
	     codec_modele_recupere_texte,
	     codec_modele_sauve_texte,
	     codec_rien,
	     codec_rien
	     )
/*
 ******************************************************************************
 * MENUS
 ******************************************************************************
 */

int codec_menu_libere(Menu *m)
{
  CODEC_FREE(m->selection) ;
  if ( m->description )
    free(m->description) ;
  /*
   * Pas propre
   * Devrait-être dans l'éditeur de Widget
   */
  POUR(struct fenetre_menu*, fm, m->menus,
       widget_detruit(fm->widget) ;
       free(fm->chemin) ;
       free(fm) ;
       ) ;
  l_detruit(&m->menus) ;
  return(0);
}

int codec_menu_sauve_texte(Menu *m)
{
  int i ;
  const char *pc, *pc2 ;

  codec_char_etoile_sauve_texte(&m->selection) ;

  pc = NULL ;
  if ( m->description )
    if ( m->description[0] )
      pc = m->description ;

  if ( pc == NULL )
    pc = CODEC_CHAMP->te_texte ;
  if ( pc == NULL )
    return(0) ;

  m->index = -1 ;
  if ( TRACE2(CODEC_OBJET) )
    eprintf("Menu_ecrit : selection=(%s) description=(%s)\n"
	, m->selection
	, pc
	    ) ;
  for(i=0;;i++)
    {
      if ( strncmp(m->selection,
		   pc,
		   strlen(m->selection))==0
	   )
	{
	  if ( TRACE2(CODEC_OBJET) )
	    eprintf("Menu_ecrit : i=%d\n", i) ;
	  m->index = i ;
	  /*
	  if ( CODEC_CHAMP->tp_ecrit )
	    (*CODEC_CHAMP->tp_ecrit)(CODEC_OBJET, CODEC_CHAMP, m->selection) ;
	  */
	  return(0) ;
	}
      pc2 = strchr(pc, '\001') ;
      pc = strchr(pc+1, '\n') ;
      if ( pc2 )
	{
	  if ( pc==NULL || pc2<pc )
	    {
	      if ( strncmp(m->selection,
			   pc2+1,
			   strlen(m->selection))==0
		   )
		{
		  if ( TRACE2(CODEC_OBJET) )
		    eprintf("Menu_ecrit2 : i=%d\n", i) ;
		  m->index = i ;
		  return(0) ;
		}
	    }
	}
      if ( pc == NULL )
	return(1) ;
      pc++ ;
      if ( *pc == '\0' )
	return(1) ;
    }
  return(0) ;
}

TYPE_DE_BASE(codec_commande_menu, Menu,
	     codec_rien,
	     codec_menu_sauve_texte,
	     codec_menu_libere,
	     codec_rien
	     )

TYPE_DE_BASE(codec_option_menu, Menu,
	     codec_char_etoile_recupere_texte,
	     codec_menu_sauve_texte,
	     codec_menu_libere,
	     codec_rien
	     )

/*
 ******************************************************************************
 * Redirection
 ******************************************************************************
 */

int codec_virtuel_sauve_texte()
{
  if ( CODEC_CHAMP->tp_ecrit )
    {
      (*CODEC_CHAMP->tp_ecrit)(CODEC_OBJET, CODEC_CHAMP,
			       globale_traduction->entree) ;
      /*   NOTE_UN_CHAMP_CHANGE(CODEC_OBJET) ; */
    }
  return(0) ;
}

int codec_virtuel_recupere_texte()
{
  const char *s ;

  if ( CODEC_CHAMP->tp_lit )
    {
      s = (*CODEC_CHAMP->tp_lit)(CODEC_OBJET, CODEC_CHAMP) ;
      codec_char_etoile_recupere_texte_sans_echappement(&s) ;
    }
  return(0) ;
}

TYPE_DE_BASE_VOID(codec_virtuel,
		  codec_virtuel_recupere_texte,
		  codec_virtuel_sauve_texte,
		  codec_rien,
		  codec_rien
		  )


/*
 ******************************************************************************
 * EDITEUR INEXISTANT
 ******************************************************************************
 */

static NanoWidget Editepas_creer(const Interface_widget* iw)
{
  if ( TRACE2(iw->objet) ) eprintf("Edite_pas CREER\n") ;
  return(NULL) ;
}
static const char *Editepas_lit(const Interface_widget* iw)
{
  if ( TRACE2(iw->objet) ) eprintf("Edite_pas LIT redirection\n") ;
  if ( *iw->ch->te_lit )
    return((*iw->ch->te_lit)(iw)) ; 
  return("") ;
}
void Editepas_ecrit(const Interface_widget* iw)
{
  if ( TRACE2(iw->objet) ) eprintf("Edite_pas ECRIT redirection\n") ;
  if ( *iw->ch->te_ecrit )
    (*iw->ch->te_ecrit)(iw) ; 
}
static Booleen Editepas_entree(const Interface_widget* iw)
{
  if ( TRACE2(iw->objet) )
    eprintf("(%s) Edite_pas ENTREE redirection\n", iw->ch->nom_champ) ;
  if ( *iw->ch->te_entree )
    return((*iw->ch->te_entree)(iw)) ; 
  return(0) ;
}
static void Editepas_mouvement(const Interface_widget* iw)
{
  if ( TRACE2(iw->objet) ) eprintf("Edite_pas MOUVEMENT redirection\n") ;
  if ( *iw->ch->te_mouvement )
    (*iw->ch->te_mouvement)(iw) ; 
}
static const char * Editepas_aide(const Interface_widget* iw)
{
  if ( TRACE2(iw->objet) ) eprintf("Edite_pas AIDE redirection\n") ;
  if ( iw->ch->te_aide )
    return( (*iw->ch->te_aide)(iw) ) ;
  return(u_aide_champ(iw->objet, iw->ch)) ;
}
static void Editepas_clavier(const Interface_widget* iw)
{
  if ( TRACE2(iw->objet) ) eprintf("Edite_pas CLAVIER redirection\n") ;
  if ( *iw->ch->te_clavier )
    (*iw->ch->te_clavier)(iw) ; 
}
static void Editepas_init(const Interface_widget* iw)
{
  if ( TRACE2(iw->objet) ) eprintf("Edite_pas INIT\n") ;
}
static void Editepas_start(const Interface_widget *iw)
{
}
static void Editepas_stop(const Interface_widget *iw)
{
}

const char *Jamais_pareil_lit(const Interface_widget* iw)
{
  static int n = 0 ;
  if ( TRACE2(iw->objet) ) eprintf("Jamais pareil LIT\n") ;
  return(int_vers_char(n++)) ;
}

Type_editeur infos_E_void ;

const Type_editeur* init_E_void(Champ *c)
{
  Type_editeur *te = &infos_E_void ;

  te->initialisation = init_E_void ;
  if ( c )
    {
       c->te             = &infos_E_void        ;
      te->nom            = "editepas"           ;
      te->bulle_d_aide      = "Éditeur inexistant" ;
      
      FONCTION_EDITEUR(te->creer               ,Editepas_creer               );
      FONCTION_EDITEUR(te->start_classe        ,Editepas_start               );
      FONCTION_EDITEUR(te->stop_classe         ,Editepas_stop                );
      FONCTION_EDITEUR(te->lit                 ,Editepas_lit                 );
      FONCTION_EDITEUR(te->ecrit               ,Editepas_ecrit               );
      FONCTION_EDITEUR(te->init                ,Editepas_init                );
      FONCTION_EDITEUR(te->entree              ,Editepas_entree              );
      FONCTION_EDITEUR(te->mouvement           ,Editepas_mouvement           );
      FONCTION_EDITEUR(te->clavier             ,Editepas_clavier             );
      FONCTION_EDITEUR(te->aide                ,Editepas_aide                );
    }
  return(te) ;
}

/*
 ******************************************************************************
 * TYPE LOGIQUE
 ******************************************************************************
 */


Type_logique infos_L_void_void ;

const Type_logique* init_L_void_void(Champ *c)
{
  Type_logique *tl = &infos_L_void_void  ;
  
  tl->initialisation = init_L_void_void ;
  if ( c )
    {
      init_E_void(c) ;
      
      c->tl         = &infos_L_void_void               ;
      tl->nom       = "Void"                      ;
      tl->bulle_d_aide = "Racine des types logiques" ;
      
      c->tp            = &infos_codec_void ;
      c->defaut        = "" ;
      c->te_texte      = "" ;
      c->options      |= O_Initialiser ;
    }
  return(tl) ;
}

DEFINITION_TYPE_LOGIQUE(L_void,L_void_void,
			"Racines des type logiques ``normaux''",
			)

DEFINITION_TYPE_LOGIQUE(L_metamorphe,L_void_void,
			"Racines des type logiques métamorphique",
			)

/*
 * Passer certaines options dans editeur
 */
DEFINITION_TYPE_LOGIQUE(L_prevenir_si_modele_change,L_fonction,
			"Sert a indiquer si l'objet veut être prévenu\n"
			"de changements dans le modèle global.",
			)

DEFINITION_TYPE_LOGIQUE(L_attribut_pov,L_void,
			"Pour POV c'est un attribut",
			c->tp = &infos_codec_vide ;
			c->options = 0 ;
			)
DEFINITION_TYPE_LOGIQUE(L_geometrie,L_void,
			"Indique la géométrie de la fenêtre",
			init_E_geometrie(c) ;
			c->tp = &infos_codec_char_etoile ;
			c->options &= ~(O_Cree|O_Affiche|O_Edite) ;
			)
DEFINITION_TYPE_LOGIQUE(L_titre,L_void,
			"Indique le titre de la fenêtre",
			init_E_titre(c) ;
			c->tp = &infos_codec_char_etoile ;
			)
DEFINITION_TYPE_LOGIQUE(L_drawing,L_void,
			"Zone d'affichage indiférenciée",
			init_E_drawing(c) ;
			)
DEFINITION_TYPE_LOGIQUE(L_graphe, L_valeur,
			"Graphe quelconque",
			init_E_long_texte(c) ;
			)
DEFINITION_TYPE_LOGIQUE(L_graphe_de_points, L_graphe,
			"Graphe de points",
			)
DEFINITION_TYPE_LOGIQUE(L_graphe_de_points_vecteurs, L_graphe,
			"Graphe de point-vecteur en chaque noeud",
			)
DEFINITION_TYPE_LOGIQUE(L_graphe_modele, L_void,
			"Affichage du graphe du modèle de NanoÉdit",
			init_E_graphe_modele(c) ;
			)
     /*
      * Sous-Classe de L_void_void ou lieu de L_void
      * Du fait de la manière de gerer "CLASSE_PARAMETRE".
      *    u_eval.c
      *    o_variable.c
      *    o_objet.h
      */
DEFINITION_TYPE_LOGIQUE(L_fonction, L_void_void,
			"Fonction de champ",
			c->tp = &infos_codec_virtuel ;
			c->options &= ~(O_Cree|O_Sauve|O_Extrait|O_Label|
					O_Affiche|O_Edite|O_Initialiser) ;
			/* c->options |= O_Virtuel ; */
			/* 27/12/2000 : La ligne précédente doit rester
			 * en commentaire
			 */
			)
DEFINITION_TYPE_LOGIQUE(L_tag, L_fonction,
			"Gestion des tag virtuels",
			)
DEFINITION_TYPE_LOGIQUE(L_affiche_pov, L_fonction,
			"Méthode de traduction en POV",
			)
DEFINITION_TYPE_LOGIQUE(L_affiche_stl, L_fonction,
			"Méthode de traduction en STL",
			)
DEFINITION_TYPE_LOGIQUE(L_affiche_obj, L_fonction,
			"Méthode de traduction en OBJ",
			)
DEFINITION_TYPE_LOGIQUE(L_affiche_gl, L_fonction,
			"Méthode d'affichage GL",
			)
DEFINITION_TYPE_LOGIQUE(L_applique, L_fonction,
			"Méthode de changement d'état courant",
			)
DEFINITION_TYPE_LOGIQUE(L_applique_gl, L_fonction,
			"Méthode de changement d'état courant GL",
			)
DEFINITION_TYPE_LOGIQUE(L_raccourci, L_fonction,
			"Raccourci clavier *utilisé par o_vue",
			)
     /*
DEFINITION_TYPE_LOGIQUE(L_applique_GL, L_fonction,
			"Fonction changeant des attributs GL",
			)
DEFINITION_TYPE_LOGIQUE(L_affiche_GL, L_fonction,
			"Fonction affichant du GL",
			)
     */
DEFINITION_TYPE_LOGIQUE(L_bouton, L_void,
			"Bouton de commande dans une fenêtre",
			c->tp = &infos_codec_vide ;
			init_E_bouton(c) ;
			)
DEFINITION_TYPE_LOGIQUE(L_valeur, L_void,
			"Éditeur de texte",
			init_E_texte(c) ;
			)
DEFINITION_TYPE_LOGIQUE(L_menu, L_void,
			"Menu",
			)
DEFINITION_TYPE_LOGIQUE(L_commande_menu, L_menu,
			"Menu déclenchant des exécutions",
			init_E_commande_menu(c) ;
			c->tp = &infos_codec_commande_menu ;
			)
DEFINITION_TYPE_LOGIQUE(L_option_menu, L_menu,
			"Menu permettant de choisir un option",
			init_E_option_menu(c) ;
			c->tp = &infos_codec_option_menu ;
			)
DEFINITION_TYPE_LOGIQUE(L_classe, L_valeur,
			"Classe de NanoÉdit",
			c->tp = &infos_codec_classe ;
			)
DEFINITION_TYPE_LOGIQUE(L_liste, L_valeur,
			"Liste d'entier",
			c->tp = &infos_codec_liste_unsigned_long ;
			c->defaut = "()" ;
			)
DEFINITION_TYPE_LOGIQUE(L_liste_objet, L_liste,
			"Liste de pointeurs sur objets NanoÉdit",
			)
DEFINITION_TYPE_LOGIQUE(L_sous_actif, L_liste,
			"Liste de sous_actifs",
			)
DEFINITION_TYPE_LOGIQUE(L_chaine, L_valeur,
			"Chaine de caractères",
			c->tp = &infos_codec_char_etoile ;
			)
DEFINITION_TYPE_LOGIQUE(L_nombre, L_valeur,
			"Valeur",
			)
DEFINITION_TYPE_LOGIQUE(L_booleen, L_nombre,
			"Booleen",
			init_E_booleen(c) ;
			c->tp = codec_informations(codec_char_decimal) ;
			c->defaut = "0" ;
			)
DEFINITION_TYPE_LOGIQUE(L_entier, L_nombre,
			"Entier",
			c->tp = &infos_codec_int ;
			PREDICAT(nombre_entier)
			)
DEFINITION_TYPE_LOGIQUE(L_long, L_nombre,
			"Entier Long",
			c->tp = &infos_codec_long ;
			PREDICAT(nombre_entier)
			)
DEFINITION_TYPE_LOGIQUE(L_flottant, L_nombre,
			"Flottant",
			c->tp = &infos_codec_flottant ;
			PREDICAT(nombre_flottant)
			)
DEFINITION_TYPE_LOGIQUE(L_nombre_positif, L_nombre,
			"Nombre positif",
			PREDICAT(nombre_positif)
			)
DEFINITION_TYPE_LOGIQUE(L_solide, L_valeur,
			"Solide",
			)
DEFINITION_TYPE_LOGIQUE(L_pointeur_extrait, L_nombre,
			"Pointeur extrait",
			c->tp = &infos_codec_pointeur_extrait ;
			)
DEFINITION_TYPE_LOGIQUE(L_flottant_borne, L_nombre,
			"Nombre flottant borné",
			init_E_ascenseur(c) ;
			c->tp = &infos_codec_flottant_borne ;
			PREDICAT(valeur_dans_les_bornes)
			)
DEFINITION_TYPE_LOGIQUE(L_debut_boite_horizontale, L_fonction,
			"Début boite horizontale (disposition widget)",
			c->tp = &infos_codec_champ_texte ;
			c->te_texte = "Début Boite Horizontale" ;
			)
DEFINITION_TYPE_LOGIQUE(L_debut_boite_verticale, L_fonction,
			"Début boite verticale (disposition widget)",
			c->tp = &infos_codec_champ_texte ;
			c->te_texte = "Début Boite Verticale" ;
			)
DEFINITION_TYPE_LOGIQUE(L_fin_boite, L_fonction,
			"Fin boite (disposition widget)",
			c->tp = &infos_codec_champ_texte ;
			c->te_texte = "Fin Boite" ;
			)
DEFINITION_TYPE_LOGIQUE(L_plan, L_valeur, "Equation Plan : ax+by+cz+d=0", )

