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
#ifndef U_GEOMETRIE_H
#define U_GEOMETRIE_H

#ifdef __CHECKER__
#include "enleve_gl.h"
#endif

#include "u_base.h"

/*
 * Conserver la logique entre double et flottant
 */
#ifndef Flottant
typedef double Flottant ;
#endif

#define infos_codec_flottant1     	       	  infos_codec_double1
#define infos_codec_flottant     	       	  infos_codec_double
#define infos_codec_triplet      	       	  infos_codec_l3_double
#define infos_codec_quadruplet   	       	  infos_codec_l4_double
#define infos_codec_quadruplet1   	       	  infos_codec_l4_double1
#define infos_codec_boite_min_max      	       	  infos_codec_l2_l3_double
#define infos_codec_transfo      	       	  infos_codec_h4l3_double
#define infos_codec_table_flottant       	  infos_codec_L_double
#define infos_codec_table_triplet        	  infos_codec_L_l3_double
#define infos_codec_table_quadruplet     	  infos_codec_L_l4_double
#define infos_codec_table_quadruplet1     	  infos_codec_L_l4_double1
#define infos_codec_table_table_triplet  	  infos_codec_L_L_l3_double
#define infos_codec_table_table_quadruplet        infos_codec_L_L_l4_double
#define infos_codec_table_transfo      	       	  infos_codec_L_h4l3_double
#define infos_codec_grille_triplet       	  infos_codec_H_L_l3_double
#define infos_codec_grille_quadruplet    	  infos_codec_H_L_l4_double
#define infos_codec_grille_flottant      	  infos_codec_H_L_double
#define infos_codec_6_quadruplet      	          infos_codec_l6_l4_double
#define infos_codec_flottant_borne                infos_codec_double_l2_double_ni

#define codec_flottant1     	       	  codec_double1
#define codec_flottant     	       	  codec_double
#define codec_triplet      	       	  codec_l3_double
#define codec_quadruplet   	       	  codec_l4_double
#define codec_quadruplet1   	       	  codec_l4_double1
#define codec_boite_min_max   	       	  codec_l2_l3_double
#define codec_transfo      	       	  codec_h4l3_double
#define codec_table_flottant       	  codec_L_double
#define codec_table_triplet        	  codec_L_l3_double
#define codec_table_quadruplet     	  codec_L_l4_double
#define codec_table_transfo     	  codec_L_h4l3_double
#define codec_table_table_triplet  	  codec_L_L_l3_double
#define codec_table_table_quadruplet      codec_L_L_l4_double
#define codec_grille_triplet       	  codec_H_L_l3_double
#define codec_grille_quadruplet    	  codec_H_L_l4_double
#define codec_grille_flottant      	  codec_H_L_double
#define codec_flottant_borne              codec_double_l2_double_ni


#define FF "%lg"
#define FF3 FF " " FF " " FF
#define FF4 FF3 " " FF

#define FLOTTANT_EPSILON (1e-6)

typedef struct triplet
{
  Flottant x,y,z ;
} Triplet ;

typedef struct quadruplet
{
  Flottant x,y,z,h ;
} Quadruplet ;

typedef struct six_quadruplet
{
  Quadruplet t[6] ;
} Six_quadruplets ;

typedef struct transfo		/* Ne pas changer l'ordre */
{
  Triplet v1 ;
  Triplet v2 ;  
  Triplet v3 ;  
  Triplet origine ;
} Transfo ;

typedef struct 
{
  Triplet point_de_vue, point_vise, haut ;
  Flottant focale ;
  Flottant ratio ;
} t_observateur ;

typedef struct
{
  Triplet min, max ;
} t_boite_min_max ;


Triplet u_triplet_fois_flottant(const Triplet*a , Flottant f) ;
Quadruplet u_quadruplet_fois_flottant(const Quadruplet *a, Flottant f) ;
Triplet      u_rotation_triplet(const Triplet *v,
				const Triplet *axe,
				Flottant angle) ;
Triplet     u_normalise_triplet(const Triplet*a) ;
Triplet        u_ajoute_triplet(const Triplet*b, const Triplet*a) ;
Triplet 	   u_pv_triplet(const Triplet*a, const Triplet*b) ;
Triplet     u_soustrait_triplet(const Triplet*a, const Triplet*b) ;
Triplet u_ponderation_2_triplet(const Triplet *a, Flottant pa,
				const Triplet *b, Flottant pb) ;
Triplet u_barycentre_2_triplet(const Triplet *a, Flottant pa,
				const Triplet *b) ;
Triplet u_milieu_2_triplet(const Triplet *a, const Triplet *b) ;
Quadruplet        u_ajoute_quadruplet(const Quadruplet*b, const Quadruplet*a) ;
Quadruplet u_ponderation_2_quadruplet(const Quadruplet *a, Flottant pa,
				const Quadruplet *b, Flottant pb) ;
Quadruplet u_barycentre_2_quadruplet(const Quadruplet *a, Flottant pa,
				const Quadruplet *b) ;
Quadruplet u_milieu_2_quadruplet(const Quadruplet *a, const Quadruplet *b) ;
Booleen u_triplet_invalide(const Triplet *a) ;


Flottant	   u_ps_triplet(const Triplet*a, const Triplet*b) ;
Flottant     u_distance_triplet(const Triplet*a, const Triplet*b) ;
Flottant	u_norme_triplet(const Triplet*a) ;
Flottant      u_projete_triplet(Triplet *pt, Triplet *depart, Triplet *vect) ;

void           u_annule_triplet(Triplet *a) ;
void        u_annule_quadruplet(Quadruplet *a) ;
void     u_annule_boite_min_max(t_boite_min_max *a) ;
void u_agrandir_boite_min_max(t_boite_min_max *a, const Triplet *b) ;
Triplet u_vecteur_min_boite_min_max(const t_boite_min_max *a) ;
Triplet u_vecteur_max_boite_min_max(const t_boite_min_max *a) ;
void u_observateur_de_boite_min_max(const t_boite_min_max *a,t_observateur *o);

Triplet u_normale_triangle(const Triplet *a,const Triplet *b,const Triplet *c);




Transfo       u_produit_transfo(const Transfo *g, const Transfo *d) ;
Triplet            u_transforme(const Transfo *t, const Triplet *p) ;
void           u_annule_transfo(Transfo *a) ;
void         u_identite_transfo(Transfo *a) ;
Flottant determinant_transfo(const Transfo *in) ;
Transfo inversion_transfo(const Transfo *in) ;


void inversion4x4(const double in[16], double out[16]) ;

Triplet u_polaires_vers_cartesiennes(Flottant alpha, Flottant beta) ;
void u_cartesiennes_vers_polaires(const Triplet *p, Flottant *alpha, Flottant *beta) ;


void u_repere_de_vecteur(const Triplet *z, Triplet *x, Triplet *y) ;

Triplet u_intersection_3_plans(const Quadruplet *a, const Quadruplet *b, const Quadruplet *c) ;
Quadruplet u_creation_plan_lie(const Triplet *normale, const Triplet *point) ;
Triplet u_point_du_plan(const Quadruplet *a) ;
Flottant u_distance_plan_triplet_signee(const Quadruplet *p, const Triplet *a);
Quadruplet u_plan_inverse(const Quadruplet *a) ;

#endif
