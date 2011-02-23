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
NanoWidget widget_parent(NanoWidget w)
{
  return(NULL) ;
}
Booleen widget_fenetre(NanoWidget w)
{
  return(1) ;
}

void widget_set_close_callback(NanoWidget shell
			       , const Champ* c
			       , POINTEUR_CALLBACK(callback))
{
}

NanoInputId widget_ajoute_fichier(NanoWidget w
				  , int fildes
				  , void *objet
				  , void (*fct)(void *obj)
				  )
{
  return(0) ;
}

void widget_enleve_fichier(NanoInputId id)
{
}

void widget_dump(NanoWidget *w, void *unused)
{
}

NanoWidget widget_trouve_fils(NanoWidget w, const char *nom)
{
  return(w) ;
}


void *widget_vers_objet(NanoWidget w)
{
  return(NULL) ;
}

void widget_marque_objet(NanoWidget w, void *o)
{
}

void *widget_vers_objet2(NanoWidget w)
{
  return(NULL) ;
}

void widget_marque2_objet(NanoWidget w, void *o)
{
}


const char *widget_nom(NanoWidget w)
{
  return("") ;
}

void widget_add_event_button_press(NanoWidget w
				   , POINTEUR_CALLBACK_EVENT(fct)
				   , const Champ *c)
{
}

void widget_add_event_button_release(NanoWidget w
				   , POINTEUR_CALLBACK_EVENT(fct)
				   , const Champ *c)
{
}


void widget_arrete_emission_key_press(NanoWidget w)
{
}

void widget_add_event_key_press(NanoWidget w
				, POINTEUR_CALLBACK_EVENT(fct)
				, const Champ *c)
{
}

void widget_add_event_expose(NanoWidget w
			     , POINTEUR_CALLBACK(fct)
			     , const Champ *c)
{
}
void widget_add_event_resize(NanoWidget w
			     , POINTEUR_CALLBACK(fct)
			     , const Champ *c)
{
}

void widget_add_event_motion(NanoWidget w
			     , POINTEUR_CALLBACK_EVENT(fct)
			     , const Champ *c)
{
}

void widget_add_event_repositionnement(NanoWidget w
				       , POINTEUR_CALLBACK_EVENT(fct))
{
}

void widget_enleve_event_motion(NanoWidget w
				, POINTEUR_CALLBACK_EVENT(fct)
				, const Champ *c)
{
}

void widget_add_event_activate(NanoWidget w
			       , POINTEUR_CALLBACK_ACTION(fct)
			       , const Champ *c)
{
}

void widget_resize(NanoWidget w, int lar, int hau)
{
}

void widget_get_size(NanoWidget w, int *lar, int *hau)
{
}

void widget_get_current_size(NanoWidget w, int *lar, int *hau)
{
}


NanoWidget widget_initialise_application(int *argc, char ***argv
					 , NanoAppContext *app_context)
{
  return(NULL) ;
}

void widget_allocation_couleur(NanoWidget w, const char *nom, NanoColor *c)
{
}

NanoIdle widget_ajoute_idle(NanoAppContext app_context
			, POINTEUR_CALLBACK_IDLE(fct)
			)
{
  return(0) ;
}

void widget_enleve_idle(NanoIdle id)
{
}


void widget_decode_evenement(NanoEvent e, Interface_widget *iw)
{
}

NanoWidget widget_initialiser_bulle_d_aide(NanoWidget parent)
{
  return(NULL) ;
}

void widget_positionne(NanoWidget w, int x, int y)
{
}

void widget_get_position(NanoWidget w, int *x, int *y)
{
}


void widget_popup(NanoWidget w)
{
}

void widget_popdown(NanoWidget w)
{
}

void widget_process_event(NanoAppContext app_context)
{
}

void widget_detruit(NanoWidget w)
{
}

void widget_raise(NanoWidget w)
{
}

void widget_manage_freres(NanoWidget *w, int i)
{
}

void widget_map(NanoWidget w)
{
}

void widget_unmap(NanoWidget w)
{
}


NanoWidget widget_horizontal_ou_vertical(NanoWidget w, const char *nom
					 , int horizontal, int homogene)
{
  return(NULL) ;
}

void widget_termine_boite(NanoWidget w,int horizontal,int homogene, Liste *lw)
{
}

void widget_termine_boite_invisible(NanoWidget w, Liste *lw)
{
}

NanoWidget widget_label(NanoWidget w, const char *nom, const char *label)
{
  return(NULL) ;
}

void widget_set_width(NanoWidget w, int lar)
{
}

void widget_set_height(NanoWidget w, int hau)
{
}

NanoWidget widget_window(NanoWidget w)
{
  return(NULL) ;
}

void widget_set_title(NanoWidget w, const char *title)
{
}

const char* widget_get_title(NanoWidget w)
{
  return("") ;
}

void widget_realize(NanoWidget w)
{
}

void widget_invalide(NanoWidget w, int invalide)
{
}

NanoWidget widget_texte(NanoWidget w, const char *nom, int editable)
{
  return(NULL) ;
}

NanoWidget widget_texte_long(NanoWidget w, const char *nom, int editable)
{
  return(NULL) ;
}

const char *widget_texte_lit(NanoWidget w)
{
  return("") ;
}

void widget_texte_ecrit(NanoWidget w, const char *texte)
{
}

void widget_add_event_texte(NanoWidget w
			    , POINTEUR_CALLBACK_ACTION(fct)
			    , const Champ *c)
{
}

NanoWidget widget_toggle(NanoWidget w, const char *nom, const char *label)
{
  return(NULL) ;
}

int widget_toggle_lit(NanoWidget w)
{
  return(0) ;
}

void widget_toggle_ecrit(NanoWidget w, int i)
{
}

NanoWidget widget_button(NanoWidget w, const char *nom, const char *label)
{
  return(NULL) ;
}

NanoWidget widget_drawing(NanoWidget w, const char *nom, NanoWidget *zone)
{
  *zone = NULL ;
  return(NULL) ;
}


NanoWidget widget_tree(NanoWidget w, const Interface_widget *iw)
{
  return(NULL) ;
}

const char* widget_tree_lit(NanoWidget w)
{
  return("") ;
}

void widget_tree_ecrit(const Interface_widget* iw)
{
}


void widget_scrollbar_ecrit(NanoWidget w, float min, float max, float value)
{
}

void widget_scrollbar_infos(NanoWidget w, float *min, float *max, float *value)
{
}

void widget_add_event_scrollbar(NanoWidget w
				, POINTEUR_CALLBACK_ACTION(fct)
				, const Champ *c)
{
}

NanoWidget widget_hscrollbar(NanoWidget w, const char *nom)
{
  return(NULL) ;
}

void widget_Graphe_start(const Interface_widget *iw)
{
}

void widget_Graphe_stop(const Interface_widget *iw)
{
}

void widget_Graphe_trace(const Interface_widget *iw)
{
}

Booleen widget_Graphe_entree(const Interface_widget *iw)
{
  return(Faux) ;
}

void widget_current_gl(NanoWidget w, GLXContext c)
{
}
void widget_swap_gl(NanoWidget w)
{
}

NanoWidget widget_gl(NanoWidget w, const char *nom)
{
  return(NULL) ;
}
void widget_destroy_context(NanoWidget w, GLXContext c)
{
}
GLXContext widget_create_context(NanoWidget w)
{
  /*
   * Car on considère que NULL n'est pas alloué
   * et dans ce cas "widget_destroy_context" n'est pas appelé
   */
  return((GLXContext)1) ;
}

void widget_unmanage_freres(NanoWidget *w, int i)
{
}

void widget_item_detruit(NanoWidget w)
{
}


NanoWidget widget_popup_menu(NanoWidget w, const char *titre)
{   
  return(NULL) ;
}
NanoWidget widget_pulldown_menu(NanoWidget w)
{   
  return(NULL) ;
}
void widget_attache_sous_menu(NanoWidget bouton, NanoWidget menu)
{
}
void widget_add_event_activate_cascade(NanoWidget w
				       , POINTEUR_CALLBACK_ACTION(fct)
				       , void *c)
{
}
void widget_add_event_menu_button(NanoWidget w
				  , POINTEUR_CALLBACK_ACTION(fct)
				  , const Champ *c)
{
}
NanoWidget widget_bouton_cascade(NanoWidget w
				 , const char *nom, const char *label)
{
  return(NULL) ;
}

NanoWidget widget_button_for_menu(NanoWidget w
				 , const char *nom, const char *label)
{
  return(NULL) ;
}
 
NanoWidget widget_button_menu(NanoWidget w, const char *nom, const char *label)
{
  return(NULL) ;
}

void widget_sensible(NanoWidget w, int i)
{
}
Booleen widget_visible(NanoWidget w)
{
   return(0) ;
}
void widget_termine_menu(NanoWidget w, Liste *lw)
{
}
Liste *widget_fils(NanoWidget w)
{
  return(NULL) ;
}
Booleen widget_est_tearoff(NanoWidget w)
{
  return(0) ;
}
void widget_detruit_menu(NanoWidget menu)
{
}
void widget_enleve_event_activate_cascade(NanoWidget w
				       , POINTEUR_CALLBACK_ACTION(fct)
				       , void *c)
{
}
void widget_label_ecrit(NanoWidget w, const char *t)
{
}
const char* widget_label_lit(NanoWidget w)
{
  return("") ;
}
void widget_positionne_menu(NanoWidget w, NanoEvent *event)
{
}

const char* widget_button_lit(NanoWidget w)
{
  return("") ;
}
void widget_button_ecrit(NanoWidget w, const char*s)
{
}

void widget_flush(NanoWidget w)
{
}
