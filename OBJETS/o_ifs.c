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
#include "u_hlsrvb.h"
#include "stl.h"

/*
 * Points communs aux 2 types de fractale
 */

/*
 * Ne pas changer l'ordre : correspond au menu
 */
typedef enum { Affiche_Point, Affiche_Segment, Affiche_Carre
	       , Affiche_Rectangle, Affiche_Fleche} Type_Affichage ;

typedef unsigned long Etat ; /* 1 bit par noeud de graphe */

struct ifs_commun
{
  /*
   * Donné par l'utilisateur
   */

  int     prof_recur ;

  Booleen seulement_les_actives ;
  Booleen affiche_ses_fils ;
  Booleen non_designable ;
  Booleen cherche_les_points_les_plus_eloignes ;  
  Booleen autorise_non_connexe ;
  Booleen fleche ;
  Booleen colorise ;
  Booleen affiche_carre ;
  Booleen segment_initial_fixe ;
  Booleen moyenne_fausse ;
  Booleen affichage_3d ;
  Booleen affiche_figure_initiale ;
  Booleen corrige_le_sens_des_fleches ;
  Booleen moyenne_hls ;
  Booleen couleur_du_graphe ;
  Booleen sphere_capped ;
  Booleen z_reste_vertical ;

  Triplet couleur_initiateur ;
  Triplet bas_gauche, haut_droit ; /* Précalculé pour le filaire */
  Triplet v, mv ;
  Triplet position[4] ;
  Flottant reduction_trait ;
  int epaisseur_trait ;
  Menu figure ;
  int nb_cote ;

  /*
   * Alloués et remplis par "recupere_transformation"
   */

  int nb_transfos ;
  Booleen *est_actif ;		/* Indice de la transformation (pas #fils) */
  Transfo *t ;
  Triplet **points ;
  Triplet *couleurs ;

  /*
   * Calculé par la fonction changement
   */
  Booleen ifs_filaire ;
  Type_Affichage type_affichage ;
  t_graphe_N_pcc_A_chaine g ;
  Etat *etats ;
} ;


Etat nouvel_etat(const struct ifs_commun *o, Etat etat, int transfo)
{
  int i, j, k ;
  Etat nouvel ;

  nouvel = 0 ;
  for(i=0;i<o->g.nb_noeuds;i++)
    if ( (1<<i) & etat )
      for(j=0;j<o->g.noeuds[i].nb_aretes;j++)
	for(k=0; o->g.noeuds[i].aretes[j].donnee[k]; k++)
	  if (  o->g.noeuds[i].aretes[j].donnee[k] == (transfo+'A') )
	    nouvel |= (1<<(o->g.noeuds[i].aretes[j].destination)) ;

  return(nouvel) ;
}

static Triplet calcul_couleur(const struct ifs_commun *o, int premiere
			      , const Triplet *couleur, const Triplet *courante
			      , int nb_recur, int etat)
{
  Triplet nouvelle ;

  if ( o->couleur_du_graphe )
    {
      if ( etat == 7 )
	etat = 0 ;
      nouvelle.x = ( etat & 1 ) ;
      nouvelle.y = ( etat & 2 ) / 2 ;
      nouvelle.z = ( etat & 4 ) / 4 ;
    }
  else
    {
      if ( o->moyenne_fausse )
	{
	  if ( premiere )
	    nouvelle = *couleur ;
	  else
	    nouvelle = u_milieu_2_triplet(couleur, courante) ;
	}
      else
	{
	  /*
	   * On ne doit pas faire d'exception pour le première couleur
	   * sinon on ne peut pas faire de dégradés.
	   */
	  nouvelle = u_barycentre_2_triplet(courante
					    , 1./(1<<(nb_recur+1))
					    , couleur
					    ) ;
	}
    }
  return(nouvelle) ;
}


/*
 * BEURK.
 * Suppose que le segment de droite contient 2 points successifs
 */

static int trouve_transfo(struct ifs_commun *o
			  , void **ob
			  , const Champ **c
			  )
{
  if ( o->ifs_filaire )
    return( c_trouve_type((const void**)ob, c, &infos_L_point) ) ;
  else
    return( c_trouve_type((const void**)ob, c, &infos_L_transfo) ) ;
}

static void recupere_transformation(struct ifs_commun *o)
{
  int i, j, k ;
  const Champ *c ;

  o->nb_transfos = 0 ;
  i = 0 ;
  ALLOUER(o->est_actif, l_longueur(OBJET(o)->fils)) ;

  j = 0 ;
  k = 0 ;
  POUR_LES_FILS(o, f,
  {
    if ( TRACE(o) )
      eprintf("%d %s\n", k++, OBJET(f)->classe->nom) ;
    if ( trouve_transfo(o, &f, &c) )
      {
	if ( TRACE(o) )
	  eprintf("Est transfo\n") ;
	if ( !o->seulement_les_actives || ACTIF(f) )
	  {
	    o->nb_transfos++ ;
	    o->est_actif[i] = Vrai ;
	  }
	else
	  {
	    o->est_actif[i] = Faux ;
	  }
	i++ ;
      }
    else
      if ( c_trouve_type((const void**)&f, &c, &infos_L_couleur) )
	j++ ;
  }
		) ;

  if ( j > i )
    eprintf("ifs_demo : trop de couleur / transfo (%d de plus)\n",j - i  ) ;

  ALLOUER(o->t       , o->nb_transfos) ;
  ALLOUER(o->couleurs, o->nb_transfos) ;
  ALLOUER(o->points  , o->nb_transfos) ;

  if ( TRACE(o) )
    eprintf("nb_transfo_total = %d, nb_transfo = %d\n", i, o->nb_transfos) ;

  i = 0 ;			/* Numéro de transfo */
  j = 0 ;			/* Numéro couleur de transfo active */
  k = 0 ;			/* Numéro couleur */
  POUR_LES_FILS(o, f,
  {
    if ( c_trouve_type((const void**)&f, &c, &infos_L_couleur) )
      {
	if ( TRACE(o) )
	  eprintf("couleur [%d] est_actif[%d]=%d\n", j, k, o->est_actif[k]) ;
	if ( o->est_actif[k++] )
	  {
	    if ( j < o->nb_transfos )
	      {
		if ( o->moyenne_hls )
		  {
		    CLR_RGB rgb ;
		    CLR_HLS hls ;
		    rgb.r = T(Triplet, f, c).x ;
		    rgb.g = T(Triplet, f, c).y ;
		    rgb.b = T(Triplet, f, c).z ;
		    hls = CLR_rgb_to_hls(&rgb) ;
		    o->couleurs[j].x = hls.h ;
		    o->couleurs[j].y = hls.l ;
		    o->couleurs[j].z = hls.s ;
		    j++ ;
		  }
		else
		  o->couleurs[j++] = T(Triplet, f, c) ;
	      }
	  }
	continue ;
      }
    if ( o->seulement_les_actives && !ACTIF(f) )
      continue ;
    if (  trouve_transfo(o, &f, &c) )
      {
	if ( i >= o->nb_transfos )
	  EXIT ;
	if ( o->ifs_filaire )
	  o->points[i++] = &T(Triplet, f, c) ;
	else
	  {
	    o->t[i++] = T(Transfo, f, c) ;
	  }
      }
  }
		) ;
  for(;j<o->nb_transfos;j++)
    u_annule_triplet(&o->couleurs[j]) ;

  ALLOUER(o->etats, (1<<o->g.nb_noeuds)*o->nb_transfos) ;
  for(i=0; i<(1<<o->g.nb_noeuds); i++)
    for(j=0; j<o->nb_transfos; j++)
      o->etats[ i*o->nb_transfos + j ] = nouvel_etat(o, i, j) ;
}



static void fleche(const Triplet *base, const Triplet *sommet
		   , float taille_relative)
{
  Triplet v ;			/* Vecteur de la flèche */
  Transfo t ;
  Triplet p, q ;
  int i ;
  const Triplet tp[] =
  {
    {0,0,1}, {.5,0,0}, {-.5,0,0}, {0,.5,0}, {0,-.5,0}
  } ;

  v = u_soustrait_triplet(sommet, base) ;
  v = u_triplet_fois_flottant(&v, 1-taille_relative) ;
  t.origine = u_ajoute_triplet(base, &v) ;
  t.v3 = u_soustrait_triplet(sommet, &t.origine) ;
  u_repere_de_vecteur(&t.v3,&t.v2,&t.v1) ;

  p = u_transforme(&t, &tp[0]) ; 

  for(i=1; i<5; i++)
    {
      t_triplet(&p) ;
      q = u_transforme(&t, &tp[i]) ;
      t_triplet(&q) ;
    }
}

static void fait_le_begin(const struct ifs_commun *o)
{
  switch(o->type_affichage)
    {
    case Affiche_Fleche:
    case Affiche_Segment:
      glBegin(GL_LINES) ;
      break ;
    case Affiche_Point:
      glBegin(GL_POINTS) ;
      break ;
    case Affiche_Carre:
    case Affiche_Rectangle:
      glBegin(GL_QUADS) ;
      break ;
    default:
      break ;
    }
}

/*
 * Cette version est 2*+ lente que "afficher"
 */

void afficher2(const struct ifs_commun *o, int prof, int nb_recur
	      , const Triplet *couleur, Booleen premiere
	      , Etat etat)
{
  int i ;
  Triplet nouvelle_couleur ;


  if ( etat == 0 )
    return ;

  for(i=0; i<o->nb_transfos; i++)
    {
      glPushMatrix() ;
      u_change_repere(&o->t[i]);

      nouvelle_couleur = calcul_couleur(o, premiere, &o->couleurs[i]
					, couleur, nb_recur, etat) ;

      if ( prof )
	{
	  if ( o->colorise || o->g.nb_noeuds )
	    {
	      if ( o->g.nb_noeuds )
		{
		  afficher2(o, prof-1, nb_recur+1
			   , &nouvelle_couleur, Faux
			   , o->etats[ o->nb_transfos*etat + i]
			   ) ;
		}
	      else
		{
		  afficher2(o, prof-1, nb_recur+1
			   , &nouvelle_couleur, Faux, 1) ;
		}
	    }
	  else
	    {
	      glCallList(OBJECT_ID(o) + prof - 1) ;
	    }	  
	}
      else
	{
	  if ( o->colorise )
	    {
	      if ( o->moyenne_hls )
		{
		  CLR_RGB rgb ;
		  CLR_HLS hls ;
		  hls.h = nouvelle_couleur.x ;
		  hls.l = nouvelle_couleur.y ;
		  hls.s = nouvelle_couleur.z ;
		  rgb = CLR_hls_to_rgb(&hls) ;
		  glColor3f(rgb.r, rgb.g, rgb.b) ;
		}
	      else
		glColor3f(nouvelle_couleur.x,nouvelle_couleur.y, nouvelle_couleur.z);
	    }

	  switch(o->type_affichage)
	    {
	    case Affiche_Fleche:
	      glBegin(GL_LINES) ;
	      fleche(&o->bas_gauche, &o->haut_droit, .1) ;
	      break ;
	    case Affiche_Segment:
	      glBegin(GL_LINES) ;
	      t_triplet(&o->haut_droit) ;
	      t_triplet(&o->bas_gauche) ;
	      break ;
	    case Affiche_Point:
	      glBegin(GL_POINTS) ;
	      t_triplet(&o->bas_gauche) ;
	      break ;
	    case Affiche_Carre:
	      glBegin(GL_QUADS) ;
	      t_triplet(&o->bas_gauche) ;
	      glVertex2f(o->haut_droit.x, o->bas_gauche.y) ;
	      t_triplet(&o->haut_droit) ;
	      glVertex2f(o->bas_gauche.x, o->haut_droit.y) ;
	      break ;	  
	    case Affiche_Rectangle:
	      glBegin(GL_QUADS) ;
	      glVertex2f(o->bas_gauche.x+o->v.x, o->bas_gauche.y+o->v.y) ;
	      glVertex2f(o->haut_droit.x+o->v.x, o->haut_droit.y+o->v.y) ;
	      glVertex2f(o->haut_droit.x+o->mv.x, o->haut_droit.y+o->mv.y) ;
	      glVertex2f(o->bas_gauche.x+o->mv.x, o->bas_gauche.y+o->mv.y) ;
	    }
	  glEnd() ;
	}
      glPopMatrix() ;
    }
}

static Booleen sortie_ne(const Interface_widget *iw)
{
  int i ;
  FILE *f ;
  struct ifs_commun *o ;
  Triplet x, y, z, vy, vz ;

  o = iw->objet ;

  f = fopen("pas_de_nom.ne", "w") ;

  fprintf(f,
	  "classe applique\n"
	  "nom 100\n"
	  "fils (101 102 %d)\n"
	  "\n"
	  , o->colorise ? 103 : 1
	  ) ;

  if ( o->colorise )
    fprintf(f,
	    "classe lrifs_compose\n"
	    "nom 103\n"
	    "fils (3 1 2)\n"
	    "type Colorié : géométrie du deuxième\n"
	    "\n"
	    ) ;
  z = u_soustrait_triplet(&o->haut_droit, &o->bas_gauche) ;
  u_repere_de_vecteur(&z, &x, &y) ;

  fprintf(f,
	  "classe variable\n"
	  "nom 3\n"
	  "nom_variable figure\n"
	  "fils (333333)\n"
	  "\n"
	  "classe applique\n"
	  "nom 333333\n"
	  ) ;

  if ( o->ifs_filaire )
    fprintf(f,
	    "fils (4 5 6 9 13 14)\n"
	    "\n"
	    "classe repere\n"
	    "nom 4\n"
	    "t_origine %g %g %g\n"
	    "t_v1 %g %g %g\n"
	    "t_v2 %g %g %g\n"
	    "t_v3 %g %g %g\n"
	    "\n"

	    "classe type_affichage\n"
	    "nom 5\n"
	    "affichage facette\n"
	    "\n"

	    "classe applique\n"
	    "nom 6\n"
	    "fils (7 8)\n"
	    "\n"
	    "classe homothetie\n"
	    "nom 7\n"
	    "angle 0.1\n"
	    "\n"
	    "classe sphere\n"
	    "nom 8\n"
	    "\n"

	    "classe applique\n"
	    "nom 9\n"
	    "fils (10 11 12)\n"
	    "\n"
	    "classe translation\n"
	    "nom 10\n"
	    "t_origine 0 0 1\n"
	    "\n"
	    "classe homothetie\n"
	    "nom 11\n"
	    "angle 0.1\n"
	    "\n"
	    "classe sphere\n"
	    "nom 12\n"
	    "\n"

	    "classe dilatation\n"
	    "nom 13\n"
	    "vecteur 0.1 0.1 1\n"
	    "\n"
	    "classe cylindre\n"
	    "nom 14\n"
	    "\n"
	    , o->bas_gauche.x, o->bas_gauche.y, o->bas_gauche.z
	    , x.x, x.y, x.z
	    , y.x, y.y, y.z
	    , z.x, z.y, z.z
	    ) ;
  else
    fprintf(f,
	    "fils (4 5 6)\n"
	    "\n"
	    "classe repere\n"
	    "nom 4\n"
	    "t_origine -1 -1 -1\n"
	    "t_v1 2 0 0\n"
	    "t_v2 0 2 0\n"
	    "t_v3 0 0 2\n"
	    "\n"
	    "classe dilatation\n"
	    "nom 5\n"
	    "vecteur 1 1 1\n"
	    "\n"
	    "classe type_affichage\n"
	    "nom 55\n"
	    "affichage facette\n"
	    "\n"
	    "classe cube\n"
	    "nom 6\n"
	    "\n"
	    ) ;

  fprintf(f,
	  "classe vue\n"
	  "geometrie 500 500 500 500\n"
	  "edite 1\n"
	  "\n"
	  "classe lampe\n"
	  "nom 101\n"
	  "\n"
	  "classe choix_stl_offset\n"
	  "nom 102\n"
	  "offset 0\n"
	  "\n"
	  ) ;
  
  fprintf(f,
	  "classe lrifs\n"
	  "nom 1\n"
	  "fils (3"
	  ) ;
  for(i=0; i<o->nb_transfos; i++)
    fprintf(f, " %d", 1000+i) ;
  fprintf(f, ")\n\n") ;
  
  if ( o->colorise )
    {
      fprintf(f,
	      "classe lrifs\n"
	      "nom 2\n"
	      "fils (3"
	      ) ;
      for(i=0; i<o->nb_transfos; i++)
	fprintf(f, " %d", 2000+i) ;
      fprintf(f, ")\n\n") ;
    }

  for(i=0; i<o->nb_transfos; i++)
    {
      if ( o->z_reste_vertical || !o->ifs_filaire )
	{
	  vy = o->t[i].v2 ;
	  vz = u_pv_triplet(&o->t[i].v1, &o->t[i].v2) ;
	  vz.x = 0 ;
	  vz.y = 0 ;
	  vz.z = (vz.z > 0 ? 1 : -1)
	    * ( o->ifs_filaire ? u_norme_triplet(&o->t[i].v2) : 1 ) ;
	}
      else
	{
	  vz = o->t[i].v3 ;
	  vy = o->t[i].v2 ;
	}

      fprintf(f,
	      "classe repere\n"
	      "nom %d\n"
	      "t_origine %g %g %g\n"
	      "t_v1 %g %g %g\n"
	      "t_v2 %g %g %g\n"
	      "t_v3 %g %g %g\n"
	      "\n"
	      , 1000+i
	      , o->t[i].origine.x, o->t[i].origine.y, o->t[i].origine.z+i/100.
	      , o->t[i].v1.x, o->t[i].v1.y, o->t[i].v1.z
	      , vy.x, vy.y, vy.z
	      , vz.x, vz.y, vz.z
	      ) ;
      if ( o->colorise )
	fprintf(f,
		"classe repere\n"
		"nom %d\n"
		"t_origine %g %g %g\n"
		"t_v1 0.5 0 0\n"
		"t_v2 0 0.5 0\n"
		"t_v3 0 0 0.5\n"
		"\n"
		, 2000+i
		, o->couleurs[i].x, o->couleurs[i].y, o->couleurs[i].z
		) ;
    }

  fprintf(f,
	  "classe stl\n"
	  "nom_fichier MODELES/%s.stl\n"
	  , c_recupere_sans_copie(u_modele(o), "nom_modele")
	  ) ;

  fclose(f) ;

  return(Faux) ;
}


void afficher(const struct ifs_commun *o, int prof, int nb_recur
	      , const Transfo *t, const Triplet *couleur, Booleen premiere
	      , Etat etat)
{
  int i ;
  Transfo tmp ;
  Triplet p, q, pp, nouvelle_couleur ;


  if ( etat == 0 )
    return ;

  for(i=0; i<o->nb_transfos; i++)
    {
      tmp = u_produit_transfo(t, &o->t[i]) ;

      nouvelle_couleur = calcul_couleur(o, premiere, &o->couleurs[i]
					, couleur, nb_recur, etat) ;

      if ( prof )
	{
	  if ( o->colorise || o->g.nb_noeuds )
	    {
	      if ( o->g.nb_noeuds )
		{
		  afficher(o, prof-1, nb_recur+1, &tmp
			   , &nouvelle_couleur, Faux
			   , o->etats[ o->nb_transfos*etat + i]
			   ) ;
		}
	      else
		{
		  afficher(o, prof-1, nb_recur+1, &tmp
			   , &nouvelle_couleur, Faux, 1) ;
		}
	    }
	  else
	    {
	      glPushMatrix() ;
	      u_change_repere(&o->t[i]) ;
	      glCallList(OBJECT_ID(o) + prof - 1) ;
	      glPopMatrix() ;
	    }	  
	}
      else
	{
	  if ( o->colorise )
	    {
	      if ( o->moyenne_hls )
		{
		  CLR_RGB rgb ;
		  CLR_HLS hls ;
		  hls.h = nouvelle_couleur.x ;
		  hls.l = nouvelle_couleur.y ;
		  hls.s = nouvelle_couleur.z ;
		  rgb = CLR_hls_to_rgb(&hls) ;
		  glColor3f(rgb.r, rgb.g, rgb.b) ;
		}
	      else
		glColor3f(nouvelle_couleur.x,nouvelle_couleur.y, nouvelle_couleur.z);
	    }

	  p = u_transforme(&tmp, &o->bas_gauche) ;
	  q = u_transforme(&tmp, &o->haut_droit) ;
	  
	  switch(o->type_affichage)
	    {
	    case Affiche_Fleche:
	      fleche(&p, &q, .1) ;
	      break ;
	    case Affiche_Segment:
	      t_triplet(&q) ;
	    case Affiche_Point:
	      t_triplet(&p) ;
	      break ;
	    case Affiche_Carre:
	      t_triplet(&p) ;
	      
	      pp.x = o->haut_droit.x ;
	      pp.y = o->bas_gauche.y ;
	      pp.z = 0 ;
	      pp = u_transforme(&tmp, &pp) ;
	      t_triplet(&pp) ;
	      
	      t_triplet(&q) ;
	      
	      pp.x = o->bas_gauche.x ;
	      pp.y = o->haut_droit.y ;
	      pp.z = 0 ;
	      pp = u_transforme(&tmp, &pp) ;
	      t_triplet(&pp) ;
	      break ;	  
	    case Affiche_Rectangle:
	      pp = u_ajoute_triplet(&o->bas_gauche, &o->v) ;
	      pp = u_transforme(&tmp, &pp) ;
	      t_triplet(&pp) ;

	      pp = u_ajoute_triplet(&o->haut_droit, &o->v) ;
	      pp = u_transforme(&tmp, &pp) ;
	      t_triplet(&pp) ;

	      pp = u_ajoute_triplet(&o->haut_droit, &o->mv) ;
	      pp = u_transforme(&tmp, &pp) ;
	      t_triplet(&pp) ;

	      pp = u_ajoute_triplet(&o->bas_gauche, &o->mv) ;
	      pp = u_transforme(&tmp, &pp) ;
	      t_triplet(&pp) ;

	    }
	}
    }
}

static void affiche_les_fils(const struct ifs_commun *o)
{
  const Champ *c ;
  int i, j, k ;
  Transfo sauve ;
  Triplet p1, p2, p ;
  Flottant s ;
  char lettre[2] ;

  if ( o->affiche_ses_fils )
    {      
      sauve = globale_etat_courant.transfo ;

      glPushAttrib(GL_CURRENT_BIT) ;
      glColor3f(0,0,0) ;

      i = 0 ;
      j = 0 ;
      k = 0 ;
      POUR_LES_FILS(o, f,
      {
	if ( c_trouve_type((const void**)&f, &c, &infos_L_couleur))
	  {
	    if ( o->couleur_du_graphe )
	      continue ;
	    if ( j<o->nb_transfos && o->est_actif[i]  && o->colorise )
	      {
		glPushMatrix() ;
		if ( o->ifs_filaire )
		  {
		    glLoadIdentity() ;
		    p1 = u_transforme(&o->t[j], &o->bas_gauche) ;
		    p2 = u_transforme(&o->t[j], &o->haut_droit) ;
		    p = u_milieu_2_triplet(&p1,&p2) ;
		    glTranslatef(p.x, p.y, p.z) ;
		    s = u_distance_triplet(&p1,&p2) ;
		    s /= 3 ;
		    glScalef(s,s,1) ;
		  }
		else
		  {
		    u_change_repere(&o->t[j]) ;
		    glTranslatef(.6, .6 ,.01) ;
		    glScalef(.5, .5, 1) ;
		  }

		u_recupere_matrice_gl_dans_etat_courant() ;		
		u_affiche_objet(f, "") ;
		glPopMatrix() ;
		j++ ;
		}
	    i++ ;
	  }
	else
	  {
	    if ( c_trouve_type((const void**)&f, &c, &infos_L_transfo))
	      {
		u_affiche_objet(f, "") ;
		if ( o->g.nb_noeuds )
		  {
		    glPushMatrix() ;
		    u_change_repere(&o->t[k]) ;
		    glTranslatef(-.2,-.2,0) ;
		    glRasterPos3f(0.,0.,0.) ;
		    lettre[0] = k + 'A' ;
		    lettre[1] = '\0' ;
		    t_chaine( lettre ) ;
		    glPopMatrix() ;
		  }
		k++ ;
	      }
	    else
	      {
		u_affiche_objet(f, "") ;
	      }
	  }
      }
		    ) ; 
      glPopAttrib() ;
      globale_etat_courant.transfo = sauve ;
    }
}

static void affiche_figure_initiale(struct ifs_commun *o)
{
  if ( !o->affiche_figure_initiale )
    return ;

  glPushAttrib(GL_LINE_BIT|GL_CURRENT_BIT) ;
  if ( 1 || !o->colorise )
    glColor3f(o->couleur_initiateur.x
	      , o->couleur_initiateur.y
	      , o->couleur_initiateur.z
	      ) ;
  switch(o->type_affichage)
    {
    case Affiche_Segment:
    case Affiche_Rectangle:
      if ( o->nb_transfos )
	{
	  glPushMatrix() ;
	  u_multiplie_epaisseur_ligne(7) ;
	  glTranslatef(0,0,-0.1) ;
	  glBegin(GL_LINES) ;
	  t_triplet(&o->bas_gauche) ;
	  t_triplet(&o->haut_droit) ;
	  if ( o->fleche )
	    fleche(&o->bas_gauche,&o->haut_droit, .1);
	  glEnd() ;
	  glPopMatrix() ;
	}
      break ;
    case Affiche_Carre:
      glBegin(GL_QUADS) ;
      glVertex2f(-1,-1) ;
      glVertex2f( 1,-1) ;
      glVertex2f( 1, 1) ;
      glVertex2f(-1, 1) ;
      glEnd() ;
      break ;
    default:
      break ;
    }
  glPopAttrib() ;
}


static void affiche_ifs(struct ifs_commun *o, const Champ *ch, const char *v)
{
  int i, j ;
  Transfo identite ;
  Triplet noir ;
  GLfloat cc[4] ;
  Type_Affichage sauve_ta ;


  o->prof_recur += atol(v) - 5 ;



  if ( TRACE(o) )
    eprintf("Affiche les points de l'ifs nb_transfo=%d\n", o->nb_transfos) ;

  if ( o->non_designable )
    glLoadName(Faux) ;
  if ( !o->affichage_3d )
    {
      glDisable(GL_DEPTH_TEST) ;
      glDepthFunc(GL_ALWAYS) ;
      glPushAttrib(GL_LIGHTING_BIT) ;
      glShadeModel(GL_FLAT) ;
    }

  affiche_figure_initiale(o) ;
  u_identite_transfo(&identite) ;
  if ( o->colorise )
    {
      glGetFloatv(GL_CURRENT_COLOR, cc) ;
      noir.x = cc[0] ;
      noir.y = cc[1] ;
      noir.z = cc[2] ;
    }
  glPushAttrib(GL_CURRENT_BIT|GL_LINE_BIT) ;
  u_multiplie_epaisseur_ligne(o->epaisseur_trait) ;
  if ( o->prof_recur > 4 )
    {
      glPushMatrix() ;
      glTranslatef(o->position[0].x, o->position[0].y, o->position[0].z) ;
      fait_le_begin(o) ;
      if ( o->g.nb_noeuds )
	{
	  if ( 1 )
	    afficher(o, o->prof_recur, 0, &identite, &noir, Vrai,(1<<o->g.nb_noeuds)-1) ;
	  else
	    afficher2(o, o->prof_recur, 0, &noir, Vrai,(1<<o->g.nb_noeuds)-1) ;
	}
      else
	{
	  afficher(o, o->prof_recur, 0, &identite, &noir, Vrai,1) ;
	}
      glEnd() ;
      glPopMatrix() ;
    }
  else
  for(i=0;i<o->prof_recur;i++)
    {
      if (o->position[i].x!=0 || o->position[i].y!=0 || o->position[i].z!=0 )
	{
	  glPushMatrix() ;
	  glTranslatef(o->position[i].x, o->position[i].y, o->position[i].z) ;
	  if ( o->colorise || o->g.nb_noeuds )
	    {
	      fait_le_begin(o) ;
	      if ( o->g.nb_noeuds )
		{
		  for(j=0;j<o->g.nb_noeuds;j++)
		    afficher(o, i, 0, &identite, &noir, Vrai,j) ;
		}
	      else
		{
		  afficher(o, i, 0, &identite, &noir, Vrai,1) ;
		}
	      glEnd() ;
	    }
	  else
	    {
	      glNewList(OBJECT_ID(o) + i, GL_COMPILE_AND_EXECUTE) ;
	      if ( i == 0 )
		fait_le_begin(o) ;
	      afficher(o, i, 0, &identite, &noir, Vrai, 1) ;
	      if ( i == 0 )
		glEnd() ;
	      glEndList() ;
	    }
	  if ( i==0 && o->fleche
	       && (o->type_affichage == Affiche_Segment
		   /*  || o->type_affichage == Affiche_Rectangle */ )
	       )
	    {
	      sauve_ta = o->type_affichage ;
	      o->type_affichage = Affiche_Fleche ;
	      fait_le_begin(o) ;
	      afficher(o, 0, 0, &identite, &noir, Vrai, 1) ;
	      glEnd() ;
	      o->type_affichage = sauve_ta ;
	    }
	  glPopMatrix() ;
	}
    }
  glPopAttrib() ;

  o->prof_recur -= atol(v) - 5 ;


  affiche_les_fils(o) ;
  if ( !o->affichage_3d )
    {
      glEnable(GL_DEPTH_TEST) ;
      glDepthFunc(GL_LESS) ;
      glPopAttrib() ;
    }

}

/*
 *
 */

void afficher_stl(FILE *f, const struct ifs_commun *o, int prof, int nb_recur
	      , const Transfo *t, Etat etat)
{
  int i ;
  Transfo tmp ;
  Triplet p, q, pp ;

  if ( etat == 0 )
    return ;

  for(i=0; i<o->nb_transfos; i++)
    {
      tmp = u_produit_transfo(t, &o->t[i]) ;

      if ( prof )
	{
	  if ( o->g.nb_noeuds )
	    {
	      afficher_stl(f, o, prof-1, nb_recur+1, &tmp
			   , o->etats[ o->nb_transfos*etat + i]
			   ) ;
	    }
	  else
	    {
	      afficher_stl(f, o, prof-1, nb_recur+1, &tmp, 1) ;
	    }
	}
      else
	{
	  p = u_transforme(&tmp, &o->bas_gauche) ;
	  q = u_transforme(&tmp, &o->haut_droit) ;
	  
	  switch(o->type_affichage)
	    {
	    case Affiche_Fleche:
	    case Affiche_Segment:
	      if ( o->sphere_capped )
		t_sphere_capped_cylindre_stl(f, &p, &q, globale_etat_courant.stl_offset, o->nb_cote) ;
	      else
		t_capped_cylindre_stl(f, &p, &q, globale_etat_courant.stl_offset, o->nb_cote) ;
	      break ;
	    case Affiche_Carre:
	      pp.x = o->haut_droit.x ;
	      pp.y = o->bas_gauche.y ;
	      pp.z = 0 ;
	      pp = u_transforme(&tmp, &pp) ;
	      t_triangle_stl_epais(f, &p, &pp, &q, globale_etat_courant.stl_offset) ;
	      
	      pp.x = o->bas_gauche.x ;
	      pp.y = o->haut_droit.y ;
	      pp.z = 0 ;
	      pp = u_transforme(&tmp, &pp) ;
	      t_triangle_stl_epais(f, &p, &q, &pp, globale_etat_courant.stl_offset) ;
	      break ;	  
	    case Affiche_Point:
	    case Affiche_Rectangle:
	      break ;
	    }
	}
    }
}

static void affiche_stl(struct ifs_commun *o, const Champ *ch, const char *v)
{
  int i, j ;
  Transfo identite ;

  if ( TRACE(o) )
    eprintf("Sortie STL d'un IFS\n") ;

  for(i=0;i<o->prof_recur;i++)
    {
      if (o->position[i].x!=0 || o->position[i].y!=0 || o->position[i].z!=0 )
	{
	  identite = globale_etat_courant.transfo ;
	  identite.origine = u_ajoute_triplet(&identite.origine
					      , &o->position[i]
					      ) ;
	  if ( o->g.nb_noeuds )
	    {
	      for(j=0;j<o->g.nb_noeuds;j++)
		afficher_stl((FILE*)atol(v), o, i, 0, &identite, j) ;
	    }
	  else
	    {
	      afficher_stl((FILE*)atol(v), o, i, 0, &identite, 1) ;
	    }
	}
    }
}

static void destruction_ifs(struct ifs_commun *o)
{
  free(o->t) ;
  free(o->couleurs) ;
  free(o->est_actif) ;
  free(o->points) ;
  free(o->etats) ;
}


CLASSE(demo_ifs_commun, struct ifs_commun,
       CHAMP(prof_recur, L_nombre_positif P_entier Edite Sauve DEFAUT("3"))
       CHAMP(seulement_les_actives, L_booleen Edite Sauve DEFAUT("0"))
       CHAMP(affiche_ses_fils, L_booleen Edite Sauve DEFAUT("0"))
       CHAMP(non_designable, L_booleen Edite Sauve DEFAUT("0"))
       CHAMP(position[0], L_point P_triplet Edite Sauve
	     DEFAUT("0 0 0")
	     BULLE_D_AIDE("Vecteur translatant les étapes 1 de l'IFS.\n")
	     )
       CHAMP(position[1], L_point P_triplet Edite Sauve
	     DEFAUT("0 0 0")
	     BULLE_D_AIDE("Vecteur translatant les étapes 2 de l'IFS.\n")
	     )
       CHAMP(position[2], L_point P_triplet Edite Sauve
	     DEFAUT("0 0 0")
	     BULLE_D_AIDE("Vecteur translatant les étapes 3 de l'IFS.\n")
	     )
       CHAMP(position[3], L_point P_triplet Edite Sauve
	     DEFAUT("0 0 0")
	     BULLE_D_AIDE("Vecteur translatant les étapes 4 de l'IFS.\n")
	     )
       DEBUT_BOITE(HORIZONTALE)
       CHAMP(colorise, L_booleen Edite Sauve DEFAUT("0"))
       CHAMP(moyenne_hls, L_booleen Edite Sauve DEFAUT("0"))
       FIN_BOITE
       DEBUT_BOITE(HORIZONTALE)
       CHAMP(moyenne_fausse, L_booleen Edite Sauve DEFAUT("0"))
       CHAMP(couleur_du_graphe, L_booleen Edite Sauve DEFAUT("0"))
       FIN_BOITE
       CHAMP(couleur_initiateur, L_couleur P_triplet Edite Sauve
	     DEFAUT("0.5 0.5 0.5"))

       CHAMP(affichage_3d, L_booleen Edite Sauve DEFAUT("0")
	     BULLE_D_AIDE("Si actif on élimine les faces cachées\n"
			  "Si inactif, le z-buffer est inactivé\n"
			  )
	     )
       CHAMP(affiche_figure_initiale, L_booleen Edite Sauve DEFAUT("1"))
       CHAMP(corrige_le_sens_des_fleches, L_booleen Edite Sauve DEFAUT("1"))
       CHAMP(epaisseur_trait, L_nombre P_entier Edite Sauve DEFAUT("1"))
       CHAMP(figure,
	     L_option_menu
	     TE_TEXTE(
		      "Défaut\n"
		      "Point\n"
		      "Ligne\n"
		      "Carré\n"
		      "Rectangle\n"
		      )
	     DEFAUT("Défaut")
	     Sauve Edite
	     BULLE_D_AIDE
	     ("Défaut    : Cela dépend du type d'IFS\n" 
	      "Point     : Le point 0,0,0\n"
	      "Ligne     : segment 0,0,0 à 1,0,0\n"
	      "Carré     : Sommets opposés -1,-1,0  1,1,0\n"
	      "Rectangle : En fait, c'est une ligne épaisse\n"
	      )
	     )
       CHAMP(nb_cote, L_nombre P_entier Edite Sauve DEFAUT("16")
	     BULLE_D_AIDE("C'est le nombre de faces créées lors de\n"
			  "la facettisation du cylindre filaire\n"
			  "lors de la sortie STL.\n"
			  "\n"
			  "Si vous mettez 4 les cylindres\n"
			  "seront ``carrés''\n"
			  "\n"
			  "Le rayon du cylindre est réglé par\n"
			  "l'offset STL.\n"
			  )
	     )
       CHAMP(g, L_graphe_de_points P_graphe_N_pcc_A_chaine
	     Edite Sauve Extrait DEFAUT("()"))
       CHAMP(z_reste_vertical, L_booleen Edite Sauve DEFAUT("1")
	     BULLE_D_AIDE("Si c'est vrai alors quand on passe\n"
			  "en 3D. les objets sont cisaillés\n"
			  )
	     )
       CHAMP(reduction_trait, L_flottant Edite Sauve DEFAUT("0")
	     BULLE_D_AIDE("Ceci n'est utile que pour la sortie NE\n"
			  "Cela indique la réduction de l'épaisseur\n"
			  "des traits à chaque itération.\n"
			  "Si c'est 1 les traits ont tous la même épaisseur.\n"
			  "Si c'est 0.5 : réduction de 2 à chaque étape.\n"
			  "Si c'est 0 alors l'épaisseur est proportionnelle\n"
			  "à la longueur du trait.\n"
			  )
	     )
       CHAMP(sphere_capped, L_booleen Edite Sauve DEFAUT("0")
	     BULLE_D_AIDE("Si c'est vrai on met une sphere au 2 bouts\n"
			  "des cylindres pour la sortie STL.\n"
			  "ATTENTION LES FICHIERS SONT MONSTRUEUX\n"
			  "\n"
			  "Dans le cas contraire, les cylindres sont\n"
			  "allongé de leur rayon pour qu'ils se\n"
			  "tiennent entre eux.\n"
			  )
	     )
       CHAMP_VIRTUEL(L_affiche_gl(affiche_ifs) AFFICHAGE(Toujours_different))
       CHAMP_VIRTUEL(L_affiche_stl(affiche_stl))
       CHAMP_VIRTUEL(LABEL("Génère fichier pas_de_nom.ne") L_bouton(sortie_ne))
       PAQUET(NB_ACTIFS)
       MENU("Figures/?d/Ifs")
       EVENEMENT("IFS")
       DESTRUCTION(destruction_ifs)
       )

/*
 *
 *
 *
 */

static void calcul_v(struct ifs_commun *o)
{
  Triplet axe ;

  o->v = u_soustrait_triplet(&o->bas_gauche, &o->haut_droit) ;
  o->v = u_normalise_triplet(&o->v) ;
  o->v = u_triplet_fois_flottant(&o->v, o->epaisseur_trait/10.) ;

  u_annule_triplet(&axe) ;
  axe.z = 1 ;
  o->mv = u_rotation_triplet(&o->v, &axe, M_PI/2) ;

  o->v = u_triplet_fois_flottant(&o->mv, -1) ;
}

static void met_a_jour_bg_hd(struct ifs_commun *o)
{
  switch( o->type_affichage )
    {
    case Affiche_Carre :
      o->bas_gauche = char_vers_triplet("-1 -1 0") ;
      o->haut_droit = char_vers_triplet("1 1 0") ;
      break ;
    default:
      o->bas_gauche = char_vers_triplet("0 0 0") ;
      o->haut_droit = char_vers_triplet("1 0 0") ;
      break ;
    }
  calcul_v(o) ;
}

static void changement_ifs(struct ifs_commun *o)
{
  if ( TRACE(o) )
    eprintf("Calcul des points de l'ifs\n") ;

  if ( o->prof_recur > 10 )  /* Limitation profondeur recursion */
    o->prof_recur = 10 ;

  o->ifs_filaire = Faux ;

  destruction_ifs(o) ;

  recupere_transformation(o) ;

  if ( TRACE(o) )
    {
      int i ;
      eprintf("Transformations :\n") ;
      for(i=0;i<o->nb_transfos;i++)
	eprintf("%s\n\n", codec_recupere_texte(&codec_transfo, &o->t[i])) ;
    }

  if ( o->figure.index == 0 )
    {
      if ( o->affiche_carre )
	o->type_affichage = Affiche_Carre ;
      else
	o->type_affichage = Affiche_Point ;
    }
  else
    o->type_affichage = o->figure.index - 1 ;

  met_a_jour_bg_hd(o) ;
}


CLASSE(demo_ifs, struct ifs_commun,
       SUR_CLASSE("demo_ifs_commun")
       CHAMP(affiche_carre, L_booleen Edite Sauve DEFAUT("0"))
       CHANGEMENT(changement_ifs)
       CHAMP_VIRTUEL(Obligatoire TYPE_DANS_FILS(L_transfo))
       )
/*
 *
 *
 *
 */


struct segment
     {
       Triplet *debut, *fin ;
     } ;

void transfo_de_segment(const Triplet *debut, const Triplet *fin
			, Transfo *t, Booleen z_reste_vertical)
{
  t->origine = *debut ;
  t->v1 = u_soustrait_triplet(fin, debut) ;
  if ( z_reste_vertical )
    {
      u_annule_triplet(&t->v3) ;
      t->v3.z = 1 ;
      t->v2 = u_rotation_triplet(&t->v1, &t->v3, M_PI/2) ;
    }
  else
    {
      u_repere_de_vecteur(&t->v1, &t->v2, &t->v3) ;
      if ( 0 )
	eprintf("repere de vecteur normes : %g %g %g\n"
		, u_norme_triplet(&t->v1)
		, u_norme_triplet(&t->v2)
		, u_norme_triplet(&t->v3)
		) ;
    }
}

void calcul_transfo(const Triplet *debut, const Triplet *fin,
		    struct segment *s, Transfo *t, Booleen z_reste_vertical
		    , Flottant reduction_trait)
{
  Transfo t1, t2, reduc_trait ;

  transfo_de_segment(debut, fin, &t1, z_reste_vertical) ;
  t1 = inversion_transfo(&t1) ;
  transfo_de_segment(s->debut, s->fin, &t2, z_reste_vertical) ;

  if ( reduction_trait != 0 )
    {
      u_identite_transfo(&reduc_trait) ;
      reduction_trait /= u_norme_triplet(&t2.v1) ;
      reduc_trait.v2.y = reduction_trait ;
      reduc_trait.v3.z = reduction_trait ;
      t1 = u_produit_transfo(&reduc_trait, &t1) ;
    }

  *t = u_produit_transfo(&t2, &t1) ;
}


#define EPS .001

void met_a_jour_distance_max(Triplet *p, Triplet *q,
			     Triplet **debut, Triplet **fin
			     , Flottant *distance_max)
{
  Flottant d ;

  d = u_distance_triplet(p, q) ;
  if ( d > *distance_max )
    {
      if ( p->x < q->x || ( p->x==q->x && p->y < p->y ) )
	{
	  *debut = p ;
	  *fin = q ;
	}
      else
	{
	  *debut = q ;
	  *fin = p ;
	}
      *distance_max = d ;
    }
}

int prend_un_segment(struct segment *s, int nb
		     , struct segment *un_arbre
		     , int *nombre_de_segments)
{
  int i ;

  for(i=0; i<nb; i++)
    {
      if ( s[i].debut == NULL )
	continue ;
      un_arbre[*nombre_de_segments] = s[i] ;
      (*nombre_de_segments)++ ;
      s[i].debut = NULL ;
      s[i].fin = NULL ;
      return(1) ;
    }
  return(0) ;
}


static int affiche_un_ifs_filaire(struct ifs_commun *o
				  , struct segment *s, int nb)
{
  struct segment *un_arbre ;
  Triplet *debut, *fin, *tmp ;
  int i, j, k ;
  Flottant distance_max ;
  Booleen segment_connexe_ajoute ;

  ALLOUER(un_arbre, nb) ;
  o->nb_transfos = 0 ;
  if ( prend_un_segment(s, nb, un_arbre, &o->nb_transfos) )
    {
      debut = un_arbre[0].debut ;
      fin = un_arbre[0].fin ;
    }
  else
    return(0) ; /* Plus rien à ajouter */
      
  /*
   * Construction de l'arbre
   */
      
  do
    {
      segment_connexe_ajoute = Faux ;      
      for(i=0; i<o->nb_transfos; i++)
	{
	  /*
	   * On cherche les segments connexes
	   */
	  for(j=0; j<nb; j++)
	    {
	      for(k=0 ; k<2; k++)
		{
		  if ( s[j].debut == NULL )
		    continue ;
		  if ( u_distance_triplet(un_arbre[i].debut, s[j].fin) < EPS )
		    {
		      un_arbre[o->nb_transfos++] = s[j] ;
		      debut = s[j].debut ;
		      s[j].debut = NULL ;
		      s[j].fin = NULL ;
		      segment_connexe_ajoute = Vrai ;
		      break ;
		    }
		  if ( u_distance_triplet(un_arbre[i].fin, s[j].debut) < EPS )
		    {
		      un_arbre[o->nb_transfos++] = s[j] ;
		      fin = s[j].fin ;
		      s[j].debut = NULL ;
		      s[j].fin = NULL ;
		      segment_connexe_ajoute = Vrai ;
		      break ;
		    }
		  tmp = s[j].debut ;
		  s[j].debut = s[j].fin ;
		  s[j].fin = tmp ;
		}
	    }
	}
      if ( o->autorise_non_connexe
	   && segment_connexe_ajoute == Faux
	   && nb != o->nb_transfos
	   )
	{
	  if ( prend_un_segment(s, nb, un_arbre, &o->nb_transfos)==0 )
	    {
	      ICI ;
	    }
	  segment_connexe_ajoute = Vrai ;
	}
    }
  while( segment_connexe_ajoute ) ;

  if ( o->autorise_non_connexe || o->cherche_les_points_les_plus_eloignes )
    {
      /*
       * Recherche des points les plus éloignés
       */
      distance_max = 0 ;
      for(i=0; i<o->nb_transfos; i++)
	for(j=0; j<o->nb_transfos; j++)
	  {
	    met_a_jour_distance_max(un_arbre[i].fin, un_arbre[j].fin
				    , &debut, &fin, &distance_max) ;
	    met_a_jour_distance_max(un_arbre[i].fin, un_arbre[j].debut
				    , &debut, &fin, &distance_max) ;
	    met_a_jour_distance_max(un_arbre[i].debut, un_arbre[j].fin
				    , &debut, &fin, &distance_max) ;
	    met_a_jour_distance_max(un_arbre[i].debut, un_arbre[j].debut
				    , &debut, &fin, &distance_max) ;
	  }
    }


  if ( TRACE(o) )
    {
      eprintf("Arbre de segment :\n") ;
      for(i=0; i<o->nb_transfos; i++)
	{
	  eprintf("%s vers ", triplet_vers_char(un_arbre[i].debut)) ;
	  eprintf("%s\n", triplet_vers_char(un_arbre[i].fin)) ;
	}
      eprintf("Début : %s\n", triplet_vers_char(debut)) ;
      eprintf("Fin : %s\n", triplet_vers_char(fin)) ;
    }


  o->bas_gauche = *debut ;
  o->haut_droit = *fin ;

  /* 8/2/2001, corrige le sens */
  if ( o->corrige_le_sens_des_fleches )
    for(i=0; i<o->nb_transfos; i++)
      {
	if ( u_distance_triplet(un_arbre[i].fin, debut) < EPS )
	  {
	    for(i=0; i<o->nb_transfos; i++)
	      {
		tmp = un_arbre[i].debut ;
		un_arbre[i].debut = un_arbre[i].fin ;
		un_arbre[i].fin = tmp ;
	      }
	    break ;
	  }
      }
  /*
   * Calcul des transformations
   */

  if ( o->segment_initial_fixe )
    {
      o->bas_gauche.x = -3 ;
      o->bas_gauche.y = 0 ;
      o->bas_gauche.z = 0 ;
      o->haut_droit.x = 3 ;
      o->haut_droit.y = 0 ;
      o->haut_droit.z = 0 ;
      debut = &o->bas_gauche ;
      fin = &o->haut_droit ;
    }

  ALLOUER(o->t, o->nb_transfos) ;
  for(i=0; i<o->nb_transfos; i++)
    {
      calcul_transfo(debut, fin, &un_arbre[i], &o->t[i], o->z_reste_vertical
		     , o->reduction_trait) ;
    }

  if ( o->autorise_non_connexe )
    return(0) ;

  free(un_arbre) ;
  return(1) ;
}

static void changement_ifs_filaire(struct ifs_commun *o)
{
  int i ;
  int nb ;
  struct segment *s ;

  if ( TRACE(o) )
    eprintf("Affiche IFS filaire, ic.seulement_les_actives=%d\n"
	    , o->seulement_les_actives) ;

  if ( o->prof_recur > TAILLE_TABLE(o->position) )  /* Limitation profondeur recursion */
    o->prof_recur = TAILLE_TABLE(o->position) ;

  o->ifs_filaire = Vrai ;

  destruction_ifs(o) ;

  recupere_transformation(o) ;
  nb = o->nb_transfos ;

  if ( TRACE(o) )
    eprintf("Affiche IFS filaire avec %d segments, figure=%d\n", nb
	    , o->figure.index) ;
  ALLOUER(s, nb) ;
  for(i=0; i<nb; i++)
    {
      if ( TRACE(o) )
	{
	  eprintf("%s ==>", triplet_vers_char(o->points[i])) ;
	  eprintf("%s\n", triplet_vers_char(o->points[i]+1)) ;
	}
      s[i].debut = o->points[i] ;
      s[i].fin = o->points[i]+1 ;
    }

  o->autorise_non_connexe = Vrai ;

  if ( o->figure.index == 0 )
    o->type_affichage = Affiche_Segment ;
  else
    o->type_affichage = o->figure.index - 1 ;

  while( affiche_un_ifs_filaire(o, s, nb) )
    {
    }

  calcul_v(o) ;

  if ( TRACE(o) )
    {
      eprintf("Transformations :\n") ;
      for(i=0;i<o->nb_transfos;i++)
	eprintf("%s\n\n", codec_recupere_texte(&codec_transfo, &o->t[i])) ;
    }

}

CLASSE(demo_ifs_filaire, struct ifs_commun,
       SUR_CLASSE("demo_ifs_commun")

       CHAMP(cherche_les_points_les_plus_eloignes, L_booleen Edite Sauve DEFAUT("0"))
       CHAMP(fleche, L_booleen Edite Sauve DEFAUT("0"))
       CHAMP(segment_initial_fixe, L_booleen Edite Sauve DEFAUT("0"))
       CHAMP_VIRTUEL(Obligatoire CLASSE_PARAMETRE("segment_pt_pt"))
       CHANGEMENT(changement_ifs_filaire)
       )



     
     
