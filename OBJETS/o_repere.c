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
#include "u_etat_courant.h"
#include "u_vue.h"

static const Flottant dist_tag_dilatation = 0.85 ;
static const Flottant dist_tag_rotation = 0.70 ;
static const Flottant dist_tag_rotation2 = 0.2 ;
static const Flottant dist_tag_echange = 0.4 ;
static const Flottant dist_tag_symetrie = 0.1 ;

#define TAG_MASQUE              0x00FF
#define TAG_DILATATION          0x0100
#define TAG_ROTATION            0x0200
#define TAG_DEPLACEMENT         0x0400
#define TAG_INVERSION           0x0800
#define TAG_SYMETRIE            0x1000
#define TAG_DEPLACEMENT_NEGATIF 0x2000
#define TAG_ECHANGE             0x4000

#include "u_repere.h"

static void affiche_repere(Repere *o)
{
  Triplet pt, pt2, v ;
  int tag_deplacement, tag ;
  int i, j, options ;
  Triplet colors[3] = { {1,0,0}, {0,1,0}, {0,0,1} } ;

  if ( o->options & OPTION_AFFICHE_TAG_POINT )
    tag_deplacement = 1 ;
  else
    tag_deplacement = TAG_DEPLACEMENT ;
  tag = 0 ;

  glPushAttrib( GL_CURRENT_BIT ) ;

  if ( ACTIF(o) )
    {
      if ( o->options & OPTION_AFFICHE_TAG_ORIGINE )
	t_tag_origine(tag++, &o->t.origine) ;

      if ( o->options & OPTION_AFFICHE_TAG_VALEUR )
	{
	  pt = u_ajoute_triplet(&o->px, &o->t.v2) ;
	  pt = u_ajoute_triplet(&pt, &o->t.v3) ;
	  t_tag_origine(tag++, &pt) ;
	}
      if ( o->options & OPTION_AFFICHE_TAG_VECTEUR )
	{
	  glBegin(GL_LINES) ;
          glVertex3f(0.,0.,0.) ;
          glVertex3f(o->vecteur.x , o->vecteur.y , o->vecteur.z) ;
	  glEnd() ;
	  t_tag_origine(tag++, &o->vecteur) ;
	}
    }
  if ( o->options & OPTION_AFFICHE_SEGMENT ) /* Pour axe */
    {
      glBegin(GL_LINES) ;
      t_triplet(&o->t.origine) ;
      t_triplet(&o->px) ;
      glEnd() ;      
      if ( o->options & OPTION_AFFICHE_FLECHE_X )
	t_fleche_proportionnelle(&o->t.origine, &o->px, 0, 0.1) ;
    }

  options = o->options ;
  for(i=0; i<3; i++, options >>= 1)
    {
      if ( options & OPTION_AFFICHE_X )
	{
	  t_couleur(&colors[i]) ;
	  if ( options & OPTION_AFFICHE_FLECHE_X )
	    t_fleche_proportionnelle(&o->t.origine, &o->px + i, 0, 0.1) ;
	  glBegin(GL_LINES) ;
	  t_triplet(&o->t.origine) ;
	  t_triplet(&o->px + i) ;
	  glEnd() ;
	}
      if ( options & OPTION_AFFICHE_X_NEGATIF )
	{
	  Triplet n ;
	  t_couleur(&colors[i]) ;
	  n = u_soustrait_triplet(&o->t.origine, &o->t.v1 + i) ;
	  glBegin(GL_LINES) ;
	  t_triplet(&o->t.origine) ;
	  t_triplet(&n) ;
	  glEnd() ;
	}
      if ( ACTIF(o) )
	{
	  if ( options & OPTION_AFFICHE_TAG_POINT_X )
	    t_tag_origine(tag_deplacement + i, &o->px + i) ;

	  if ( (options & OPTION_AFFICHE_TAG_POINT_X)
	       && (options & OPTION_AFFICHE_X_NEGATIF )
	       )
	    {
	      Triplet n ;
	      n = u_soustrait_triplet(&o->t.origine, &o->t.v1 + i) ;
	      t_tag_origine(TAG_DEPLACEMENT_NEGATIF + i, &n) ;
	    }
	  
	  if ( options & OPTION_AFFICHE_TAG_DILATATION_X )
	    {
	      pt = u_barycentre_2_triplet(&o->t.origine
					  , dist_tag_dilatation
					  , &o->px + i);
	      t_tag_dilatation(TAG_DILATATION + i, &pt) ;
	    }
	  if ( options & OPTION_AFFICHE_TAG_ROTATION_X )
	    {
	      pt = u_barycentre_2_triplet(&o->t.origine
					  , dist_tag_rotation
					  , &o->px + i);
	      t_tag_rotation(TAG_ROTATION + i, &pt) ;
	    }
	  if ( ((o->options>>((i+1)%3)) & OPTION_AFFICHE_TAG_ROTATION2_X)
	     && ((o->options>>((i+2)%3)) & OPTION_AFFICHE_TAG_ROTATION2_X)
	       )
	    {
	      pt = u_milieu_2_triplet(&o->px + (i+1)%3, &o->px + (i+2)%3) ;
	      pt = u_barycentre_2_triplet(&o->t.origine
					  , dist_tag_rotation2
					  , &pt);
	      t_tag_rotation(TAG_ROTATION + i, &pt) ;
	    }
	  if ( ((o->options>>((i+1)%3)) & OPTION_AFFICHE_TAG_ECHANGE_X)
	     && ((o->options>>((i+2)%3)) & OPTION_AFFICHE_TAG_ECHANGE_X)
	       )
	    {
	      pt = u_milieu_2_triplet(&o->px + (i+1)%3, &o->px + (i+2)%3) ;
	      pt = u_barycentre_2_triplet(&o->t.origine
					  , dist_tag_echange
					  , &pt);
	      t_tag(TAG_ECHANGE + i, &pt) ;
	    }
	  if ( options & OPTION_AFFICHE_TAG_INVERSION_X )
	    {
	      pt = u_milieu_2_triplet(&o->t.origine, &o->px + i);
	      t_tag(TAG_INVERSION + i, &pt) ;
	    }

	  for(j=0; j<3; j++)
	    {
	      if ( j == i )
		continue ;
	      if ( (o->options & (OPTION_AFFICHE_X<<j))
		   || (j==0 && (o->options & OPTION_AFFICHE_SEGMENT)) )
		{
		  if ( options & OPTION_AFFICHE_TAG_SYMETRIE_X )
		    {
		      pt = u_milieu_2_triplet(&o->t.origine, &o->px + j);
		      v = u_triplet_fois_flottant(&o->t.v1 + i
						  , -dist_tag_symetrie
						  /u_norme_triplet(&o->t.v1+i)
						  *u_norme_triplet(&o->t.v1+j)
						  ) ;
		      pt2 = u_ajoute_triplet(&pt, &v) ;
		      t_tag_general(TAG_SYMETRIE + i, &pt2, &colors[i]) ;
		      pt2 = u_soustrait_triplet(&pt, &v) ;
		      t_tag_general(TAG_SYMETRIE + i, &pt2, &colors[i]) ;
		    }
		}
	    }
	}
    }
  glPopAttrib() ;
}




static void ecrit_pov_repere(Repere *o, const Champ *c, const char *v)
{
  fprintf( (FILE*)atol(v),
	   "matrix <%g, %g, %g,  %g, %g, %g,  %g, %g, %g,  %g, %g, %g>\n",
	   o->t.v1.x     , o->t.v1.y     , o->t.v1.z,
	   o->t.v2.x     , o->t.v2.y     , o->t.v2.z,
	   o->t.v3.x     , o->t.v3.y     , o->t.v3.z,
	   o->t.origine.x, o->t.origine.y, o->t.origine.z
	   ) ;
}

static void applique_repere(Repere *o, const Champ *ch, Booleen *modif)
{
  globale_etat_courant.transfo = u_produit_transfo(&globale_etat_courant.transfo, &o->t) ;
  *modif = Vrai ;
}

static void applique_gl_repere(Repere *o)
{
  u_change_repere(&o->t) ;
}

static const char * attributs_gl_repere(Repere *o, const Champ *ch)
{
  return( int_vers_char(NANOEDIT_MATRIX_BIT) ) ;
}

static void repere_valide(Repere *o)
{
  o->options =
    OPTION_AFFICHE_TAG_ORIGINE
    | OPTION_AFFICHE_XYZ
    | OPTION_AFFICHE_TAG_POINT_XYZ
    | OPTION_AFFICHE_TAG_POINT
    ;
  if ( o->fleche )
    o->options |= OPTION_AFFICHE_FLECHE_XYZ ;
  if ( !o->cache_tag )
    o->options |= OPTION_AFFICHE_TAG_DILATATION_XYZ
      | OPTION_AFFICHE_TAG_ROTATION_XYZ
      ;
  if ( !o->cache_tag_rotation2 )
    o->options |= OPTION_AFFICHE_TAG_ROTATION2_XYZ ;
  if ( !o->cache_tag_echange )
    o->options |= OPTION_AFFICHE_TAG_ECHANGE_XYZ ;
  if ( !o->cache_tag_symetrie )
    o->options |= OPTION_AFFICHE_TAG_INVERSION_XYZ
      | OPTION_AFFICHE_TAG_SYMETRIE_XYZ
      ;
  if ( determinant_transfo(&o->t) <= 1e-4 )
    NOTE_CHAMP_INVALIDE(o, t) ;
}

static void changement_repere(Repere*o)
{
  o->px = u_ajoute_triplet(&o->t.v1, &o->t.origine) ;
  o->py = u_ajoute_triplet(&o->t.v2, &o->t.origine) ;
  o->pz = u_ajoute_triplet(&o->t.v3, &o->t.origine) ;    
  repere_valide(o) ;
  o->options &= ~OPTION_AFFICHE_TAG_POINT ;
}

static void changement_repere2d(Repere*o)
{
  o->px = u_ajoute_triplet(&o->t.v1, &o->t.origine) ;
  o->py = u_ajoute_triplet(&o->t.v2, &o->t.origine) ;
  o->t.v3 = u_pv_triplet(&o->t.v1, &o->t.v2) ;
  o->pz = u_ajoute_triplet(&o->t.v3, &o->t.origine) ;    
  repere_valide(o) ;
  o->options &= ~(OPTION_AFFICHE_TOUT_Z | OPTION_AFFICHE_TAG_POINT) ;
}



#define CHAMP_REPERE_AFFICHAGE						\
       NON_OPTION(~0)							\
       CHAMP(t, L_transfo NonInitialiser)				\
       CHAMP_VIRTUEL(L_attribut_pov)					\
       CHAMP_VIRTUEL(L_affiche_gl(affiche_repere)			\
		     AFFICHAGE(Toujours_different))			\
       CHAMP_VIRTUEL(L_applique_gl_rapide(applique_gl_repere		\
					  ,attributs_gl_repere))	\
       CHAMP_VIRTUEL(L_applique(applique_repere))


#define CHAMP_REPERE_COMMUN						\
       CHAMP_REPERE_AFFICHAGE \
       CHAMP(fleche, L_booleen Edite Sauve DEFAUT("0") LABEL("Affiche les flèches")) \
       CHAMP(cache_tag, L_booleen Edite Sauve DEFAUT("0") LABEL("Ne pas afficher les Tag rotation et dilatation")) \
       CHAMP(cache_tag_symetrie, L_booleen Edite Sauve DEFAUT("0") LABEL("Ne pas afficher les Tag symetrie et inversion")) \
       CHAMP(cache_tag_rotation2, L_booleen Edite Sauve DEFAUT("1") LABEL("Ne pas afficher les Tag rotation en coin")) \
       CHAMP(cache_tag_echange, L_booleen Edite Sauve DEFAUT("1") LABEL("Ne pas afficher les Tag échange d'axe"))

static void changement_repere_pts(Repere*o)
{
  o->t.v1 = u_soustrait_triplet(&o->px, &o->t.origine) ;
  o->t.v2 = u_soustrait_triplet(&o->py, &o->t.origine) ;
  o->t.v3 = u_soustrait_triplet(&o->pz, &o->t.origine) ;    
  repere_valide(o) ;
}

static void changement_repere_pts2d(Repere*o)
{
  Triplet pt ;

  o->t.v1 = u_soustrait_triplet(&o->px, &o->t.origine) ;
  o->t.v2 = u_soustrait_triplet(&o->py, &o->t.origine) ;
  pt = u_pv_triplet(&o->t.v1, &o->t.v2) ;
  o->t.v3 = u_normalise_triplet(&pt) ;
  repere_valide(o) ;
  o->options &= ~OPTION_AFFICHE_TOUT_Z ;
}


void ecrit_tag_repere(Repere *o, const Champ *c, const char *v)
{
  int i , j;
  Triplet *axe, in ;
  Flottant angle, ac ;

  i = lit_numero_tag(c) ;
  axe = &(&o->t.v1)[i & TAG_MASQUE] ;
  in = char_vers_triplet(v) ;

  if ( i & TAG_DILATATION )
    {
      ac = u_projete_triplet(&in, &o->t.origine, axe) ;
      ac /= dist_tag_dilatation ;
      *axe = u_triplet_fois_flottant(axe, ac) ;
    }
  else if ( i & TAG_ROTATION )
    {
      angle = atof(v)*M_PI/2 ;
      for(j=0;j<3;j++)
	if ( i != j )
	  {
	    (&o->t.v1)[j] = u_rotation_triplet(&(&o->t.v1)[j], axe,
					       angle - o->angle.angle) ;
	  }
      o->angle.angle = angle ;      
    }
  else if ( i & TAG_ECHANGE )
    {
      i = i & TAG_MASQUE ;
      in = (&o->t.v1)[(i+1)%3] ;
      (&o->t.v1)[(i+1)%3] = (&o->t.v1)[(i+2)%3] ;
      (&o->t.v1)[(i+2)%3] = in ;
    }
  else if ( i & TAG_DEPLACEMENT )
    {
      if ( o->centre_deplacable & (1<<i) )
	{
	  *axe = u_soustrait_triplet(&o->t.origine, axe) ; // X negatif
	  *axe = u_soustrait_triplet(&in, axe) ;
	  *axe = u_triplet_fois_flottant(axe, 0.5) ;
	  o->t.origine = u_soustrait_triplet(&in, axe) ;
	}
      else
	{	  
	  *axe = u_soustrait_triplet(&in, &o->t.origine) ;
	}
    }
  else if ( i & TAG_DEPLACEMENT_NEGATIF )
    {
      if ( o->centre_deplacable & (1<<i) )
	{	  
	  *axe = u_soustrait_triplet(&o->px+(i&TAG_MASQUE), &in) ;
	  *axe = u_triplet_fois_flottant(axe, 0.5) ;
	  o->t.origine = u_ajoute_triplet(&in, axe) ;
	}
      else
	{
	  *axe = u_soustrait_triplet(&o->t.origine, &in) ;
	}
    }
  else if ( i & TAG_INVERSION )
    {
      *axe = u_triplet_fois_flottant(axe, -1) ;
      o->t.origine = u_soustrait_triplet(&o->t.origine, axe) ;
    }
  else if ( i & TAG_SYMETRIE )
    {
      if ( o->options & OPTION_AFFICHE_SEGMENT )
	o->inverse_z = !o->inverse_z ;
      else
	*axe = u_triplet_fois_flottant(axe, -1) ;
    }
  NOTE_UN_CHAMP_CHANGE(o) ;
  //  changement_repere(o) ;
}

const char* lit_tag_repere(Repere *o, const Champ *c)
{
  int i ;
  Triplet *axe, pt ;

  i = lit_numero_tag(c) ;
  axe = &(&o->px)[i & TAG_MASQUE] ;

  if ( i & TAG_ROTATION )
    {
      pt = u_barycentre_2_triplet(&o->t.origine, dist_tag_dilatation, axe);
      return( triplet_vers_char(&pt)) ;
    }
   if ( i & TAG_DILATATION )
     return("0") ;
   if ( i & TAG_ECHANGE )
     return("0") ;
   if ( i & TAG_DEPLACEMENT_NEGATIF )
     {
       Triplet n ;
       n = u_soustrait_triplet(&o->t.origine, (&o->t.v1) + (i&TAG_MASQUE)) ;
       return( triplet_vers_char(&n) ) ;
     }
   if ( i & TAG_DEPLACEMENT )
     return( triplet_vers_char(axe) ) ;
   if ( i & TAG_INVERSION )
     return( "Inversion" ) ;
   if ( i & TAG_SYMETRIE )
     return( "Symetrie" ) ;
   ICI ;
   eprintf("i=%x\n", i) ;
   return("") ;
}

CLASSE(repere, Repere,
       Extrait Edite Sauve Init3d
       CHAMP(t.origine, LABEL("Origine") L_point P_triplet DEFAUT("0 0 0"))
       L_vecteur P_triplet
       CHAMP(t.v1, LABEL("vx") DEFAUT("1 0 0"))
       CHAMP(t.v2, LABEL("vy") DEFAUT("0 1 0"))
       CHAMP(t.v3, LABEL("vz") DEFAUT("0 0 1"))
       NonEdite NonSauve Affiche NonExtrait NonInit3d
       L_point P_triplet
       CHAMP(px, LABEL("px"))
       CHAMP(py, LABEL("py"))
       CHAMP(pz, LABEL("pz"))
       CHAMP_VIRTUEL(L_affiche_pov(ecrit_pov_repere))
       CHAMP_VIRTUEL(L_tag(ecrit_tag_repere,lit_tag_repere))
       CHANGEMENT(changement_repere)
       CHAMP_REPERE_COMMUN
       MENU("Attributs/Géométrique 3D/Repère")
       EVENEMENT("R3V")
       A_EMPILER(globale_etat_courant.transfo) /* Hérité par les fils */
       )


CLASSE(repere_pts, Repere,
       SUR_CLASSE("repere") HERITE_PAS_DES_CHAMPS
       CHAMP(t.origine, LABEL("Origine") Extrait Edite Sauve L_point P_triplet DEFAUT("0 0 0") Init3d)
       L_vecteur P_triplet Affiche
       CHAMP(t.v1, LABEL("vx"))
       CHAMP(t.v2, LABEL("vy"))
       CHAMP(t.v3, LABEL("vz"))
       Extrait Edite Sauve
       L_point P_triplet
       CHAMP(px, LABEL("px") DEFAUT("1 0 0") Init3d)
       CHAMP(py, LABEL("py") DEFAUT("0 1 0") Init3d)
       CHAMP(pz, LABEL("pz") DEFAUT("0 0 1") Init3d)
       CHAMP_VIRTUEL(L_affiche_pov(ecrit_pov_repere))
       CHAMP_VIRTUEL(L_tag(ecrit_tag_repere,lit_tag_repere))
       CHANGEMENT(changement_repere_pts)
       EVENEMENT("R3P")
       CHAMP_REPERE_COMMUN
       )



#define CHAMPS_CALCULES				\
       L_vecteur P_triplet Affiche			\
       CHAMP(t.v1, LABEL("vx") DEFAUT("1 0 0"))	\
       CHAMP(t.v2, LABEL("vy") DEFAUT("0 1 0"))	\
       CHAMP(t.v3, LABEL("vz") DEFAUT("0 0 1"))	\
       L_point P_triplet					\
       CHAMP(px, LABEL("px"))			\
       CHAMP(py, LABEL("py"))			\
       CHAMP(pz, LABEL("pz"))			\
       CHAMP_REPERE_COMMUN



static void ecrit_pov_translation(Repere *o, const Champ *c,
				  const char *v)
{
  if ( v[0]=='\0' ) return ;
  fprintf( (FILE*)atol(v),
	   "translate <%g,%g,%g>\n",
	   o->t.origine.x, o->t.origine.y, o->t.origine.z
	   ) ;
}

static void changement_repere_translation(Repere *o)
{
  o->cache_tag = 1 ;
  o->cache_tag_symetrie = 1 ;
  changement_repere(o) ;
  o->options &= ~OPTION_AFFICHE_TAG_POINT_XYZ ;
}

CLASSE(translation, Repere,
       SUR_CLASSE("repere") HERITE_PAS_DES_CHAMPS
       CHAMP(t.origine, LABEL("Origine") Extrait Edite Sauve L_vecteur P_triplet DEFAUT("0 0 0") Init3d)
       CHAMPS_CALCULES
       EVENEMENT("TR")
       MENU("Attributs/Géométrique 3D/Translation")
       CHAMP_VIRTUEL(L_affiche_pov(ecrit_pov_translation))
       CHANGEMENT(changement_repere_translation)
       )

static void ecrit_pov_dilatation(Repere *o, const Champ *c,
				 const char *v)
{
  if ( v[0]=='\0' ) return ;
  fprintf( (FILE*)atol(v),
	   "scale <%g,%g,%g>\n",
	   o->vecteur.x, o->vecteur.y, o->vecteur.z
	   ) ;
}

static void changement_repere_dilatation(Repere *o)
{
  o->t.v1.x = o->vecteur.x ;
  o->t.v2.y = o->vecteur.y ;
  o->t.v3.z = o->vecteur.z ;
  o->cache_tag = 1 ;
  o->cache_tag_symetrie = 1 ;
  changement_repere(o) ;
  o->options &= ~(OPTION_AFFICHE_TAG_POINT_XYZ
		  | OPTION_AFFICHE_TAG_ORIGINE
		  ) ;
  o->options |= OPTION_AFFICHE_TAG_VECTEUR ;
}

CLASSE(dilatation, Repere,
       SUR_CLASSE("repere") HERITE_PAS_DES_CHAMPS
       CHAMP(vecteur, LABEL("Vect. Dilatation") Extrait Edite Sauve L_vecteur P_triplet DEFAUT("1 1 1") Init3d)
       CHAMP(t.origine, LABEL("Origine") Affiche L_point P_triplet)
       CHAMPS_CALCULES
       CHAMP_VIRTUEL(L_affiche_pov(ecrit_pov_dilatation))
       CHANGEMENT(changement_repere_dilatation)
       EVENEMENT("DIL")
       MENU("Attributs/Géométrique 3D/Dilatation")
       )

static void ecrit_pov_homothetie(Repere *o, const Champ *c,
				 const char *v)
{
  if ( v[0]=='\0' ) return ;
  fprintf( (FILE*)atol(v),
	   "scale <%g,%g,%g>\n",
	   o->homo, o->homo, o->homo
	   ) ;
}

static void changement_repere_homothetie(Repere*o)
{
  o->t.v1.x = o->homo ;
  o->t.v2.y = o->homo ;
  o->t.v3.z = o->homo ;
  changement_repere(o) ;
  o->cache_tag = 1 ;
  o->cache_tag_symetrie = 1 ;
  o->options &= ~(OPTION_AFFICHE_TAG_POINT_XYZ
		  | OPTION_AFFICHE_TAG_ORIGINE
		  ) ;
  o->options |= OPTION_AFFICHE_TAG_VALEUR ;
}

CLASSE(homothetie, Repere,
       SUR_CLASSE("repere") HERITE_PAS_DES_CHAMPS
       CHAMP(homo, LABEL("Coef. Homothétie")
	     Extrait Edite Sauve L_nombre P_flottant DEFAUT("1") Init3d)
       CHAMP(t.origine, LABEL("Origine") Affiche L_point P_triplet)
       CHAMPS_CALCULES
       CHAMP_VIRTUEL(L_affiche_pov(ecrit_pov_homothetie))
       CHANGEMENT(changement_repere_homothetie)
       EVENEMENT("HO")
       MENU("Attributs/Géométrique 3D/Homothétie")
       )

static void changement_repere_rotation(Repere*o)
{
  static const Triplet v[] = { {1,0,0}, {0,1,0}, {0,0,1} } ;

  o->t.v1 = u_rotation_triplet(&v[0], &o->vecteur, o->angle.angle) ;
  o->t.v2 = u_rotation_triplet(&v[1], &o->vecteur, o->angle.angle) ;
  o->t.v3 = u_rotation_triplet(&v[2], &o->vecteur, o->angle.angle) ;
  changement_repere(o) ;
  o->cache_tag = 1 ;
  o->cache_tag_symetrie = 1 ;
  o->options &= ~(OPTION_AFFICHE_TAG_POINT_XYZ
		  | OPTION_AFFICHE_TAG_ORIGINE
		  ) ;
  o->options |= OPTION_AFFICHE_TAG_VALEUR | OPTION_AFFICHE_TAG_VECTEUR ;
}

CLASSE(rotation, Repere,
       SUR_CLASSE("repere") HERITE_PAS_DES_CHAMPS
       CHAMP(angle.angle, Extrait L_nombre E_angle P_double Init3d)
       CHAMP(angle, LABEL("Angle rotation")
	     Edite Sauve L_nombre E_angle P_angle DEFAUT("0d"))
       CHAMP(vecteur, LABEL("Vect. rotation") Extrait Edite Sauve L_vecteur P_triplet DEFAUT("0 0 1") Init3d)
       CHAMP(t.origine, LABEL("Origine") DEFAUT("0 0 0") Affiche L_point P_triplet)
       CHAMPS_CALCULES
       CHAMP_VIRTUEL(L_affiche_pov(ecrit_pov_repere))
       CHANGEMENT(changement_repere_rotation)
       EVENEMENT("RO")
       MENU("Attributs/Géométrique 3D/Rotation")
       )

/*
 *
 */

static void changement_produit_repere(Repere*o)
{
  Transfo tmp ;
  const char *v ;

  if ( !FILS_CHANGE(o) )
    return ;

  u_identite_transfo(&o->t) ;

  POUR_LES_FILS(o, f,
		{
		  v = c_recupere_type_sans_copie(f, &infos_L_transfo) ;
		  if ( v[0] )
		    {
		      codec_sauve_texte(codec_transfo, &tmp, v) ;
		      o->t = u_produit_transfo(&o->t, &tmp) ;
		    }
		}
		) ;
  changement_repere(o) ;
  o->options = OPTION_AFFICHE_XYZ ;
}

CLASSE(produit_de_repere, Repere,
       CHAMP_REPERE_AFFICHAGE
       CHANGEMENT(changement_produit_repere)
       EVENEMENT("R*")
       PAQUET(NB_ACTIFS)
       MENU("Attributs/Géométrique 3D/Produit")
       )

/*
 *
 *
 *
 */


CLASSE(repere_pts2d, Repere,
       SUR_CLASSE("repere") HERITE_PAS_DES_CHAMPS
       MENU("Attributs/Géométrique 2D/Repère points")
       CHAMP(t.origine, LABEL("Origine") Extrait Edite Sauve L_point P_triplet DEFAUT("0 0 0") Init3d)
       L_vecteur P_triplet Affiche
       CHAMP(t.v1, LABEL("vx"))
       CHAMP(t.v2, LABEL("vy"))
       Extrait Edite Sauve
       L_point P_triplet
       CHAMP(px, LABEL("px") DEFAUT("1 0 0") Init3d)
       CHAMP(py, LABEL("py") DEFAUT("0 1 0") Init3d)
       CHAMP_VIRTUEL(L_affiche_pov(ecrit_pov_repere))
       CHAMP_VIRTUEL(L_tag(ecrit_tag_repere,lit_tag_repere))
       CHANGEMENT(changement_repere_pts2d)
       CHAMP_REPERE_COMMUN
       EVENEMENT("R2P")
       )

CLASSE(repere_2d, Repere,
       SUR_CLASSE("repere") HERITE_PAS_DES_CHAMPS
       MENU("Attributs/Géométrique 2D/Repère vecteur")

       Extrait Edite Sauve Init3d
       CHAMP(t.origine, LABEL("Origine") L_point P_triplet DEFAUT("0 0 0"))
       L_vecteur P_triplet
       CHAMP(t.v1, LABEL("vx") DEFAUT("1 0 0"))
       CHAMP(t.v2, LABEL("vy") DEFAUT("0 1 0"))
       NonEdite NonSauve Affiche NonExtrait NonInit3d
       L_point P_triplet
       CHAMP(px, LABEL("px"))
       CHAMP(py, LABEL("py"))
       CHANGEMENT(changement_repere2d)
       CHAMP_VIRTUEL(L_tag(ecrit_tag_repere,lit_tag_repere))
       CHAMP_REPERE_COMMUN
       EVENEMENT("R2V")
       )

/*
 * Repere orthonormé directe défini par deux points
 */

static void changement_repere_axe(Repere*o)
{
  if ( !UN_CHAMP_CHANGE(o) )
    return ;

  o->t.v1 = u_soustrait_triplet(&o->px, &o->t.origine) ;
  u_repere_de_vecteur(&o->t.v1, &o->t.v2, &o->t.v3) ;
  if ( o->normalise )
    {
      o->t.v1 = u_normalise_triplet(&o->t.v1) ;
      o->t.v2 = u_normalise_triplet(&o->t.v2) ;
      o->t.v3 = u_normalise_triplet(&o->t.v3) ;
    }

  if ( o->inverse_z )
    {
      o->t.v3 = u_triplet_fois_flottant(&o->t.v3, -1) ;
      o->t.v2 = u_triplet_fois_flottant(&o->t.v2, -1) ;
    }

  o->cache_tag = 1 ;
  o->cache_tag_symetrie = 1 ;
  o->py = u_ajoute_triplet(&o->t.v2, &o->t.origine) ;
  o->pz = u_ajoute_triplet(&o->t.v3, &o->t.origine) ;    
  repere_valide(o) ;

  if ( o->cache_repere )
    {
      o->options &= ~OPTION_AFFICHE_XYZ ;
    }
  o->options &= ~OPTION_AFFICHE_TAG_POINT_XYZ ;
  o->options |= OPTION_AFFICHE_SEGMENT
    | OPTION_AFFICHE_TAG_INVERSION_X
    | OPTION_AFFICHE_TAG_SYMETRIE_Y
    | OPTION_AFFICHE_TAG_POINT_X
    ;
}

CLASSE(repere_axe, Repere,
       SUR_CLASSE("repere") HERITE_PAS_DES_CHAMPS
       EVENEMENT("R3A")
       MENU("Attributs/Géométrique 3D/Repère axe")

       Edite Sauve Extrait Init3d
       CHAMP(t.origine, LABEL("Origine") L_point P_triplet DEFAUT("0 0 0"))
       CHAMP(px, LABEL("Point Direction") L_point P_triplet DEFAUT("1 0 0"))
       CHAMP(inverse_z, L_booleen NonExtrait)
       CHAMP(cache_repere, L_booleen NonExtrait)
       CHAMP(normalise, L_booleen NonExtrait DEFAUT("1"))
       CHAMP(fleche, L_booleen NonExtrait DEFAUT("0"))
       CHAMP_VIRTUEL(L_tag(ecrit_tag_repere,lit_tag_repere))

       CHAMP_REPERE_AFFICHAGE

       CHANGEMENT(changement_repere_axe)
       )
