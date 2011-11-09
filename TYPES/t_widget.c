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
#include "u_xt_interface.h"
#include "u_repartiteur.h"
#include "t_geometrie.h"
#include "u_fichier.h"
#include "s_string.h"
#include <ctype.h>

/*
 * Utilitaires
 */

int u_initialiser_iw(Interface_widget *iw
		     , NanoWidget w
		     , void *p
		     , NanoEvent *e)
{

  iw->w     = w ;
  iw->texte = widget_nom(w) ;
  iw->objet = xt_widget_vers_objet(w) ;
  if ( iw->objet == NULL ) return(1) ; /* Bug événement */
  if ( OBJET_OU_NULL(iw->objet) == NULL )
    {
      ICI ;
      return(1) ; /* Bug motif */
    }
  iw->ch = (Champ*)p ;

  /*
    Pas bonne idée car parfois ce n'est pas le champ...
  Booleen trouve ;
  trouve = Faux ;
  POUR_TOUS_LES_CHAMPS
    (iw->objet, ch,
     if ( iw->ch == ch )
    {
      trouve = Vrai ;
      break ;
    }
     ) ;
  if ( !trouve )
    {
      ICI ;
      return(1) ;
    }
  */
  if ( 0 ) /* Plante si p n'est pas un champ.... */
    if ( TRACE2(iw->objet) )
      eprintf("Evenement X11, ch=%s (%s) event=%p\n"
	      ,iw->ch ? iw->ch->nom_champ  : "(null)"
	      ,iw->texte
	      ,e
	      ) ;
  
  if ( e )
    {
      widget_decode_evenement(e, iw) ;
      if ( iw->configure_x == -1 )
	return(1) ;
    }
  return(0) ;
}
/*
 * CALLBACK general
 */

PROTOTYPE_CALLBACK_ACTION(u_callback_bouton, w, p)
{
  Interface_widget iw ;

  if ( u_initialiser_iw(&iw, w, p, NULL) ) return ;

  if ( TRACE(iw.objet) )
    eprintf("u_callback_bouton\n") ;
  if ( (*iw.ch->te->entree)(&iw) == 0 )
    {
      if ( TRACE(iw.objet) )
	eprintf("entree retourne 0\n") ;
      OBJET(iw.objet)->urgent = Vrai ;
      evaluation(iw.objet) ;
    }
  if ( TRACE(iw.objet) )
    eprintf("FIN u_callback_bouton\n") ;
}

void w_expose(void *o)
{
  Interface_widget *iw = o ;

  if ( OBJET_OU_NULL(iw->objet) && iw->ch->te_ecrit )
    {
      (*iw->ch->te_ecrit)(iw) ;
    }
  free(iw) ;
}

static Booleen meme_expose(void *a, void *b)
{
  Interface_widget *aa = a, *bb = b ;
  return( aa->objet == bb->objet && aa->ch == bb->ch ) ;
}

PROTOTYPE_CALLBACK(u_callback_expose, w, p)
{
  Interface_widget *iw ;

  ALLOUER(iw,1) ;
  if ( u_initialiser_iw(iw, w, p, NULL) )
    {
      free(iw) ;
    }
  else
    {
      u_ajoute_action_unique_avec_test(PRIORITE_EXPOSE, w_expose, iw, meme_expose) ;
    }
  FIN_CALLBACK ;
}

/*
 * Event Handler
 */

PROTOTYPE_CALLBACK_EVENT(u_ev_motion, w, p, event)
{
  Interface_widget iw ;

  if ( u_initialiser_iw(&iw, w, p, event) ) FIN_CALLBACK_EVENT_CONTINUE ;
  /* On met urgent avant pour que le handler puisse l'enlever */
  OBJET(iw.objet)->urgent = Vrai ;
  (*iw.ch->te->mouvement)(&iw) ;
  evaluation(iw.objet) ;
  FIN_CALLBACK_EVENT_ARRETE ;
}

PROTOTYPE_CALLBACK_EVENT(u_ev_button_release, w, c, event)
{
  Interface_widget iw ;

  if ( u_initialiser_iw(&iw, w, c, event) ) FIN_CALLBACK_EVENT_CONTINUE ;
  
  iw.b = -iw.b - 1 ;
  (*iw.ch->te->entree)(&iw);
  switch( iw.b )
    {
    case -1:
    case -2:
    case -3: /* Ajout de ce cas précis le 16/01/2003 */
      widget_enleve_event_motion(w, u_ev_motion, iw.ch) ;
      widget_enleve_event_button_release(w, u_ev_button_release, iw.ch) ;
      FIN_CALLBACK_EVENT_ARRETE ;
      break ;
    }
  FIN_CALLBACK_EVENT_CONTINUE ;
}

PROTOTYPE_CALLBACK_EVENT(u_ev_button_press, w, c, event)
{
  Interface_widget iw ;

  if ( u_initialiser_iw(&iw, w, c, event) ) FIN_CALLBACK_EVENT_CONTINUE ;
  switch( iw.b )
    {
    case 0:
    case 1:
    case 2: /* Ajout de ce cas précis le 16/01/2003 */
    case 3:
    case 4:
      if ( TRACE(iw.objet) )
	eprintf("u_input bouton 1 ou 2 ou 4 ou 5\n") ;
      if ( (*iw.ch->te->entree)(&iw) && iw.b < 3 )
	{
	  widget_add_event_button_release (w, u_ev_button_release, iw.ch) ;
	  widget_add_event_motion(w, u_ev_motion, iw.ch);
	}
      OBJET(iw.objet)->urgent = Vrai ;
      evaluation(iw.objet) ;
      FIN_CALLBACK_EVENT_ARRETE ;
      break ;
    }
  FIN_CALLBACK_EVENT_CONTINUE ;
}

PROTOTYPE_CALLBACK_EVENT(u_ev_motion_na, w, c, event)
{
  Interface_widget iw ;

  if ( u_initialiser_iw(&iw, w, c, event) ) FIN_CALLBACK_EVENT_CONTINUE ;
  if ( iw.b == -1 )
    (*iw.ch->te->mouvement)(&iw) ;
  FIN_CALLBACK_EVENT_CONTINUE ;
}

/*
 * Format pour un caractère normal : "BC X Y"
 *  B : 0 1:shift 2:ctrl 4:meta
 *  C : le caractère ou le nom de la touche
 *  X Y : Position dans la fenêtre
 * Format pour une touche spéciale : "B N X Y"
 *  N : Nom de la touche
 */
PROTOTYPE_CALLBACK_EVENT(u_ev_key_press, w, c, event)
{
  Interface_widget iw ;
  char historique[30] ;

  if ( u_initialiser_iw(&iw, w, c, event) ) FIN_CALLBACK_EVENT_CONTINUE ;

  if ( TRACE2(iw.objet) )
    eprintf("iw.touche = (%s)\n", iw.touche) ;

  historique[0] = '0' + iw.etat ;
  if ( iw.touche[0] == '\0' )
     FIN_CALLBACK_EVENT_CONTINUE ;
  historique[1] = ' ' ;
  historique[2] = '\0' ;
  if ( iw.touche[1] == '\0' )
    historique[1] = iw.touche[0] ;
  else
    strcpy(historique+2, iw.touche) ; 

  sprintf( historique+strlen(historique), " %d %d", iw.x, iw.y) ;

  iw.texte = historique ;
  (*iw.ch->te->clavier)(&iw) ;
  OBJET(iw.objet)->urgent = Vrai ;
  evaluation(iw.objet) ;

  if ( TRACE2(iw.objet) )
    eprintf("historique = %s\n", historique) ;

  widget_arrete_emission_key_press(w) ;
  FIN_CALLBACK_EVENT_ARRETE ;  
}



/*
 ******************************************************************************
 * EDITEUR TEXTE
 ******************************************************************************
 */

static NanoWidget Texte_creer(const Interface_widget *iw)
{
  return(widget_texte(iw->w, iw->ch->nom_champ, EDITE(iw->ch))) ;
}

static NanoWidget Long_texte_creer(const Interface_widget *iw)
{
  return(widget_texte_long(iw->w, iw->ch->nom_champ, EDITE(iw->ch))) ;
}


static const char *Texte_lit(const Interface_widget *iw)
{
  return(widget_texte_lit(iw->w)) ;
}

static void Texte_ecrit(const Interface_widget *iw)
{
  widget_texte_ecrit(iw->w, iw->texte) ;
}

DEFINITION_TYPE_EDITEUR(E_texte, E_void,
			"Éditeur de texte",
			{
			  FONCTION_EDITEUR(te->creer, Texte_creer) ;
			  FONCTION_EDITEUR(te->lit  , Texte_lit  ) ;
			  FONCTION_EDITEUR(te->ecrit, Texte_ecrit) ;
			}
			)

DEFINITION_TYPE_EDITEUR(E_long_texte, E_texte,
			"Éditeur de texte avec ascenseur",
			{
			  FONCTION_EDITEUR(te->creer, Long_texte_creer) ;
			}
			)

/*
 ******************************************************************************
 * BASCULE BOOLEENNE
 ******************************************************************************
 */

static NanoWidget Booleen_creer(const Interface_widget *iw)
{
  return( widget_toggle(iw->w, iw->ch->nom_champ, iw->ch->label) ) ;
}

static const char *Booleen_lit(const Interface_widget *iw)
{
  return( widget_toggle_lit(iw->w) ? "1" : "0" ) ;
}

static void Booleen_ecrit(const Interface_widget *iw)
{
  widget_toggle_ecrit(iw->w, iw->texte[0]=='0' ? 0 : 1) ;
}

DEFINITION_TYPE_EDITEUR(E_booleen, E_void,
			"Bascule booléenne",
			{
			  FONCTION_EDITEUR(te->creer, Booleen_creer) ;
			  FONCTION_EDITEUR(te->lit  , Booleen_lit  ) ;
			  FONCTION_EDITEUR(te->ecrit, Booleen_ecrit) ;
			  c->options &= ~O_Label ;
			  c->options |= O_Edite ;
			}
			)

/*
 ******************************************************************************
 * GEOMETRIE
 ******************************************************************************
 */

PROTOTYPE_CALLBACK_EVENT(u_resize_event, w, c, event)
{
  Interface_widget iw ;

  ICI;
  if ( u_initialiser_iw(&iw, w, c, event) ) FIN_CALLBACK_EVENT_CONTINUE ;
  ICI;
  FIN_CALLBACK_EVENT_CONTINUE ;
}


static NanoWidget Geometrie_creer(const Interface_widget *iw)
{
  /*
  widget_add_event_resize(OBJET(iw->objet)->edite,
			  u_resize_event, iw->ch) ;
  */
  return( widget_label(iw->w, iw->ch->nom_champ, "") ) ;
}

static const char *Geometrie_lit(const Interface_widget *iw)
{
  NanoWidget w ;
  static Chaine s ;
  int l, h, x, y ;

  w = widget_shell(iw->w) ;
  //  widget_get_size(w, &l, &h) ; Remplacé par la ligne suivante le 6/2/2004
  widget_get_current_size(w, &l, &h) ;
  widget_get_position(w, &x, &y) ;  

  s_efface(&s) ;
  s_printf(&s, "%d %d %d %d", x, y, l, h) ;
  if ( DEBUG_GEOMETRIE || TRACE(iw->objet) )
    eprintf("GEOM_LIT %s (%s)\n", s_char(&s), widget_get_title(w)) ;
  return( s_char(&s) ) ;

}

static void Geometrie_ecrit(const Interface_widget *iw)
{
  Quadruplet q ;
  NanoWidget w ;
  int x, y, l, h, l2, h2 ;

  if ( iw->texte[0] )
    {
      codec_sauve_texte(codec_quadruplet, &q, iw->texte) ;
      w = widget_shell(iw->w) ;
      widget_get_current_size(w, &l, &h) ;
      widget_get_size(w, &l2, &h2) ;
      widget_get_position(w, &x, &y) ;  
      if ( DEBUG_GEOMETRIE || TRACE2(iw->objet) )
	{
	  eprintf("GEOM_ECR Geometrie taille souhaité : (%s)\n", iw->texte) ;
	  eprintf("GEOM_ECR Position actuelle : (%d,%d)\n", x,y) ;
	  eprintf("GEOM_ECR Taille : (%dx%d) Taille_courante (%dx%d)\n", l2,h2,l,h) ;
	}

      /*
       * Ne change la position que si cela bouge de beaucoup
       * (taille de l'encadrement fenetre
      */
      if ( ABS(q.x-x)>40 || ABS(q.y-y)>40 )
	widget_positionne(w, q.x, q.y) ;
      if ( ABS(q.z-l)>1 || ABS(q.h-h)>1 )
	widget_resize(w, (int)q.z, (int)q.h) ;
    }
}

DEFINITION_TYPE_EDITEUR(E_geometrie, E_void,
			"Position et taille de la fenêtre",
			{
			  FONCTION_EDITEUR(te->creer, Geometrie_creer) ;
			  FONCTION_EDITEUR(te->lit  , Geometrie_lit  ) ;
			  FONCTION_EDITEUR(te->ecrit, Geometrie_ecrit) ;
			  c->options &= ~O_Label ;
			  c->options |=  O_Affiche ;
			}
			)

/*
 ******************************************************************************
 * TITRE
 ******************************************************************************
 */

static NanoWidget Titre_creer(const Interface_widget *iw)
{
  return( widget_label(iw->w, iw->ch->nom_champ, "") ) ;
}

static const char *Titre_lit(const Interface_widget *iw)
{
  return( widget_get_title(widget_shell(iw->w)) ) ;
}

static void Titre_ecrit(const Interface_widget *iw)
{
  widget_set_title(widget_shell(iw->w), iw->texte) ;
}

DEFINITION_TYPE_EDITEUR(E_titre, E_void,
			"Titre de la fenêtre",
			{
			  FONCTION_EDITEUR(te->creer, Titre_creer) ;
			  FONCTION_EDITEUR(te->lit  , Titre_lit  ) ;
			  FONCTION_EDITEUR(te->ecrit, Titre_ecrit) ;
			  c->options &= ~(O_Label|O_Affiche|O_Edite) ;
			  c->options |= O_Cree ;
			}
			)

/*
 ******************************************************************************
 * FONCTION
 ******************************************************************************
 */

static NanoWidget Bouton_creer(const Interface_widget *iw)
{
  NanoWidget w ;
  w = widget_button(iw->w, iw->ch->nom_champ, iw->ch->label) ;
  widget_add_event_activate(w, u_callback_bouton, iw->ch) ;
  return(w) ;
}

DEFINITION_TYPE_EDITEUR(E_bouton, E_void,
			"bouton déclenchant une exécution immédiate",
			{
			  FONCTION_EDITEUR(te->creer, Bouton_creer) ;
			  c->options &= ~(O_Label|O_Sauve|O_Initialiser) ;
			  c->options |= O_Edite ;
			}
			)

/*
 ******************************************************************************
 * MENU
 ******************************************************************************
 */
static const char *description_menu_(void *o, const Champ *c)
{
  const char *d ;

  d = NULL ;
  if ( T(Menu, o, c).description )
    if ( T(Menu, o, c).description[0] )
      d = T(Menu, o, c).description ;

  if ( d == NULL )
    d = c->te_texte ;

  return(d) ;
}



static char **description_menu(const char *debut, const char *d)
{
  const char *f, *slash ;
  char *add ;
  char **t ;
  int len ;
  int i, j, k ,nb, nb_tab ;
  int *tabulation ;
  static Chaine s ;

  len = strlen(debut) ;
  t = NULL ;
  nb = 0 ;
  while(*d)
    {
      f = strchr(d, '\n') ;
      if ( f==NULL )
	f = d + strlen(d) ;
      if ( strncmp(d, debut, len)==0 )
	{
	  slash = d+len ;
	  while( *slash!='\0' && *slash!='/' && *slash!='\001' )
	    slash++ ;
	  if ( *slash!='/' )
	    slash = NULL ;
	  if ( slash==NULL || slash > f )
	    {
	      /*
	       * C'est un feuille de menu
	       */
	      ALLOUER(add, f-(d+len)+1) ;
	      strncpy(add, d+len, f-(d+len)) ;
	      add[f-(d+len)] = '\0' ;
	    }
	  else
	    {
	      /*
	       * C'est un noeud du menu
	       */
	      ALLOUER(add, slash-d+2 ) ;
	      strncpy(add, d, slash-d+1) ;
	      add[slash-d+1] = '\0' ;
	      for(i=0;i<nb;i++)
		if ( strcmp(t[i], add) == 0 )
		  {
		    free(add) ;
		    add = NULL ;
		    break ;
		  }
	    }
	  if (add)
	    {
	      REALLOUER(t, nb+1) ;
	      t[nb] = add ;
	      nb++ ;
	    } 
	}
      if ( *f=='\0' )
	break ;
      d = f + 1 ;
    }
  REALLOUER(t, nb+1) ;
  t[nb] = NULL ; 

  /*
   * Tabulations
   */

  nb_tab = 0 ;
  for(i=0;i<nb;i++)
    {
      j = 0 ;
      for(d=t[i]; *d; d++)
	if ( *d == '\t' )
	  j++ ;
      if ( j > nb_tab )
	nb_tab = j ;
    }
  nb_tab++ ;
  ALLOUER(tabulation, nb_tab) ;
  for(i=0;i<nb_tab;i++)
    tabulation[i] = 0 ;

  for(i=0;i<nb;i++)
    {
      j = 0 ;
      k = 0 ;
      for(d=t[i]; *d; d++)
	{
	  if ( *d == '\t' )
	    {
	      if ( j > tabulation[k] )
		tabulation[k] = j ;
	      j = 0 ;
	      k++ ;
	    }
	  else
	    j++ ;
	}
      if ( j > tabulation[k] )
	tabulation[k] = j ;
    }

  for(i=0;i<nb;i++)
    {
      s_efface(&s) ;
      k = 0 ;
      for(add=t[i], f=add; *add; add++)
	{
	  if ( *add == '\t' )
	    {
	      *add = '\0' ;
	      s_printf(&s, "%-*s", tabulation[k], f) ;
	      f = add+1 ;
	      k++ ;
	    }
	}
      s_strcat(&s, f) ;

      reaffecte_chaine_avec_copie(&t[i], s_char(&s)) ;
    }
  free(tabulation) ;

     
  return(t) ;
}

static PROTOTYPE_CALLBACK_ACTION(action_bouton_menu, w, ch)
{
  struct fenetre_menu *fmr ;
  Interface_widget iw ;

  fmr = (struct fenetre_menu*) ch ;

  c_sauve_ici(fmr->objet, fmr->ch, widget_nom(w)) ;
  if ( fmr->ch->te->entree )
    {
      iw.objet = fmr->objet ;
      iw.ch = fmr->ch ;
      iw.texte = widget_nom(w) ;
      iw.objet_contextuel = fmr->objet_contextuel ;
      (*fmr->ch->te->entree)(&iw) ;
    }
}


static PROTOTYPE_CALLBACK_ACTION(u_affiche_sous_menu, w, ch) ;

struct fenetre_menu* creer_menu(NanoWidget bouton
				, void *objet
				, const Champ *ch
				, const char *nom
				, Booleen racine_menu
				, void *objet_contextuel
			       )
{
  struct fenetre_menu *fm ; 

  ALLOUER(fm,1) ;
  fm->chemin = strdup(nom) ;
  fm->index = 0 ;
  fm->ch = ch ;
  fm->objet = objet ;
  fm->objet_contextuel = objet_contextuel ;

  if ( racine_menu )
    {
      fm->widget = widget_popup_menu(widget_sommet(bouton), ch->nom_champ) ;
    }
  else
    {
      fm->widget = widget_pulldown_menu(widget_sommet(bouton)) ;
      widget_attache_sous_menu(bouton, fm->widget) ;
      widget_add_event_activate_cascade(bouton, u_affiche_sous_menu, fm) ;
    }
  l_ajoute(&T(Menu,objet,ch).menus, fm) ;
  return(fm) ;
}

void vide_menu(struct fenetre_menu *fm)
{
  Liste *f ;

  f = widget_fils(fm->widget) ;
  widget_unmanage_liste(f) ;
  POUR(NanoWidget, fils, f,
       {
	 if ( !widget_est_tearoff(fils) )
	   {
	     widget_attache_sous_menu(fils, NULL) ;
	     widget_item_detruit(fils) ;
	   }
	 else
	   {
	     free( widget_vers_objet(fils) ) ;
	   }
       }
       ) ;
  l_detruit(&f) ;
}


void remplire_menu(struct fenetre_menu *fm)
{
  char **t, *pc ;
  const char *d ;
  int i, fin, milieu ;
  Booleen actif ;
  Liste *a_manager ;
  NanoWidget w ;
  const char *nom_image ;

  /*
   * Ne fait rien si la définition du menu est la même
   */
  d = description_menu_(fm->objet, fm->ch) ;
  pc = widget_vers_objet(fm->widget) ;
  if ( pc && strcmp(pc, d) == 0 )
    return ;

  /*
   * On vide complètement et on rerempli
   */

  vide_menu(fm) ;

  pc = strdup(d) ;
  widget_marque_objet(fm->widget, pc) ;

  t = description_menu(fm->chemin, d) ;

  a_manager = NULL ;
  for(i=0;t[i];i++)
    {
     fin = strlen(t[i]) - 1 ;
      if ( t[i][fin] == '/' )
	{
	  milieu = fin - 1 ;
	  for( milieu = fin - 1 ; milieu>=0 ; milieu-- )
	    if ( t[i][milieu]=='/' )
	      break ;
	  w = widget_bouton_cascade(fm->widget, t[i], &t[i][milieu+1]) ;
	  creer_menu(w, fm->objet, fm->ch, t[i], Faux, NULL) ;
	}
      else
	{
	  if ( t[i][fin] == '@' )
	    {
	      actif = Faux ;
	      t[i][fin--] = '\0' ;
	    }
	  else
	    actif = Vrai ;
	  milieu = fin - 1 ;
	  for( milieu = fin - 1 ; milieu>=0 ; milieu-- )
	    if ( t[i][milieu]=='\001' )
	      {
		t[i][milieu] = '\0' ;
		break ;
	      }
	  /*
	   * Pas d'image si c'est un raccourci clavier
	   */
	  if ( strchr(t[i], '[') )
	    nom_image = NULL ;
	  else
	    nom_image = u_trouve_fichier(directories_objets, t[i]+milieu+1, "/MODELES/", ".ppm") ;
	  if ( nom_image )
	    {
	      Chaine avec_ppm = s_ouvre_ecriture() ;
	      s_strcat(&avec_ppm, nom_image) ;
	      if ( strcmp(nom_image+strlen(nom_image)-4, ".ppm") )
		s_strcat(&avec_ppm, ".ppm") ;
	      w = widget_button_menu_with_icon(fm->widget, t[i]+(milieu+1),
					       t[i], s_char(&avec_ppm)) ;
	      s_ferme(&avec_ppm) ;
	    }
	  else
	    {
	      w = widget_button_menu(fm->widget, t[i]+(milieu+1), t[i]) ;
	    }

	  if ( actif )
	    {
	      widget_add_event_menu_button(w, action_bouton_menu,(Champ *)fm) ;
	    }
	  else
	    {
	      widget_sensible(w, actif) ;
	    }
	}
      l_ajoute(&a_manager, w) ;
      free(t[i]) ;
    }
  free(t) ;
  widget_termine_menu(fm->widget, a_manager) ;
  widget_manage_liste(a_manager) ;
  l_detruit(&a_manager) ;
}


/*
 * On parcourt tous les menus créés en les triant en visible et invisibles
 *
 * Attention, l'ordre de destruction des menus est important.
 * Il faut détruire les dernier menu créés en premier.
 *
 * Tout ceci est très mauvais, il faut tout reprogrammer.
 * On va attendre GTK 2.0
 */

static void menage_menu(Menu *m)
{
  Liste *visibles, *invisibles ;

  visibles = NULL ;
  invisibles = NULL ;
  POUR(struct fenetre_menu *, fm, m->menus,
       if ( widget_visible(widget_parent(fm->widget)) )
         {
	   l_ajoute_fin(&visibles, fm) ;
	 }
       else
         {
	   l_ajoute_fin(&invisibles, fm) ;
	 }
       ) ;

 if ( l_longueur(visibles) == 0 )
    POUR(struct fenetre_menu *, fm, invisibles,
	 {
	   vide_menu(fm) ;
	   widget_detruit_menu(fm->widget) ;
	   l_enleve(&m->menus, fm) ;
	   free(fm->chemin) ;
	   free(fm) ;
    }
	 )
   else
    POUR(struct fenetre_menu *, fm, visibles,
	 {
	   remplire_menu(fm) ;
    }) ;
  
  l_detruit(&visibles) ;
  l_detruit(&invisibles) ;
}


static PROTOTYPE_CALLBACK_ACTION(u_affiche_sous_menu, w, ch)
{
  struct fenetre_menu *fm ;

  fm = (struct fenetre_menu*) ch ;
  widget_enleve_event_activate_cascade(w, u_affiche_sous_menu, ch) ;
  remplire_menu(fm) ;
}

void u_vrai_affiche_menu(NanoWidget w, void *objet, const Champ *ch,
			 NanoEvent *event, void *objet_contextuel)
{
  struct fenetre_menu* fm ;

  fm = creer_menu(w, objet, ch, "", Vrai, objet_contextuel) ;
  remplire_menu(fm) ;
  widget_positionne_menu(fm->widget, event) ;
  widget_manage_freres(&fm->widget, 1) ;
}


static PROTOTYPE_CALLBACK_EVENT(u_affiche_menu, w, ch, event)
{
  Interface_widget iw ;

  if ( u_initialiser_iw(&iw, w, ch, event) ) FIN_CALLBACK_EVENT_CONTINUE ;

  if ( iw.b != 0 )
    FIN_CALLBACK_EVENT_CONTINUE ;

  u_vrai_affiche_menu(w, iw.objet, iw.ch, event, NULL) ;

  FIN_CALLBACK_EVENT_ARRETE ;
}

/*
 *
 */


static void commande_menu_ecrit(const Interface_widget* iw)
{
  menage_menu(&T(Menu, iw->objet, iw->ch)) ;
}

static NanoWidget Menu_creer(const Interface_widget* iw)
{
  NanoWidget w ;
  
  w = widget_button_for_menu(iw->w, iw->ch->nom_champ, iw->ch->label) ;
  widget_add_event_button_press(w, u_affiche_menu, iw->ch) ;

  return(w) ;
}

static Booleen commande_menu_entree(const Interface_widget *iw)
{
   Interface_widget iw2 ;
  int i ;

  iw2 = *iw ;

  c_sauve_ici(iw->objet, iw->ch, iw2.texte) ;
  if ( iw->ch->te_entree )
    {
      i = (*iw->ch->te_entree)(&iw2) ;
    }
  else
    i = 0 ;

  evaluation(iw->objet) ;

  return(i) ;
}

DEFINITION_TYPE_EDITEUR(E_commande_menu, E_void,
			"Menu d'exécution",
			{
                          FONCTION_EDITEUR(te->creer  , Menu_creer          ) ;
                          FONCTION_EDITEUR(te->entree , commande_menu_entree) ;
			  FONCTION_EDITEUR(te->ecrit  , commande_menu_ecrit ) ;
			  FONCTION_EDITEUR(te->lit    , Jamais_pareil_lit) ;
			  
			  c->options &= ~(O_Label|O_Sauve|
					 O_Edite|O_Initialiser) ;
			  c->options |= O_Affiche ;
			}
			)

/*
 *
 */

static Booleen option_menu_entree(const Interface_widget *iw)
{
  if ( OBJET(iw->objet)->edite )
    widget_button_ecrit(widget_trouve_fils(OBJET(iw->objet)->edite
					   , iw->ch->nom_champ)
			, iw->texte) ;

  return(0) ;
}

static void Option_menu_ecrit(const Interface_widget* iw)
{
  widget_button_ecrit(iw->w, iw->texte) ;
}

static const char* Option_menu_lit(const Interface_widget* iw)
{
  return(widget_button_lit(iw->w)) ;
}




DEFINITION_TYPE_EDITEUR(E_option_menu, E_void,
			"Menu à choix multiple",
			{
			  FONCTION_EDITEUR(te->creer       , Menu_creer) ;
			  FONCTION_EDITEUR(te->lit         , Option_menu_lit) ;
			  FONCTION_EDITEUR(te->ecrit     , Option_menu_ecrit) ;
			  FONCTION_EDITEUR(te->entree   , option_menu_entree) ;
			}
			)


/*
 ******************************************************************************
 * DRAWING AREA
 ******************************************************************************
 */

static NanoWidget Drawing_creer(const Interface_widget *iw)
{
  NanoWidget d, sw ;

  sw = widget_drawing(iw->w, iw->ch->nom_champ, &d) ;
  widget_add_event_expose         (d, u_callback_expose  , iw->ch) ;
  widget_add_event_button_press   (d, u_ev_button_press  , iw->ch) ;
  widget_add_event_key_press      (d, u_ev_key_press     , iw->ch) ;
  return(sw) ;
}

DEFINITION_TYPE_EDITEUR(E_drawing, E_void,
			"Zone d'affichage indifférenciée",
			{
                          FONCTION_EDITEUR(te->creer ,Drawing_creer    ) ;
                          FONCTION_EDITEUR(te->lit   ,Jamais_pareil_lit) ;
			  
			  c->options &= ~(O_Label | O_Sauve | O_Edite) ;
			  c->options |=  O_Cree | O_Affiche ;
			}
			)

/*
*******************************************************************************
*******************************************************************************
*******************************************************************************
 */

PROTOTYPE_CALLBACK_EVENT(menu_contextuel, w, p, e)
{
  Interface_widget iw ;

  if ( u_initialiser_iw(&iw, w, p, e) )
      FIN_CALLBACK_EVENT_CONTINUE ;

  if ( iw.b == 2 )
    {
      /*
       * p est le noeud sur lequel on a cliqué
       * iw.objet est le noeud modele/operateur
       */
      u_vrai_affiche_menu(w, iw.objet
			  , c_trouve(iw.objet, "menu_contextuel")
			  , e, p) ;
      widget_arrete_emission_button_press(w) ;

      FIN_CALLBACK_EVENT_ARRETE ;
    }
  else
      FIN_CALLBACK_EVENT_CONTINUE ;
}



static NanoWidget Graphe_creer(const Interface_widget *iw)
{
  NanoWidget sw ;

  sw = widget_tree(iw->w, iw, menu_contextuel) ;
  widget_add_event_key_press(sw, u_ev_key_press, iw->ch) ;


  return(sw) ;
}

static const char* Graphe_lit(const Interface_widget *iw)
{
  return( widget_tree_lit(iw->w) ) ;
}

static void Graphe_ecrit(const Interface_widget* iw)
{
  widget_tree_ecrit(iw) ;
}


DEFINITION_TYPE_EDITEUR
(E_graphe_modele, E_void,
 "Graphe du modèle NanoÉdit",
 FONCTION_EDITEUR(te->start_classe, widget_Graphe_start ) ;
 FONCTION_EDITEUR(te->stop_classe , widget_Graphe_stop  ) ;
 FONCTION_EDITEUR(te->creer       , Graphe_creer ) ;
 FONCTION_EDITEUR(te->ecrit       , Graphe_ecrit ) ;
 FONCTION_EDITEUR(te->lit         , Graphe_lit   ) ;
 FONCTION_EDITEUR(te->entree      , widget_Graphe_entree) ;

 c->options &= ~(O_Label | O_Sauve | O_Edite) ;
 c->options |=  O_Cree | O_Affiche ;
 )

/*
 *
 */

static int global_ne_tient_pas_compte_du_prochain_signal = 0 ;

static void ascenseur_sb_ecrit(const Interface_widget *iw)
{
  float v, min, max ;

  min = 0 ;
  max = 1 ;
  v = 0 ;
  sscanf(iw->texte, "%g%g%g", &v, &min, &max) ;
  if ( max <= min )
    {
      max = min + 1 ;      
    }

  global_ne_tient_pas_compte_du_prochain_signal = 1 ;
  widget_scrollbar_ecrit(widget_trouve_fils(iw->w, "sb"), min, max, v) ;
  global_ne_tient_pas_compte_du_prochain_signal = 0 ;
}

static void ascenseur_te_ecrit(const Interface_widget *iw)
{
  widget_texte_ecrit(widget_trouve_fils(iw->w, "te"), iw->texte) ;
}

static const char* ascenseur_lit(const Interface_widget *iw)
{
  return( widget_texte_lit(widget_trouve_fils(iw->w, "te")) ) ;
}

static void ascenseur_ecrit(const Interface_widget *iw)
{
  if ( strcmp(ascenseur_lit(iw),iw->texte) )
    {
      ascenseur_sb_ecrit(iw) ;
      ascenseur_te_ecrit(iw) ;
    }
}

PROTOTYPE_CALLBACK_ACTION(u_ascenseur_callback, w, p)
{
  Interface_widget iw ;
  static char buf[100] ;
  float min ,max, v ;

  if ( global_ne_tient_pas_compte_du_prochain_signal )
    return ;

  if ( u_initialiser_iw(&iw, w, p, NULL) ) return ;

  widget_scrollbar_infos(w, &min, &max, &v) ;
  sprintf(buf, "%g %g %g", v, min, max) ;
  iw.w = widget_parent(iw.w) ;
  iw.texte = buf ;
  /*  ascenseur_te_ecrit(&iw) ; */
  c_sauve_ici(iw.objet, iw.ch, buf ) ;
  OBJET(iw.objet)->urgent = Vrai ;
  evaluation(iw.objet) ;
}

PROTOTYPE_CALLBACK_ACTION(u_texte_ascenseur_callback, w, p)
{
  Interface_widget iw ;

  if ( u_initialiser_iw(&iw, w, p, NULL) ) return ;
  iw.texte = widget_texte_lit(w) ;
  iw.w = widget_parent(iw.w) ;
  ascenseur_sb_ecrit(&iw) ;
}


static NanoWidget ascenseur_creer(const Interface_widget *iw)
{
  NanoWidget w, t, b ;
  Liste *a_manager ;

  a_manager = NULL ;
  b = widget_horizontal_ou_vertical(iw->w, iw->ch->nom_champ, 0, 0) ;

  t = widget_texte(b, "te", 1) ;
  l_ajoute_fin(&a_manager, t) ;

  w = widget_hscrollbar(b, "sb") ;
  l_ajoute_fin(&a_manager, w) ;

  widget_termine_boite(b, 0, 0, a_manager) ;
  widget_manage_liste(a_manager) ;
  l_detruit(&a_manager) ;

  widget_add_event_texte(t, u_texte_ascenseur_callback, iw->ch) ;
  widget_add_event_scrollbar(w, u_ascenseur_callback, iw->ch) ;
  return(b) ;
}

DEFINITION_TYPE_EDITEUR(E_ascenseur, E_void,
			"Ascenseur à effet immédiat",
			{
			  FONCTION_EDITEUR(te->creer, ascenseur_creer) ;
			  FONCTION_EDITEUR(te->lit, ascenseur_lit) ;
			  FONCTION_EDITEUR(te->ecrit, ascenseur_ecrit) ;
			  c->defaut = "0.5 0 1" ;
			}
			)
/*
 ******************************************************************************
 * EDITEUR ANGLE
 ******************************************************************************
 */

static PROTOTYPE_CALLBACK_ACTION(action_menu_choix, w, ch)
{
  Interface_widget iw ;
  
  iw.texte = strdup(widget_button_lit((NanoWidget)ch)) ;
  iw.w = widget_vers_objet(w) ;
  iw.objet = widget_vers_objet(iw.w) ;
  iw.ch = xt_objet_et_widget_vers_champ(iw.objet, iw.w) ;

  widget_button_ecrit( (NanoWidget)ch, widget_button_lit(w)) ;

  if ( iw.ch->te->entree )
    {
      
      (*iw.ch->te->entree)(&iw) ;
    }
  free((char*)iw.texte) ;
}

static PROTOTYPE_CALLBACK_EVENT(affiche_menu_choix, w, ch, event)
{
  widget_positionne_menu((NanoWidget)ch, event) ;
  FIN_CALLBACK_EVENT_ARRETE ;
}

NanoWidget menu_choix(NanoWidget conteneur, char **choix)
{
  NanoWidget menu, bouton_menu, bouton_choix ;
  Liste *a_manager ;

  bouton_menu = widget_button_for_menu(conteneur, "bouton_menu_choix", choix[0]) ;

  menu = widget_popup_menu(widget_sommet(bouton_menu), "menu_choix") ;
  widget_marque_objet(menu, conteneur) ;

  a_manager = NULL ;
  while ( *choix )
    {
      bouton_choix = widget_button_menu(menu, *choix, *choix) ;
      widget_add_event_menu_button(bouton_choix
				   , action_menu_choix
				   , bouton_menu
				   ) ;
      l_ajoute(&a_manager, bouton_choix) ;
      choix++ ;
    }

  widget_termine_menu(menu, a_manager) ;
  l_detruit(&a_manager) ;

  widget_add_event_button_press(bouton_menu, affiche_menu_choix, menu) ;


  return(bouton_menu) ;
}  

static NanoWidget Angle_creer(const Interface_widget *iw)
{
  NanoWidget t, b ;
  static char *types[] = { "Radian", "Degré", NULL } ;
  Liste *a_manager ;

  b = widget_horizontal_ou_vertical(iw->w, iw->ch->nom_champ, 1, 0) ;

  a_manager = NULL ;
  t = widget_texte(b, "te", 1) ;
  l_ajoute_fin(&a_manager, t) ;
  l_ajoute_fin(&a_manager, menu_choix(b, types)) ;
  widget_termine_boite(b, 0, 0, a_manager) ;
  widget_manage_liste(a_manager) ;
  l_detruit(&a_manager) ;

  return(b) ;
}

static const char *Angle_lit(const Interface_widget *iw)
{
  const char *valeur, *type ;
  static Chaine s = {0} ;

  valeur = widget_texte_lit(widget_trouve_fils(iw->w, "te")) ;
  type = widget_button_lit(widget_trouve_fils(iw->w, "bouton_menu_choix")) ;

  s_efface(&s) ;
  s_strcat(&s, valeur) ;
  if ( strcmp(type, "Degré") == 0 )
      s_putchar(&s, 'd') ;
  else
      s_putchar(&s, 'r') ;
  return s_char(&s) ;
}

static void Angle_ecrit(const Interface_widget *iw)
{
  const char *valeur, *type ;
  NanoWidget texte ;
  char *unite ;

  valeur = iw->texte ;
  type = widget_button_lit(widget_trouve_fils(iw->w, "bouton_menu_choix")) ;

  texte = widget_trouve_fils(iw->w, "te") ;
  widget_texte_ecrit(texte, valeur) ;

  if ( valeur[strlen(valeur)-1] == 'd' )
    unite = "Degré" ;
  else
    unite = "Radian" ;

  widget_button_ecrit(widget_trouve_fils(iw->w, "bouton_menu_choix"), unite) ;

}

static Booleen Angle_entree(const Interface_widget *iw)
{
  const char *valeur, *type ;

  valeur = widget_texte_lit(widget_trouve_fils(iw->w, "te")) ;
  type = widget_button_lit(widget_trouve_fils(iw->w, "bouton_menu_choix")) ;
  if ( strcmp(type, iw->texte) != 0 )
    {
      static Chaine s = {0} ;
      s_efface(&s) ;
      if ( strcmp(type, "Degré") == 0 )
	{
	  /* Radian --> degré */
	  s_strcat(&s, flottant_vers_char(atof(valeur)*180/M_PI)) ;
	  s_putchar(&s, 'd') ;
	}
      else
	{
	  /* degré --> radian */
	  s_strcat(&s, flottant_vers_char(atof(valeur)*M_PI/180)) ;
	  s_putchar(&s, 'r') ;
	}
      widget_texte_ecrit(widget_trouve_fils(iw->w, "te")
			 , s_char(&s)) ;
    }

  return(0) ;
}

DEFINITION_TYPE_EDITEUR(E_angle, E_void,
			"Éditeur textuel d'angle (radian/degré)",
			{
                          FONCTION_EDITEUR(te->creer , Angle_creer ) ;
                          FONCTION_EDITEUR(te->lit   , Angle_lit   ) ;
                          FONCTION_EDITEUR(te->ecrit , Angle_ecrit ) ;
                          FONCTION_EDITEUR(te->entree, Angle_entree) ;
			}
			)


