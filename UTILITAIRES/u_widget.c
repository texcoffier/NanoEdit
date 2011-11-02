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
#include <GL/glx.h>
#include <X11/keysym.h>
#include <ctype.h>
#include "o_objet.h"
#include "u_widget.h"
#include "u_xt_interface.h"

#define PRECISION_ASCENSEUR 1000.

void widget_keyval_to_string(int t, int t2, Interface_widget *iw)
{
  iw->touche[0] = '\0' ;
  iw->touche[1] = '\0' ;

  if ( t>=XK_KP_0 && t<= XK_KP_9 )
    t = t - XK_KP_0 + '0' ;
  
  if ( t>=' ' && t<127 )
    {
      if ( isdigit(t) )
	iw->etat &= ~1 ; /* Enleve le Shift au cas ou AZERTY */
      iw->touche[0] = tolower(t) ;
    }
  else
    {
      /* Le prend sans modifieur */
      switch( t2 )
	{
	case XK_Home        : strcpy(iw->touche, "Home"      ); break ;
	case XK_Left        : strcpy(iw->touche, "Left"      ); break ;
	case XK_Up          : strcpy(iw->touche, "Up"        ); break ;
	case XK_Right       : strcpy(iw->touche, "Right"     ); break ;
	case XK_Down        : strcpy(iw->touche, "Down"      ); break ;
	case XK_Page_Up     : strcpy(iw->touche, "Page_Up"   ); break ;
	case XK_Page_Down   : strcpy(iw->touche, "Page_Down" ); break ;
	case XK_End         : strcpy(iw->touche, "End"       ); break ;
	case XK_Begin       : strcpy(iw->touche, "Begin"     ); break ;
	case XK_Delete      : strcpy(iw->touche, "Delete"    ); break ;
	case XK_F1          : strcpy(iw->touche, "F1"        ); break ;
	case XK_F2          : strcpy(iw->touche, "F2"        ); break ;
	case XK_F3          : strcpy(iw->touche, "F3"        ); break ;
	case XK_F4          : strcpy(iw->touche, "F4"        ); break ;
	case XK_F5          : strcpy(iw->touche, "F5"        ); break ;
	case XK_F6          : strcpy(iw->touche, "F6"        ); break ;
	case XK_F7          : strcpy(iw->touche, "F7"        ); break ;
	case XK_F8          : strcpy(iw->touche, "F8"        ); break ;
	case XK_F9          : strcpy(iw->touche, "F9"        ); break ;
	case XK_F10         : strcpy(iw->touche, "F10"       ); break ;
	case XK_F11         : strcpy(iw->touche, "F11"       ); break ;
	case XK_F12         : strcpy(iw->touche, "F12"       ); break ;
	case XK_KP_Space    : strcpy(iw->touche, " "         ); break ;
	case XK_KP_Tab      : strcpy(iw->touche, "\t"        ); break ;
	case XK_KP_Enter    : strcpy(iw->touche, "\n"        ); break ;
	case XK_KP_Equal    : strcpy(iw->touche, "="         ); break ;
	case XK_KP_Multiply : strcpy(iw->touche, "*"         ); break ;
	case XK_KP_Add      : strcpy(iw->touche, "+"         ); break ;
	case XK_KP_Subtract : strcpy(iw->touche, "-"         ); break ;
	case XK_KP_Decimal  : strcpy(iw->touche, "."         ); break ;
	case XK_KP_Divide   : strcpy(iw->touche, "/"         ); break ;


	case XK_Shift_L   :
	case XK_Shift_R   : iw->etat |= 1 ; return ;
	case XK_Control_L :
	case XK_Control_R : iw->etat |= 2 ; return ;
	case XK_Alt_L     :
	case XK_Alt_R     : iw->etat |= 4 ; return ;
	case XK_Caps_Lock :
	case XK_Shift_Lock:
	case XK_Meta_L    :
	case XK_Meta_R    :
	case XK_Super_L   :
	case XK_Super_R   :
	case XK_Hyper_L   :
	case XK_Hyper_R   :
	  return ;
	default           : strcpy(iw->touche, "?") ;
	}
    }
}

#ifdef __CHECKER__

#include "u_widget_vide.h"

#else

#ifdef UTILISE_MOTIF
#include "u_widget_motif.h"
#else
#include "u_widget_gtk.h"
#endif

#endif

NanoWidget widget_sommet(NanoWidget w)
{
  NanoWidget ww ;

  for(;;)
    {
      ww = widget_parent(w) ;
      if ( ww == NULL )
	return(w) ;
      w = ww ;
    }
}

NanoWidget widget_shell(NanoWidget w)
{
  while( !widget_fenetre(w) )
    {
      w = widget_parent(w) ;
      if ( w == NULL )
	{
	  ICI ;
	  return(NULL) ;
	}
    }
  return(w) ;
}

NanoWidget widget_fils0(NanoWidget w)
{
  Liste *l ;

  l = widget_fils(w) ;
  w = l_element(l, 0) ;
  l_detruit(&l) ;

  return(w) ;
}

void widget_manage_liste(const Liste *a_manager)
{
  NanoWidget *w ;
  int i ;

  w = l_liste_vers_table(a_manager, &i) ;
  widget_manage_freres(w, i) ;
  free( w ) ;
}

void widget_unmanage_liste(const Liste *a_manager)
{
  NanoWidget *w ;
  int i ;

  w = l_liste_vers_table(a_manager, &i) ;
  widget_unmanage_freres(w, i) ;
  free( w ) ;
}

