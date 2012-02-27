/*
    NanoÉdit: Un modeleur algébrique interactif.
    Copyright (C) 1996-2010 Thierry EXCOFFIER, Université Claude Bernard, LIRIS

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

#include <gtk/gtk.h>
#include <gtk/gtkgl.h>
#include <gdk/gdk.h>
#include "s_string.h"
#include <locale.h>
/* #include <gdk/gdkx.h> */

#ifdef GTK_WIDGET_MAPPED
#define gtk_widget_get_mapped GTK_WIDGET_MAPPED
#endif

/*
 *
 * Problème :
 *    widget_get_position
 * Réaction différente si c'est un widget ``shell''
 */

#if 0
#define EPRINTF eprintf
#define COUCOU ICI
#else
#define EPRINTF if (0) eprintf
#define COUCOU
#endif

static GtkWidget *cherche_label(GtkWidget *w)
{
  GList *gl ;
  GtkWidget *c ;

  COUCOU ;
  if ( GTK_IS_LABEL(w) )
    return(w) ;
  if ( GTK_IS_CONTAINER(w) )
    {
      gl = gtk_container_get_children(GTK_CONTAINER(w)) ;
      if ( gl )
	{
	  c = cherche_label( gl->data ) ;
	  g_list_free(gl);
	  return( c ) ;
	}
    }
  return(NULL) ;
}


NanoWidget widget_parent(NanoWidget w)
{
  COUCOU ;
  if ( GTK_IS_ADJUSTMENT(w) )
    return(widget_parent(g_object_get_data(G_OBJECT(w), "NanoEdit"))) ;
else
  return( (NanoWidget) GTK_WIDGET(w)->parent ) ;
}
Booleen widget_fenetre(NanoWidget w)
{
  COUCOU ;
  return( GTK_IS_WINDOW(w) ) ;
}

void widget_set_close_callback(NanoWidget shell
			       , const Champ* c
			       , POINTEUR_CALLBACK(callback))
{
  EPRINTF("SET CALL BACK %p\n", c) ;
  g_signal_connect (G_OBJECT (shell), "delete-event",
		    (GCallback)callback, (void*)c);
}

struct data_and_function
{
  void *data ;
  void (*fct)(void *obj) ;
} ;

gboolean redirect_io(GIOChannel *source,
		     GIOCondition condition,
		     gpointer data)
{
  struct data_and_function *daf = data ;
  daf->fct(daf->data) ;    
  return TRUE;
}

NanoInputId widget_ajoute_fichier(NanoWidget w
				  , int fildes
				  , void *objet
				  , void (*fct)(void *obj)
				  )
{
  struct data_and_function *daf ;

  COUCOU ;
  ALLOUER(daf, 1) ; // Memory leak
  daf->data = objet ;
  daf->fct = fct ;
  
  return(
	 (NanoInputId)g_io_add_watch(g_io_channel_unix_new(fildes)
				    , GDK_INPUT_READ|GDK_INPUT_EXCEPTION
				    , (GIOFunc)redirect_io
				    , (gpointer)daf
				    )
	 ) ;
}

void widget_enleve_fichier(NanoInputId id)
{
  COUCOU ;
  g_source_remove( (gint)id ) ;
}

void widget_dump(GtkWidget *w, void *unused)
{
  static int prof = 0 ;

  COUCOU ;
  eprintf("%p %*s%s %s (parent=%p)%s\n", w, prof, "",
	  g_type_name(G_OBJECT_TYPE(w)), widget_nom(w),
	  gtk_widget_get_parent_window(w),
	  gtk_widget_get_mapped(w)?"Mapped":"NOTMAPPED") ;

  if ( GTK_IS_CONTAINER(w) )
    {
      prof += 4 ;
      gtk_container_foreach(GTK_CONTAINER(w), widget_dump, NULL) ;
      prof -= 4 ;
    }
}

static GtkWidget *globale_widget_trouve ;

static void trouve_nom(GtkWidget *w, gpointer nom)
{
  if ( strcmp(gtk_widget_get_name(GTK_WIDGET(w)), (char*)nom) == 0 )
    {
      if ( globale_widget_trouve == NULL )
	globale_widget_trouve = w ;
      else
	EXIT ;
    }
  if ( GTK_IS_CONTAINER(w) )
    {
      gtk_container_foreach(GTK_CONTAINER(w), trouve_nom, (gpointer)nom) ;
    }
}

NanoWidget widget_trouve_fils(NanoWidget w, const char *nom)
{
  COUCOU ;
  globale_widget_trouve = NULL ;
  gtk_container_foreach(GTK_CONTAINER(w), trouve_nom, (gpointer)nom) ;
  if ( globale_widget_trouve == NULL )
    {
      eprintf("Trouve pas dans %p : %s\n", w, nom) ;
      widget_dump(w, NULL) ;
      EXIT ;
    }
  return( (NanoWidget)globale_widget_trouve ) ;
}


void *widget_vers_objet(NanoWidget w)
{
  void *o ;

  COUCOU ;
  do
    {
      o = g_object_get_data(G_OBJECT(w), "NanoEdit") ;
      if ( o == NULL )
	{
	  if ( GTK_IS_ADJUSTMENT(w) )
	    EXIT ;
	  w = widget_parent(w) ;
	  if ( w == NULL )
	    return(NULL) ;
	}
    }
  while(o==NULL) ;

  if ( GTK_IS_ADJUSTMENT(w) )
    o = widget_vers_objet(o) ; /* L'objet est dans la scrollbar */

  return( o ) ;
}

void widget_marque_objet(NanoWidget w, void *o)
{
  COUCOU ;
  g_object_set_data(G_OBJECT(w), "NanoEdit", o) ;
}

void *widget_vers_objet2(NanoWidget w)
{
  COUCOU ;
  return( g_object_get_data(G_OBJECT(w), "NanoEdit2") ) ;
}

void widget_marque2_objet(NanoWidget w, void *o)
{
  COUCOU ;
  g_object_set_data(G_OBJECT(w), "NanoEdit2", o) ;
}


const char *widget_nom(NanoWidget w)
{
  COUCOU ;
  if ( GTK_IS_ADJUSTMENT(w) )
    return("ajustement") ;
  else
    return( gtk_widget_get_name(GTK_WIDGET(w)) ) ;
}

void widget_add_event_button_press(NanoWidget w
				   , POINTEUR_CALLBACK_EVENT(fct)
				   , const Champ *c)
{
  GtkWidget *ww ;

  ww = cherche_label(w) ;
  if ( ww )
    ww = widget_parent(ww) ;
  if ( ww )
    w = ww ;
  g_signal_connect( G_OBJECT(w), "button-press-event",
		    (GCallback)fct, 
		    (gpointer)c);  
  g_signal_connect( G_OBJECT(w), "scroll-event",
		    (GCallback)fct, 
		    (gpointer)c);  
}

void widget_add_event_button_release(NanoWidget w
				   , POINTEUR_CALLBACK_EVENT(fct)
				   , const Champ *c)
{
  GtkWidget *ww ;
  
  COUCOU ;
  ww = cherche_label(w) ;
  if ( ww )
    ww = widget_parent(ww) ;
  if ( ww )
    w = ww ;

  g_signal_connect( GTK_OBJECT(w), "button-release-event",
		    (GCallback)fct, 
		    (gpointer)c);  
}

void widget_enleve_event_button_release(NanoWidget w
					, POINTEUR_CALLBACK_EVENT(fct)
					, const Champ *c)
{
  EPRINTF("widget_enleve_event_button_release\n") ;
  g_signal_handlers_disconnect_by_func(G_OBJECT(w)
			      , (GCallback)fct
			      , (gpointer)c) ;
}


void widget_arrete_emission_key_press(NanoWidget w)
{
  COUCOU ;
  g_signal_stop_emission_by_name(w, "key-press-event") ;
}

void widget_arrete_emission_button_press(NanoWidget w)
{
  COUCOU ;
  g_signal_stop_emission_by_name(w, "button-press-event") ;
  // g_signal_stop_emission_by_name(w, "scroll-event") ;
}

void widget_add_event_map_fenetre(NanoWidget w
			  , POINTEUR_CALLBACK_EVENT(map)
			  , POINTEUR_CALLBACK_EVENT(unmap)
			  )
{
  COUCOU ;
  g_signal_connect( G_OBJECT(w), "unmap-event", (GCallback)unmap, NULL);  
  g_signal_connect( G_OBJECT(w), "map-event", (GCallback)map,  NULL);  
}

void widget_add_event_key_press(NanoWidget w, POINTEUR_CALLBACK_EVENT(fct)
				, const Champ *c)
{
  COUCOU ;
  g_signal_connect( G_OBJECT(w),"key-press-event",(GCallback)fct,(gpointer)c);
}

void widget_add_event_expose(NanoWidget w, POINTEUR_CALLBACK(fct)
			     , const Champ *c)
{
  COUCOU ;
  g_signal_connect( G_OBJECT(w), "expose-event", (GCallback)fct, (gpointer)c) ;
}
void widget_add_event_resize(NanoWidget w, POINTEUR_CALLBACK(fct)
			     , const Champ *c)
{
  COUCOU ;
  ICI;
  g_signal_connect(G_OBJECT(w), "configure-event", (GCallback)fct,(gpointer)c);
}

void widget_add_event_motion(NanoWidget w
			     , POINTEUR_CALLBACK_EVENT(fct)
			     , const Champ *c)
{
  EPRINTF("widget_add_event_motion\n") ;
  g_signal_connect( G_OBJECT(w), "motion-notify-event",(GCallback)fct, 
		      (gpointer)c);  
}

void widget_add_event_repositionnement(NanoWidget w
				       , POINTEUR_CALLBACK_EVENT(fct))
{
  COUCOU ;
  g_signal_connect( G_OBJECT(w), "configure-event", (GCallback)fct, NULL);
}

void widget_enleve_event_repositionnement(NanoWidget w
					  , POINTEUR_CALLBACK_EVENT(fct))
{
  EPRINTF("widget_enleve_event_repositionnement\n") ;
  g_signal_handlers_disconnect_by_func(G_OBJECT(w),(GCallback)fct,NULL);
}

void widget_enleve_event_motion(NanoWidget w
				, POINTEUR_CALLBACK_EVENT(fct)
				, const Champ *c)
{
  EPRINTF("widget_enleve_event_motion\n") ;
  g_signal_handlers_disconnect_by_func(G_OBJECT(w),(GCallback)fct
				       ,(gpointer)c) ;
}

void widget_add_event_activate(NanoWidget w
			       , POINTEUR_CALLBACK_ACTION(fct)
			       , const Champ *c)
{
  EPRINTF("SET EVENT ACTIVATE %p\n", c) ;
  g_signal_connect(G_OBJECT(w), "clicked", (GCallback)fct, (gpointer)c);
}

void widget_resize(NanoWidget w, int lar, int hau)
{
  gtk_widget_set_size_request(GTK_WIDGET(w), 1, 1) ;
  gtk_window_resize(GTK_WINDOW(w), lar, hau) ;
  EPRINTF("widget_set_size = %d %d\n", lar, hau) ;
}

void widget_get_size(NanoWidget w, int *lar, int *hau)
{
  GtkRequisition r ;

  COUCOU ;
  gtk_widget_get_child_requisition( GTK_WIDGET(w), &r) ;
  *lar = r.width ;
  *hau = r.height ;
  EPRINTF("widget_get_size = %d %d\n", *lar, *hau) ;
}

void widget_get_current_size(NanoWidget w, int *lar, int *hau)
{
  COUCOU ;
  *lar = GTK_WIDGET(w)->allocation.width ;
  *hau = GTK_WIDGET(w)->allocation.height ;
  EPRINTF("widget_get_current_size = %d %d\n", *lar, *hau) ;


}


NanoWidget widget_initialise_application(int *argc, char ***argv
					 , NanoAppContext *app_context)
{
  EPRINTF("============================================ DEBUT INIT\n") ;
  setlocale(LC_CTYPE, "");
  setlocale(LC_NUMERIC, "C");
  gtk_init(argc, argv);
  gtk_rc_parse(".gtkrc") ;
  setlocale(LC_NUMERIC, "C");
  EPRINTF("============================================ FIN INIT\n") ;
  return(NULL) ;
}

void widget_allocation_couleur(NanoWidget w, const char *nom, NanoColor *c)
{
  COUCOU ;
  gdk_color_parse(nom, (GdkColor*)c) ;
  gdk_colormap_alloc_color(gdk_colormap_get_system(), (GdkColor*)c, FALSE, TRUE) ;
}

NanoIdle widget_ajoute_idle(NanoAppContext app_context
			, POINTEUR_CALLBACK_IDLE(fct)
			)
{
  EPRINTF("Ajoute idle\n") ;
  return( g_idle_add((GSourceFunc)fct, NULL) ) ;
}

void widget_enleve_idle(NanoIdle id)
{
  COUCOU ;
  g_source_remove( (gint)id ) ;
  //  g_idle_remove( (guint)id ) ;
}

int widget_timeout_add(int (*fct)(void *obj), int interval, void *data)
{
  COUCOU ;
  return g_timeout_add(interval, fct, data) ;
}

void widget_timeout_remove(void *data)
{
  COUCOU ;
  g_source_remove( (gint)data ) ;
  // gtk_timeout_remove((int)data) ;
}




void widget_decode_evenement(NanoEvent e, Interface_widget *iw)
{
  GdkWindow *win ;
  GdkEventButton *ev ;
  GdkEventKey *ek ;
  GdkEventScroll *es ;
  GdkModifierType mt ;
  gint x, y, x_root, y_root ;
  int xx, yy ;

  EPRINTF("Début décode\n") ;

  iw->e = e ;
  iw->etat = 0 ;
  ev = (GdkEventButton*)e ;

  switch( ev->type )
    {
    case GDK_2BUTTON_PRESS:
    case GDK_3BUTTON_PRESS:
    case GDK_BUTTON_PRESS:
    case GDK_BUTTON_RELEASE:
    case GDK_MOTION_NOTIFY:
      iw->double_clique = (ev->type == GDK_2BUTTON_PRESS) ;
      iw->x = ev->x ;
      iw->y = ev->y ;
      iw->x_root = ev->x_root ;
      iw->y_root = ev->y_root ;
      break ;
    case  GDK_CONFIGURE:

      break ;
    default:
      win = gtk_widget_get_parent_window(iw->w) ;
      gdk_window_get_pointer(win, &x, &y, &mt) ;
      gdk_window_get_root_origin(win, &x_root, &y_root) ;
      widget_get_position(iw->w, &xx, &yy) ;
      iw->x = x - xx ;
      iw->y = y - yy ;
      iw->x_root = x + x_root ;
      iw->y_root = y + y_root ;
      break ;
    }


  iw->touche[0] = '\0' ;

  /*  EPRINTF("Décode événement : (%d,%d) position (%d %d)\n"
      , iw->x, iw->y, xx, yy) ; */

  iw->configure_x = 0 ;
  if ( ev->type == GDK_CONFIGURE )
    {
      iw->configure_x = ((GdkEventConfigure*)ev)->x ;
      iw->configure_y = ((GdkEventConfigure*)ev)->y ;
      iw->configure_l = ((GdkEventConfigure*)ev)->width ;
      iw->configure_h = ((GdkEventConfigure*)ev)->height ;
      /* widget_get_position(iw->w, &iw->configure_x, &iw->configure_y) ; */
    }


  if ( 0
       || ev->type == GDK_BUTTON_PRESS
       || ev->type == GDK_2BUTTON_PRESS
       || ev->type == GDK_3BUTTON_PRESS
       || ev->type == GDK_BUTTON_RELEASE
       )
    {
      iw->b = ev->button-Button1 ;
      if ( ev->state & Button1Mask )
	iw->b = 0 ;
      if ( ev->state & Button2Mask )
	iw->b = 1 ;
      if ( ev->state & Button3Mask )
	iw->b = 2 ;
      if(ev->state & ShiftMask)
	iw->etat |=1;
      if(ev->state & ControlMask)
	iw->etat |=2;
      if(ev->state & Mod1Mask)
	iw->etat |=4;
    }
    
  if ( ev->type == GDK_SCROLL )
    {
      es = (GdkEventScroll*)e ;
      switch(es->direction)
	{
	case GDK_SCROLL_UP: iw->b = 4 ; break ;
	case GDK_SCROLL_DOWN: iw->b = 3 ; break ;
	default: break ;
	}
    }

  if ( ev->type == GDK_KEY_PRESS )
    {
      ek = (GdkEventKey*)e ;

      iw->etat = (ek->state & 8)/2
	+ (ek->state & 4)/2
	+ (ek->state & 1) ;

      widget_keyval_to_string( ek->keyval, ek->keyval, iw) ;
    }
  EPRINTF("Fin décode\n") ;
}

NanoWidget widget_initialiser_bulle_d_aide(NanoWidget parent)
{
  GtkWidget *w, *l ;
  COUCOU ;

  w = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  /* gtk_window_set_policy(GTK_WINDOW(w), TRUE, TRUE, TRUE) ; */
  gtk_window_set_title(GTK_WINDOW(w), "Aide") ;
  l = gtk_text_view_new() ;
  gtk_widget_set_name(l, "help_label") ;
  gtk_widget_set_size_request(l, 500, 200) ;
  gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(l), GTK_WRAP_WORD) ;

  gtk_container_add (GTK_CONTAINER(w), l) ;

  return((NanoWidget)w) ;
}

NanoWidget widget_vrai_bulle_d_aide(NanoWidget parent, const char *texte)
{
  GtkWidget *w, *l ;
  COUCOU ;

  w = gtk_window_new(GTK_WINDOW_POPUP);
  l = gtk_label_new(texte) ;
  gtk_container_add (GTK_CONTAINER(w), l) ;
  gtk_widget_show(l) ;

  return((NanoWidget)w) ;
}

void widget_positionne(NanoWidget w, int x, int y)
{
  COUCOU ;
  if ( DEBUG_GEOMETRIE )
    eprintf("widget_positionne %d %d\n", x, y) ;
  gtk_window_move(GTK_WINDOW(w), x, y) ;
  
  GTK_WIDGET(w)->allocation.x = x ;
  GTK_WIDGET(w)->allocation.y = y ;
}

void widget_get_position(NanoWidget w, int *x, int *y)
{
  COUCOU ;
  *x = GTK_WIDGET(w)->allocation.x ;
  *y = GTK_WIDGET(w)->allocation.y ;
  if ( DEBUG_GEOMETRIE )
    eprintf("widget_get_position allocation %d %d\n", *x, *y) ;
}


void widget_popup(NanoWidget w)
{
  EPRINTF("widget popup\n") ;
  gtk_widget_show_all(w) ;
}

void widget_popdown(NanoWidget w)
{
  COUCOU ;
  gtk_widget_hide_all(w) ;
}

int widget_event_pending(NanoAppContext app_context)
{
  return gtk_events_pending() ;
}

int widget_process_event(NanoAppContext app_context)
{
  gtk_main_iteration() ;
  return gtk_events_pending() ;
}

void widget_detruit(NanoWidget w)
{
  COUCOU ;
  gtk_object_destroy(GTK_OBJECT(w)) ;
}

void widget_raise(NanoWidget w)
{
  COUCOU ;
  gdk_window_raise( GTK_WIDGET(w)->window ) ;
}

void widget_manage_freres(NanoWidget *w, int i)
{
  int j ;
  EPRINTF("Manage freres\n") ;
  for(j=0;j<i;j++)
    gtk_widget_show( w[j] ) ;
}

void widget_map(NanoWidget w)
{
  COUCOU ;
  gtk_widget_show(w) ;
}

void widget_unmap(NanoWidget w)
{
  COUCOU ;
  gtk_widget_hide(w) ;
}


NanoWidget widget_horizontal_ou_vertical(NanoWidget w, const char *nom
					 , int horizontal, int homogene)
{
  GtkWidget *f ;
  COUCOU ;
  if ( horizontal )
    f = gtk_hbox_new(homogene, 0) ;
  else
    f = gtk_vbox_new(homogene, 0) ;

 gtk_container_set_resize_mode( GTK_CONTAINER(f), GTK_RESIZE_QUEUE) ;

  gtk_widget_set_name( GTK_WIDGET(f), nom) ;

  return( (NanoWidget)f ) ;
}

void widget_termine_boite(NanoWidget w,int horizontal,int homogene, Liste *lw)
{
  int nb, e ;

  EPRINTF("Termine boite w=%p(%s) horizontal=%d\n"
	  , w, widget_nom(w), horizontal) ;
  
  if ( horizontal == -1 )
    POUR(GtkWidget*, f, lw,
	 EPRINTF("ajoute %s\n", widget_nom(f)) ;
	 gtk_container_add (GTK_CONTAINER(w), GTK_WIDGET(f)) ;
	 )
  else
    {
      nb = l_longueur(lw) ;
      POUR(GtkWidget*, f, lw,
	   EPRINTF("ajoute en haut %s\n", widget_nom(f)) ;
	   e = ( homogene || i_f == nb-1 ) ;
	   gtk_box_pack_start( GTK_BOX(w), GTK_WIDGET(f), e, e, 1) ;
	   ) ;
    }
}

void widget_termine_boite_invisible(NanoWidget w, Liste *lw)
{
  COUCOU ;
  POUR(GtkWidget*, f, lw,
	 EPRINTF("ajoute invisible %s\n", widget_nom(f)) ;
       gtk_container_add (GTK_CONTAINER(w), GTK_WIDGET(f));
       ) ;
}

NanoWidget widget_label(NanoWidget w, const char *nom, const char *label)
{
  GtkWidget *f ;

  f = gtk_label_new(label) ;
  gtk_widget_set_name(f, nom) ;
  return( (NanoWidget)f) ;
}

void widget_set_width(NanoWidget w, int lar)
{
  int a, b ;
  COUCOU ;
  gtk_widget_get_size_request(GTK_WIDGET(w),&a, &b) ;
  if ( GTK_IS_WINDOW(w) )
    gtk_window_set_default_size(GTK_WINDOW(w), lar, b) ;
  else
    gtk_widget_set_size_request(GTK_WIDGET(w), lar, b) ;

}

void widget_set_height(NanoWidget w, int hau)
{
  int a, b ;
  COUCOU ;
  gtk_widget_get_size_request(GTK_WIDGET(w),&a, &b) ;
  if ( GTK_IS_WINDOW(w) )
    gtk_window_set_default_size(GTK_WINDOW(w), a, hau) ;
  else
    gtk_widget_set_size_request(GTK_WIDGET(w), a, hau) ;
}

NanoWidget widget_window(NanoWidget w, const char *name)
{
  GtkWidget *f ;

  EPRINTF("widget_window\n") ;
  f = gtk_window_new (GTK_WINDOW_TOPLEVEL) ;
  gtk_widget_set_name(GTK_WIDGET(f), name) ;
  gtk_window_set_resizable(GTK_WINDOW(f), TRUE) ;
  gtk_widget_set_events(GTK_WIDGET(f), GDK_STRUCTURE_MASK) ;

  return(f) ;
}

void widget_set_title(NanoWidget w, const char *title)
{
  COUCOU ;
  gtk_window_set_title( GTK_WINDOW(w), title) ;
  /*
  gtk_widget_set_name( GTK_WIDGET(w), title) ;
  gtk_widget_ensure_style( GTK_WIDGET(w) ) ;
  */
}

const char* widget_get_title(NanoWidget w)
{
  COUCOU ;
  const char *a ;
  a = gtk_window_get_title( GTK_WINDOW(w) ) ;
  if ( a ) return a ;
  return "????" ;
}

void widget_realize(NanoWidget w)
{
  EPRINTF("REALIZE============================================\n") ;
}

void widget_change_style(GtkWidget *w, void *style)
{
  COUCOU ;
  gtk_widget_set_style(GTK_WIDGET(w), (GtkStyle*)style) ;
  if ( GTK_IS_CONTAINER(w) )
    {
      gtk_container_foreach(GTK_CONTAINER(w), widget_change_style, style) ;
    }
}

static void widget_set_style_non_editable(GtkWidget *w)
{
  static GtkStyle *texte_non_editable = NULL ;

  COUCOU ;
  if ( texte_non_editable == NULL )
    {
      texte_non_editable = gtk_rc_get_style_by_paths
	(gtk_settings_get_default(), "widget_non_editable", NULL,G_TYPE_NONE) ;
    }
  widget_change_style(GTK_WIDGET(w), texte_non_editable) ;
  
  // No more copy/paste possible...
  //gtk_widget_set_sensitive(GTK_WIDGET(w), FALSE);
}



void widget_invalide(NanoWidget w, int invalide)
{
  static GtkStyle *fond_rouge = NULL ;

  COUCOU ;
  if ( fond_rouge == NULL )
    {
      fond_rouge = gtk_rc_get_style_by_paths
	(gtk_settings_get_default(), "widget_fond_rouge", NULL,G_TYPE_NONE) ;
    }

  if ( invalide )
    {
      widget_change_style(GTK_WIDGET(w), fond_rouge) ;
    }
  else
    {
      if ( GTK_IS_EDITABLE(w) && !gtk_editable_get_editable(GTK_EDITABLE(w)) )
	widget_set_style_non_editable(w) ;
      else
	gtk_widget_set_style(GTK_WIDGET(w), NULL) ;
    }
}



NanoWidget widget_texte(NanoWidget w, const char *nom, int editable)
{
  GtkWidget *t ;

  COUCOU ;
  t = gtk_entry_new() ;
  gtk_widget_set_name(GTK_WIDGET(t), nom) ;
  gtk_editable_set_editable(GTK_EDITABLE(t), editable) ;
  if ( !editable )
    {
      widget_set_style_non_editable(t) ; /* 22/02/2002 */
    }
  return( (NanoWidget)t ) ;

}

NanoWidget widget_texte_long(NanoWidget w, const char *nom, int editable)
{
  GtkWidget *t, *b ;
  // GtkWidget *sb ;

  COUCOU ;
  b = gtk_hbox_new(FALSE,0) ;

  t = gtk_text_view_new() ;
  gtk_widget_set_name(GTK_WIDGET(t), nom) ;
  gtk_editable_set_editable(GTK_EDITABLE(t), editable) ;

  //  sb = gtk_vscrollbar_new (GTK_TEXT_VIEW(t)->vadj);
  gtk_box_pack_start(GTK_BOX(b), t, TRUE, TRUE, 0);
  //gtk_box_pack_start(GTK_BOX(b), sb, FALSE, FALSE, 0);


  gtk_widget_show(GTK_WIDGET(t)) ;
  //gtk_widget_show(GTK_WIDGET(sb)) ;

  return( (NanoWidget)b ) ;
}

const char *widget_texte_lit(NanoWidget w)
{
  static char *s = NULL ;
  COUCOU ;
  if ( s )
    g_free(s) ;
  if ( GTK_IS_EDITABLE(w) )
    {
      s = gtk_editable_get_chars( GTK_EDITABLE(w), 0, -1) ;
    }
  else
    {
      GtkTextBuffer *tb = tb = gtk_text_view_get_buffer(GTK_TEXT_VIEW(w)) ;
      GtkTextIter ts, te ;
      gtk_text_buffer_get_start_iter(tb, &ts) ;
      gtk_text_buffer_get_end_iter(tb, &te) ;
      s =  gtk_text_buffer_get_text(tb, &ts, &te, FALSE) ;
    }
  EPRINTF("Texte lit = (%s)\n", s) ;
  return(s) ;
}

void widget_texte_ecrit(NanoWidget w, const char *texte)
{
  gint pos ;
  COUCOU ;
  /*
  if ( GTK_IS_TEXT_VIEW(w) )
    gtk_text_freeze(GTK_TEXT_VIEW(w)) ;
  */
  if ( GTK_IS_EDITABLE(w) )
    {
      gtk_editable_delete_text(GTK_EDITABLE(w), 0, -1) ;
      pos = 0 ;
      gtk_editable_insert_text(GTK_EDITABLE(w),
			       texte,
			       strlen(texte), &pos) ;
    }
  else
    {
      gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(w)),
			       texte,
			       strlen(texte));
    }

  /*
  if ( GTK_IS_TEXT(w) )
    gtk_text_thaw(GTK_TEXT(w)) ;
  */
}

void widget_add_event_texte(NanoWidget w
			    , POINTEUR_CALLBACK_ACTION(fct)
			    , const Champ *c)
{
  COUCOU ;
  g_signal_connect(G_OBJECT(w), "changed", (GCallback)fct,(gpointer)c);
}

NanoWidget widget_toggle(NanoWidget w, const char *nom, const char *label)
{
  GtkWidget *t ;

  COUCOU ;
  t = gtk_toggle_button_new_with_label(label) ;
  gtk_widget_set_name(t, nom) ;
  return( (NanoWidget)t ) ;
}

int widget_toggle_lit(NanoWidget w)
{
  COUCOU ;
  return(GTK_TOGGLE_BUTTON(w)->active) ;
}

void widget_toggle_ecrit(NanoWidget w, int i)
{
  COUCOU ;
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w), i) ;
}

NanoWidget widget_button(NanoWidget w, const char *nom, const char *label)
{
  GtkWidget *t ;
  EPRINTF("widget_button %s %s\n", nom, label) ;
  t = gtk_button_new_with_label(label) ;
  gtk_widget_set_name(t, nom) ;
  return( (NanoWidget)t ) ;
}

NanoWidget widget_drawing(NanoWidget w, const char *nom, NanoWidget *zone)
{
  GtkWidget *sw ;

  COUCOU ;
  sw = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_set_name(GTK_WIDGET(sw), nom) ;
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),
				  GTK_POLICY_AUTOMATIC,
				  GTK_POLICY_AUTOMATIC);
  *zone = gtk_drawing_area_new() ;
  gtk_widget_set_name(GTK_WIDGET(*zone), "real_graphe") ;
  gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(sw), *zone);
  return(sw) ;
}

enum
{
   TW_VISIBLE,
   TW_RESUME,
   TW_DATA,
   TW_POINTER,
   TW_LAST
};


gboolean widget_tree_bascule_always(GtkTreeSelection *selection,
			     GtkTreeModel *ts,
			     GtkTreePath *path,
			     gboolean path_currently_selected,
			     gpointer data)
{
  return TRUE ;
}

void* treestore_get_object(GtkTreeModel *ts, GtkTreePath *path)
{
  GValue oo = {0} ;
  GtkTreeIter ti ;

  gtk_tree_model_get_iter(ts, &ti, path) ;  
  gtk_tree_model_get_value(ts, &ti, TW_POINTER, &oo) ;
  return g_value_get_pointer(&oo) ;
}

void* treestore_get_object_iter(GtkTreeModel *ts, GtkTreeIter *ti)
{
  GValue oo = {0} ;
  gtk_tree_model_get_value(ts, ti, TW_POINTER, &oo) ;
  return g_value_get_pointer(&oo) ;
}

gboolean widget_tree_bascule(GtkTreeSelection *selection,
			     GtkTreeModel *ts,
			     GtkTreePath *path,
			     gboolean path_currently_selected,
			     gpointer data)
{
  void *o ;

  o = treestore_get_object(ts, path) ;
  if ( o )
    {
      if ( !path_currently_selected )
	{
	  o_bascule_actif(o) ;
	  evaluation(o) ;
	  return FALSE ;
	}      
      return TRUE ;
    }
  return FALSE ;
}


static void widget_tree_expand(GtkTreeView *tv,
			       GtkTreeIter *ti,
			       GtkTreePath *path,
			       gpointer     user_data) 
{
  GValue oo = {0} ;
  void *o ;
  gtk_tree_model_get_value(gtk_tree_view_get_model(tv), ti, TW_POINTER, &oo) ;
  o = g_value_get_pointer(&oo) ;
  if ( o && OBJET(o)->ferme )
    {
      OBJET(o)->ferme = Faux ;
      evaluation(o) ; // Pour refaire les 'selected' qu'étaient pas visible
    }

}

void  bascule_visibilite(GtkCellRendererToggle *cell_renderer,
			 gchar                 *path,
			 gpointer               user_data)
{
  GtkTreeModel *ts = GTK_TREE_MODEL(user_data) ;
  GtkTreeIter ti ;
  GValue oo = {0} ;
  void *o ;

  gtk_tree_model_get_iter_from_string(ts, &ti, path) ;  
  gtk_tree_model_get_value(ts, &ti, TW_POINTER, &oo) ;
  o = g_value_get_pointer(&oo) ;
  if ( o )
    {
      OBJET(o)->visible = !OBJET(o)->visible ;
      // Horrible hack to not activate the object
      OBJET(o)->actif = !OBJET(o)->actif ;
    }
}


static void widget_tree_collapse(GtkTreeView *tv,
			       GtkTreeIter *ti,
			       GtkTreePath *path,
			       gpointer     user_data) 
{
  GValue oo = {0} ;
  void *o ;
  gtk_tree_model_get_value(gtk_tree_view_get_model(tv), ti, TW_POINTER, &oo) ;
  o = g_value_get_pointer(&oo) ;
  if ( o )
    OBJET(o)->ferme = Vrai ;
}

const Champ* avec_zone_editable(void *o)
{
  const void *oo ;

  COUCOU ;
  if ( o == NULL )
    return(NULL) ;
  oo = o ;
  return( u_champ_trouve_n(&oo, 0, O_Init3d|O_InitChaine) ) ;
}


#include <gdk/gdkkeysyms.h>

static gboolean text_edit(GtkWidget   *widget,
			  GdkEventKey *event,
			  gpointer     user_data)
{
  if ( event->keyval == GDK_Return  )
    {
      void *o = user_data ;
      const Champ *c = avec_zone_editable(o) ;
      if ( c )
	{
	  c_sauve_ici(o, c,gtk_entry_get_text(GTK_ENTRY(widget)));
	  evaluation(o) ; // Pour refaire les 'selected' qu'étaient pas visible
	}
      else
	ICI ;
      return TRUE ;
    }
  return FALSE;
}

static void text_editing_started (GtkCellRenderer *cell, 
				  GtkCellEditable *editable,
				  const gchar *path, GCallback data)
{
  if (GTK_IS_ENTRY(editable))
    {
      GtkEntry *entry = GTK_ENTRY (editable);
      GValue oo = {0} ;
      void *o ;
      GtkTreeIter ti ;
      GtkTreeModel *ts = (GtkTreeModel*)data ;
      gtk_tree_model_get_iter_from_string(ts, &ti, path) ;  
      gtk_tree_model_get_value(ts, &ti, TW_POINTER, &oo) ;
      o = g_value_get_pointer(&oo) ;

      if ( avec_zone_editable(o) )
	g_signal_connect(GTK_OBJECT(entry), "key-press-event", 
			 (GCallback)text_edit, o);
      else
	evaluation(o);
    }
}


static gboolean manage_button(GtkWidget      *widget,
			     GdkEventButton *event,
			     gpointer        user_data)
{
  void *o ;
  GtkTreePath *path ;
  GtkTreeViewColumn *tvc ;
  int (*fct)(NanoWidget W, NanoEvent *E, void *P) ;
  gint cell_x, cell_y ;

  gtk_tree_view_get_path_at_pos(GTK_TREE_VIEW(widget),
				event->x,
				event->y,
				&path,
				&tvc,
				&cell_x,
				&cell_y);

  // gtk_tree_view_get_cursor(GTK_TREE_VIEW(widget), &path, &tvc) ;
  if ( path == NULL )
    return FALSE ;
  o = treestore_get_object(
			   GTK_TREE_MODEL(gtk_tree_view_get_model(
								  GTK_TREE_VIEW(widget))),
			   path) ;
  fct = user_data ;
  if ( o == NULL )
    return FALSE ;

  return fct(widget, (NanoEvent*)event, o) ;
}

/*
void widget_tree_row_inserted(GtkTreeModel *tree_model,
			      GtkTreePath  *path,
			      GtkTreeIter  *iter,
			      gpointer      user_data)
{
  
  void *o ;

  o = treestore_get_object_iter(GTK_TREE_MODEL(tree_model), iter) ;
  ICI; eprintf("Insert %s\n", o ? OBJET(o)->classe->nom : "???");
  
}
*/

void parcours_tree(GtkTreeModel *tree_model, GtkTreeIter  *iter,
		   void *expected)
{
  GtkTreeIter it ;
  void *fils, *fils_gtk ;
  int i ;

  eprintf("expected %s\n",
	  expected ? OBJET(expected)->classe->nom : "NULL");

  if ( gtk_tree_model_iter_children(tree_model, &it, iter) == 0 )
    {
      if ( FILS(expected,0) )
	{	  
	  eprintf("To delete : %s\n",OBJET(FILS(expected,0))->classe->nom);
	  u_enleve_fils(expected, 0) ;
	}
      return ;
    }
  i = 0 ;
  do
    {
      fils = FILS(expected,i) ;
      fils_gtk = treestore_get_object_iter(GTK_TREE_MODEL(tree_model),&it);
      eprintf("%s == %s ===> %d\n",
	      fils_gtk ? OBJET(fils_gtk)->classe->nom : "NULL",
	      fils ? OBJET(fils)->classe->nom : "NULL",
	      fils_gtk == fils) ;


      if ( fils_gtk != fils )
	{
	  if ( fils_gtk == FILS(expected,i+1) )
	    {
	      eprintf("To delete : %s\n",OBJET(fils)->classe->nom);
	      u_enleve_fils(expected, i) ;
	      fils = FILS(expected,i) ;	// No need for i++ because deleted
	    }
	  else
	    {
	      eprintf("To insert : %s\n",OBJET(fils_gtk)->classe->nom);
	      u_ajoute_fils(expected, fils_gtk, i);
	      fils = fils_gtk ;
	    }
	}

      parcours_tree(tree_model, &it, fils) ;
      i++ ;
    }
  while( gtk_tree_model_iter_next(tree_model, &it) ) ;

  if ( FILS(expected,i) )
    {
      eprintf("To delete : %s\n",OBJET(FILS(expected,i))->classe->nom);	  
      u_enleve_fils(expected, i) ;
    }
}

#include "u_modele.h"

void widget_tree_row_deleted(GtkTreeModel *tree_model,
			      GtkTreePath  *path,
			      GtkTreeIter  *iter,
			      gpointer      user_data)
{
  void *racine, *fils_gtk ;
  GtkTreeIter it ;
  Liste **modele ;
  int i_f, ok ;
  void *f ;

  gtk_tree_model_iter_children(tree_model, &it, NULL) ;
  racine = u_modele(treestore_get_object_iter(GTK_TREE_MODEL(tree_model),&it));
  modele = contenu_modele_ptr(racine) ;
  ok = 1 ;
  for(i_f=0; ok; i_f++)
    {
      eprintf("%d\n",i_f);
      f = l_element(*modele, i_f) ;
      fils_gtk = treestore_get_object_iter(GTK_TREE_MODEL(tree_model),&it);
      if ( fils_gtk != f )
	{
	  if ( fils_gtk == l_element(*modele, i_f+1) )
	    {
	      eprintf("*To delete : %s\n",OBJET(f)->classe->nom);
	      l_enleve_indice(modele, i_f) ;
	      l_enleve_indice(&OBJET(f)->parent,
			      l_trouve_indice(OBJET(f)->parent,racine));
	      f = l_element(*modele, i_f) ;
	    }
	  else
	    {
	      eprintf("*To insert : %s\n",OBJET(fils_gtk)->classe->nom);
	      l_insere(modele, fils_gtk, i_f);
	      l_ajoute(&OBJET(fils_gtk)->parent, racine) ;
	      f = fils_gtk ;
	    }
	}
      parcours_tree(tree_model, &it, f) ;
      ok = gtk_tree_model_iter_next(tree_model, &it) ;
    }
  f = l_element(*modele,i_f) ;
  if ( f )
    {
      eprintf("*To delete : %s\n",OBJET(f)->classe->nom);	  
      l_enleve_indice(modele, i_f) ; // XXX Enleve plusieurs...
      l_enleve_indice(&OBJET(f)->parent,
		      l_trouve_indice(OBJET(f)->parent,racine));
    }


  evaluation(racine) ;
}


NanoWidget widget_tree(NanoWidget w, const Interface_widget *iw
		       ,POINTEUR_CALLBACK_EVENT(fct))
{
  GtkWidget *t, *sw ;
  GtkTreeStore *tree_model ;
  GtkCellRenderer *renderer;
  GtkTreeViewColumn *column;

  COUCOU ;
  tree_model = gtk_tree_store_new(TW_LAST,
				  G_TYPE_BOOLEAN,
				  G_TYPE_STRING,
				  G_TYPE_STRING,
				  G_TYPE_POINTER
				  ) ;
  t = gtk_tree_view_new_with_model(GTK_TREE_MODEL(tree_model));
  gtk_tree_view_set_enable_tree_lines(GTK_TREE_VIEW(t), TRUE) ;
  gtk_tree_view_set_enable_search(GTK_TREE_VIEW(t),FALSE) ;
  gtk_tree_view_set_reorderable(GTK_TREE_VIEW(t),TRUE) ;
  gtk_tree_selection_set_mode(gtk_tree_view_get_selection(GTK_TREE_VIEW(t)),
			      GTK_SELECTION_MULTIPLE) ;

  gtk_tree_selection_set_select_function
    (gtk_tree_view_get_selection(GTK_TREE_VIEW(t)),
     widget_tree_bascule, NULL, NULL) ;


  g_signal_connect(G_OBJECT(t), "row-expanded",
		   (GCallback)widget_tree_expand, NULL);
  g_signal_connect(G_OBJECT(t), "row-collapsed",
		   (GCallback)widget_tree_collapse, NULL);
  

  gtk_widget_set_name(GTK_WIDGET(t), iw->ch->nom_champ) ;

  renderer = gtk_cell_renderer_toggle_new();
  g_signal_connect(G_OBJECT(renderer), "toggled",
		   G_CALLBACK(bascule_visibilite), tree_model);

  column = gtk_tree_view_column_new_with_attributes("V",
                                                   renderer,
                                                   "active", TW_VISIBLE,
                                                   NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW(t), column);


  renderer = gtk_cell_renderer_text_new();
  column = gtk_tree_view_column_new_with_attributes("Information",
                                                   renderer,
                                                   "text", TW_RESUME,
                                                   NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW(t), column);

  renderer = gtk_cell_renderer_text_new();
  g_object_set(renderer,"editable", TRUE, NULL);
  g_signal_connect(GTK_OBJECT(renderer), 
		   "editing-started", (GCallback)text_editing_started,
		   tree_model) ;

  column = gtk_tree_view_column_new_with_attributes("Valeur",
						    renderer,
						    "text", TW_DATA,
                                                   NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW(t), column);


  
  sw = gtk_scrolled_window_new(NULL, NULL);

  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),
				  GTK_POLICY_AUTOMATIC,
				  GTK_POLICY_AUTOMATIC);


  gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(sw), t);

  g_signal_connect(G_OBJECT(t), "button-press-event",
		   (GCallback)manage_button, fct);
 
  gtk_widget_show_all(GTK_WIDGET(sw)) ;


  return sw ;
}

const char* widget_tree_lit(NanoWidget w)
{
  /*
   * Il y a un blanc pour que l'arbre soit mise à jour même quand il est vide
   */
  return(" ") ; /* NE PAS CHANGER : toujours être mise à jour car incomplet */
}

GtkWidget *widget_fils_n(GtkWidget *w, int i)
{
  GList *g ;
  GtkWidget *f ;
  
  COUCOU ;
  g = gtk_container_get_children(GTK_CONTAINER(w)) ;
  if ( g )
    {
      f = g_list_nth_data(g, i) ;
      g_list_free(g);      
      return( f ) ;
    }
  else
    return(NULL) ;
}

GtkWidget *widget_premier_fils(GtkWidget *w)
{
  return widget_fils_n(w, 0) ;
}

GtkWidget *widget_deuxieme_fils(GtkWidget *w)
{
  return widget_fils_n(w, 1) ;
}

GtkWidget *widget_troisieme_fils(GtkWidget *w)
{
  return widget_fils_n(w, 2) ;
}

void widget_annule_flag3(GtkWidget *w, void *unused)
{
  COUCOU ;
  o_annule_flag3(widget_vers_objet2(w)) ;
}


void set_object_informations(void *o, GtkTreeStore *ts, GtkTreeIter *ti,
			     GtkTreeView *tv)
{
  const char *infos ;
  static Chaine s ;
  const char *edite ;

  COUCOU ;
  if ( o )
    {
      s_efface(&s) ;
      if (  l_longueur(OBJET(o)->parent) != 1 && !OBJET(o)->flag3)
	s_printf(&s, "[%d] ", INDEX_OBJET(OBJET(o))) ;	  
      if ( avec_zone_editable(o) )
	{
	  if ( OBJET(o)->classe->informations )
	    {
	      infos = (*OBJET(o)->classe->informations)(o) ;
	    }
	  else
	    infos = NULL ;
	}
      else
	infos = o_informations(o) ;
      if ( infos && infos[0] )
	s_printf(&s, "%s (%s)", OBJET(o)->classe->nom, infos) ;
      else
	s_printf(&s, "%s", OBJET(o)->classe->nom) ;
      if ( OBJET(o)->flag3 )
	s_printf(&s, " VOIR [%d]", INDEX_OBJET(OBJET(o))) ;
      else
	OBJET(o)->flag3 = Vrai ;

      if ( OBJET(o)->invalide )
	s_printf(&s, "**INVALIDE**") ;
      
      if ( ACTIF(o) )
	s_printf(&s, " ACTIF=%d", OBJET(o)->actif) ;
      

      /* Affiche parents */
      if ( 0 )
	POUR_TOUS_LES_PARENTS_(OBJET(o), p,
			       {s_printf(&s, " %x", p);} ) ;

      if ( avec_zone_editable(o) )
	edite = c_recupere_ici_sans_copie(o,avec_zone_editable(o));
      else
	edite = "" ;
						   

      gtk_tree_store_set (ts, ti,
			  TW_VISIBLE, OBJET(o)->visible,
			  TW_RESUME , s_char(&s),
			  TW_DATA   , edite,
			  TW_POINTER, o,
			  -1);
      /*
      if ( avec_zone_editable(o) )
	widget_texte_ecrit(widget_troisieme_fils(widget_premier_fils(w)), c_recupere_ici_sans_copie(o,avec_zone_editable(o))) ;
      */
    }
  else
      gtk_tree_store_set (ts, ti,
			  TW_VISIBLE, 0,
			  TW_RESUME, "DETRUIT",
			  -1);
}

PROTOTYPE_CALLBACK_ACTION(entree_validee, w, p)
{
  const Champ *ch = p ;

  COUCOU ;
  c_sauve_ici( widget_vers_objet2(w), ch, widget_texte_lit(w)) ;
  evaluation(widget_vers_objet2(w)) ;
}

PROTOTYPE_CALLBACK_ACTION(change_visibilite, w, p)
{

  COUCOU ;
  OBJET(p)->visible = gtk_toggle_button_get_active(w) ;
  NOTE_UN_CHAMP_CHANGE(u_modele(p)) ;
  evaluation(p) ;
}




const char* widget_tree_ecrit_(GtkTreeStore *ts, const char *fils,
			       const Champ *ch, void *objet,
			       void *pere,
			       GtkTreeIter *parent,
			       GtkTreeView *tv)
{
  void *o ;
  GtkTreeIter w ;
  int opened = 0 ;

  COUCOU ;
  for(;;)
    {
      if ( *fils == '\0' )
	return(fils) ;
      if ( *fils == ')' )
	return(fils+1) ;
      o = (void*)strtoul(fils, (char**)&fils, 10) ;
      if ( o )
	{
	  fils++ ;
	  if ( *fils == '*' )
	    fils++ ;
	}

      gtk_tree_store_append(ts, &w, parent); 

      set_object_informations(o, ts, &w, tv) ;

      if ( !opened && parent && !OBJET(pere)->ferme )
	{
	  opened = 1 ;
	  gtk_tree_view_expand_row(tv,
				   gtk_tree_model_get_path(GTK_TREE_MODEL(ts),
							   parent),  
				   FALSE) ;
	}

      if ( o )
	{
	  EPRINTF("item = %s\n", o_informations(o)) ;
	  o_annule_flag3(o) ;
	  
	  if ( *fils != ')' )
	    {
	      fils = widget_tree_ecrit_(ts, fils, ch, objet, o, &w, tv);
	    }
	  else
	    fils++ ;

	  if ( ACTIF(o) )
	    gtk_tree_selection_select_iter(gtk_tree_view_get_selection(tv),
					   &w);
	}
      else
	fils++ ; /* Le blanc après le 0 */

    }

}

void widget_deselectionne(GtkWidget *w, void *o)
{
}


void widget_tree_ecrit(const Interface_widget* iw)
{      
  GtkTreeStore *ts = GTK_TREE_STORE(gtk_tree_view_get_model(iw->w)) ;

  /*
  g_signal_handlers_disconnect_by_func(G_OBJECT(ts),
				       (GCallback)widget_tree_row_inserted,
				       NULL);
  */
  g_signal_handlers_disconnect_by_func(G_OBJECT(ts),
				       (GCallback)widget_tree_row_deleted,
				       NULL);
  gtk_tree_selection_set_select_function
    (gtk_tree_view_get_selection(GTK_TREE_VIEW(iw->w)),
     widget_tree_bascule_always, NULL, NULL) ;
  gtk_tree_store_clear(ts) ;

  widget_tree_ecrit_(ts, iw->texte, iw->ch, iw->objet, iw->objet, NULL, iw->w);

  gtk_tree_selection_set_select_function
    (gtk_tree_view_get_selection(GTK_TREE_VIEW(iw->w)),
     widget_tree_bascule, NULL, NULL) ;
  /*
  g_signal_connect(G_OBJECT(ts), "row-inserted",
		   (GCallback)widget_tree_row_inserted, NULL);
  */
  g_signal_connect(G_OBJECT(ts), "row-deleted",
		   (GCallback)widget_tree_row_deleted, NULL);

}


void widget_scrollbar_ecrit(NanoWidget w, float min, float max, float value)
{
  GtkAdjustment *a ;
  EPRINTF("widget_scrollbar_ecrit ajustement=%p\n", gtk_range_get_adjustment(GTK_RANGE(w))) ;
  a = gtk_range_get_adjustment(GTK_RANGE(w)) ;
  EPRINTF("set clamp (%g %g)\n", min, max) ;
  /*  gtk_adjustment_clamp_page(a, min, max) ; BIZARD */
  a->lower = min ;
  a->page_size = (max-min)/100 ;
  a->upper = max+a->page_size ;
  gtk_adjustment_set_value(a, value) ;
  EPRINTF("widget_scrollbar_ecrit fin\n") ;
}

void widget_scrollbar_infos(NanoWidget w, float *min, float *max, float *value)
{
  GtkAdjustment *a ;
  EPRINTF("widget_scrollbar_infos ajustment=%p\n", w) ;
  if ( !GTK_IS_ADJUSTMENT(w) )
    EXIT ;
  a = w ; /* gtk_range_get_adjustment(GTK_RANGE(w)) ; */
  *min = a->lower ;
  *max = a->upper-a->page_size ;
  *value = a->value ;
}

void widget_add_event_scrollbar(NanoWidget w
				, POINTEUR_CALLBACK_ACTION(fct)
				, const Champ *c)
{
  GtkAdjustment *a ;
  COUCOU ;
  a = gtk_range_get_adjustment(GTK_RANGE(w)) ;
  g_signal_connect(G_OBJECT(a), "value-changed", (GCallback)fct, (gpointer)c);
}

NanoWidget widget_hscrollbar(NanoWidget w, const char *nom)
{
  GtkWidget *t ;

  COUCOU ;
  t = gtk_hscrollbar_new(NULL) ;
  gtk_widget_set_name(t, nom) ;
  widget_marque_objet(gtk_range_get_adjustment(GTK_RANGE(t)), t) ;
  return( (NanoWidget)t ) ;
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
  COUCOU ;
  if ( !  gdk_gl_drawable_make_current(GDK_GL_DRAWABLE(gtk_widget_get_gl_drawable (w)),
				       (GdkGLContext*)c) )
    {
      eprintf("widget_current_gl\n") ;
      EXIT ;
    }
}
void widget_swap_gl(NanoWidget w)
{
  EPRINTF("widget_swap_gl\n") ;
  gdk_gl_drawable_swap_buffers(GDK_GL_DRAWABLE(gtk_widget_get_gl_drawable (w))) ;
}

static GdkGLConfig *glconfig = NULL ;



NanoWidget widget_gl(NanoWidget w, const char *nom)
{
  GtkWidget *f ;

  static int attr[] = { GDK_GL_RGBA
			, GDK_GL_DOUBLEBUFFER
			, GDK_GL_STENCIL_SIZE, 1
			, GDK_GL_DEPTH_SIZE, 16
			, GDK_GL_ATTRIB_LIST_NONE
  } ;

  COUCOU ;
  if ( glconfig == NULL )
    {
      int argc = 1 ;
      char *argv[] = {"nanoedit", NULL} ;
      char **ptr_argv = argv ;
      gdk_gl_init(&argc, &ptr_argv);
      glconfig = gdk_gl_config_new(attr);

      printf("glconfig=%p\n", glconfig) ;
    }

  f = gtk_drawing_area_new() ;
  gtk_widget_set_gl_capability(f, glconfig, NULL, 1, 0);

  gtk_widget_set_name( GTK_WIDGET(f), nom) ;
  gtk_widget_set_events(GTK_WIDGET(f)
			,GDK_BUTTON_MOTION_MASK 
			|GDK_POINTER_MOTION_MASK 
			|GDK_BUTTON_PRESS_MASK 
			|GDK_BUTTON_RELEASE_MASK
			|GDK_ENTER_NOTIFY_MASK
			|GDK_KEY_PRESS_MASK
			|GDK_KEY_RELEASE_MASK
			|GDK_EXPOSURE_MASK
			|GDK_SCROLL_MASK 
			) ;

//  GTK_WIDGET_SET_FLAGS(GTK_WIDGET(f), GTK_CAN_FOCUS);
  gtk_widget_set_can_focus(GTK_WIDGET(f),TRUE);
  
  g_signal_connect( G_OBJECT(f), "enter-notify-event",
		    (GCallback)gtk_widget_grab_focus, 
		    NULL);  
  return((NanoWidget)f) ;
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
  return (GLXContext)gtk_widget_get_gl_context (w) ;
}

void widget_unmanage_freres(NanoWidget *w, int i)
{
}

static Liste *globale_widget_detruit = NULL ;

void delete_popup(GtkWidget *w, gpointer data)
{
  EPRINTF("----------------------------- Destruction de %p (%d)\n", w,
	  l_longueur(globale_widget_detruit)) ;
  /*  l_ajoute(&globale_widget_detruit, w) ; */
  /*  gtk_widget_hide(w) ; */
  eprintf("ICI\n") ;
}

void widget_item_detruit(NanoWidget w)
{

  COUCOU ;
  /*  if ( l_enleve(&globale_widget_detruit, w) == 0 ) */
 
  /*
   * Necessaire de détruire l'image ?
   */
  /*
  GdkImage *image ;
  GdkBitmap *mask ;

  gtk_image_get(w, &image, &mask) ;
  if ( image )
    {
      gdk_image_destroy(image) ;
    }
  */
  gtk_object_destroy(GTK_OBJECT(w)) ;
}


NanoWidget widget_popup_menu(NanoWidget w, const char *titre)
{   
  GtkWidget *ww, *to ;
  ww = gtk_menu_new() ;

  to = gtk_tearoff_menu_item_new() ;
  if ( titre == NULL || strstr(titre, "contextu") == NULL )
    {
      // Pas un menus contextuel donc détachable
      gtk_widget_show(GTK_WIDGET(to)) ;
    }
  else
    eprintf("titre=%s\n", titre) ;
  gtk_menu_shell_append(GTK_MENU_SHELL(ww), to) ;


  /*  gtk_signal_connect(GTK_OBJECT (ww), "cancel", delete_popup, NULL) ; */

  /*
   * Pour que les menu ne s'auto détruisent pas tout seul
   * (pour être comme Motif)
   */
  g_object_ref(GTK_OBJECT(ww)) ;

  if ( titre )
    gtk_menu_set_title(GTK_MENU(ww),
		       titre) ;

  EPRINTF("Fin création menu %p\n", ww) ;

  return(ww) ;
}
NanoWidget widget_pulldown_menu(NanoWidget w)
{   
  COUCOU ;
  return(widget_popup_menu(w, NULL)) ;
}
void widget_attache_sous_menu(NanoWidget bouton, NanoWidget menu)
{
  GtkWidget *sm ;
  Booleen visible ;

  COUCOU ;
  if ( menu != NULL )
    {
      gtk_menu_item_set_submenu(bouton, menu) ;
      g_object_set_data(G_OBJECT(bouton), "sous-menu", menu) ;
    }
  else
    {
      sm = g_object_get_data(G_OBJECT(bouton), "sous-menu") ;
      if ( sm )
	visible = gtk_widget_get_mapped(sm) ;
      else
	visible = 0 ;
      gtk_menu_item_set_submenu(bouton, NULL) ;
      if ( visible )
	{
	  EPRINTF("attache NULL fait show\n") ;
	  /*	  widget_dump(sm, NULL) ; */
	  gtk_widget_realize(sm) ;
	  gtk_widget_map(sm) ;
	  gtk_widget_show_all(sm) ;
	}
    }
}
void widget_add_event_activate_cascade(NanoWidget w
				       , POINTEUR_CALLBACK_ACTION(fct)
				       , void *c)
{
  COUCOU ;
  g_signal_connect (G_OBJECT (w), "select",(GCallback)fct, (void*)c);
}
void widget_add_event_menu_button(NanoWidget w
				  , POINTEUR_CALLBACK_ACTION(fct)
				  , const Champ *c)
{
  COUCOU ;
  g_signal_connect (G_OBJECT (w), "activate", (GCallback)fct, (void*)c);
}
NanoWidget widget_bouton_cascade(NanoWidget w
				 , const char *nom, const char *label)
{
  GtkWidget *lab ;

  COUCOU ;
  lab = gtk_menu_item_new_with_label(label) ;
  gtk_widget_set_name(lab, nom) ;

  return(lab) ;
}

NanoWidget widget_button_for_menu(NanoWidget w
				 , const char *nom, const char *label)
{
  GtkWidget *ww, *f ;

  COUCOU ;
  ww = widget_bouton_cascade(w, nom, label) ;

  f = gtk_frame_new(NULL) ;
  gtk_container_add(GTK_CONTAINER(f), ww) ;

  gtk_widget_show(ww) ;

  return( f ) ;
}
 
NanoWidget widget_button_menu(NanoWidget w, const char *nom, const char *label)
{
  COUCOU ;
  return( widget_bouton_cascade(w, nom, label) ) ;
}

NanoWidget widget_image(int lar, int hau)
{
  GdkImage *image ; 
  COUCOU ;
  
  image = gdk_image_new(GDK_IMAGE_NORMAL
			,gdk_visual_get_system()
			, lar
			, hau
			) ;
  

  return( gtk_image_new_from_image(image, NULL) ) ;
}


void widget_image_update_open_file(GtkWidget *w, int lar, int hau, FILE *f)
{
  GdkBitmap *mask ;
  GdkImage *image ; 
  guint32 pixel ;
  int i, j ;
  COUCOU ;

  gtk_image_get_image(GTK_IMAGE(w), &image, &mask) ;

  for(j=0; j<hau; j++)
    for(i=0; i<lar; i++)
      {
	pixel = fgetc(f) ;
	pixel = (pixel<<8) | fgetc(f) ;
	pixel = (pixel<<8) | fgetc(f) ;
	gdk_image_put_pixel(image, i, j, pixel) ;
      }
}

NanoWidget widget_button_menu_with_icon(NanoWidget w, const char *nom, const char *label, const char *icon_file)
{
  GtkWidget *item, *icone ;
  int lar, hau ;
  FILE *f ;
  int ok ;

  COUCOU ;
  item = gtk_image_menu_item_new_with_label(label) ;
  gtk_widget_set_name(item, nom) ;

  f = fopen(icon_file, "r") ;
  if ( f )
    {
      ok = 1 ;
      if ( fgetc(f) != 'P' )
	ok = 0 ;
      if ( fgetc(f) != '6' )
	ok = 0 ;
      if ( fgetc(f) != '\n' )
	ok = 0 ;
      fscanf(f, "%d%d", &lar, &hau) ;
      if ( lar * hau > 1000000 || lar<=0 || hau<=0 )
	ok = 0 ;
      if ( lar>10000 || hau>10000 )
	{
	  ICI ;
	  eprintf("Bug à trouver\n") ;
	  ok = 0 ;
	}
      if ( ok )
	{
	  while( fgetc(f) != '\n' )
	    {
	    }
	  while( fgetc(f) != '\n' )
	    {
	    }
	  icone = widget_image(lar, hau) ;
	  widget_image_update_open_file(icone, lar, hau, f) ;
	  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item), icone) ;
	}
      fclose(f) ;
    }

  gtk_widget_show(item) ;
  
  return(item) ;
}

void widget_sensible(NanoWidget w, int i)
{
  COUCOU ;
  gtk_widget_set_sensitive( GTK_WIDGET(w), i) ;
}
Booleen widget_visible(NanoWidget w)
{
  COUCOU ;
 if ( l_trouve(globale_widget_detruit, w) )
   return(0) ;
 else
   return( gtk_widget_get_mapped(w) ) ;
}
void widget_termine_menu(NanoWidget w, Liste *lw)
{
  Liste *inverse ;
  COUCOU ;

  inverse = l_inverse(lw) ;
  POUR(GtkWidget*, f, inverse,
       gtk_widget_show(GTK_WIDGET(f)) ;
       gtk_menu_shell_append(GTK_MENU_SHELL(w), f) ;
       ) ;
  l_detruit(&inverse) ;
}
Liste *widget_fils(NanoWidget w)
{
  GList *fils, *i ;
  Liste *lf ;
  COUCOU ;

  fils = gtk_container_get_children(GTK_CONTAINER(w)) ;
  lf = NULL ;

  for(i=g_list_first(fils); i; i = g_list_next(i) )
    l_ajoute(&lf, i->data) ;
  g_list_free(fils);

  return(lf) ;
}
Booleen widget_est_tearoff(NanoWidget w)
{
  COUCOU ;
  return( GTK_IS_TEAROFF_MENU_ITEM(w) ) ;
}
void widget_detruit_menu(NanoWidget menu)
{
  COUCOU ;
  gtk_object_destroy(GTK_OBJECT(menu)) ;
}
void widget_enleve_event_activate_cascade(NanoWidget w
				       , POINTEUR_CALLBACK_ACTION(fct)
				       , void *c)
{
  EPRINTF("widget_enleve_event_activate_cascade\n") ;
  g_signal_handlers_disconnect_by_func(G_OBJECT (w),(GCallback)fct, (void*)c);
}
void widget_label_ecrit(NanoWidget w, const char *t)
{
  COUCOU ;
  gtk_label_set_text( GTK_LABEL(w), t) ;
}
const char* widget_label_lit(NanoWidget w)
{
  COUCOU ;
  return gtk_label_get_text(GTK_LABEL(w));
}
void widget_positionne_menu(NanoWidget w, NanoEvent *event)
{
  COUCOU ;
  gtk_menu_popup(GTK_MENU(w),
		 NULL,
		 NULL,
		 NULL,
		 NULL,
		 ((GdkEventButton*)event)->button,
		 ((GdkEventButton*)event)->time
		 ) ;
}

const char* widget_button_lit(NanoWidget w)
{
  COUCOU ;
  return( widget_label_lit(cherche_label(w)) ) ;
}
void widget_button_ecrit(NanoWidget w, const char*s)
{
  COUCOU ;
  widget_label_ecrit(cherche_label(w), s) ;
}

void widget_flush(NanoWidget w)
{
  /*  gdk_flush() ; */
}
/*
static GdkFilterReturn oublier_evenement(GdkXEvent *xevent,
					 GdkEvent *event,
					 gpointer  data)
{
  return(GDK_FILTER_REMOVE) ;
}
*/
void widget_ne_recoit_plus_evenement(NanoWidget w)
{
  /*
  if ( w )
  gdk_window_set_events   (GTK_WIDGET(w)->window, 0) ;
  */
  /*
  if ( w )
    gdk_window_add_filter(GTK_WIDGET(w)->window, oublier_evenement, NULL) ;
  */
}

void widget_recoit_a_nouveau_evenement(NanoWidget w)
{
  /*
  GdkEvent e ;
  e.type = GDK_BUTTON_PRESS ;
  while( gtk_widget_event(GTK_WIDGET(w), &e) ) ;
  */
  /*
  if ( w )
    gdk_window_remove_filter(GTK_WIDGET(w)->window, oublier_evenement, NULL) ;
  */
}

int widget_ajoute_timeout(int interval, Booleen (*fct)(void*), void *data)
{
  COUCOU ;
  return g_timeout_add(interval, (GSourceFunc)fct, data) ;
}

void widget_enleve_timeout(int id)
{
  COUCOU ;
  g_source_remove( (gint)id ) ;
}
