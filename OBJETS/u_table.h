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
 * Créé le Fri May 28 11:28:45 1999 par Thierry EXCOFFIER
 */

#ifndef _HOME_EXCO_NE_1_4_OBJETS_U_TABLE_H
#define _HOME_EXCO_NE_1_4_OBJETS_U_TABLE_H

#define NB_TAG_PLAGE    (DEBUT_TAG_DEPLACABLE/4)
#define TAG_DEPLACEMENT (1*NB_TAG_PLAGE)
#define TAG_POIDS       (2*NB_TAG_PLAGE)
#define TAG_SPLIT       (3*NB_TAG_PLAGE)
#define TAG_DESTRUCTION DEBUT_TAG_DEPLACABLE

#define T_PONCTUEL         00000001
#define T_FILAIRE          00000002
#define T_POLYGONE         00000004 /* FILAIRE fermé */
#define T_FACETTE          00000010
#define T_TAG              00000020
#define T_TAG_DESTRUCTION  00000040
#define T_TAG_POIDS        00000100
#define T_LISSAGE          00000200
#define T_TAG_SPLIT        00000400
#define T_CULL_FACE        00001000 /* Si vrai ===> la surface est fermée */
#define T_DEGENERE_HAUT    00002000
#define T_DEGENERE_BAS     00004000
#define T_DEGENERE_GAUCHE  00010000
#define T_DEGENERE_DROITE  00020000
#define T_FACETTE_HAUT     00040000
#define T_FACETTE_BAS      00100000
#define T_FACETTE_GAUCHE   00200000
#define T_FACETTE_DROITE   00400000
#define T_RACCORDE_GD      01000000
#define T_RACCORDE_HB      02000000


void t_table(const void *o, const Champ *ch, int comment) ;
const char * lit_tag_table(const void *o, const Champ *ch) ;
void ecrit_tag_table(void *o, const Champ *ch, const char *v) ;
void t_table_pov(const void *o, const Champ *ch,
		 int comment, const char *urgent) ;
void t_table_stl(const void *o, const Champ *ch, int comment, const char *urgent) ;
void t_table_stl_general(const void *t, const Codec_informations *tp
			 ,int comment , FILE *f) ;

void changement_table(void *o, const Champ *ch) ;

void t_grille(const void *o, const Champ *ch, int comment, const char *urgent,
	      const Champ *chn) ;
void t_grille_pov(const void *o, const Champ *ch,
		  int comment, const char *urgent, const Champ *chn) ;
const char * lit_tag_grille(const void *o, const Champ *ch) ;
void ecrit_tag_grille(void *o, const Champ *ch, const char *v) ;


void t_table_general(const void *t, const Codec_informations *tp, int comment) ;
void t_table_pov_general(const void *t, const Codec_informations *tp
			 ,int comment , FILE *f) ;
void t_grille_general(const void *t, const Codec_informations *tp
		      , int comment, const char *urgent,
		      const void *n, const Codec_informations *tpn) ;
void t_grille_pov_general(const void *o, const Codec_informations *tp,
			  int comment, FILE *f
			  , const void *n, const Codec_informations *tpn) ;


void t_grille_stl_general(const void *t, const Codec_informations *tp,
		  int comment, FILE *f) ;
void t_grille_stl(const void *o, const Champ *ch, int comment, const char *urgent) ;

void t_grille_obj_general(const void *t, const Codec_informations *tp,
		  int comment, FILE *f) ;

void changement_grille(void *o, const Champ *ch) ;




void t_tenseur(const void *t, const Codec_informations *tp
	       , int comment, const char *urgent) ;
const char * lit_tag_tenseur(const void *o, const Champ *ch) ;
void ecrit_tag_tenseur(void *o, const Champ *ch, const char *v) ;
void changement_tenseur(void *o, const Champ *ch) ;

#endif
