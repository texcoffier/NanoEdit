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
#include "o_nuplet.h"		/* Pour information_triplet() */
#include "u_etat_courant.h"
#include "stl.h"

static const Flottant distance_bout = 0.15 ;

typedef struct
{
  Triplet depart ;
  Triplet arrivee ;
  Triplet vecteur ;
  Triplet milieu ;
  Booleen affiche_tag_milieu ;
  Booleen affiche_tag_extenseur ;
} Segment ;

static Triplet position_tag(Segment *s, int i)
{
  switch(i)
    {
    case 0 :
      return( u_ponderation_2_triplet(&s->arrivee, distance_bout,
				      &s->depart , 1-distance_bout) ) ;
    case 1 :
      return( u_ponderation_2_triplet(&s->arrivee, 1-distance_bout,
				      &s->depart , distance_bout) ) ;
    default:
      ICI ;
      return( char_vers_triplet("") ) ;
    }
}

static void affiche_segment(Segment *s)
{
  Triplet pt ;

  glBegin(GL_LINES) ;
  glVertex3f(s->depart.x , s->depart.y , s->depart.z) ;
  glVertex3f(s->arrivee.x, s->arrivee.y, s->arrivee.z) ;
  glEnd();

  if ( ACTIF(s) )
    {
      t_tag_origine(0, &s->depart) ;
      t_tag_origine(1, &s->arrivee) ;
      /*
       * Pas très propre
       */
      if ( strcmp(OBJET(s)->classe->nom, "segment_pt_pt") == 0 )
	{
	  if ( s->affiche_tag_milieu )
	    t_tag(2, &s->milieu) ;

	  if ( s->affiche_tag_extenseur )
	    {
	      pt = position_tag(s, 0) ;
	      t_tag_dilatation(5, &pt) ;
	    }
	}
      
      if ( s->affiche_tag_extenseur )
	{
	  pt = position_tag(s, 1) ;
	  t_tag_dilatation(6, &pt) ;
	}
    }
}


static void u_changement_milieu(Segment *s)
{
  Triplet milieu, vecteur ;
  /*
   * Recalcule ancienne valeur du milieu
   */
  milieu = u_milieu_2_triplet(&s->arrivee, &s->depart) ;
  /*
   * Différence des deux milieux donne la translation
   */
  vecteur = u_soustrait_triplet(&s->milieu, &milieu) ;
  /*
   * On Translate les deux points
   */
  s->depart = u_ajoute_triplet(&s->depart, &vecteur) ;
  s->arrivee = u_ajoute_triplet(&s->arrivee, &vecteur) ;
}

static void changement_segment_pt_pt(Segment *s)
{
  if ( CHAMP_CHANGE(s, depart) || CHAMP_CHANGE(s,arrivee) )
    {
      s->vecteur = u_soustrait_triplet(&s->arrivee, &s->depart) ;
      s->milieu = u_milieu_2_triplet(&s->arrivee, &s->depart) ;
      /* NOTE_CHAMP_CHANGE(s, vecteur) ; */
    }
  else
    if ( CHAMP_CHANGE(s, milieu) )
      u_changement_milieu(s) ;
}

static void changement_segment_pt_vec(Segment *s)
{
  if ( CHAMP_CHANGE(s, depart) || CHAMP_CHANGE(s,vecteur) )
    {
      s->arrivee = u_ajoute_triplet(&s->depart, &s->vecteur) ;
      s->milieu = u_milieu_2_triplet(&s->arrivee, &s->depart) ;
    }
}

static const char *informations_segment(const Segment *s)
{
  static Buffer info ;	/* STATICOK : car retournée */
  sprintf(info, "vers %.3g,%.3g,%.3g",
	  s->arrivee.x, s->arrivee.y, s->arrivee.z) ;
  return(info) ;
}

static void ecrit_tag(Segment *s,const Champ *c, const char *v)
{
  Triplet pt ;
  Flottant a ;

  pt = char_vers_triplet(v) ;
  a = u_projete_triplet(&pt, &s->depart, &s->vecteur) ;

  switch( c->defaut[0] )
    {
    case '1' :
      s->vecteur = u_triplet_fois_flottant(&s->vecteur, a/(1-distance_bout)) ;
      s->arrivee = u_ajoute_triplet(&s->depart, &s->vecteur) ;
      NOTE_CHAMP_CHANGE(s, arrivee) ;
      break ;
    case '0' :
      s->vecteur = u_triplet_fois_flottant(&s->vecteur,
					   (1-a)/(1-distance_bout)) ;
      s->depart = u_soustrait_triplet(&s->arrivee, &s->vecteur) ;
      NOTE_CHAMP_CHANGE(s, depart) ;
      break ;
    default : ICI ;
    }
  /*  NOTE_CHAMP_CHANGE(s, vecteur) ; 06/05/1999 */
  s->milieu = u_milieu_2_triplet(&s->arrivee, &s->depart) ;
}

static const char* lit_tag(Segment *s,const Champ *c, const char *v)
{
  Triplet pt ;
  
  pt = position_tag(s, c->defaut[0]-'0') ;
  return( triplet_vers_char(&pt) ) ;
}

static const char* lit_bipoint(Segment *s,const Champ *c, const char *v)
{
  static char infos[100] ;
  sprintf(infos, "%g %g %g\n%g %g %g"
	  ,s->depart.x, s->depart.y, s->depart.z 
	  ,s->arrivee.x, s->arrivee.y, s->arrivee.z 
	  ) ;
  return(infos) ;
}

static void ecrit_bipoint(Segment *s,const Champ *c, const char *v)
{
  sscanf(v, FF3 FF3
	  ,&s->depart.x, &s->depart.y, &s->depart.z 
	  ,&s->arrivee.x, &s->arrivee.y, &s->arrivee.z 
	  ) ;
  NOTE_CHAMP_CHANGE(s, depart) ;
  if ( strcmp( OBJET(s)->classe->nom, "segment_pt_vec") == 0 )
    {
      s->vecteur = u_soustrait_triplet(&s->arrivee, &s->depart) ;
      NOTE_CHAMP_CHANGE(s, vecteur) ;
    }
  else
    NOTE_CHAMP_CHANGE(s, arrivee) ;
}

static const char* lit_vecteur_lie(Segment *s,const Champ *c, const char *v)
{
  static char infos[100] ;
  sprintf(infos, "%g %g %g\n%g %g %g"
	  ,s->depart.x, s->depart.y, s->depart.z 
	  ,s->vecteur.x, s->vecteur.y, s->vecteur.z 
	  ) ;
  return(infos) ;
}

static void ecrit_vecteur_lie(Segment *s,const Champ *c, const char *v)
{
  sscanf(v, FF3 FF3
	  ,&s->depart.x, &s->depart.y, &s->depart.z 
	  ,&s->vecteur.x, &s->vecteur.y, &s->vecteur.z 
	  ) ;
  NOTE_CHAMP_CHANGE(s, depart) ;
  if ( strcmp( OBJET(s)->classe->nom, "segment_pt_pt") == 0 )
    {
      s->arrivee = u_ajoute_triplet(&s->depart, &s->vecteur) ;
      NOTE_CHAMP_CHANGE(s, arrivee) ;
    }
  else
    NOTE_CHAMP_CHANGE(s, vecteur) ;
}

static void affiche_segment_stl(Segment *s, const Champ *c, const char *v)
{
  if ( globale_etat_courant.stl_offset )
    t_cylindre_stl((FILE*)atol(v)
		   , &s->depart, &s->arrivee
		   , globale_etat_courant.stl_offset/2
		   , 8
		   ) ;
}

/*
 * L_point3 est un raccourci pour P_triplet L_point
 */
CLASSE(segment_pt_pt, Segment,
       EVENEMENT("1s1e")
       MENU("Figures/1d/Segment")
       Init3d
       CHAMP(depart , L_point P_triplet Edite Sauve Extrait DEFAUT("0 0 0"))
       CHAMP(arrivee, L_point P_triplet Edite Sauve Extrait DEFAUT("1 0 0"))
       CHAMP(milieu, L_point P_triplet Edite NonAffiche)
       CHAMP(vecteur, NonInit3d L_vecteur P_triplet Affiche)
       CHAMP(affiche_tag_extenseur, L_booleen Edite Sauve DEFAUT("1"))
       CHAMP(affiche_tag_milieu, L_booleen Edite Sauve DEFAUT("1"))
       CHAMP_VIRTUEL(L_fonction2(ecrit_tag, lit_tag) DEFAUT("0") )
       CHAMP_VIRTUEL(L_fonction2(ecrit_tag, lit_tag) DEFAUT("1") )
       CHAMP_VIRTUEL(L_affiche_gl(affiche_segment)
		     AFFICHAGE(Toujours_different))
       CHAMP_VIRTUEL(L_bipoint P_virtuel(ecrit_bipoint, lit_bipoint))
       CHAMP_VIRTUEL(L_vecteur_lie P_virtuel(ecrit_vecteur_lie, lit_vecteur_lie))
       CHAMP_VIRTUEL(L_affiche_stl(affiche_segment_stl))
       CHANGEMENT(changement_segment_pt_pt)
       INFORMATIONS(informations_segment)
       DECALAGE(1)
       )

CLASSE(segment_pt_vec, Segment,
       SUR_CLASSE("segment_pt_pt") HERITE_PAS_DES_CHAMPS
       Init3d
       CHAMP(depart , L_point P_triplet   Edite Sauve Extrait DEFAUT("0 0 0"))
       CHAMP(vecteur, L_vecteur P_triplet Edite Sauve Extrait DEFAUT("1 0 0"))
       CHAMP(milieu, L_point P_triplet Edite NonAffiche)
       CHAMP(arrivee, L_point P_triplet   Affiche )
       CHAMP(affiche_tag_extenseur, L_booleen Edite Sauve DEFAUT("1"))
       CHAMP(affiche_tag_milieu, L_booleen Edite Sauve DEFAUT("1"))
       CHAMP_VIRTUEL(L_fonction2(ecrit_tag, lit_tag) DEFAUT("1") )
       CHAMP_VIRTUEL(L_affiche_gl(affiche_segment)
		     AFFICHAGE(Toujours_different))
       CHAMP_VIRTUEL(L_bipoint P_virtuel(ecrit_bipoint, lit_bipoint))
       CHAMP_VIRTUEL(L_vecteur_lie P_virtuel(ecrit_vecteur_lie, lit_vecteur_lie))
       CHAMP_VIRTUEL(L_affiche_stl(affiche_segment_stl))
       CHANGEMENT(changement_segment_pt_vec)
       )



typedef struct
{
  Flottant longueur ;		/* Valeur de la distance */
  Bitriplet s ;
}  Longueur ;

static void changement_longueur_bipoint(Longueur *o)
{
  o->longueur = u_distance_triplet(&o->s.t1, &o->s.t2) ;
}

CLASSE(longueurs, Longueur,
       CHAMP(longueur, L_nombre_positif P_flottant Affiche Init3d)
       EVENEMENT("LO")
       MENU("NON")
       )

CLASSE(longueur_bipoint, Longueur,
       SUR_CLASSE("longueurs")
       BULLE_D_AIDE
       ("Longueur entre les 2 points d'un bipoint.\n"
	)
       CHAMP(s, L_bipoint P_bitriplet Extrait Sauve Edite)
       CHANGEMENT(changement_longueur_bipoint)
       MENU("Valeurs/Longueur")
       )

static void changement_longueur_vecteur_lie(Longueur *o)
{
  o->longueur = u_norme_triplet(&o->s.t2) ;
}

CLASSE(longueur_vecteur_lie, Longueur,
       SUR_CLASSE("longueurs")
       BULLE_D_AIDE
       ("Longueur d'un vecteur lié.\n"
	)
       CHAMP(s, L_vecteur_lie P_bitriplet Extrait Sauve Edite)
       CHANGEMENT(changement_longueur_vecteur_lie)
       MENU("Valeurs/Longueur")
       )



typedef struct
{
  Table_flottant longueurs ;
  Table_triplet points ;
}  Longueurs_table ;

static void changement_longueurs_table(Longueurs_table *o)
{
  int i ;

  if ( !UN_CHAMP_CHANGE(o) )
    return ;
  if ( o->points.nb <= 0 )
    return ;

  CHANGE_TAILLE(o, longueurs, o->points.nb - 1) ;

  for(i=1; i<o->points.nb; i++)
    o->longueurs.table[i-1] = u_distance_triplet(&o->points.table[i-1],
						 &o->points.table[i]
						 ) ;

}

CLASSE(longueurs_table, Longueurs_table,
       BULLE_D_AIDE
       ("Longueur entre les 2 points d'une table.\n"
	"Cela donne une table de longueurs.\n"
	)
       CHAMP(points, L_table_point P_table_triplet Extrait Obligatoire)
       CHAMP(longueurs, L_table_flottant Affiche)
       CHANGEMENT(changement_longueurs_table)
       MENU("Valeurs/Longueur")
       EVENEMENT("LT")
       )

