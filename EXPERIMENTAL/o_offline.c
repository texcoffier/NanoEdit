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
#include <GL/gl.h>
#include <GL/glu.h>
#include "t_gl.h"
#include "o_objet.h"
#include "t_geometrie.h"
#include "u_graphique.h"

#define L 1024
#define H 1024

struct offline
{
  Gl gl ;
  char image[H][L][3] ;
  int nb_iter ;
} ;

static void image()
{
  glBegin(GL_QUADS) ;
  glTexCoord2f(0.,0.) ; glVertex2f(0.,0.) ;
  glTexCoord2f(1.,0.) ; glVertex2f(1.,0.) ;
  glTexCoord2f(1.,1.) ; glVertex2f(1.,1.) ;
  glTexCoord2f(0.,1.) ; glVertex2f(0.,1.) ;
  glEnd() ;
}


static void offline_trace(const Interface_widget *iw)
{
  struct offline *o = iw->objet ;

  Gl_start(&o->gl) ;

  gluBuild2DMipmaps(GL_TEXTURE_2D, 3, L, H, GL_RGB, GL_UNSIGNED_BYTE,o->image);

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP) ;
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP) ;
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR) ;
  glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE) ;

  glEnable(GL_BLEND) ;
  glBlendFunc(GL_SRC_COLOR, GL_DST_COLOR) ;

  glClearColor(0.,0.,0.,0.) ;
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) ;


  glEnable(GL_TEXTURE_2D) ;

  glScalef(0.9,0.9,0.9) ;

  glTranslatef(-0.9, -0.9, 0) ;
  image() ;
  glTranslatef(0.9, 0, 0) ;
  image() ;
  glTranslatef(0, 0.9, 0) ;
  image() ;
  glRotatef(10,0,0,1) ;
  glTranslatef(-0.9, 0, 0) ;
  image() ;

  glDisable(GL_TEXTURE_2D) ;


  Gl_stop(&o->gl) ;
}

static void offline_changement(struct offline *s)
{
  Interface_widget iw ;
  int i ;

  if ( s->gl.gl )
    {
      iw.objet = s ;
      
      memset(s->image, 255, L*H*3) ;

      for(i=0; i<s->nb_iter; i++)
	{
	  offline_trace(&iw) ;
	  glReadPixels(0, 0, L, H, GL_RGB, GL_UNSIGNED_BYTE, s->image) ;
	}
      
    }
}

static void offline_affiche(struct offline *s, const Champ *ch, const char *q)
{
  //  glTexParameterf(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_EXT, GL_TRUE) ;

  gluBuild2DMipmaps(GL_TEXTURE_2D, 3, L, H, GL_RGB, GL_UNSIGNED_BYTE, s->image);

  //  glTexImage2D(GL_TEXTURE_2D, 0, 3, L, H, 0 , GL_RGB, GL_UNSIGNED_BYTE,
  //       s->image) ;
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP) ;
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP) ;
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR) ;
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST) ;
  glEnable(GL_TEXTURE_2D) ;
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE) ;

  glBegin(GL_QUADS) ;
  glTexCoord2f(0.,0.) ; glVertex2f(0.,0.) ;
  glTexCoord2f(1.,0.) ; glVertex2f(1.,0.) ;
  glTexCoord2f(1.,1.) ; glVertex2f(1.,1.) ;
  glTexCoord2f(0.,1.) ; glVertex2f(0.,1.) ;
  glEnd() ;

  glDisable(GL_TEXTURE_2D) ;
}


CLASSE(offline,
       struct offline,
       CHAMP(nb_iter, L_nombre P_entier Edite Sauve DEFAUT("5"))
       CHAMP(gl,
	     L_gl Edite
	     TE_HAUTEUR(H) TE_LARGEUR(L)
	     TE_ECRIT(offline_trace))
       CHAMP_VIRTUEL(L_affiche_gl(offline_affiche)
		     AFFICHAGE(Toujours_different)
		     )
       CHANGEMENT(offline_changement)
      )
