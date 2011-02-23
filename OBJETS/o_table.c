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
#include <GL/glu.h>
#include <string.h>
#include <time.h>
#include "u_xt_interface.h"
#include "o_objet.h"		/* Car il faut definir un objet */
#include "t_geometrie.h"
#include "u_fichier.h"
#include "u_graphique.h"
#include "s_string.h"
#include "u_table.h"
#include "u_repartiteur.h"
#include "u_etat_courant.h"

typedef struct
{
  Table t ;
  Liste *sous_actifs ;
} Tbl ;


static void affiche_table(Tbl *o)
{
  if ( ACTIF(o) )
    t_table(o, c_trouve(o,"t"), T_TAG|T_TAG_DESTRUCTION|T_TAG_POIDS|T_TAG_SPLIT) ;
  else
    t_table(o, c_trouve(o,"t"), T_PONCTUEL) ;
}
static void affiche_ligne_brisee(Tbl *o)
{
  if ( ACTIF(o) )
    t_table(o, c_trouve(o,"t"), T_FILAIRE|T_TAG|T_TAG_DESTRUCTION|T_TAG_POIDS|T_TAG_SPLIT) ;
  else
    t_table(o, c_trouve(o,"t"), T_FILAIRE) ;
}

static void ecrit_pov_ligne_brisee(Tbl *o,
				   const Champ *c, const char *v)
{
  t_table_pov(o, c_trouve(o,"t"), T_FILAIRE|T_PONCTUEL, v) ;
}
static void ecrit_pov_polygone(Tbl *o,
				   const Champ *c, const char *v)
{
  t_table_pov(o, c_trouve(o,"t"), T_POLYGONE|T_PONCTUEL, v) ;
}
static void ecrit_pov_facette(Tbl *o,
				   const Champ *c, const char *v)
{
  t_table_pov(o, c_trouve(o,"t"), T_FACETTE, v) ;
}
static void ecrit_pov_ponctuel(Tbl *o,
				   const Champ *c, const char *v)
{
  t_table_pov(o, c_trouve(o,"t"), T_PONCTUEL, v) ;
}
static void ecrit_stl_ligne_brisee(Tbl *o,
				   const Champ *c, const char *v)
{
  t_table_stl(o, c_trouve(o,"t"), T_FILAIRE|T_PONCTUEL, v) ;
}
static void ecrit_stl_polygone(Tbl *o,
				   const Champ *c, const char *v)
{
  t_table_stl(o, c_trouve(o,"t"), T_POLYGONE|T_PONCTUEL, v) ;
}
static void ecrit_stl_facette(Tbl *o,
				   const Champ *c, const char *v)
{
  t_table_stl(o, c_trouve(o,"t"), T_FACETTE|T_POLYGONE|T_PONCTUEL, v) ;
}
static void ecrit_stl_ponctuel(Tbl *o,
				   const Champ *c, const char *v)
{
  t_table_stl(o, c_trouve(o,"t"), T_PONCTUEL, v) ;
}

/*
 * Attention procédure de changement bidirectionnelle
 * Elle marche pour TOUS les "codec_L"
 */

static void changement_table_n_uplet(void *o)
{
  changement_table(o, c_trouve(o, "t")) ;
}

CLASSE(table, Tbl,
       CHAMP(t.nb, L_entier Affiche NonInitialiser)
       CHAMP_VIRTUEL(L_tag(ecrit_tag_table,lit_tag_table))
       MENU("NON")
       )

CLASSE(table_non_contrainte, Tbl,
       SUR_CLASSE("table")
       CHAMP_VIRTUEL(L_affiche_gl(affiche_table)
		     AFFICHAGE(Different_si_actif|Different_suivant_qualite))
       CHAMP_VIRTUEL(L_affiche_pov(ecrit_pov_ponctuel))
       CHAMP_VIRTUEL(L_affiche_stl(ecrit_stl_ponctuel))
       )

CLASSE(table_nc_point3, Tbl,
       SUR_CLASSE("table_non_contrainte")
       CHAMP(t   , L_table_point3 Edite Sauve)
       MENU("Valeurs/Table/Table non contrainte de points3")
       EVENEMENT("TNCP3")
       )

CLASSE(table_nc_point4, Tbl,
       SUR_CLASSE("table_non_contrainte")
       CHAMP(t   , L_table_point4 P_table_quadruplet1 Edite Sauve)
       MENU("Valeurs/Table/Table non contrainte de points4")
       EVENEMENT("TNCP4")
       )

CLASSE(table_nc_flottant, Tbl,
       SUR_CLASSE("table_non_contrainte")
       CHAMP(t   , L_table_flottant Edite Sauve InitChaine)
       MENU("Valeurs/Table/Table non contrainte de flottants")
       EVENEMENT("TNCFL")
       )

CLASSE(table_contrainte, Tbl,
       SUR_CLASSE("table_non_contrainte")
       CHANGEMENT(changement_table_n_uplet)
       PAQUET(NB_ACTIFS)
       )

CLASSE(table_flottant, Tbl,
       SUR_CLASSE("table_contrainte")
       CHAMP(t   ,L_table_flottant Edite Obligatoire TYPE_DANS_FILS(L_nombre))
       EVENEMENT("TFL")
       MENU("Valeurs/Table/Table")
       )

CLASSE(table_point3, Tbl,
       SUR_CLASSE("table_contrainte")
       CHAMP(t   ,L_table_point3 Edite Obligatoire TYPE_DANS_FILS(L_point))
       EVENEMENT("TP3")
       MENU("Valeurs/Table/Table")
       )

CLASSE(table_point4, Tbl,
       SUR_CLASSE("table_contrainte")
       CHAMP(t   , L_table_point4 P_table_quadruplet1 Edite Obligatoire TYPE_DANS_FILS(L_point))
       EVENEMENT("TP4")
       MENU("Valeurs/Table/Table")
       )

CLASSE(table_triplet, Tbl,
       SUR_CLASSE("table_contrainte")
       CHAMP(t   ,L_table_point3 Edite Obligatoire TYPE_DANS_FILS(L_triplet))
       EVENEMENT("T3")
       MENU("Valeurs/Table/Table")
       )

CLASSE(table_quadruplet, Tbl,
       SUR_CLASSE("table_contrainte")
       CHAMP(t   , L_table_point4 P_table_quadruplet1 Edite Obligatoire TYPE_DANS_FILS(L_quadruplet))
       EVENEMENT("TQ")
       MENU("Valeurs/Table/Table")
       )

CLASSE(table_transfo, Tbl,
       SUR_CLASSE("table")
       CHAMP(t   , L_table_transfo Obligatoire Edite TYPE_DANS_FILS(L_transfo))
       EVENEMENT("T4")
       PAQUET(NB_ACTIFS)
       CHANGEMENT(changement_table_n_uplet)
       MENU("Valeurs/Table/Table")
       )

/*
 *
 */

typedef struct
{
  Table_quadruplet t ;		/* EN PREMIER */

  Table_triplet tp3 ;
  Table_flottant poids ;
} Point_plus_poids ;

static void changement_table_point_plus_poids(Point_plus_poids *o)
{
  int i ;

  if ( CHAMP_CHANGE(o,tp3) || CHAMP_CHANGE(o,poids) )
    {
      if ( TRACE(o) )
	eprintf("Copie tp3+poids dans tp4\n") ;
      REALLOUER(o->t.table, o->tp3.nb) ;
      o->t.nb = o->tp3.nb ;
      for(i=0;i<o->tp3.nb;i++)
	{
	  o->t.table[i].x = o->tp3.table[i].x ;
	  o->t.table[i].y = o->tp3.table[i].y ;
	  o->t.table[i].z = o->tp3.table[i].z ;
	  o->t.table[i].h = (i<o->poids.nb) ? o->poids.table[i] : 1 ;
	  if ( TRACE(o) )
	    eprintf("%s\n", quadruplet_vers_char(&o->t.table[i])) ;
	}
    }
  else
    if ( CHAMP_CHANGE(o, t) )
      {
	if ( TRACE(o) )
	  eprintf("Copie t dans tp3+poids\n") ;
	REALLOUER(o->tp3.table, o->t.nb) ;
	REALLOUER(o->poids.table, o->t.nb) ;
	o->tp3.nb = o->t.nb ;
	o->poids.nb = o->t.nb ;
	for(i=0;i<o->t.nb;i++)
	  {
	    if ( TRACE(o) )
	      eprintf("%s\n", quadruplet_vers_char(&o->t.table[i])) ;
	    o->tp3.table[i].x = o->t.table[i].x ;
	    o->tp3.table[i].y =	o->t.table[i].y ;
	    o->tp3.table[i].z =	o->t.table[i].z ;
	    o->poids.table[i] = o->t.table[i].h ;
	  }
      }
}

CLASSE(table_point3_plus_poids, Point_plus_poids,
       SUR_CLASSE("table")
       CHAMP(t, L_table_point4)
       CHAMP(tp3  , L_table_point  P_table_triplet  Extrait Edite Sauve)
       CHAMP(poids, L_table_nombre P_table_flottant Extrait Edite Sauve)
       CHANGEMENT(changement_table_point_plus_poids)
       CHAMP_VIRTUEL(L_affiche_pov(ecrit_pov_ponctuel))
       CHAMP_VIRTUEL(L_affiche_stl(ecrit_stl_ponctuel))
       CHAMP_VIRTUEL(L_affiche_gl(affiche_table)
		     AFFICHAGE(Different_si_actif|Different_suivant_qualite))
       MENU("Valeurs/Table/Table")
       EVENEMENT("TPP")
       )
/*
 *
 */


CLASSE(ligne_brisee, Tbl,
       SUR_CLASSE("table")
       CHAMP(t, L_table_point P_table_triplet Extrait Edite Sauve)
       CHAMP_VIRTUEL(L_affiche_gl(affiche_ligne_brisee)
		     AFFICHAGE(Different_si_actif|Different_suivant_qualite))
       CHAMP_VIRTUEL(L_affiche_pov(ecrit_pov_ligne_brisee))
       CHAMP_VIRTUEL(L_affiche_stl(ecrit_stl_ligne_brisee))
       MENU("Figures/1d/Ligne Brisée")
       EVENEMENT("Shft+LB")
       )

/*
 *
 */

static void affiche_polygone(Tbl *o)
{
  if ( ACTIF(o) )
    t_table(o, c_trouve(o,"t"), T_POLYGONE|T_TAG|T_TAG_POIDS) ;
  else
    t_table(o, c_trouve(o,"t"), T_POLYGONE) ;
}

CLASSE(polygone, Tbl,
       SUR_CLASSE("table")
       CHAMP(t, L_table_point P_table_triplet Extrait Edite Sauve)
       CHAMP_VIRTUEL(L_affiche_gl(affiche_polygone)
		     AFFICHAGE(Different_si_actif|Different_suivant_qualite))
       CHAMP_VIRTUEL(L_affiche_pov(ecrit_pov_polygone))
       CHAMP_VIRTUEL(L_affiche_stl(ecrit_stl_polygone))
       MENU("Figures/1d/Polygone")
       EVENEMENT("Shft+PO")
       )

/*
 *
 */

static void affiche_facette(Tbl *o)
{
  /*
  Triplet p1 ;

  if ( ACTIF(o) )
    {
      u_annule_triplet(&p1) ;
      t_tag_origine(0, &p1) ;
      glEnable(GL_LIGHTING) ;
    }

  p1.z = 1 ;
  glBegin(GL_POLYGON) ;
  glNormal3dv((GLdouble*)&p1) ;
  glVertex3f(0,0,0) ;
  glVertex3f(1,0,0) ;
  glVertex3f(1,1,0) ;
  glVertex3f(0,1,0) ;
  glEnd() ;
  if ( ACTIF(o) )
    glDisable(GL_LIGHTING) ;
  return ;
  */

  if ( ACTIF(o) )
    t_table(o, c_trouve(o,"t"), T_FACETTE|T_TAG|T_TAG_POIDS) ;
  else
    t_table(o, c_trouve(o,"t"), T_FACETTE) ;
}

CLASSE(facette, Tbl,
       SUR_CLASSE("table")
       CHAMP(t, L_table_point P_table_triplet Extrait Edite Sauve)
       CHAMP_VIRTUEL(L_affiche_gl(affiche_facette)
		     AFFICHAGE(Different_si_actif|Different_suivant_qualite))
       CHAMP_VIRTUEL(L_affiche_pov(ecrit_pov_facette))
       CHAMP_VIRTUEL(L_affiche_stl(ecrit_stl_facette))
       MENU("Figures/2d/Facette")
       EVENEMENT("Ctrl+FA")
       )





/*
 ******************************************************************************
 ******************************************************************************
 * GRILLES
 ******************************************************************************
 ******************************************************************************
 */

typedef struct
{
  Grille_triplet t ;	/* Les coordonnees des points */
} Gt ;

static void ecrit_pov_grille_facette(Gt *o, const Champ *c, const char *v)
{
  t_grille_pov(o, c_trouve(o,"t"), T_FACETTE, v, NULL) ;
}
static void ecrit_pov_grille_filaire(Gt *o, const Champ *c, const char *v)
{
  t_grille_pov(o, c_trouve(o,"t"), T_FILAIRE|T_PONCTUEL, v, NULL) ;
}
static void ecrit_pov_grille_ponctuelle(Gt *o, const Champ *c, const char *v)
{
  t_grille_pov(o, c_trouve(o,"t"), T_PONCTUEL, v, NULL) ;
}

/*
 * Attention procédure de changement bidirectionnelle
 */

static void changement_grille_n_uplet(void *o)
{
  changement_grille(o, c_trouve(o, "t")) ;
}

static void affiche_iso_grille(Gt *o, const Champ *ch, const char *urgent)
{
  if ( ACTIF(o) )
    t_grille(o, c_trouve(o,"t"), T_FILAIRE|T_TAG|T_TAG_POIDS, urgent, NULL) ;
  else
    t_grille(o, c_trouve(o,"t"), T_FILAIRE, urgent, NULL) ;
}

/*
 *
 */

static void affiche_point_grille(Gt *o, const Champ *ch, const char *urgent)
{
  if ( ACTIF(o) )
    t_grille(o, c_trouve(o,"t"), T_TAG|T_TAG_POIDS, urgent, NULL) ;
  else
    t_grille(o, c_trouve(o,"t"), T_PONCTUEL, urgent, NULL) ;
}

/*
 *
 */

static void affiche_facette_grille(Gt *o, const Champ *ch, const char *urgent)
{
  if ( ACTIF(o) )
    t_grille(o, c_trouve(o,"t"), T_FACETTE|T_TAG|T_TAG_POIDS, urgent, NULL) ;
  else
    t_grille(o, c_trouve(o,"t"), T_FACETTE, urgent, NULL) ;
}

/*
 *
 */

static void affiche_stl_grille(Gt *o, const Champ *ch, const char *urgent)
{
  t_grille_stl(o, c_trouve(o,"t"), T_FACETTE|T_FILAIRE|T_PONCTUEL, urgent) ;
}

static void affiche_stl_iso_grille(Gt *o, const Champ *ch, const char *urgent)
{
  t_grille_stl(o, c_trouve(o,"t"), T_FILAIRE|T_PONCTUEL, urgent) ;
}

static void affiche_stl_pt_grille(Gt *o, const Champ *ch, const char *urgent)
{
  t_grille_stl(o, c_trouve(o,"t"), T_PONCTUEL, urgent) ;
}

/*
 *
 */

CLASSE(grille, Gt,
       CHAMP(t.nb_lignes  , L_entier Affiche NonInitialiser)
       CHAMP(t.nb_colonnes, L_entier Affiche NonInitialiser)
       CHAMP_VIRTUEL(L_tag(ecrit_tag_grille,lit_tag_grille))
       MENU("NON")
       )



CLASSE(grille_non_contrainte, Gt,
       SUR_CLASSE("grille")
       CHAMP_VIRTUEL(L_affiche_gl(affiche_point_grille)
		     AFFICHAGE(Different_si_actif|Different_suivant_qualite))
       CHAMP_VIRTUEL(L_affiche_pov(ecrit_pov_grille_ponctuelle))
       )

CLASSE(grille_contrainte, Gt,
       SUR_CLASSE("grille_non_contrainte")
       CHANGEMENT(changement_grille_n_uplet)
       PAQUET(NB_ACTIFS)
       )





CLASSE(grille_nc_point3, Gt,
       SUR_CLASSE("grille_non_contrainte")
       CHAMP(t, L_grille_point3 Edite Sauve)
       CHAMP_VIRTUEL(L_affiche_stl(affiche_stl_pt_grille))
       EVENEMENT("GNCP3")
       MENU("Valeurs/Grille/Grille Non Contrainte de point3")
       TYPE_DANS_FILS(L_table_point)
       )
CLASSE(grille_nc_point4, Gt,
       SUR_CLASSE("grille_non_contrainte")
       CHAMP(t, L_grille_point4 Edite Sauve)
       CHAMP_VIRTUEL(L_affiche_stl(affiche_stl_pt_grille))
       EVENEMENT("GNCP4")
       MENU("Valeurs/Grille/Grille Non Contrainte de point4")
       TYPE_DANS_FILS(L_table_point)
       )
CLASSE(grille_nc_flottant, Gt,
       SUR_CLASSE("grille_non_contrainte")
       CHAMP(t, L_grille_flottant Edite Sauve)
       CHAMP_VIRTUEL(L_affiche_stl(affiche_stl_pt_grille))
       EVENEMENT("GNCFL")
       MENU("Valeurs/Grille/Grille Non Contrainte de flottant")
       TYPE_DANS_FILS(L_table_nombre)
       )





CLASSE(grille_point3, Gt,
       SUR_CLASSE("grille_contrainte")
       CHAMP(t,
	     L_grille_point3 TYPE_DANS_FILS(L_table_point)
	     Edite Obligatoire
	     )
       CHAMP_VIRTUEL(L_affiche_stl(affiche_stl_pt_grille))
       EVENEMENT("GP3")
       MENU("Valeurs/Grille/Grille")
       TYPE_DANS_FILS(L_table_point)
       )

CLASSE(grille_point4, Gt,
       SUR_CLASSE("grille_contrainte")
       CHAMP(t,
	     L_grille_point4
	     Edite Obligatoire 
	     TYPE_DANS_FILS(L_table_point)
	     )
       CHAMP_VIRTUEL(L_affiche_stl(affiche_stl_pt_grille))
       EVENEMENT("GP4")
       MENU("Valeurs/Grille/Grille")
       TYPE_DANS_FILS(L_table_point)
       )

CLASSE(grille_flottant, Gt,
       SUR_CLASSE("grille_contrainte")
       CHAMP(t,
	     L_grille_flottant
	     Edite Obligatoire 
	     TYPE_DANS_FILS(L_table_flottant)
	     )
       EVENEMENT("GFL")
       CHAMP_VIRTUEL(L_affiche_stl(affiche_stl_pt_grille))
       MENU("Valeurs/Grille/Grille")
       TYPE_DANS_FILS(L_table_nombre)
       )



CLASSE(iso_grille_general, Gt,
       SUR_CLASSE("grille")
       EVENEMENT("Shft+IG")
       CHAMP_VIRTUEL(L_affiche_gl(affiche_iso_grille)
		     AFFICHAGE(Different_si_actif|Different_suivant_qualite))
       CHAMP_VIRTUEL(L_affiche_stl(affiche_stl_iso_grille))
       CHAMP_VIRTUEL(L_affiche_pov(ecrit_pov_grille_filaire))
)


CLASSE(iso_grille, Gt,
       SUR_CLASSE("iso_grille_general")
       CHAMP(t,
	     L_grille_point P_grille_triplet Obligatoire 
	     Edite Extrait Sauve
	     )
       MENU("Figures/1d/Iso")
       TYPE_DANS_FILS(L_table_point)
       )

CLASSE(iso_grille_flottant, Gt,
       SUR_CLASSE("iso_grille_general")
       CHAMP(t,
	     L_grille_nombre P_grille_flottant Obligatoire 
	     Edite Extrait Sauve
	     )
       EVENEMENT("1i1g")
       MENU("Figures/1d/Iso")
       TYPE_DANS_FILS(L_table_nombre)
       )
     /*
CLASSE_ENLEVEE(point_grille, Gt,
       SUR_CLASSE("grille")
       CHAMP(t,
	     L_grille_point P_grille_triplet Obligatoire 
	     Edite Extrait Sauve
	     )
       EVENEMENT("0p03")
       MENU("Figures/0d/Point")
       CHAMP_VIRTUEL(L_affiche_gl(affiche_point_grille))
       CHAMP_VIRTUEL(L_affiche_pov(ecrit_pov_grille))
       )
     */
CLASSE(facette_grille, Gt,
       SUR_CLASSE("grille")
       CHAMP(t,
	     L_grille_point P_grille_triplet Obligatoire 
	     Edite Extrait Sauve
	     )
       EVENEMENT("Ctrl+FG")
       MENU("Figures/2d/Facette")
       CHAMP_VIRTUEL(L_affiche_gl(affiche_facette_grille)
		     AFFICHAGE(Different_si_actif|Different_suivant_qualite))
       CHAMP_VIRTUEL(L_affiche_pov(ecrit_pov_grille_facette))
       CHAMP_VIRTUEL(L_affiche_stl(affiche_stl_grille))
       TYPE_DANS_FILS(L_table_point)
       )
/*
 *
 */

typedef struct
{
  Grille_quadruplet t ;
  Grille_triplet gp3 ;
  Grille_flottant poids ;
} Grille_plus_poids ;

static void changement_grille_point_plus_poids(Grille_plus_poids *o)
{
  int i, j ;

  if ( CHAMP_CHANGE(o,gp3) || CHAMP_CHANGE(o,poids) )
    {
      if ( TRACE(o) )
	eprintf("Copie gp3+poids dans tp4\n") ;
      
      for(j=o->gp3.nb_lignes; j<o->t.nb_lignes; j++)
	free(o->t.grille[j]) ;
      REALLOUER(o->t.grille, o->gp3.nb_lignes) ;
      for(j=o->t.nb_lignes; j<o->gp3.nb_lignes; j++)
	o->t.grille[j] = NULL ;

      o->t.nb_lignes   = o->gp3.nb_lignes   ;
      o->t.nb_colonnes = o->gp3.nb_colonnes ;

      for(j=0;j<o->gp3.nb_lignes;j++)
	{
	  REALLOUER(o->t.grille[j], o->gp3.nb_colonnes) ;
	  for(i=0;i<o->gp3.nb_colonnes;i++)
	    {
	      o->t.grille[j][i].x = o->gp3.grille[j][i].x ;
	      o->t.grille[j][i].y = o->gp3.grille[j][i].y ;
	      o->t.grille[j][i].z = o->gp3.grille[j][i].z ;
	      o->t.grille[j][i].h
		= (i<o->poids.nb_colonnes && j<o->poids.nb_lignes) ? o->poids.grille[j][i] : 1 ;
	    }
	}
      
    }
  else
    if ( CHAMP_CHANGE(o, t) )
      {
	if ( TRACE(o) )
	  eprintf("Copie t dans tp3+poids\n") ;
	REALLOUER(o->gp3.grille, o->t.nb_lignes) ;
	REALLOUER(o->poids.grille, o->t.nb_lignes) ;
        o->  gp3.nb_colonnes = o->t.nb_colonnes ;
        o->poids.nb_colonnes = o->t.nb_colonnes ;
        o->  gp3.nb_lignes   = o->t.nb_lignes   ;
        o->poids.nb_lignes   = o->t.nb_lignes   ;
	for(j=0;j<o->t.nb_lignes;j++)
	  {
	    REALLOUER(o->gp3.grille[j], o->t.nb_colonnes) ;
	    REALLOUER(o->poids.grille[j], o->t.nb_colonnes) ;
	    for(i=0;i<o->t.nb_colonnes;i++)
	      {
		o->gp3.grille[j][i].x = o->t.grille[j][i].x ;
		o->gp3.grille[j][i].y = o->t.grille[j][i].y ;
		o->gp3.grille[j][i].z = o->t.grille[j][i].z ;
		o->poids.grille[j][i] = o->t.grille[j][i].h ;
	      }
	  }
      }
}

CLASSE(grille_point3_plus_poids, Grille_plus_poids,
       SUR_CLASSE("grille")
       CHAMP(t, NOM("t") L_grille_point4)
       CHAMP(gp3  , L_grille_point  P_grille_triplet  Extrait Edite Sauve)
       CHAMP(poids, L_grille_nombre P_grille_flottant Extrait Edite Sauve)
       CHAMP_VIRTUEL(L_affiche_gl(affiche_point_grille)
		     AFFICHAGE(Different_si_actif|Different_suivant_qualite))
       CHAMP_VIRTUEL(L_affiche_pov(ecrit_pov_grille_ponctuelle))
       CHANGEMENT(changement_grille_point_plus_poids)
       MENU("Valeurs/Grille/Grille")
       EVENEMENT("GPP")
       )

/*
 ******************************************************************************
 ******************************************************************************
 * TENSEUR
 ******************************************************************************
 ******************************************************************************
 */

typedef struct
{
  t_P_H_L_l3_double t ;	/* Les coordonnees des points */
} Tenseur ;

static void affiche_point_tenseur(Tenseur *o, const Champ *ch, const char *urgent)
{
  int comment ;

  switch( globale_etat_courant.type_affichage )
    {
    case Ponctuel:
      comment = T_PONCTUEL ;
      break ;
    default:
      comment = T_FILAIRE ;
      break ;
    }
  if ( ACTIF(o) )
    {
      comment |= T_TAG ;
      if ( c_trouve(o, "t")->tp == &infos_codec_P_H_L_l4_double1 )
	comment |= T_TAG_POIDS ;
    }

  t_tenseur(o, c_trouve(o,"t")->tp, comment, urgent) ;
}

static void changement_du_tenseur(void *o)
{
  changement_tenseur(o, c_trouve(o, "t")) ;
}


CLASSE(tenseur, Tenseur,
       CHAMP_VIRTUEL(L_affiche_gl(affiche_point_tenseur)
		     AFFICHAGE(Different_si_actif|Different_suivant_qualite))
       CHAMP_VIRTUEL(L_tag(ecrit_tag_tenseur,lit_tag_tenseur))
       PAQUET(NB_ACTIFS)
       CHANGEMENT(changement_du_tenseur)
       )

CLASSE(tenseur_point3, Tenseur,
       SUR_CLASSE("tenseur")
       CHAMP(t, NOM("t") L_tenseur P_tenseur_l3_double Edite Sauve
	     TYPE_DANS_FILS(L_grille_point))
       MENU("Valeurs/Tenseur")
       EVENEMENT("3P3")
       )

CLASSE(tenseur_point4, Tenseur,
       SUR_CLASSE("tenseur")
       CHAMP(t, NOM("t") L_tenseur P_tenseur_l4_double1 Edite Sauve
	     TYPE_DANS_FILS(L_grille_point))
       MENU("Valeurs/Tenseur")
       EVENEMENT("3P4")
       )


/*
 ******************************************************************************
 ******************************************************************************
 * NUAGE
 * Implementation peu performante car le nuage
 * est detruit et relu a chaque UNDO/REDO
 ******************************************************************************
 ******************************************************************************
 */

#define NB_POINTS 10000

static Booleen charge_un_peu_du_nuage(void *objet)
{
  int i ;
  Triplet *f ;
  static time_t dernier_affichage ;
  struct nuage *o = objet ;

  if ( o->a_lire == NULL )
    {
      return(Faux) ; /* On ne continue pas */
    }

  REALLOUER(o->t.table, o->t.nb+NB_POINTS) ;
  f = &o->t.table[o->t.nb] ;

  for(i=0;i<NB_POINTS;i++)
    {
      if ( fscanf(o->a_lire,FF3, &f->x, &f->y, &f->z) != 3 )
	{
	  if ( feof(o->a_lire) )
	    {
	      REALLOUER(o->t.table, o->t.nb) ;
	      fclose(o->a_lire) ;
	      o->a_lire = NULL ;
	      if ( o->repositionne_observateur )
		u_observateur_de_boite_min_max(&o->englobant, &o->observateur);
	      o->repositionne_observateur = Faux ;
	      evaluation(o) ;
	      return(Faux) ;
	    }
	  fscanf(o->a_lire,"%*[^\n]\n") ; /* Saute la mauvaise ligne */
	}
      else
	{
	  u_agrandir_boite_min_max(&o->englobant, f) ;
	  f++ ;
	  o->t.nb++ ;
	}
    }
  if ( time(0L) != dernier_affichage )
    {
      dernier_affichage = time(0L) ;
      evaluation(o) ; /* Seulement une fois par seconde */
    }
  return(Vrai) ;
}


static void destruction_nuage(Nuage *o)
{
  u_enleve_action(charge_un_peu_du_nuage, o) ;
  if ( o->t.table )
    {
      free(o->t.table) ;
      o->t.table = NULL ;
      o->t.nb = 0 ;
    }
  if ( o->a_lire )
    {
      fclose(o->a_lire) ;
      o->a_lire = NULL ;
    }
}

static void changement_nuage(Nuage *o)
{
  if ( CHAMP_CHANGE(o, nom_fichier) )
    {
      if ( !CREATION(o) )
	o->repositionne_observateur = Vrai ;

      destruction_nuage(o) ;
      o->a_lire = fopen(u_trouve_fichier(directories_objets, o->nom_fichier,
					 "/MODELES/",""),"r") ;
      u_annule_boite_min_max(&o->englobant) ;
      u_ajoute_action(PRIORITE_ARRIERE_PLAN, charge_un_peu_du_nuage, o) ;
    }
}

static void affiche_nuage(Nuage *o, const Champ *ch, const char *urgent)
{
 int i, k ;
 GLint mode ;

 glPushAttrib(GL_POINT_BIT) ;

  k = '8' - urgent[0] ;
  if ( k < 0 )
    k = 0 ;
  k = 1<<k ;

 glGetIntegerv(GL_RENDER_MODE, &mode) ; 

 if ( mode != GL_SELECT )
   {
     glBegin(GL_POINTS) ;
     if ( k!=1 )
       {
	 srand(0) ;
	 for (i=0 ; i<o->t.nb ; i+=k )
	   {	 
	     t_triplet(&o->t.table[i]) ;
	   }
       }
     else
       for (i=0 ; i<o->t.nb ; i++)
	 {	 
	   t_triplet(&o->t.table[i]) ;
	 }
     glEnd();
     u_multiplie_taille_point(4) ;
     glBegin(GL_POINTS) ;
     if ( TRACE(o) )
       eprintf("Liste des sous-actifs :") ;
     
     POUR(long, n, o->sous_actifs,
	  {
	    if ( TRACE(o) )
	      fprintf(stderr, " %ld", n) ;
	    n -= TAG_DEPLACEMENT ;
	    if ( n < o->t.nb )
	      t_triplet(&o->t.table[n]) ;
	  }) ;
     if ( TRACE(o) )
       fprintf(stderr, "\n") ;
     glEnd();
   }
 else
   {
     glPushName(0) ;
     for (i=0 ; i<o->t.nb ; i+=k)
       {	
	 glLoadName(i+TAG_DEPLACEMENT) ;
	 glBegin(GL_POINTS) ;
	 t_triplet(&o->t.table[i]) ;
	 glEnd();
       }
     glPopName() ;
   }
 
 glPopAttrib() ;
}


static void change_point_nuage(Nuage *o, const Champ *c, const char *v)
{
  int i ;

  i = lit_numero_tag(c) - TAG_DEPLACEMENT ;
  if ( i >= 0 && i < o->t.nb )
    {
      /*
       * Changement d'un point
       */
      if ( TRACE(o) )
	eprintf("ecrit tag numéro %d, valeur %s\n", i, v) ;
      o->t.table[i] = char_vers_triplet(v) ;
    }
}

static const char * info_point_nuage(Nuage *o, const Champ *c)
{
  int i ;
  static Chaine s ;

  i = lit_numero_tag(c) - TAG_DEPLACEMENT ;
  if ( i >= 0 && i < o->t.nb )
    {
      s_efface(&s) ;
      s_printf(&s
	       ,"%s : Point %d du nuage"
	       ,triplet_vers_char(&o->t.table[i])
	       ,i
	       ) ;
      return( s_char(&s) ) ;
    }
  return("") ;
}


CLASSE(nuage, Nuage,
       CHAMP(nom_fichier, L_chaine Edite Sauve InitChaine
	     DEFAUT("/vol/image/SCAN/SORTIE/6835.dat")
	     )
       CHAMP(t.nb, LABEL("Nombre de points du nuage") L_entier Affiche)
       CHAMP(sous_actifs, L_sous_actif Affiche Sauve)
       CHAMP(englobant, L_boite_min_max Affiche)
       CHAMP(observateur, L_observateur Edite Extrait)
       CHAMP_VIRTUEL(L_affiche_gl(affiche_nuage)
		     AFFICHAGE(Different_si_actif|Different_suivant_qualite))
       CHAMP_VIRTUEL(L_tag(change_point_nuage,info_point_nuage))
       CHANGEMENT(changement_nuage)
       DESTRUCTION(destruction_nuage)
       MENU("Figures/0d/Nuage")
       EVENEMENT("N3")
       )

/*
 ******************************************************************************
 ******************************************************************************
 * LIGNES DANS NUAGE
 ******************************************************************************
 ******************************************************************************
 */


struct lignes_dans_nuage
{
  Table_table_triplet lignes ;
  int position_d_insertion ; /* 0 : avant le premier point */
  int ligne_active ;
  Liste *sous_actifs ; 
} ;

/*
 * Macro pour simplifier
 */

#define LC  o->lignes.table[o->ligne_active]
#define NB LC.nb

/*
 * Fonction des boutons
 */

static Booleen detruit_ligne(const Interface_widget *iw)
{
  struct lignes_dans_nuage *o = iw->objet ;

  if ( o->ligne_active < 0 )
    return(Faux) ;

  if ( TRACE(o) )
    eprintf("Detruit ligne %d\n", o->ligne_active) ;
  free(LC.table) ;
  memmove(&LC, &LC+1, (o->lignes.nb-o->ligne_active-1)*sizeof(LC)) ;
  o->lignes.nb-- ;
  if ( o->ligne_active == o->lignes.nb )
    o->ligne_active-- ;
  return(Faux) ;
}

static void ajoute_ligne(struct lignes_dans_nuage *o)
{
  o->ligne_active = o->lignes.nb++ ;
  REALLOUER(o->lignes.table, o->lignes.nb) ;
  ALLOUER(LC.table,1) ;
  LC.nb = 0 ;
}

static Booleen ajoute_ligne_iw(const Interface_widget *iw)
{
  ajoute_ligne((struct lignes_dans_nuage*) iw->objet) ;
  return(Faux) ;
}


/*
 * Codage du sous actif
 */
typedef union {
  int n ;
  struct {
    unsigned int ligne:15 ;	/* Numero de la ligne brisee */
    unsigned int type:2 ;	/* 0=point, 1=segment, 2=ligne, 3=icone */
    unsigned int n:15 ;		/* Numero du point ou segment */
  } s ;
} Num ;

enum { Point, Segment, Ligne, Icone } ;

/*
static void fleche(Triplet *debut_segment,
		   Triplet *fin_segment,
		   int plein)
{
  Triplet v, base, sommet ;

  v = u_soustrait_triplet(fin_segment, debut_segment) ;
  v = u_triplet_fois_flottant(&v, 1./3) ;
  base = u_ajoute_triplet(debut_segment, &v) ;
  sommet = u_ajoute_triplet(&base, &v) ;
  t_fleche(&base, &sommet, plein) ;
}
*/

static void affiche_lignes_dans_nuage(struct lignes_dans_nuage *o)
{
 int i, j ;
 Num a ;
 Triplet tmp ;

 glPushAttrib(GL_LINE_BIT|GL_CURRENT_BIT|GL_POINT_BIT|GL_LIGHTING_BIT) ;
 glPushName(0) ;
 u_multiplie_taille_point(4) ;
 glShadeModel(GL_SMOOTH) ;

 /*
  * Affiche les lignes brisees autre qu'active
  */
 a.s.type = Ligne ;
 for(i=0;i<o->lignes.nb;i++)
   {
     if ( i==o->ligne_active && ACTIF(o) )
       continue ;
     if ( o->lignes.table[i].nb == 0 )
       continue ;

     a.s.ligne = i ;
     glLoadName(a.n) ;

     u_multiplie_taille_point(6./4) ;
     glBegin(GL_POINTS) ;
     t_triplet(&o->lignes.table[i].table[0]) ;
     t_triplet(&o->lignes.table[i].table[o->lignes.table[i].nb-1]) ;
     glEnd() ;
     u_multiplie_taille_point(4./6) ;

     if ( ACTIF(o) )
       {
	 glBegin(GL_POINTS) ;
	 for(j=1;j<o->lignes.table[i].nb-1;j++)
	   t_triplet(&o->lignes.table[i].table[j]) ;
	 glEnd();     
       }

     if ( o->lignes.table[i].nb != 1 )
       {
	 glBegin(GL_LINE_STRIP) ;
	 for(j=0;j<o->lignes.table[i].nb;j++)
	   t_triplet(&o->lignes.table[i].table[j]) ;
	 glEnd();     
       }
   }

 /*
  * Affiche la ligne brisee active
  */

 if ( o->ligne_active >= 0 && ACTIF(o) && NB != 0 )
   {
     a.s.ligne = o->ligne_active ;
     a.s.type = Point ;

     u_multiplie_taille_point(6./4) ;

     a.s.n = 0 ;
     glLoadName(a.n) ;
     glBegin(GL_POINTS) ;
     t_triplet(&LC.table[0]) ;
     glEnd();

     a.s.n = NB-1 ;
     glLoadName(a.n) ;
     glBegin(GL_POINTS) ;
     t_triplet(&LC.table[NB-1]) ;
     glEnd() ;

     u_multiplie_taille_point(4./6) ;

     for(i=1;i<NB-1;i++)
       {
	 a.s.n = i ;
	 glLoadName(a.n) ;
	 glBegin(GL_POINTS) ;
	 t_triplet(&LC.table[i]) ;
	 glEnd();
       }
     
     a.s.type = Segment ;
     u_multiplie_epaisseur_ligne(2) ;
     for (i=0 ; i<NB-1 ; i++)
       {
	 a.s.n = i+1 ;
	 glLoadName(a.n) ;
	 glBegin(GL_LINE_STRIP) ;
	 if ( i+1==o->position_d_insertion )
	   glColor3f(1,1,1) ;
	 else
	   glColor3f(1,1,0) ;
	 t_triplet(&LC.table[i]) ;
	 if ( i+1==o->position_d_insertion )
	   glColor3f(1,0,0) ;
	 else
	   glColor3f(0,0,1) ;
	 t_triplet(&LC.table[i+1]) ;
	 glEnd();
       }


     if ( NB >= 2 )
       {
	 u_multiplie_epaisseur_ligne(2) ;

	 a.s.n = 0 ;
	 glLoadName(a.n) ;
	 tmp = u_soustrait_triplet(&LC.table[1], &LC.table[0]) ;
	 tmp = u_triplet_fois_flottant(&tmp, .5) ;
	 tmp = u_soustrait_triplet(&LC.table[0], &tmp) ;

	 glBegin(GL_LINE_STRIP) ;
	 if ( 0==o->position_d_insertion )
	   glColor3f(1,1,1) ;
	 else
	   glColor3f(1,1,0) ;
	 t_triplet(&tmp) ;
	 glColor3f(0,0,1) ;
	 t_triplet(&LC.table[0]) ;
	 glEnd();

	 a.s.n = NB ;
	 glLoadName(a.n) ;
	 tmp = u_soustrait_triplet(&LC.table[NB-1], &LC.table[NB-2]) ;
	 tmp = u_triplet_fois_flottant(&tmp, .5) ;
	 tmp = u_ajoute_triplet(&LC.table[NB-1], &tmp) ;

	 glBegin(GL_LINE_STRIP) ;
	 if ( NB==o->position_d_insertion )
	   glColor3f(1,1,1) ;
	 else
	   glColor3f(1,1,0) ;
	 t_triplet(&LC.table[NB-1]) ;
	 glColor3f(0,0,1) ;
	 t_triplet(&tmp) ;
	 glEnd();

       }
   }
 glPopName() ;
 glPopAttrib() ;
}


static void changement_lignes_dans_nuage(struct lignes_dans_nuage *o)
{
  Table_triplet tp ;
  Num l, ll ;
  int n ;
  Nuage *nuage ;
  Booleen point_selectionne ;

  /*
   * Verification des domaines de valeur
   */
  if ( o->ligne_active < -1 )
    o->ligne_active = -1 ;
  else
    if ( o->ligne_active >= o->lignes.nb )
      o->ligne_active = o->lignes.nb - 1 ;

  if ( o->ligne_active >= 0 )
    {
      if ( o->position_d_insertion < 0 )
	o->position_d_insertion = 0 ;
      else
	if ( o->position_d_insertion > NB )
	  o->position_d_insertion = NB ;
    }
  else
     o->position_d_insertion = 0 ;
  /*
   * Si on clique sur une ligne mais que l'on clique aussi
   * sur un point du nuage, on ne change pas de ligne,
   * on ajoute un point
   */
  point_selectionne = Faux ;
  POUR_LES_FILS(o, f,
		{
		  if ( c_recupere_type_sans_copie(f, &infos_L_sous_actif)[2] )
		    {
		      point_selectionne = Vrai ;
		      break ;
		    }
		}) ;

  if ( point_selectionne )
    {
      n = 0 ;
      POUR(long, i, o->sous_actifs,
	   {
	     l.n = i ;
	     if ( l.s.type!=Ligne || l.s.ligne==o->ligne_active )
	       {
		 n = 1 ;
		 break ;
	       }
	   }
	   );
      
    }
  else
    n = 1 ;

  /*
   * Dans l'objet "lignes_de_nuage"
   * Si l'utilisateur a cliqué sur une ligne : change ligne active
   *                           sur un point de ligne : on detruit le point
   */
  if ( n==1 && CHAMP_CHANGE(o,sous_actifs) )
    {
      /*
       * On clique sur une ligne brisée
       */
      if ( TRACE(o) )
	{
	  eprintf("ligne_courante=%d pt_insert=%d\n",
		  o->ligne_active, o->position_d_insertion) ;
	  POUR(long, i, o->sous_actifs,
	       {
		 l.n = i ;
		 eprintf("l.s.n=%d l.s.type=%d l.s.ligne=%d\n",
			 l.s.n, l.s.type, l.s.ligne) ;
	       }
	       );
	}
      /*
       * On désactive toutes les sous-activations des fils
       */
      POUR_LES_FILS(o, f,
		    {
		      c_sauve_type(f, &infos_L_sous_actif, "") ;
		    }
		    ) ;
      /*
       * Si l'on clique seulement sur une autre ligne
       * on change de ligne active
       */
      n = l_longueur(o->sous_actifs) ;
      l.n = (long)l_element(o->sous_actifs,0) ;
      if ( n==1 && l.s.type==Ligne && l.s.ligne!=o->ligne_active )
        {
          if ( TRACE(o) )
            eprintf("Changement de ligne active\n") ;
          o->ligne_active = l.s.ligne ;
          o->position_d_insertion = 0 ;
          l_detruit(&o->sous_actifs) ;
          return ;
        }
      if ( TRACE(o) )
	eprintf("On n'a pas cliqué sur une ligne: "
		"n=%d type=%s ligne=%d ligne_active=%d\n",
		n, l.s.type==Ligne ? "Ligne" : "PasLigne",
		l.s.ligne, o->ligne_active) ;
      /*
       * On extrait de la liste, seulement l'info sur la ligne courante
       * D'abord les points puis les segments.
       */
      n = 0 ;
      POUR(long, i, o->sous_actifs,
	   {
	     ll.n = i ;
	     if ( ll.s.type!=Point )
	       continue ;
	     l.n = i ;
	     n++ ;
	   }
	     ) ;

      if ( n==0 )
	POUR(long, i, o->sous_actifs,
	     {
	       ll.n = i ;
	       if ( ll.s.type!=Segment )
		 continue ;
	       l.n = i ;
	       n++ ;
	     }
	     ) ;
      /*
       * On refuse de travailler si l'on clique sur plus d'un objet ???
       */
      l_detruit(&o->sous_actifs) ;
      if ( n != 1 )
	{
	  if ( n!=0 )
	    c_sauve(FILS(u_racine(o),0),
		    "message",
		    "Cliquez plus précisemment") ;
	  return ;
	}
      /*
       * On interprete la décision de l'utilisateur
       */
      switch(l.s.type)
	{
	case Point:
	  if ( TRACE(o) )
	    eprintf("Destruction d'un sommet\n") ;
	  memmove(&LC.table[l.s.n], &LC.table[l.s.n+1],
		  (char*)&LC.table[NB] - (char*)&LC.table[l.s.n+1]) ;
	  o->position_d_insertion = l.s.n ;
	  NB-- ;
	  break ;
	case Segment:
	  if ( TRACE(o) )
	    eprintf("Changement de position d'insertion\n") ;
	  o->position_d_insertion = l.s.n ;
	  break ;
	}
      return ;
    }
  /*
   * Si un nuage fils change.
   * On insere les coordonnée du point cliqué
   */  
  if (point_selectionne)
    {
      if ( o->ligne_active == -1 )
	ajoute_ligne(o) ;
      if ( TRACE(o) )
	eprintf("Un fils a changé\n") ;
      POUR_LES_FILS(o, f,
       {
	 if ( strcmp(OBJET(f)->classe->nom,"nuage") )
	   continue ;
	 if ( TRACE(o) )
	   eprintf("Un nuage fils a changé\n") ;
	 nuage = (Nuage*)f ;
	 if ( nuage->sous_actifs )
	   {
	     if ( TRACE(o) )
	       eprintf("Un sous-actif du nuage fils a changé table=%x\n",
		       nuage->t.table) ;
	     tp.table = nuage->t.table ;
	     POUR(long, i, nuage->sous_actifs,
		  {
		    i -= TAG_DEPLACEMENT ;
		    if ( TRACE(o) )
		      eprintf("\tsous-actif=%i pos-insert=%d\n",
			      i, o->position_d_insertion);
		    REALLOUER(LC.table, ++NB) ;
		    memmove(&LC.table[o->position_d_insertion+1],
			    &LC.table[o->position_d_insertion],
			    (char*)&LC.table[NB]
			    - (char*)&LC.table[o->position_d_insertion+1]) ;
		    LC.table[o->position_d_insertion] = tp.table[i] ;
		    if ( o->position_d_insertion )
		      o->position_d_insertion++ ;
		  }
		    ) ;
	     l_detruit(&nuage->sous_actifs) ;
	     if ( TRACE(o) )
	       eprintf("Note champ change fin\n") ;
	   }
       }
		    );
    }
}

#undef NB
#undef LC

CLASSE(lignes_dans_nuage, struct lignes_dans_nuage,
       BULLE_D_AIDE
       (
	"Editeur de lignes brisées extraites d'un nuage de points.\n"
	"Pour que l'éditeur fonctionne :\n"
	"  - Il doit être paramétré par des nuages de points.\n"
	"  - Il doit être actif.\n"
	"Remarques :\n"
	"  - La ligne brisée active est en couleur.\n"
	"  - Le segment avec un dégradé Blanc->Rouge indique\n"
	"    l'endroit ou sera inséré le prochain point :\n"
	"    il le sera du coté du blanc.\n"
	"  - La ligne brisée active est prolongé des deux cotés\n"
	"    pour permettre d'ajouter des points en début ou en fin.\n"
	"Si l'on clique avec le bouton du milieu sur :\n"
	"  - Un point du nuage : il est inséré dans le segment Blanc->bleu.\n"
	"  - Un point de la ligne brisée active : le point est détruit.\n"
	"  - Un segment de la ligne active : change le segment Blanc->Bleu.\n"
	"  - Une autre ligne brisée : cela l'active.\n"
	)
       CHAMP(lignes,
	     L_table_table_point3 Edite Sauve TE_HAUTEUR(600)
	     BULLE_D_AIDE
	     (
	      "Chaque ligne contient les coordonnées d'un point.\n"
	      "Les () encadrent des lignes de points.\n"
	      )
	     )
       CHAMP_VIRTUEL(
	     NOM("detruit_ligne")
	     LABEL("detruit_ligne_active")
	     L_bouton(detruit_ligne)
	     Edite
	     BULLE_D_AIDE
	     (
	      "Cliquez ici pour détruire la ligne active (en couleur).\n"
	      "Pour changer de ligne active, cliquez sur la nouvelle ligne\n"
	      "avec le bouton du milieu."
	      )
	     )
       CHAMP_VIRTUEL(
	     NOM("ajoute_ligne")
	     LABEL("ajoute_ligne_vide")
	     L_bouton(ajoute_ligne_iw)
	     Edite
	     BULLE_D_AIDE
	     (
	      "Cliquez ici pour ajouter une nouvelle ligne (vide).\n"
	      "La nouvelle ligne sera active et vous pouvez donc\n"
	      "immédiatement commencer à lui ajouter des points\n"
	      "en cliquant avec le bouton du milieu sur des points\n"
	      "du nuage de point.\n"
	      )
	     )
       CHAMP(position_d_insertion, L_entier Edite Sauve
	     BULLE_D_AIDE
	     (
	      "Position d'insertion du prochain point\n"
	      "dans la ligne courante.\n"
	      "La valeur 0 indique que l'insertion est faite\n"
	      "avant le premier point de la ligne brisée.\n"
	     )
	     )
       CHAMP(ligne_active, L_entier Edite Sauve
	     BULLE_D_AIDE
	     (
	      "Numéro de la ligne active (en couleur)\n"
	      "dans laquelle les points vont être ajouté/détruit.\n"
	     )
	     )
       CHAMP_VIRTUEL(L_affiche_gl(affiche_lignes_dans_nuage)
		     AFFICHAGE(Different_si_actif|Different_suivant_qualite))
       CHAMP(sous_actifs, L_sous_actif)
       CHANGEMENT(changement_lignes_dans_nuage)
       PAQUET(NB_ACTIFS)
       MENU("Figures/1d/lignes dans nuage") ;
       EVENEMENT("Shft+LDN")
       )










/*
 ******************************************************************************
 ******************************************************************************
 * Extrait Ligne
 ******************************************************************************
 ******************************************************************************
 */


struct extrait_ligne
{
  Table_triplet ligne ;
  int position_d_insertion ; /* 0 : avant le premier point */
  Liste *sous_actifs ; 
} ;

static void affiche_extrait_ligne(struct extrait_ligne *o)
{
 int i ;
 Num a ;
 Triplet tmp ;

 if ( o->ligne.nb == 0 )
   return ;

 glPushAttrib(GL_LINE_BIT|GL_CURRENT_BIT|GL_POINT_BIT) ;

 if ( ACTIF(o) )
   {
     glPushName(0) ;
     a.s.type = Point ;
     /*
      * Des gros points pour le début et la fin
      */
     u_multiplie_taille_point(6) ;

     a.s.n = 0 ;
     glLoadName(a.n) ;
     glBegin(GL_POINTS) ;
     t_triplet(&o->ligne.table[0]) ;
     glEnd();

     if ( o->ligne.nb > 0 )
       {
	 a.s.n = o->ligne.nb-1 ;
	 glLoadName(a.n) ;
	 glBegin(GL_POINTS) ;
	 t_triplet(&o->ligne.table[o->ligne.nb-1]) ;
	 glEnd() ;
       }
     /*
      * Pour les autres points
      */
     u_multiplie_taille_point(4./6) ;

     for(i=1;i<o->ligne.nb-1;i++)
       {
	 a.s.n = i ;
	 glLoadName(a.n) ;
	 glBegin(GL_POINTS) ;
	 t_triplet(&o->ligne.table[i]) ;
	 glEnd();
       }

     if ( o->ligne.nb > 1 )
       {
	 /*
	  * Pour les segments de la ligne
	  */
	 a.s.type = Segment ;
	 u_multiplie_epaisseur_ligne(2) ;
	 for (i=0 ; i<o->ligne.nb-1 ; i++)
	   {
	     a.s.n = i+1 ;
	     glLoadName(a.n) ;
	     glBegin(GL_LINE_STRIP) ;
	     if ( i+1==o->position_d_insertion )
	       glColor3f(1,1,1) ;
	     else
	       glColor3f(1,1,0) ;
	     t_triplet(&o->ligne.table[i]) ;
	     if ( i+1==o->position_d_insertion )
	       glColor3f(1,0,0) ;
	     else
	       glColor3f(0,0,1) ;
	     t_triplet(&o->ligne.table[i+1]) ;
	     glEnd();
	   }
	 /*
	  * Pour les deux petites extensions de segment
	  * au début et à la fin.
	  */
	 u_multiplie_epaisseur_ligne(2) ;
	 
	 a.s.n = 0 ;
	 glLoadName(a.n) ;
	 tmp = u_soustrait_triplet(&o->ligne.table[1], &o->ligne.table[0]) ;
	 tmp = u_triplet_fois_flottant(&tmp, .5) ;
	 tmp = u_soustrait_triplet(&o->ligne.table[0], &tmp) ;
	 
	 glBegin(GL_LINE_STRIP) ;
	 if ( 0==o->position_d_insertion )
	   glColor3f(1,1,1) ;
	 else
	   glColor3f(1,1,0) ;
	 t_triplet(&tmp) ;
	 glColor3f(0,0,1) ;
	 t_triplet(&o->ligne.table[0]) ;
	 glEnd();
	 
	 a.s.n = o->ligne.nb ;
	 glLoadName(a.n) ;
	 tmp = u_soustrait_triplet(&o->ligne.table[o->ligne.nb-1],
				   &o->ligne.table[o->ligne.nb-2]) ;
	 tmp = u_triplet_fois_flottant(&tmp, .5) ;
	 tmp = u_ajoute_triplet(&o->ligne.table[o->ligne.nb-1], &tmp) ;
	 
	 glBegin(GL_LINE_STRIP) ;
	 if ( o->ligne.nb==o->position_d_insertion )
	   glColor3f(1,1,1) ;
	 else
	   glColor3f(1,1,0) ;
	 t_triplet(&o->ligne.table[o->ligne.nb-1]) ;
	 glColor3f(0,0,1) ;
	 t_triplet(&tmp) ;
	 glEnd();
       }
     glPopName() ;
   }
 else
   {
     /*
      * Affiche les gros points de début et fin
      */
     u_multiplie_taille_point(6) ;
     glBegin(GL_POINTS) ;
     t_triplet(&o->ligne.table[0]) ;
     if ( o->ligne.nb > 0 )
       t_triplet(&o->ligne.table[o->ligne.nb-1]) ;
     glEnd() ;
     /*
      * Affiche la ligne
      */
     if ( o->ligne.nb != 1 )
       {
	 glBegin(GL_LINE_STRIP) ;
	 for(i=0;i<o->ligne.nb;i++)
	   t_triplet(&o->ligne.table[i]) ;
	 glEnd();     
       }
   }
 glPopAttrib() ;
}


static void changement_extrait_ligne(struct extrait_ligne *o)
{
  Table_triplet tp ;
  Num l, ll ;
  int n ;
  Nuage *nuage ;
  Booleen point_selectionne ;

  /*
   * Verification des domaines de valeur
   */
  if ( o->position_d_insertion < 0 )
    o->position_d_insertion = 0 ;
  else
    if ( o->position_d_insertion > o->ligne.nb )
      o->position_d_insertion = o->ligne.nb ;
  /*
   * Éditable seulement si actif
   */
  if ( !ACTIF(o) )
    return ;
  /*
   * A-t-on cliqué sur un point d'un nuage
   */
  point_selectionne = Faux ;
  POUR_LES_FILS(o, f,
		{
		  if ( c_recupere_type_sans_copie(f, &infos_L_sous_actif)[2] )
		    {
		      point_selectionne = Vrai ;
		      break ;
		    }
		}) ;
  /*
   * On clique sur un point/ligne de la ligne
   * ===> on detruit le point ou change le segment actif
   */
  if ( CHAMP_CHANGE(o,sous_actifs) )
    {
      /*
       * On désactive toutes les sous-activations des fils
       */
      POUR_LES_FILS(o, f,
		    {
		      c_sauve_type(f, &infos_L_sous_actif, "") ;
		    }
		    ) ;
      /*
       * Compte le nombre de points activé dans l'objet
       * "l" contient le numéro du point.
       */
      n = 0 ;
      POUR(long, i, o->sous_actifs,
	   {
	     ll.n = i ;
	     if ( ll.s.type!=Point )
	       continue ;
	     l.n = i ;
	     n++ ;
	   }
	   ) ;
      /*
       * Si il n'y a pas de point activé, alors ocmpte le nombre
       * de lignes activées.
       * "l" contient le numéro de la ligne.
       */
      if ( n==0 )
	POUR(long, i, o->sous_actifs,
	     {
	       ll.n = i ;
	       if ( ll.s.type!=Segment )
		 continue ;
	       l.n = i ;
	       n++ ;
	     }
	     ) ;
      /*
       * On refuse de travailler si l'on clique sur plus d'un objet ???
       */
      l_detruit(&o->sous_actifs) ;
      if ( n != 1 )
	{
	  if ( n!=0 )
	    c_sauve(FILS(u_racine(o),0),
		    "message",
		    "Cliquez plus précisemment") ;
	  return ;
	}
      /*
       * On interprete la décision de l'utilisateur
       */
      switch(l.s.type)
	{
	case Point:
	  if ( TRACE(o) )
	    eprintf("Destruction d'un sommet\n") ;
	  memmove(&o->ligne.table[l.s.n], &o->ligne.table[l.s.n+1],
		  (char*)&o->ligne.table[o->ligne.nb]
		  - (char*)&o->ligne.table[l.s.n+1]) ;
	  o->position_d_insertion = l.s.n ;
	  o->ligne.nb-- ;
	  break ;
	case Segment:
	  if ( TRACE(o) )
	    eprintf("Changement de position d'insertion\n") ;
	  o->position_d_insertion = l.s.n ;
	  break ;
	}
      return ;
    }
  /*
   * Si un nuage fils change.
   * On insere les coordonnée du point cliqué
   */  
  if (point_selectionne)
    {
      if ( TRACE(o) )
	eprintf("Un fils a changé\n") ;
      POUR_LES_FILS(o, f,
       {
	 if ( strcmp(OBJET(f)->classe->nom,"nuage") )
	   continue ;
	 if ( TRACE(o) )
	   eprintf("Un nuage fils a changé\n") ;
	 nuage = (Nuage*)f ;
	 if ( nuage->sous_actifs )
	   {
	     if ( TRACE(o) )
	       eprintf("Un sous-actif du nuage fils a changé table=%x\n",
		       nuage->t.table) ;
	     tp.table = nuage->t.table ;
	     POUR(long, i, nuage->sous_actifs,
		  {
		    i -= TAG_DEPLACEMENT ;
		    if ( TRACE(o) )
		      eprintf("\tsous-actif=%i pos-insert=%d\n",
			      i, o->position_d_insertion);
		    REALLOUER(o->ligne.table, ++o->ligne.nb) ;
		    memmove(&o->ligne.table[o->position_d_insertion+1],
			    &o->ligne.table[o->position_d_insertion],
			    (char*)&o->ligne.table[o->ligne.nb]
			    - (char*)&o->ligne.table[o->position_d_insertion+1]) ;
		    o->ligne.table[o->position_d_insertion] = tp.table[i] ;
		    if ( o->position_d_insertion )
		      o->position_d_insertion++ ;
		  }
		    ) ;
	     l_detruit(&nuage->sous_actifs) ;
	     if ( TRACE(o) )
	       eprintf("Note champ change fin\n") ;
	   }
       }
		    );
    }
    }

CLASSE(extrait_ligne, struct extrait_ligne,
       BULLE_D_AIDE
       (
	"Editeur de lignes brisées extraites d'un nuage de points.\n"
	"Pour que l'éditeur fonctionne :\n"
	"  - Il doit être paramétré par des nuages de points.\n"
	"  - Il doit être actif.\n"
	"Remarques :\n"
	"  - Le segment avec un dégradé Blanc->Rouge indique\n"
	"    l'endroit ou sera inséré le prochain point :\n"
	"    il le sera du coté du blanc.\n"
	"  - La ligne brisée active est prolongé des deux cotés\n"
	"    pour permettre d'ajouter des points en début ou en fin.\n"
	"Si l'on clique avec le bouton du milieu sur :\n"
	"  - Un point du nuage : il est inséré dans le segment Blanc->bleu.\n"
	"  - Un point de la ligne brisée active : le point est détruit.\n"
	"  - Un segment de la ligne active : change le segment Blanc->Bleu.\n"
	)
       CHAMP(ligne,
	     L_table_point3 Edite Sauve TE_HAUTEUR(600)
	     BULLE_D_AIDE
	     (
	      "Chaque ligne contient les coordonnées d'un point.\n"
	      )
	     )
       CHAMP(position_d_insertion, L_entier Edite Sauve
	     BULLE_D_AIDE
	     (
	      "Position d'insertion du prochain point\n"
	      "dans la ligne courante.\n"
	      "La valeur 0 indique que l'insertion est faite\n"
	      "avant le premier point de la ligne brisée.\n"
	     )
	     )
       CHAMP_VIRTUEL(L_affiche_gl(affiche_extrait_ligne)
		     AFFICHAGE(Different_si_actif|Different_suivant_qualite))
       CHAMP(sous_actifs, L_sous_actif)
       CHANGEMENT(changement_extrait_ligne)
       PAQUET(NB_ACTIFS)
       MENU("Figures/1d/extrait ligne de nuage") ;
       EVENEMENT("Shft+EL")
       )





