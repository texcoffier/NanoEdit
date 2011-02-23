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
#include "u_modele.h"

void modele_ajoute_fils(Modele *modele, void *fils)
{
  if ( OBJET(fils)->parent != NULL )
    EXIT ;

  l_ajoute_fin(&CONTENU(&modele->oper), fils) ;

  POUR(Modele*, m , modele->oper.liste->modeles,
       l_ajoute(&OBJET(fils)->parent, m) ; /* NON SYMETRIQUE */
       ) ;
}

Liste **contenu_modele_ptr(void *modele)
{
  const Champ *oper ;

  if ( c_trouve_type((const void**)&modele, &oper, &infos_L_operateur) )
    return &CONTENU(&T(Operateur, (Modele*)modele, oper)) ;

  abort() ;
}

Liste *contenu_modele(void *modele)
{
  return *contenu_modele_ptr(modele) ;
}

void operateur_debut_eval(void *o)
{
  const Champ *oper ;

  if ( c_trouve_type((const void**)&o, &oper, &infos_L_operateur) )  
    Variable_sauve(o, oper) ;
}

void operateur_fin_eval(void *o)
{
  const Champ *oper ;

  if ( c_trouve_type((const void**)&o, &oper, &infos_L_operateur) )  
    Variable_recupere(o, oper) ;
}

void *duplique_objet_parmi_d_autres(void *o)
{
  Objet *o_ = OBJET(o) ;
  void *nouveau ;

  if ( o == NULL )
    return NULL ;

  o_ = OBJET(o) ;
  if ( o_->flag5 )
    return(o_->nom) ;
  
  o_->flag5 = Vrai ;
  nouveau = o_objet(char_vers_classe(o_->classe->nom)) ;
  o_->nom = nouveau ;

  POUR_TOUS_LES_FILS_
    (o_, f, o_il_devient_mon_fils(nouveau, duplique_objet_parmi_d_autres(f), Faux) ; )

  POUR_TOUS_LES_CHAMPS_
    (o_, ch,
     {
       if ( strcmp(ch->nom_champ, "classe") == 0 )
	    continue ;
       if ( strcmp(ch->nom_champ, "fils") == 0 )
	    continue ;
       if ( strcmp(ch->nom_champ, "actif") == 0 )
	    continue ;
       if ( strcmp(ch->nom_champ, "edite") == 0 )
	    continue ;
       if ( strcmp(ch->nom_champ, "nom") == 0 )
	    continue ;
       if ( SAUVABLE_(o_, ch) )
	 {
	   c_sauve(nouveau, ch->nom_champ, c_recupere_ici_sans_copie(o, ch)) ;
	 }
     }
     ) ;

  return(nouveau) ;
}

void *duplique_objet(void *o)
{
  void *nouveau ;
  o_annule_flag5(u_racine(m)) ;
  nouveau = duplique_objet_parmi_d_autres(o) ;
  return( nouveau ) ;
}

Parcour variable_change(void *o, void *operateur)
{
  if ( TRACE(operateur) )
    eprintf("variable ?\n") ;
  if ( ((Champ*)l_element(OBJET(o)->classe->champs,0))->tl
       == &infos_L_variable )
    {
      if ( TRACE(operateur) )
	eprintf("OUI\n") ;
      NOTE_FILS_CHANGE(o) ;
    }
  return(Continue) ;
}



