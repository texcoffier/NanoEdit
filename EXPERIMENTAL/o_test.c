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
#include "o_objet.h"		/* Car il faut définir un objet */
#include "t_geometrie.h"
#include "u_table.h"

typedef struct
{
  Flottant sextuplet[6] ;
} Sextuplet ;

struct yyy			/* Votre structure de donnée */
{
  Flottant entieryyy[6] ;
} ;

TYPE_PHYSIQUE(codec_sext, Sextuplet ,
	      /*	      codec_l3_double_nolf((t_l3_double*)a) ; */
		codec_double   (&a->sextuplet[0]) ;
		codec_double   (&a->sextuplet[1]) ;
		codec_double   (&a->sextuplet[2]) ;
		codec_double_ni(&a->sextuplet[3]) ;
		codec_double_ni(&a->sextuplet[4]) ;
		codec_double_ni(&a->sextuplet[5]) ;
		codec_linefeed() ;
	      )


CLASSE(yyy,		/* Nom de la classe */
       struct yyy,		/* La structure de donnée liée à l'objet */
       CHAMP(entieryyy, L_point TP(codec_sext) Edite Sauve)
       EVENEMENT("TYY")
       MENU("TestYYY")
      )


typedef struct
{
  Table t ;
} Tbl ;

TYPE_PHYSIQUE(codec_L_table6      ,Table     ,
	      codec_L((void**)&a->table, &a->nb, codec_sext) ;
	      )


static void affiche_table(Tbl *o)
{
  if ( ACTIF(o) )
    t_table(o, c_trouve(o,"t"), T_TAG|T_TAG_DESTRUCTION) ;
  else
    t_table(o, c_trouve(o,"t"), T_PONCTUEL) ;
}

static void changement_table_n_uplet(void *o)
{
  changement_table(o, c_trouve(o, "t")) ;
}

CLASSE(table6, Tbl,
       CHAMP(t   , L_table_point3 TP(codec_L_table6) Edite Obligatoire TYPE_DANS_FILS(L_point))
       CHAMP_VIRTUEL(L_tag(ecrit_tag_table,lit_tag_table))
       CHAMP_VIRTUEL(L_affiche_gl(affiche_table)
		     AFFICHAGE(Different_si_actif|Different_suivant_qualite))
       CHANGEMENT(changement_table_n_uplet)
       EVENEMENT("TTA")
       MENU("TestTable")
       PAQUET(NB_ACTIFS)
       )


