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
#define NB 10
#define NUMERO_TAG 1000 /* Pour éviter les conflits avec les # de champs */
typedef float CouleurNB[NB][3] ; /* Le type de ma table de couleur */
struct segment
   {
     Triplet depart, arrivee ;
     CouleurNB couleur ;	/* Couleur des sous-objets */
   } ;
/* Un type physique est créé car c'est une table
 * de longueur très particulière.
 * Le type physique est fait pour que
 * le tableau soit initialisé à 1 pour donner du blanc. */
TYPE_PHYSIQUE(codec_couleurNB,	/* Le nom du type physique */
	      CouleurNB,	/* La structure C associée au type physique */
	      codec_l(a,	/* Table de taille fixe */
		      NB*3,	/* La taille */
		      codec_float1 /* Flottant avec valeur par défaut 1 */
		      ) ;
	      )
#define P_couleurNB TP(codec_couleurNB)

static Triplet point(Triplet *a, Triplet *b, float i) /* Utilitaire */
{
  i /= NB ;
  return( u_ponderation_2_triplet(a, i,   b, 1-i) ) ;
}
static void affiche_segment(struct segment *s)
{
  int i ;
  Triplet pt ;
  glPushAttrib(GL_CURRENT_BIT) ; /* Sauve couleur courante */
  glBegin(GL_LINES) ;
  for(i=0;i<NB;i++)
    {
      glColor3f(s->couleur[i][0], s->couleur[i][1], s->couleur[i][2]) ; 
      pt = point( &s->depart, &s->arrivee, i+.1) ;
      t_triplet(&pt) ;
      pt = point( &s->depart, &s->arrivee, i+1-.1) ;
      t_triplet(&pt) ;
    }
  glEnd();
  if ( ACTIF(s) ) /* Pour ne pas surcharger l'affichage */
    for(i=0;i<NB;i++)
      {
	pt = point( &s->depart, &s->arrivee, i+.5) ;
	t_tag(NUMERO_TAG+i, &pt) ;
      }
  glPopAttrib() ;		/* Restaure couleur courante */
}
/* La fonction suivante permet de changer la valeur du tag,
 * dans notre cas on change la couleur. */
static void ecrit_tag(struct segment *s, const Champ *c, const char *v)
{
  int i ;
  Triplet couleur ;

  i = lit_numero_tag(c) - NUMERO_TAG ;
  if ( i >= 0 && i < NB ) /* En cas de bug... */
    {
      couleur = char_vers_triplet(v) ;
      s->couleur[i][0] = couleur.x ;
      s->couleur[i][1] = couleur.y ;
      s->couleur[i][2] = couleur.z ;
    }
}
/* Lecture de la valeur du tag */
static const char * lit_tag(struct segment *s, const Champ *c)
{
  static Buffer chaine ;
  int i ;
  chaine[0] = '\0' ;
  i = lit_numero_tag(c) - NUMERO_TAG ;
  if ( i >= 0 && i < NB ) /* En cas de bug... */
    {
      sprintf(chaine, "Couleur = %g %g %g",
	      s->couleur[i][0], s->couleur[i][1], s->couleur[i][2]) ;
    }
  return(chaine) ;
}
CLASSE(segment22,
       struct segment,
       Edite Sauve
       CHAMP(depart     , L_point P_triplet                 Init3d Extrait)
       CHAMP(arrivee    , L_point P_triplet                Init3d Extrait)
       CHAMP(couleur    , L_table P_couleurNB               )
       CHAMP_VIRTUEL(     L_tag(ecrit_tag,lit_tag)               )
       CHAMP_VIRTUEL(     L_affiche_gl(affiche_segment)
			  AFFICHAGE(Different_si_actif)         )
      )

