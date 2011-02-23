/*
    NanoÉdit: Un modeleur algébrique interactif.
    Copyright (C) 1996-2006 Thierry EXCOFFIER, Université Claude Bernard, LIRIS

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

#define OPERATEUR_AVEC_UNDO 1

#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <time.h>
#include "u_xt_interface.h"
#include "s_string.h"
#include "t_geometrie.h"
#include "u_fichier.h"
#include "u_repartiteur.h"
#include "u_applique.h"
#include "u_vue.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <GL/gl.h>
#include "u_etat_courant.h"
#include "u_modele.h"


#ifdef hpux
char * strerror(int) ;
#endif

#define LONGUEUR_HISTORIQUE 36

/* MODIFIER LA FONCTION C CORRESPONDANTE !!!!! XXX MOCHE */
const char *definition_menu_fichier =
		      "sauve\n"
		      "charge\n"
		      "undo    Ctrl+Z\n"
		      "redo    Ctrl+Y\n"
                      "Lance le TUTORIAL\n"
		      "réinitialise\n"
		      "quitte SANS SAUVER\n"
		      ;

/* MODIFIER LA FONCTION C CORRESPONDANTE !!!!! XXX MOCHE */
const char *definition_menu_action =
                      "detruire            <Delete>\n"
                      "editer              <F1>\n"
                      "desediter           <F2>\n"
                      "desactiver          <F3>\n"
                      "active fils         <Page_Down>\n"
                      "active parents      <Page_Up>\n"
                      "ajoute fils (debut) <F5>\n"
                      "ajoute fils (fin)   <F6>\n"
                      "enleve fils         <F7>\n"
                      "inverse ordre       <F8>\n"
                      "enleve DETRUIT      <F9>\n"
                      "duplique actifs     <F10>\n"
                      ;

void modele_ajoute_des_fils(Modele *m, Liste *l)
{
  POUR_LES_ELEMENTS(l, objet, modele_ajoute_fils(m, objet) ; ) ;
}

/*
 *
 */
static void affiche_operateur(Modele *o, const Champ *c, const char *urgent)
{
  const Champ *ch ;

  ch = ADRESSE_CHAMP(o, oper) ;
  Variable_sauve(o, ch) ;
  if ( TRACE(o) )
    eprintf("Affichage de %d objets\n", l_longueur(Operateur_contenu(o, ch)));
  

  if ( globale_etat_courant.operateur_racine == NULL
       && !o->designe_fils_au_lieu_operateur )
    {
      globale_etat_courant.operateur_racine = o ;
      affiche_gl_recursif(Operateur_contenu(o, ch), urgent) ;
      globale_etat_courant.operateur_racine = NULL ;
    }
  else
    affiche_gl_recursif(Operateur_contenu(o, ch), urgent) ;

  Variable_recupere(o, ch) ;
}

static void ecrit_pov_operateur(Modele *o, const Champ *c,
				const char *v)
{
  Variable_sauve(o, ADRESSE_CHAMP(o, oper) ) ;
  pov_fait_union((FILE*)atol(v), CONTENU(&o->oper), v) ;
  Variable_recupere(o, ADRESSE_CHAMP(o, oper) );
}

static void ecrit_stl_operateur(Modele *o, const Champ *c,
				const char *v)
{
  Variable_sauve(o, ADRESSE_CHAMP(o, oper) ) ;
  stl_fait_union((FILE*)atol(v), CONTENU(&o->oper), v) ;
  Variable_recupere(o, ADRESSE_CHAMP(o, oper) );
}

static void ecrit_obj_operateur(Modele *o, const Champ *c,
				const char *v)
{
  Variable_sauve(o, ADRESSE_CHAMP(o, oper) ) ;
  obj_fait_union((FILE*)atol(v), CONTENU(&o->oper), v) ;
  Variable_recupere(o, ADRESSE_CHAMP(o, oper) );
}

/*
 * Recupere les nom de variables en les mettant
 * dans l'ordre.
 */

static char *informations(Modele *o)
{
  static Chaine b ;
  int vv, i ;
  Booleen trouve ;
  char *v ;

  v = o->oper.noms_variables ;
  if ( v == NULL )
    return(NOMFICHIER(&o->oper)) ;

  s_efface(&b) ;
  /* s_strcat(&b, o->oper.fichier) ; */
  trouve = Vrai ;
  if ( TRACE(o) )
    eprintf("Les variables sont : %s\n", v) ;
  for(vv=0;trouve;vv++)
    {
      if ( TRACE(o) )
	eprintf("Cherche variable %d\n", vv) ;
      trouve = Faux ;
      for(i=0;v[i] && !trouve;)
	{
	  if ( isdigit(v[i]) && ( i==0 || v[i-1]==' ') )
	    {
	      if ( TRACE(o) )
		eprintf("Trouve '%c'\n",
			v[i] ) ;
	      if ( atol(&v[i]) == vv )
		{
		  if ( TRACE(o) )
		    eprintf("Variable trouvee\n") ;
		  while( isdigit(v[i]) ) i++ ;
		  s_putchar(&b, ' ') ;
		  if ( v[i] )
		    {
		      i++ ;	/* Le : ou = */
		      while(    v[i]!=' '
				&& v[i]!='\0' )
			s_putchar(&b, v[i++]) ;
		    }
		  else
		    s_putchar(&b, '?') ;
		  trouve = Vrai ;
		}
	      else
		{
		  if ( TRACE(o) )
		    eprintf("Pas la bonne variable\n") ;
		  while( isdigit(v[i]) ) i++ ;
		}
	    }
	  else
	    i++ ;
	}
    }
  return( s_char(&b) ) ;
}

static const char* informations_operateur(const Modele* o)
{
  return(o->info) ;
}


/*
 *
 */

static char *nom_tmpdir(const Modele *o)
{
  static char dir[99] ;

  if ( EST_OPERATEUR(o) && OPERATEUR_AVEC_UNDO )
    sprintf(dir, "/tmp/nanoedit-%s-%lx", getenv("LOGNAME"), (long)o) ;
  else
    sprintf(dir, "/tmp/nanoedit-%s", getenv("LOGNAME")) ;
  return(dir) ;
}

static char *nom_fichier_temporaire(const Modele *o, int i)
{
  static NomFichier buf ;	/* STATICOK : car retournée */

  sprintf(buf, "%s/%05d", nom_tmpdir(o), i) ;
  return(buf) ;
}


static char *nom_fichier(Modele *o, Booleen historique)
{
  static NomFichier buf ;	/* STATICOK : car retournée */

  if ( historique )
    {
      if ( o->numero_sauvegarde==0 )
	  mkdir(nom_tmpdir(o), 0777) ;
      return( nom_fichier_temporaire(o,
				     o->animation
				     ? o->numero_sauvegarde
				     : o->numero_sauvegarde%LONGUEUR_HISTORIQUE
				     )
	      ) ;
    }

  if ( o->recharge )
    {
      // ICI;
      o->recharge = Faux ;
      sprintf(buf, "%s/%s", nom_tmpdir(o), NOM_FICHIER_SAUVEGARDE ) ;
      return( buf ) ;
    }
  
  if ( o->nom_modele[0] != '/' && o->nom_modele[0] != '.' )
    sprintf(buf, "MODELES/%s.ne", o->nom_modele) ;
  else
  
    sprintf(buf, "%s.ne", o->nom_modele) ;
  return(buf) ;
}

static void nanoedit_menage_historique(const Modele *o)
{
  NomFichier buf ;

  sprintf(buf, "rm -rf %s", nom_tmpdir(o) ) ;
  system(buf) ;
}

/*
 *
 */

static void regenere_charge(Modele *o)
{
  static Chaine s ;
  static time_t derniere_fois = 0, maintenant ;
  FILE *f ;
  Buffer tmp ;

  time(&maintenant) ;

  if ( maintenant > derniere_fois + 1 )
    {
      derniere_fois = maintenant ;
      if ( TRACE(o) )
	{
	  eprintf("regenere_charge\n") ;
	  eprintf("directories_objets = %s\n", directories_objets) ;
	}
      s_efface(&s) ;
      s_printf(&s,
	       "find `echo \"%s \" | sed -e 's,  *,/MODELES ,g'`"
	       "     -name '?*.ne' -print 2>/dev/null |"
	       "sed -e 's,.*/MODELES/,,' -e 's/.ne$//' -e 's/.*/&\001&/'  | "
	       "sort",
	       directories_objets) ;
      f = popen(s_char(&s), "r") ;
      if ( f == NULL )
	{
	  INFO(o) ;
	  exit(1) ;
	}
      
      s_efface(&s) ;
      while ( fgets(tmp, sizeof(tmp)-1, f) )
	s_strcat(&s, tmp) ;
      if ( pclose(f) )
	perror("regenere_charge") ;
    }

  reaffecte_chaine_avec_copie(&o->charge.description, s_char(&s)) ;
}


/*
 *
 */
/*
 * Distance classe / arguments
 */

#define MAX_ACTIF 100

static int distance_classe_arguments(const Classe *c, Liste *l)
{
  const Champ *tmpch ;
  const void *o, *tmpobj ;
  int poids ;

  poids = 0 ;
  POUR_LES_CHAMPS___CLASSE
    (c, ch,
     {
       if ( OBLIGATOIRE(ch) || EXTRAIT(ch) )
	 {
	   o = l_element(l, ch->numero_fils) ;
	   if ( o == NULL )
	     {
	       if ( OBLIGATOIRE(ch) )
		 {
		   return(-1) ;
		 }
	     }
	   else
	     {
	       tmpch = ch ;
	       tmpobj = o ;
	       if ( !c_trouve_type(&tmpobj, &tmpch, ch->type_dans_fils) )
		 {
		   return(-1) ;
		 }
	       if ( ch->classe_parametre )
		 {
		   if ( strcmp(OBJET(tmpobj)->classe->nom,
			       ch->classe_parametre)!=0 )
		     return(-1) ;
		   else
		     poids += MAX_ACTIF ;
		 }
	       if ( OBLIGATOIRE(ch) )
		 poids += MAX_ACTIF ;
	       poids = (poids-(poids%MAX_ACTIF)) + ch->numero_fils + 1 ;
	     }
	 }
     }) ;
  return(poids) ;
}

static int u_calcule_distances(Modele *o)
{
  Liste *sauve_arguments ;
  int i ;

  if ( TRACE(o) )
    {
      eprintf("calcul distance paquet=%d actif=%d\n", o->paquet, o->actifs) ;
      POUR_LES_ELEMENTS(o->actifs, a, eprintf("%s\n", o_informations(a)););
    }
  /*
   * Si l'utilisateur le demande, on tronque la liste des parametres
   */
  if ( o->paquet >=0 )
    {
      sauve_arguments = NULL ;
      for(i=0; i<o->paquet && l_element(o->actifs,i); i++)
	l_ajoute(&sauve_arguments, l_element(o->actifs,i)) ;
    }
  else
    sauve_arguments = o->actifs ;

  POUR_LES_CLASSES
    (cl,
     {
       cl->distance = distance_classe_arguments(cl, sauve_arguments) ;
     }) ;

  if ( o->paquet >=0 )
    l_detruit(&sauve_arguments) ;

  return(0) ;
}

/*
 *
 */
static char* u_nom_et_parametres(const Classe *cl)
{
  static Chaine s ;
  int nb_parametres ;

  s_efface(&s) ;
  s_strcat(&s, cl->menu) ;
  nb_parametres = 0 ;
  POUR(Champ*, ch, cl->champs,
       if ( ch->options & (O_Extrait|O_Obligatoire) )
       {
	 nb_parametres++ ;
       }
       ) ;
  if ( cl->paquet == NB_ACTIFS )
    nb_parametres++ ;
  if ( nb_parametres )
    {
      s_strcat(&s,"(") ;
      POUR(Champ*, ch, cl->champs,
	   if ( EXTRAIT(ch) || OBLIGATOIRE(ch) )
	   {
	     nb_parametres-- ;
	     if ( ch->classe_parametre )
	       {
		 s_strcat(&s, "\"") ;
		 s_strcat(&s, ch->classe_parametre) ;
		 s_strcat(&s, "\"") ;
	       }
	     else
	       s_strcat(&s, ch->type_dans_fils->nom) ;
	     if ( nb_parametres )
	       s_putchar(&s, ',') ;
	   }
	   ) ;
      if ( cl->paquet == NB_ACTIFS )
	s_strcat(&s,"...") ;
      s_putchar(&s,')') ;
    }
  return( s_char(&s) ) ;
}



static void regenere_creation(Modele *o)
{
  static Chaine s ;

  if ( TRACE(o) )
    eprintf("regenere_creation\n") ;
  u_calcule_distances(o) ;
  s_efface(&s) ;

    POUR_LES_CLASSES(cl,
		     {
		       /*
		       if ( cl->menu == NULL )
			 {
			   break ;
			 }
		       */
		       if ( strcmp(cl->menu,"NON") )
			 {
			     s_printf(&s, "%s\t [%d]\t %s\001%s%s\n"
				      , u_nom_et_parametres(cl)
				      , cl->distance
				      , decode_evenement(cl->evenement)
				      , cl->nom
				      , cl->distance<0 ? "@" : ""
				      ) ;
			 }
		     }
		     ) ;
  reaffecte_chaine_avec_copie(&o->creation.description, s_char(&s)) ;
}

static int compare_plausible(const void *a, const void *b)
{
  const Classe *aa = *((const Classe**)a) ;
  const Classe *bb = *((const Classe**)b) ;
  return( bb->distance - aa->distance ) ;
}
static void regenere_creation_plausible(Modele *o)
{
  static Chaine s ;
  const Classe **c ;
  int i, j ;
  char *pc ;
  
  if ( TRACE(o) )
    eprintf("regenere_creation_plausible\n") ;
  u_calcule_distances(o) ;
  s_efface(&s) ;
  ALLOUER(c, nombre_de_classes) ;
  POUR_LES_CLASSES(cl, c[i_cl] = cl ; ) ;
  qsort(c, nombre_de_classes, sizeof(*c), compare_plausible) ;
  for(i=0;i<20 && c[i]->distance && i<nombre_de_classes; i++)
    if ( strcmp(c[i]->menu,"NON") )
      {
	pc = u_nom_et_parametres(c[i]) ;
	for(j=strlen(pc)-1;j>=0 && pc[j]!='/';j--)
	/* vide */ ;
	j++ ;
	  
	s_printf(&s, "%s\t [%d]\t %s\001%s%s\n"
		 , &pc[j]
		 , c[i]->distance
		 , decode_evenement(c[i]->evenement)
		 , c[i]->nom
		 , c[i]->distance<0 ? "@" : ""
		 ) ;
      }
  free(c) ;

  reaffecte_chaine_avec_copie(&o->creation_plausible.description, s_char(&s)) ;
}

/*
 *
 */
static Parcour reconstruit_actif(void *objet, void *donnee)
{
  if ( ACTIF(objet) )
    l_ajoute( &((Modele*)donnee)->actifs, objet) ;
  return(Continue) ;
}

static int cmp_actif(const void *a, const void *b)
{
  return( ACTIF(((void**)a)[0]) - ACTIF(((void**)b)[0]) ) ;
}
static void regenere_actif(Modele *o)
{
  static Chaine s ;
  const char *pc ;

  if ( TRACE(o) )
    {
      eprintf("regenere_actif\n") ;
      
      if ( EST_OPERATEUR(o) )
	eprintf("C'est un opérateur\n") ;
      else
	eprintf("classe = %s\n", OBJET(o)->classe->nom) ;
    }

  if ( o->oper.liste == NULL )
    return ;


  l_detruit(&o->actifs) ;

  if ( CONTENU(&o->oper))
    o_parcour_liste(CONTENU(&o->oper), reconstruit_actif, NULL, o) ;

  l_sort(o->actifs, cmp_actif) ;

  s_efface(&s) ;
  POUR(void*, a, o->actifs,
       {
	 pc = o_informations(a) ;
	 s_strcat(&s, OBJET(a)->classe->nom) ;
	 if ( pc[0] )
	   {
	     s_putchar(&s, '(') ;
	     s_printf(&s, pc) ;
	     s_putchar(&s, ')') ;
	   }
	 s_putchar(&s, '\n') ;
       }
       ) ;
  reaffecte_chaine_avec_copie(&o->liste_actif.description, s_char(&s)) ;
}

void regenere_menu_undo(Modele *o)
{
  static Chaine commande ;
  char *pc, *fin ;
  int i ;
  
  s_efface(&commande) ;
  for(i=0; i<LONGUEUR_HISTORIQUE; i++)
    {
      pc = nom_fichier_temporaire(o,i) ;
      if ( !fichier_existe(pc) )
	break ;
      //      pc[strlen(pc)-3] = '\0' ;
      for(fin=pc ;; )
	{
	  if ( strchr(fin, '/' ) )
	    fin = strchr(fin, '/' ) + 1 ;
	  else
	    break ;
	}
      
      s_printf(&commande, "%s\001%s\n"
	       , i == o->numero_sauvegarde % LONGUEUR_HISTORIQUE ? "courant" : fin
	       
	       , pc) ;
    }
  reaffecte_chaine_avec_copie(&o->undo.description, s_char(&commande)) ;
}


static void regenere_menu_vrai(void *o)
{
  Modele *modele = o ;

  regenere_actif(modele) ;
  regenere_creation( modele) ;
  regenere_creation_plausible( modele) ;
  regenere_menu_undo(modele) ;
}

static void regenere_menu(Modele *modele)
{
  u_ajoute_action_unique(PRIORITE_REGENERE_MENU, regenere_menu_vrai, modele) ;
}

/*
 *
 */

static Parcour u_cherche_vue_ouverte(void *objet, void *d)
{
  if ( strcmp("vue", OBJET(objet)->classe->nom) == 0
       && OBJET(objet)->edite
       )
    {
      if ( 
	  *(void**)d == NULL
	  || ( c_recupere_sans_copie(*(void**)d, "b_persp")[0] != '1'
	       && c_recupere_sans_copie(objet, "b_persp")[0] == '1'
	       )
	   )
	*(void**)d = objet ;
    }

  return(Continue) ;
}

static void u_genere_icone(Modele *o, const char *fichier_courant)
{
  void *vue ;
  static Chaine commande ;

  if ( o->animation )
    return ;

  vue = NULL ;

  POUR_TOUS_LES_ELEMENTS(CONTENU(&o->oper), aa,
			 o_parcour(aa, u_cherche_vue_ouverte, NULL, &vue) ;
			 )

  if ( vue )
    {
      s_efface(&commande) ;
      /*
       * Pour la pagode, il faut de grandes icones
       * C'est vraiment pas propre....
       */
      if ( strchr(fichier_courant, '!') )
	s_printf(&commande, "icone 70 %s", fichier_courant) ;
      else
	s_printf(&commande, "icone 30 %s", fichier_courant) ;
      s_deplace(&commande, -3) ; /* enlève le "ne" */
      s_printf(&commande, ".ppm", fichier_courant) ;
      c_sauve(vue, "dump_image", s_char(&commande)) ;
    }
}

/*
 *
 */

static Parcour u_sauve_(void *objet, void *d)
{
  o_sauve(objet, (FILE*)d ) ;
  return(Continue) ;
}


static const char *u_sauve_modele(Modele *o, const char *nom)
{
  FILE *f ;
  static Chaine s ;
  char *tmp, *fin ;
  struct stat st ;

  if ( o->oper.liste == NULL )
    return "VIDE NULL !" ;


  s_efface(&s) ;
  /*
  if ( nom[0]!='/' )
    s_printf(&s, "MODELES/%s", nom) ;
  else
  */
    s_strcat(&s, nom) ;

  for(tmp = s_char(&s)+1 ; ; tmp = fin+1 ) /* + pour sauter le premier / */
    {
      fin = strchr(tmp, '/') ;
      if ( fin == NULL )
	break ;
      *fin = '\0' ;
      if ( stat(s_char(&s), &st) )
	{
	  if ( errno == ENOENT )
	    {
	      if ( mkdir(s_char(&s), 0755) )
		{
		  s_printf(&s, " : %s", strerror(errno)) ;
		  return(s_char(&s)) ;
		}
	    }
	  else
	    {
	      s_printf(&s, " : %s", strerror(errno)) ;
	      return(s_char(&s)) ;
	    }
	}
      else
	{
	  if ( !S_ISDIR(st.st_mode) )
	    {
	      s_printf(&s, " N'est pas un répertoire") ;
	      return(s_char(&s)) ;
	    }
	}
      *fin = '/' ;
    }
      
  f = fopen( s_char(&s), "w") ;
  if ( f == NULL )
    {
      if ( TRACE(o) )
	eprintf("Impossible d'ouvrir le fichier en écriture\n") ;
      s_printf(&s, " : %s", strerror(errno)) ;
      return(s_char(&s)) ;
    }
  o_parcour_liste(CONTENU(&o->oper), u_sauve_, NULL, f) ;
  fclose(f) ;

  u_genere_icone(o, s_char(&s)) ;

  // Sauve le nom du modele courant
  if ( ! EST_OPERATEUR(o) )
    {
      s_efface(&s) ;
      s_printf(&s, "%s/%s", nom_tmpdir(o), FICHIER_CONTENANT_LE_NOM_DU_MODELE) ;
      f = fopen( s_char(&s), "w") ;
      if ( f )
	{
	  fprintf(f, "%s", o->nom_modele) ;
	  fclose(f) ;
	}
    }

  return("Modèle sauvegardé") ;
}



static void u_sauve_modele2(Modele *o, Champ *ch, const char *nom)
{
  if ( o->ne_pas_sauver )
    {
      o->ne_pas_sauver = Faux ;
      return ;
    }
  if ( nom[0] == '\0' )
    {
      if ( OBJET(o)->edite )
	{
	  /* UNDO/REDO seulement si fenetre modèle ouverte */
	  u_sauve_modele(o, nom_fichier(o, Vrai)) ;
	  o->numero_sauvegarde++ ;
	}
    }
  else
    {
      c_sauve(o, "message", u_sauve_modele(o, nom)) ;
    }
}

static void u_sauve_temporaire2(void *o)
{
  u_sauve_modele2((Modele*)o, NULL, "") ;
}

/* Seulement appelé par widget */
static Booleen u_sauve_temporaire(void *o)
{
  if ( ((Modele*)o)->animation )
    {
      u_ajoute_action_unique(PRIORITE_SAUVEGARDE_ANIMATION
			     , u_sauve_temporaire2, o) ;
    }
  else
    u_ajoute_action_unique(PRIORITE_SAUVEGARDE          
			   , u_sauve_temporaire2, o) ;
  ((Modele*)o)->timout_id_sauve_temporaire = 0 ;
  return(0) ;
}


static void u_sauve_modele3(Modele *o, Champ *ch, const char *nom)
{
  NomFichier tmp ;

  if ( nom[0] )
    {
      if ( strcmp(nom, NOM_FICHIER_SAUVEGARDE) == 0 )
	{
	  /*
	   * Ici parce que o_xt_interface ne connait pas tmpdir
	   */
	  sprintf(tmp, "%s/%s", nom_tmpdir(o), NOM_FICHIER_SAUVEGARDE) ;
	  nom = tmp ;
	}
      u_sauve_modele2(o, ch, nom) ;
    }
  else
    {
      if ( o->animation )
	u_sauve_temporaire(o) ;
      else
	{
	  if ( o->timout_id_sauve_temporaire )
	    {
	      widget_enleve_timeout(o->timout_id_sauve_temporaire) ;
	      o->timout_id_sauve_temporaire = 0 ;
	    }
	  o->timout_id_sauve_temporaire = 
	    widget_ajoute_timeout(1000, u_sauve_temporaire, o) ;
	}
    }
}

/*
 *
 */

static Parcour u_max_actif(void *objet, void *d)
{
  Objet *objet_ = OBJET(objet) ;

  if ( ACTIF_(objet_) >= *(int*)d )
    *(int*)d = ACTIF_(objet_) + 1 ;

  return(Continue) ;
}

static void u_calcul_max_actif(void *objet)
{
  Modele *o = objet ;

  o_parcour_sauf_racine(o, u_max_actif, NULL, &o->numero_actif) ;
}

void operateur_revert(Modele *o, const char *nom_reel)
{
  if ( TRACE(o) )
    eprintf("Operateur revert, nomreel=%s, nom_modele=%s\n",
	    nom_reel, c_recupere_sans_copie(o, "nom_modele")) ;

  revert_modele(&o->oper, nom_reel, !EST_OPERATEUR(o) ) ;
  /*
  c_sauve_ici(o, ADRESSE_CHAMP(o, oper), "") ; // Force rechargement
  c_sauve_ici(o, ADRESSE_CHAMP(o, oper), nom_reel) ;
  */
}

static int u_recupere(Modele *o, int direction)
{
  const char* nom_reel ;
  static Chaine s ;
  FILE *fic ;
  Booleen recharge ;

  // ICI; eprintf("recharge=%d dir=%d nom=%s\n", o->recharge, direction, o->nom_modele);
  if ( TRACE(o) )
    eprintf("u_recupere, direction=%d recharge=%d nom_modele=%s\n",
	    direction, o->recharge, c_recupere_sans_copie(o, "nom_modele")) ;

  recharge = o->recharge ;

  if ( recharge )
    nom_reel = nom_fichier(o, Faux) ;
  else
    {
      if ( direction==0 )
	nom_reel = o->nom_modele ;
      else
	{
	  o->numero_sauvegarde += direction - 1 ;
	  nom_reel = nom_fichier(o, Vrai) ;
	}
    }
  // ICI; eprintf("nomreel=%s\n", nom_reel);

  if ( direction == 0 )
    {
      if ( strcmp(NOMFICHIER(&o->oper), nom_reel) == 0 )
	operateur_revert(o, nom_reel) ;
      else
	{
	  c_sauve_ici(o, ADRESSE_CHAMP(o, oper), nom_reel) ;
	  c_sauve(o, "nom_modele", NOMFICHIER(&o->oper)) ;
	}
    }
  else
    {
      operateur_revert(o, nom_reel) ;
      // operateur_revert((Operateur*)&o->oper, nom_reel) ;
    }

  s_efface(&s) ;
  s_printf(&s, "NanoÉdit %s : %s", VERSION_NANOEDIT, NOMFICHIER(&o->oper)) ;
  c_sauve(o, "titre", s_char(&s)) ;
  regenere_menu(o) ;

  o->numero_actif = 1 ;
  u_ajoute_action_unique(PRIORITE_CALCUL_MAX_ACTIF, u_calcul_max_actif, o) ;
  if ( TRACE(o) )
    eprintf("max actif = %d\n", o->numero_actif) ;
  if ( direction != 0 )
    {
      o->numero_sauvegarde++ ;
      o->ne_pas_sauver = Vrai ;
    }

  s_efface(&s) ;
  s_printf(&s, "Fin chargement de ``%s''", o->nom_modele) ;
  c_sauve(o, "message", s_char(&s)) ;

  /*
   * Remet le vrai nom de fichier après avoir rechargé une sauvegarde
   */
  if ( recharge
       || strncmp(o->nom_modele,nom_tmpdir(o),strlen(nom_tmpdir(o)))==0 )
    {
      s_efface(&s) ;
      s_printf(&s, "%s/%s", nom_tmpdir(o),FICHIER_CONTENANT_LE_NOM_DU_MODELE) ;
      fic = fopen( s_char(&s), "r") ;
      if ( fic )
	{
	  nom_reel = u_ligne(fic) ;
	  fclose(fic) ;
	}
      else
	nom_reel = NULL ;
      if ( nom_reel )
	c_sauve(o, "nom_modele", nom_reel) ;
    }  

  if ( TRACE(o) )
    eprintf("FIN u_recupere nom_modele=%s\n",
	    c_recupere_sans_copie(o, "nom_modele")) ;

  return(0);
}



static Parcour u_actif(void *objet, void *d)
{
  Objet *objet_ = OBJET(objet) ;

  if ( ACTIF_(objet_) )
    {
      POUR(void*, i, u_operateur_ou_modele_liste(objet),
	   {
	     NOTE_FILS_CHANGE(i) ;
	   }) ;
      switch( (long)d )
	{
        case 1 : detruire_objet_remonte_fils(objet) ; break ;
        case 2 :              ouvrir_fenetre(objet) ; break ;
        case 3 :              fermer_fenetre(objet) ; break ;
        case 4 :
	  o_bascule_actif(objet) ;
	  break ;
	case 5 :
	POUR_LES_PARENTS_(objet_, f,
		       if ( f != u_modele(f) )
		          if ( !OBJET(f)->actif ) o_bascule_actif(f) ;
		       ) ;
	break ;
	case 6 :
	POUR_LES_FILS_(objet_, f,
		       if ( !OBJET(f)->actif ) o_bascule_actif(f) ;
		       ) ;
	break ;
	case 8 :
	  l_enleve(&objet_->fils, NULL) ;
	  NOTE_FILS_CHANGE_(objet_) ;
	  break ;
	}
    }
  return(Continue) ;
}
/*
 *
 */
static Parcour necessite_sauve(void *objet, void *donnee)
{
  Objet *objet_ = OBJET(objet) ;

  POUR_TOUS_LES_CHAMPS_
    (objet_, ch,
     {
       if ( ( /* 25/02/2000 objet_->mettre_a_jour_widget || */ CHAMP_CHANGE_(objet_, ch) )
	    && SAUVABLE_(objet_, ch) )
	 {
	   if ( TRACE_(objet_) )
	     eprintf("Nécessite sauvegarde champ=%s\n", ch->nom_champ) ;
	   return(Arrete) ;
	 }
     }
     ) ;
  return(Continue) ;
}



/*
 *
 */
/*
e_creation(iw->objet, iw->texte, ""     , NULL    ) ;
e_creation(m        , NULL     , dernier, position) ;
*/
static void e_creation(Modele *o, const char *classe, const char *evenement,
		const char *position)
{
  void *objet, *f1 ;
  int dmin ;
  const Classe *bonne ;
  static const Classe *precedente_bonne = NULL ;
  Buffer message ;
  Liste *fin, *sauve_arguments, *sauve_suite ;
  int i ;
  int paquet ;
  int decalage ;
  Quadruplet p1 ;

  u_calcule_distances(o) ;

  if ( classe==NULL )
    {
      if ( TRACE(o) )
	eprintf("e_creation classe non fournie : CLAVIER\n") ;
  
      message[0] = '\0' ;
      
      dmin = -1 ;
      bonne = NULL ; /* Pour éviter un warning */
      POUR_LES_CLASSES(cl,
		       if ( cl->distance == -1 ) continue ;
		       if ( strcmp(cl->evenement, evenement ) )
			  continue ;
		       if ( cl->distance > dmin )
		       /*  (cl->distance == dmin && r == re) */
			  {
			    dmin = cl->distance ;
			    bonne = cl ;
			  }
			  ) ;
    }
  else
    {
      if ( TRACE(o) )
	eprintf("e_creation classe fournie : %s\n", classe) ;
      bonne = char_vers_classe(classe) ;
      if ( bonne == NULL )
	{
	  sprintf(message, "La classe par menu : %s n'existe pas",
		  classe) ;
	  c_sauve(o, "message", message) ;
	  return ;
	}
      dmin = bonne->distance ;
    }

  if ( dmin != -1 )
    {
      if ( TRACE(o) )
	eprintf("e_creation : bonne = %s\n", bonne->nom) ;
      
      /*
       * Au cas ou il manque des arguments
       */
      if ( precedente_bonne && precedente_bonne != bonne ) return ;
      /*
       * creation
       */
      if ( TRACE(o) )
	eprintf("e_creation creation de l'objet\n") ;
      objet = o_objet(bonne) ;
      /*
       * Determination des parametres necessaire a la creation
       * de la liste des composants
       */

      /*
       * Nombre de fils a adopter
       */

      if ( o->paquet != NB_NON_DEFINI)
	{
	  paquet = o->paquet ;
	  if ( bonne->paquet!=NB_ACTIFS )
	    if ( (dmin%MAX_ACTIF)<paquet )
	      paquet = (dmin%MAX_ACTIF) ;
	}
      else
	{
	  paquet = bonne->paquet ;
	  if ( paquet == NB_NON_DEFINI )
	    paquet = (dmin%MAX_ACTIF) ;
      
	  if ( paquet == NB_ACTIFS )
	    paquet = l_longueur(o->actifs) ;
	}
      if ( TRACE(o) )
	eprintf("Paquet:interface=%d classe=%d dmin=%d --> %d\n"
		,o->paquet
		,bonne->paquet
		,dmin
		,paquet
		) ;

      /*
       * Pour les créations après la première, on ne veut
       * que des créations complètes
       */
      if ( precedente_bonne )
	{
	  i = 0 ;
	  POUR_LES_CHAMPS___CLASSE
	    (bonne, ch,
	     {
	       if ( OBLIGATOIRE(ch) || EXTRAIT(ch) )
		 i++ ;
	     }) ;
	  if ( TRACE(o) )
	    eprintf("%d arguments extrait, paquet de %d\n", i, paquet) ;
	  if ( i > paquet )
	    return ;
	}
      /*
       * Decalage apres creation
       */

      if ( o->decalage != NB_NON_DEFINI )
	decalage = o->decalage ;
      else
	{
	  decalage = bonne->decalage ;
	  if ( decalage == NB_NON_DEFINI )
	    decalage = paquet ;
	}

      if ( decalage == NB_ACTIFS )
	decalage = l_longueur(o->actifs) ;

      /*
       * Valeur par defaut et integration
       */
      if ( position )
	{
	  if ( TRACE(o) )
	    eprintf("Positionne en : %s\n", position) ;
	  f1 = l_element(o->actifs, 0) ;
	  if ( f1 && paquet==0 && o_prend_position(f1, &p1, 0)==0 )
	    o_positionne(objet, &p1, 0) ;
	  else
	    {
	      p1 = char_vers_quadruplet(position) ;
	      o_positionne(objet, &p1, 0) ;
	    }
	}
      o_positionne_chaine(objet, o->message) ;

      modele_ajoute_fils(o, objet) ;

      // NOTE_UN_CHAMP_CHANGE(objet);
      evaluation(objet) ;

      /*
       * Bidouillage de la liste des arguments pour couper
       * au bon endroit (ne pas mettre trop de composants)
       */
      sauve_arguments = o->actifs ;
      fin = o->actifs ;
      for(i=0 ; i<paquet-1 && fin ; i++)
	fin = fin->suivant ;      
      if ( fin )
	{
	  sauve_suite = fin->suivant ;
	  fin->suivant = NULL ;
	}
      else
	sauve_suite = NULL ; /* Pour éviter un warning */

      if ( paquet != 0 )
	{
	  o_ils_deviennent_mes_fils(objet, o->actifs, Faux) ;
	  POUR_LES_FILS(objet, fils,
			{
			  c_sauve(fils,"actif", "0") ;
			  /* Déconnecte du modèle */
			  POUR(Modele*, m, o->oper.liste->modeles,
			       l_enleve(&OBJET(fils)->parent, m) ; ) ;
			  // l_enleve(&OBJET(fils)->parent,o) ;
			  l_enleve(&CONTENU(&o->oper), fils) ;
			}) ;
	}
      /*
      POUR_LES_CHAMPS_CLASSE(O, ch, c_mise_a_jour(O, ch) ; ) ;
      */
      if ( fin )
	fin->suivant = sauve_suite ;
      
      if ( paquet != 0 )	/* D'autre creations ? */
	{
	  fin = sauve_arguments ;
	  for(i=0 ; i<decalage && fin ; i++)
	    fin = fin->suivant ;
	  if ( fin )
	    {
	      o->actifs = fin ;
	      precedente_bonne = bonne ;
	      e_creation(o, classe, evenement, position) ;
	      precedente_bonne = NULL ;
	      o->actifs = sauve_arguments ;
	    }
	}
      sprintf(message, "Création de ``%s'' faite", bonne->nom) ;
      c_sauve(o, "message", message) ;
    }
  else
    {
      sprintf(message, "Ne crée pas ``%s'', problème d'argument",
	      evenement) ;
      c_sauve(o, "message", message) ;
    }
}
/*
 *
 */

static void clavier_vers_action(Modele *m, const char *v,
			 const char *def,
			 Booleen (*fct)(const Interface_widget *iw) )
{
  Interface_widget iw ;
  char touche[100] ;
  const char *pc ;
  int i ;

  if ( TRACE(m) )
    eprintf("Clavier vers action : (%s) dans (%s)\n", v, def) ;
  iw.objet = m ;
  strcpy(touche, v+2) ;
  *strstr(touche," ") = '\0' ;
  pc = strstr(def, touche) ;
  if ( pc )
    {
      for(;;pc--)
	{
	  if ( *pc == '\n' || pc==def )
	    break ;
	}
      if ( *pc == '\n' )
	pc++ ;
      for(i=0;pc[i]!='\0' && pc[i]!='\n';i++)
	touche[i] = pc[i] ;
      touche[i] = '\0' ;
      
      iw.texte = touche ;
      if ( TRACE(m) )
	eprintf("Appel avec (%s)\n", touche) ;
      (*fct)(&iw) ;
    }
  return ;
}


/*
 *
 */


static void u_chargement(Modele *o)
{
  if ( TRACE(o) )
    eprintf("Récupère (%s)\n", o->nom_modele) ;
  u_recupere(o,0) ;
}

#define FSI(T,C) if ( strcmp(iw->texte,T)==0 ) { C ; return(Faux) ; }


static void nanoedit_exit(Modele *o)
{
  if ( !o->animation )
    nanoedit_menage_historique(o) ;
  exit(0) ;
}

static Booleen menu_fichier(const Interface_widget *iw)
{
  void *o ;

  o = iw->objet ;
  if ( TRACE(o) )
    eprintf("menu fichier = (%s)\n", iw->texte) ;

  w_applique(iw->objet) ;
  FSI("sauve"               , u_sauve_modele2(o, NULL, nom_fichier(o, Faux)) ;
                              regenere_charge(o) ; ) ;
  FSI("charge"              , u_chargement(o) ) ;
  FSI("undo    Ctrl+Z"      , u_enleve_toutes_les_actions();u_recupere(o,-1)) ;
  FSI("redo    Ctrl+Y"      , u_enleve_toutes_les_actions();u_recupere(o,1) ) ;
  FSI("réinitialise"        , system("$MAKE default Nanoedit") ;
	                      c_sauve(u_racine(o), "stop", "1") ) ;
  FSI("quitte SANS SAUVER"  , nanoedit_exit(iw->objet) ) ;
  FSI("Lance le TUTORIAL"   , c_sauve(u_racine(o),"previent_netscape","1") ) ;
  return(Faux) ;
}

static Booleen menu_charge(const Interface_widget *iw)
{
  Modele *m ;

  m = iw->objet ;
  if ( TRACE(m) )
    eprintf("menu charge : (%s)\n", iw->texte) ;
  if ( iw->texte[0] == '\0' )
    return(Faux) ;
  w_applique(m) ;		/* Tout de suite sinon on écrase des valeurs */
  c_sauve(m, "nom_modele", iw->texte) ;
  u_chargement(m) ;
  return(Faux) ;
}

static Booleen menu_undo(const Interface_widget *iw)
{
  Modele *m ;

  m = iw->objet ;
  if ( TRACE(m) )
    eprintf("menu undo : (%s)\n", iw->texte) ;
  if ( iw->texte[0] == '\0' )
    return(Faux) ;
  w_applique(m) ;		/* Tout de suite sinon on écrase des valeurs */

  operateur_revert(m, iw->texte) ;
  //  operateur_revert((Operateur*)&m->oper, iw->texte) ;
  return(Faux) ;
}

Booleen parent_actif(void *o)
{
  POUR_LES_PARENTS(o, p,
		   {
		     if ( ACTIF(p) || parent_actif(p) )
		       return(Vrai) ;
		   }
		   ) ;
  return(Faux) ;
}

/*
 * Cela serait plus jolie avec une fonction de fusion
 * de liste en nelveant les doublons
 */

static void echange_fils_du_premier_parent(void *a, void *b)
{
  POUR_TOUS_LES_PARENTS_(OBJET(a), p,
		   {
		     NOTE_UN_CHAMP_CHANGE(p) ;
		     NOTE_FILS_CHANGE(p) ;

		     if ( a < b && l_trouve(OBJET(b)->parent, p) )
		       continue ;

		     if ( l_trouve(OBJET(p)->fils, a) )
		       l_echange(OBJET(p)->fils , a, b) ;
		     else
		       l_echange(CONTENU(&((Modele*)p)->oper), a, b) ;
		   }
		   ) ;
}

static void echange_objet(void *a, void *b)
{
  Liste *parent_a, *parent_b ;

  parent_a = OBJET(a)->parent ;
  parent_b = OBJET(b)->parent ;
  echange_fils_du_premier_parent(a,b) ;
  echange_fils_du_premier_parent(b,a) ;
  OBJET(a)->parent = parent_b ;
  OBJET(b)->parent = parent_a ;
  NOTE_UN_CHAMP_CHANGE(a);
  NOTE_UN_CHAMP_CHANGE(b);
}


static void inverse_ordre(Modele *o)
{
  Liste *l, *fin ;

  POUR_LES_ELEMENTS(o->actifs, e,
		    {
		      if ( parent_actif(e) )
			{
			  c_sauve(o, "message",
				  "Ne peut faire d'inversion sans créer "
				  "de boucle") ;
			  return ;
			}
		    }
		    ) ;
  l = l_inverse(o->actifs) ;
  fin = l ;
  POUR_LES_ELEMENTS
    (o->actifs, e,
     {
       if ( e == fin->objet )
	 break ;
       echange_objet(e, fin->objet) ;
       fin = fin->suivant ;
       if ( fin && fin->objet == e )
	 break ;
     }
     ) ;
  l_detruit(&l) ;
}

static void adopte_fils(Modele *o, Booleen debut)
{
  void *premier ;
  Liste *l ;

  if ( l_longueur(o->actifs) <= 1 )
    return ;

  premier = l_element(o->actifs, 0) ;
  if ( debut )
    l = l_inverse(o->actifs->suivant) ;
  else
    l = o->actifs->suivant ;
  o_bascule_actif(premier) ;

  POUR_LES_ELEMENTS(l, a,
		    {
		      o_bascule_actif(a) ;
		      o_il_devient_mon_fils(premier, a, debut) ;

	     POUR_TOUS_LES_PARENTS_(OBJET(a), p,
				   if ( ! l_trouve(OBJET(p)->fils, a) )
				     {
				       l_enleve(&CONTENU(&((Modele*)p)->oper),
						a) ;
				     }
				   ) ;


		    }
		    ) ;
  if ( debut )
    l_detruit(&l) ;
}

static void abandonne_fils(Modele *o)
{
  void *premier ;

  premier = NULL ;
  POUR_LES_ELEMENTS(o->actifs, a,
		    {
		      o_bascule_actif(a) ;
		      if ( premier == NULL )
			{
			  premier = a ;
			  continue ;
			}
		      l_enleve( &OBJET(premier)->fils, a) ;
		      l_enleve( &OBJET(a)->parent, premier) ;
		      if ( OBJET(a)->parent == NULL )
			{
	  /*	  o_il_devient_mon_fils(FILS(u_racine(premier),0) ,a) ;*/
			  // o_il_devient_mon_fils(o ,a, Faux) ;
			  modele_ajoute_fils(o, a) ;
			}
		    }
		    ) ;
}






static void o_duplique_actifs(Modele *m)
{
  POUR_LES_ELEMENTS(CONTENU(&m->oper), f, o_annule_flag5(f) ;) ;
  POUR_LES_ELEMENTS(m->actifs, o,
		    modele_ajoute_fils(m, duplique_objet_parmi_d_autres(o) ) ;
		    ) ;
  NOTE_FILS_CHANGE(m) ;
}

static Booleen menu_contextuel(const Interface_widget *iw)
{
  if ( iw->objet_contextuel == NULL
       || OBJET_OU_NULL(iw->objet_contextuel) == NULL )
    {
      EXIT ;
    }

  FSI("Detruire",
      detruire_objet_remonte_fils(iw->objet_contextuel) ;
      ) ;
  FSI("Editer",
      ouvrir_fenetre(iw->objet_contextuel) ;
      ) ;
  FSI("Enleve DETRUIT",
      l_enleve(&OBJET(iw->objet_contextuel)->fils, NULL) ;
      NOTE_FILS_CHANGE(iw->objet_contextuel) ;
      ) ;
  FSI("Duplique",
      POUR_LES_ELEMENTS(CONTENU(&((Modele*)iw->objet)->oper), f, o_annule_flag5(f) ;) ;
      modele_ajoute_fils((Modele*)iw->objet
			 , duplique_objet(iw->objet_contextuel)) ;
      NOTE_FILS_CHANGE(iw->objet) ;
      ) ;
  FSI("Aide",
  {
    Interface_widget iw2 = *iw ;
    iw2.objet = iw->objet_contextuel ;
    iw2.ch = NULL ;
    u_popup_help_reel(&iw2) ;
  }
      ) ;

  return(Faux) ;
}

static Booleen menu_action(const Interface_widget *iw)
{
  Modele *m ;
  void *o ;
  Liste *l ;

  o = iw->objet ;
  m = iw->objet ;

  l = CONTENU(&m->oper) ;

  FSI("detruire            <Delete>"   ,o_parcour_liste(l,u_actif,NULL,(void*)1) ) ;
  FSI("editer              <F1>"       ,o_parcour_liste(l,u_actif,NULL,(void*)2) ) ;
  FSI("desediter           <F2>"       ,o_parcour_liste(l,u_actif,NULL,(void*)3) ) ;
  FSI("desactiver          <F3>"       ,o_parcour_liste(l,u_actif,NULL,(void*)4) ) ;
  FSI("active fils         <Page_Down>",o_parcour_liste(l,NULL, u_actif,(void*)6)) ;
  FSI("active parents      <Page_Up>"  ,o_parcour_liste(l,u_actif,NULL,(void*)5) ) ;
  FSI("ajoute fils (debut) <F5>"       ,adopte_fils(o, Vrai) ) ;
  FSI("ajoute fils (fin)   <F6>"       ,adopte_fils(o, Faux) ) ;
  FSI("enleve fils         <F7>"       ,abandonne_fils(o) ) ;
  FSI("inverse ordre       <F8>"       ,inverse_ordre(o) ) ;
  FSI("enleve DETRUIT      <F9>"       ,o_parcour_liste(l,u_actif,NULL,(void*)8) ) ;
  FSI("duplique actifs     <F10>"      ,o_duplique_actifs(m) ) ;
  return(Faux) ;
}

static Booleen menu_creation(const Interface_widget *iw)
{
  if ( TRACE(iw->objet) )
    eprintf("menu creation = (%s)\n", iw->texte) ;
  c_sauve( u_racine(iw->objet), "widget_applique",
	   int_vers_char((long)iw->objet)) ;

  regenere_menu(iw->objet) ;

  e_creation(iw->objet, iw->texte, "", NULL) ;
  return(Faux) ;
}

const char* decode_evenement_court(const char *e)
{
  static char *ee = NULL ;
  char *eee ;

  if ( ee )
    free(ee) ;

  ee = strdup(decode_evenement(e)) ;
  eee = strchr(ee, '\t') ;
  if ( eee )
    strcpy_overlap(eee, eee+1) ;
  return(ee) ;
}

static Booleen menu_liste(const Interface_widget *iw)
{
  /* Action a faire sur la liste : aucune pour l'instant */
  return(Faux) ;
}

#define DD ICI; \
eprintf("historique=(%s)\n", historique) ;\
eprintf("historique_interne=(%s)\n", m->historique_interne) ;\
eprintf("m->historique=(%s)\n", m->historique) ;


static void u_ajoute_touche(Modele *m, const Champ *ch, const char *v)
{
  char *historique ;
  const char *position ;
  int possible, i, pas_bon ;
  static char *dernier = NULL ;	/* STATICOK : Dernière chaine tapée */
  Quadruplet pt, pt_saisi ;
  static Chaine suites_possibles, affiche ;

  /*
  if ( strcmp( OBJET(m)->classe->nom, "modele") != 0 )
    abort() ;
  */
  w_applique(m) ;

  if ( m->historique_interne == NULL )
    m->historique_interne = strdup("") ;

  if ( TRACE(m) )
    eprintf("Touche = (%s)\n", v) ;

  position = NULL ;
  if ( v[strlen(v)-1] == '!' )
    {
      position = v + strlen(v)-2 ;
      while(*position != ' ' ) position-- ;
      position-- ;
      while(*position != ' ' ) position-- ;
      position-- ;
      while(*position != ' ' ) position-- ;
      if ( TRACE(m) )
	eprintf("position = (%s)\n", position) ;
    }

  if ( strncmp(v, "2z ", 3) == 0 )
    v = "0 Ctrl+Z " ;
  else if ( strncmp(v, "2y ", 3) == 0 )
    v = "0 Ctrl+Y " ;

  if ( v[1] == ' ' )
    {
      clavier_vers_action(m, v, definition_menu_action, menu_action) ;
      clavier_vers_action(m, v, definition_menu_fichier, menu_fichier) ;
      c_sauve(m, "historique", "") ;
      m->historique_interne[0] = '\0' ;
      return ;
    }

  if ( m->historique_interne[0]=='\0' && position && v[1]>='1' && v[1]<='9' )
    {
      if ( TRACE(m) )
	{
	  eprintf("Déplacement des actifs : \n") ;
	  POUR(void*, o, m->actifs,
	       eprintf("%s, ", o_informations(o)) ;
	       ) ;
	  eprintf("\n") ;
	}
      /*
       * Cas du déplacement d'un objet
       */
      pt_saisi = char_vers_quadruplet(position) ;
      if ( TRACE(m) )
	{
	  eprintf("Vers les coordonnees : (%g %g %g) position[-1]='%c'\n",
		  pt_saisi.x, pt_saisi.y, pt_saisi.z, position[-1]) ;
	}
      if ( position[-1] == '3' )
	return ; /* Car pas bonne fenetre */
      pas_bon = position[-1] - '0' ;
      POUR(void*, o, m->actifs,
	   {
	     o_prend_position( o, &pt, v[1]-'1' ) ;
	     if ( TRACE(m) )
	       eprintf("Position avant : %s\n", quadruplet_vers_char(&pt)) ;
	     for(i=0;i<3;i++)
	       if ( i != pas_bon )
		 (&pt.x)[i] = (&pt_saisi.x)[i] ;
	     if ( TRACE(m) )
	       eprintf("Vers les coordonnees : (%g %g %g %g)\n",
		       pt.x, pt.y, pt.z, pt.h) ;
	     o_positionne( o, &pt, v[1]-'1' ) ;
	     evaluation(o) ; /* A cause des OPERATEUR pour remonter le changement */
	   }
	   ) ;
      c_sauve(m, "historique", "") ;
      m->historique_interne[0] = '\0' ;
      return ;
    }

  if ( v[1] == '.' && dernier )
      historique = strdup(dernier) ;
  else
    {
      ALLOUER(historique, strlen(m->historique_interne)+3) ;
      sprintf(historique, "%s%c%c", m->historique_interne, v[0], v[1]) ;
    }



  u_calcule_distances(m) ;
  s_efface(&suites_possibles) ;
  for(;;)
    {
      possible = Faux ;
      POUR_LES_CLASSES
	(cl,
	 if ( strncmp(historique, cl->evenement, strlen(historique) ) == 0 )
	 {
	   if ( strcmp(historique, cl->evenement) == 0 )
	     {
	       reaffecte_chaine_avec_copie(&dernier, cl->evenement) ;
	       e_creation(m, NULL, dernier, position) ;
	       /* c_sauve(m, "graphe", "1") ; */
	       historique[0] = '\0' ;
	     }
	   else
	     {
	       if ( cl->distance >= 0 )
		 s_printf(&suites_possibles, " %s(%s)", decode_evenement_court(cl->evenement), cl->nom) ;
	     }
	   possible = Vrai ;
	 }
	 ) ;
			 
      if ( possible ) break ;
      memmove(historique, historique+2, strlen(historique)) ;
    }

  reaffecte_chaine_sans_copie(&m->historique_interne, historique) ;

  s_efface(&affiche) ;
  s_printf(&affiche, decode_evenement_court(historique)) ;
  if ( possible && historique[0] )
    s_printf(&affiche, " :%s", s_char(&suites_possibles)) ;

  c_sauve(m, "historique", s_char(&affiche)) ;
}

static void w_ajoute_touche(const Interface_widget *iw)
{
  u_ajoute_touche(iw->objet, iw->ch, iw->texte) ;
}

/*
 */
static Parcour change_la_vue(void *f, void *d)
{
  if ( c_recupere_type_sans_copie(f, &infos_L_prevenir_si_modele_change)[0]
       == '1' )
    {
      /*      NOTE_DESCENDANT_CHANGE(f) ; NON car bouclage */
      recupere_fenetre(f) ;
    }
  return(Continue) ;
}


/*
 *
 */
static Booleen debut_fin_animation(const Interface_widget *iw)
{
  Modele *o = iw->objet ;

  if ( o->animation==Faux )
    {
      o->animation = Vrai ;
      o->numero_sauvegarde = 0 ;
      nanoedit_menage_historique(o) ;
      u_enleve_toutes_les_actions() ; /* Pour ne pas faire 2 sauvegardes */
    }
  else
    {
      o->animation = Faux ;
    }
  return(Faux) ;
}

static Liste **contenu(Modele *o)
{
  return & CONTENU(&o->oper) ;
}


static void enleve_null(void *objet)
{
  Modele *o = objet ;

  if ( o->oper.liste == NULL )
    return ;

  POUR_LES_ELEMENTS(*contenu(o), f,
		    {
		      if ( OBJET_OU_NULL(f) )
			{
			  /*
			  if ( l_longueur( OBJET(f)->parent ) > 1 )
			    {
			      l_enleve( &OBJET(f)->parent, o) ;
			      l_remplace( *contenu(o), f, NULL) ;
			    }
			  */
			}
		      else
			l_remplace( *contenu(o), f, NULL) ;
		    }
		    ) ;
  l_enleve(contenu(o), NULL) ;
}

static void changement_modele(Modele *o)
{
  Booleen change ;
  Liste *l ;

  u_ajoute_action_unique(PRIORITE_ENLEVE_NULL, enleve_null, o) ;

  if ( o->init==0 )
    {
      o->init = 1 ;
      o->numero_actif = 1 ;
      regenere_charge(o) ;
      if ( !EST_OPERATEUR(o) )
	u_chargement(o) ;
    }

  if ( !EST_OPERATEUR(o) && (FILS_CHANGE(o) || CHAMP_CHANGE(o,oper)) )	/* Changement dans les fils */
    {
      o_appel_changement_liste(CONTENU(&o->oper)) ;
      NOTE_UN_CHAMP_CHANGE(o) ;
    }

  change = Faux ;
  if ( o->oper.liste )
    {
      l = *contenu(o) ;
      change = ( o_parcour_liste(l, necessite_sauve, NULL, NULL) == Arrete );
      if ( change || DESCENDANT_CHANGE(o) )
	{
	  o_parcour_liste(l, change_la_vue, NULL, NULL) ;
	  NOTE_UN_CHAMP_CHANGE(o) ;
	}
    }
  regenere_menu(o) ;
}

static void destruction_modele(Modele *o)
{
  if ( o->timout_id_sauve_temporaire )
    {
      widget_enleve_timeout(o->timout_id_sauve_temporaire) ;
    }
}


CLASSE(modele, Modele,
       MENU("NON")
       BULLE_D_AIDE
       ("Cet objet vous permet de modifier un modèle NanoEdit.\n"
	"Pour plus d'information, demandez l'aide sur les différents champs"
	)
       AFFICHE_CHAMP_CLASSE(Faux)
       CHAMP(oper, L_operateur NonCree Sauve NonAffiche)
       // 2010-07-29 Ne marche pas on on ne reçoit pas le window resize.
       //       CHAMP(geometrie, L_geometrie Cree Edite NonAffiche Sauve DEFAUT("2 2 530 900"))
       CHAMP(nom_modele,
	     BULLE_D_AIDE
	     ("C'est le nom du fichier contenant le modèle.\n"
	      "Vous entrez le nom puis demandez à CHARGER ou SAUVER."
	      )
	     Edite LABEL("Modèle") L_chaine InitChaine
	     )
       DEBUT_BOITE(HORIZONTALE)
       CHAMP(numero_sauvegarde,
	     BULLE_D_AIDE
	     ("Cela indique le nombre de modifications faites sur le modèle\n."
	      "La sauvegarde est faite dans le fichier :\n"
	      "    /tmp/nanoedit-`logname`/NUMERO%100.ne"
	      )
	     LABEL("N° sauvegarde")
	     TE_LARGEUR(40)
	     L_entier  Affiche
	     )
       CHAMP(numero_actif, L_entier)
       CHAMP(paquet,
	     BULLE_D_AIDE
	     ("Lorsque vous créez un objet,\n"
	      "il prend le bon nombre d'argument.\n"
	      "Si vous voulez lui en faire prendre plus ou moins\n"
	      "vous pouvez l'indiquer ici.\n"
	      "\n"
	      "La valeur -2 indique que l'on prend le nombre par defaut.\n"
	      "La valeur -1 indique que l'on prend tous les actifs."
	      )
	     LABEL("Paquet") L_entier Edite     DEFAUT("-2")
	     TE_LARGEUR(40)
	     )
       CHAMP(decalage,
	     BULLE_D_AIDE
	     ("Quand il y a trop d'actif, il est possible de créer\n"
	      "plusieur objets.\n"
	      "Par défaut (-2) il saute le bon nombre d'actif.\n"
	      "Sinon vous pouvez lui indiquer le nombre d'actif\n"
	      "qu'il faut ``sauter''.\n"
	      "\n"
	      "Essayez en créant des segments à partir de 5 points\n"
	      "et en choisissant -2 puis 1 puis 2 puis 3 pour\n"
	      "la valeur du décalage."
	      )
	     LABEL("Décalage") L_entier  Edite    DEFAUT("-2")
	     TE_LARGEUR(40)
	     )
       FIN_BOITE
       CHAMP(historique,
	     BULLE_D_AIDE
	     ("Ce champ contient l'ensemble des touches clavier\n"
	      "que vous avez déjà tapés et qui sont peut être\n"
	      "le début d'un raccourci clavier.\n"
	      "\n"
	      "Un raccourci clavier est une suite de paire de caractères.\n"
	      "\n"
	      "Le premier est un chiffre entre 0 et 7 indiquant\n"
	      "si les touches Shift, Control, Alt sont appuyées.\n"
	      "0=rien 1=Shift 2=Control 3=Shift+Control 4=Alt ...\n"
	      "\n"
	      "Le deuxième est un caractère normal pouvant être\n"
	      "un chiffre.\n"
	      )
	     L_chaine  Affiche
	     )
       CHAMP(message,
	     BULLE_D_AIDE
	     ("Ce champ contient les messages d'erreur mais sert aussi\n"
	      "à entrer des valeurs textuelles avant de créer un objet.\n"
	      "Par exemple pour les objets «vue» et «opérateur»."
	      )
	     L_chaine NonLabel Edite DEFAUT("Bienvenue sur NanoÉdit")
	     )
       DEBUT_BOITE(HORIZONTALE)
       CHAMP(fichier,
	     L_commande_menu(menu_fichier)
	     BULLE_D_AIDE
	     ("Utilisez ce menu pour sauvegarder/lire un modèle.\n"
	      "Le nom du modèle est indiqué dans le champ modèle.\n"
	      "\n"
	      "En lecture, le fichier va être recherché dans tous\n"
	      "les répertoires contenant des fichiers C décrivant\n"
	      "des objets Nanoédit, dans le sous-répertoire MODELES.\n"
	      "\n"
	      "Le modèle nommé '4vues' contient les quatre vues 3D\n"
	      )
	     LABEL("Fichier")
	     TE_TEXTE(definition_menu_fichier)
	     )
       CHAMP(undo,
	     L_commande_menu(menu_undo)
	     BULLE_D_AIDE
	     ("Revenir en arrière ou avant....\n"
	      )
	     LABEL("Undo")
	     )
       CHAMP(charge,
	     L_commande_menu(menu_charge)
	     BULLE_D_AIDE
	     ("Pour charger un modèle existant des répertoires MODELES.\n"
	      )
	     LABEL("Charge")
	     )
       CHAMP(les_actifs,
	     LABEL("Action")
	     L_commande_menu(menu_action)
	     BULLE_D_AIDE
	     ("Ce menu permet de faire une action sur l'ensemble des\n"
	      "objets qui sont actif\n"
	      )
             TE_TEXTE(definition_menu_action)
	     )
       CHAMP(menu_contextuel,
	     L_commande_menu(menu_contextuel)
             TE_TEXTE("Editer\n"
		      "Detruire\n"
		      "Enleve DETRUIT\n"
		      "Duplique\n"
		      "Aide\n"
		      )
	     NonCree
	     )
       CHAMP(liste_actif,
	     LABEL("Liste")
	     BULLE_D_AIDE
	     ("Ce menu contient la liste triée des objets actifs.\n"
	      "Dans le futur, des sous-menu d'action seront certainement\n"
	      "ajouté...\n"
	      )
	     L_commande_menu(menu_liste)
	     TE_TEXTE("Aucun objet actif"))
       CHAMP(animation, L_bouton(debut_fin_animation) LABEL("Anime")
	     BULLE_D_AIDE
	     ("Quand vous cliquez ici,\n"
	      "toutes les modifications du modèle sont sauvée\n"
	      "dans le répertoire de sauvegarde /tmp/nanoedit-....\n"
	      "afin de pouvoir être rejouées après\n"
	      "ATTENTION : vous perdez l'historique des Undo\n"
	      "en cliquant ici.\n"
	      "Quand vous avez terminé l'animation cliquez sur\n"
	      "Fichier/Quitter sans sauver\n"
	      )
	     )
       CHAMP(creation_plausible,	     
	     LABEL("Créable")
	     L_commande_menu(menu_creation)
	     BULLE_D_AIDE
	     ("Ce menu permet de créer les objets\n"
	      "les plus probable étant donné votre\n"
	      "sélection d'objet actif.\n"
	      "ATTENTION : tous les objets que l'on\n"
	      "peut créer ne sont pas dans ce menu.\n"
	      )
	     TE_TEXTE("Aucun objet en particulier\nActivez au moins un objet"))
       CHAMP(creation,	     
	     LABEL("Création")
	     L_commande_menu(menu_creation)
	     BULLE_D_AIDE
	     ("Ce menu permet de créer des objets.\n"
	      "Les choix autorisés dans le menu dépendent de\n"
	      "la liste des objets actifs.\n"
	      "\n"
	      "On trouve pour les objets :\n"
	      " - Le nom générique de l'objet\n"
	      " - Les arguments attendus par l'objet.\n"
	      "   (Obligatoire ou Optionnel, ce n'est pas indiqué.)\n"
	      " - Entre crochet, un nombre indiquant l'adéquation\n"
	      "   de la liste des objets actifs aux paramètres attendus.\n"
	      " - L'accélérateur clavier (voir l'aide du champ historique).\n"
	      )
	     TE_TEXTE("Il y a un bug, ce menu est vide"))
       FIN_BOITE
       CHAMP(oper.liste,
	     
	     BULLE_D_AIDE
	     ("Vous pouvez en cliquant avec le bouton de gauche\n"
	      "dans cette zone activer ou inactiver des objets.\n"
	      )	      
	     L_graphe_modele
	     P_modele
	     TE_CLAVIER(w_ajoute_touche)
	     TE_HAUTEUR(600)
	     Edite 
	     )

       CHANGEMENT(changement_modele)
       DESTRUCTION(destruction_modele)
       CHAMP(titre       , L_titre DEFAUT("NanoÉdit"))
       CHAMP(recharge    , L_booleen NonCree)
       /* C'est en 2 2 à cause de beaucoup de choses....
	* Voir : t_widget.c:Geometrie_ecrit
	* Problème de window manager qui ne donne pas les même position
	* à un pixel près
	*/
       CHAMP(ouvre_fenetres, L_entier)
       CHAMP_VIRTUEL(NOM("u_ajoute_touche") L_fonction(u_ajoute_touche))
       CHAMP_VIRTUEL(NOM("u_sauve_modele") L_fonction(u_sauve_modele3))
       )

/*
 *
 */

struct variable
{
  char *nom_variable ;
} ;

static void changement_variable(struct variable *o)
{
  NOTE_UN_CHAMP_CHANGE(o) ;
}

CLASSE(variable, struct variable,
       BULLE_D_AIDE("Cet objet défini une variable,\n"
		    "son paramètre est la valeur de la variable.\n"
		    "On utilise la variable pour créer des modèles paramétrés."
		    )
       CHAMP(nom_variable,
	     L_variable
	     E_texte
	     P_chaine
	     TYPE_DANS_FILS(L_void)
	     BULLE_D_AIDE
	     ("Le nom de la variable est de la forme :\n"
	      "   numéro_argument=nom_variable\n"
	      "Par exemple '1=rayon' indique que cette variable\n"
	      "est liée au DEUXIÈME paramètre de l'opérateur\n"
	      "lors de l'appel et que son nom est 'rayon'.\n"
	      )
	     Edite Sauve Obligatoire InitChaine
	     )
       PAQUET(1)
       MENU("Divers/Variable")
       EVENEMENT("0v0a")
       CHANGEMENT(changement_variable)
       )
     
/*
 * Cette fonction AFFICHE est faite
 * pour que l'ensemble des graphisme soient
 * activés.
 * Une boîte englobante serait mieux.
 * Il permet aussi d'afficher tous les objets
 * et pas seulement le premier du modèle.
 */




static void destruction_operateur(Modele* o)
{
  NomFichier buf ;
  
  sprintf(buf, "rm -r %s", nom_tmpdir(o)) ;
  system(buf) ;
  destruction_modele(o) ;
}

static void changement_operateur(Modele* o)
{
  if ( TRACE(o) )
    eprintf("Changement dans l'opérateur, contenu = %p\n", contenu(o)) ;

  if ( !CHAMP_CHANGE(o, oper) && CHAMP_CHANGE(o, nom_modele) )
    {
      if ( TRACE(o) )
	eprintf("Rechargement du modèle de l'opérateur %s!=%s\n",
		NOMFICHIER(&o->oper), o->nom_modele) ;
      c_sauve_ici(o, ADRESSE_CHAMP(o, oper), o->nom_modele) ;
    }
  if ( CHAMP_CHANGE(o, oper) )
    {
      c_sauve(o, "info", informations(o)) ;
      if ( TRACE(o) )
	eprintf("nom_modele=%s NOMFICHIER=%s\n",
		c_recupere_sans_copie(o,"nom_modele"), NOMFICHIER(&o->oper));
      if ( strcmp(c_recupere_sans_copie(o,"nom_modele"), "") == 0 )
	c_sauve(o, "nom_modele", NOMFICHIER(&o->oper)) ;
    }

  if ( FILS_CHANGE(o) || CHAMP_CHANGE(o,oper) )	/* Changement dans les fils */
    {
      Variable_sauve(o, ADRESSE_CHAMP(o, oper) ) ;
      o_parcour_liste(CONTENU(&o->oper), variable_change, NULL, o) ;
      if ( TRACE(o) )
	eprintf("Appel changement sur le contenu %p\n", contenu(o)) ;
      o_appel_changement_liste(CONTENU(&o->oper)) ;
      Variable_recupere(o,  ADRESSE_CHAMP(o, oper)) ;
      if ( TRACE(o) )
	eprintf("contenu %p\n", contenu(o)) ;
      NOTE_UN_CHAMP_CHANGE(o) ;

      if ( OPERATEUR_AVEC_UNDO )
	u_sauve_temporaire(o);
      if ( TRACE(o) )
	eprintf("contenu %p\n", contenu(o)) ;
    }
  if ( TRACE(o) )
    eprintf("Changement dans l'opérateur : FIN\n") ;
  
  if ( o->init==0 || OBJET(o)->edite )
    {
      changement_modele(o) ;
    }
  if ( TRACE(o) )
	eprintf("contenu %p\n", contenu(o)) ;
}

/*
 * Si quelquechose change, l'opérateur doit refaire ses listes.
 * Ceci n'est utile que si l'opérateur contient un plan
 * sur lequel il se projète une ombre....
 * Il faudrait trouver mieux....
 */

const char* me_prevenir_toujours(void *o, const Champ *ch)
{
  if ( TRACE(o) )
    eprintf("me_prevenir_toujours\n") ;
  destruction_liste_objet(o) ;
  return("0") ;
}


CLASSE(operateur, Modele,
       /*
	* Le champ suivant est ici en premier
	* car quand on fait une extraction avec "classe_parametre"
	* C'est lui qui doit être sous-classé.
	*/
       /*
	* AFFICHAGE(Toujours_different) car l'opérateur
	* peut contenir des objets Actif ou des plan de projection
	* d'ombre qui changent dès que le modèle change
	*/
       CHAMP_VIRTUEL(L_affiche_gl(affiche_operateur)
		     AFFICHAGE(Toujours_different)
		     )
       CHAMP_VIRTUEL(L_affiche_pov(ecrit_pov_operateur))
       CHAMP_VIRTUEL(L_affiche_stl(ecrit_stl_operateur))
       CHAMP_VIRTUEL(L_affiche_obj(ecrit_obj_operateur))
       /*
	* La surclasse est indiquée après pour que les champs de la surclasse
	* suivent les champs précédents
	*/
       CHAMP(designe_fils_au_lieu_operateur, L_booleen Edite Sauve
	     BULLE_D_AIDE("Si vrai, lorsque l'on clique sur l'opérateur\n"
			  "c'est l'un des sous-objets qui est activé\n"
			  "et non l'opérateur lui-même\n")
	     )
       SUR_CLASSE("modele")
       CHAMP(info, L_chaine) /* Pour laisser Nano faire la désallocation */
       INFORMATIONS(informations_operateur) ;

       BULLE_D_AIDE("Cet objet permet de charger le modèle dont on\n"
		    "donne le nom, tout en affectant les variables\n"
		    "du modèle chargé aux valeurs des différents fils\n"
		    "de cet objet.\n"
		    )
       DESTRUCTION(destruction_operateur)
       CHANGEMENT(changement_operateur)
       PAQUET(NB_ACTIFS)
       MENU("Divers/Operateur")
       EVENEMENT("0o0p")
       /* CHAMP_VIRTUEL( L_prevenir_si_modele_change(me_prevenir_toujours)) */
       )
