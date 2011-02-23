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
#include "u_xt_interface.h"
#include "o_objet.h"
#include "s_string.h"
#include "u_repartiteur.h"
#include "u_modele.h"

/*

Remarques :

- La remontée d'information est prioritaire.
  Sinon l'initialisation échoue

- Quand un objet a changé il prévient tout ses parents.
  Sinon cela ne fonctionne pas en cas de réseau

- On boucle l'évaluation jusqu'a ce que rien ne change.
  Sinon les réseaux sont mal évalués

*/


extern Bits bit_dernier_champ_objet ;

#if 0

const char *indentation = "  +  -  |  /  *  &  $  #  @   a  b  c  d" ;
static int profondeur_totale = 0 ; /* STATICOK : revient toujours a 0 */
#define UP profondeur_totale++
#define DOWN profondeur_totale--
#define D(T) eprintf("%.*s%s(%p)(%s)[%s] %s",profondeur_totale,indentation, OBJET(o)->classe->nom,o,u_change(o),o_informations(o), T) ;
#define D2(T) eprintf("%.*s%s",profondeur_totale,indentation,T) ;
#define TEST(X) X
#define EPRINTF eprintf
#undef TRACE
#define TRACE(X) 1
#undef TRACE2
#define TRACE2(X) 1

#else

#define UP
#define DOWN
#define D(T)
#define D2(T)
#define TEST(X)
#define EPRINTF if(0) eprintf

#endif

static Liste *globale_crochet_changement = NULL ;

void ajoute_crochet_changement(void (*fct)(void *o))
{
  l_ajoute_si_pas_deja(&globale_crochet_changement, fct) ;
}


static void signature_objet(void *o, Chaine *s)
{
  s_efface(s) ;
  POUR_LES_CHAMPS_CLASSE
    (o, ch,
     if ( !NE_PAS_TESTER(ch) ) // 2007/02/22 Ajout test
       {
	 s_strcat(s, c_recupere_ici_sans_copie(o, ch)) ;
	 s_putchar(s, '\001') ;
       }
     ) ;
}

static void o_changement(void *o)
{
  Objet *o_ = OBJET(o) ;
  static Chaine s, t ;
  const void *oo ;
  const Champ *c ;

  o_->invalide = 0 ;
  POUR_LES_CHAMPS_(o_, ch,
		   {
		     oo = o ;
		     c = ch ;
		     if ( !appartient(&oo, &c, ch->tl) )
		       {
			 if ( TRACE_(o_) )
			   eprintf("%s : champ %s invalide\n",
				   o_->classe->nom, ch->nom_champ) ;
			 o_->invalide |= BIT_CHAMP(ch) ;
		       }
		     else
		       {
			 if ( EXTRAIT(ch) )
			   {
			     oo = FILS(o, ch->numero_fils) ;
			     if ( oo )
			       {
				 if ( OBJET(oo)->invalide )
				   {
				     if ( TRACE_(o_) )
				       eprintf("%s : champ %s invalide!\n",
					       o_->classe->nom,ch->nom_champ) ;
				     o_->invalide |= BIT_CHAMP(ch) ;
				   }
				 else
				   if ( ch->classe_parametre==NULL
					&& !c_trouve_type(&oo, &c,
						       ch->type_dans_fils) )
				     {
				       if ( TRACE_(o_) )
					 eprintf("%s : champ %s invalide!!\n"
						 ,o_->classe->nom
						 ,ch->nom_champ) ;
				       o_->invalide |= BIT_CHAMP(ch) ;
				     }
			       }
			     else
			       if ( OBLIGATOIRE(ch) )
				 o_->invalide |= BIT_CHAMP(ch) ;
			   }
		       }
		   }) ;
  /*
   * On fait une sauvegarde même pour les champs modifié que l'on
   * ne doit pas sauver. A cause du "point" par exemple.
   * On ne sauve pas le champ "point" mais "x", "y" et "z"
   */
  c_sauve( u_modele(o), "u_sauve_modele", "" ) ; /* METTRE OBLIGATOIRE "" */

  if ( o_->classe->changement )
    {
      if ( TRACE_(o_) )
	eprintf( "Changement objet=%p %s\n", o, u_change(o) );
      if ( !UN_CHAMP_CHANGE_(o_) )
	signature_objet(o, &s) ;
      (*o_->classe->changement)(o) ;
      if ( !UN_CHAMP_CHANGE_(o_) )
	{
	  signature_objet(o, &t) ;
	  if ( strcmp( s_char(&s), s_char(&t) ) )
	    {
	    NOTE_UN_CHAMP_CHANGE_(o_) ;
	    }
	}
    }

  /* XXX TOUS LES PARENTS */

  if ( UN_CHAMP_CHANGE_(o_) )
    {
      POUR( void *, fct, globale_crochet_changement,
	    (*(void (*)(void*))fct)(o) ; ) ;
      recupere_fenetre(o) ;
      D("On préviens les parents qu'un de leur FILS à changé :") ;
      POUR_LES_PARENTS_(o_,f,
			{
			  EPRINTF(" %x", f) ;
			  NOTE_FILS_CHANGE(f);
			} ) ;
      EPRINTF("\n");
    }
  else
    {
      D("On préviens les parents qu'un de leur DESCENDANT à changé :") ;
      POUR_LES_PARENTS_(o_,f,
			{
			  EPRINTF(" %x", f) ;
			  NOTE_DESCENDANT_CHANGE(f);
			}
			) ;
      EPRINTF("\n");
    }

  o_->creation = Faux ;
  o_->change = 0 ;
}

static Booleen editable(const void *o, const Champ *ch)
{
  const void *f ;
  const Champ *c ;

  if ( MODIFIABLE(ch) || !AFFICHE(ch) )
    {
      if ( EXTRAIT(ch) && ch->tp != &infos_codec_pointeur_extrait )
	{
	  f = FILS(o, ch->numero_fils) ;
	  if ( f )
	    {
	      if ( c_trouve_type(&f, &c, ch->type_dans_fils) )
		{
		  return( editable(f, c) ) ;
		}
	    }
	}
      return(Vrai) ;
    }
  return(Faux) ;
}

static void c_descend_valeur(void* o, const Champ *ch)
{
  const void *f ;
  char *v ;
  const Champ *c ;
  const void *o2 ;

  if ( EXTRAIT(ch)
       && ch->tp != &infos_codec_pointeur_extrait
       && !CHAMP_INVALIDE_(OBJET(o), ch)
       && ch->classe_parametre==NULL
       /*  && (BIT_CHAMP(ch) & OBJET(o)->change) */
       )
    {
      f = FILS(o,ch->numero_fils) ;
      if ( f )
	{
	  o2 = f ;
	  if ( c_trouve_type(&o2, &c, ch->type_dans_fils) )
	    {
	      if ( editable(o2, c) )
		{
		  v = strdup(c_recupere_ici_sans_copie(o,ch)) ;
		  if ( TRACE2(o) )
		    eprintf("Descend valeur car editable %s\n", v) ;
                  c_sauve_ici((void**)o2, c, v) ;
		  free(v) ;
		}
	    }
	}
    }
}

void c_monte_valeur(void* o, const Champ *ch)
{
  const void *f ;
  char *v ;
  const Champ *tmpc ;

  if ( EXTRAIT(ch) )
    {
      if ( TRACE2(o) )
	eprintf("Monte valeur '%s'\n", ch->nom_champ) ;
      f = FILS(o,ch->numero_fils) ;
      if ( f )
	{
	  /*
	   * Ce test n'est pas du tout joli
	   * Il faudrait faire une extension plus intelligente
	   * De la gestion des types
	   * Ajouter un parametre aux fonctions de decision ?
	   */
	  v = NULL ;
	  if ( TRACE2(o) )
	    eprintf("Type dans fils '%s'\n", ch->type_dans_fils->nom) ;
	  if ( c_trouve_type(&f, &tmpc, ch->type_dans_fils) )
	    {
	      if ( ch->classe_parametre )
		{
		  if ( TRACE2(o) )
		    eprintf("Classe parametre '%s' à extraire de %s\n",
			    ch->classe_parametre, o_informations(f));
		  if (strcmp( OBJET(f)->classe->nom, ch->classe_parametre)==0)
		    v = strdup(int_vers_char( (long)f )) ;
		}
	      else
		{
		  if ( ch->tp == &infos_codec_pointeur_extrait )
		    v = strdup(int_vers_char( (long)PTR(f,tmpc) )) ;
		  else
		    v = strdup(c_recupere_ici_sans_copie(f, tmpc)) ;
		}
	    }
	  if ( v )
	    {
	      if ( TRACE2(o) )
		eprintf("Valeur : '%s'\n", v) ;
	      c_sauve_ici(o, ch, v) ;	      
	      free(v) ;
	    }
	  else
	    {
	      /*
	       * On ne peut extraire le champ : donc invalide
	       */
	      /*	      OBJET(o)->invalide     |= BIT_CHAMP(ch) ;
	      OBJET(o)->change       |= BIT_CHAMP(ch) ; 
	      */
	    }
	}
    }
}
Booleen un_fils_change(void *o)
{
  /*
  if ( FILS_CHANGE(o) )
    return(Vrai) ;
  */
  POUR_LES_FILS(o,
		f,
		if ( OBJET(f)->change || CREATION(f) ) return(Vrai) ;
		) ;
  return(Faux) ;
}
/*
 *
 */
static int evaluer_reseau(void *o) ;

static void evaluer_liste(Liste *l)
{
  int change, i ;

  if ( l == NULL )
    return ;

  i = 0 ;
  do
    {
      change = Faux ;
      POUR(void*, f, l,
	   {
	     if ( f )
	       change |= evaluer_reseau(f) ;
	   }
	   ) ;
    }
  while(change && i++<1) ; // Une fois
}


static void remonte_vers_parents(void *o)
{
  POUR_LES_PARENTS(o, p,
		   {
		     D("On remonte les valeurs des fils\n") ;
		     POUR_LES_CHAMPS_CLASSE(p, ch,
					    if ( EXTRAIT(ch)
						 && FILS(p,ch->numero_fils)==o)
					      c_monte_valeur(p, ch) ;
					    ) ;
		   }) ;
}

/*
 * Retourne 0 si pas de changement
 */
static int evaluer_reseau(void *o)
{
  Objet *o_ = OBJET(o) ;
  int change ;

  // Ajout 2/2/2007
  if ( EST_OPERATEUR(o) )
    operateur_debut_eval(o) ;

  UP ;
  D("<<<<<<<<<<<<<<<<<<<<<<<<<<< Evaluer réseau\n") ;
  if ( o_->change )
    {
      if ( CREATION_(o_) || FILS_CHANGE_(o_) || DESCENDANT_CHANGE_(o_) || o_->fils == NULL )
	{
	  D("C'est une création ou fils change : on fait une remontée\n") ;
	}
      else
	{
	  D("L'objet a changé : Descente, APPEL CHANGEMENT\n") ;
	  o_changement(o) ;
	  D("On descend les champs extraits\n") ;
	  POUR_LES_CHAMPS_CLASSE_(o_, ch, c_descend_valeur(o, ch) ; ) ;
	  D("Évaluation récursive pour DESCENDRE\n") ;
	  evaluer_liste(o_->fils) ;
	  o_changement(o) ;
	  remonte_vers_parents(o) ;
	  DOWN ;

	  // Ajout 2/2/2007
	  if ( EST_OPERATEUR(o) )
	    operateur_fin_eval(o) ;
	  return(1) ;
	}
    }
  D("Boucle d'évaluation des fils\n") ;
  evaluer_liste(o_->fils) ;
  change = o_->change ;
  // XXX
  /*
  if ( EST_OPERATEUR(o) && l_longueur(((Modele*)o)->oper.liste->modeles) > 1 )
    NOTE_FILS_CHANGE(o) ;
  */
  if ( o_->change )
    {
      D("Changement de remonté\n") ;
      o_changement(o) ;
      remonte_vers_parents(o) ;
    }
  D(">>>>>>>>>>>>>>>>>>>>>>>>>>> Evaluer réseau\n") ;
  DOWN ;

  // Ajout 2/2/2007
  if ( EST_OPERATEUR(o) )
    operateur_fin_eval(o) ;


  return(change) ;
}

void o_appel_changement(void *o)
{
  int i ;

  UP ;
  // graphe_coherent(u_racine()) ;
  D("DEBUT o_appel_changement\n") ;
  for(i=0; i<1 && evaluer_reseau(o)  ; i++)
    {
    }
  D("FIN o_appel_changement\n") ;
  DOWN ;
}

void o_appel_changement_liste(Liste *l)
{
  UP ;
  D2("DEBUT o_appel_changement_liste\n") ;
  evaluer_liste(l) ;
  D2("FIN o_appel_changement_liste\n") ;
  DOWN ;
}

void o_appel_changement_sommet(void *o)
{
  o = u_racine(o) ;
  /* La ligne suivante est décommentée le 5/12/1999
   * Pour que l'exposition de fenêtre soit déclenchée sans que l'on considère
   * que le réseau soit modifié.
   */
  /*
  NOTE_UN_CHAMP_CHANGE(o) ;
  */
  o_appel_changement(o) ;
}

