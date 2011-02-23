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
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "u_init_cls.h"
#include "o_objet.h"
#include "o_interface.h"
#include "t_base.h"
#include "s_string.h"

Classe *les_classes ;
int nombre_de_classes ;
Liste *champs_objet ;

/*
 * Interpreteur de définition de classe
 */

#define I(C,T) case CONCATENE(MC_,C): cl->C = (T)def[1] ; break
#define J(C,T) case CONCATENE(MC_,C): if (deuxieme) ch->C = (T)def[1] ; break
#define K(C,T) case CONCATENE(MC_,C): if (deuxieme) ev->C = (T)def[1] ; break
#define L(C,P) case CONCATENE(MC_,C): def-- ; if (deuxieme) { P } break

static void u_interprete(Classe *cl, Booleen deuxieme)
{
  const Mot_clef  *def, *tmp_def ;
  Champ     *ch, ch_sauve ;	/* Champ courant en train d'être défini */
  Booleen   dans_definition_champ ;
  int       i, j ;
  const Classe *tmp ;
  char *s ;
  static Chaine ev ;

  MET_A_0(&ch_sauve) ;

  if ( deuxieme && cl->sc )
    {
      /*
       * Ceci sera vrai un fois la hierarchie des classes initialisee
       * donc au deuxieme passage dans cette fonction.
       * Et qu'il y a une surclasse.
       *
       */
      tmp = cl->sc ;
      tmp_def = cl->definition ;
      *cl = *cl->sc ;
      cl->sc = tmp ;
      cl->definition = tmp_def ;
      cl->champs = NULL ; /* Pas de l_detruit : utilisé ailleur */
    }
  if ( deuxieme && cl->sc==NULL )
    {
      /*
       * Valeur par défaut, héritable et donc pas
       * dans la macro CLASSE()
       */
      cl->paquet = NB_NON_DEFINI ;
      cl->decalage = NB_NON_DEFINI ;
    }
  dans_definition_champ = Faux ;
  ch                    = &ch_sauve ;

  for( def=cl->definition;
       *def!=MC_fin_definition;
       def+=2)
    {
    switch(*def)
      {
	I(nom             , char*              				) ;
	I(Cstruct         , char*              				) ;
	I(taille_structure, int                				) ;
	I(destruction     , void (*)(void*)   				) ;
	I(destruction_nom , const char*   				) ;
	I(changement      , void (*)(void*)   				) ;
	I(changement_nom  , const char*   				) ;
	I(informations    , const char *(*)(const void*) 		) ;
	I(informations_nom, const char*   				) ;
	I(affiche_champs_classe, Booleen       				) ;
	I(paquet          , int                				) ;
	I(decalage        , int                				) ;
	I(menu            , const char*   				) ;
	I(fichier         , const char*   				) ;
	I(ligne           , int                				) ;

      case MC_surclasse: 
	cl->surclasse = (char *)def[1]  ;
	if ( deuxieme && cl->sc && strcmp(cl->sc->nom,cl->surclasse)==0  )
	  {
	    /*
	     * Insere les champs de la surclasse au bon endroit
	     */
	    i = ch_sauve.numero_fils ;
	    POUR(Champ*, champ, cl->sc->champs,
		 {
		   ALLOUER(ch,1) ;
		   *ch = *champ ;
		   ch->numero_fils += i ;
		   ch->classe = cl ;
		   l_ajoute_fin(&cl->champs, ch) ;
		   /* Remplacé le 4.3.2003
		   if ( ch->numero_fils > ch_sauve.numero_fils )
		     ch_sauve.numero_fils = ch->numero_fils ;
		   */
		   if ( ch->numero_fils >= ch_sauve.numero_fils
			&& ( EXTRAIT(ch) || OBLIGATOIRE(ch) )  )
		     ch_sauve.numero_fils = ch->numero_fils + 1 ;


		   if ( 0 )
		     if ( strcmp(cl->surclasse, "test_a") == 0 )
		       {
			 eprintf("\n\nAjoute champ %s\n", ch->nom_champ) ;
			 eprintf("num fils %d\n", ch->numero_fils) ;
			 eprintf("extrait %d\n", ch->options & O_Extrait) ;
		       }
		 }
		 );
	  }
	ch = &ch_sauve ;
	break ;

      case MC_evenement:
	s = (char*)def[1] ;
	j = 1 ;
	for(i=0;s[i];i++)
	  {
	    if ( !isdigit(s[i]) )
	      {
		j = 0 ;
		break ;
	      }
	    i++ ;
	    if ( s[i] == '\0' )
	      {
		j = 0 ;
	      }
	  }
	if ( j == 1 )
	  cl->evenement = s ;
	else
	  {
	    s_efface(&ev) ;
	    j = '0'
	      + (strstr(s,"Shft")!=NULL ? 1 : 0)
	      + (strstr(s,"Ctrl")!=NULL ? 2 : 0)
	      + (strstr(s,"Meta")!=NULL ? 4 : 0)
	      ;
	    i = strlen(s)-1 ;
	    while ( i>=0 && s[i]!='+' ) i-- ;
	    i++ ;
	    while( s[i] )
	      {
		s_printf(&ev, "%c%c", j, tolower(s[i])) ;
		i++ ;
	      }
	    cl->evenement = strdup(s_char(&ev)) ;
	  }
	break ;

      case MC_bulle_d_aide:
	s = (char*)def[1] ;
	i = strlen(s) ;
	if ( i>0 && s[i-1] == '\n' )
	  {
	    /*
	     * enlève les \n à la fin
	     */
	    s = strdup(s) ;
	    do
	      {
		i-- ;
		s[i] = '\0' ;		
	      }
	    while( i>0 && s[i-1]=='\n' ) ;
	  }
	if ( dans_definition_champ )
	  {
	    if (deuxieme) ch->bulle_d_aide = s ;
	  }
	else
	  cl->bulle_d_aide = s ;
	break ;
				 					
	J(label           , const char*        				) ;
	J(nom_champ       , const char*        				) ;
	J(Cnom_champ      , const char*        				) ;
        case MC_herite_pas_des_champs :
	  if ( deuxieme )
	    {
	      POUR(Champ*, dch, cl->champs, free(dch) ;) ;
	      l_detruit(&cl->champs) ;
	      ch_sauve.numero_fils = 0 ;	  
	    }
	  def-- ;
	  break;
        case MC_option     : if (deuxieme) ch->options |=  (int)def[1] ; break;
        case MC_non_option : if (deuxieme) ch->options &= ~(int)def[1] ; break;
        J(position        , int                                           ) ;
        J(taille          , int                                           ) ;
        J(defaut          , char*                                         ) ;
        J(numero_fils     , int                                           ) ;
        J(classe_parametre, char*                                         ) ;
	J(affichage       , Affichage        				  ) ;
        J(tp_lit          , const char*(*)(void*,const Champ*)            ) ;
        J(tp_lit_nom      , const char*                                   ) ;
        J(tp_ecrit        , void (*)(void*,const Champ*, const char*)     ) ;
        J(tp_ecrit_nom    , char*                                         ) ;
        J(te_texte        , const char*                                   ) ;
        J(te_largeur      , int                                           ) ;
        J(te_hauteur      , int                                           ) ;
        J(te_lit          , const char*(*)(const Interface_widget*)       ) ;
        J(te_lit_nom      , const char*                                   ) ;
        J(te_ecrit        , void (*)(const Interface_widget*)             ) ;
        J(te_ecrit_nom    , char*                                         ) ;
        J(te_clavier      , void (*)(const Interface_widget*)             ) ;
        J(te_clavier_nom  , char*                                         ) ;
        J(te_entree       , Booleen(*)(const Interface_widget*)           ) ;
        J(te_entree_nom   , char*                                         ) ;
        J(te_mouvement    , void(*)(const Interface_widget*)              ) ;
        J(te_mouvement_nom, char*                                         ) ;
        J(te_aide         , const char*(*)(const Interface_widget*)       ) ;
        J(te_aide_nom     , char*                                         ) ;
        J(te_expose      ,  void (*)(const Interface_widget*)             ) ; /* Obsolete */
        J(te_expose_nom  ,  char*                                         ) ; /* Obsolete */

      case MC_type_dans_fils:
	if ( deuxieme )
	  {
	    Champ tmp ;
	    ch->type_dans_fils = (const Type_logique *)def[1] ;
	    (*ch->type_dans_fils->initialisation)(&tmp) ;
	  }
	break ;
      case MC_tl:
	if ( deuxieme )
	  {
	    ch->tl = (const Type_logique*)def[1] ;
	    (*ch->tl->initialisation)(ch) ;
	  }
	break ;
      case MC_te:
	if ( deuxieme )
	  {
	    ch->te = (const Type_editeur*)def[1] ;
	    (*ch->te->initialisation)(ch) ;
	  }
	break ;
      case MC_tp:
	if ( deuxieme )
	  {
	    ch->tp = (const Codec_informations*)def[1] ;
	  }
	break ;
									
	L(debut_champ,
	  {
	    ALLOUER(ch,1) ;
	    *ch = ch_sauve ;
	    ch->classe = cl ;
	    dans_definition_champ = Vrai ;
	  }
	  ) ;

	L(fin_champ,
	  {
	    l_ajoute_fin(&cl->champs, ch) ;
	    if ( EXTRAIT(ch) || OBLIGATOIRE(ch) )
	      ch_sauve.numero_fils++ ;
	    if ( ch->type_dans_fils == NULL )
	      {
		ch->type_dans_fils = ch->tl ;
	      }
	    ch = &ch_sauve ;
	    dans_definition_champ = Faux ;
	      
	  }
	  ) ;

      case MC_fin_definition : break ;
      default:
	ICI ;
	eprintf("*def=%d classe=%s\n", *def, cl->nom) ;
	exit(1) ;
    }
    }
  i = 0 ;
  POUR(Champ*, champ, cl->champs,
       {
	 champ->index = i++ ;
       }
       ) ;
  if ( i>8*sizeof(Bits)-2 )
    eprintf("******************** Trop de champs dans %s (%d)\n", cl->nom, i) ;
}
/*
 * Décodage des événements clavier
 */
static const char *modificateur[] =
{
  ""     , "Shft+" , "Ctrl+" , "ShftCtrl+" ,
  "Meta+", "ShftMeta+" , "CtrlMeta+" , "ShftCtrlMeta+"
} ;

const char *decode_evenement(const char *e)
{
  const char *pc ;
  static Chaine s ;

  if ( *e == '\0' )
    return("") ;

  s_efface(&s) ;
  s_printf(&s, "%s\t", modificateur[*e-'0']) ;
 
  for(pc=e;*pc;)
    {
      if ( *pc != *e )
	return(e) ; /* Pas le même modificateur */
      pc++ ;
      if ( *pc == '\0' )
	return(e) ; /* Evénement invalide */
      s_putchar(&s, toupper(*pc)) ;
      pc++ ;
    }

  return(s_char(&s)) ;
}

/*
 * Création de la hiérarchie et reinterpretation des racines de classes
 */
static void u_creation_hierarchie()
{
  POUR_LES_CLASSES(cl,
		   {
		     cl->sc = (Classe*)char_vers_classe(cl->surclasse) ;
		   }
		   ) ;
}
/*
 * Re-interpretation des autres classes de la racine vers les feuilles.
 */
static void u_fait_heritage()
{
  int i ;
  Buffer tmp ;
  int u = 0 ;

  do
    {
      i = 0 ;
      POUR_LES_CLASSES(cl,
		      {
			if ( cl->index==-1 )
			  {
			    if ( cl->sc && cl->sc->index == -1 )
			      continue ;
			    u_interprete(cl, Vrai) ;
			    POUR(Champ*, ch, cl->champs,
				 {
				   ch->nom_champ = strdup(ch->nom_champ) ;
				   while( strchr(ch->nom_champ,'.') )
				     *((char*)strchr(ch->nom_champ,'.'))='_' ;
				   while( strchr(ch->nom_champ,'[') )
				     *((char*)strchr(ch->nom_champ,'['))='_' ;
				   while( strchr(ch->nom_champ,']') )
				     *((char*)strchr(ch->nom_champ,']'))='_' ;
				 }
				   ) ;
			    cl->index = i_cl ;

			    if ( cl->evenement==NULL )
			      {
				sprintf(tmp,"0n0d0%c", u++ + 'a') ;
				cl->evenement = strdup(tmp) ;
			      }
			    if ( cl->menu==NULL )
			      {
				sprintf(tmp,"NON_DÉFINI/%s", cl->nom) ;
				cl->menu = strdup(tmp) ;
			      }
			    i = 1 ;
			  }
		      }
		      ) ;
    }
  while(i) ;
}
/*
 * Allocation et première initialisation des classes
 */
static void u_allocation_classe(Mot_clef **dc)
{
  int id ;

  for( nombre_de_classes = 0 ; dc[nombre_de_classes] ; ) nombre_de_classes++ ;

  ALLOUER(les_classes, nombre_de_classes) ;
  memset(les_classes, '\0', sizeof(Classe)*nombre_de_classes) ;

  id = -1 ;
  POUR_LES_CLASSES
	(cl,
	 {
	   cl->definition = dc[i_cl] ;
	   cl->index = -1 ;
	   u_interprete(cl,Faux) ;
	   cl->id = id-- ;
	 }
	 ) ;
}
/*
 * Initialisation de la classe objet.
 */
static void u_init_classe_objet()
{
  int nb ;

  champs_objet = char_vers_classe("objet")->champs ;
  nb = 0 ;
  POUR_LES_CHAMPS_OBJET(ch,
			ch->position -= sizeof(Objet) ;
			if ( ch->index > nb )
			   nb = ch->index ;
			) ;
  nb++ ;
  bit_dernier_champ_objet = 1 << (nb-1) ;
  POUR_LES_CLASSES(c,
		   if ( c->champs != champs_objet )
		      POUR(Champ*, ch, c->champs, ch->index += nb ;) ;
		   ) ;
}
/*
 *
 */
void u_init_classes(int **dc)
{
  u_allocation_classe( (Mot_clef**)dc ) ;
  u_creation_hierarchie() ;
  u_fait_heritage() ;
  u_init_classe_objet() ;
}
