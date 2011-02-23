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
#include <GL/gl.h>

typedef struct 
{
  Gl gl ;
} Createur ;


static void clavier(const Interface_widget *iw)
{
  Createur *c ;
  const char *v ;
  void *fils ;

  c = iw->objet ;
  v = iw->texte ;

  fils = o_objet(char_vers_classe("texte")) ;
  c_sauve(fils, "valeur", v) ;

  o_il_devient_mon_fils(c, fils, Faux) ;
}

static void affiche(const Interface_widget *iw)
{
  Createur *o = iw->objet ;

  Gl_start(&o->gl) ;
  glClearColor(0,1,0,0) ;
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) ;
  Gl_stop(&o->gl) ;
}

CLASSE(createur_de_fils, Createur,
       CHAMP(gl,
	     L_gl
	     Affiche
             TE_CLAVIER(clavier) 
	     TE_ECRIT(affiche)
	     TE_HAUTEUR(100)
	     )
       )
