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
#include <ctype.h>
#include <X11/IntrinsicP.h>
#include <Xm/Xm.h>
#include <Xm/Protocols.h>
#include <Xm/MenuShell.h>
#include <Xm/Label.h>
#include <Xm/ToggleB.h>
#include <Xm/Form.h>
#include <Xm/Text.h>
#include <Xm/RowColumn.h>
#include <Xm/PushB.h>
#include <Xm/ScrolledW.h>
#include <Xm/DrawingA.h>
#include <Xm/ScrollBar.h>
#include <GL/GLwMDrawA.h>	/* Pour GLXContext */
/* #include <GL/MesaDrawingAreaP.h> */
#include <Xm/PrimitiveP.h> /* Pour le suivant */
#include <GL/GLwMDrawAP.h>
#include <Xm/CascadeB.h>

#if 0
#define EPRINTF eprintf
#else
#define EPRINTF if (0) eprintf
#endif

NanoWidget widget_parent(NanoWidget w)
{
  return( (NanoWidget) XtParent( (Widget)w ) ) ;
}
Booleen widget_fenetre(NanoWidget w)
{
  return(XtIsVendorShell((Widget)w) || XmIsMenuShell((Widget)w)) ;
}

/*
 * The following similar code fragment is from Dave Mink
 * (mink@cadcam.pms.ford.com):
 */

void widget_set_close_callback(NanoWidget shell
			       , const Champ* c
			       , POINTEUR_CALLBACK(callback))
{
  /* get window manager delete protocol atom */
  Atom deletewin_protocol = XmInternAtom(
					 XtDisplay((Widget)shell),
					 "WM_DELETE_WINDOW",
					 True
					 );
  /* turn off default delete response */
  XtVaSetValues((Widget)shell, XmNdeleteResponse, XmDO_NOTHING, NULL);
  /* add callback for window manager delete protocol */
  XmAddWMProtocolCallback((Widget)shell
			  , deletewin_protocol
			  , (XtCallbackProc)callback
			  , (XtPointer)c
			  );
}

NanoInputId widget_ajoute_fichier(NanoWidget w
				  , int fildes
				  , void *objet
				  , void (*fct)(void *obj)
				  )
{
  return((NanoInputId)XtAppAddInput(XtWidgetToApplicationContext(w)
				    , fildes
				    , (XtPointer)(XtInputReadMask)
				    , (XtInputCallbackProc)fct
				    , objet
				    )
	 ) ;
}

void widget_enleve_fichier(NanoInputId id)
{
  XtRemoveInput( (XtInputId)id ) ;
}

NanoWidget widget_trouve_fils(NanoWidget w, const char *nom)
{
  Buffer buf ;
  sprintf(buf, "*%s", nom) ;
  return( (NanoWidget)XtNameToWidget( (Widget)w, buf ) ) ;
}

static void *widget_vers_objet_haut(NanoWidget w)
{
  void *o ;

  o = NULL ;

  for( ; w!=NULL && o==NULL ; w = widget_parent(w))
    XtVaGetValues((Widget)w, XmNuserData, &o, NULL) ;

  return(o) ;
}

void *widget_vers_objet_bas(NanoWidget w)
{
  void *o ;
  Liste *lw ;

  lw = widget_fils(w) ;
  o = NULL ;

  POUR(NanoWidget, f, lw,
       {
	 XtVaGetValues((Widget)f, XmNuserData, &o, NULL) ;
	 if ( o )
	   break ;
       }
       ) ;

  if ( o == NULL )
    {
       POUR(NanoWidget, f, lw,
	    {
	      o = widget_vers_objet_bas(f) ;
	      if ( o )
		break ;
	    }
	    ) ;
    }

  l_detruit(&lw) ;
  return(o) ;
}

void *widget_vers_objet(NanoWidget w)
{
  void *o ;

  o = widget_vers_objet_haut(w) ;
  if ( o )
    return(o) ;

  return( widget_vers_objet_bas(w) ) ;
}

void widget_marque_objet(NanoWidget w, void *o)
{
  XtVaSetValues((Widget)w, XmNuserData, o, NULL) ;
}

const char *widget_nom(NanoWidget w)
{
  if ( w )
    return( XtName((Widget)w) ) ;
  else
    return( "(null)" ) ;
}

void widget_add_event_button_press(NanoWidget w
				   , POINTEUR_CALLBACK_EVENT(fct)
				   , const Champ *c)
{
  XtAddEventHandler((Widget)w
		    , ButtonPressMask
		    , False
		    , (XtEventHandler)fct
		    , (XtPointer)c
		       );
}

void widget_add_event_button_release(NanoWidget w
				   , POINTEUR_CALLBACK_EVENT(fct)
				   , const Champ *c)
{
  XtAddEventHandler((Widget)w
		    , ButtonReleaseMask
		    , False
		    , (XtEventHandler)fct
		    , (XtPointer)c);
}

struct bidon
{
   void (*map)(NanoWidget, void*, NanoEvent *, char *valeur_de_retour) ;
   void (*unmap)(NanoWidget, void*, NanoEvent *, char *valeur_de_retour) ;
} ;

static PROTOTYPE_CALLBACK_EVENT(filtre_map, w, c, event)
{
  if ( ((XAnyEvent*)event)->type == UnmapNotify )
    (*((struct bidon*)c)->map)
      (w, NULL, event, valeur_de_retour) ;

  if ( ((XAnyEvent*)event)->type == UnmapNotify )
      (*((struct bidon*)c)->unmap)
	(w, NULL, event, valeur_de_retour) ;
}


void widget_add_event_map_fenetre(NanoWidget w
			  , POINTEUR_CALLBACK_EVENT(map)
			  , POINTEUR_CALLBACK_EVENT(unmap)
				  )
{
  struct bidon *b ;

  ALLOUER(b,1) ;
  b->map = map ;
  b->unmap = unmap ;

  XtAddEventHandler((Widget)w
		    , ExposureMask
		    , False
		    , (XtEventHandler)filtre_map
		    , (XtPointer)b);
}

void widget_arrete_emission_key_press(NanoWidget w)
{
}

void widget_arrete_emission_button_press(NanoWidget w)
{
}

void widget_add_event_key_press(NanoWidget w
				, POINTEUR_CALLBACK_EVENT(fct)
				, const Champ *c)
{
  XtAddEventHandler((Widget)w
		    , KeyPressMask
		    , False
		    , (XtEventHandler)fct
		    , (XtPointer)c);
}

void widget_add_event_expose(NanoWidget w
			     , POINTEUR_CALLBACK(fct)
			     , const Champ *c)
{
  if ( XmIsDrawingArea((Widget)w) )
    XtAddCallback((Widget)w, XmNexposeCallback
		  , (XtCallbackProc)fct, (XtPointer)c) ;
  else
    XtAddCallback((Widget)w, GLwNexposeCallback
		  , (XtCallbackProc)fct, (XtPointer)c) ;
}

void widget_add_event_resize(NanoWidget w
			     , POINTEUR_CALLBACK(fct)
			     , const Champ *c)
{
  XtAddCallback((Widget)w, GLwNresizeCallback
		, (XtCallbackProc)fct, (XtPointer)c) ;
}

void widget_add_event_motion(NanoWidget w
			     , POINTEUR_CALLBACK_EVENT(fct)
			     , const Champ *c)
{
  XtAddEventHandler((Widget)w, ButtonMotionMask, False
		    , (XtEventHandler)fct, (XtPointer)c);
}

void widget_add_event_repositionnement(NanoWidget w
			     , POINTEUR_CALLBACK_EVENT(fct))
{
  XtAddEventHandler((Widget)w, SubstructureNotifyMask, False
		    , (XtEventHandler)fct, NULL);
}

void widget_enleve_event_repositionnement(NanoWidget w
			     , POINTEUR_CALLBACK_EVENT(fct))
{
  XtRemoveEventHandler((Widget)w, SubstructureNotifyMask, False
		    , (XtEventHandler)fct, NULL);
}

void widget_enleve_event_button_release(NanoWidget w
			     , POINTEUR_CALLBACK_EVENT(fct), const Champ *c)
{
  XtRemoveEventHandler((Widget)w, ButtonReleaseMask, False
		    , (XtEventHandler)fct, c);
}

void widget_enleve_event_motion(NanoWidget w
				, POINTEUR_CALLBACK_EVENT(fct)
				, const Champ *c)
{
  XtRemoveEventHandler((Widget)w, ButtonMotionMask, False
		       ,(XtEventHandler)fct , (XtPointer)c);
}

void widget_add_event_activate(NanoWidget w
			       , POINTEUR_CALLBACK_ACTION(fct)
			       , const Champ *c)
{
  XtAddCallback((Widget)w, XmNactivateCallback
		, (XtCallbackProc)fct, (XtPointer)c) ;
}

void widget_add_event_menu_button(NanoWidget w
			       , POINTEUR_CALLBACK_ACTION(fct)
			       , const Champ *c)
{
  widget_add_event_activate(w, fct, c) ;
}

void widget_add_event_activate_cascade(NanoWidget w
				       , POINTEUR_CALLBACK_ACTION(fct)
				       , void *c)
{
  XtAddCallback((Widget)w, XmNcascadingCallback
		, (XtCallbackProc)fct, (XtPointer)c) ;
}
void widget_enleve_event_activate_cascade(NanoWidget w
				       , POINTEUR_CALLBACK_ACTION(fct)
				       , void *c)
{
  XtRemoveCallback((Widget)w, XmNcascadingCallback
		   , (XtCallbackProc)fct, (XtPointer)c) ;
}
void widget_enleve_event_unmap(NanoWidget w
				       , POINTEUR_CALLBACK_ACTION(fct)
				       , void *c)
{
  XtRemoveCallback((Widget)w, XmNunmapCallback
		   , (XtCallbackProc)fct, (XtPointer)c) ;
}

void widget_add_event_destroy(NanoWidget w
			      , POINTEUR_CALLBACK_ACTION(fct)
			      , void *c)
{
  XtAddCallback((Widget)w, XmNdestroyCallback
		, (XtCallbackProc)fct, (XtPointer)c) ;
}


void widget_resize(NanoWidget w, int lar, int hau)
{
  EPRINTF("resize\n") ;
  if ( lar == 0 || hau == 0 )
    return ;

  switch(3)
    {
    case 0:
      {
	/* Ca marche vraiment pas */
	XtResizeWidget((Widget)w, lar, hau, 0) ;
	break ;
      }
    case 1:
      {
	Dimension l, h ;
	XtMakeResizeRequest(w, lar, hau, &l, &h) ;
	break ;
      }
    case 2:
      {
	XtWidgetGeometry wg ;
	XtWidgetGeometry r ;
	wg.request_mode = CWWidth|CWHeight ;
	wg.width = lar ;
	wg.height = hau ;
	XtMakeGeometryRequest(w, &wg, &r) ;  
	break ;
      }
    case 3:
      {
	XtVaSetValues((Widget)w, XmNwidth, lar, XmNheight, hau, NULL) ;
	break ;
      }

    }
}

void widget_get_size(NanoWidget w, int *lar, int *hau)
{
  *lar = ((RectObj)w)->rectangle.width ;
  *hau = ((RectObj)w)->rectangle.height ;
  EPRINTF("Get size = %d %d\n", *lar, *hau) ;
}

void widget_get_current_size(NanoWidget w, int *lar, int *hau)
{
  widget_get_size(w,lar,hau) ;
  EPRINTF("Get current size = %d %d\n", *lar, *hau) ;
}

void widget_positionne(NanoWidget w, int x, int y)
{
  EPRINTF("Positionne en %d %d\n", x, y) ;
  XtVaSetValues((Widget)w, XmNx, x+1, XmNy, y+1, NULL) ;
}

void widget_get_position(NanoWidget w, int *x, int *y)
{
  Dimension xx, yy ;
  XtVaGetValues((Widget)w, XmNx, &xx, XmNy, &yy, NULL) ;
  *x = xx ;
  *y = yy ;
  EPRINTF("get position = %d %d\n", *x, *y) ;
}


NanoWidget widget_initialise_application(int *argc, char ***argv
					 , NanoAppContext *app_context)
{
  return( XtAppInitialize((XtAppContext*)app_context, "NanoEdit",
			  NULL,0, argc, *argv, NULL, NULL, 0)
	  ) ;
}

void widget_popup(NanoWidget w)
{
  XtPopup(w, XtGrabNone) ;
}

void widget_popdown(NanoWidget w)
{
  XtPopdown(w) ;
}

int widget_event_pending(NanoAppContext app_context)
{
  return XtAppPending(app_context) ;
}

int widget_process_event(NanoAppContext app_context)
{
  XtAppProcessEvent((XtAppContext)app_context, XtIMAll) ;
  return XtAppPending(app_context) ;
}

void widget_allocation_couleur(NanoWidget w, const char *nom, NanoColor *c)
{
  XColor tmp ;

  if ( XAllocNamedColor(XtDisplay((Widget)w),
			DefaultColormap(XtDisplay((Widget)w),
					0), /* Devrait extraire le BON écran */
			nom,
			(XColor*)c,
			&tmp) == 0 )
    {
      eprintf("Manque de couleur !\n") ;
    }
}

NanoIdle widget_ajoute_idle(NanoAppContext app_context, POINTEUR_CALLBACK_IDLE(fct))
{
  EPRINTF("Ajoute IDLE\n") ;
  return( XtAppAddWorkProc((XtAppContext)app_context
		   , (XtWorkProc)fct
		   , NULL)
	  ) ;
}

void widget_enleve_idle(NanoIdle id)
{
  EPRINTF("Enlève IDLE\n") ;
  XtRemoveWorkProc( (XtWorkProcId)id) ;
}

void widget_detruit(NanoWidget w)
{
  EPRINTF("widget_detruit = %p\n", w) ;
  XtDestroyWidget(w) ;
}

void widget_item_detruit(NanoWidget w)
{
  widget_detruit(w) ;
}

Booleen widget_visible(NanoWidget w)
{
  XWindowAttributes wa ;

  XGetWindowAttributes(XtDisplay((Widget)w), XtWindow((Widget)w), &wa) ;
  return( wa.map_state == IsViewable ) ;
}

Liste *widget_fils(NanoWidget w)
{
  int i, nb ;
  Liste *l ;
  Widget *fils ;

  XtVaGetValues(w
		, XtNnumChildren, &nb
		, XtNchildren, &fils
		, NULL) ;
  l = NULL ;
  for(i=0;i<nb;i++)
    l_ajoute(&l, fils[i]) ;
  return(l) ;
}

NanoWidget widget_vrai_bulle_d_aide(NanoWidget parent, const char *texte)
{
  Widget w ;

  w = XtVaCreateManagedWidget("help_shell", vendorShellWidgetClass,
			      parent,
			      NULL) ;
  XtVaCreateManagedWidget("help_label",
			  xmLabelWidgetClass,
			  w,
			  NULL) ;
  return(w) ;
}


NanoWidget widget_initialiser_bulle_d_aide(NanoWidget parent)
{
  Widget w ;

  w = XtVaCreateManagedWidget("help_shell", vendorShellWidgetClass,
			      parent,
			      XmNallowShellResize, True,
			      /*	      XmNoverrideRedirect, True, */
			      NULL) ;
  XtVaCreateManagedWidget("help_label",
			  xmTextWidgetClass,
			  w,
			  XmNshadowThickness,0,
			  XmNresizeHeight, True,
			  XmNresizeWidth, True,
			  XmNscrollHorizontal, False,
			  XmNscrollVertical, False,
			  XmNcursorPositionVisible,0,
			  XmNeditable,0,
			  XmNeditMode, XmMULTI_LINE_EDIT,
			  XmNresizable,True,
#ifdef LesstifVersion
			  XmNcolumns, 80,
			  XmNrows, 40,
#endif
			  NULL) ;
  return(w) ;
}

void widget_label_ecrit(NanoWidget w, const char *t)
{
  XtVaSetValues((Widget)w
		, XmNlabelString
		, XmCvtCTToXmString((char*)t)
		, NULL
		) ;
}

const char *widget_label_lit(NanoWidget w)
{
  XmString s ;
  static char *chaine ;

  if ( chaine )
    XtFree(chaine) ;
    
  XtVaGetValues( (Widget)w, XmNlabelString, &s, NULL) ;
  chaine = XmCvtXmStringToCT(s) ;
  return(chaine) ;
}

void widget_raise(NanoWidget w)
{
  XRaiseWindow( XtDisplay((Widget)w), XtWindow((Widget)w) ) ;
}


void widget_decode_evenement(NanoEvent e, Interface_widget *iw)
{
  XButtonEvent *ev ;
  XKeyEvent *ek ;
  KeySym *map ;
  int nbmod ;
  int min, max, nb ;

  iw->e = e ;
  ev = (XButtonEvent*)e ;
  EPRINTF("Event type = %d\n", ev->type) ;

  iw->x = ev->x ;
  iw->y = ev->y ;
  iw->x_root = ev->x_root ;
  iw->y_root = ev->y_root ;

  iw->etat = (ev->state & 8)/2
    + (ev->state & 4)/2
    + (ev->state & 1) ;

  if ( ev->type == ButtonPress || ev->type == ButtonRelease )
    {
      iw->b = ev->button-Button1 ;
      if ( ev->state & Button1Mask )
	iw->b = 0 ;
      if ( ev->state & Button2Mask )
	iw->b = 1 ;
      if ( ev->state & Button3Mask )
	iw->b = 2 ;
    }
  iw->configure_x = 0 ;
  if ( ev->type == CreateNotify
       || ev->type == MapNotify 
       || ev->type == UnmapNotify 
       || ev->type == ReparentNotify )
    iw->configure_x = -1 ;

  /*
  if ( ev->type == MapNotify )
    {
      widget_get_size(iw->w, &iw->configure_l, &iw->configure_h) ;
      widget_get_position(iw->w, &iw->configure_x, &iw->configure_y) ;
    }
  */
  if ( ev->type == ConfigureNotify )
    {
      iw->configure_l = ((XConfigureEvent*)ev)->width ;
      iw->configure_h = ((XConfigureEvent*)ev)->height ;
      if ( ((XConfigureEvent*)ev)->send_event )
	{
	  iw->configure_x = ((XConfigureEvent*)ev)->x ;
	  iw->configure_y = ((XConfigureEvent*)ev)->y ;
	}
      else
	{
	  widget_get_position(iw->w, &iw->configure_x, &iw->configure_y) ;
	}
      EPRINTF("configure = %d %d %dx%d\n"
	      , iw->configure_x 
	      , iw->configure_y 
	      , iw->configure_l 
	      , iw->configure_h
	      ) ;
    }
  if ( ev->type == KeyPress )
    {
      ek = (XKeyEvent*)e ;
      XDisplayKeycodes(XtDisplay((Widget)iw->w), &min, &max) ;
      nb = max - min + 1 ;
      map = XGetKeyboardMapping(XtDisplay((Widget)iw->w), min, nb, &nbmod) ;
      if ( map == NULL )
	return ;
      widget_keyval_to_string
	(map[ (ek->keycode-min)*nbmod + ((nbmod-1)&ek->state) ]
	 ,map[ (ek->keycode-min)*nbmod ]
	 , iw) ;
    }
}

void widget_manage_freres(NanoWidget *w, int i)
{
  if ( i )
    XtManageChildren((Widget*)w, i) ;
}

void widget_unmanage_freres(NanoWidget *w, int i)
{
  if ( i )
    XtUnmanageChildren((Widget*)w, i) ;
}

void widget_map(NanoWidget w)
{
  XtMapWidget((Widget)w) ;
}

void widget_unmap(NanoWidget w)
{
  XtUnmapWidget((Widget)w) ;
}

NanoWidget widget_horizontal_ou_vertical(NanoWidget w, const char *nom
					 , int horizontal, int homogene)
{
  return( (NanoWidget) XtVaCreateWidget(nom, xmFormWidgetClass, w
				/* , XmNrubberPositioning, True */
					, NULL) ) ;
}

NanoWidget widget_label(NanoWidget w, const char *nom, const char *label)
{
  return( XtVaCreateWidget(nom
			   , xmLabelWidgetClass
			   , w
			   , XmNlabelString, XmCvtCTToXmString((char*)label)
			   , NULL)
	  ) ;
}

NanoWidget widget_popup_menu(NanoWidget w, const char *titre)
{   
  Arg ap[1] ;
  NanoWidget ww ;

  ap[0].name = XmNtearOffModel ;
  ap[0].value = XmTEAR_OFF_ENABLED ;
  ww = XmCreatePopupMenu(w, "popup", ap, 1) ;
  EPRINTF("widget_popup = %p\n", ww) ;
  return(ww) ;
}

Booleen widget_est_tearoff(NanoWidget w)
{
  return(XmIsTearOffButton(w)) ;
}

void widget_add_event_unmap(NanoWidget w
			    , POINTEUR_CALLBACK_ACTION(fct)
			    , const Champ *c)
{
  XtAddCallback((Widget)w, XmNunmapCallback
		, (XtCallbackProc)fct, (XtPointer)c) ;
}

void widget_add_event_tearoff(NanoWidget w
			    , POINTEUR_CALLBACK_ACTION(fct)
			    , const Champ *c)
{
  XtAddCallback((Widget)w, XmNtearOffMenuActivateCallback
		, (XtCallbackProc)fct, (XtPointer)c) ;
}

NanoWidget widget_pulldown_menu(NanoWidget w)
{   
  Arg ap[1] ;
  NanoWidget ww ;

  ap[0].name = XmNtearOffModel ;
  ap[0].value = XmTEAR_OFF_ENABLED ;
  ww = XmCreatePulldownMenu(w, "pulldown", ap, 1) ;

  EPRINTF("widget_pulldown = %p\n", ww) ;

  return(ww) ;
}


void widget_attache_sous_menu(NanoWidget bouton, NanoWidget menu)
{
  XtVaSetValues(bouton, XmNsubMenuId, menu, NULL) ;
}

void widget_detruit_menu(NanoWidget menu)
{
  EPRINTF("widget_detruit_menu = %p\n", menu) ;
#ifdef LesstifVersion
  XtDestroyWidget(XtParent(menu)) ;
#else
  XtDestroyWidget(menu) ;
#endif
}

NanoWidget widget_menu_attache(NanoWidget bouton)
{
  NanoWidget w ;
  w = NULL ;
  XtVaGetValues(bouton, XmNsubMenuId, &w, NULL) ;
  return(w) ;
}

void widget_congele(NanoWidget w)
{
  XtUnmapWidget((Widget)w) ;
}

void widget_degele(NanoWidget w)
{
  XtMapWidget((Widget)w) ;
}

NanoWidget widget_bouton_cascade(NanoWidget w
				 , const char *nom, const char *label)
{  
  return(XtVaCreateWidget(nom, xmCascadeButtonWidgetClass, (Widget)w,
			  XmNlabelString, XmCvtCTToXmString((char*)label),
			  NULL) ) ;
}

void widget_termine_menu(NanoWidget w, Liste *lw)
{
}

void widget_positionne_menu(NanoWidget w, NanoEvent *event)
{
  XmMenuPosition(w, (XButtonPressedEvent*)event);
}

void widget_sensible(NanoWidget w, int i)
{ 
  XtVaSetValues(w, XmNsensitive, i, NULL) ;
}


static void widget_ajoute_a_boite(NanoWidget w, NanoWidget precedent
				  , NanoWidget suivant, NanoWidget f
				  , int horizontal, int homogene)
{
  if ( horizontal == -1 )
    return ;

  if ( horizontal )
    {
      XtVaSetValues(f
		    , XmNtopAttachment, XmATTACH_FORM
		    , XmNbottomAttachment, XmATTACH_FORM
		    , NULL) ;
      
      if ( !homogene )
	{
	  if ( precedent )
	    XtVaSetValues(f
			  , XmNleftAttachment, XmATTACH_WIDGET
			  , XmNleftWidget, precedent
			  , NULL) ;
	  else
	    XtVaSetValues(f
			  , XmNleftAttachment, XmATTACH_FORM
			  , NULL) ;	
	  /*
	  if ( suivant )
	    XtVaSetValues(f
			  , XmNrightAttachment, XmATTACH_WIDGET
			  , XmNrightWidget, suivant
			  , NULL) ;
	  else
	  */
	  if ( suivant==NULL )
	    XtVaSetValues(f
			  , XmNrightAttachment, XmATTACH_FORM
			  , NULL) ;
	}
    }
  else
    {
      XtVaSetValues(f
		    , XmNleftAttachment, XmATTACH_FORM
		    , XmNrightAttachment, XmATTACH_FORM
		    , NULL) ;	     
      if ( !homogene )
	{
	  if ( precedent )
	    XtVaSetValues(f
			  , XmNtopAttachment, XmATTACH_WIDGET
			  , XmNtopWidget, precedent
			  , NULL) ;
	  else
	    XtVaSetValues(f
			  , XmNtopAttachment, XmATTACH_FORM
			  , NULL) ;
	  /*
	  if ( suivant )
	    XtVaSetValues(f
			  , XmNbottomAttachment, XmATTACH_WIDGET
			  , XmNbottomWidget, suivant
			  , NULL) ;
	  else
	  */
	  if ( suivant==NULL )
	    XtVaSetValues(f
			  , XmNbottomAttachment, XmATTACH_FORM
			  , NULL) ;
	}
    }
}

void widget_termine_boite(NanoWidget w, int horizontal, int homogene,Liste *lw)
{
  int nb, i ;
  NanoWidget *tw ;

  if ( horizontal == -1 )
    return ;

  nb = l_longueur(lw) ;
  ALLOUER(tw, nb) ;
  POUR(NanoWidget, ww, lw, tw[i_ww] = ww ; ) ;
  for(i=0;i<nb;i++)
    {
      widget_ajoute_a_boite(w
			    , i!=0 ? tw[i-1] : NULL
			    , i!=nb-1 ? tw[i+1] : NULL
			    , tw[i], horizontal, homogene
			    );
    }
  free(tw) ;

  if ( homogene )
    {
      if ( horizontal )
	{
	  POUR(Widget, ww, lw,
	       {
		 XtVaSetValues(ww
			       , XmNleftAttachment, XmATTACH_POSITION
			       , XmNleftPosition, (100*i_ww)/nb
			       , XmNrightAttachment, XmATTACH_POSITION
			       , XmNrightPosition, (100*(i_ww+1))/nb
			       , NULL) ;
	       }) ;
	}
      else
	{
	  POUR(Widget, ww, lw,
	       {
		 XtVaSetValues(ww
			       , XmNtopAttachment, XmATTACH_POSITION
			       , XmNtopPosition, (100*i_ww)/nb
			       , XmNbottomAttachment, XmATTACH_POSITION
			       , XmNbottomPosition, (100*(i_ww+1))/nb
			   , NULL) ;
	       }) ;
	}
    }
}

void widget_termine_boite_invisible(NanoWidget w, Liste *lw)
{
}


void widget_set_width(NanoWidget w, int lar)
{
  XtVaSetValues((Widget)w, XmNwidth, lar, NULL) ;
}

void widget_set_height(NanoWidget w, int hau)
{
  XtVaSetValues((Widget)w, XmNheight, hau, NULL) ;
}

NanoWidget widget_window(NanoWidget w, const char *nom)
{
  return( (NanoWidget)XtVaCreateManagedWidget
	  ("edite", vendorShellWidgetClass, (Widget)w,
	   XmNinput, True,
	   NULL)
	  ) ;
}

void widget_set_title(NanoWidget w, const char *title)
{
  XtVaSetValues((Widget)w, XmNtitle, strdup(title), NULL) ;
}

const char *widget_get_title(NanoWidget w)
{
  char *c = NULL ;
  XtVaGetValues(w, XmNtitle, &c, NULL) ;
  return(c) ;
}

void widget_realize(NanoWidget w)
{
  XtRealizeWidget((Widget)w) ;
}

void widget_invalide(NanoWidget w, int invalide)
{
  static long fond_par_defaut = -1 ;
  static long fond_rouge = 0 ;
  unsigned long fond ;
  XColor nc ;

  fond = fond_par_defaut ;
  XtVaGetValues((Widget)w, XmNbackground, &fond, NULL ) ;

  if ( fond_rouge == 0 )
    {
      widget_allocation_couleur(w, "#FF0000", (NanoColor*)&nc) ;
      fond_rouge = nc.pixel ;
    }

  if ( fond_par_defaut == -1 )
    fond_par_defaut = fond ;

  if ( fond!=fond_rouge && invalide )
    XtVaSetValues((Widget)w, XmNbackground, fond_rouge, NULL) ;
  else
    if ( fond!=fond_par_defaut && !invalide )
      XtVaSetValues((Widget)w, XmNbackground, fond_par_defaut, NULL) ;

}

NanoWidget widget_texte(NanoWidget w, const char *nom, int editable)
{
  return(XtVaCreateWidget(nom,
			  xmTextWidgetClass,
			  (Widget)w,
			  XmNshadowThickness,       editable ?  3 : 0,
			  XmNcursorPositionVisible, editable,
			  XmNeditable,              editable,
			  NULL)
	 ) ;
}

NanoWidget widget_texte_long(NanoWidget w, const char *nom, int editable)
{
  return(XtVaCreateWidget(nom,
			  xmTextWidgetClass,
			  (Widget)w,
			  XmNshadowThickness,       editable ?  3 : 0,
			  XmNcursorPositionVisible, editable,
			  XmNeditable,              editable,
			  XmNeditMode,              XmMULTI_LINE_EDIT,
			  XmNscrollVertical,        True,
			  NULL)
	 ) ;
}

const char *widget_texte_lit(NanoWidget w)
{
  static char *s = NULL ;	/* STATICOK car retournée */

  if ( s != NULL )
    XtFree(s) ;
  s = NULL ;
  XtVaGetValues( (Widget)w, XmNvalue, &s, NULL) ; 
  return(s) ;
}

void widget_texte_ecrit(NanoWidget w, const char *texte)
{
#ifdef LesstifVersion
      XtVaSetValues(w, XmNvalue, strdup(texte), NULL) ;
#else
      XtVaSetValues(w, XmNvalue, texte, NULL) ;
#endif
}

void widget_add_event_texte(NanoWidget w
			    , POINTEUR_CALLBACK_ACTION(fct)
			    , const Champ *c)
{
  XtAddCallback((Widget)w, XmNvalueChangedCallback, (XtCallbackProc)fct,
		(XtPointer)c) ;
}

NanoWidget widget_toggle(NanoWidget w, const char *nom, const char *label)
{
  return( XtVaCreateWidget(nom,
			   xmToggleButtonWidgetClass,
			   w,
			   XmNlabelString,
			   XmCvtCTToXmString((char*)label),
			   NULL)
	  );
}

int widget_toggle_lit(NanoWidget w)
{
  return(XmToggleButtonGetState((Widget)w)) ;
}

void widget_toggle_ecrit(NanoWidget w, int i)
{
  XmToggleButtonSetState((Widget)w, i, False) ;
}

NanoWidget widget_button(NanoWidget w, const char *nom, const char *label)
{ 
  return( XtVaCreateWidget(nom, xmPushButtonWidgetClass, w,
			   XmNlabelString, XmCvtCTToXmString((char*)label),
			   NULL)
	  ) ;
}
NanoWidget widget_button_menu(NanoWidget w, const char *nom, const char *label)
{ 
  return(widget_button(w,nom,label)) ;
}
NanoWidget widget_button_menu_with_icon(NanoWidget w, const char *nom, const char *label, const char *icon_file)
{ 
  return(widget_button(w,nom,label)) ;
}
NanoWidget widget_button_for_menu(NanoWidget w
				 , const char *nom, const char *label)
{
  return( widget_button(w, nom, label) ) ;
}


const char* widget_button_lit(NanoWidget w)
{
  return( widget_label_lit(w) ) ;
}

void widget_button_ecrit(NanoWidget w, const char*s)
{
  widget_label_ecrit(w, s) ;
}


NanoWidget widget_drawing(NanoWidget w, const char *nom, NanoWidget *zone)
{
  NanoWidget sw ;

  sw  = XtVaCreateManagedWidget(nom, 
                                xmScrolledWindowWidgetClass, 
                                (Widget)w, 
				XmNscrollingPolicy,    XmAUTOMATIC,
                                XmNscrollBarPlacement, XmBOTTOM_RIGHT,
                                NULL);

  *zone = XtVaCreateManagedWidget("real_graphe",
				  xmDrawingAreaWidgetClass,
				  sw,
				  XmNsensitive, True,
				  XmNancestorSensitive, True,
				  NULL) ;
  return(sw) ;
}


NanoWidget widget_tree(NanoWidget w, const Interface_widget *iw
		       ,POINTEUR_CALLBACK_EVENT(fct))
{
  NanoWidget sw, zone ;

  sw = widget_drawing(w, iw->ch->nom_champ, &zone) ;

  widget_add_event_expose         (zone, u_callback_expose  , iw->ch) ;
  widget_add_event_button_press   (zone, u_ev_button_press  , iw->ch) ;
  widget_add_event_button_release (zone, u_ev_button_release, iw->ch) ;
  widget_add_event_key_press      (zone, u_ev_key_press     , iw->ch) ;

  return(sw) ;
}

const char *widget_tree_lit(NanoWidget w)
{
  static int n = 1 ;
  return( int_vers_char(n++) ) ;
}

void widget_tree_ecrit(const Interface_widget* iw)
{
  if ( *iw->ch->te_ecrit )
    (*iw->ch->te_ecrit)(iw) ; 
}



void widget_scrollbar_ecrit(NanoWidget w, float min, float max, float value)
{
  XtVaSetValues((Widget)w,
		XmNminimum , -1+(int)(PRECISION_ASCENSEUR*min),
		XmNmaximum , 1+(int)(PRECISION_ASCENSEUR*max),
		XmNvalue, (int)(PRECISION_ASCENSEUR*value),
		NULL
		) ;
}

void widget_scrollbar_infos(NanoWidget w, float *min, float *max, float *value)
{
  int imin, imax, iv ;

  XtVaGetValues((Widget)w,
		XmNminimum ,&imin,
		XmNmaximum , &imax,
		XmNvalue, &iv,
		NULL
		) ;
  imin++ ;
  imax-- ;

  *value = iv/PRECISION_ASCENSEUR ;
  *min = imin/PRECISION_ASCENSEUR ;
  *max = imax/PRECISION_ASCENSEUR ;
}

NanoWidget widget_hscrollbar(NanoWidget w, const char *nom)
{
  return( XtVaCreateManagedWidget(nom, xmScrollBarWidgetClass, (Widget)w,
				  XmNorientation, XmHORIZONTAL,
				  XmNsliderSize, 1,
				  NULL)
	  ) ;
}

void widget_add_event_scrollbar(NanoWidget w
				, POINTEUR_CALLBACK_ACTION(fct)
				, const Champ *c)
{
  XtAddCallback((Widget)w, XmNdragCallback      , (XtCallbackProc)fct
		,(XtPointer)c) ;
  XtAddCallback((Widget)w, XmNincrementCallback , (XtCallbackProc)fct
		,(XtPointer)c) ;
  XtAddCallback((Widget)w, XmNdecrementCallback , (XtCallbackProc)fct
		, (XtPointer)c) ;
}




/*
 *****************************************************************************
 *****************************************************************************
 *****************************************************************************
 */

static void u_liste_objet(void *a, Display *d, Window w, GC gc,
			  int *y, int dy, GC gca, GC gc_invalide)
{
  static int profondeur = 0 ;
  Buffer tout, avant, nom, actif, pointe, pointeur ;
  const char *info ;
  Objet *a_ ;

  if ( a == NULL )
    {
      strcpy(tout, "DETRUIT") ;
    }
  else
    {
      a_ = OBJET(a) ;
      info = o_informations(a) ;
      if ( info && strcmp(info,"") )
	sprintf(nom, "%s(%s)", a_->classe->nom, info) ;
      else
	sprintf(nom, "%s", a_->classe->nom) ;
      
      
      if ( l_longueur(a_->parent)>1 )
	sprintf(pointeur, "[%d]", INDEX_OBJET(a_)) ;
      else
	pointeur[0] = '\0' ;
      
      if ( ACTIF_(a_) )
	sprintf(actif,"  ACTIF=%d", ACTIF_(a_)) ;
      else
	actif[0] = '\0' ;
      
      if ( a_->flag2 )
	sprintf(pointe, " voir %s", pointeur) ;
      else
	pointe[0] = '\0' ;
      
      if ( l_longueur(a_->parent)!=1 && !a_->flag2 )
	sprintf(avant, "%s ", pointeur) ;
      else
	avant[0] = '\0' ;
      
      sprintf(tout, "%s%s%s%s",
	      avant, nom,
	      pointe, actif) ;

      if ( UN_CHAMP_INVALIDE_(a_) )
	XDrawString(d,w,gc_invalide,
		    profondeur*20-2,*y-2,tout,strlen(tout)) ;
    }
  XDrawString(d,w,(a!=NULL&&ACTIF_(a_))?gca:gc,profondeur*20,*y,tout,strlen(tout)) ;
  *y += dy ;

  if ( a==NULL || a_->flag2)
    return ;

  a_->flag2 = 1 ;

  profondeur++ ;
  if ( a_->fils )
    {
      POUR_TOUS_LES_FILS_(a_,f,u_liste_objet(f,d,w,gc,y,dy,gca,gc_invalide););
    }
  profondeur-- ;
}

static void u_trouve(void *a, int yb, int *y, int dy)
{
  Objet *a_ ;

  *y += dy ;
  if ( a == NULL )
    return ;
  
  a_ = OBJET(a) ;
  if ( yb<=*y && yb>*y-dy )
    {
      if ( strcmp(a_->classe->nom, "modele") )
	o_bascule_actif(a) ;
    }

  if ( a_->flag2 )
    return ;
  a_->flag2 = 1 ;

  if ( a_->fils )
    {
      POUR_TOUS_LES_FILS_(a_, f, u_trouve(f,yb,y,dy) ; ) ;
    }
}

static GC gc = 0, gc_actif = 0, gc_invalide=0 ;
static XFontStruct *f ;
static int dy ;
static const int ystart = 20 ;

void widget_Graphe_start(const Interface_widget *iw)
{
  XGCValues values;
  XCharStruct ov ;
  int dir, ascent, descent ;
  Display *d ;
  char *fonte ;
  Widget widget ;
  XColor *c ;

  if ( gc != 0 )
    return ;

  if ( strcmp(getenv("USER"), "et") )
	fonte =  "-*-*-medium-r-*-*-*-100-*-*-p-*-iso8859-1" ;
  else
	fonte =  "-*-*-bold-r-*-*-*-140-*-*-p-*-iso8859-1" ;


  widget = (Widget)iw->w ;
  d = XtDisplay(widget) ;
  values.font = XLoadFont(d, fonte) ;
  gc = XtGetGC(widget,GCFont,&values) ;
  c = (XColor*)&((Xt_interface*)iw->objet)->table[7] ;
  values.foreground = c->pixel ;

  gc_actif = XCreateGC(XtDisplay(widget),RootWindow(XtDisplay(widget),0),
		       GCFont|GCForeground,&values) ;

  
  c = (XColor*)&((Xt_interface*)iw->objet)->table[1] ;
  values.foreground = c->pixel ;
  gc_invalide = XCreateGC(XtDisplay(widget),RootWindow(XtDisplay(widget),0),
		       GCFont|GCForeground,&values) ;

  f = XQueryFont(d, values.font) ;
  XTextExtents(f,"XÉj",1,&dir,&ascent,&descent,&ov) ;
  dy = ascent + descent ;
  XFreeFont(d, f) ;

}

void widget_Graphe_stop(const Interface_widget *iw)
{
  gc = 0 ;
}

void widget_Graphe_trace(const Interface_widget *iw)
{
  Display *d ;
  int y ;
  Objet *objet_ ;
  Widget widget ;
  Window w ;
  const Champ *oper ;
  const void *oo ;

  objet_ = OBJET(iw->objet) ;
  if ( objet_->edite )
    {
      y = ystart ;

      widget = iw->w ;
      if ( strcmp(XtName(widget), "real_graphe") )
	widget = XtNameToWidget(widget, "*real_graphe") ;
	  
      d = XtDisplay(widget) ;
      w = XtWindow(widget) ;
      XClearWindow(d,w) ;

      oo = iw->objet ;
      if ( EST_OPERATEUR(iw->objet) )
	{
	  c_trouve_type(&oo, &oper, &infos_L_operateur) ;
	  if ( T(Operateur, iw->objet, oper).fichier[0] )
	    POUR_TOUS_LES_ELEMENTS
	      (
	       CONTENU(&T(Operateur, iw->objet, oper)), f,
	       {
		 o_annule_flag2(f) ;
		 u_liste_objet(f,d,w,gc,&y,dy,gc_actif,gc_invalide);
	       }
	       );
	}
      else
	{
	  o_annule_flag2(iw->objet) ;
	  u_liste_objet(iw->objet, d, w, gc, &y, dy, gc_actif, gc_invalide) ;
	}
      
      if ( y<1000 ) y = 1000 ; /* Problème du raccourci clavier */
      XtVaSetValues(widget, "width", 1000, "height", y+10, NULL) ;
    }
}


Booleen widget_Graphe_entree(const Interface_widget *iw)
{
  int yc ;
  const Champ *oper ;
  const void *oo ;

  yc = ystart - dy ;

  oo = iw->objet ;
  if ( EST_OPERATEUR(iw->objet) )
    {
      c_trouve_type(&oo, &oper, &infos_L_operateur) ;
      if ( T(Operateur, iw->objet, oper).fichier[0] )
	POUR_TOUS_LES_ELEMENTS(CONTENU(&T(Operateur, oo, oper)), f,
			       {
				 o_annule_flag2(f) ;
				 u_trouve(f, iw->y, &yc, dy);
			       }
			       ) ;
    }
  else
    {
      o_annule_flag2(iw->objet) ;
      u_trouve(iw->objet, iw->y, &yc, dy) ;
    }
  return(1) ;
}

void widget_current_gl(NanoWidget w, GLXContext c)
{
  GLwDrawingAreaMakeCurrent((Widget)w, c) ;
}
void widget_swap_gl(NanoWidget w)
{
  GLwDrawingAreaSwapBuffers((Widget)w) ;
}
NanoWidget widget_gl(NanoWidget w, const char *nom)
{
  return( XtVaCreateWidget(nom,
			   glwMDrawingAreaWidgetClass,
			   (Widget)w,
			   GLwNdepthSize, 16,
			   GLwNrgba, True,
			   GLwNdoublebuffer, True,
			   GLwNstencilSize, 1,
			   NULL
			   )
	  ) ;
}

void widget_destroy_context(NanoWidget w, GLXContext c)
{
  glXDestroyContext(XtDisplay((Widget)w), c) ;
}

GLXContext widget_create_context(NanoWidget w)
{
  XVisualInfo *xvi ;

  XtVaGetValues( (Widget)w, GLwNvisualInfo, &xvi, NULL);
  return( glXCreateContext(XtDisplay((Widget)w), xvi, 0, GL_TRUE) ) ;
}
void widget_flush(NanoWidget w)
{
}

void widget_ne_recoit_plus_evenement(NanoWidget w)
{
}

void widget_recoit_a_nouveau_evenement(NanoWidget w)
{
}

int widget_ajoute_timeout(int interval, Booleen (*fct)(void*), void *data)
{
  return( 1 ) ;
}

void widget_enleve_timeout(int id)
{
}
