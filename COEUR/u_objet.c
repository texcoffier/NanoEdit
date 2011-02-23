
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
#include "t_geometrie.h"
#include "u_fichier.h"
#include "u_repartiteur.h"
#include "u_memoire.h"

Gestion_memoire globale_gm_objet = { 0 } ;

Bits bit_dernier_champ_objet ;

void o_dump(void *objet)
{
  POUR_TOUS_LES_CHAMPS(objet,ch,
			{
			  eprintf("%30s = %s\n",
				  ch->nom_champ,
				  c_recupere_ici_sans_copie(objet, ch)
				  );
			}) ;
}

/*
 *
 */
const char *u_change(void *o)
{
  static Chaine s ;
  Objet *o_ = OBJET(o) ;

  s_efface(&s) ;
  s_printf(&s,"%x", o_->change) ;
  if ( CREATION_(o_) )        s_strcat(&s, " CREATION") ;
  if ( CA_CHANGE_(o_) )        s_strcat(&s, " CA_CHANGE") ;
  if ( FILS_CHANGE_(o_) )        s_strcat(&s, " FILS_CHANGE") ;
  if ( DESCENDANT_CHANGE_(o_) )        s_strcat(&s, " DESCENDANT_CHANGE") ;
  if ( UN_CHAMP_CHANGE_(o_) )
    {
      POUR_TOUS_LES_CHAMPS_(o_,ch,
			    {
			      if ( CHAMP_CHANGE_(o_, ch) )
				s_printf(&s, " %s", ch->nom_champ) ;
			    }
			    ) ;
    }
  return( s_char(&s) ) ;
}

/*
 ******************************************************************************
 * Les appels de méthode
 ******************************************************************************
 */

static Liste *globale_crochet_destruction = NULL ;

void ajoute_crochet_destruction(void (*fct)(void *o))
{
  l_ajoute_si_pas_deja(&globale_crochet_destruction, fct) ;
}

void o_destruction(void* o)
{
  Objet *o_ = OBJET(o) ;
  void *sauve ;

  if ( TRACE2_(o_) )
    eprintf( "Destruction objet=%p\n", o) ;

  if ( o_->classe->destruction )
    {
      if ( TRACE_(o_) )
	eprintf( "Destruction objet=%p\n", o) ;	      
      (*o_->classe->destruction)(o) ;
    }

  POUR( void *, fct, globale_crochet_destruction,
	(*(void (*)(void*))fct)(o) ; ) ;

  POUR_LES_FILS_(o_, fils, l_enleve( &OBJET(fils)->parent, o) ; ) ;
  /*
   * On le fait après à cause de la destruction de L_operateur 
   * qui rétabli la symétrie pere/fils
   */
  POUR_LES_PARENTS_(o_,p,
		    if ( TRACE_(o_) )
		      eprintf("Son père le perd : %s\n"
			      , OBJET(p)->classe->nom);
		    l_remplace(OBJET(p)->fils, o, NULL) ;
		    NOTE_FILS_CHANGE(p) ;
		   ) ;

  codec_debut_libere() ;
  sauve = CODEC_OBJET ;
  CODEC_OBJET = (void*)o ;
  POUR_LES_CHAMPS_(o_, ch, (*ch->tp->libere)(PTR_(o_,ch)) ; ) ;
  codec_fin_libere() ;
  CODEC_OBJET = sauve ;

  l_detruit(&o_->parent) ; /* Pas utile si fait dans le bon ordre */

  u_enleve_donnee(o) ;		/* Enlève tout le thread */
  libere_zone_memoire(&globale_gm_objet, o_) ;
}

void o_destruction_remonte(void *o)
{
  Objet *o_ ;
  const Champ *oper ;

  o_ = OBJET(o) ;
  POUR_TOUS_LES_PARENTS_
    (o_, p,
     {
       NOTE_FILS_CHANGE(p) ;
       if ( !l_trouve(OBJET(p)->fils, o) )
	 {
	   oper = c_type_exacte_existe(p, &infos_L_operateur) ;
	   POUR_LES_FILS_(o_, f,
			  {
			    if ( l_longueur(OBJET(f)->parent) == 1 )
			      {
				l_ajoute( &CONTENU(&T(Operateur, p, oper)),
					  f) ;
				l_remplace(OBJET(f)->parent, o, p) ;
			      }
			    else
			      l_enleve(&OBJET(f)->parent, o) ;
			  }
			  ) ;
	   l_remplace(CONTENU(&T(Operateur, p, oper)), o, NULL) ;
	   break ; // Une fois suffit car liste commune
	 }
       else
	 {
	   o_ils_deviennent_mes_fils(p, o_->fils, Faux) ;
	   l_remplace(OBJET(p)->fils, o, NULL) ;
	 }
     }
     );
  l_detruit(&o_->parent) ;
  o_destruction(o) ;
}

const char *o_informations(const void* o)
{
  const Champ *ch ;
  Objet *o_ = OBJET(o) ;

  if ( o_->classe->informations )
    {
      if ( TRACE_(o_) )
	eprintf( "Informations o=%p\n", o) ;	      
      return ((*o_->classe->informations)(o) ) ;
    }
  else
    {
      ch = u_champ_trouve_n(&o, 0, O_Init3d|O_InitChaine) ;
      if ( ch )
	return(c_recupere_ici_sans_copie(o, ch)) ;
      return("") ;
    }
}

/*
 ******************************************************************************
 * CREATION / LECTURE / SAUVEGARDE
 ******************************************************************************
 */

void u_dump_memoire()
{
  ICI ;
  eprintf("A FAIRE\n") ;
}


void* o_objet(const Classe *cl)
{
  Objet *objet ;

  if ( globale_debug ) ICI ;
  objet = alloue_zone_memoire(&globale_gm_objet, cl->taille_structure) ;
  memset(objet, 0, cl->taille_structure) ;
  /*
   * Initialisations
   */
  if ( globale_debug ) ICI ;
  objet->classe = cl ;
  objet->creation     = Vrai ;
  POUR_TOUS_LES_CHAMPS_(objet,ch,
		       {
			 if ( INITIALISER(ch) )
			   c_sauve_ici(objet+1, ch, ch->defaut) ;
		       }) ;
  if ( globale_debug ) ICI ;
  objet->nom          = objet+1 ;
  objet->invalide     = 0 ;
  if ( globale_debug )
    {
      objet->trace = Vrai ;
      objet->trace2 = Vrai ;
      o_dump(objet+1) ;
    }
  /* NOTE_RIEN_NE_CHANGE_(objet) ;*/ /* Car eval du bas en haut */
  if ( globale_debug ) ICI ;

  return(objet+1) ;
}

/*
 *
 */

void o_sauve(const void* o, FILE *f)
{
  const char *v ;
  int i ;
  Objet *o_ = OBJET(o) ;

  fprintf(f,"\n") ;
  if ( TRACE2_(o_) )
    eprintf( "Sauve\n") ;	      
  POUR_TOUS_LES_CHAMPS_
    (o_, ch,
     {
       if ( SAUVABLE_(o_, ch) )
	 {
	   if ( TRACE2_(o_) )
	     eprintf("Champ(%s) Valeur(%s)\n",
		     ch->nom_champ, c_recupere_ici_sans_copie(o, ch));
	   v = c_recupere_ici_sans_copie(o, ch) ;
	   if ( ch->defaut==NULL || strcmp(v, ch->defaut) != 0 )
	     {
	       if ( TRACE2_(o_) )
		 eprintf("Le champ (%s) est différent de sa valeur par défaut (%s)\n", v, ch->defaut) ;
	       fprintf(f, "%s ", ch->nom_champ) ;
	       for(i=0;v[i];i++)
		 if ( v[i]=='\n' )
		   {
		     putc('\n',f) ;
		     putc('\t',f) ;
		   }
		 else
		   putc(v[i],f) ;
	       putc('\n',f) ;			    
	     }
	 }
     }
     ) ;
  if ( TRACE2_(o_) )
    eprintf( "Fin Sauve\n") ;	      
}

/*
 *
 */

#include <iconv.h>

char* to_utf8(char *txt)
{
  static iconv_t utf8_to_latin1 = NULL, latin1_to_utf8 ;
  char *input ;
  char *output, *converted ;
  size_t in, out ;

  if ( utf8_to_latin1 == NULL )
    {
      utf8_to_latin1 = iconv_open("latin1", "utf8");
      latin1_to_utf8 = iconv_open("utf8", "latin1");
    }
  input = txt ;
  in = strlen(txt) ;
  out = in*2+1 ;
  ALLOUER(converted, out) ;
  output = converted ;
  if ( iconv(utf8_to_latin1, &input, &in, &output, &out) == -1 )
    {
      // Fail to convert : so it is latin1 and not UTF 8
      input = txt ;
      in = strlen(txt) ;
      out = in*2+1 ;
      output = converted ;
      iconv(latin1_to_utf8, &input, &in, &output, &out) ;
      return converted ;
    }
  free(converted) ;
  return txt ;
}
 
void* o_recupere(FILE *f)
{
  Buffer nom_classe, nom_champ ;
  const char * valeur_champ ;
  const Classe *cl ;
  void *o ;
  Objet *o_ ;
  Buffer err ;
  Liste *l ;
  int i ;

  if ( fscanf(f, "%*s%s", nom_classe) != 1 )
    return((void*)-1) ; /* Saute le "Classe" */
  fgetc(f) ; /* Saute le \n (ne pas mettre dans le scanf : BUG */
  cl = char_vers_classe(nom_classe) ;
  if ( cl )
    {
      o = o_objet( cl ) ;
      o_ = OBJET(o) ;
      for(;;)
	{
	  i = fgetc(f) ;
	  if ( i<0 || i=='\n' ) break ;
	  nom_champ[0] = i ;
	  for(i=1;;i++)
	    {
	      nom_champ[i] = fgetc(f) ;
	      if ( nom_champ[i] == ' '
		   || nom_champ[i]=='\t'
		   || nom_champ[i]=='\n' )
		{
		  nom_champ[i] = '\0' ;
		  break ;
		}
	    }
	  if ( strcmp(nom_champ, "classe") == 0 )
	    continue ; /* N'accepte pas de champ classe */
	  valeur_champ = u_ligne(f) ;
	  l = o_->fils ;
	  o_->fils = NULL ;	/* La liste n'est pas bonne pour l'instant */

	  c_sauve(o, nom_champ, to_utf8((char*)valeur_champ)) ;
	  if ( o_->fils==NULL )
	    o_->fils = l ;
	}
    }
  else
    {
      eprintf( "Classe %s inconnue, je saute l'objet\n", nom_classe) ;
      while( (valeur_champ = u_ligne(f)) )
	{
	  eprintf( "%s\n", nom_champ) ;
	  if ( valeur_champ[0] == '\0' )
	    break ;
	}
      sprintf(err, "Classe %s inconnue", nom_classe) ;
      return(o_recupere(f)) ;
    }
  return(o) ;
}

/*
 ******************************************************************************
 * UTILITAIRES
 ******************************************************************************
 */

Champ* u_champ_trouve_n(const void **o, int n, int init)
{
  int position ;

  position = 0 ;
  POUR_LES_CHAMPS_CLASSE(*o, ch,
		  {
		    if ( ch->options & init )
		      {
			if ( position == n )
			  return((Champ*)ch) ;
			else
			  position++ ;
		      }
		  }
		  ) ;
  return(NULL) ;
}


void o_positionne(void *o, const Quadruplet *pt, int n)
{
  const Champ *ch ;
  char *s ;

  ch = u_champ_trouve_n((const void**)&o, n, O_Init3d) ;
  if ( ch )
    {
      s = strdup(codec_recupere_texte(codec_quadruplet, pt)) ;
      c_sauve_ici(o, ch, s) ;
      free(s) ;
    }
}

void o_positionne_chaine(void *o, const char* val)
{
  /*
   * Supression de la fonctionnalité le 16/05/2002
   */

  /*
  const Champ *ch ;

  ch = u_champ_trouve_n((const void**)&o, 0, O_InitChaine) ;
  if ( ch )
    c_sauve_ici(o, ch, val ) ;
  */
}


int o_prend_position(const void *o, Quadruplet *pt, int n)
{
  const Champ *ch ;

  ch = u_champ_trouve_n(&o, n, O_Init3d) ;
  if ( ch )
    {
      codec_sauve_texte(codec_quadruplet, pt, c_recupere_ici_sans_copie(o, ch)) ;
      return(0) ;
    }
  return(1) ;
}

void o_ils_deviennent_mes_fils(void *o, Liste *fils, Booleen debut)
{
  Objet *o_ = OBJET(o) ;

  POUR(void*, f, fils,
       {
	 if ( f==NULL )
	   continue ;
	 if ( !l_trouve(o_->fils,f) ) /* Évite fils multiple */
	   {
	     if ( debut )
	       l_ajoute(&o_->fils, f) ;
	     else
	       l_ajoute_fin(&o_->fils, f) ;

	     l_ajoute(&OBJET(f)->parent, o) ;
	   }
       }
       ) ;
  NOTE_FILS_CHANGE(o) ;
  /*  POUR_LES_CHAMPS_CLASSE_(o_, ch, c_mise_a_jour(o, ch) ; ) ; */
}

void o_il_devient_mon_fils(void *o, void *fils, Booleen debut)
{
  Liste *tmp ;

  tmp = NULL ;
  l_ajoute(&tmp, fils) ;
  o_ils_deviennent_mes_fils(o, tmp, debut) ;
  l_detruit(&tmp) ;
}

void o_annule_flag(void *o)
{
  Objet *o_ = OBJET(o) ;
  /*  if ( o_->flag ) */
    {
      o_->flag = 0 ;
      POUR_LES_FILS_(o_, f, o_annule_flag(f) ; ) ;
    }
}

void o_annule_flag2(void *o)
{
  Objet *o_ = OBJET(o) ;
  o_->flag2 = 0 ;
  POUR_LES_FILS_(o_, f, o_annule_flag2(f) ; ) ;
}

void o_annule_flag3(void *o)
{
  Objet *o_ = OBJET(o) ;
  o_->flag3 = 0 ;
  POUR_LES_FILS_(o_, f, o_annule_flag3(f) ; ) ;
}
void o_annule_flag4(void *o)
{
  Objet *o_ = OBJET(o) ;
  o_->flag4 = 0 ;
  POUR_LES_FILS_(o_, f, o_annule_flag4(f) ; ) ;
}
void o_annule_flag5(void *o)
{
  Objet *o_ = OBJET(o) ;
  o_->flag5 = 0 ;
  POUR_LES_FILS_(o_, f, o_annule_flag5(f) ; ) ;
}

Parcour o_parcour_r(void *o,
		    Parcour (*avant)(void *, void*),
		    Parcour (*apres)(void *, void*),
		    void *donnee)
{
  Objet *o_ = OBJET(o) ;

  if ( o_->flag == 0 )
    {
      o_->flag = 1 ;
      if ( avant && (*avant)(o, donnee) )
	return(Arrete) ;

      POUR_LES_FILS_(o_,
		    f,
		    {
		      if ( o_parcour_r(f, avant, apres, donnee) )
			return(Arrete) ;
		    }
		    ) ;

      if ( apres && (*apres)(o, donnee)!=Continue )
	return(Arrete) ;
    }
  return(Continue) ;
}

Parcour o_parcour_liste(Liste *l,
			Parcour (*avant)(void *, void*),
			Parcour (*apres)(void *, void*),
			void *donnee)
{
  Parcour r ;

  r = Continue ;
  POUR_LES_ELEMENTS(l, f,
		    {
		      if ( f
			   && o_parcour_r(f, avant, apres, donnee)!=Continue )
			{
			  r = Arrete ;
			  break ;
			}
		    }
		    ) ;
  POUR_LES_ELEMENTS(l, f,
		    {
		      if ( f && OBJET_OU_NULL(f) ) o_annule_flag(f) ;
		    }
		    ) ;
  return(r) ;
}

Parcour o_parcour_sauf_racine(void *o,
			      Parcour (*avant)(void *, void*),
			      Parcour (*apres)(void *, void*),
			      void *donnee)
{
  return( o_parcour_liste(OBJET(o)->fils, avant, apres, donnee) ) ;
}

Parcour o_parcour(void *o,
		  Parcour (*avant)(void *, void*),
		  Parcour (*apres)(void *, void*),
		  void *donnee)
{
  Parcour p ;
  Objet *o_ = OBJET(o) ;

  if ( TRACE2_(o_) )
    eprintf("Lance parcour, avant=%p apres=%p donnée = %p\n",
	    avant, apres, donnee) ;
  p = o_parcour_r(o, avant, apres, donnee) ;
  o_annule_flag(o) ;
  return(p) ;
}

void o_bascule_actif(void *o)
{
  c_sauve(o, "actif", int_vers_char(!atol(c_recupere_sans_copie(o,"actif")))) ;
}

void o_bascule_sous_actif(void *o, int i)
{
  Liste *l ;
  char *tmp ;

  l = NULL ;
  codec_sauve_texte(codec_liste_unsigned_long, &l,
		    c_recupere_type_sans_copie(o,&infos_L_sous_actif)) ;

  if ( l_trouve(l, (void*)(long)i) )
    l_enleve(&l, (void*)(long)i) ;
  else
    l_ajoute(&l, (void*)(long)i) ;

  tmp = strdup(codec_recupere_texte(codec_liste_unsigned_long, &l)) ;
  c_sauve_type(o, &infos_L_sous_actif, tmp ) ;
  free(tmp) ;
  l_detruit(&l) ;
}

void ecrit_numero_tag(const Champ *ch, int i)
{
  ((Champ*)ch)->parametre = i ;
}

int lit_numero_tag(const Champ *ch)
{
  return(ch->parametre) ;
}

/*
 *
 */

int la_taille_de_la_table_globale ;
int la_position_dans_la_table_globale ;
Mot_clef *la_table_globale ; 
void u_ajoute_a_table_globale(Mot_clef a)
{
  if ( la_position_dans_la_table_globale > la_taille_de_la_table_globale - 2 )
    {
      la_taille_de_la_table_globale += 100 ;
      REALLOUER(la_table_globale, la_taille_de_la_table_globale) ;
    }
  la_table_globale[la_position_dans_la_table_globale++] = a ;
}
void u_ajoute_a_table_globale2(Mot_clef a, Mot_clef b)
{
  u_ajoute_a_table_globale(a) ;
  u_ajoute_a_table_globale(b) ;
}
void u_ajoute_a_table_globale_string(Mot_clef a, const char *b)
{
  u_ajoute_a_table_globale(a) ;
  u_ajoute_a_table_globale((Mot_clef)b) ;
}
void u_ajoute_a_table_globale_int(Mot_clef a, int b)
{
  u_ajoute_a_table_globale(a) ;
  u_ajoute_a_table_globale((Mot_clef)b) ;
}
void u_ajoute_a_table_globale_affichage(Mot_clef a, Affichage b)
{
  u_ajoute_a_table_globale(a) ;
  u_ajoute_a_table_globale((Mot_clef)b) ;
}
void u_ajoute_a_table_globale_methode_char_classe(Mot_clef a, const char* (*b)(const void *))
{
  u_ajoute_a_table_globale(a) ;
  u_ajoute_a_table_globale((Mot_clef)b) ;
}
void u_ajoute_a_table_globale_methode_classe(Mot_clef a, void (*b)(void *))
{
  u_ajoute_a_table_globale(a) ;
  u_ajoute_a_table_globale((Mot_clef)b) ;
}
void u_ajoute_a_table_globale_methode_widget(Mot_clef a, void (*b)(const Interface_widget *))
{
  u_ajoute_a_table_globale(a) ;
  u_ajoute_a_table_globale((Mot_clef)b) ;
}
void u_ajoute_a_table_globale_methode_lit_widget(Mot_clef a, const char* (*b)(const Interface_widget *))
{
  u_ajoute_a_table_globale(a) ;
  u_ajoute_a_table_globale((Mot_clef)b) ;
}
void u_ajoute_a_table_globale_methode_entree_widget(Mot_clef a, Booleen (*b)(const Interface_widget *))
{
  u_ajoute_a_table_globale(a) ;
  u_ajoute_a_table_globale((Mot_clef)b) ;
}
void u_ajoute_a_table_globale_tp_ecrit(Mot_clef a, void (*b)(void*, const Champ *, const char *v))
{
  u_ajoute_a_table_globale(a) ;
  u_ajoute_a_table_globale((Mot_clef)b) ;
}
void u_ajoute_a_table_globale_tp_lit(Mot_clef a, const char* (*b)(void*, const Champ *))
{
  u_ajoute_a_table_globale(a) ;
  u_ajoute_a_table_globale((Mot_clef)b) ;
}

/*
 * Recherche d'incoherences dans le graphe
 */

void parent_fonctionne(void *o)
{
  POUR_LES_PARENTS(o, p, parent_fonctionne(p) ; ) ;
}

void noeud_coherent(void *o)
{
  const Champ *oper ;
  const void *oo ;
  int i, j ;


  /*
   * Est-ce un cycle ?
   */
  if ( OBJET(o)->flag4 )
    {
      fprintf(stderr, "Cycle\n") ;
      abort() ;
    }
  /*
   * Pas de boucle en remontée
   */
  parent_fonctionne(o) ;
  /*
   * Un des pères est bien déjà parcouru ?
   */
  i = j = 0 ;
  POUR_TOUS_LES_PARENTS_(OBJET(o), p,
		   {
		     j++ ;
		     if ( OBJET(p)->flag4 )
		       i++ ;
		   }) ;
  if ( i == 0 && j != 0 )
    {
      fprintf(stderr, "Sur %d parents, aucun n'a été parcouru !\n", j) ;
      o_dump(o) ;
      abort() ;
    }
  /*
   * Le père est bien indiqué ?
   */
  /*
    if ( ! l_trouve( OBJET(f)->parent, o ) )
    abort() ;
  */
  
  OBJET(o)->flag4 = 1 ;

  POUR_LES_FILS(o, f,
		{
		  noeud_coherent(f) ;
		}
		);
  oo = o ;
  if ( EST_OPERATEUR(oo) || oo == u_modele() )
    {
      oper = c_type_exacte_existe(oo, &infos_L_operateur) ;
      POUR_LES_ELEMENTS(CONTENU(&T(Operateur, oo, oper)), f,
			{
			  noeud_coherent(f) ;
			}
			);
    }
  OBJET(o)->flag4 = 0 ;
}

int graphe_coherent(const void *o)
{
  static int arrete_recursion = Faux ;

  if ( !arrete_recursion )
    {
      if ( u_racine(o) )
	{
	  arrete_recursion = Vrai ;
	  noeud_coherent(u_racine(o)) ;
	  arrete_recursion = Faux ;
	  return 1 ;
	}
    }
  return 1 ;
}

/*
 *
 */
/*
void *u_racine(void *o)
{
  Objet *o_ = OBJET(o) ;

  while( o_->parent )
    {
      o = l_element(o_->parent,0) ;
      o_ = OBJET(o) ;
    }
  return(o) ;
}

void *u_modele(void *o)
{
  return( FILS(u_racine(o),0) ) ;
}
*/
void *u_operateur_ou_modele_(const void *o, Booleen liste)
{
  void *modele ;
  void *p ;

  modele = u_modele(o) ;
  for(;;)
    {
      if ( o == modele )
	{
	  if ( liste )
	    {
	      static Liste moche ;
	      moche.suivant = NULL ;
	      moche.objet = modele ;
	      return &moche ;
	    }
	  else
	    return(modele) ;
	}
      p = l_element(OBJET(o)->parent, 0) ;
      if ( p == NULL )
	return NULL ;
      if ( !l_trouve(OBJET(p)->fils, o) )
	{
	  if ( liste )
	    return (void*)OBJET(o)->parent ; // Tous parents si premier l'est
	  else
	    return p ;
	}
      o = p ;
    }
  EXIT ;
}

void *u_operateur_ou_modele(const void *o)
{
  return u_operateur_ou_modele_(o, Faux) ;
}

const Liste *u_operateur_ou_modele_liste(const void *o)
{
  return u_operateur_ou_modele_(o, Vrai) ;
}

void u_enleve_fils(void *parent, int child)
{
  void *fils = FILS(parent, child) ;
  l_enleve_indice( &OBJET(parent)->fils, child) ;
  l_enleve_indice( &OBJET(fils)->parent,
		   l_trouve_indice(OBJET(fils)->parent, parent)) ;
}

void u_ajoute_fils(void *parent, void *child, int i)
{
  l_ajoute( &OBJET(child)->parent, parent) ;
  l_insere( &OBJET(parent)->fils, child, i) ;
}

/*
 * Relecture du modèle
 */

/*****************************************************************************/
/* LENT O(n^2) */
static void* m_trouve(void *o, Liste *l)
{
  POUR_LES_ELEMENTS(l, objet,
		    if ( OBJET(objet)->nom == (Objet*)o ) return(objet) ;
		    ) ;
  return(NULL) ;
}

Liste *u_charge(const char *nom, Booleen ouvre_fenetre)
{
  FILE *f ;
  Liste *l ;
  void *o ;
  Objet *objet_ ;
  char *nom_dir ;
  int i ;

  f = fopen( nom, "r") ;
  if ( f == NULL ) return(NULL) ;

  /*
   * Empile le répertoire courant pour trouver plus rapidement
   * les fichiers.
   */
  nom_dir = strdup(nom) ;
  for(i=strlen(nom_dir)-1; i>=0 ; i--)
    if ( nom_dir[i] == '/' )
      {
	nom_dir[i] = '\0' ;
	break ;
      }
  if ( i != 0 )
    u_empile_directory(nom_dir) ;
  free(nom_dir) ;
  /*
   * Récupère les objets un par un
   */
  l = NULL ;
  for(o = o_recupere(f) ; o!=(void*)-1 ; o = o_recupere(f) )
    {
      if ( o )
	l_ajoute_fin(&l, o) ;
    }
  fclose(f) ;
  /*
   * Dépile répertoire
   */
  u_depile_directory() ;
  /*
   * Remet les liens pere/fils en place
   */
  POUR_LES_ELEMENTS
    (l,
     objet,
     {
       POUR_TOUS_LES_FILS(objet,
			  fils,
			  {
			    if ( fils == NULL )
			      continue ;
			    fils  = m_trouve(fils,l) ;
			    ((Liste*)l__fils)->objet = fils ;
			    if ( fils )
			      {
				l_ajoute_fin( &OBJET(fils)->parent, objet) ;
			      }
			  }
			  ) ;
     }
     ) ;
  /*
   * Enlève les inutiles de "l".
   */
  POUR_LES_ELEMENTS
    (l,
     objet,
     {
       objet_ = OBJET(objet) ;
       if ( ouvre_fenetre && objet_->edite )
	 ouvrir_fenetre(objet) ;
       objet_->edite  = NULL ;
       if ( objet_->parent )
	 l_enleve(&l, objet) ;
     }
     ) ;

  return(l) ;
}
