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
#include "codec.h"

TYPE_PHYSIQUE(codec_float1, float,
	      if ( codec_float(a) ) { *a = 1. ; return(1) ; }
	      )
TYPE_PHYSIQUE(codec_double1, double,
	      if ( codec_double(a) ) { *a = 1. ; return(1) ; }
	      )
TYPE_PHYSIQUE(codec_float_lf, float,
	      codec_float(a) ;
	      codec_linefeed() ;		
	      )
TYPE_PHYSIQUE(codec_double_lf, double,
	      codec_double(a) ;
	      codec_linefeed() ;	
	      )
TYPE_PHYSIQUE(codec_float1_lf, float,
	      codec_float1(a) ;
	      codec_linefeed() ;		
	      )
TYPE_PHYSIQUE(codec_double1_lf, double,
	      codec_double1(a) ;
	      codec_linefeed() ;		
	      )

TYPE_PHYSIQUE(codec_l3_float, t_l3_float,
	      {
		codec_float(a->table+0) ;
		codec_float(a->table+1) ;
		codec_float(a->table+2) ;
		codec_linefeed() ;
	      }
	      )

TYPE_PHYSIQUE(codec_l3_float_chaine, t_l3_float_chaine,
	      {
		codec_float(a->table+0) ;
		codec_float(a->table+1) ;
		codec_float(a->table+2) ;
		codec_chaine(&a->chaine) ;
		codec_linefeed() ;
	      }
	      )
TYPE_PHYSIQUE(codec_pcc, t_pcc,
	      {
		codec_float(a->position+0) ;
		codec_float(a->position+1) ;
		codec_float(a->position+2) ;
		codec_float(a->couleur+0) ;
		codec_float(a->couleur+1) ;
		codec_float(a->couleur+2) ;
		codec_chaine(&a->chaine) ;
		codec_linefeed() ;
	      }
	      )

TYPE_PHYSIQUE(codec_l2_float, t_l2_float,
	      {
		codec_float(a->table+0) ;
		codec_float(a->table+1) ;
		codec_linefeed() ;
	      }
	      )

TYPE_PHYSIQUE(codec_l2_double, t_l2_double,
	      {
		codec_double(a->table+0) ;
		codec_double(a->table+1) ;
		codec_linefeed() ;
	      }
	      )

TYPE_PHYSIQUE(codec_l3_int, t_l3_int,
	      {
		codec_int(a->table+0) ;
		codec_int(a->table+1) ;
		codec_int(a->table+2) ;
		codec_linefeed() ;
	      }
	      )

TYPE_PHYSIQUE(codec_l4_int, t_l4_int,
	      {
		codec_int(a->table+0) ;
		codec_int(a->table+1) ;
		codec_int(a->table+2) ;
		codec_int(a->table+3) ;
		codec_linefeed() ;
	      }
	      )

TYPE_PHYSIQUE(codec_l3_unsigned_char, t_l3_unsigned_char,
	      {
		codec_unsigned_char(a->table+0) ;
		codec_unsigned_char(a->table+1) ;
		codec_unsigned_char(a->table+2) ;
		codec_linefeed() ;
	      }
	      )

TYPE_PHYSIQUE(codec_l4_unsigned_char, t_l4_unsigned_char,
	      {
		codec_unsigned_char(a->table+0) ;
		codec_unsigned_char(a->table+1) ;
		codec_unsigned_char(a->table+2) ;
		codec_unsigned_char(a->table+3) ;
		codec_linefeed() ;
	      }
	      )


TYPE_PHYSIQUE(codec_l3_float1, t_l3_float,
	      {
		codec_float(a->table+0) ;
		codec_float(a->table+1) ;
		codec_float1(a->table+2) ;
		codec_linefeed() ;
	      }
	      )

TYPE_PHYSIQUE(codec_l3_double, t_l3_double,
	      {
                codec_double(a->table+0) ;
		codec_double(a->table+1) ;
		codec_double(a->table+2) ;
		codec_linefeed() ;
	      }
	      )

TYPE_PHYSIQUE(codec_double_l2_double_ni, t_l3_double,
	      {
                codec_double(a->table+0) ;
		codec_double_ni(a->table+1) ;
		codec_double_ni(a->table+2) ;
		codec_linefeed() ;
	      }
	      )


TYPE_PHYSIQUE(codec_l3_double1, t_l3_double,
	      {
		codec_double(a->table) ;
		codec_double(a->table+1) ;
		codec_double1(a->table+2) ;
		codec_linefeed() ;
	      }
	      )

TYPE_PHYSIQUE(codec_l4_float, t_l4_float,
		codec_float(a->table+0) ;
		codec_float(a->table+1) ;
		codec_float(a->table+2) ;
		codec_float(a->table+3) ;
		codec_linefeed() ;
	      )
/*
 * Type existant pour le système de particule, on ne veut
 * pas que les flottants soient mis à 0 s'ils ne sont
 * pas définis.
 */
TYPE_PHYSIQUE(codec_l6_float, t_l6_float,
		codec_float(a->table+0) ;
		codec_float(a->table+1) ;
		codec_float(a->table+2) ;
		codec_float_ni(a->table+3) ;
		codec_float_ni(a->table+4) ;
		codec_float_ni(a->table+5) ;
		codec_linefeed() ;
	      )

TYPE_PHYSIQUE(codec_l4_float1, t_l4_float,
		codec_float(a->table+0) ;
		codec_float(a->table+1) ;
		codec_float(a->table+2) ;
		codec_float1(a->table+3) ;
		codec_linefeed() ;
	      )

TYPE_PHYSIQUE(codec_l4_double, t_l4_double,
		codec_double(a->table+0) ;
		codec_double(a->table+1) ;
		codec_double(a->table+2) ;
		codec_double(a->table+3) ;
		codec_linefeed() ;
	      )

TYPE_PHYSIQUE(codec_l4_double1, t_l4_double,
		codec_double(a->table+0) ;
		codec_double(a->table+1) ;
		codec_double(a->table+2) ;
		codec_double1(a->table+3) ;
		codec_linefeed() ;
	      )


TYPE_PHYSIQUE(codec_l2_l3_float, t_l2_l3_float,
	      {
		codec_l3_float(a->table+0) ;
		codec_l3_float(a->table+1) ;
		codec_linefeed() ;
	      }
	      )

TYPE_PHYSIQUE(codec_l2_l3_double, t_l2_l3_double,
	      {
		codec_l3_double(a->table+0) ;
		codec_l3_double(a->table+1) ;
		codec_linefeed() ;
	      }
	      )

TYPE_PHYSIQUE(codec_l3_l3_double, t_l3_l3_double,
	      {
		codec_l3_double(a->table+0) ;
		codec_l3_double(a->table+1) ;
		codec_l3_double(a->table+2) ;
		codec_linefeed() ;
	      }
	      )

TYPE_PHYSIQUE(codec_l4_l3_double, t_l4_l3_double,
	      {
		codec_l3_double(a->table+0) ;
		codec_l3_double(a->table+1) ;
		codec_l3_double(a->table+2) ;
		codec_l3_double(a->table+3) ;
		codec_linefeed() ;
	      }
	      )
TYPE_PHYSIQUE(codec_l6_l4_double, t_l6_l4_double,
	      {
	      codec_l(a, 6, codec_l4_double) ;
	      }
	      )


TYPE_PHYSIQUE(codec_h4l3_double, t_l4_l3_double,
	      {
		codec_ouvre_crochet() ;
		codec_ouvre_parenthese() ;
		codec_l3_double(a->table+0) ;
		codec_ferme_parenthese() ;
		codec_ouvre_parenthese() ;
		codec_l3_double(a->table+1) ;
		codec_ferme_parenthese() ;
		codec_ouvre_parenthese() ;
		codec_l3_double(a->table+2) ;
		codec_ferme_parenthese() ;
		codec_ouvre_parenthese() ;
		codec_l3_double(a->table+3) ;
		codec_ferme_parenthese() ;
		codec_ferme_crochet() ;
	      }
	      )

TYPE_PHYSIQUE(codec_L_double     ,t_L_double     ,
	      codec_L((void**)&a->table, &a->lar, codec_double) ;
	      )
TYPE_PHYSIQUE(codec_L_float      ,t_L_float     ,
	      codec_L((void**)&a->table, &a->lar, codec_float) ;
	      )
TYPE_PHYSIQUE(codec_L_int        ,t_L_int        ,
	      codec_L((void**)&a->table, &a->lar, codec_int) ;
	      )
TYPE_PHYSIQUE(codec_L_l2_float  ,t_L_l2_float  ,
	      codec_L((void**)&a->table, &a->lar, codec_l2_float) ;
	      )
TYPE_PHYSIQUE(codec_L_l2_double  ,t_L_l2_double  ,
	      codec_L((void**)&a->table, &a->lar, codec_l2_double) ;
	      )
TYPE_PHYSIQUE(codec_L_l3_double  ,t_L_l3_double  ,
	      codec_L((void**)&a->table, &a->lar, codec_l3_double) ;
	      )
TYPE_PHYSIQUE(codec_L_l4_double  ,t_L_l4_double  ,
	      codec_L((void**)&a->table, &a->lar, codec_l4_double) ;
	      )
TYPE_PHYSIQUE(codec_L_l4_int  ,t_L_l4_int  ,
	      codec_L((void**)&a->table, &a->lar, codec_l4_int) ;
	      )
TYPE_PHYSIQUE(codec_L_l4_double1  ,t_L_l4_double  ,
	      codec_L((void**)&a->table, &a->lar, codec_l4_double1) ;
	      )
TYPE_PHYSIQUE(codec_L_L_l3_double,t_L_L_l3_double,
	      codec_L((void**)&a->table, &a->lar, codec_L_l3_double) ;
	      )
TYPE_PHYSIQUE(codec_L_L_l4_double,t_L_L_l4_double,
	      codec_L((void**)&a->table, &a->lar, codec_L_l4_double) ;
	      )
TYPE_PHYSIQUE(codec_L_h4l3_double,t_L_l4_l3_double,
	      codec_L((void**)&a->table, &a->lar, codec_h4l3_double) ;
	      )
TYPE_PHYSIQUE(codec_HL_l3_double ,t_HL_l3_double ,
	      codec_HL((void**)&a->table, &a->hau, &a->lar, codec_l3_double) ;
	      )
TYPE_PHYSIQUE(codec_HL_l3_float ,t_HL_l3_float ,
	      codec_HL((void**)&a->table, &a->hau, &a->lar, codec_l3_float) ;
	      )
TYPE_PHYSIQUE(codec_HL_l4_double ,t_HL_l4_double ,
	      codec_HL((void**)&a->table, &a->hau, &a->lar, codec_l4_double) ;
	      )
TYPE_PHYSIQUE(codec_HL_l3_unsigned_char, t_HL_l3_unsigned_char,
	      codec_HL((void**)&a->table, &a->hau, &a->lar, codec_l3_unsigned_char))

TYPE_PHYSIQUE(codec_HL_l4_unsigned_char, t_HL_l4_unsigned_char,
	      codec_HL((void**)&a->table, &a->hau, &a->lar, codec_l4_unsigned_char) ;
	      )

TYPE_PHYSIQUE(codec_HL_unsigned_char, t_HL_unsigned_char,
	      codec_HL((void**)&a->table, &a->hau, &a->lar, codec_unsigned_char) ;
	      )
TYPE_PHYSIQUE(codec_HH_float ,t_HH_float ,
	      codec_HH((void**)&a->table, &a->hau, codec_float) ;
	      )
TYPE_PHYSIQUE(codec_H_L_l3_double,t_H_L_l3_double,
	      codec_H_L((void***)&a->table, &a->hau, &a->lar, codec_l3_double) ;
	      )
TYPE_PHYSIQUE(codec_H_L_l4_double,t_H_L_l4_double,
	      codec_H_L((void***)&a->table, &a->hau, &a->lar, codec_l4_double) ;
	      )
TYPE_PHYSIQUE(codec_H_L_float    ,t_H_L_float    ,
	      codec_H_L((void***)&a->table, &a->hau, &a->lar, codec_float) ;
	      )
TYPE_PHYSIQUE(codec_H_L_double   ,t_H_L_double   ,
	      codec_H_L((void***)&a->table, &a->hau, &a->lar, codec_double) ;
	      )
TYPE_PHYSIQUE(codec_h3_L_float    ,t_h3_L_float    ,
	      codec_h_L((void**)a->table, 3, &a->lar, codec_float) ;
	      )
TYPE_PHYSIQUE(codec_L_h3_float    ,t_h3_L_float    ,
	      codec_L_h((void**)a->table, 3, &a->lar, codec_float) ;
	      )


TYPE_PHYSIQUE(codec_PHL_l3_float ,t_PHL_l3_float ,
	      codec_PHL((void**)&a->table, &a->pro, &a->hau, &a->lar, codec_l3_float) ;
	      )
TYPE_PHYSIQUE(codec_PHL_float    ,t_PHL_float    ,
	      codec_PHL((void**)&a->table, &a->pro, &a->hau, &a->lar, codec_float) ;
	      )

TYPE_PHYSIQUE(codec_P_H_L_l3_float ,t_P_H_L_l3_float ,
	      codec_P_H_L((void****)&a->table, &a->pro, &a->hau, &a->lar, codec_l3_float) ;)
TYPE_PHYSIQUE(codec_P_H_L_l3_double ,t_P_H_L_l3_double ,
	      codec_P_H_L((void****)&a->table, &a->pro, &a->hau, &a->lar, codec_l3_double) ;)
TYPE_PHYSIQUE(codec_P_H_L_l4_float1 ,t_P_H_L_l4_float ,
	      codec_P_H_L((void****)&a->table, &a->pro, &a->hau, &a->lar, codec_l4_float1) ;)
TYPE_PHYSIQUE(codec_P_H_L_l4_double1 ,t_P_H_L_l4_double ,
	      codec_P_H_L((void****)&a->table, &a->pro, &a->hau, &a->lar, codec_l4_double1) ;)



TYPE_PHYSIQUE(codec_angle, t_angle,
	      {
		if ( G->action == Recupere_texte )
		  {
		    double angle ;
		    if ( a->unite == 'd' )
		      angle = a->angle/M_PI*180 ;
		    else
		      angle = a->angle ;

		    codec_double(&angle) ;
		    codec_char(&a->unite) ;
		    codec_linefeed() ;
		  }
		else if ( G->action == Sauve_texte )
		  {
		    double angle ;
		    codec_double(&angle) ;
		    codec_char(&a->unite) ;
		    codec_linefeed() ;
		    if ( a->unite == 'd' )
		      a->angle = angle/180*M_PI ;
		    else
		      {
			a->unite = 'r' ;
			a->angle = angle ;
		      }
		  }
		else
		  {
		    codec_double(&a->angle) ;
		    codec_char(&a->unite) ;
		    codec_linefeed() ;
		  }
	      }
	      )


/*
 * Le graphe sans aucune information
 */
CODEC_GRAPHE(,codec_rien, void*, codec_rien, void*, codec_rien, void*)
/*
 * Le graphe avec un l3_float en chaque noeud
 */
CODEC_GRAPHE(_N_l3_float, codec_rien, void*, codec_l3_float, t_l3_float, codec_rien, void*)
/*
 * Le graphe avec un l2_l3_float en chaque noeud
 */
CODEC_GRAPHE(_N_l2_l3_float, codec_rien, void*, codec_l2_l3_float, t_l2_l3_float, codec_rien, void*)
/*
 * Le graphe avec un l6_float en chaque noeud
 */
CODEC_GRAPHE(_N_l6_float, codec_rien, void*, codec_l6_float, t_l6_float, codec_rien, void*)
/*
 * Le graphe avec un l3_float+chaine en chaque noeud
 * et une chaine en chaque arete.
 */
CODEC_GRAPHE(_N_l3_float_chaine_A_chaine, codec_rien, void*, codec_l3_float_chaine, t_l3_float_chaine, codec_chaine, char*)
/*
 * Le graphe avec un l3_float+l3_float+chaine en chaque noeud
 * et une chaine en chaque arete.
 */
CODEC_GRAPHE(_N_pcc_A_chaine, codec_rien, void*, codec_pcc, t_l3_float_chaine, codec_chaine, char*)

/*
 * Liste d'entier
 */
CODEC_LISTE(_int, codec_int, int)
CODEC_LISTE(_unsigned_long, codec_unsigned_long, unsigned long)
/*
*******************************************************************************
 * Exemples d'enumération
*******************************************************************************
 */
/*
 * Arbre avec un entier sur les noeuds et feuilles
 */
static char* globale_enum_noeud_feuille[] =
{
  "NOEUD",
  "FEUILLE",
} ;

TYPE_PHYSIQUE(codec_enum_noeud_feuille, int,
	      return( codec_enum(a, 2, globale_enum_noeud_feuille) ) ;
	      )
TYPE_PHYSIQUE(codec_pointeur_arbre_N_int_F_int, t_pointeur_arbre_N_int_F_int,
	      return( codec_pointeur_non_null((void**)a, codec_arbre_N_int_F_int) ) ;
	      )

TYPE_PHYSIQUE(codec_noeud_arbre_N_int_F_int, t_arbre_N_int_F_int,
	      {
		codec_int(&a->t.noeud.donnee_noeud) ;
		codec_pointeur_arbre_N_int_F_int(&a->t.noeud.g) ;
		codec_pointeur_arbre_N_int_F_int(&a->t.noeud.d) ;
	      }
	      )

TYPE_PHYSIQUE(codec_feuille_arbre_N_int_F_int, t_arbre_N_int_F_int,
	      return( codec_int(&a->t.donnee_feuille) ) ;
	      )

static Codec globale_union_arbre[] =
{
  codec_noeud_arbre_N_int_F_int,
  codec_feuille_arbre_N_int_F_int,
} ;

TYPE_PHYSIQUE(codec_arbre_N_int_F_int, t_arbre_N_int_F_int,
	      if ( codec_union(a, &a->type, 2,
			   codec_enum_noeud_feuille,
			    globale_union_arbre,
			    1) )
	      {
		a->type = 1 ;
		a->t.donnee_feuille = -1 ;
	      }
	      )

/*
 * Octree
 */

static char* globale_enum_octree[] = { "0", "1", "(" } ;

TYPE_PHYSIQUE(codec_enum_octree, int,
	      return( codec_enum(a, 3, globale_enum_octree) ) ;
	      )

TYPE_PHYSIQUE(codec_pointeur_octree, t_pointeur_octree,
	      return( codec_pointeur_non_null((void**)a, codec_octree) ) ;
	      )

TYPE_PHYSIQUE(codec_octree_fils, t_octree,
	      int err ;
	      codec_ouvre_parenthese() ;
	      err = codec_l(a->fils, 8, codec_pointeur_octree) ;
	      codec_ferme_parenthese() ;
	      return(err) ;
	      )

static Codec globale_union_octree[]={codec_rien,codec_rien,codec_octree_fils};

TYPE_PHYSIQUE(codec_octree, t_octree,
	      return( codec_union(a,&a->type,3,
			    codec_enum_octree,globale_union_octree,
			    0) ) ;
	      )
/*
 *
 */

TYPE_PHYSIQUE(codec_l3_double_nolf, t_l3_double,
	      {
                codec_double(a->table+0) ;
		codec_double(a->table+1) ;
		codec_double(a->table+2) ;
	      }
	      )
TYPE_PHYSIQUE(codec_L_hh_l3_double_double, t_L_hh_l3_double_double,
	      codec_L_hh((void**)&a->point, codec_l3_double_nolf,
			 (void**)&a->poids, codec_double_lf,
			 &a->lar) ;
	      )
