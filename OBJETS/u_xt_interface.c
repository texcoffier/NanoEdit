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
#include "u_repartiteur.h"
#include "u_widget.h"


static void redirection(void *o)
{
  struct fichier *f ;

  f = o ;
  if ( (*f->fonction)(f->objet, f->fildes) == Arrete )
    {
      xt_enleve_fichier(f->objet, f->fonction, f->fildes) ;
    }
}

void xt_ajoute_fichier(void *objet,
		       Parcour (*fonction)(void *objet, int fildes),
		       int fildes
		       )
{
  struct fichier *f ;
  Xt_interface *interf ;

  interf = u_racine(objet) ;
  ALLOUER(f, 1) ;
  f->objet = objet ;
  f->fonction = fonction ;
  f->fildes = fildes ;
  f->id = widget_ajoute_fichier(interf->sommet, fildes, f, redirection) ;
  l_ajoute(&interf->attentes_fichier, f) ;
}

void xt_enleve_fichier(void *objet,
		      Parcour (*fonction)(void *objet, int fildes),
		      int fildes
		      )
{
  Xt_interface *interf ;
  
  interf = u_racine(objet) ;
  POUR(struct fichier *, f, interf->attentes_fichier,
       {
	 if ( f->fildes == fildes
	      && f->objet == objet
	      && f->fonction == fonction
	      )
      {
	widget_enleve_fichier(f->id) ;
	l_enleve( &interf->attentes_fichier, f ) ;
	free(f) ;
	return ;
      }
  }
	 ) ;
}


void* xt_widget_vers_objet(NanoWidget w)
{
  void *objet ;

  objet = widget_vers_objet(w) ;
  /*
  if ( objet == NULL )
    {
      ICI ;
      eprintf("WIDGET NOM=(%s)\n", widget_nom(w)) ;
      EXIT ;
    }
  */
  return(objet) ;
}


const Champ* xt_objet_et_widget_vers_champ(void *o, NanoWidget w)
{
  if ( w == 0 )
    {
      return(NULL) ;
    }
  POUR_LES_CHAMPS( o, ch,
		   {
		     if ( CREE(ch)
			  &&
			  ( strcmp( ch->nom_champ, widget_nom(w) ) == 0
			    || strcmp( ch->nom_champ,
				       widget_nom(w)+sizeof(LABEL_)-1) == 0
			    )
			  )
		       return(ch) ;
		   }
		   ) ;
  return(xt_objet_et_widget_vers_champ(o, widget_parent(w))) ;
}

const Champ* xt_widget_vers_champ(NanoWidget w)
{
  return(xt_objet_et_widget_vers_champ(xt_widget_vers_objet(w), w)) ;
}

/*
 * Prend les valeurs des Widgets et les sauvegardes dans la structure
 */
void w_applique(void *objet)
{
  const char *s ;
  Interface_widget iw ;

  if ( OBJET(objet)->edite == NULL )
    {
      eprintf("Edite est NULL!!!\n") ;
      return ;
    }
  if ( TRACE(objet) ) eprintf("w_applique\n") ;

  iw.objet = objet ;
  POUR_LES_CHAMPS(objet, ch,
		  if ( MODIFIABLE(ch) /* && INITIALISER(ch) 20/05/1999 */ )
		  {
		    iw.ch = ch ;
		    iw.w  = widget_trouve_fils(OBJET(objet)->edite,ch->nom_champ) ;
		    s = (*ch->te->lit)(&iw) ;
                    if ( strcmp( s, c_recupere_ici_sans_copie(objet,ch) ) )
                      c_sauve_ici(objet, ch, s) ;
		  }
		  ) ;
} 

/*
 *
 */

static void xt_premier_plan(void *objet)
{
  NanoWidget w ;

  w = OBJET(objet)->edite ;
  if ( w )
    widget_raise(w) ;
}

/*
 *
 */
static void u_ferme(void*o)
{
  NanoWidget tmp ;
  Objet *o_ = OBJET(o) ;

  if ( TRACE2(o) )
    {
      eprintf("Fermeture réelle de fenêtre\n") ;
    }

  if ( o_->edite )
    {
      l_ajoute_fin( &((Classe*)o_->classe)->fenetres_unmappees, o_->edite) ;
      widget_marque_objet(o_->edite, NULL) ;
      /*
       * Pour bétonner la fermeture
       */
      POUR_LES_CHAMPS(o, ch,	
		      if ( AFFICHE(ch) )
		      {
			tmp  = widget_trouve_fils(o_->edite, ch->nom_champ) ;
			widget_marque_objet(tmp, NULL) ;
		      }
		      ) ;
      unmapper_fenetre(o) ;
      o_->edite = NULL ;
    }
}

/*
 *
 */

/* 04/07/2000 Ne cré pas les widget en trop */
#define WIDGET_CREE(O, C) ( CREE(C) && !(!(O)->classe->affiche_champs_classe && CHAMP_OBJET(C)) )


static void u_positionne_fenetre(void *objet, NanoWidget shell)
{
  const Champ *geometrie ;
  Interface_widget iw ;
  const void *o ;
  
  o = objet ;
  if ( c_trouve_type(&o, &geometrie, &infos_L_geometrie) && objet == o )
    {
      iw.w = shell ;
      iw.objet = objet ;
      iw.texte = c_recupere_ici_sans_copie(objet, geometrie) ;
      if ( DEBUG_GEOMETRIE )
	eprintf("REPOSITIONNE : %s\n", iw.texte) ;
      (*geometrie->te->ecrit)(&iw) ;
    }
}

/*
 * Un problème avec GL est que les fenêtres GL non mappée
 * prennent de la place dans la carte graphique.
 */
NanoWidget u_trouve_fenetre_adequate(void *objet)
{
  Quadruplet q ;
  int lar, hau ;
  Liste **fu ;
  NanoWidget f ;
  int max_size ;

  fu = (Liste**) &OBJET(objet)->classe->fenetres_unmappees ;
  /*
   * Cherche fenetre de même taille
   */
  if ( c_type_existe(objet, &infos_L_geometrie) )
    {
      q = char_vers_quadruplet( c_recupere_type_sans_copie(objet
							   , &infos_L_geometrie
							   )
				) ;
      POUR(NanoWidget, w, *fu,
      {
	widget_get_size(w, &lar, &hau) ;
	if ( lar == q.z && hau == q.h )
	  {
	    l_enleve(fu, w ) ;
	    return( w ) ;
	  }
      }) ;
    }
  /*
   * Cherche plus grande fenetre
   */
  f = OBJET(objet)->classe->fenetres_unmappees->objet ;
  max_size = 0 ;
  POUR(NanoWidget, w, *fu,
  {
    widget_get_size(w, &lar, &hau) ;
    if ( lar*hau > max_size )
      {
	f = w ;
	max_size = lar*hau ;
      }
  }) ;

  l_enleve(fu, f) ;
  return( f ) ;
}

static void u_remap_fenetre(Xt_interface *o, void *objet)
{
  NanoWidget form ;
  Interface_widget iw ;
  Objet *objet_ ;

  objet_ = OBJET(objet) ;

  objet_->edite = u_trouve_fenetre_adequate(objet) ;

  iw.objet = objet ;

  form = objet_->edite ;
  widget_marque_objet(form, objet) ;
  POUR_LES_CHAMPS(objet,c,
		  if ( WIDGET_CREE(objet_, c) )
		  {
		    iw.w  = widget_trouve_fils(form, c->nom_champ) ;
		    iw.ch = c ;
		    (*c->te->init)(&iw) ;
		  }
		  ) ;
  POUR_TOUS_LES_CHAMPS
    (objet,ch,
     {
       if ( WIDGET_CREE(objet_, ch) )
	 widget_marque_objet(widget_trouve_fils(form, ch->nom_champ),objet);
     }
     ) ;
  /* Passe ces deux lignes avant le Map le 10/05/2000 */
  u_enleve_action((Booleen (*)(void *))widget_unmap, OBJET(objet)->edite) ;
  recupere_fenetre(objet) ;

  /*
   * BEURK, positionne bien la fenetre avant de l'ouvrir
   * Sinon, le configure la repositionne au mauvais endroit
   * lorsqu'elle apparait.
   * Ceci n'est pas fait dans "recupere_fenetre car c'est différé.
   */
  if ( 1 )
    {
      u_positionne_fenetre(objet, objet_->edite) ;
    }
  
  widget_recoit_a_nouveau_evenement(objet_->edite) ;
  widget_map(objet_->edite) ;
}
/*
 * Quand on reçoit le repositionnement de fenetre, on remet
 * la position à jour dans l'objet.
 * Cette fonction n'est plus appelée car elle implique
 * trop de problème.
 * Il faut maintenant cliquer sur Applique.
 */

static PROTOTYPE_CALLBACK_EVENT(repositionne_fenetre, w, c, event)
{
  Interface_widget iw ;
  const Champ *geometrie ;
  Buffer buf ;
  const void *o ;

  if ( u_initialiser_iw(&iw, w, c, event) ) FIN_CALLBACK_EVENT_CONTINUE ;
  
  o = iw.objet ;
  if ( c_trouve_type(&o, &geometrie, &infos_L_geometrie) && o == iw.objet )
    {
      sprintf(buf, "%d %d %d %d",
	      iw.configure_x, iw.configure_y, iw.configure_l, iw.configure_h) ;
      if ( DEBUG_GEOMETRIE )
	eprintf("événement repositionne en : %s\n", buf) ;
      c_sauve_ici(iw.objet, geometrie, buf ) ;
      //   evaluation(iw.objet) ;
    }
  
  FIN_CALLBACK_EVENT_CONTINUE ;
}

static PROTOTYPE_CALLBACK_EVENT(elle_se_montre, w, c, event)
{
  void *o ;

  if ( DEBUG_GEOMETRIE )
    eprintf("MAP\n") ;
  o = widget_vers_objet(w) ;
  /*  u_positionne_fenetre(o, OBJET(o)->edite) ; */
  // Commenté le 6/2/2004 puis decommenté
  widget_add_event_repositionnement(w, repositionne_fenetre) ;
  FIN_CALLBACK_EVENT_CONTINUE ;
}

static PROTOTYPE_CALLBACK_EVENT(elle_se_cache, w, c, event)
{
  if ( DEBUG_GEOMETRIE )
    eprintf("UNMAP\n") ;

  // Commenté le 6/2/2004 puis decommenté
  widget_enleve_event_repositionnement(w, repositionne_fenetre) ;
  FIN_CALLBACK_EVENT_CONTINUE ;
}

static const Champ *champ_suivant(void *objet, const Champ *ch)
{
  int trouve ;

  trouve = 0 ;
  POUR_TOUS_LES_CHAMPS(objet, c,
		       {
			 if ( trouve == 1 )
			   return(c) ;
			 if ( ch == c )
			   trouve = 1 ;
		       }
		       ) ;
  return(NULL) ;
}

static PROTOTYPE_CALLBACK(u_close_help, w, c)
{
  widget_unmap(w) ;
  FIN_CALLBACK ;
}

void u_popup_help_reel(const Interface_widget *iw)
{
  Xt_interface *interface ;
  const char *aide ;

  if ( TRACE(iw->objet) )
    eprintf("Début aide\n") ;

  aide = iw->ch
    ? (*iw->ch->te->aide)(iw)
    : u_aide_objet(iw->objet);

  if ( *aide == '\0' )
    return ;

  interface = (Xt_interface*)u_racine(iw->objet) ;
  if ( interface->help_shell == 0 )
    {
      interface->help_shell =
	widget_initialiser_bulle_d_aide(interface->sommet) ;
      if ( TRACE(iw->objet) )
	eprintf("Label Créé\n") ;
      widget_set_close_callback(interface->help_shell,NULL,u_close_help);
    }

  if ( TRACE(iw->objet) )
    eprintf("Aide calculée\n") ;

  widget_texte_ecrit(widget_trouve_fils(interface->help_shell, "help_label")
		   , aide) ;
  /*
    widget_positionne(interface->help_shell, iw.x_root, iw.y_root) ;
  if ( TRACE(iw.objet) )
    eprintf("Fenêtre positionnée\n") ;
  */
  widget_popup(interface->help_shell) ;
}

static PROTOTYPE_CALLBACK_EVENT(u_popup_help, w, p, e)
{
  Interface_widget iw ;

  if ( u_initialiser_iw(&iw, w, p, e) ) FIN_CALLBACK_EVENT_CONTINUE ;

  if ( iw.b != 2 )
    FIN_CALLBACK_EVENT_CONTINUE ;

  u_popup_help_reel(&iw) ;

  FIN_CALLBACK_EVENT_ARRETE ;
}

/*
static PROTOTYPE_CALLBACK_EVENT(u_popdown_help, w, p, e)
{
  Xt_interface *interface ;
  Interface_widget iw ;
  void *o ;

  if ( u_initialiser_iw(&iw, w, p, e) ) FIN_CALLBACK_EVENT_CONTINUE ;

  if ( iw.b != 2 )
     FIN_CALLBACK_EVENT_CONTINUE ;

  o = xt_widget_vers_objet(w) ;
  if ( o == NULL ) FIN_CALLBACK_EVENT_CONTINUE ;
  interface = u_racine(o) ;
  widget_popdown(interface->help_shell) ;
  FIN_CALLBACK_EVENT_CONTINUE ;
}
*/


/*
 * Retourne le nombre de Widget créé
 */

NanoWidget widget_labelise(NanoWidget parent, const Champ *ch,NanoWidget *reel)
{
  Liste *a_manager ;
  Buffer buf ;
  NanoWidget w, l ;
  Interface_widget iw ;
  static int i = 0 ;

  sprintf(buf, "L%d", i++) ;
  w = widget_horizontal_ou_vertical(parent, buf, 1, 0) ;

  a_manager = NULL ;

  sprintf(buf, LABEL_ "%s", ch->nom_champ) ;
  l = widget_label(w, buf, (char*)ch->label) ;
  widget_add_event_button_press(l, u_popup_help, ch) ;
  /*  widget_add_event_button_release(l, u_popdown_help, ch) ; */
  l_ajoute_fin(&a_manager, l) ;

  iw.w = w ;
  iw.ch = ch ;
  iw.w = (*ch->te->creer)(&iw) ;
  *reel = iw.w ;
  l_ajoute_fin(&a_manager, iw.w) ;

  widget_termine_boite(w, 1, 0, a_manager) ;

  widget_manage_liste(a_manager) ;
  l_detruit(&a_manager) ;

  return(w) ;
}


PROTOTYPE_CALLBACK(u_callback_close, w, p)
{
  Interface_widget iw ;

  if ( u_initialiser_iw(&iw, w, p, NULL) )
    FIN_CALLBACK ;

  u_ferme(iw.objet) ;
  FIN_CALLBACK ;
}



static int u_creation_champ(NanoWidget parent, void *objet, const Champ **c,
			    int horizontal, int homogene, NanoWidget shell)
{
  Interface_widget iw ;
  int invisible ;
  const Champ *ch ;
  NanoWidget w, reel ;
  Liste *a_manager, *non_visible ;
  int nb, h, ho ;

  if ( *c == NULL )
    return(0) ;

  if ( TRACE2(objet) )
    eprintf("Début création champ\n") ;
  a_manager = NULL ;
  non_visible = NULL ;
  iw.objet = objet ;
  for(ch=*c; ch ; ch = champ_suivant(objet, ch) )
    {
      if ( strcmp(ch->nom_champ, CHAMP_FERME) == 0 )
	widget_set_close_callback(shell, ch, u_callback_close) ;
      if ( ch->tl == &infos_L_fin_boite )
	{
	  break ;
	}
      if ( ch->tl == &infos_L_debut_boite_horizontale
	   || ch->tl == &infos_L_debut_boite_verticale )
	{
	  if ( TRACE2(objet) )
	    eprintf("Début boite\n") ;
	  h = (ch->tl == &infos_L_debut_boite_horizontale) ;
	  ho = BOITE_HOMOGENE(ch) ;
	  w = widget_horizontal_ou_vertical(parent
					    , horizontal==-1
					    ? OBJET(objet)->classe->nom
					    : ch->nom_champ
					    , h
					    , ho
					    ) ;
	  ch = champ_suivant(objet, ch) ;
	  if ( u_creation_champ(w, objet, &ch, h, ho, shell) )
	    {
	      l_ajoute_fin(&a_manager, w) ;
	    }
	  else
	    {
	      widget_detruit(w) ;
	    }
	  continue ;
	}
      if ( !CREE(ch) )
	{
	  continue ;
	}  
      iw.ch = ch ;
      invisible = !OBJET(objet)->classe->affiche_champs_classe
	&& CHAMP_OBJET(ch) ;

      /* 04/07/2000 Ne cré pas les widget en trop */
      if ( invisible )
	continue ;

      if ( TRACE2(objet) )
	eprintf("Création champ %s\n", ch->nom_champ) ;
      if ( !invisible && A_LABEL(ch) )
	{
	  iw.w = widget_labelise(parent, ch, &reel) ;
	}
      else
	{
	  if ( invisible )
	    iw.w = widget_parent(parent) ;
	  else
	    iw.w = parent ;

	  iw.w = (*ch->te->creer)(&iw) ;
	  reel = iw.w ;
	}


      if ( !invisible && AFFICHE(ch) )
	{
	  widget_add_event_button_press(reel, u_popup_help, ch) ;
	  /*  widget_add_event_button_release(reel, u_popdown_help, ch) ; */
	  if ( ch->te_largeur )
	    widget_set_width(reel, ch->te_largeur) ;
	  if ( ch->te_hauteur )
	    widget_set_height(reel, ch->te_hauteur) ;
	  l_ajoute_fin(&a_manager, iw.w) ;
	}
      else
	l_ajoute(&non_visible, iw.w) ;
    } 

  widget_termine_boite(parent, horizontal, homogene, a_manager) ;
  *c = ch ;
  nb = l_longueur(a_manager) + l_longueur(non_visible) ;
  widget_manage_liste(a_manager) ;
  l_detruit(&a_manager) ;
  widget_termine_boite_invisible(parent, non_visible) ;
  l_detruit(&non_visible) ;

  if ( TRACE2(objet) )
    eprintf("Fin création champ\n") ;
  return(nb) ;
}


static void u_creation_fenetre(Xt_interface *o, void *objet)
{
  NanoWidget shell ;
  const Champ *geometrie, *title ;
  const void *oo ;
  const Champ *c ;


  shell = widget_window(o->sommet, OBJET(objet)->classe->nom) ;

  
  oo = objet ;
  /* && oo == objet pour éviter de trouver le champ DANS le metamorphe */
  if ( c_trouve_type(&oo, &title, &infos_L_titre) && oo == objet )
    widget_set_title(shell, c_recupere_ici_sans_copie(oo, title)) ;
  else
    widget_set_title(shell, OBJET(objet)->classe->nom) ;
  

  /*
   * ICI : pas propre (devrait être dans type geometrie)
   * probleme de placement avec LESSTIF
   */

  oo = objet ;
  if ( c_trouve_type(&oo, &geometrie, &infos_L_geometrie) && oo == objet )
    {
      /*
      Quadruplet geom ;
      geom = char_vers_quadruplet( c_recupere_ici_sans_copie(oo, geometrie)) ;
      widget_positionne(shell, (int)geom.x, (int)geom.y) ;
      if ( geom.z != 0 )
	widget_set_width(shell, (int)geom.z) ;
      if ( geom.h != 0 )
	widget_set_height(shell, (int)geom.h) ;
      */
    }
  else
    {
      widget_positionne(shell, 400, o->y%800) ;
      o->y += 20 ;
    }


  widget_add_event_map_fenetre(shell, elle_se_montre, elle_se_cache) ;  

  l_ajoute( &((Classe*)OBJET(objet)->classe)->fenetres_unmappees, shell) ;

  c = (const Champ*)l_element(champs_objet,0) ;
  u_creation_champ(shell, objet, &c, -1, -1, shell) ;

  if ( 0 )
    u_positionne_fenetre(objet, shell) ;

  widget_realize(shell) ;
}


/*
 *
 */
static void u_ouvre(Xt_interface *o, void *objet)
{
  /*
   * On n'ouvre pas de fenêtres si la classe
   * interface est celle du sommet (o_interface.c)
   * car elle ne supporte pas les fenêtres.
   */
  if ( OBJET(o)->classe->sc == NULL )
    return ;
  if ( OBJET(objet)->classe->fenetres_unmappees==NULL )
    {
      u_creation_fenetre(o, objet) ;
    }
  u_remap_fenetre(o, objet) ;
  /* 10/05/2000
  u_enleve_action((Booleen (*)(void *))unmap_widget, OBJET(objet)->edite) ;
  recupere_fenetre(objet) ;
  */
}

static void xt_ouvrir_fenetre_edition(void *objet)
{
  if ( OBJET(objet)->edite==NULL )
    u_ouvre((Xt_interface*)u_racine(objet), objet) ;
  else
    xt_premier_plan(objet) ;
}

static void xt_applique(void *objet)
{
  w_applique(objet) ;
  evaluation(objet) ;
}

/*
 * Prend la structure et la sauvegarde dans les widgets
 */
static void w_recupere(void *objet)
{
  const char *old_texte ;
  Interface_widget iw ;
  Objet *objet_ ;

  objet_ = OBJET_OU_NULL(objet) ;
  if ( objet_==NULL || objet_->edite == 0 )
    return ;
  if ( TRACE_(objet_) ) eprintf("w_recupère widget\n") ;
  /*
  printf("==========================\n") ;
  printf("========================== %s\n", objet_->classe->nom) ;
  printf("========================== %s\n", OBJET(objet_->parent->objet)->classe->nom) ;
  o_dump(objet_->parent->objet) ;
  printf("==========================999=========================\n") ;
  */

  /*  objet_->mettre_a_jour_widget = Faux ; 25/02/2000 */
  iw.objet = objet ;
  objet_->urgent = Faux ;
  POUR_LES_CHAMPS_
    (objet_,ch,
     if ( CREE(ch) )
     {		
       if ( TRACE2_(objet_) )
	 eprintf("Mise à jour widget du champ (%s/%s)\n",
		 ch->nom_champ, ch->tp->nom) ;
       iw.ch = ch ;
       iw.w = widget_trouve_fils(objet_->edite, ch->nom_champ) ;
       if ( iw.w == NULL )
	 abort() ;
       iw.texte = c_recupere_ici_sans_copie(objet, ch) ;
       // eprintf("%x : %s : %s\n", objet, ch->nom_champ, iw.texte) ;
       if ( iw.texte )
	 {
	   iw.texte = strdup(iw.texte) ;
	   old_texte = (*ch->te->lit)(&iw) ;
	   if ( TRACE2(objet) )
	     eprintf("texte=(%s) old_texte=(%s)\n", iw.texte, old_texte) ;

	   if ( strcmp(iw.texte, old_texte) )
	     (*ch->te->ecrit)(&iw) ;
	   free((char*)iw.texte) ;
	 }
       widget_invalide(iw.w, CHAMP_INVALIDE_(objet_,ch)) ;
       if ( TRACE2_(objet_) )
	 eprintf("Fin Mise à jour widget du champ (%s/%s)\n",
		 ch->nom_champ, ch->tp->nom) ;

     }
     ) ;
}

/*
 *
 *
 */

void ouvrir_fenetre(void *o)
{
  if ( TRACE2(o) )
    eprintf("ouvrir_fenetre\n") ;
  u_enleve_action_unique(u_ferme, o) ;
  u_ajoute_action_unique(PRIORITE_OUVRIR_FENETRE,xt_ouvrir_fenetre_edition,o) ;
}

static void *globale_objet_a_tester ;

Booleen contient_objet(void *d)
{
  Interface_widget *iw = d ;
  Booleen res ;

  res = ( iw->objet == globale_objet_a_tester ) ;
  if ( res )
    free(iw) ;

  return(res) ;
}

void unmapper_fenetre(void *o)
{
  if ( TRACE2(o) )
    eprintf("unmapper_fenetre\n") ;
  u_ajoute_action_unique(PRIORITE_UNMAPPER_FENETRE
			 , widget_unmap
			 , OBJET(o)->edite) ;

  globale_objet_a_tester = o ;
  u_enleve_action_unique_par_test_de_donnee(w_expose, contient_objet) ;
}


void fermer_fenetre(void *o)
{
  if ( TRACE2(o) )
    eprintf("fermer_fenetre\n") ;
  u_enleve_action_unique(xt_ouvrir_fenetre_edition, o) ;
  u_enleve_action_unique(w_recupere, o) ;

  globale_objet_a_tester = o ;
  u_enleve_action_unique_par_test_de_donnee(w_expose, contient_objet) ;

  /*
  u_ajoute_action_unique(PRIORITE_FERMER_FENETRE, u_ferme, o) ;
  */
  u_ferme(o) ;

  widget_ne_recoit_plus_evenement(OBJET(o)->edite) ;
}

/*
 * Probleme du rand 28/11/2000 : perturbe ou non la liste
 * du répartiteur ?
 * Réponse : cela fonctionne mais oblige le répartiteur à travailler
 * avec une liste plus longue.
 * Solution adoptée : le répartiteur tire au hazard en cas d'égalité.
 * Mais cela souvève des problèmes.
 */
void recupere_fenetre(void *o)
{
  if ( TRACE2(o) )
    eprintf("recupere_fenetre\n") ;
  if ( OBJET(o)->edite || trouve_action((Booleen (*)(void *))xt_ouvrir_fenetre_edition, o) )
    {
      if ( OBJET(o)->urgent )
	u_ajoute_action_unique(PRIORITE_RECUPERE_URGENT, w_recupere, o) ;
      else
	u_ajoute_action_unique(PRIORITE_RECUPERE/*+(rand()%10)*/, w_recupere, o) ;
    }
}

void applique_fenetre(void *o)
{
  if ( TRACE2(o) )
    eprintf("applique_fenetre\n") ;
  /* Le fait de suite car la fermeture est faite de suite */
  xt_applique(o) ;
  /*
  u_ajoute_action_unique(PRIORITE_APPLIQUE, xt_applique, o) ;
  */
}

void detruire_objet(void *o)
{
  if ( TRACE2(o) )
    eprintf("detruire_objet\n") ;
  POUR_TOUS_LES_PARENTS_(OBJET(o), oo, NOTE_FILS_CHANGE( oo );) ;
  fermer_fenetre(o) ;
  u_ajoute_action_unique(PRIORITE_DETRUIRE_OBJET, o_destruction, o) ;
}

void detruire_objet_remonte_fils(void *o)
{
  if ( TRACE2(o) )
    eprintf("detruire_objet_remonte_fils (pere=%s)\n",
	    OBJET(OBJET(o)->parent->objet)->classe->nom) ;
  POUR_TOUS_LES_PARENTS_(OBJET(o), oo, NOTE_FILS_CHANGE( oo );) ;
  fermer_fenetre(o) ;
  u_ajoute_action_unique(PRIORITE_DETRUIRE_OBJET, o_destruction_remonte, o) ;
}


void evaluation_(void *o)
{
  NOTE_FILS_CHANGE(o) ;
  POUR(void*, i, u_operateur_ou_modele_liste(o),
       {
	 if ( ! FILS_CHANGE(i) )
	   evaluation_(i) ;
       }) ;

}

void evaluation(void *o)
{
  if ( TRACE2(o) )
    eprintf("évaluation\n") ;

  // Commenté le 5/2/2007 car dans 'test_bouton_charge' cela
  // fait descendre des valeurs d'un objet dans ces fils
  // alors qu'il ne faudrait pas.
  // NOTE_UN_CHAMP_CHANGE(o) ;

  /*
   * Ce qui suit permet d'indiquer aux opérateurs que leur fils change
   */
  
  POUR(void*, i, u_operateur_ou_modele_liste(o),
       {
	 evaluation_(i) ;
       }) ;

  u_ajoute_action_unique(PRIORITE_EVALUATION, o_appel_changement, u_racine(o));
}
