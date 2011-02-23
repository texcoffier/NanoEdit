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
 * Faites l'affichage de la ligne brisée découpée
 * par le rectangle.
 *
 * Ne faites pas le découpage suivant Z si cela vous
 * donne trop de travail....
 * On suppose que toutes les valeurs ont un Z=0
 */

#include "o_objet.h"
#include "t_geometrie.h"
#include "u_graphique.h"
#include <GL/gl.h>

struct decoupage
{
  Table_triplet points ;	/* Ligne brisée à découper */
  Flottant cloture[6] ;		/* Xmin Xmax Ymin Ymax Zmin Zmax */
} ;

static int calcul_code(const Flottant *p, const Flottant cloture[6])
{
  /* EXCOIN */
  return(
	 ((p[0]<cloture[0]) ?  1 : ((p[0]>cloture[1]) ?  2 : 0 )) +
	 ((p[1]<cloture[2]) ?  4 : ((p[1]>cloture[3]) ?  8 : 0 )) +
	 ((p[2]<cloture[4]) ? 16 : ((p[2]>cloture[5]) ? 32 : 0 ))
	 ) ;
  /* EXCOOUT */
}

/*
 * Affiche la ligne brisée en la découpant (Découpage fait à chaque affichage)
 */
static void affiche_segment(const Flottant cloture[6], Triplet p, Triplet q)
{
  /* EXCOIN */
  Flottant abscisse_curviligne ;
  int i, bord, point_a_rapprocher, direction ;
  Flottant *tp[2] ;
  int code[2] ;

  /*
   * Pour simplifier la programmation on utilise des tableaux
   */
  tp[0] = (Flottant*)&p ;
  tp[1] = (Flottant*)&q ;
  code[0] = calcul_code(tp[0], cloture) ;
  code[1] = calcul_code(tp[1], cloture) ;

  for(bord=0; bord<7 ; bord++)	/* Pour tous les bords de la clôture  */
    {
      /*
       * Pour bord=7, le "if" suivant est toujours vrai
       */
      if ((code[0]|code[1])==0)       /* COMPLÈTEMENT DANS L'ECRAN */
	{
	  glBegin(GL_LINES) ;
	  t_triplet(&p) ;
	  t_triplet(&q) ;
	  glEnd() ;
	  return ;
	}
      if ((code[0]&code[1])!=0)       /* COMPLÈTEMENT A L'EXTÉRIEUR */
	{
	  return ;
	}
      if ( ( (code[0]^code[1]) & (1<<bord))!=0 )  /* À DÉCOUPER */
	{
	  /*
	   * Détermine la direction de découpage : X=0, Y=1 ou Z=2
	   */
	  direction = bord/2  ;
	  /*
	   * Abscisse curviligne de l'intersection
	   */
	  abscisse_curviligne = (cloture[bord]-tp[0][direction])
	                        / (tp[1][direction]-tp[0][direction]); 
	  /*
	   * Détermine le point à rapprocher
	   */
	  point_a_rapprocher = ( (code[1] & (1<<bord))  !=  0 ) ;
	  /*
	   * Calcul les coordonnées du point
	   */
	  for(i=0; i<3; i++)
	    tp[point_a_rapprocher][i] = abscisse_curviligne
	                                *(tp[1][i]-tp[0][i]) + tp[0][i] ;
	  /*
	   * Pour éviter les erreurs de calcul flottant
	   */
	   tp[point_a_rapprocher][direction] = cloture[bord] ;
	  /*
	   * Recalcule le code du point modifié.
	   * Plusieur bits à la fois peuvent être annulés.
	   */
	  code[point_a_rapprocher] = calcul_code(tp[point_a_rapprocher],
						 cloture) ;
	  /*
	   * Cas tordus d'imprécision numériques
	   */
	  if ( 0 )
	    code[point_a_rapprocher] =
	      ((code[point_a_rapprocher]>>bord)<<bord) ;
	}
    }
  eprintf("BUG!\n") ;
  /* EXCOOUT */
}


static void affiche_decoupage(struct decoupage *o)
{
  int i ;

  /*
   * Affiche la ligne découpée
   */
  for(i=1;i<o->points.nb;i++)
    affiche_segment(o->cloture, o->points.table[i-1], o->points.table[i]) ;
  /*
   *
   */
  glEnable(GL_LINE_STIPPLE);
  glLineStipple(1,0x101) ;
  /*
   * Affiche la ligne brisée non découpée
   */
  glBegin(GL_LINE_STRIP) ;
  for(i=0;i<o->points.nb;i++)
    glVertex2f(o->points.table[i].x, o->points.table[i].y) ;
  glEnd() ;
    
  /*
   * Affiche le rectangle (cube) de découpage
   */
  glPushMatrix() ;
  glTranslatef(o->cloture[0], o->cloture[2], o->cloture[4]) ;
  glScalef(o->cloture[1] - o->cloture[0],
	   o->cloture[3] - o->cloture[2],
	   o->cloture[5] - o->cloture[4]
	   ) ;
  glLineStipple(1,0xF0F) ;
  t_cube() ;
  glPopMatrix() ;
  /*
   *
   */
  glDisable(GL_LINE_STIPPLE);
}

/*
 * Definition de la classe
 */

CLASSE(decoupage, struct decoupage,
       Edite Extrait Sauve
       CHAMP(points, L_table_point3
	     DEFAUT("0 4\n"
		    "4 0\n"
		    "0 -4 0\n"
		    "-4 0\n"
		    "0 4\n"
		    "0 5\n"
		    "5 0\n"
		    "0 -5\n"
		    "-5 0\n"
		    "0 5\n"
		    "2 -5\n"
		    "0.25 0.25\n"
		    "20 20\n"
		    )
	     )
       Init3d /* Pour pouvoir modifier la boite intéractivement */
       L_nombre P_flottant
       CHAMP(cloture[0], LABEL("X min") DEFAUT("-2.3") )
       CHAMP(cloture[1], LABEL("X max") DEFAUT("2.3") )
       CHAMP(cloture[2], LABEL("Y min") DEFAUT("-2.3") )
       CHAMP(cloture[3], LABEL("Y max") DEFAUT("2.3") )
       CHAMP(cloture[4], LABEL("Z min") DEFAUT("-0.1") )
       CHAMP(cloture[5], LABEL("Z max") DEFAUT("0.1") )

       CHAMP_VIRTUEL(L_affiche_gl(affiche_decoupage))
       MENU("Demonstration/Découpage (clipping)")
       EVENEMENT("Shft+DE")
       )
