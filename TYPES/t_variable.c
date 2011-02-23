/*
    NanoÉdit: Un modeleur algébrique interactif.
    Copyright (C) 1996-2005 Thierry EXCOFFIER, Université Claude Bernard, LIRIS

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
#include "u_fichier.h"
#include "s_string.h"
#include <string.h>
#include "u_etat_courant.h"
#include "u_modele.h"

static void *pile_sauvegarde_variables[10*MAX_VARIABLE] ;
static int nb_dans_pile_sauvegarde_variables = 0 ;

static int globale_prof_sauve = 0 ;

#if 0
#define EPRINTF eprintf
#else
#define EPRINTF if(0) eprintf
#endif

#define REALLY_NULL_POINTER ((void*)1)

/*
 ******************************************************************************
 * VARIABLE
 ******************************************************************************
 */

void *Variable_contenu(const void *o, const Champ *c)
{
  int i ;
  void *f ;

  i = atol( c_recupere_ici_sans_copie(o,c) ) ;
  if ( i<0 || i>=MAX_VARIABLE )
    f = NULL ;
  else
    f = globale_etat_courant.variables[i] ;
  if ( TRACE(o) )
    eprintf("f1=%p\n", f) ;
  if ( f == NULL )
      f = FILS(o,0) ;
  if ( f == REALLY_NULL_POINTER )
    f = NULL ;
  if ( TRACE(o) )
    eprintf("f2=%p\n", f) ;
  if ( f == o )
    EXIT ;
  return(f) ;
}

static Booleen sous_classe_variable(const void **o, const Champ **c,
				    const Type_logique *t)
{
  const void *f ;

  f = Variable_contenu(*o,*c) ;
  if ( f )
    if ( c_trouve_type(&f,c,t) )
      {
	if ( TRACE(*o) || TRACE(f) )
	  eprintf("trouve le type\n") ;
	*o = f ;
	return(Vrai) ;
      }
  if ( TRACE(*o) )
    eprintf("trouve pas le type\n") ;
  return(Faux) ;
}

DEFINITION_TYPE_LOGIQUE(L_variable, L_metamorphe,
			"Variable NanoÉdit",
			c->tp = &infos_codec_chaine ;
			SOUS_CLASSE(sous_classe_variable)
			)

/*
 ******************************************************************************
 * OPERATEUR (Pour le champs contenu)
 ******************************************************************************
 */

static void empile(void *o)
{
  if ( nb_dans_pile_sauvegarde_variables
       == TAILLE_TABLE(pile_sauvegarde_variables)
       )
    EXIT ;
  EPRINTF("%*sEmpile [%d] %p\n"
	  , globale_prof_sauve*8, "", nb_dans_pile_sauvegarde_variables, o) ;
  pile_sauvegarde_variables[nb_dans_pile_sauvegarde_variables++] = o ;
}

static void *depile()
{
  if ( nb_dans_pile_sauvegarde_variables <= 0 )
    EXIT ;
  EPRINTF("%*sDépile [%d] %p\n"
	  , globale_prof_sauve*8, "", nb_dans_pile_sauvegarde_variables-1,
	  pile_sauvegarde_variables[nb_dans_pile_sauvegarde_variables-1]) ;
  return( pile_sauvegarde_variables[--nb_dans_pile_sauvegarde_variables] ) ;
}

static Parcour u_detruit(void *o, void *data)
{
  detruire_objet(o) ;
  return(Continue) ;
}



void Variable_recupere(const void *o, const Champ *c)
{
  void *tmp ;

  globale_prof_sauve-- ;
  EPRINTF("%*sRecuperation des variables de %p\n", globale_prof_sauve*8, "",o) ;
  POUR_TOUS_LES_FILS(o, inutilisee,
		     {
		       tmp = depile() ;
		       EPRINTF("%*scourante[%d] = %p (%s) nouvelle = %p (%s)\n",
			       globale_prof_sauve*8, "",
			       i_inutilisee,
			       globale_etat_courant.variables[i_inutilisee],
			       globale_etat_courant.variables[i_inutilisee]
			       ? o_informations(globale_etat_courant.variables[i_inutilisee])
			       :"NULL",
			       tmp, /* nouvelle */
			       tmp ? o_informations(tmp) :"NULL"
			       ) ;
		       globale_etat_courant.variables[i_inutilisee] = tmp ;
		     }
		     ) ;
}

void Variable_sauve(const void *o, const Champ *c)
{
  const Champ *ch ;
  void *tmp[MAX_VARIABLE] ;
  const void *ff ;
  int nb ;

  EPRINTF("%*sSauvegarde des variables de %p\n", globale_prof_sauve*8, "", o) ;

  nb = 0 ;
  POUR_TOUS_LES_FILS(o, f,
  {
    ff = f ;
    if ( f && c_trouve_type(&ff, &ch, &infos_L_variable) )
      {
	tmp[i_f] = Variable_contenu(ff, ch) ;
      }
    else
      {
	tmp[i_f] = f ;
      }
    EPRINTF("%*scourante[%d] = %p (%s) nouvelle = %p (%s)\n",
	    globale_prof_sauve*8, "", i_f,
	    globale_etat_courant.variables[i_f],
	    globale_etat_courant.variables[i_f]
	    ? o_informations(globale_etat_courant.variables[i_f])
	    : "NULL",
	    tmp[i_f],
	    tmp[i_f]
	    ? o_informations(tmp[i_f])
	    : "NULL");
    nb++ ;
  }
		     ) ;
  
  /*
   * On empile dans l'ordre inverse, pour dépiler dans le bon sens
   */
  POUR_TOUS_LES_FILS(o, f,
  {
    empile(globale_etat_courant.variables[nb-1-i_f]) ;
  }
		     ) ;
  
  POUR_TOUS_LES_FILS(o, f,
  {
    if ( tmp[i_f] == NULL )
      tmp[i_f] = REALLY_NULL_POINTER ;
    globale_etat_courant.variables[i_f] = tmp[i_f] ;
  }
       ) ;
  globale_prof_sauve++ ;
}

Liste* Operateur_contenu(const void* o, const Champ *c )
{
  return ( CONTENU(&T(Operateur, o, c)) ) ;
}

#if 0
#undef TRACE
#define TRACE(X) 1
#endif

static Liste *first = NULL ;

static void u_libere_modele(Operateur *op)
{
  if ( op->liste /* && CONTENU(op) */ )
    {
      if ( TRACE(CODEC_OBJET) )
	eprintf("Détruit %s (%d parents) codec_objet=%x\n", NOMFICHIER(op), l_longueur(op->liste->modeles), CODEC_OBJET) ;

      /*
       * Pour que la fonction o_destruction fonctionne.
       * En effet la relation père /fils doit être symétrique
       */
      POUR_LES_ELEMENTS(CONTENU(op), e,
			l_enleve(&OBJET(e)->parent, CODEC_OBJET) ; ) ;

      l_enleve(&op->liste->modeles, CODEC_OBJET) ;
      if ( op->liste->modeles == NULL )
	{
	  if ( TRACE(CODEC_OBJET) )
	    eprintf("Détruit pour de bon %s\n", NOMFICHIER(op)) ;
	  o_parcour_liste(CONTENU(op), u_detruit, NULL, NULL) ;
	  l_detruit(&CONTENU(op)) ;
	  CODEC_FREE(NOMFICHIER(op)) ;
	  free(NOMCOMPLET(op)) ;
	  l_enleve(&first, op->liste) ;
	}
      op->liste = NULL ;
    }  
}

static int codec_operateur_libere(Operateur *op)
{
  u_libere_modele(op) ;
  if ( op->noms_variables )
    free(op->noms_variables) ;
  return(0) ;
}

static Parcour u_variable_list(void *o, void *data)
{
  Operateur *op = data ;
  const Champ *ch ;
  static Chaine c ;
  const char *nom_variable ;

  if ( TRACE(o) )
    eprintf("variable ?\n") ;
  ch = (Champ*)l_element(OBJET(o)->classe->champs,0) ;
  if ( ch->tl == &infos_L_variable )
    {
      nom_variable = c_recupere_ici_sans_copie(o, ch) ;
      if ( TRACE(o) ) eprintf( "Variable %s\n", nom_variable) ;
      s_efface(&c) ;
      s_printf(&c, "%s %s", op->noms_variables, nom_variable) ;
      free(op->noms_variables) ;
      op->noms_variables = strdup( s_char(&c) ) ;
    }
  NOTE_FILS_CHANGE(o) ;
  /*  ACTIF(o) = Faux ; */
  return(Continue) ;
}


void revert_modele(Operateur *op, const char* nom_fichier, int ouvre_fenetre)
{
  /* enlève parents */
  POUR_LES_ELEMENTS(CONTENU(op), e, l_detruit(&OBJET(e)->parent);) ;
  /* Destruction du modèle */
  o_parcour_liste(CONTENU(op), u_detruit, NULL, NULL) ;
  l_detruit(&CONTENU(op)) ;
  /* Charge contenu */
  nom_fichier = u_trouve_fichier(directories_objets, nom_fichier,
				 "/MODELES/", ".ne") ;
  CONTENU(op) = u_charge(nom_fichier, ouvre_fenetre) ;

  /* Met les parents (autant que d'opérateurs) */
  POUR_LES_ELEMENTS(CONTENU(op), e,
		    POUR_LES_ELEMENTS(op->liste->modeles, m,
				      {
					l_ajoute(&OBJET(e)->parent, m);
				      }
				      ) ;
		    ) ;      

}


int codec_operateur_recupere_texte(const Operateur *a)
{
  return codec_char_etoile_recupere_texte( (const char**) &NOMFICHIER(a) ) ;
}

int codec_operateur_sauve_texte(Operateur *op)
{
  const char *a ;
  Booleen cache ;

  codec_operateur_libere(op) ;
  ALLOUER(op->liste, 1) ;
  NOMFICHIER(op) = NULL ;
  op->liste->modeles = NULL ;

  codec_char_etoile_sauve_texte(&NOMFICHIER(op)) ;

  op->noms_variables = malloc(1) ;
  op->noms_variables[0] = '\0' ;

  if ( TRACE(CODEC_OBJET) )
    eprintf("CO=%x Demande à charger '%s'\n", CODEC_OBJET, NOMFICHIER(op)) ;

  cache = Faux ;
  POUR(Liste_Referenced*, li, first,
       {
	 if ( strcmp(NOMFICHIER(op), li->fichier) == 0 )
	   {
	     if ( TRACE(CODEC_OBJET) )
	       eprintf("Trouve dans le cache : %x\n", li) ;
	     free(NOMFICHIER(op)) ;
	     free(op->liste) ;
	     op->liste = li ;
	     if ( TRACE(CODEC_OBJET) )
	       eprintf("Ajoute liste des modeles : %x\n", CODEC_OBJET) ;
	     l_ajoute(&li->modeles, CODEC_OBJET) ;
	     cache = Vrai ;
	     break ;
	   }
       }) ;


  if ( TRACE(CODEC_OBJET) )
    eprintf("CO=%x Dans le cache = %d\n", CODEC_OBJET, cache) ;

  if ( ! cache )
    {
      a = u_trouve_fichier(directories_objets, NOMFICHIER(op),
			   "/MODELES/", ".ne") ;
      if ( TRACE(CODEC_OBJET) )
	eprintf("nom_complet = %s nom_fichier = %s\n", a, NOMFICHIER(op)) ;

      if ( TRACE(CODEC_OBJET) )
	eprintf("CO=%x Debut charge, environ traduc = %d\n", CODEC_OBJET,
		globale_change_environnement_traduction_n) ;

      if ( a )
	{
	  if ( strncmp(a, "./MODELES/", 10) == 0 )
	    NOMCOMPLET(op) = strdup(a+10) ;
	  else if ( strncmp(a, "./", 2) == 0 )
	    NOMCOMPLET(op) = strdup(a+2) ;
	  else
	    NOMCOMPLET(op) = strdup(a) ;

	  if ( NOMCOMPLET(op)[strlen(NOMCOMPLET(op))-3] == '.' )
	    NOMCOMPLET(op)[strlen(NOMCOMPLET(op))-3] = '\0' ;
	  
	}
      else
	NOMCOMPLET(op) = strdup("") ;

      if ( a )
	{
	  if ( EST_OPERATEUR(CODEC_OBJET) )
	    CONTENU(op) = u_charge(a, Faux) ;
	  else
	    CONTENU(op) = u_charge(a, Vrai) ;
	}
      else
	CONTENU(op) = NULL ;

      if ( TRACE(CODEC_OBJET) )
	eprintf("CO=%x Fin charge, environ traduc = %d\n", CODEC_OBJET,
		globale_change_environnement_traduction_n) ;


      l_ajoute(&first, op->liste) ;
      if ( TRACE(CODEC_OBJET) )
	eprintf("Ajoute liste des modeles : %x\n", CODEC_OBJET) ;
      l_ajoute(&op->liste->modeles, CODEC_OBJET) ;

      if ( a == NULL )
	{
	  return(1) ;
	}
    }

  if ( TRACE(CODEC_OBJET) )
    eprintf("contenu = %p\n", CONTENU(op)) ;

  /*
   * Pour recalculer ce qui depend de variables si changement
   */
  if ( CONTENU(op) )
    {
      o_parcour_liste(CONTENU(op), NULL, u_variable_list, op) ;
      /* 24/03/2000 */
      
      POUR_LES_ELEMENTS(CONTENU(op), l,
			l_ajoute(&OBJET(l)->parent,CODEC_OBJET);
			) ;      
    }

  return(0) ;
}


static Booleen sous_classe_operateur(const void **o,
				     const Champ **c,
				     const Type_logique *t)
{
  Liste *l ;
  const Champ *cc ;

  l = Operateur_contenu(*o,*c) ;
  if ( TRACE2(*o) )
    eprintf("sousclasse operateur en %s\n", t->nom) ;
  if ( l )
    {
      // f = l_element(l,0) ;
      Variable_sauve(*o, *c) ;

      /* CECI RALENTI LES CALCULS MAIS ASSURE QUE LES OPERATEURS
	 SONT RECALCULE A CHAQUE FOIS.
	 Sinon on a un probleme dans le test : operateur_double_test.ne
      */
      
      if ( l_longueur(T(Operateur, *o,*c).liste->modeles) != 1 )
	{
	  o_parcour_liste(l, variable_change, NULL, (void*)*o) ;
	  o_appel_changement_liste(l) ;
	}
      
      POUR(const void*, f, l,
	{
	  if ( f == NULL ) // 7/1/2005
	    continue ;
	  if ( c_trouve_type(&f,&cc,t) ) /* 31/1/2000, "c" --> "&cc" */
	    {
	      if ( TRACE2(*o) )
		eprintf("Trouve dans les fils\n") ;
	      Variable_recupere(*o, *c) ;
	      *o = f ;
	      *c = cc ;		/* 31/1/2000, ajout */
	      if ( TRACE2(*o) )
		{
		  eprintf("Trouve dans les fils : %s",o_informations(*o));
		  eprintf(" champ : %s\n",c_recupere_type_sans_copie(*o, t)) ;
		}
	      return(Vrai) ;
	    }
	}
	   ) ;
      Variable_recupere(*o, *c) ;
    }
  else
    NOTE_CHAMP_INVALIDE_(*o, *c) ;

  if ( TRACE2(*o) )
    eprintf("NE Trouve PAS dans les fils\n") ;
  return(Faux) ;
}

TYPE_DE_BASE(codec_operateur, Operateur,
	     codec_operateur_recupere_texte,
	     codec_operateur_sauve_texte,
	     codec_operateur_libere,
	     codec_rien
	     )

DEFINITION_TYPE_LOGIQUE(L_operateur, L_metamorphe,
			"Opérateur NanoÉdit",
			c->tp = &infos_codec_operateur ;
			SOUS_CLASSE(sous_classe_operateur)
			)


