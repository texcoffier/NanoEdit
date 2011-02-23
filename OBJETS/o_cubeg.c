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
 * sortie POV / STL ===> héritage
 */


#include <math.h>
#include <GL/gl.h>		/* Car il affiche du GL */
#include "o_objet.h"		/* Car il faut definir un objet */
#include "u_graphique.h"
#include "u_etat_courant.h"
#include "t_geometrie.h"
#include "u_table.h"
#include "u_vue.h"


#define MILIEU_FACE   1000
#define ROTATION      2000
#define HAUTEUR       3000
#define VISIBLE       4000
#define MILIEU_FACE2  5000
#define ANGLE_PLAN    6000

#define type_affichage globale_etat_courant.type_affichage

static int globale_option[] = { T_PONCTUEL, T_FILAIRE, T_FACETTE } ;

/*
 *****************************************************************************
 * Ordre des plans : X, Y, Z, X+1, Y+1, Z+1
 *****************************************************************************
 */

static int facettes[6][4] =
  {
    {4,6,2,0},{0,1,5,4},{0,2,3,1},{1,3,7,5},{6,7,3,2},{4,5,7,6}
  } ;

static int opposee[6] = { 3,4,5 ,0,1,2 } ;


/*
 * UTILITAIRES
 */

static void calcul_sommets(const Quadruplet p[6], Triplet pt[8])
{
  int i ;

  for(i=0; i<8; i++)
    {
      pt[i] = u_intersection_3_plans(&p[3*!!(i&1)]
				     , &p[3*!!(i&2)+1]
				     , &p[3*!!(i&4)+2]
				     ) ;
    }
}

static Triplet milieu_face(const Triplet *pt, int i)
{
  int j ;
  Triplet milieu ;

  u_annule_triplet(&milieu) ;
  for(j=0;j<4;j++)
    milieu = u_ajoute_triplet(&pt[facettes[i][j]], &milieu) ;

  return(u_triplet_fois_flottant(&milieu,1/4.)) ;
}

static Triplet centre(const Triplet *pt)
{
  int j ;
  Triplet milieu ;

  u_annule_triplet(&milieu) ;
  for(j=0;j<8;j++)
    milieu = u_ajoute_triplet(&pt[j], &milieu) ;

  return(u_triplet_fois_flottant(&milieu,1/8.)) ;
}

static Booleen point_invalide(const Triplet pt[8])
{
  int i ;

  for(i=0; i<8; i++)
    if ( u_triplet_invalide(&pt[i]) )
	return(Vrai) ;
  return(Faux) ;
}


static void affiche_numero_face(Quadruplet p[6])
{
  Triplet pt[8], milieu ;
  int i ;

  calcul_sommets(p, pt) ;
  if ( point_invalide(pt) )
    return ;
  glDisable(GL_DEPTH_TEST) ;
  glPushAttrib( GL_CURRENT_BIT ) ;
  glColor3f(1,1,1) ;
  for(i=0; i<6; i++)
    {
      milieu = milieu_face(pt, i) ;
      t_tag_origine(MILIEU_FACE+i, &milieu) ;
      glRasterPos3f(milieu.x, milieu.y, milieu.z) ;
      t_chaine( int_vers_char(i) ) ;
    }
  glPopAttrib() ;
  glEnable(GL_DEPTH_TEST) ;
}

static Booleen point_dans_face(int p, int f)
{
  return(
	 facettes[f][0] == p
	 || facettes[f][1] == p
	 || facettes[f][2] == p
	 || facettes[f][3] == p
	 ) ;
}
/*
static Booleen point_dans_cubeg(const Triplet *q, const Quadruplet p[6])
{
  int i ;

  for(i=0; i<6; i++)
    if ( u_distance_plan_triplet_signee(&p[i], q) > 0 )
      return(Faux) ;

  return(Vrai) ;
}
*/
static void intersection_2_plans(const Quadruplet pl[6], int i, int j
				 , Triplet *p, Triplet *q)
{
  int k ;

  k = 3 - ((i%3) + (j%3)) ;
  *p = u_intersection_3_plans(&pl[i], &pl[j], &pl[k]) ;
  *q = u_intersection_3_plans(&pl[i], &pl[j], &pl[k+3]) ;
}

/*
 *****************************************************************************
 *
 *****************************************************************************
 */

struct cubeg
{
  Quadruplet p[6] ;
  Triplet pt[8] ;
  Booleen invisible, valide ;
} ;
/*
static void cubeg_dump(const struct cubeg *cg)
{
  int i ;

  eprintf("CUBEG inVisible=%d Valide=%d\n", cg->invisible, cg->valide) ;
  for(i=0; i<6; i++)
    eprintf("plan[%d] = %s\n", i, quadruplet_vers_char(&cg->p[i])) ;
  for(i=0; i<8; i++)
    eprintf("point[%d] = %s\n", i, triplet_vers_char(&cg->pt[i])) ;
}
*/


static void cubeg_gl(const struct cubeg *cubeg)
{
  int i, j ;
  Booleen pointilles ;
  Type_affichage affiche ;
  Triplet c, d, e ;

  if ( !cubeg->valide )
    return ;

  if ( ACTIF(cubeg) && cubeg->invisible && type_affichage!=Invisible )
    {
      pointilles = Vrai ;
      affiche = Filaire ;
    }
  else
    {
      pointilles = Faux ;
      affiche = cubeg->invisible ? Invisible : type_affichage ;
    }

  if ( pointilles )
    {
      glPushAttrib(GL_LINE_BIT) ;
      glLineStipple(1,0xAAAA) ;
    }

  switch(affiche)
    {
    case Ponctuel:
      glBegin(GL_POINTS) ;
      for(i=0;i<8;i++)
	t_triplet(&cubeg->pt[i]) ;
      glEnd() ;
      break ;
    case Filaire:
      glBegin(GL_LINES) ;
      for(i=1; i<8; i*=2)
	for(j=0; j<8; j++)
	  if ( j < (j^i) )
	    {
	      t_triplet(&cubeg->pt[j]) ;
	      t_triplet(&cubeg->pt[j^i]) ;
	    }
      glEnd() ;
      break ;
    case Facette:
      glEnable(GL_LIGHTING) ;
      glBegin(GL_QUADS) ;
      for(i=0; i<6; i++)
	{
	  glNormal3f(cubeg->p[i].x, cubeg->p[i].y, cubeg->p[i].z) ;
	  for(j=0; j<4; j++)
	    t_triplet(&cubeg->pt[facettes[i][j]]) ;
	}
      glEnd() ;
      glDisable(GL_LIGHTING) ;
      break ;
    case Invisible:
      break ;
    case AfficheDebug:
      break ;
    }

  if ( pointilles )
    glPopAttrib() ;

  if ( ACTIF(cubeg) && type_affichage != Invisible )
    {
      for(i=0; i<6; i++)
	{
	  d = milieu_face(cubeg->pt, i) ;
	  e = u_ajoute_triplet(&d, (Triplet*)&cubeg->p[i]) ;
	  glBegin(GL_LINES) ;
	  t_triplet(&d) ;
	  t_triplet(&e) ;
	  glEnd() ;
	  t_fleche_proportionnelle(&d, &e, 0, .2) ;
	}

      /* Tag visible au centre */
      c = centre(cubeg->pt) ;

      t_tag_origine(VISIBLE, &c) ;
      glRasterPos3f(c.x, c.y, c.z) ;

      glDisable(GL_DEPTH_TEST) ;
      t_chaine( cubeg->invisible ? "I" : "V" ) ;
      glEnable(GL_DEPTH_TEST) ;
    }
}


static Grille_triplet* grille_de_cubeg(struct cubeg *o)
{
  static Grille_triplet g = { 0 } ;

  if ( g.nb_lignes == 0 )
    {
      struct informations_table it ;

      it = codec_informations_table(&infos_codec_grille_triplet, &g) ;
      codec_change_taille_table(&it, 2, 4) ;
    }

  g.grille[0][0] = o->pt[0] ;
  g.grille[0][1] = o->pt[1] ;
  g.grille[0][2] = o->pt[3] ;
  g.grille[0][3] = o->pt[2] ;
  g.grille[1][0] = o->pt[4] ;
  g.grille[1][1] = o->pt[5] ;
  g.grille[1][2] = o->pt[7] ;
  g.grille[1][3] = o->pt[6] ;

  return(&g) ;
}




static void ecrit_pov_cubeg(struct cubeg *o, const Champ *c, const char *v)
{
  static Grille_triplet *g ;

  if ( o->invisible )
    return ;

  g = grille_de_cubeg(o) ;
  t_grille_pov_general(g, &infos_codec_grille_triplet
		       , globale_option[type_affichage]
		       | T_CULL_FACE
		       | T_FACETTE_HAUT
		       | T_FACETTE_BAS
		       | T_RACCORDE_GD
		       , (FILE*)atol(v), NULL, NULL) ;
}

static void ecrit_stl_cubeg(struct cubeg *o, const Champ *c, const char *v)
{
  static Grille_triplet *g ;

  if ( o->invisible )
    return ;

  g = grille_de_cubeg(o) ;
  t_grille_stl_general(g, &infos_codec_grille_triplet
		       , globale_option[type_affichage]
		       | T_CULL_FACE
		       | T_FACETTE_HAUT
		       | T_FACETTE_BAS
		       | T_RACCORDE_GD
		       , (FILE*)atol(v)) ;
}


static void cubeg_valide(struct cubeg *cubeg)
{
  int i, j ;

  calcul_sommets(cubeg->p, cubeg->pt) ;

  if ( point_invalide(cubeg->pt) )
    {
      cubeg->valide = Faux ;
      return ;
    }
  for(i=0; i<6; i++)
    for(j=0; j<8; j++)
      if ( ! point_dans_face(j, i) )
	if ( u_distance_plan_triplet_signee(&cubeg->p[i], &cubeg->pt[j]) > 0 )
	  {
	    cubeg->valide = Faux ;
	    return ;
	  }

  cubeg->valide = Vrai ;
}

static void affiche_cubeg(struct cubeg *o)
{
  int i ;
  Triplet milieu ;

  if ( !o->valide )
    return ;

  cubeg_gl(o) ;

  if ( ACTIF(o) )
    {
      for(i=0;i<6;i++)
	{
	  milieu = milieu_face(o->pt, i) ;
	  t_tag_origine(MILIEU_FACE+i, &milieu) ;
	  t_tag_rotation2(ROTATION+i, &milieu) ;
	}
    }
}

void ecrit_tag_cubeg_general(struct cubeg *o, const char *v, int i)
{
  if ( i == VISIBLE )
    o->invisible = !o->invisible ;
}


void ecrit_tag_cubeg(struct cubeg *o, const Champ *c, const char *v)
{
  int i ;
  Triplet normale, pt, p[8] ;
  Quadruplet sauve ;

  i = lit_numero_tag(c) ;
  normale = char_vers_triplet(v) ;
  NOTE_UN_CHAMP_CHANGE(o);; // XXX 27/07/2010 Pas utile avant version 2.0

  if ( i>=MILIEU_FACE && i<MILIEU_FACE+6 )
    {
      i -= MILIEU_FACE ;
      sauve = o->p[i] ;
      o->p[i] = u_creation_plan_lie((Triplet*)&o->p[i], &normale) ;
      cubeg_valide(o) ;
      if ( ! o->valide )
	{
	  o->p[i] = sauve ;
	  cubeg_valide(o) ;
	}
    }
  else if ( i>=ROTATION && i<ROTATION+6 )
    {
      i -= ROTATION ;
      normale = u_polaires_vers_cartesiennes(normale.x, -normale.y) ;
      calcul_sommets(o->p, p) ;
      pt = milieu_face(p,i) ;
      sauve = o->p[i] ;
      o->p[i] = u_creation_plan_lie(&normale, &pt) ;
      cubeg_valide(o) ;
      if ( ! o->valide )
	{
	  o->p[i] = sauve ;
	  cubeg_valide(o) ;
	}
    }
  else ecrit_tag_cubeg_general(o, v, i) ;
}

const char* lit_tag_cubeg_general(struct cubeg *o, int i)
{
  if ( i == VISIBLE )
    {
      if ( o->invisible )
	return("L'objet est invisible") ;
      else
	return("L'objet est visible") ;
    }

  return("BUG") ;
}

const char* lit_tag_cubeg(struct cubeg *o, const Champ *c)
{
  int i ;
  Triplet centre ;
  static char buf[99] ;
  Flottant alpha, beta ;

  i = lit_numero_tag(c) ;

  if ( !o->valide )
    return("cubeg INVALIDE car point à l'infini") ;

  if ( i>=MILIEU_FACE && i<MILIEU_FACE+6 )
    {
      i -= MILIEU_FACE ;
      centre = milieu_face(o->pt, i) ;
      return( triplet_vers_char(&centre) ) ;
    }
  else if ( i>=ROTATION && i<ROTATION+6 )
    {
      i -= ROTATION ;
      u_cartesiennes_vers_polaires((Triplet *)&o->p[i], &alpha, &beta) ;
      sprintf(buf, "%g %g", alpha, beta) ;
      return(buf) ;
    }
  else return( lit_tag_cubeg_general(o, i) ) ;
}


static void changement_cubeg(struct cubeg *o)
{
  int i ;

  cubeg_valide(o) ;

  if ( ! o->valide )
    for(i=0; i<8; i++)
      if ( u_triplet_invalide(&o->pt[i]) )
	{
	  NOTE_CHAMP_INVALIDE(o, p[3*!!(i&1)]) ;
	  NOTE_CHAMP_INVALIDE(o, p[3*!!(i&2)+1]) ;
	  NOTE_CHAMP_INVALIDE(o, p[3*!!(i&4)+2]) ;
	}
}

CLASSE(cubeg, struct cubeg,
       CHAMP(p, L_cubeg P_6_quadruplets)
       Extrait Sauve Edite L_plan P_quadruplet
       CHAMP(p[0], DEFAUT("-1 0 0 0"))
       CHAMP(p[1], DEFAUT("0 -1 0 0"))
       CHAMP(p[2], DEFAUT("0 0 -1 0"))
       CHAMP(p[3], DEFAUT("1 0 0 -1"))
       CHAMP(p[4], DEFAUT("0 1 0 -1"))
       CHAMP(p[5], DEFAUT("0 0 1 -1"))
       CHAMP(invisible, L_booleen Edite Sauve DEFAUT("0"))
       CHAMP_VIRTUEL(L_tag(ecrit_tag_cubeg,lit_tag_cubeg))
       EVENEMENT("ShftCtrl+CGNC")
       MENU("Figures/3d/Cube Général")
       CHAMP_VIRTUEL(L_affiche_gl(affiche_cubeg)
		     AFFICHAGE(Different_suivant_type_affichage))
       CHAMP_VIRTUEL(L_affiche_pov(ecrit_pov_cubeg))
       CHAMP_VIRTUEL(L_affiche_stl(ecrit_stl_cubeg))
       CHANGEMENT(changement_cubeg)
       )

/*
 *****************************************************************************
 *
 *****************************************************************************
 */

struct cubeg_contraint
{
  struct cubeg cubeg ;
  t_angle angles[4] ;


  Quadruplet contrainte[6] ; /* mon fils */
  int face ; /* numero face du fils */
  int face_finale ;
  Flottant hauteur ;
} ;


static void affiche_cubeg_contraint(struct cubeg_contraint *o)
{
  Triplet milieu ;
  int i, j ;

  cubeg_gl(&o->cubeg) ;

  if ( ACTIF(o) )
    {
      affiche_numero_face(o->contrainte) ;

      if ( o->cubeg.valide )
	{
	  /*
	   * Réglage hauteur
	   */
	  milieu = milieu_face(o->cubeg.pt, o->face_finale) ;
	  t_tag_origine(HAUTEUR, &milieu) ;      
	  
	  glDisable(GL_DEPTH_TEST) ;
	  glPushAttrib( GL_CURRENT_BIT ) ;
	  glColor3f(1,1,1) ;
	  glRasterPos3f(milieu.x, milieu.y, milieu.z) ;
	  t_chaine( flottant_vers_char(o->hauteur) ) ;
	  glPopAttrib() ;
	  glEnable(GL_DEPTH_TEST) ;
	  /*
	   *
	   */
	  j = 0 ;
	  for(i=0; i<6; i++)
	    if ( i != o->face  &&  i != opposee[o->face] )
	      {
		milieu = milieu_face(o->cubeg.pt, i) ;
		t_tag_rotation(ANGLE_PLAN+j, &milieu) ;
		j++ ;
	      }
	}
    }
}


static void changement_cubeg_contraint(struct cubeg_contraint *o)
{
  Triplet p, q,  v ;
  Quadruplet plan ;
  int i, j ;
  
  for(i=0; i<6; i++)
    o->cubeg.p[i] = o->contrainte[i] ;

  /*
   * Face qui ne touche pas
   */
  calcul_sommets(o->cubeg.p, o->cubeg.pt) ;
  v = u_normalise_triplet((Triplet*)&o->cubeg.p[o->face]) ;
  v = u_triplet_fois_flottant(&v, o->hauteur) ;

  q = milieu_face(o->cubeg.pt, o->face) ;
  p = u_ajoute_triplet(&q, &v) ;

  o->cubeg.p[o->face] = u_creation_plan_lie((Triplet*)&o->cubeg.p[o->face], &p) ;

  /*
   * Face en contact
   */
  o->cubeg.p[opposee[o->face]] = u_plan_inverse(&o->contrainte[o->face]) ;

  /*
   * Remet dans le bon sens
   */
  if ( o->hauteur < 0 )
    {
      plan = u_plan_inverse(&o->cubeg.p[opposee[o->face]]) ;
      o->cubeg.p[opposee[o->face]] = u_plan_inverse(&o->cubeg.p[o->face]) ;
      o->cubeg.p[o->face] = plan ;
      o->face_finale = opposee[o->face] ;
    }
  else
    o->face_finale = o->face ;
  /*
   * offsets
   */
  /*
  j = 0 ;
  for(i=0; i<6; i++)
    if ( i != o->face  &&  i != opposee[o->face] )
      {
	o->cubeg[i] = u_plan_offset(&o->cubeg[i], 
      }
  */
  /*
   * Rotation des plans
   */
  j = 0 ;
  calcul_sommets(o->cubeg.p, o->cubeg.pt) ;
  for(i=0; i<6; i++)
    if ( i != o->face  &&  i != opposee[o->face] )
      {
	if ( o->angles[j].angle )
	  {
	    intersection_2_plans(o->cubeg.p
				 , o->hauteur>0 ? opposee[o->face] : o->face
				 , i, &p, &q) ;
	    v = u_soustrait_triplet(&p, &q) ;
	    q = u_rotation_triplet((Triplet*)&o->cubeg.p[i]
				   , &v, o->angles[j].angle) ;
	    o->cubeg.p[i] = u_creation_plan_lie(&q, &p) ;
	  }

	j++ ;
      }
  /*
   *
   */
  cubeg_valide(&o->cubeg) ;
  if ( !o->cubeg.valide )
    NOTE_CHAMP_INVALIDE(o, cubeg.p) ;

}


void ecrit_tag_cubeg_contraint(struct cubeg_contraint *o, const Champ *c, const char *v)
{
  int i ;
  Triplet pt ;

  i = lit_numero_tag(c) ;

  if ( i>=MILIEU_FACE && i<MILIEU_FACE+6 )
    {
      i -= MILIEU_FACE ;
      o->face = i ;
    }
  else if ( i==HAUTEUR )
    {
      pt = char_vers_triplet(v) ;
      o->hauteur = u_distance_plan_triplet_signee(&o->contrainte[o->face]
						  , &pt) ;
    }
  else if ( i>=ANGLE_PLAN && i<ANGLE_PLAN+4 )
    {
      o->angles[i-ANGLE_PLAN].angle = atof(v) ;
    }
  else ecrit_tag_cubeg_general(&o->cubeg, v, i) ;
}

const char* lit_tag_cubeg_contraint(struct cubeg_contraint *o, const Champ *c)
{
  int i ;
  Triplet pt[8], milieu ;

  i = lit_numero_tag(c) ;

  if ( i>=MILIEU_FACE && i<MILIEU_FACE+6 )
    {
      i -= MILIEU_FACE ;
      return( flottant_vers_char(i) ) ;
    }
  else if ( i==HAUTEUR )
    {
      calcul_sommets(o->cubeg.p, pt) ;
      milieu = milieu_face(pt, o->face) ;
      return( triplet_vers_char(&milieu) ) ;
    }
  else if ( i>=ANGLE_PLAN && i<ANGLE_PLAN+4 )
    {
      return( flottant_vers_char(o->angles[i-ANGLE_PLAN].angle) ) ;
    }
  else return( lit_tag_cubeg_general(&o->cubeg, i) ) ;
}



CLASSE(cubeg_contraint, struct cubeg_contraint,
       CHAMP(cubeg.p, L_cubeg P_6_quadruplets Affiche)
       CHAMP(contrainte, L_cubeg P_6_quadruplets Extrait Obligatoire Edite Sauve)
       CHAMP(face, L_nombre P_entier DEFAUT("0") Edite Sauve)
       CHAMP(hauteur, L_nombre P_flottant DEFAUT("1") Edite Sauve Init3d)
       CHAMP(angles[0], L_nombre P_flottant E_angle DEFAUT("0") Edite Sauve Init3d)
       CHAMP(angles[1], L_nombre P_flottant E_angle DEFAUT("0") Edite Sauve Init3d)
       CHAMP(angles[2], L_nombre P_flottant E_angle DEFAUT("0") Edite Sauve Init3d)
       CHAMP(angles[3], L_nombre P_flottant E_angle DEFAUT("0") Edite Sauve Init3d)
       /*
       CHAMP(offsets_depart, L_quadruplet P_quadruplet Edite Sauve DEFAUT("0 0 0 0"))
       CHAMP(offsets_arrivee, L_quadruplet P_quadruplet Edite Sauve DEFAUT("0 0 0 0"))
       */
       CHAMP(cubeg.invisible, NOM("invisible") L_booleen Edite Sauve DEFAUT("0"))
       CHAMP_VIRTUEL(L_tag(ecrit_tag_cubeg_contraint,lit_tag_cubeg_contraint))
       CHANGEMENT(changement_cubeg_contraint)
       EVENEMENT("ShftCtrl+CGC")
       MENU("Figures/3d/Cube Général contraint")
       CHAMP_VIRTUEL(L_affiche_gl(affiche_cubeg_contraint)
		     AFFICHAGE(Different_suivant_type_affichage))
       CHAMP_VIRTUEL(L_affiche_pov(ecrit_pov_cubeg))
       CHAMP_VIRTUEL(L_affiche_stl(ecrit_stl_cubeg))
       )


/*
 *
 */


struct cubeg_remplisseur
{
  struct cubeg cubeg ;
  Quadruplet depart[6] ;
  Quadruplet arrivee[6] ;
  int face_depart, face_arrivee ;
} ;

static void affiche_cubeg_remplisseur(struct cubeg_remplisseur *o)
{
  if ( ACTIF(o) )
    {
      affiche_numero_face(o->depart) ;
      affiche_numero_face(o->arrivee) ;
    }
  cubeg_gl(&o->cubeg) ;
}


static void changement_cubeg_remplisseur(struct cubeg_remplisseur *o)
{
  int i ;
  
  for(i=0; i<6; i++)
    o->cubeg.p[i] = o->depart[i] ;

  /*
   * Face en contact avec le cube départ
   */
  o->cubeg.p[opposee[o->face_depart]] = u_plan_inverse(&o->cubeg.p[o->face_depart]) ;

  /*
   * Face opposée.
   */
  o->cubeg.p[o->face_depart] = o->arrivee[o->face_arrivee] ;

  cubeg_valide(&o->cubeg) ;
  if ( ! o->cubeg.valide )
    {
      o->cubeg.p[o->face_depart] = u_plan_inverse(&o->arrivee[o->face_arrivee]) ;
      cubeg_valide(&o->cubeg) ;
      if ( ! o->cubeg.valide )
	{
	  NOTE_CHAMP_INVALIDE(o, face_arrivee) ;
	}
    }
}


void ecrit_tag_cubeg_remplisseur(struct cubeg_remplisseur *o, const Champ *c, const char *v)
{
  int i ;

  i = lit_numero_tag(c) ;

  if ( i>=MILIEU_FACE && i<MILIEU_FACE+6 )
    {
      i -= MILIEU_FACE ;
      o->face_depart = i ;
    }
  if ( i>=MILIEU_FACE2 && i<MILIEU_FACE2+6 )
    {
      i -= MILIEU_FACE2 ;
      o->face_arrivee = i ;
    }
  else ecrit_tag_cubeg_general(&o->cubeg, v, i) ;
}

const char* lit_tag_cubeg_remplisseur(struct cubeg_remplisseur *o, const Champ *c)
{
  int i ;

  i = lit_numero_tag(c) ;

  if ( i>=MILIEU_FACE && i<MILIEU_FACE+6 )
    {
      i -= MILIEU_FACE ;
      return( flottant_vers_char(i) ) ;
    }
  if ( i>=MILIEU_FACE2 && i<MILIEU_FACE2+6 )
    {
      i -= MILIEU_FACE2 ;
      return( flottant_vers_char(i) ) ;
    }
  else return( lit_tag_cubeg_general(&o->cubeg, i) ) ;
}



CLASSE(cubeg_remplisseur, struct cubeg_remplisseur,
       CHAMP(cubeg.p, L_cubeg P_6_quadruplets Affiche)
       CHAMP(depart, L_cubeg P_6_quadruplets Extrait Obligatoire Edite Sauve)
       CHAMP(face_depart, L_nombre P_entier DEFAUT("0") Edite Sauve)
       CHAMP(arrivee, L_cubeg P_6_quadruplets Extrait Obligatoire Edite Sauve)
       CHAMP(face_arrivee, L_nombre P_entier DEFAUT("0") Edite Sauve)
       CHAMP(cubeg.invisible, NOM("invisible") L_booleen Edite Sauve DEFAUT("1"))
       CHAMP_VIRTUEL(L_tag(ecrit_tag_cubeg_remplisseur,lit_tag_cubeg_remplisseur))
       CHANGEMENT(changement_cubeg_remplisseur)
       EVENEMENT("ShftCtrl+CGR")
       MENU("Figures/3d/Cube General Remplisseur")
       CHAMP_VIRTUEL(L_affiche_gl(affiche_cubeg_remplisseur)
		     AFFICHAGE(Different_suivant_type_affichage))
       CHAMP_VIRTUEL(L_affiche_pov(ecrit_pov_cubeg))
       CHAMP_VIRTUEL(L_affiche_stl(ecrit_stl_cubeg))
       )


