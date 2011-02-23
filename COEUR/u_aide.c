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
#include "o_objet.h"
#include "s_string.h"

const char *u_nom_classe(const Classe *c)
{
  static Chaine s ;

  s_efface(&s) ;
  for( ; c ; c=c->sc )
    {
      s_printf(&s, ":%s", c->nom) ;
    }
  return( s_char(&s)+1 ) ;
}

const char *u_nom_type_logique(const Type_logique *c)
{
  static Chaine s ;

  s_efface(&s) ;
  for( ; c ; c=c->surclasse )
    {
      s_printf(&s, ":%s", c->nom) ;
    }
  return( s_char(&s)+1 ) ;
}

const char *u_nom_type_editeur(const Type_editeur *c)
{
  static Chaine s ;

  s_efface(&s) ;
  for( ; c ; c=c->surclasse )
    {
      s_printf(&s, ":%s", c->nom) ;
    }
  return( s_char(&s)+1 ) ;
}



const char *u_aide_objet(const void *o)
{
  Objet *o_ = OBJET(o) ;
  static Chaine s ;

  if ( TRACE2(o) )
    eprintf("aide sur objet\n") ;
  s_efface(&s) ;
  s_printf(&s, "Objet de classe %s\n", u_nom_classe(o_->classe)) ;
  if ( TRACE2(o) )
    eprintf("ajoute bulle\n") ;
  if ( o_->classe->bulle_d_aide )
    s_strcat(&s, o_->classe->bulle_d_aide) ;
  if ( TRACE2(o) )
    eprintf("fin ajoute bulle\n") ;
  return( s_char(&s) ) ;
}


const char *u_aide_type_logique(const Type_logique *c)
{
  static Chaine s ;
  
  s_efface(&s) ;
  s_printf(&s, "Type logique : %s\n", u_nom_type_logique(c)) ;
  for( ; c ; c=c->surclasse )
    {
      if ( c->bulle_d_aide )
	{
	  s_printf(&s, "%s\n", c->bulle_d_aide) ;
	  break ;
	}
    }
  return( s_char(&s) ) ;
}

const char *u_aide_type_editeur(const Type_editeur *c)
{
  static Chaine s ;
  
  s_efface(&s) ;
  s_printf(&s, "Type éditeur : %s\n", u_nom_type_editeur(c)) ;
  for( ; c ; c=c->surclasse )
    {
      if ( c->bulle_d_aide )
	{
	  s_printf(&s, "%s\n", c->bulle_d_aide) ;
	  break ;
	}
    }
  return( s_char(&s) ) ;
}

const char *u_invalide(const void *o, const Champ *ch)
{
  static Chaine s ;
  const void *oo ;
  const Champ *c ;
  int invalide ;

  if ( TRACE2(o) )
    eprintf("aide sur invalide ?\n") ;
  invalide = 0 ;
  s_efface(&s) ;
  c = ch ;
  oo = o ;
  if ( !appartient(&oo, &c, ch->tl) )
    {
      s_printf(&s,
	       "CE CHAMP EST INVALIDE CAR IL\n"
	       "NE RESPECTE PAS LE PREDICAT (%s)\n"
	       "DE SON TYPE LOGIQUE\n\n"
	       , ch->tl->predicat_verifie_nom
	       ) ;
      invalide = 1 ;
    }

  if ( TRACE2(o) )
    eprintf("EXTRAIT(%s)= %d\n", ch->nom_champ, EXTRAIT(ch)) ;
  if ( EXTRAIT(ch) )
    {
      oo = FILS(o, ch->numero_fils) ;
      if ( TRACE2(o) )
	eprintf("oo= %p\n", oo) ;
      if ( oo )
	{
	  if ( OBJET(oo)->invalide )
	    {
	      s_printf(&s,
		       "CE CHAMP EST INVALIDE CAR IL\n"
		       "EST EXTRAIT D'UN OBJET INVALIDE\n\n"
		       ) ;
	      invalide = 1 ;
	    }
	  if ( TRACE2(o) )
	    eprintf("classe_parametre = %s\n", ch->classe_parametre) ;
	  if ( ch->classe_parametre
	       && strcmp( OBJET(oo)->classe->nom, ch->classe_parametre )!=0 )
	    {
	      s_printf(&s,
		       "CE CHAMP EST INVALIDE CAR IL\n"
		       "LA CLASSE DU PARAMETRE (%s) N'EST PAS\n"
		       "CELLE DEMANDEE : %s\n\n"
		       ,OBJET(oo)->classe->nom, ch->classe_parametre
		       ) ;
	      invalide = 1 ;
	    }
	  if ( (ch->classe_parametre==NULL || ch->classe_parametre[0]=='\0')
	       && !c_trouve_type(&oo, &c, ch->type_dans_fils) )
	    {
	      s_printf(&s,
		       "CE CHAMP EST INVALIDE CAR IL\n"
		       "ON NE TROUVE PAS LE TYPE LOGIQUE %s\n"
		       "DANS L'OBJET DE CLASSE : %s\n\n"
		       ,ch->type_dans_fils->nom, OBJET(oo)->classe->nom
		       ) ;
	      invalide = 1 ;
	    }
	}
      else
	{
	  if ( OBLIGATOIRE(ch) )
	    {
	      s_printf(&s,
		       "CE CHAMP EST INVALIDE CAR IL EST OBLIGATOIREMENT\n"
		       "EXTRAIT D'UN FILS (IL N'EXISTE PAS)\n\n"
		       ) ;
	      invalide = 1 ;
	    }
	}
    }

  if ( !invalide && OBJET(o)->invalide )
    {
      POUR_LES_CHAMPS(o, c,
		      {
			if ( CHAMP_INVALIDE_(OBJET(o),c) )
			  {
			    if ( invalide == 0 )
			      {
				s_printf(&s
					 , "OBJET INVALIDE CAR :\n"
					 ) ;
			      }
			    s_printf(&s, "Champ ``%s'' invalide\n"
				     , c->nom_champ) ;
			    invalide = 1 ;
			  }
		      }) ;
    }

  if ( invalide==0 && OBJET(o)->invalide )
    {
      s_printf(&s,
	       "INVALIDE CAR LA FONCTION ``%s''\n"
	       "de ``%s'' (ligne %d)\n"
	       "A NOTE CE CHAMP OU L'OBJET COMME INVALIDE\n"
	       , OBJET(o)->classe->changement_nom
	       , OBJET(o)->classe->fichier
	       , OBJET(o)->classe->ligne
	       ) ;
      invalide = 1 ;
    }

  if ( invalide )
    s_putchar(&s, '\n') ;

  
  return(s_char(&s)) ;
}

const char *u_aide_champ(const void *o, const Champ *ch)
{
  static Chaine s ;
  int i ;

  if ( TRACE2(o) )
    eprintf("aide sur champ\n") ;

  s_efface(&s) ;

  s_strcat(&s, u_invalide(o, ch)) ;
  s_printf(&s, "Champ %s : ", ch->nom_champ) ;
  s_strcat(&s, ch->bulle_d_aide ? ch->bulle_d_aide : "") ;
  s_strcat(&s, "\n\n") ;

  if ( TRACE2(o) )
    eprintf("aide sur champ (3d)\n") ;

  if ( INIT_3D(ch) )
    {
      i = 0 ;
      POUR_LES_CHAMPS_CLASSE
	(o, c,
	 {
	   if ( c == ch )
	     {
	       s_printf(&s,
			"La valeur du champ peut-être modifiée\n"
			"interactivement dans une fenêtre 3D en\n"
			"positionnant le curseur au bon endroit et\n"
			"en appuyant sur la touche '%c'\n\n"
			, '1'+i) ;
	       break ;
	     }
	   if ( INIT_3D(c) )
	     i++ ;
	 }
	 ) ;
    }

  if ( TRACE2(o) )
    eprintf("Finalise\n") ;
  
  s_strcat(&s, u_aide_type_logique(ch->tl)) ;
  s_putchar(&s, '\n') ;
  s_strcat(&s, u_aide_type_editeur(ch->te)) ;
  s_putchar(&s, '\n') ;
  s_printf(&s, "Codec (type physique) : %s\n\n", ch->tp->nom) ;

  s_strcat(&s, "C'est un champ de :\n") ;
  s_strcat(&s, u_aide_objet(o)) ;
  s_putchar(&s, '\n') ;

  if ( TRACE2(o) )
    eprintf("Fin aide sur champ\n") ;

  return( s_char(&s) ) ;
}
