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
#include "t_geometrie.h"
#include "u_graphique.h"
#include <GL/gl.h> 
#define NB 10			/* Découpage du segment en NB parties */
#define NUMERO_TAG 1000
struct segment
   {
     Triplet depart, arrivee ;
     Liste *sous_actifs ;	/* L'objet doit prévoir le stockage */
   } ;
/* Calcul du point milieu de la partie "i" */
static Triplet point(Triplet *a, Triplet *b, float i) /* Utilitaire */
{
  i /= NB ;
  return( u_ponderation_2_triplet(a, i,   b, 1-i) ) ;
}
static void affiche_segment(struct segment *s)
{
  int i ;
  Triplet pt ;
  glPushName(0) ;		/* Il faut nommer les sous-objets */
  for(i=0;i<NB;i++)
    {
      glLoadName(NUMERO_TAG+i) ; /* Donne un nom aux graphismes suivants */
      glBegin(GL_LINES) ;	/* On doit mettre cette ligne après LoadName */
      pt = point( &s->depart, &s->arrivee, i+.1) ;
      t_triplet(&pt) ;
      pt = point( &s->depart, &s->arrivee, i+1-.1) ;
      t_triplet(&pt) ;
      glEnd();
    }
  glPopName() ;			/* Enlève ce que l'on a empilé */

  /* La boucle POUR permet de parcourir la liste des numéros de sous-actif */
  if ( ACTIF(s) ) /* Pour ne pas surcharger l'affichage */
    POUR(int, sous_objet_actif, s->sous_actifs,
	 {
	   /* On affiche un tag au milieu de chaque sous-segment actif */
	   pt = point(&s->depart,&s->arrivee,sous_objet_actif-NUMERO_TAG+.5) ;
	   t_tag(sous_objet_actif, &pt) ;
	 }) ;
}
CLASSE(segment21,
       struct segment,
       Edite Sauve
       CHAMP(depart     , L_point P_triplet        Init3d Extrait)
       CHAMP(arrivee    , L_point P_triplet        Init3d Extrait)
       CHAMP(sous_actifs, L_sous_actif                           ) /**/
       CHAMP_VIRTUEL(     L_affiche_gl(affiche_segment)
			  AFFICHAGE(Different_si_actif)  /**/    )
      )

