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
#include "t_gl.h"
#include "s_string.h"
#include "u_xt_interface.h"
#include <GL/gl.h>		/* Car il affiche du GL */
#include <time.h>
/*
 ******************************************************************************
 * GL
 ******************************************************************************
 */
static GLXContext globale_contexte = 0 ;

static global_dans_gl = 0 ;

long Gl_start(Gl *gl)
{
  if ( global_dans_gl )
    eprintf("2 Appels Gl_start successifs\n") ;
  global_dans_gl = 1 ;

  if ( globale_contexte == 0 )
    {
      globale_contexte =  widget_create_context(gl->gl) ;
    }
  widget_current_gl(gl->gl, globale_contexte) ;  
  widget_get_current_size(gl->gl, &gl->lar, &gl->hau) ;

  glViewport(0, 0, gl->lar, gl->hau) ;
  glMatrixMode(GL_PROJECTION) ;
  glLoadIdentity() ;
  glMatrixMode(GL_MODELVIEW) ;
  glLoadIdentity() ;
  glBindTexture(GL_TEXTURE_2D, 1);

  glGetError() ; // Remet à 0 les erreurs.

  if ( 1 ) // Toujours afficher les problèmes (même si pas d'erreur)
  {
    GLint d ;
    glGetIntegerv(GL_ATTRIB_STACK_DEPTH, &d ) ;
    if ( d != 0 )
      eprintf("Y a un bug GL la pile d'attribut n'est pas vide ! : %d\n", d) ;
    glGetIntegerv(GL_MODELVIEW_STACK_DEPTH, &d ) ;
    if ( d != 1 )
      eprintf("Y a un bug GL la pile de matrice n'est pas vide ! : %d\n", d-1) ;
  }

  return (long)gl->gl ;
}

void Gl_stop_sans_swapping(Gl *gl)
{
  GLint d ;
  static int temps_dernier_affichage ;
  int temps_courant ;

  if ( !global_dans_gl )
    eprintf("2 Appels Gl_stop successifs\n") ;
  global_dans_gl = 0 ;

  glFlush() ;

  temps_courant = time(0) ;
  if ( 1 || temps_courant != temps_dernier_affichage )
    {
      temps_courant = temps_dernier_affichage ;
      glGetIntegerv(GL_ATTRIB_STACK_DEPTH, &d ) ;
      if ( d != 0 )
	eprintf("Vous avez oubliez de faire %d glPopAttrib()\n", d) ;
      glGetIntegerv(GL_MODELVIEW_STACK_DEPTH, &d ) ;
      if ( d != 1 )
	eprintf("Vous avez oubliez de faire %d glPopMatrix()\n", d-1) ;
    }
}

void Gl_stop(Gl *gl)
{
  Gl_stop_sans_swapping(gl) ;
  widget_swap_gl(gl->gl) ;
}

static PROTOTYPE_CALLBACK(u_callback_gl_resize, w, c)
{
  Interface_widget iw ;

  if ( u_initialiser_iw(&iw, w, c, NULL) ) FIN_CALLBACK ;
  OBJET(iw.objet)->urgent = Vrai ;
  /*  evaluation(iw.objet) ; */
  FIN_CALLBACK ;
}

const char * Gl_aide(const Interface_widget *iw)
{
  static Chaine s ;

  if ( iw->ch->te_aide )
    return( (*iw->ch->te_aide)(iw) ) ;
  else
    {
      s_efface(&s) ;
      s_printf(&s, "%s\n\nPosition souris : %d %d\n",
	       u_aide_champ(iw->objet, iw->ch), iw->x, iw->y) ;
      return(s_char(&s)) ;
    }
}


NanoWidget Gl_creer(const Interface_widget* iw)
{
  NanoWidget w ;

  if ( TRACE2(iw->objet) )
    eprintf("GL_creer debut\n") ;
  w = widget_gl(iw->w, iw->ch->nom_champ) ;
  if ( TRACE2(iw->objet) )
    eprintf("GL_creer milieu\n") ;
  widget_add_event_button_press   (w, u_ev_button_press   , iw->ch) ;
  widget_add_event_key_press      (w, u_ev_key_press      , iw->ch) ;
  widget_add_event_expose         (w, u_callback_expose   , iw->ch) ;
  widget_add_event_resize         (w, u_callback_gl_resize, iw->ch) ;
  widget_add_event_motion         (w, u_ev_motion_na      , iw->ch);
  if ( TRACE2(iw->objet) )
    eprintf("GL_creer fin\n") ;

  return(w) ;
}

void Gl_stop_classe(const Interface_widget *iw)
{
  if ( globale_contexte )
    {
      widget_destroy_context(iw->w, globale_contexte) ;
      globale_contexte = 0 ;
    }
}

void Gl_init(const Interface_widget *iw)
{
  if ( TRACE(iw->objet) )
    eprintf("GL INIT\n") ;

  T(Gl,iw->objet,iw->ch).gl = iw->w ;
}

TYPE_DE_BASE(codec_gl, Gl, codec_rien, codec_rien, codec_rien, codec_rien)

DEFINITION_TYPE_EDITEUR(E_gl, E_void,
			"Zone d'affichage GL générique",
			{
                          FONCTION_EDITEUR(te->creer       ,Gl_creer         );
                          FONCTION_EDITEUR(te->init        ,Gl_init          );
                          FONCTION_EDITEUR(te->stop_classe ,Gl_stop_classe   );
                          FONCTION_EDITEUR(te->aide        ,Gl_aide          );
                          FONCTION_EDITEUR(te->lit         ,Jamais_pareil_lit);
			  c->options &= ~(O_Sauve | O_Initialiser | O_Label) ;
			}
			)

DEFINITION_TYPE_LOGIQUE(L_gl, L_void,
			"Widget GL",
			init_E_gl(c) ;
			c->tp = &infos_codec_gl ;
			)
