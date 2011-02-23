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
#include <string.h>		/* Pour free */
#include "o_objet.h"
#include "t_base.h"

/*
 * Attention :
 * L'utilisation de prédicats n'est pas empilable.
 * On ne peut vérifier qu'un seul prédicat.
 * Sinon, c'est vraiment compliqué de faire le cache de type.
 */

const char *type_vers_char(const Type_logique* t)
{
  return(t->nom) ;
}

/*
 * Trouve le numero du champ à partir de son pointeur
 * C'est pour les macros CHAMP_INVALIDE, INVALIDATION_CHAMP, CHAMP_CHANGE, ...
 */

Champ* c_trouve_champ(const void *o, const char *position, int taille)
{
  Objet *o_ = OBJET(o) ;

  POUR_LES_CHAMPS_CLASSE_(o_,ch,
			 {
			   if ( PTR_(o_,ch) == position
				&& ch->taille == taille )
			     return(ch) ;
			 } ) ;
  POUR_LES_CHAMPS_OBJET(ch,
			 {
			   if ( PTR_(o_,ch) == position
				&& ch->taille == taille )
			     return(ch) ;
			 } ) ;

  eprintf("---------------------------------- CHAMP INTROUVABLE\n"
	  "Vous cherchez un champ de %d octets à la position %d dans :\n"
	  ,
	  taille, position-(char*)o
	  ) ;
  POUR_LES_CHAMPS_CLASSE_(o_,ch,
			 {
			   eprintf("%40s position=%3d taille=%3d\n",
				   ch->nom_champ, PTR_(o_,ch)-(char*)o, ch->taille) ;
			 } ) ;
  POUR_LES_CHAMPS_OBJET(ch,
			 {
			   eprintf("%40s position=%3d taille=%3d\n",
				   ch->nom_champ, PTR_(o_,ch)-(char*)o, ch->taille) ;
			 } ) ;
  abort() ;
  return(0) ;
}

/*
 * Appartenance a un type logique
 */

Booleen tl_inclus(const Type_logique *a,
		 const Type_logique *b)
{
  if ( b == NULL ) return(Vrai) ;
  for(;;)
    {
      if ( a == NULL ) return(Faux) ;
      if ( a == b    ) return(Vrai) ;
      a = a->surclasse ;
    }
}

Booleen appartient(const void **o, const Champ **ch, const Type_logique *t)
{
  if ( tl_inclus((*ch)->tl,t) )
    {
      if ( t->predicat_verifie ) /* On verifie si les calculs sont exactes */
	{			/* Ceci est normalement inutile */
	  return( (*t->predicat_verifie)(*o,*ch) ) ;
	}
      return(Vrai) ;
    }
  
  if ( t->predicat_verifie )
    {
      /*
       * Ligne mise en commentaire 15/6/1999 : seul le predicat compte
       *  pas la position dans la hierarchie
       */
      /* Je remet le "if" le 15/01/2001
       * car cela empêche les optimisations du cache de type.
       * Mais on rmeplace :
       *     t->surclasse == NULL || appartient(o, ch, t->surclasse)
       * par :
       *     
       */
      if ( tl_inclus((*ch)->tl, t->surclasse) )
	if ( (*t->predicat_verifie)(*o,*ch) )
	  return(Vrai) ;
    }
  if ( (*ch)->tl->sous_classe )
    if ( (*(*ch)->tl->sous_classe)(o, ch, t) )
      return(Vrai) ;

  return(Faux) ;
}


/*
 * Trouve le champs a partir de son nom : c'est un acces directe
 */

const Champ * c_trouve(const void* o, const char *nom_champ )
{
  POUR_TOUS_LES_CHAMPS( o, ch,
		   if ( strcmp( ch->nom_champ, nom_champ ) == 0 ) return(ch) ;
		   ) ;
  return(NULL) ;
}

/*
 * Acces pour l'extraction
 */

Booleen c_trouve_type1(const void **o, const Champ **champ,
		      const Type_logique *t) 
{
  Objet *o_ = OBJET(*o) ;

  /*  if ( *objet == NULL ) return(Faux) ; */

  POUR_LES_CHAMPS_CLASSE_
    (o_, ch,
     if ( appartient(o,(const Champ **)&ch,t) ) 
     {
       *champ = ch ;
       return(Vrai) ;
     }
     ) ;
  return(Faux) ;
}

static struct ht
  {
    const Classe *cl ;
    const Champ *ch ;
    const Type_logique *t ;
    int nb_acces ;
    Booleen cachable ;
    Booleen existe ;
    struct ht *suivant ;    
} *globale_table[1009] = { NULL } ;

void c_dump_trouve_type_table(FILE *f)
{
  int i ;
  struct ht *pht ;

  fprintf(f, "<SMALL><PRE>") ;
  for(i=0;i<TAILLE_TABLE(globale_table);i++)
    {
      if ( globale_table[i] )
	fprintf(f, "%5d\n",i) ;
      for(pht=globale_table[i]; pht; pht = pht->suivant)
	{
	  fprintf(f
		  , "%25s %-35s %9d --> %s\n"
		  , pht->cl->nom
		  , pht->t->nom
		  , pht->nb_acces
		  , pht->cachable ? pht->ch->nom_champ :
		  ( pht->existe ? "<B>Non Cachable</B>" : "Existe Pas")
		  ) ;
	}      
    }
  fprintf(f, "</PRE></SMALL>") ;
}

Booleen c_trouve_type(const void **o, const Champ **champ,
		       const Type_logique *t) 
{
  struct ht *pht ;
  int indice ;
  Objet *o_ = OBJET(*o) ;
  Booleen f ;

  if ( t->predicat_verifie )
    return( c_trouve_type1(o, champ, t) ) ;

  indice = ((long)o_->classe ^ (long)t) % TAILLE_TABLE(globale_table) ;
  pht = globale_table[indice] ;
  while( pht )
    {
      if ( pht->cl == o_->classe && pht->t == t )
	break ;
      pht = pht->suivant ;
    }
  if ( pht == NULL )
    {
      ALLOUER(pht, 1) ;
      pht->cl       = o_->classe    ;
      pht->t        = t             ;
      pht->suivant  = globale_table[indice] ;
      pht->ch       = NULL          ;
      pht->nb_acces = 0             ;
      pht->cachable = Vrai ;
      pht->existe   = Faux ;
      POUR_LES_CHAMPS_CLASSE_
	(o_, ch,
	 /*
	  * On n'optimise plus après la première sous-classe
	  */
	 if ( ch->tl->sous_classe )
	{
	  pht->existe   = Vrai ;
	  pht->cachable = Faux ;
	  pht->ch = ch ;
	  break ;
	}
	 if ( tl_inclus(ch->tl,t) )
	{
	  pht->existe   = Vrai ;
	  pht->ch = ch ;
	  break ;
	}
	 if ( t->predicat_verifie && tl_inclus(ch->tl, t->surclasse ) )
	{
	  pht->existe   = Vrai ;
	  pht->ch = ch ;
	  break ;
	}
	 ) ;
      if ( pht->ch == NULL )
	pht->cachable = Faux ;
      globale_table[indice] = pht ;
    }
  pht->nb_acces++ ;
  if ( pht->cachable )
    {
      *champ = pht->ch ;
      if ( t->predicat_verifie ) /* On verifie si les calculs sont exactes */
	{			/* Ceci est normalement inutile */
	  return( (*t->predicat_verifie)(o,pht->ch) ) ;
	}
      return(Vrai) ;
    }

  if ( pht->existe )
    {
      f = Faux ;
      POUR_LES_CHAMPS_CLASSE_
	(o_, ch,
	 if ( ch == pht->ch )
	 f = Vrai ;
	 if ( f )
	 if ( appartient(o,(const Champ **)&ch,t) ) 
	{
	  *champ = ch ;
	  return(Vrai) ;
	}
	 ) ;
    }
  return(Faux) ;
}

Booleen c_type_existe(const void* o, const Type_logique *t)
{
  const Champ *ch ;
  return( c_trouve_type(&o, &ch, t) ) ;
}

const Champ* c_type_exacte_existe(const void* o, const Type_logique *t)
{
  POUR_LES_CHAMPS(o, ch, if ( ch->tl == t ) return(ch) ; ) ;
  return(NULL) ;
}
/*
 *
 */

const char *c_recupere_type_sans_copie(const void* o, const Type_logique *t)
{
  const Champ *c ;

  if ( c_trouve_type(&o, &c, t) )
    return(c_recupere_ici_sans_copie(o, c)) ;
  else
    {
      return("") ;
    }
}

void c_sauve_type(void* o, const Type_logique *t, const char *valeur)
{
  const Champ *ch ;
  if ( c_trouve_type((const void **)&o, &ch, t) )
    c_sauve_ici(o, ch, valeur) ;
}

/*
 * Recuperation de la valeur
 */

const char * c_recupere_ici_sans_copie( const void* o, const Champ *ch)
{
  const char *c ;

  codec_debut_recupere_texte() ;
  CODEC_OBJET = (void*)o ;
  CODEC_CHAMP = (void*)ch ;
  (*ch->tp->recupere_texte)(PTR((Objet*)o, ch)) ;
  c = codec_fin_recupere_texte() ;
  return(c) ;
}

const char * c_recupere_sans_copie( const void* o, const char *nom_champ )
{
  const Champ *ch ;
  ch = c_trouve((Objet*)o, nom_champ) ;
  if ( ch )
    return(c_recupere_ici_sans_copie(o, ch)) ;
  else
    return("") ;
}

/*
 * Sauvegarde de valeur
 */

void c_sauve_ici(void* o, const Champ *ch, const char *valeur)
{
  const char *ancienne ;
  char *tmp ;
  Objet *o_ ;

  o_ = OBJET(o) ;
  if ( o_->creation || VIRTUEL(ch) )
    {
      ancienne = NULL ;
      if ( TRACE2_(o_) )
	eprintf("%s:=(%s)\n", ch->nom_champ, valeur) ;
    }
  else
    {
      ancienne = c_recupere_ici_sans_copie(o, ch) ;
      if ( TRACE2_(o_) )
	eprintf("%s ancienne=(%s) nouvelle=(%s)\n",
		ch->nom_champ, ancienne, valeur) ;
      if ( strcmp(ancienne, valeur) == 0 )
	return ;
    }

  codec_debut_sauve_texte(valeur) ;
  CODEC_OBJET = (void*)o ;
  CODEC_CHAMP = (void*)ch ;
  (*ch->tp->sauve_texte)(PTR((Objet*)o, ch), valeur) ;
  codec_fin_sauve_texte() ;

  if ( TRACE2_(o_) )
    eprintf("APRES SAUVEGARDE %s:=(%s)\n", ch->nom_champ,
	    c_recupere_ici_sans_copie(o, ch) ) ;

  /*
   * Si ce qui suit est toujours executé, cela veut dire
   * que l'affichage modifie l'objet, il y a donc + d'évaluation
   * dans u_eval.c (Modele exemple texte(matrice))
   *
   * J'ai fait en sorte que cela soit toujours exécuté le
   * 7/7/1999 pour une raison inconnue
   *
   * Le 16/7/1999 Je n'exécute que si cela n'est pas virtuel.
   * Les tests fonctionnent toujours.
   *
   * Le 4/2/2000 Finalement, il faut que cela soit exécuter
   * quand c'est virtuel : par exemple pour les TAGS.
   * Tout ceci n'est pas claire et necessite une reflexion sur l'évaluation
   * des champs virtuels.
   * JE CONSIDERE QUE SI LE CHAMP VIRTUEL A UNE FONCTION DE LECTURE,
   * ELLE RETOURNE LA DERNIERE VALEUR ECRITES.
   * Le teste est (Codec*)ch->tp_lit != NULL pour savoir s'il y a une
   * fonction de lecture virtuelle, (voir codec_virtuel_recupere_texte)
   *
   * 27/12/2000 Le teste était :
   *    !VIRTUEL(ch) || (Codec*)ch->tp_lit != NULL
   * Il devient
   *    !VIRTUEL(ch)
   * On ne teste donc plus les champs virtuels.
   * En fait, l'option Virtuelle devrait s'appeler ``Immutable''
   * (Pas de bit changement nécessaire)
   * J'enlève l'option Virtuel des TAG.
   *
   */
  if ( !VIRTUEL(ch) )
    {
      if ( ancienne )
	{
	  tmp = strdup(ancienne) ;
	  if ( TRACE2_(o_) )
	    eprintf("Relecture nouvelle = (%s)\n",
		    c_recupere_ici_sans_copie(o, ch));

	  if ( strcmp(c_recupere_ici_sans_copie(o, ch), tmp) )
	    {
	      o_->change |= BIT_CHAMP(ch) ;
	      /*
	       * Ajouté le 6/12/2006 pour que l'opérateur soit considéré
	       * comme modifié si le changement passe par son père.
	       * En effet, l'écriture via une variable n'indique pas
	       * que l'opérateur est modifié et la fonction 'changement'
	       * ne s'en rend pas compte car elle ne descend pas
	       * dans l'opérateur.
	       */
	      POUR(void*, i, u_operateur_ou_modele_liste(o),
		   {
		     NOTE_DESCENDANT_CHANGE(i) ;
		   }) ;
	    }
	  free(tmp) ;
	}
      else
	o_->change |= BIT_CHAMP(ch) ;
    }
}

void c_sauve(void* o, const char *nom_champ, const char *valeur)
{
  const Champ *ch ;
  ch = c_trouve(o, nom_champ) ;
  if ( ch )
    c_sauve_ici(o, ch, valeur) ;
  else
    {
      /*
       * else ajouté le 24/12/2000 pour "Exemples/modele_menu"
       */
      /*
       * C'est peut être le nom d'un champ accessible
       * via une variable ou un opérateur
       */
      POUR_LES_CHAMPS(o, c,
		      if ( tl_inclus(c->tl, &infos_L_metamorphe) )
      {
	(*c->tl->sous_classe)((const void**)&o, (const Champ**)&c
			      , &infos_L_void) ;
	c_sauve(o, nom_champ, valeur) ;
	return ;
      }
		      ) ;
      eprintf("Sauvegarde de ``%s'' dans le champ ``%s''\n"
	      "Ce le nom n'existe pas dans l'objet :\n"
	      , valeur
	      , nom_champ
	      ) ;
      o_dump(o) ;
      ICI ;
    }
}


