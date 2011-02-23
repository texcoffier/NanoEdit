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
#include <math.h>
#include <GL/gl.h>		/* Car il affiche du GL */
#include "o_objet.h"
#include "t_geometrie.h"
#include "u_vue.h"
#include "u_graphique.h"
#include "u_etat_courant.h"
#include "stl.h"


struct ifs_test
{
  int prof_recur;
  Table_transfo transfos;
};



static void display_line(Triplet p,Triplet q)
{
  printf("Affichage d'une ligne rouge\n");
  glColor3f(1.,0.,0.);
  glBegin(GL_LINES);
  glVertex3f(p.x,p.y,p.z); 
  glVertex3f(q.x,q.y,q.z); 
  
  glEnd();
}



static void ligne_initiale()
{
  glColor3f(1.,0.,0.);
  glBegin(GL_LINES);
  glVertex3f(-1.,-1.,0.);
  glVertex3f(1.,1.,0.);
  glEnd();
}


static void affiche_triplet(Triplet p)
{
  printf("x->%f,y->%f,z->%f\n",p.x,p.y,p.z);
}


static void afficher(const struct ifs_test *o, int prof
	      , const Transfo *t)
{
  int i ;
  Transfo tmp ;
  Triplet p, q, pp,bas_gauche,haut_droit;

  bas_gauche.x=-1.;
  bas_gauche.y=-1.;
  bas_gauche.z=0.;

  haut_droit.x=1.;
  haut_droit.y=1.;
  haut_droit.z=0.;

  printf("afficher profondeur:%d, nb_transfos:%d\n",prof,o->transfos.nb);

  for(i=0; i<o->transfos.nb; i++)
    {
      tmp = u_produit_transfo(t, &o->transfos.table[i]) ;

      if ( prof )
	{
	 
	      afficher(o,prof-1,&tmp);
	}
      else
	{
	  affiche_triplet(p);
	  affiche_triplet(q);
	  p = u_transforme(&tmp, &bas_gauche) ;
	  q = u_transforme(&tmp, &haut_droit) ;
	  display_line(p,q);
	}
    }
}

static void afficher_ifs(struct ifs_test *o)
{
 Transfo identite ;

 u_identite_transfo(&identite) ;

 ligne_initiale();

 afficher(o,o->prof_recur,&identite);

}



CLASSE(ifs_test,struct ifs_test,
       CHAMP(prof_recur, L_nombre_positif P_entier Edite Sauve DEFAUT("3"))
       CHAMP(transfos,L_table_transfo P_table_transfo Edite Sauve Extrait) 
       CHAMP_VIRTUEL(L_affiche_gl(afficher_ifs))
       )
