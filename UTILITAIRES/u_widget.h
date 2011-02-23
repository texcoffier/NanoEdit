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
 * Créé le Thu May 25 13:25:05 2000 par Thierry EXCOFFIER
 */

#ifndef _HOME_EXCO_NE_1_11_UTILITAIRES_U_WIDGET_H
#define _HOME_EXCO_NE_1_11_UTILITAIRES_U_WIDGET_H

typedef void* NanoWidget ;
typedef void* NanoEvent ;
typedef unsigned long NanoInputId ;
typedef void* NanoAppContext ;
typedef unsigned long NanoIdle ;
typedef struct { long xxx[10] ; } NanoColor ;

#ifndef GLX_H
#ifndef __GLX_glx_h__
#ifndef __glx_h__
typedef int GLXContext ;
#endif
#endif
#endif

#ifdef UTILISE_MOTIF

#define PROTOTYPE_CALLBACK(NOM, W, P) void NOM(NanoWidget W, void*P)
#define PROTOTYPE_CALLBACK_ACTION(NOM, W, P) PROTOTYPE_CALLBACK(NOM, W, P)
#define PROTOTYPE_CALLBACK_EVENT(NOM, W, P, E) void NOM(NanoWidget W, void*P, NanoEvent *E, char *valeur_de_retour)
#define POINTEUR_CALLBACK(NOM) void (*NOM)(NanoWidget, void*)
#define POINTEUR_CALLBACK_EVENT(NOM) void (*NOM)(NanoWidget, void*, NanoEvent*, char*)
#define POINTEUR_CALLBACK_ACTION(NOM) POINTEUR_CALLBACK(NOM)
#define FIN_CALLBACK
#define FIN_CALLBACK_EVENT_CONTINUE { *valeur_de_retour = 1 ; return ; }
#define FIN_CALLBACK_EVENT_ARRETE { *valeur_de_retour = 0 ; return ; }
#define FIN_CALLBACK_IDLE return(0)
#define FIN_CALLBACK_IDLE_ARRETE return(1)


#else

#define PROTOTYPE_CALLBACK_EVENT(NOM, W, P, E) int NOM(NanoWidget W, NanoEvent *E, void *P)
#define PROTOTYPE_CALLBACK(NOM, W, P) PROTOTYPE_CALLBACK_EVENT(NOM,W,P,unused)
#define PROTOTYPE_CALLBACK_ACTION(NOM, W, P) void NOM(NanoWidget W, void *P)
#define POINTEUR_CALLBACK(NOM) int (*NOM)(NanoWidget, NanoEvent*, void*)
#define POINTEUR_CALLBACK_EVENT(NOM) POINTEUR_CALLBACK(NOM)
#define POINTEUR_CALLBACK_ACTION(NOM) void (*NOM)(NanoWidget, void*)
#define FIN_CALLBACK return(1)
#define FIN_CALLBACK_EVENT_CONTINUE return(0)
#define FIN_CALLBACK_EVENT_ARRETE return(1)
#define FIN_CALLBACK_IDLE return(1)
#define FIN_CALLBACK_IDLE_ARRETE return(0)

#endif


#define PROTOTYPE_CALLBACK_IDLE(NOM) int NOM()
#define POINTEUR_CALLBACK_IDLE(NOM) int (*NOM)()

NanoWidget widget_parent(NanoWidget w) ;
Booleen widget_fenetre(NanoWidget w) ;
void widget_set_close_callback(NanoWidget shell
			       , const Champ* c
			       , POINTEUR_CALLBACK(callback)) ;
NanoInputId widget_ajoute_fichier(NanoWidget w
				  , int fildes
				  , void *objet
				  , void (*fct)(void *obj)
				  ) ;
void widget_enleve_fichier(NanoInputId id) ;
int widget_timeout_add(int (*fct)(void *obj), int interval, void *data) ;
void widget_timeout_remove(void *data) ;
NanoWidget widget_trouve_fils(NanoWidget w, const char *nom) ;
void *widget_vers_objet(NanoWidget w) ;
const char* widget_nom(NanoWidget w) ;
void widget_add_event_map_fenetre(NanoWidget w
			  , POINTEUR_CALLBACK_EVENT(map)
			  , POINTEUR_CALLBACK_EVENT(unmap)
			  ) ;
void widget_add_event_button_press(NanoWidget w
				   , POINTEUR_CALLBACK_EVENT(fct)
				   , const Champ *c) ;
void widget_add_event_button_release(NanoWidget w
				   , POINTEUR_CALLBACK_EVENT(fct)
				   , const Champ *c) ;
void widget_arrete_emission_key_press(NanoWidget w) ;
void widget_arrete_emission_button_press(NanoWidget w) ;
void widget_add_event_key_press(NanoWidget w
				, POINTEUR_CALLBACK_EVENT(fct)
				, const Champ *c) ;
void widget_add_event_expose(NanoWidget w
			     , POINTEUR_CALLBACK(fct)
			     , const Champ *c) ;
void widget_add_event_resize(NanoWidget w
			     , POINTEUR_CALLBACK(fct)
			     , const Champ *c) ;
void widget_add_event_motion(NanoWidget w
			     , POINTEUR_CALLBACK_EVENT(fct)
			     , const Champ *c) ;
void widget_add_event_repositionnement(NanoWidget w
				       , POINTEUR_CALLBACK_EVENT(fct)) ;
void widget_enleve_event_repositionnement(NanoWidget w
					  , POINTEUR_CALLBACK_EVENT(fct)) ;
void widget_enleve_event_motion(NanoWidget w
				, POINTEUR_CALLBACK_EVENT(fct)
				, const Champ *c) ;
void widget_enleve_event_button_release(NanoWidget w
					, POINTEUR_CALLBACK_EVENT(fct)
					, const Champ *c) ;
void widget_add_event_activate(NanoWidget w
			       , POINTEUR_CALLBACK_ACTION(fct)
			       , const Champ *c) ;
void widget_add_event_activate_cascade(NanoWidget w
				       , POINTEUR_CALLBACK_ACTION(fct)
				       , void *fm) ;
void widget_enleve_event_activate_cascade(NanoWidget w
				       , POINTEUR_CALLBACK_ACTION(fct)
				       , void *c) ;
void widget_add_event_texte(NanoWidget w
			    , POINTEUR_CALLBACK_ACTION(fct)
			    , const Champ *c) ;
void widget_add_event_destroy(NanoWidget w
			      , POINTEUR_CALLBACK_ACTION(fct)
			      , void *c) ;
void widget_add_event_unmap(NanoWidget w
			    , POINTEUR_CALLBACK_ACTION(fct)
			    , const Champ *c) ;
void widget_add_event_tearoff(NanoWidget w
			    , POINTEUR_CALLBACK_ACTION(fct)
			    , const Champ *c) ;
void widget_enleve_event_unmap(NanoWidget w
				       , POINTEUR_CALLBACK_ACTION(fct)
				       , void *c) ;
void widget_add_event_menu_button(NanoWidget w
			       , POINTEUR_CALLBACK_ACTION(fct)
			       , const Champ *c) ;

void widget_decode_evenement(NanoEvent e, Interface_widget *iw) ;
void widget_resize(NanoWidget w, int lar, int hau) ;
NanoWidget widget_initialise_application(int *argc, char ***argv
					 , NanoAppContext *app_context) ;
void widget_allocation_couleur(NanoWidget w, const char *nom, NanoColor *c) ;
NanoIdle widget_ajoute_idle(NanoAppContext app_context
			, POINTEUR_CALLBACK_IDLE(fct)
			) ;
void widget_enleve_idle(NanoIdle id) ;
NanoWidget widget_initialiser_bulle_d_aide(NanoWidget parent) ;
NanoWidget widget_vrai_bulle_d_aide(NanoWidget parent, const char *texte) ;
void widget_positionne(NanoWidget w, int x, int y) ;
void widget_popup(NanoWidget w) ;
void widget_popdown(NanoWidget w) ;
int widget_process_event(NanoAppContext app_context) ;
void widget_detruit(NanoWidget w) ;
void widget_item_detruit(NanoWidget w) ;
Booleen widget_visible(NanoWidget w) ;
Liste *widget_fils(NanoWidget w) ;
void widget_congele(NanoWidget w) ;
void widget_degele(NanoWidget w) ;
Booleen widget_est_tearoff(NanoWidget w) ;
void widget_raise(NanoWidget w) ;
void widget_marque_objet(NanoWidget w, void *o) ;
void widget_manage_freres(NanoWidget *w, int nb) ;
void widget_unmanage_freres(NanoWidget *w, int i) ;
void widget_map(NanoWidget w) ;
int widget_event_pending(NanoAppContext app_context) ;
void widget_unmap(NanoWidget w) ;
void widget_set_width(NanoWidget w, int lar) ;
void widget_set_height(NanoWidget w, int hau) ;
NanoWidget widget_horizontal_ou_vertical(NanoWidget w, const char *nom
					 , int horizontal, int homogene) ;
void widget_termine_boite(NanoWidget w,int horizontal,int homogene,Liste *lw) ;
void widget_termine_boite_invisible(NanoWidget w, Liste *lw) ;
void widget_termine_menu(NanoWidget w, Liste *lw) ;
void widget_sensible(NanoWidget w, int i) ;

NanoWidget widget_label(NanoWidget w, const char *nom, const char *label) ;
void widget_label_ecrit(NanoWidget w, const char *t) ;
const char* widget_label_lit(NanoWidget w) ;

NanoWidget widget_texte(NanoWidget w, const char *nom, int editable) ;
NanoWidget widget_texte_long(NanoWidget w, const char *nom, int editable) ;
const char *widget_texte_lit(NanoWidget w) ;
void widget_texte_ecrit(NanoWidget w, const char *texte) ;

NanoWidget widget_toggle(NanoWidget w, const char *nom, const char *label) ;
int widget_toggle_lit(NanoWidget w) ;
void widget_toggle_ecrit(NanoWidget w, int i) ;

NanoWidget widget_button(NanoWidget w, const char *nom, const char *label) ;
const char* widget_button_lit(NanoWidget w) ;
void widget_button_ecrit(NanoWidget w, const char*) ;
NanoWidget widget_button_for_menu(NanoWidget w
				 , const char *nom, const char *label) ;

NanoWidget widget_popup_menu(NanoWidget w, const char* titre) ;
NanoWidget widget_pulldown_menu(NanoWidget w) ;
NanoWidget widget_bouton_cascade(NanoWidget w
				 , const char *nom, const char *label) ;
NanoWidget widget_button_menu(NanoWidget w,const char *nom, const char *label);
NanoWidget widget_button_menu_with_icon(NanoWidget w,const char *nom, const char *label, const char *icon_file);
void widget_positionne_menu(NanoWidget w, NanoEvent *event) ;
void widget_attache_sous_menu(NanoWidget bouton, NanoWidget menu) ;
NanoWidget widget_menu_attache(NanoWidget bouton) ;
void widget_detruit_menu(NanoWidget menu) ;



NanoWidget widget_window(NanoWidget w, const char *nom) ;
void widget_set_title(NanoWidget w, const char *title) ;
void widget_realize(NanoWidget w) ;
void widget_invalide(NanoWidget w, int invalide) ;
void widget_get_size(NanoWidget w, int *lar, int *hau) ;
void widget_get_current_size(NanoWidget w, int *lar, int *hau) ;
void widget_get_position(NanoWidget w, int *x, int *y) ;
const char* widget_get_title(NanoWidget w) ;
NanoWidget widget_drawing(NanoWidget w, const char *nom, NanoWidget *zone) ;
NanoWidget widget_tree(NanoWidget w, const Interface_widget *iw, POINTEUR_CALLBACK_EVENT(fct)) ;
const char* widget_tree_lit(NanoWidget w) ;
void widget_tree_ecrit(const Interface_widget* iw) ;

void widget_scrollbar_ecrit(NanoWidget w, float min, float max, float value) ;
void widget_scrollbar_infos(NanoWidget w, float *min, float *max, float*value);
void widget_add_event_scrollbar(NanoWidget w
				, POINTEUR_CALLBACK_ACTION(fct)
				, const Champ *c) ;
NanoWidget widget_hscrollbar(NanoWidget w, const char *nom) ;


void widget_Graphe_start(const Interface_widget *iw) ;
void widget_Graphe_stop(const Interface_widget *iw) ;
void widget_Graphe_trace(const Interface_widget *iw) ;
Booleen widget_Graphe_entree(const Interface_widget *iw) ;

void widget_current_gl(NanoWidget w, GLXContext c) ;
void widget_swap_gl(NanoWidget w) ;
NanoWidget widget_gl(NanoWidget w, const char *nom) ;
void widget_destroy_context(NanoWidget w, GLXContext c) ;
GLXContext widget_create_context(NanoWidget w) ;
void widget_flush(NanoWidget w) ;

void widget_ne_recoit_plus_evenement(NanoWidget w) ;
void widget_recoit_a_nouveau_evenement(NanoWidget w) ;

int widget_ajoute_timeout(int interval, Booleen (*fct)(void*), void *) ;
void widget_enleve_timeout(int id) ;
/*
 * Génériques
 */

void widget_manage_liste(const Liste *a_manager) ;
void widget_unmanage_liste(const Liste *a_manager) ;
NanoWidget widget_shell(NanoWidget w) ;
NanoWidget widget_sommet(NanoWidget w) ;
NanoWidget widget_fils0(NanoWidget w) ;


#define DEBUG_GEOMETRIE 0






#endif
