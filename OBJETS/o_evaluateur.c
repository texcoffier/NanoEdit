/*
    NanoÉdit: Un modeleur algébrique interactif.
    Copyright (C) 2006 Thierry EXCOFFIER, Université Claude Bernard, LIRIS

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
#include <ctype.h>
#include <math.h>
#include "o_objet.h"
#include "t_geometrie.h"

typedef struct
{
  char *expression ;
  Table_flottant resultat ;
  Flottant nombre ;
  Triplet triplet ;
} Evaluateur ;

/* Types recherchés dans les fils, du premier au dernier.
 * On s'arrête quand on a trouvé un objet de ce type.
 */
static Type_logique* types_lus[2][6] =
  {
    {
      // Accès via $
      &infos_L_table_nombre,
      &infos_L_quadruplet,
      &infos_L_triplet,
      &infos_L_nombre,
      &infos_L_valeur,
      NULL
    },
    {
      // Accès via #
      &infos_L_quadruplet,
      &infos_L_triplet,
      &infos_L_valeur,
      NULL
    }
} ;

static void changement_evaluateur(Evaluateur *o)
{
  char *p ;
  Table_flottant last = {0}, current = {0}  ;
  char what, what2 ;
  Objet *f ;
  const char* c ;
  int i ;

  if ( ! (UN_CHAMP_CHANGE(o) || FILS_CHANGE(o) ) )
    return ;

  codec_sauve_texte(codec_table_flottant, &last, "0") ;
  codec_sauve_texte(codec_table_flottant, &current, "0") ;

  what = ' ' ;
  what2 = ' ' ;
  for(p=o->expression; *p;)
    {
      if ( *p == '\n' || *p == ' ' || *p == '\t')
	{
	  p++ ;
	  continue ;
	}
      if ( strncmp(p,"//",2) == 0 )
	{
	  break ;
	}
      if ( strncmp(p,">=",2) == 0 )
	{
	  what = 'G' ;
	  p += 2 ;
	  continue ;
	}
      if ( strncmp(p,"<=",2) == 0 )
	{
	  what = 'L' ;
	  p += 2 ;
	  continue ;
	}
      if ( *p == '~' || *p == '%' || *p == '+' || (*p == '-' && p[1]==' ') || *p == '/' || *p == '*' || *p == '^'  ||  *p == '<' || *p == '>' || *p == '=')
	{
	  what = *p++ ;
	  continue ;
	}
      if ( strncmp(p,"cos",3) == 0 )
	{
	  what2 = 'c' ;
	  p += 3 ;
	  continue ;
	}
      if ( strncmp(p,"sin",3) == 0 )
	{
	  what2 = 's' ;
	  p += 3 ;
	  continue ;
	}
      if ( strncmp(p,"norme",5) == 0 )
	{
	  what2 = 'n' ;
	  p += 5 ;
	  continue ;
	}

      if ( isdigit(*p) || *p == '.' || (*p == '-' && p[1] != ' ') || *p == '+' )
	{
	  Flottant v = strtod(p, &p) ;
	  codec_sauve_texte(codec_table_flottant, &current, flottant_vers_char(v)) ;
	}
      else if ( *p == '$' || *p == '#' )
	{
	  Type_logique** tlo ;
	  tlo = types_lus[*p != '$'] ;

	  f = FILS(o, strtol(p+1, &p, 10)) ;
	  if ( f == NULL )
	    NOTE_CHAMP_INVALIDE(o, expression) ;
	  else
	    {
	      c = "" ;
	      for(i=0; tlo[i]; i++)
		{
		  c = c_recupere_type_sans_copie(f, tlo[i]) ;
		  if ( c[0] )
		    break ;
		}
	      codec_sauve_texte(codec_table_flottant, &current, c) ;
	      if ( current.nb == 0 )
		codec_sauve_texte(codec_table_flottant, &current, "0") ;
	    }
	  if ( *p == ':' )
	    {
	      i = strtol(p+1, &p, 10) ;
	      codec_sauve_texte(codec_table_flottant, &current,
				flottant_vers_char(current.table[i])) ;
	    }
	}
      else
	{
	  NOTE_CHAMP_INVALIDE(o, expression) ;
	  p++ ;
	}

      switch(what2)
	{
	case 'c':
	  for(i=0;i<current.nb;i++)
	    current.table[i] = cos(current.table[i]) ;
	  break ;
	case 's':
	  for(i=0;i<current.nb;i++)
	    current.table[i] = sin(current.table[i]) ;
	  break ;
	case 'n':
	  {
	    double somme ;
	    somme = 0 ;
	    for(i=0;i<current.nb;i++)
	      somme += current.table[i] * current.table[i] ;
	    current.table[0] = sqrt(somme) ;
	    current.nb = 1 ;
	    break ;
	  }
	}
      what2 = ' ' ;

      if ( what == ' ' )
	{
	  c = codec_recupere_texte(codec_table_flottant, &current) ;
	  codec_sauve_texte(codec_table_flottant, &last, c) ;
	}
      else
      if ( what == '~' )
	{
	  if ( current.nb < 3 || last.nb < 3 )
	    continue ;

	  o->triplet = u_pv_triplet((Triplet*)last.table,
				    (Triplet*)current.table) ;
	  last.table[0] = o->triplet.x ;
	  last.table[1] = o->triplet.y ;
	  last.table[2] = o->triplet.z ;
	  last.nb = 3 ;	  
	}
      else
	for(i=0; i<last.nb; i++)
	  {
	    switch(what)
	      {
	      case '<': last.table[i] = (last.table[i] < current.table[i%current.nb]) ; break ;
	      case '>': last.table[i] = (last.table[i] > current.table[i%current.nb]) ; break ;
	      case 'L': last.table[i] = (last.table[i] <= current.table[i%current.nb]) ; break ;
	      case 'G': last.table[i] = (last.table[i] >= current.table[i%current.nb]) ; break ;
	      case '=': last.table[i] = (last.table[i] == current.table[i%current.nb]) ; break ;
	      case '+': last.table[i] += current.table[i%current.nb] ; break ;
	      case '-': last.table[i] -= current.table[i%current.nb] ; break ;
	      case '*': last.table[i] *= current.table[i%current.nb] ; break ;
	      case '/': last.table[i] /= current.table[i%current.nb] ; break ;
	      case '^': last.table[i]  =
		  pow(last.table[i], current.table[i%current.nb]) ; break ;
	      case '%': last.table[i]  =
		  fmod(last.table[i], current.table[i%current.nb]) ; break ;
	      }
	  }
    }
  c = codec_recupere_texte(codec_table_flottant, &last) ;
  codec_sauve_texte(codec_table_flottant, &o->resultat, c) ;
  o->nombre = o->resultat.table[0] ;
  o->triplet.x = o->resultat.table[0] ;
  o->triplet.y = o->resultat.table[1] ;
  o->triplet.z = o->resultat.table[2] ;

  codec_libere(codec_table_flottant, &current) ;
  codec_libere(codec_table_flottant, &last) ;
  
}

CLASSE(evaluateur, Evaluateur,
       BULLE_D_AIDE
       ("Syntaxe : liste d'opérations séparées par un espace\n"
	"Les opérations sont :\n"
	"  FLOTTANT  Un nombre\n"
	"  $NUMERO   La valeur du fils indiqué par le numéro\n"
	"  * / + - ^ L'opération\n"
	"  ^         Puissance\n"
	"  %         Modulo\n"
	"  ~         Produit vectoriel\n"
	"  cos sin norme  Fonction agissant sur ce qui suit\n"
	"  $0 $1 ... Accède à la valeur du fils 0, 1, ...\n"
	"  #0 #1 ... Idem mais prend prioritairement triplet/quedruplet\n"
	"  $2:3      Accède au 4ème flottant du 3ème fils\n"
	"Attention 1 + 3 / 2 donne 4 car 1+3 est fait puis après /2\n"
	)
       CHAMP(expression, L_chaine Edite Sauve InitChaine)
       CHAMP(resultat, L_table_nombre P_table_flottant Affiche)
       CHAMP(triplet, L_triplet P_triplet Affiche)
       CHAMP(nombre, L_nombre P_flottant Affiche)
       CHANGEMENT(changement_evaluateur)
       EVENEMENT("EV")       
       PAQUET(NB_ACTIFS)
       MENU("Valeurs/Evaluateur")
       )

