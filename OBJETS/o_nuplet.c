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
#include <GL/gl.h>		/* Car il affiche du GL */
#include "o_objet.h"
#include "t_geometrie.h"
#include "u_graphique.h"

struct mon_triplet
{
  Triplet triplet ;	/* Les coordonnees du point */
} ;

struct mon_quadruplet
{
  Quadruplet quadruplet ;	/* Les coordonnees du point */
} ;


/*
 * Cette fonction sert a afficher un objet quelconque
 * dont le premier champ apres "objet" est un point.
 * C'est le cas pour le point, la couleur, le point_poids,...
 */

static void affiche_point(Triplet *o, const Champ *ch, const char *urgent)
{
  if ( ACTIF(o) )
    {
      t_tag_origine(0, o) ;
    }
  else
    {
      glBegin(GL_POINTS) ;		/* On veut afficher des points */
      glVertex3f(o->x, o->y, o->z) ;
      glEnd();			/* On arrete d'afficher des points */
    }
}

static void affiche_point4(Triplet *o, const Champ *ch, const char *urgent)
{
  if ( ACTIF(o) )
    {
      t_tag_origine(0, o) ;
      t_tag_poids(4, o) ;
    }
  else
    {
      glBegin(GL_POINTS) ;		/* On veut afficher des points */
      glVertex3f(o->x, o->y, o->z) ;
      glEnd();			/* On arrete d'afficher des points */
    }
}

static void affiche_vecteur(Triplet *o, const Champ *ch, const char *urgent)
{
  glBegin(GL_LINES) ;
  glVertex3f(0.,0.,0.) ;
  glVertex3f(o->x, o->y, o->z) ;
  glEnd();
  affiche_point(o, ch, urgent) ;
}

/*
 * Information
 */
/*
{
  static Buffer info ;
  sprintf(info, "%.3g,%.3g,%.3g", o->x, o->y, o->z) ;
  return(info) ;
}

{
  static Buffer info ;
  sprintf(info, "%.3g,%.3g,%.3g,%.3g", o->x, o->y, o->z, o->h) ;
  return(info) ;
}
*/


/*
 * Le point peut etre contraint par 3 flottants ou
 * bien par la position du curseur sur l'ecran
 */
CLASSE(point3, struct mon_triplet,
       EVENEMENT("0p03")
       MENU("Figures/0d/Point")
       BULLE_D_AIDE
       ("Point de R3 paramétrable par 3 nombres")
       Init3d
       CHAMP(triplet, L_point3)
       L_nombre P_flottant Extrait Edite Sauve
       CHAMP(triplet.x,NOM("x") LABEL("Px"))
       CHAMP(triplet.y,NOM("y") LABEL("Py"))
       CHAMP(triplet.z,NOM("z") LABEL("Pz"))
       NonInit3d NonExtrait
       CHAMP_VIRTUEL(L_affiche_gl(affiche_point)
		     AFFICHAGE(Different_si_actif)
		     AFFICHAGE(Toujours_different)  /* Pas de liste pour 1pt */
		     )
       )
CLASSE(point4, struct mon_quadruplet,
       EVENEMENT("0p04")
       MENU("Figures/0d/Point")
       BULLE_D_AIDE
       ("Point de R3 avec un poids paramétrable par 4 nombres")
       Init3d
       CHAMP(quadruplet, L_point4 P_quadruplet1)
       L_nombre P_flottant Extrait Edite Sauve
       CHAMP(quadruplet.x,NOM("x") LABEL("Px"))
       CHAMP(quadruplet.y,NOM("y") LABEL("Py"))
       CHAMP(quadruplet.z,NOM("z") LABEL("Pz"))
       CHAMP(quadruplet.h,NOM("h") LABEL("Ph") DEFAUT("1") )
       NonInit3d NonExtrait
       CHAMP_VIRTUEL(L_affiche_gl(affiche_point4)AFFICHAGE(Different_si_actif))
       )
/*
 * Transformation d'un triplet en un point
 */
CLASSE(point_triplet, struct mon_triplet,
       SUR_CLASSE("point3") HERITE_PAS_DES_CHAMPS
       BULLE_D_AIDE
       ("Objet traduisant un triplet (vecteur, couleur, ...) en point")
       CHAMP(triplet,
	     L_point3
	     Extrait Obligatoire Init3d Edite Sauve
	     TYPE_DANS_FILS(L_triplet)
	     )
       CHAMP_VIRTUEL(L_affiche_gl(affiche_point) AFFICHAGE(Different_si_actif))
       )
/*
 * Vecteur
 */
CLASSE(vecteur3, struct mon_triplet,
       EVENEMENT("0v03")
       MENU("Figures/0d/Vecteur")
       BULLE_D_AIDE("Vecteur de R3 paramétrable par 3 nombres")
       Init3d
       CHAMP(triplet, L_vecteur3)
       L_nombre P_flottant Extrait Edite Sauve
       CHAMP(triplet.x, NOM("x") LABEL("Vx"))
       CHAMP(triplet.y, NOM("y") LABEL("Vy"))
       CHAMP(triplet.z, NOM("z") LABEL("Vz"))
       NonInit3d NonExtrait
       CHAMP_VIRTUEL(L_affiche_gl(affiche_vecteur)
		     AFFICHAGE(Different_si_actif))
       )
CLASSE(vecteur4, struct mon_quadruplet,
       EVENEMENT("0v04")
       MENU("Figures/0d/Vecteur")
       BULLE_D_AIDE("Vecteur de R3 avec un poids paramétrable par 4 nombres")
       Init3d
       CHAMP(quadruplet, L_vecteur4)
       L_nombre P_flottant Extrait Edite Sauve
       CHAMP(quadruplet.x, NOM("x") LABEL("Vx"))
       CHAMP(quadruplet.y, NOM("y") LABEL("Vy"))
       CHAMP(quadruplet.z, NOM("z") LABEL("Vz"))
       CHAMP(quadruplet.h, NOM("h") LABEL("Vh"))
       NonInit3d NonExtrait
       CHAMP_VIRTUEL(L_affiche_gl(affiche_vecteur)
		     AFFICHAGE(Different_si_actif))
       )
/*
 * Transformation d'un triplet en un vecteur
 */
CLASSE(vecteur_triplet, struct mon_triplet,
       SUR_CLASSE("vecteur3") HERITE_PAS_DES_CHAMPS
       BULLE_D_AIDE
       ("Objet traduisant un triplet (vecteur, couleur, ...) en vecteur")
       CHAMP(triplet,
	     L_vecteur3
	     Extrait Obligatoire Init3d Edite Sauve
	     TYPE_DANS_FILS(L_triplet)
	     )
       CHAMP_VIRTUEL(L_affiche_gl(affiche_vecteur)
		     AFFICHAGE(Different_si_actif))
       )

/*
 *
 */

typedef struct
{
  Quadruplet barycentre ;	/* Obligatoire ici car sous-classe de point */
  Table_quadruplet t ;
} Barycentre ;

static void changement_barycentre(Barycentre *o)
{
  int i ;
  Triplet somme, tmp ;
  Flottant poids ;

  if ( TRACE(o) )
    eprintf("Nb points = %d\n", o->t.nb) ;
  if ( !CHAMP_CHANGE(o, t) )
    return ;

  u_annule_triplet(&somme) ;
  i = 0 ;
  poids = 0 ;
  for(i=0;i<o->t.nb;i++)
    {
      tmp = u_triplet_fois_flottant((Triplet*)&o->t.table[i],
				  o->t.table[i].h) ;
      somme = u_ajoute_triplet(&somme,&tmp) ;
      poids += o->t.table[i].h ;
    }
  if ( poids ) 
    {
      tmp = u_triplet_fois_flottant(&somme, 1./poids) ;
      o->barycentre.x = tmp.x ;
      o->barycentre.y = tmp.y ;
      o->barycentre.z = tmp.z ;
      o->barycentre.h = poids ;
    }
  if ( TRACE(o) )
    eprintf("Barycentre = %s\n", quadruplet_vers_char(&o->barycentre)) ;
}

CLASSE(barycentre4, Barycentre,
       BULLE_D_AIDE
       ("Barycentre d'un ensemble de points\n"
	"\n"
	"Si les points ont un poids, on en tient compte.\n"
	"Le poids du barycentre est la somme des poids des points.\n")
       SUR_CLASSE("point4") HERITE_PAS_DES_CHAMPS
       CHAMP(barycentre, L_point P_quadruplet Affiche)
       CHAMP(t.nb, L_entier Affiche)
       CHAMP(t   , L_table_point P_table_quadruplet1
	     Edite Sauve Extrait Obligatoire)
       CHAMP_VIRTUEL(L_affiche_gl(affiche_point)
		     AFFICHAGE(Different_si_actif))
       CHANGEMENT(changement_barycentre)
       MENU("Figures/0d/Barycentre4")
       )

/*
 *
 */

typedef struct
{
  Flottant distance ;		/* Valeur de la distance */
  Triplet p, q ;
}  Distance_pt_pt ;

static void changement_distance_pt_pt(Distance_pt_pt *o)
{
  o->distance = u_distance_triplet(&o->p, &o->q) ;
}

CLASSE(distance_pt_pt, Distance_pt_pt,
       BULLE_D_AIDE
       ("Distance entre 2 points.\n"
	)
       CHAMP(distance, L_nombre_positif P_flottant Affiche Init3d)
       CHAMP(p, L_point P_triplet Extrait Obligatoire Sauve Edite)
       CHAMP(q, L_point P_triplet Extrait Obligatoire Sauve Edite)
       CHANGEMENT(changement_distance_pt_pt)
       MENU("Valeurs/Distance")
       EVENEMENT("0d0p")
       )


/*
 *
 */

typedef struct
{
  Triplet resultat ;
  Triplet v1, v2 ;
}  Paire_vecteur ;

static void changement_pv(Paire_vecteur *o)
{
  o->resultat = u_pv_triplet(&o->v1, &o->v2) ;
}

CLASSE(pv, Paire_vecteur,
       BULLE_D_AIDE
       ("Produit vectoriel.\n"
	)
       CHAMP(resultat, L_vecteur3 Affiche Init3d)
       CHAMP(v1, L_vecteur P_triplet Affiche Extrait Obligatoire Sauve)
       CHAMP(v2, L_vecteur P_triplet Affiche Extrait Obligatoire Sauve)
       CHANGEMENT(changement_pv)
       CHAMP_VIRTUEL(L_affiche_gl(affiche_vecteur)
		     AFFICHAGE(Different_si_actif))
       MENU("Valeurs/Produit Vectoriel")
       EVENEMENT("0p0v")
       )

/*
 *
 */

typedef struct
{
  Triplet resultat ;
  Triplet v ;
}  Un_vecteur ;

static void changement_normalisation(Un_vecteur *o)
{
  o->resultat = u_normalise_triplet(&o->v) ;
}

CLASSE(normalise_vecteur, Un_vecteur,
       BULLE_D_AIDE
       ("Normalise vecteur.\n"
	)
       CHAMP(resultat, L_vecteur3 Affiche Init3d)
       CHAMP(v, L_vecteur P_triplet Affiche Extrait Obligatoire Sauve)
       CHANGEMENT(changement_normalisation)
       CHAMP_VIRTUEL(L_affiche_gl(affiche_vecteur)
		     AFFICHAGE(Different_si_actif))
       MENU("Valeurs/Normalise vecteur")
       EVENEMENT("0n0v")
       )

/*
 *
 */

typedef struct
{
  Triplet p, q, somme ;
}  Somme_Triplet ;

static void changement_somme_triplet(Somme_Triplet *o)
{
  o->somme = u_ajoute_triplet(&o->p, &o->q) ;
}

CLASSE(somme_triplet, Somme_Triplet,
       BULLE_D_AIDE
       ("Somme de deux triplets.\n"
	)
       CHAMP(somme, L_triplet P_triplet Affiche Init3d)
       CHAMP(p, L_triplet P_triplet Extrait Obligatoire Sauve Edite)
       CHAMP(q, L_triplet P_triplet Extrait Obligatoire Sauve Edite)
       CHANGEMENT(changement_somme_triplet)
       MENU("Valeurs/Somme")
       EVENEMENT("0s03")
       )


