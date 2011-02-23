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
#include "u_vue.h"
#include "u_graphique.h"
#include "t_geometrie.h"
#include <GL/gl.h>
#include "u_etat_courant.h"
#include "u_modele.h"

/*
 * test GL
 */

/*

#include <gtk/gtk.h>
#include <gtkgl/gtkglarea.h>


GtkWidget* wwidget_gl()
{
  GtkWidget *f ;

  static int attr[] = { GDK_GL_RGBA
			, GDK_GL_DOUBLEBUFFER
			, GDK_GL_STENCIL_SIZE, 1
			, GDK_GL_DEPTH_SIZE, 16
			, GDK_GL_NONE
  } ;

  f = gtk_gl_area_new(attr) ;

  return(f) ;
}

void test_gl()
{
  NanoWidget w, gl ;
  Liste *l ;
  int i ;
  int argc ;
  char *argv[] = { "ii" } ;
  char **argvv ;

  argc = 1 ;
  argvv = argv ;

  gtk_init(&argc, &argvv);
  gtk_rc_parse(".gtkrc") ;
  w = gtk_window_new (GTK_WINDOW_TOPLEVEL) ;
  gl = wwidget_gl() ;
  gtk_container_add (GTK_CONTAINER(w), GTK_WIDGET(gl)) ;
  gtk_widget_show(gl) ;
  gtk_widget_show(w) ;

  for(i=0; i<20; i++)
    gtk_main_iteration() ;

  gtk_gl_area_make_current(GTK_GL_AREA(gl)) ;
  glViewport(0,0,100,100) ;
  glMatrixMode(GL_MODELVIEW) ;
  glLoadIdentity() ;
  glMatrixMode(GL_PROJECTION) ;
  glLoadIdentity() ;
  glMatrixMode(GL_MODELVIEW) ;

  for(;;)
    {
      glClearColor(.5,.5,.5,1) ;
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) ;
      {
	GLfloat p[4] ;
	glColor3f(1,1,1) ;
	p[0] = 1 ;
	p[1] = 1 ;
	p[2] = 1 ;
	p[3] = 1 ;
	glLightfv(GL_LIGHT0,GL_POSITION,p ) ;
	glLightfv(GL_LIGHT0,GL_DIFFUSE,p ) ;
	glEnable(GL_LIGHT0) ;
	glEnable(GL_LIGHTING) ;
	glBegin(GL_TRIANGLES) ;
	glNormal3f(0,0,1) ;
	glVertex3f(0,0,0) ;
	glVertex3f(1,0,0) ;
	glVertex3f(1,1,0) ;
	glEnd() ;
	glDisable(GL_LIGHTING) ; 
	glDisable(GL_LIGHT0) ;
	
	glBegin(GL_LINES) ;
	glVertex3f(0,0,0) ;
	glVertex3f(-5,-5,-5) ;
	glEnd() ;
      }
      gtk_gl_area_swap_buffers(GTK_GL_AREA(gl)) ;
    }
}

*/

struct globale_etat_courant globale_etat_courant =
{ {1,1,1},			/* couleur */
  .8,				/* Diffuse */
 .1,				/* Spéculaire */
 {.1,.1,.1},			/* Ambiante */
  0,				/* Réflexion */
  0,				/* Réfraction */
  2,				/* Indice réfraction */
  4,				/* État surface */
  Facette,			/* Type affichage */
  { {1,0,0}, {0,1,0}, {0,0,1}, {0,0,0} }, /* Transformation */
  0,				/* N° Lampe libre */
  0.1,				/* Épaisseur trait */
  NULL,				/* Opérateur racine */
  NULL,				/* Pas de détournement d'affichage */
  {12, 6, 6},			/* Découpage palette HLS */
  Vrai,				/* Affiche les lignes de rappel */
  Faux,				/* L'objet est désignable */
  Point_de_R3,			/* Palette couleur */
  .2,				/* Offset pour STL */
  /*
   * les priorités initiales sont toutes à 0
   */

} ;


int alloue_plan_decoupage_gl(GLdouble plan[4])
{
  int i ;

  for(i=GL_CLIP_PLANE0; i<GL_CLIP_PLANE0+GL_MAX_CLIP_PLANES; i++)
    if ( !glIsEnabled(i) )
      {
	glClipPlane(i, plan) ;
	glEnable(i) ;
	return(i) ;
      }
  eprintf("Trop de plan de découpage GL, affichage incorrecte\n") ;
  return(-1) ;
}

void libere_plan_decoupage_gl(int i)
{
  if ( i != -1 )
    glDisable(i) ;
}


const Liste* fils_a_afficher2(const void *o, Booleen *pas_de_fils)
{
  Liste *l ;

  l = OBJET(o)->fils ;
  /*
   * Saute les fils NULL
   */
  while( l && l_element(l,0) == NULL )
    l = l->suivant ;
  /*
   * S'il y a un premier fils, on saute l'observateur
   */
  if ( l )
    {
      if ( c_type_existe(l_element(l,0), &infos_L_observateur) )
	l = l->suivant ;
    }
  /*
   * Si c'est nul, on affiche tout
   */
  if ( l == NULL )
    {
      *pas_de_fils = Vrai ;
      l = contenu_modele( u_operateur_ou_modele(o) ) ;
    }
  else
    *pas_de_fils = Faux ;

  return(l) ;
}

const Liste* fils_a_afficher(const void *o)
{
  Booleen inutile ;
  return( fils_a_afficher2(o, &inutile) ) ;
}

const char* me_prevenir(const void *o, const Champ *ch)
{
  Booleen pas_de_fils ;

  if ( OBJET(o)->creation )
    return("création") ;
  if ( OBJET(o)->edite == NULL )
    return("0") ; /* Pour les objets non-édité */
  fils_a_afficher2(o, &pas_de_fils) ;

  if ( TRACE(o) )
    eprintf("me_prevenir edite=%d fils=%x modele=%x pas_de_fils=%d\n",
	    OBJET(o)->edite, fils_a_afficher(o),
	    u_operateur_ou_modele((void*)o), pas_de_fils
	    ) ;
  if ( pas_de_fils )
    return("1") ;
  else
    return("0") ;
}


void u_empile_et_multiplie_line_width(float v)
{
  GLfloat w ;

  glPushAttrib(GL_LINE_BIT) ;
  glGetFloatv(GL_LINE_WIDTH, &w) ;
  glLineWidth(v*w) ;
}

static int  globale_gl_a_demarre = 0 ;

void destruction_liste_objet(void *o)
{
  if ( TRACE2(o) )
    eprintf("Destruction liste : %ld-%ld %ld-%ld\n", (long)o,(long)o+ 2*3*10,
	    (long)OBJET(o)->classe, (long)OBJET(o)->classe+2*3*10) ;

  /*
   * 2*3*10 : actif/inactif ponctuel/filaire/facette qualites
   */

  if ( globale_gl_a_demarre )
    {
      glDeleteLists(OBJECT_ID(o), 2*3*10) ;
      glDeleteLists(OBJET(o)->classe->id, 2*3*10) ;
    }
}

static unsigned int globale_position_arbre ;

void Gl_Affiche_Erreur(void *f, const Champ *ch, const char *message)
{
#define GLER(X) case X: eprintf(#X) ; break ;
 
  switch ( glGetError() )
    {
    case 0: if ( f && TRACE2(f) ) eprintf("OK : %s %s\n",  OBJET(f)->classe->nom , message) ; 
		return ;

	GLER(GL_INVALID_VALUE)
	GLER(GL_INVALID_ENUM)
	GLER(GL_INVALID_OPERATION)
	GLER(GL_STACK_OVERFLOW)
	GLER(GL_STACK_UNDERFLOW)
	GLER(GL_OUT_OF_MEMORY)
    }

  if ( f )
    eprintf(" objet:%s", OBJET(f)->classe->nom ) ;

  if ( ch )
    eprintf(" champ:%s", ch->nom_champ ) ;

  if ( message )
    eprintf(" %s", message) ;

  eprintf("\n") ;
}



void u_affiche_objet_(void *f, const Champ *ch, const char *urgent)
{
  Quadruplet pt ;
  GLint asd, msd ,d ;

  if ( globale_etat_courant.type_affichage == AfficheDebug )
    {
      u_annule_quadruplet(&pt) ;
      o_prend_position(f, &pt, 0) ;
      glRasterPos3f(pt.x, pt.y, pt.z) ;
      t_chaine( OBJET(f)->classe->nom ) ;
    }
  Gl_Affiche_Erreur(NULL, NULL, "en dehors de l'affichage des objets\n") ;

  glGetIntegerv(GL_ATTRIB_STACK_DEPTH, &asd ) ;
  glGetIntegerv(GL_MODELVIEW_STACK_DEPTH, &msd ) ;
  c_sauve_ici(f, ch, urgent) ;
  Gl_Affiche_Erreur(f, ch, NULL) ;
  glGetIntegerv(GL_ATTRIB_STACK_DEPTH, &d ) ;
  if ( d != asd )
    eprintf("%s : problème dans la pile d'attribut GL (avant %d apres %d)\n",
	    OBJET(f)->classe->nom, asd, d) ;
  glGetIntegerv(GL_MODELVIEW_STACK_DEPTH, &d ) ;
  if ( d != msd )
    eprintf("%s : problème dans la pile de matrices GL (avant %d apres %d)\n",
	    OBJET(f)->classe->nom, msd, d) ;
}

void u_affiche_objet(void *f, const char *urgent)
{
  GLuint i ;
  Objet *f_ ;
  const Champ *ch ;
  GLint dedans ;

  Gl_Affiche_Erreur(f, NULL, "Début u_affiche_objet\n") ;

  glGetIntegerv(GL_LIST_INDEX, &dedans) ;
  dedans = 1 ;

  if ( !c_trouve_type((const void**)&f, &ch, &infos_L_affiche_gl) )
    {
      POUR_LES_FILS(f, ff,
      {
	if ( OBJET(ff)->visible )
	  u_affiche_objet(ff, urgent) ;
      }
		    ) ;
      return ;
    }

  globale_gl_a_demarre = 1 ;

  f_ = OBJET(f) ;

  if ( TRACE2_(f_) )
    eprintf("u_affiche_objet %p pos=%d designable=%d detourne=%p\n"
	    , f
	    , globale_position_arbre
	    , globale_etat_courant.designable
	    , globale_etat_courant.detourne_affichage) ;


  if ( globale_etat_courant.designable )
    {
      glInitNames() ;
      glPushName((GLuint)globale_position_arbre++) ;
      if ( globale_etat_courant.operateur_racine )
	glPushName(OBJECT_ID(globale_etat_courant.operateur_racine)) ;
      else
	glPushName(OBJECT_ID(f)) ;
    }

  if ( f_->actif )
    {
      glPushAttrib(GL_LINE_BIT|GL_POINT_BIT|GL_LIGHTING_BIT) ;
      u_multiplie_epaisseur_ligne(2) ;
      u_multiplie_taille_point(2) ;
      glShadeModel(GL_FLAT) ;
    }

  if ( globale_etat_courant.detourne_affichage && globale_etat_courant.designable )
    {
      (*globale_etat_courant.detourne_affichage)(f, globale_position_arbre-1) ;
    }

  Gl_Affiche_Erreur(f, NULL, "Milieu 1 u_affiche_objet\n") ;

  /*
   * On n'utilise pas de liste si actif car cela ne marche
   * pas avec les TAG qui sont calculés en fonction de la vue.
   */
  if ( globale_etat_courant.designable
       || f_->actif
       || (ch->affichage & Toujours_different)
       || dedans
       )
    {
      if ( TRACE_(f_) )
	eprintf("Pas de Creation liste : (d=%d,a=%d,td=%d,ta=%d) %s\n",
		globale_etat_courant.designable, f_->actif,
		(ch->affichage & Toujours_different),
		globale_etat_courant.type_affichage,
		f_->classe->nom
		) ;
      u_affiche_objet_(f, ch, urgent) ;
    }
  else
    {
      /* Voir DeleteLists pour mettre à jour en même temps */
      if ( ch->affichage & Toujours_identique )
	i = f_->classe->id ;
      else
	i = OBJECT_ID(f) ;
      if ( ch->affichage & Different_si_actif )
	i++ ;
      if ( ch->affichage & Different_suivant_type_affichage )
	i += 2*globale_etat_courant.type_affichage ;
      if ( ch->affichage & Different_suivant_qualite )
	i += 2*3*(urgent[0]-'2') ;

      if ( glIsList(i) )
	{
	  glCallList(i) ;
	}
      else
	{
	  if ( TRACE_(f_) )
	    eprintf("Creation liste : %d\n", i) ;

	  glNewList(i, GL_COMPILE_AND_EXECUTE) ;
	  u_affiche_objet_(f, ch, urgent) ;
	  glEndList() ;
	}
    }

  POUR_LES_FILS(f, ff,
  {
    if ( OBJET(ff)->visible )
      u_affiche_objet(ff, urgent) ;
  }
		) ;



  if ( f_->actif )
    {
      glPopAttrib() ;
    }

  Gl_Affiche_Erreur(f, NULL, "Fin u_affiche_objet\n") ;
}

void affiche_gl_recursif(const Liste *a_afficher, const char *urgent)
{
  char *etat ;
  Booleen modif ;

  Gl_Affiche_Erreur(NULL, NULL, "en dehors de affiche_gl_recursif\n") ;

  etat = u_sauve_etat() ;

  Gl_Affiche_Erreur(NULL, NULL, "dans sauve etat\n") ;

  if ( globale_etat_courant.operateur_racine == NULL )
    globale_position_arbre = 1000 ;

  POUR_LES_ELEMENTS
    (a_afficher, f,
     {
       if ( f && strcmp(OBJET(f)->classe->nom,"lampe") == 0 )
	 {
	   c_sauve_type(f, &infos_L_applique, (char*)&modif) ;
	   c_sauve_type(f, &infos_L_applique_gl, "") ;
	 }
     }) ;

  Gl_Affiche_Erreur(NULL, NULL, "dans les appliques initiaux\n") ;

  POUR_LES_ELEMENTS(a_afficher, f, u_affiche_objet(f, urgent) ; ) ;
  
  u_restaure_etat(etat) ;

  Gl_Affiche_Erreur(NULL, NULL, "dans restaure etat\n") ;
}

void affiche_gl(const Liste *a_afficher, const char *urgent)
{
  int i ;
  GLint max ;
  int *etat_lampe ;

  glPushAttrib(GL_ALL_ATTRIB_BITS) ;

  glGetIntegerv(GL_MAX_LIGHTS, &max) ;
  ALLOUER(etat_lampe, max) ;
  for(i=0;i<max;i++)
    {
      etat_lampe[i] = glIsEnabled(GL_LIGHT0+i) ;
      glDisable(GL_LIGHT0+i) ;
    }
  affiche_gl_recursif(a_afficher, urgent) ;

  for(i=0;i<max;i++)
    {
      if ( etat_lampe[i] )
	glEnable(GL_LIGHT0+i) ;
      else
	glDisable(GL_LIGHT0+i) ;
    }

  free(etat_lampe) ;

  glPopAttrib() ;
}

void affiche_modele(void *o, const char *urgent)
{
  affiche_gl( OBJET(u_operateur_ou_modele(o))->fils, urgent) ;
}
