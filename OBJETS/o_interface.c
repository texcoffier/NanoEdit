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
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <sys/times.h> /* Pour aleatoire */
#include <sys/stat.h> /* Pour aleatoire */
#include <fcntl.h>
#include <stdlib.h>

#include "u_xt_interface.h"
#include "o_objet.h"
#include "o_interface.h"
#include "s_string.h"
#include "u_httpd.h"
#include "u_fichier.h"
#include "u_repartiteur.h"
#include "u_modele.h"

/*
 ******************************************************************************
 * Utilitaires
 ******************************************************************************
 */

#define RDM(X) (srand((long)(X)), rand()+(rand()<<16))

static long aleatoire()
{
  long a ;
  struct tms t ;
  struct stat st ;
  static int fichier = -1 ;
  
  a = RDM(times(&t)) ;
  a += RDM(aleatoire) ;
  a += RDM(&a) ;
  times(&t) ;
  a += RDM(t.tms_utime) ;
  a += RDM(t.tms_stime) ;
  if ( fichier != -1 )
    close(fichier) ;
  unlink(".inode") ;
  fichier = creat(".inode",0) ;
  if ( fichier < 0 )
    {
      perror("open .inode") ;
      exit(1) ;
    }
  unlink(".inode") ;
  a += RDM(fichier) ;
  if ( fstat(fichier, &st) )
    {
      perror("fstat .inode") ;
      exit(1) ;
    }
  a += RDM(st.st_ino) ;
  a += RDM(st.st_dev) ;
  a += RDM(st.st_mtime) ;
  return(a) ;
}

static char *url_alea(long o, char* url)
{
  Buffer u ;

  if ( url[0] != '/' )
    return(url) ;

  sprintf(u, "/%08lX%s", o, url) ;
  return(strdup(u)) ;
}

static const char *u_rc(const char *a, const char *b, const char *c)
{
  static Chaine s ;

  s_efface(&s) ;
  s_printf(&s,"%s%s%s", a, remplace_code(b), c) ;
  return( s_char(&s) ) ;
}

static const char *u_chaine(const char*v)
{
  return( u_rc("<B>", v, "</B>") ) ;
}
static const char *u_fichier(const char*v)
{
  static Chaine s ;
  char *url ;

  if ( strncmp(v, DIRECTORY_BASE, strlen(DIRECTORY_BASE)) == 0 )
    url = strdup(v + strlen(DIRECTORY_BASE)) ;
  else
    url = strdup(v) ;

  while( strchr(url, '/') )
    strchr(url, '/')[0] = ':' ;

  s_efface(&s) ;
  s_printf(&s, "<A HREF=\"texte%s\"\"><B>%s</B></A>", url, remplace_code(v)) ;
  free(url) ;
  return( s_char(&s) ) ;
}
static const char *u_ptr(void *v)
{
  return( u_rc("<B><TT>", int_vers_char((long)v), "</TT></B>") ) ;
}
static const char *u_entier(int v)
{
  return( u_rc("<B>", int_vers_char(v), "</B>") ) ;
}
static const char *u_classe(const Classe *cl)
{
  static Chaine s ;

  s_efface(&s) ;
  s_printf(&s,
	   "``<A HREF=\"classe?classe=%d\">%s</A>''",
	   cl->index, remplace_code(cl->nom)
	   ) ;
  return(s_char(&s)) ;
}
static const char *u_champ(const Champ *ch)
{
  static Chaine s ;

  s_efface(&s) ;
  s_printf(&s,
	   "``<A HREF=\"champ?classe=%d&champ=%d\">%s</A>''",
	   ch->classe->index, ch->index, remplace_code(ch->nom_champ)
	   ) ;
  return(s_char(&s)) ;
}
static const char *u_champ_objet(const Champ *ch, const void *o)
{
  static Chaine s ;

  s_efface(&s) ;
  s_printf(&s,
	   "``<A HREF=\"champ?classe=%d&champ=%d&objet=%d\">%s</A>''",
	   ch->classe->index, ch->index, o, remplace_code(ch->nom_champ)
	   ) ;
  return(s_char(&s)) ;
}
static const char *u_objet(const void *o)
{
  static Chaine s ;
  Objet *o_ = OBJET(o) ;

  s_efface(&s) ;
  s_printf(&s, "<A HREF=\"objet?objet=%d\">%s</A>", o, o_->classe->nom) ;
  return(s_char(&s)) ;
}
static const char *u_type_logique(const Type_logique *tl)
{
  static Chaine s ;

  if ( tl==NULL )
    return("???") ;
  s_efface(&s) ;
  s_printf(&s,
	   "``<A HREF=\"type_logique?type_logique=%ld\">%s</A>''",
	   (long)tl, remplace_code(tl->nom)
	   ) ;
  return(s_char(&s)) ;
}
static char *u_prend_argument(char **args, const char *nom)
{
  int i ;
  for( i=0; args[i]&&args[i+1] ; i += 2 )
    if ( strcmp(nom, args[i]) == 0 )
      return( args[i+1] ) ;
  return(NULL) ;
}
static void *u_prend_entier(char **args, const char *nom)
{
  char *v ;
  v = u_prend_argument(args, nom) ;
  if ( v )
    return(  (void*) atol(v) ) ;
  else
    return(NULL) ;
}
static Classe *u_prend_classe(char **args)
{
  long i = (long)u_prend_entier(args, "classe") ;
  if ( i>=0 && i<nombre_de_classes )
    return( &les_classes[i] ) ;
  else
    return(NULL) ;
}
static Champ *u_prend_champ(char **args, const Classe *cl)
{
  long i ;

  if ( cl==NULL )
    return(NULL) ;
  i = (long)u_prend_entier(args, "champ") ;
  POUR_TOUS_LES___CHAMPS(cl, ch,
		       if ( ch->index==i ) return(ch) ;
		       ) ;
  return(NULL) ;
}
/*
 ******************************************************************************
 * Partie de page
 ******************************************************************************
 */
static void u_debut(FILE *c, char *label)
{
  fprintf(c,
	  "<HTML><HEAD><TITLE>%s</TITLE></HEAD><BODY><H1>%s</H1>\n",
	  label, label) ;
}

static void u_fin(FILE *c)
{
  fprintf(c,
	  "<HR>Auteur : <A HREF=\"Welcome\">NanoEdit</A> * "
	  "<A HREF=\"reinitialise\">Réinitialisation</A> * "
	  "<A HREF=\"hp\">Hiérarchie des classes</A> * "
	  "<A HREF=\"tl\">Hiérarchie types logiques</A> * "
	  "<A HREF=\"tp\">Les types physiques (codec)</A> * "
	  "<A HREF=\"memoire\">Utilisation Mémoire</A> * "
	  "<A HREF=\"action\">Actions en cours</A> * "
	  "<A HREF=\"stattype\">Stats CT</A> * "
	  "</BODY></HTML>\n"
	  ) ;
}
/*
 *
 */
static void u_affiche_objet_html(FILE *c, void *o)
{
  Booleen deuxieme_entete_fait, affiche_entete ;
  Objet *o_ ;

  o_ = OBJET(o) ;
  deuxieme_entete_fait = Faux ;
  affiche_entete = Faux ;
  POUR_TOUS_LES_CHAMPS
    (o, ch,
     {
       /* eprintf("%s/%s\n", o_->classe->nom, ch->nom_champ) ; */
       if ( ch->index==0 )
	 {
	   fprintf(c,"Champs communs à toutes les classes :\n") ;
	   affiche_entete = Vrai ;
	 }
       if ( !CHAMP_OBJET(ch) && !deuxieme_entete_fait )
	 {
	   fprintf(c,"</TABLE>\n") ;
	   fprintf(c,"<P>Champs particulier à cette classe :\n") ;
	   affiche_entete = Vrai ;
	   deuxieme_entete_fait = Vrai ;
	 }
       if ( affiche_entete )
	 {
	   fprintf(c,
		   "<TABLE BORDER><TR><TH>Nom<TH>Type Logique<BR>"
		   "Type Physique<BR>"
		   "Type Edition<TH>Valeur<br>Défaut"
		   "<TH>Valeur<br>courante<TH>Commentaire\n") ;
	   affiche_entete = Faux ;
	 }
       fprintf(c, "<TR><TD>%s\n", u_champ_objet(ch,o)) ;
       fprintf(c, "<TD>%s<BR>", u_type_logique(ch->tl)) ;
       fprintf(c, "%s<BR>", u_chaine(ch->tp->nom)) ;
       fprintf(c, "%s", u_chaine(ch->te->nom)) ;
       fprintf(c, "<TD>%s", u_chaine(ch->defaut)) ;
       fprintf(c, "<TD>%s", u_chaine(c_recupere_ici_sans_copie(o,ch))) ;
       fprintf(c, "<TD>%s\n", u_chaine(ch->bulle_d_aide?ch->bulle_d_aide:"")) ;
     }
     ) ;
  fprintf(c, "</TABLE>\n") ;
  fprintf(c, "<P>Ses fils :\n") ;
  POUR_LES_FILS_(o_ ,f, fprintf(c, "%s *\n", u_objet(f)) ; ) ;
  fprintf(c, "<P>Ses parents :\n") ;
  POUR_LES_PARENTS_(o_ ,p, fprintf(c, "%s *\n", u_objet(p)) ; ) ;
  fprintf(c, "<P>Pour cet objet :\n<UL>\n") ;
  fprintf(c, "<LI><A HREF=\"detruire?objet=%ld\">Détruire</A>.\n", (long)o) ;
  fprintf(c, "<LI><A HREF=\"active?objet=%ld\">%s</A>.\n",
	   (long)o, ACTIF_(o_) ? "Inactivation" : "Activation") ;
  fprintf(c, "<LI><A HREF=\"edite?objet=%ld\">%s fenêtre d'édition</A>.\n",
	   (long)o, o_->edite ? "Ferme" : "Ouvre") ;
  fprintf(c, "</UL>\n") ;
}
/*
 *
 */
static void u_page_tl(FILE *c, const Type_logique *tl)
{
  if ( tl->surclasse )
    fprintf(c, "Il a comme surclasse le type logique %s.<br>\n",
	    u_type_logique(tl->surclasse)) ;
  else
    fprintf(c, "Il n'a pas de surclasse.<br>\n") ;
  if ( tl->predicat_verifie )
    {
      fprintf(c, "Il doit vérifier le prédicat %s",
	      u_chaine(tl->predicat_verifie_nom)) ;
      fprintf(c, "(%p).<br>\n",
	      u_ptr((void*)tl->predicat_verifie)) ;
    }
  if ( tl->sous_classe )
    {
      fprintf(c, "Il est ``sous-classable'' par %s\n",
	      u_chaine(tl->sous_classe_nom)) ;
      fprintf(c, "(%p).<br>\n",
	      u_ptr((void*)tl->sous_classe)) ;
    }
}
/*
 *
 */
#define TSTO(XX) {							\
                   if ( ch->options & XX )				\
                      fprintf(c, " %s", u_chaine(GUILLEMET(XX)+2) ) ;	\
                 }

static void u_options(FILE *c, Champ *ch)
{
  TSTO(O_Edite      ) ;
  TSTO(O_Extrait    ) ;
  TSTO(O_Affiche    ) ;
  TSTO(O_Cree       ) ;
  TSTO(O_Label      ) ;
  TSTO(O_Sauve      ) ;
  TSTO(O_Obligatoire) ;
  TSTO(O_InitChaine ) ;
  TSTO(O_Init3d     ) ;
  TSTO(O_Initialiser) ;
  TSTO(O_Virtuel    ) ;
}
/*
 ******************************************************************************
 * PAGES
 ******************************************************************************
 */
#define ERREUR_SI_NULL(V)			\
{						\
  if ( V==NULL )				\
    {						\
    u_mauvaise_url(c,url,a,args,fildes) ;	\
    return ;					\
    }						\
}
/*
 *
 */

Booleen u_nom_correcte(const char *nom)
{
  int i ;
  
  for(i=0 ; nom[i] ; i++ )
    if ( isdigit(nom[i]) || islower(nom[i]) || nom[i]=='_' )
      continue ;
    else
      return(Faux) ;
  return(Vrai) ;
}

int surclasse(const Classe *a, const Classe *b)
{
  if ( a == b )
    return(1) ;
  if ( a == NULL )
    return(0) ;
  return( surclasse(a->sc, b) ) ;
}

void u_verifie_coherence_champs(FILE *c, Interface *interface)
{
  char *e ;
  int le ;

  fprintf(c,"<H2>Erreurs de logique dans les définitions de classes :</H2>\n");

  if ( (sizeof(Objet)) % sizeof(double) )
    {
      eprintf("Taille objet non multiple taille du double\n") ;
      exit(1) ;
    }

  POUR_LES_CLASSES
    (cl,
     {
       e = strdup(decode_evenement(cl->evenement)) ;
       le = strlen(e) ;
       POUR_LES_CLASSES
	 (cl2,
	  {
	    if ( surclasse(cl, cl2) || surclasse(cl2, cl) )
	      continue ;
	    if ( cl2->sc && strcmp(cl2->sc->evenement, cl2->evenement) == 0 )
	      continue ;
	    if ( strncmp(e, decode_evenement(cl2->evenement), le) )
	      continue ;
	    fprintf(c
		    , "Conflit de raccourcis clavier %s(%s) %s(%s)<br>\n"
		    , cl->nom, e
		    , cl2->nom, decode_evenement(cl2->evenement)
		    ) ;
	  }) ;
       free(e) ;

       POUR(Champ*, ch, cl->champs,
	    {
	      POUR(Champ*, ch2, cl->champs,
		   {
		     if ( (long)ch >= (long)ch2 )
		       continue ;
		     if ( strcmp(ch->nom_champ, ch2->nom_champ) == 0 )
		       fprintf(c,
			       "%s/ Des champs ont le même nom (%s,%s)<br>\n",
			       u_classe(cl),  u_champ(ch), u_champ(ch2)) ;
		   }
		   );
	      if ( ch->taille != ch->tp->taille && ch->tp->taille!=0 )
		fprintf(c,
			"%s/%s Incohérence de taille entre\n"
			"la_structure C (%d) et le type physique (%d).<br>\n"
			,
			u_classe(cl),
			u_champ(ch),
			ch->taille,
			ch->tp->taille
			) ;
	      if ( !u_nom_correcte(ch->nom_champ) )
		fprintf(c,
			"%s/%s Le nom du champ contient\n"
			"autre chose que [0-9a-z_]<br>\n"
			,
			u_classe(cl),
			u_champ(ch)
			) ;
	      if ( !ch->tl->predicat_verifie ^ !ch->tl->predicat_verifie_nom )
		fprintf(c,
			"%s/%s/%s\n"
			"Le type logique a un prédicat sans nom.<br>\n"
			,
			u_classe(cl),
			u_champ(ch),
			ch->tl->nom
			) ;
	      if ( !ch->tl->sous_classe ^ !ch->tl->sous_classe_nom )
		fprintf(c,
			"%s/%s/%s\n"
			"Le type logique a un sous-classage sans nom.<br>\n"
			,
			u_classe(cl),
			u_champ(ch),
			ch->tl->nom
			) ;

	      if ( strcmp(ch->nom_champ, cl->nom)==0 )
		fprintf(c,
			"%s/%s Nom du champ est égal au nom de la classe<br>\n"
			,
			u_classe(cl),
			u_champ(ch)
			) ;
	      if ( ch->index > (int)sizeof(Bits)*8-2 )
		fprintf(c,
			"%s/%s Je ne supporte pas assez de champs\n<br>"
			,
			u_classe(cl),
			u_champ(ch)
			) ;
	      if ( cl->affiche_champs_classe && !CHAMP_OBJET(ch) )
		POUR_LES_CHAMPS_OBJET
		  (co,
		   {
		     if ( strcmp(co->nom_champ, ch->nom_champ)== 0 )
		       fprintf(c,
			       "%s/%s Nom de champ réservé\n<br>"
			       ,
			       u_classe(cl),
			       u_champ(ch)
			       ) ;
		   }
		   ) ;
	    }
	    ) ;
     }
     );
}
/*
 *
 */
static void u_liste_objet(FILE *c, void *a)
{
  Objet *a_ = OBJET(a) ;

  if ( a_->flag )
    {
      fprintf(c, "<LI>%s déjà fait.<br>\n", u_objet(a)) ;
      return ;
    }
  a_->flag = 1 ;
  fprintf(c, "<LI>%s %s\n", u_objet(a), o_informations(a)) ;
  if ( a_->fils )
    {
      fprintf(c, "<UL>\n") ;
      POUR_LES_FILS_(a_, f, u_liste_objet(c, f) ; ) ;
      fprintf(c, "</UL>\n") ;
    }
  if ( EST_OPERATEUR(a) || a == u_modele() )
    {
      fprintf(c, "<p>Contenu opérateur : <UL>\n") ;
      POUR(void*, f, CONTENU(&((Modele*)a)->oper), u_liste_objet(c, f) ; ) ;
      fprintf(c, "</UL>\n") ;
    }
}
/*
 *
 */
static Parcour u_affiche_actif(void *o, void *donnee)
{
  Objet *o_ = OBJET(o) ;

  if ( ACTIF_(o_) )
    {
      fprintf((FILE*)donnee, "%s(%d) *\n", u_objet(o), ACTIF_(o_)) ;
    }
  return(Continue) ;
}

static void hierarchie_classe(FILE *c, Classe *cl)
{
  int un ;

  un = 0 ;
  fprintf(c, "<LI> %s (%s, %s)\n", u_classe(cl), decode_evenement(cl->evenement), cl->menu) ;

  POUR_LES_CLASSES(cf,
		   {
		   if ( cf->sc != cl )
		     continue ;
		   if ( un == 0 )
		     {
		       fprintf(c, "<UL>\n") ;
		       un = 1 ;
		     }
		   hierarchie_classe(c, cf) ;
		   }) ;
  if ( un )
    fprintf(c, "</UL>\n") ;
}

static void u_hier_classe(FILE *c, char *url, void *a, char **args, int fildes)
{
  u_debut(c, "Hiérarchie des classes d'objet de NanoÉdit") ;
  fprintf(c, "<UL>\n") ;
  POUR_LES_CLASSES
    (cl,
     {
       if ( cl->sc == NULL )
	 hierarchie_classe(c, cl) ;
     }
     ) ;
  fprintf(c, "</UL>\n") ;
  u_fin(c) ;
}

static void hierarchie_tl(FILE *c, const Type_logique *tl, const void **t, int nb)
{
  int un ;
  int i ;

  un = 0 ;
  fprintf(c, "<LI> %s", u_type_logique(tl)) ;
  if ( tl->predicat_verifie )
    fprintf(c, ", Prédicat(%s)", tl->predicat_verifie_nom) ;
  if ( tl->sous_classe )
    fprintf(c, ", Sous_classable(%s)", tl->sous_classe_nom) ;

  fprintf(c, ", <EM>%s</EM>\n", tl->bulle_d_aide?tl->bulle_d_aide:"") ;
  for(i=0;i<nb;i++)
    {
      if ( ((const Type_logique*)t[i])->surclasse == tl )
	{
	  if ( un == 0 )
	    {
	      fprintf(c, "<UL>\n") ;
	      un = 1 ;
	    }
	  hierarchie_tl(c, t[i], t, nb) ;
	}
    }
  if ( un )
    fprintf(c, "</UL>\n") ;
}

static void ajoute_a_la_table(const void ***tl, int *nb,
			      const void *t)
{
  int i ;

  if ( t==NULL )
    return ;
  for(i=0;i<*nb;i++)
    if ( (*tl)[i] == t )
      return ;			/* Déjà dedans */

  REALLOUER(*tl, *nb+1) ;
  (*tl)[*nb] = t ;
  (*nb)++ ;
}

static void creation_table_tl(const void ***tl, int *nb)
{
  int i ;

  *tl = NULL ;
  *nb = 0 ;
  POUR_LES_CLASSES
    (cl,
     POUR_TOUS_LES___CHAMPS(cl, ch,
			    ajoute_a_la_table(tl, nb, ch->tl) ;
			    ajoute_a_la_table(tl, nb, ch->type_dans_fils) ;
			    ) ;
     ) ;
  for(i=0;i<*nb;i++)
    ajoute_a_la_table(tl, nb, ((const Type_logique*)(*tl)[i])->surclasse) ;
}

static void u_hier_tl(FILE *c, char *url, void *a, char **args, int fildes)
{
  const void **tl ;
  int nb ;
  int i ;

  u_debut(c, "Hiérarchie des types logiques") ;
  creation_table_tl(&tl, &nb) ;

  fprintf(c, "<UL>\n") ;
  for(i=0;i<nb;i++)
    if ( ((const Type_logique*)tl[i])->surclasse == NULL )
      hierarchie_tl(c, tl[i], tl, nb) ;
  free(tl) ;
  fprintf(c, "</UL>\n") ;
  u_fin(c) ;
}

static void liste_des_demos(FILE *c)
{
  FILE *a ;
  char *debut, *courant, *fin, *debut_title, *fin_title ;
  static Chaine s ;
  Buffer tmp ;
  
  fprintf(c, "<br>Ou assistez à des démonstrations (avec Mozilla)\n"
	  "parmi la liste suivante&nbsp;:\n"
	  "<UL>") ;

  debut = strdup(directories_objets) ;
  for( courant=debut; courant; courant = (fin?fin+1:NULL) )
    {
      fin = strchr(courant,' ') ;
      if ( fin )
	*fin = '\0' ;

      s_efface(&s) ;
      s_printf(&s, "%s/DEMO/Welcome.html", courant) ;
      a = fopen(s_char(&s), "r") ;
      if ( a==NULL )
	continue ;
      tmp[ fread(tmp,1,sizeof(tmp)-1,a) ] = '\0' ;
      fclose(a) ;

      while( strchr(s_char(&s),'/') ) *strchr(s_char(&s),'/') = ':' ;

      fprintf(c,"<LI> <A HREF=\"fichier%%3A%s\">", s_char(&s)) ;

      debut_title = strstr(tmp, "<TITLE>") ;
      if ( debut_title==NULL )
	debut_title = strstr(tmp, "<title>") ;
      if ( debut_title==NULL )
	{
	  fprintf(c, "%s", s_char(&s)) ;
	}
      else
	{
	  debut_title += 7 ;
	  fin_title = strstr(debut_title, "</TITLE>") ;
	  if ( fin_title == NULL )
	    fin_title = strstr(debut_title, "</title>") ;
	  if ( fin_title == NULL )
	    fin_title = strchr(debut_title, '<') ;
	  if ( fin_title == NULL )
	    fin_title = debut_title+80 ;
	  *fin_title = '\0' ;

	  fprintf(c, "%s", debut_title) ;
	}
      fprintf(c, "</A>\n") ;
    }
  fprintf(c, "</UL>\n") ;
}

static void u_page_accueil(FILE *c, char *url, void *a, char **args, int fildes)
{
  u_debut(c, "Bienvenue sur NanoÉdit") ;

  fprintf(c, "<big><strong>Allez voir le <A HREF=\"fichier/COURS/Welcome.html\">"
	  "cours d'utilisation de NanoÉdit</A> avant\n"
	  "de vous amuser</strong></big>.\n"
	  "<p>Si les fen&ecirc;tres se d&eacute;placent toute seule\n"
	  "alors changez de window manager.\n"
	 ) ;

  if ( ((Interface*)a)->http_ecrit )
    liste_des_demos(c) ;

  fprintf(c, "<H2>Modèle :</H2>\n") ;
  u_liste_objet(c, a) ;
  o_annule_flag(a) ;
  u_verifie_coherence_champs(c, a) ;
  fprintf(c, "<H2>Liste des actifs :</H2>\n") ;
  o_parcour( a, NULL, u_affiche_actif, c) ;  
  u_fin(c) ;
}
/*
 *
 */
static void u_mauvaise_url(FILE *c, char *url, void *a, char **args, int fildes)
{
  u_debut(c, "Mauvaise URL") ;
  fprintf(c, "URL : <PRE>%s</PRE><p>", remplace_code(url) ) ;
  for( ; args[0] && args[1] ; args += 2 )
    {
      fprintf(c, "%s", remplace_code(args[0])) ;
      fprintf(c, " = %s<br>", remplace_code(args[1])) ;
    }
  u_fin(c) ;
}
/*
 *
 */
static void u_page_objet(FILE *c, char *url, void *a, char **args, int fildes)
{
  Buffer nom ;
  void *ob = u_prend_entier(args, "objet") ;

  ERREUR_SI_NULL(ob) ;
  sprintf(nom, "Instance de la classe %s\n", u_classe(OBJET(ob)->classe)) ;
  u_debut(c, nom) ;
  u_affiche_objet_html(c, ob) ;
  u_fin(c) ;
}
/*
 *
 */
static void u_page_reinitialise(FILE *c, char *url, void *a,
				char **args, int fildes)
{
  c_sauve(a, "stop", "1") ;
  u_debut(c,"Réinitialisation NanoEdit en cours\n") ;
  u_fin(c) ;
}
static void u_page_exit(FILE *c, char *url, void *a,
				char **args, int fildes)
{
  Interface *i ;

  u_debut(c,"Au revoir\n") ;
  u_fin(c) ;
  fclose(c) ;

  i = a ;
  close(i->socket) ;

  exit(0) ;
}
/*
 *
 */
static void u_creer(FILE *c, char *url, void *a, char **args, int fildes)
{
  Classe *cl = u_prend_classe(args) ;

  ERREUR_SI_NULL(cl) ;
  o_il_devient_mon_fils(u_modele(a), o_objet(cl), Faux) ;

  evaluation(a) ;
  u_page_accueil(c, url, a, args, fildes) ;
}
/*
 *
 */
static void u_active(FILE *c, char *url, void *a, char **args, int fildes)
{
  void *ob = u_prend_entier(args, "objet") ;

  ERREUR_SI_NULL(ob) ;
  o_bascule_actif(ob) ;
  evaluation(a) ;
  u_page_accueil(c, url, a, args, fildes) ;
}
/*
 *
 */
static void u_edite(FILE *c, char *url, void *a, char **args, int fildes)
{
  void *ob = u_prend_entier(args, "objet") ;

  ERREUR_SI_NULL(ob) ;

  ouvrir_fenetre(ob) ;
  u_page_accueil(c, url, a, args, fildes) ;
}
/*
 *
 */
static void u_detruire_objet(FILE *c, char *url, void *a, char **args, int fildes)
{
  void *ob = u_prend_entier(args, "objet") ;
  if ( ob )
    detruire_objet(ob) ;
  u_page_accueil(c, url, a, args, fildes) ;
}
/*
 *
 */
static void u_ecrit(FILE *c, char *url, void *a, char **args, int fildes)
{
  void *o = (void*)atol(url) ;
  char *champ = strstr(url,"_") + 1 ;
  char *valeur = u_prend_argument(args, "valeur") ;

  ERREUR_SI_NULL(valeur) ;
  ERREUR_SI_NULL(champ) ;
  ERREUR_SI_NULL(o) ;

  c_sauve(o, champ, valeur) ;
  evaluation(a) ;

  args[2] = "objet" ;
  args[3] = (char *)int_vers_char((long)o) ;
  u_page_objet(c, url, a, args, fildes) ;
}
/*
 *
 */
static void u_page_type_logique(FILE *c, char *url, void *a, char **args, int fildes)
{
  Buffer nom ;
  Type_logique *tl =  u_prend_entier(args, "type_logique") ;

  ERREUR_SI_NULL(tl) ;
  sprintf(nom, "Type logique ``%s''", u_chaine(tl->nom)) ;
  u_debut(c, nom) ;
  u_page_tl(c, tl) ;
  u_fin(c) ;
}

static void u_codec(FILE *c, const Codec_informations *t)
{
  fprintf(c, "Nom : %s<br>\n",t->nom) ;
  fprintf(c, "Structure C : %s<br>\n",t->nom_structure) ;
  fprintf(c, "Taille : %d<br>\n",t->taille) ;
  fprintf(c, "Recupere_texte : %s<br>\n",t->recupere_texte_nom) ;
  fprintf(c, "Sauve_texte : %s<br>\n",t->sauve_texte_nom) ;
  fprintf(c, "Libere : %s<br>\n",t->libere_nom) ;
}

/*
 *
 */
static void u_page_champ(FILE *c, char *url, void *a, char **args, int fildes)
{
  Buffer nom, tmp ;
  Classe *cl ;
  const void *o, *o2 ;
  Champ *ch, *ch2 ;
  const void **tl ;
  int nb, i ;


  cl = u_prend_classe(args) ;
  o = u_prend_entier(args,"objet") ;
  if ( cl==NULL && o!=NULL )
    cl = (Classe*)OBJET(o)->classe ;
  ch = u_prend_champ(args, cl) ;

  ERREUR_SI_NULL(ch) ;

  strcpy(tmp, u_chaine(ch->nom_champ)) ;
  sprintf(nom, "Champ %s de la classe %s.\n", ch->nom_champ, u_classe(cl)) ;
  u_debut(c, nom) ;

  if ( strstr(url," ") )
    {
      /*
       * Un message d'erreur existe
       */
      fprintf(c, "<TABLE><TR><TD BGCOLOR=red>%s</TABLE>\n",
	      u_chaine(strstr(url," ")+1)
	      ) ;
    }

  if ( ch->options )
    {
      fprintf(c, "Les options de ce champ sont :") ;
      u_options(c, ch) ;
      fprintf(c, ".\n") ;
    }

  fprintf(c, "Le nom du champ dans la structure C est %s.\n",
	  u_chaine(ch->Cnom_champ)) ;
  fprintf(c, "Sa taille est de %s octets,\n",
	  u_entier(ch->taille));
  fprintf(c, " sa position dans la structure est %s\n",
	  u_entier(ch->position)) ;
  fprintf(c, " sa valeur par défaut est (%s).<p>\n",
	  u_chaine(ch->defaut)) ;
  
  if ( (EXTRAIT(ch) || OBLIGATOIRE(ch)) && ch->classe_parametre==NULL )
    {
      
      fprintf(c, "Le champ est obligatoire ou extrait,\n") ;
      fprintf(c, "le type dans le fils est %s\n",
	      u_chaine(ch->type_dans_fils ?
		       ch->type_dans_fils->nom :
		       "???" )) ;
      fprintf(c, "et est extrait du fils %s (compté à partir de 0)\n",
	      u_entier(ch->numero_fils) ) ;
    }
  if ( ch->classe_parametre )
    {
      fprintf(c,
	      "La valeur du champ est un pointeur sur l'objet "
	      "de type %s parmi les fils.\n",
	      u_chaine(ch->classe_parametre)) ;
    }
  if ( ch->label )
    {
      fprintf(c, "La label du widget est %s.\n", u_chaine(ch->label)) ;
    }

  if (o)
    {
      fprintf(c, "<FORM ACTION=\"ecrit%ld_%s\">\n", (long)o, ch->nom_champ) ;
      fprintf(c, "Cette instance de champ est dans %s.\n", u_objet(o)) ;
      fprintf(c, "Sa valeur est : <PRE>%s</PRE>",
	      remplace_code( c_recupere_ici_sans_copie(o, ch) )
	      );
      fprintf(c, "Entrez sa nouvelle valeur : <INPUT NAME=\"valeur\">\n" ) ;
      fprintf(c, "</FORM>\n") ;
    }

  fprintf(c, "<table border><tr valign=top>\n") ;

  if ( ch->tl )
    {
      fprintf(c, "<td><b>Type Logique</b><br>%s<hr>\n",
	      u_type_logique(ch->tl)) ;

      u_page_tl(c, ch->tl) ;
    }

  fprintf(c, "<td><b>Type Physique</b><br>``%s''<hr>\n",
	  u_chaine(ch->tp->nom) ) ;

  u_codec(c, ch->tp) ;

  fprintf(c, "<td><b>Type d'édition</b><br>``%s''<hr>\n",u_chaine(ch->te->nom)) ;
  fprintf(c, "Les méthodes manipulant les Widgets sont :<br>\n") ;
  fprintf(c, "%s.<br>\n", u_chaine(ch->te->creer_nom)) ;
  fprintf(c, "%s.<br>\n", u_chaine(ch->te->lit_nom)) ;
  fprintf(c, "%s.<br>\n", u_chaine(ch->te->ecrit_nom)) ;
  fprintf(c, "%s.<br>\n", u_chaine(ch->te->init_nom)) ;
  fprintf(c, "%s.<br>\n", u_chaine(ch->te->entree_nom)) ;
  fprintf(c, "%s.<br>\n", u_chaine(ch->te->mouvement_nom)) ;
  fprintf(c, "%s.<br>\n", u_chaine(ch->te->aide_nom)) ;
  fprintf(c, "%s.<br>\n", u_chaine(ch->te->clavier_nom)) ;
  fprintf(c, "%s.<br>\n", u_chaine(ch->te->start_classe_nom)) ;
  fprintf(c, "%s.<br>\n", u_chaine(ch->te->stop_classe_nom)) ;

  fprintf(c, "</table><p>\n") ;
  fprintf(c, "Retour à la Classe %s<P>", u_classe(cl)) ;

  if ( o )
    {
      fprintf(c,
	      "Ce champ peut être considéré dans les classes logiques :\n") ;
      
      creation_table_tl(&tl, &nb) ;
      for(i=0;i<nb;i++)
	{
	  o2 = o ;
	  ch2 = ch ;
	  if ( appartient(&o2,
			  (const Champ**)&ch2,
			  (const Type_logique*)tl[i]) )
	    {
	      fprintf(c, " %s", u_type_logique((const Type_logique*)tl[i])) ;
	    }
	}
      free(tl) ;
    }
  

  u_fin(c) ;
}
/*
 *
 */
static void u_page_definition(FILE *c, char *url, void *a, char **args, int fildes)
{
  Buffer nom ;
  const Mot_clef *def ;
  Classe *cl = u_prend_classe(args) ;

  ERREUR_SI_NULL(cl) ;
  sprintf(nom, "Définition de la classe %s", u_classe(cl)) ;
  u_debut(c, nom) ;

  for(def=cl->definition; *def!=MC_fin_definition; def++)
    {
      fprintf(c, "Commande %s", u_entier(def[0])) ;
      if ( *def < MC_debut_champ )
	{
	  fprintf(c, "  Argument %s", u_entier(def[1])) ;
	  def++ ;
	}
      fprintf(c, "<br>\n") ;	
    }

  fprintf(c, "<HR>\n") ;
  fprintf(c, "Retour à la classe %s\n", u_classe(cl) ) ;
  u_fin(c) ;
}
/*
 *
 */
static void u_page_classe(FILE *c, char *url, void *a, char **args, int fildes)
{
  Buffer nom ;
  int num_ch ;
  Classe *cl = u_prend_classe(args) ;

  ERREUR_SI_NULL(cl) ;

  sprintf(nom, "Classe %s", cl->nom) ;
  u_debut(c, nom) ;
  fprintf(c, "%s\n<P>\n", cl->bulle_d_aide?cl->bulle_d_aide:"") ;

  fprintf(c,
	  "La <A HREF=\"definition?classe=%d\">définition "
	  "interprétée</A> de la classe %s",
	  cl->index,
	  u_chaine(cl->nom)
	  ) ;
  fprintf(c, "(%s:%d).\n", u_fichier(cl->fichier), cl->ligne) ;
  if ( cl->sc )
    fprintf(c, "(dont la surclasse est %s)\n", u_classe(cl->sc)) ;
  fprintf(c,"Elle utilise la structure C nommée %s\n", u_chaine(cl->Cstruct)) ;
  fprintf(c,"occupant %s octets.\n", u_entier(cl->taille_structure)) ;
  if ( cl->destruction )
    fprintf(c,"Lors de la destruction, la fonction %s "
	    "est appelée pour terminer la libération des ressources.\n",
	    u_chaine(cl->destruction_nom)) ;
  if ( cl->changement )
    fprintf(c,"La fonction %s est appelée en cas de changement\n"
	    "de l'un des champs de la structure ou l'un des paramètres.\n",
	    u_chaine(cl->changement_nom));
  if ( cl->informations )
    fprintf(c,"Pour obtenir un résumé (texte) du contenu de l'objet"
	    "on appel la fonction %s.\n",
	    u_chaine(cl->informations_nom));
  if ( !cl->affiche_champs_classe )    
    fprintf(c,"<b>Lors de l'édition de l'objet les\n"
	    "champs par défaut (OK, FERME, classe, ...)\n"
	    "ne sont pas affichés</b>.\n") ;
  fprintf(c, "Lors de la création, le nombre de fils adoptés est ") ;
  switch(cl->paquet)
    {
    case NB_NON_DEFINI:
      fprintf(c,"fixé interactivement par l'utilisateur.\n") ;
      break ;
    case NB_ACTIFS:
      fprintf(c, "égal au nombre d'objet actifs.\n") ;
      break ;
    default:
      fprintf(c, "égal a %s.\n", u_entier(cl->paquet)) ;
      break ;
    }
  fprintf(c,
	  "Une fois l'adoption faite, on se décale dans la liste des actifs ");
  switch(cl->decalage)
    {
    case NB_NON_DEFINI:
      fprintf(c,"d'un nombre fixé interactivement par l'utilisateur.\n") ;
      break ;
    case NB_ACTIFS:
      fprintf(c, "d'un nombre égal au nombre d'objet actifs.\n") ;
      break ;
    default:
      fprintf(c, "de %s.\n", u_entier(cl->decalage)) ;
      break ;
    }
  if ( cl->menu )
  fprintf(c, "Le chemin dans le menu de création est : %s.\n",
	  u_chaine(cl->menu)) ;
  if ( cl->evenement )
  fprintf(c, "Le raccourci de création est : %s.\n",
	  u_chaine(cl->evenement)) ;
  num_ch = 0 ;
  POUR_LES_CLASSES(c, if ( c->sc == cl ) { num_ch = 1 ; break ; } ) ;
  if ( num_ch )
    {
      fprintf(c, "Les sous-classes sont :") ;
      POUR_LES_CLASSES(classe,
		       {
			 if ( classe->sc == cl )
			     fprintf(c," %s\n", u_classe(classe)) ;
		       }
		       ) ;
    }

  
  fprintf(c,
	  "<P><A HREF=\"creer&classe=%d\">"
	  "Créer une Instance (sans paramètre) de cette classe</A>\n",
	  cl->index ) ;
	    
  fprintf(c,"<h2>Les champs de la classe</h2>\n") ;
  fprintf(c,"<TABLE BORDER><TR><TH>Nom<TH>Options\n") ;
  fprintf(c,"<TH>Défaut<TH>Type Logique<BR>Type Physique<BR>Type Édition<TH>Aide\n") ;

  num_ch = 0 ;

  POUR(Champ*, ch, cl->champs,
       {
	 fprintf(c, "<TR><TD>%s<BR>(%d)\n", u_champ(ch), ch->index) ;
	 fprintf(c, "<TD>") ;
	 u_options(c, ch) ;
         fprintf(c, "<TD>%s\n"    ,       u_chaine(ch->defaut      )) ;
         fprintf(c, "<TD>%s<BR>\n", u_type_logique(ch->tl          )) ;
         fprintf(c, "%s<BR>\n"    ,       u_chaine(ch->tp->nom     )) ;
         fprintf(c, "%s\n"        ,       u_chaine(ch->te->nom     )) ;
         fprintf(c, "<TD>%s\n"    ,       u_chaine(ch->bulle_d_aide?ch->bulle_d_aide:"")) ;
       }
       ) ;

  fprintf(c, "</TABLE>\n") ;
  fprintf(c, "<HR>\n") ;
  if ( cl->index>0 )
    fprintf(c, "Classe précédente %s</A>.\n",
	    u_classe(&les_classes[cl->index-1])) ;
  if ( cl->index<nombre_de_classes-1 )
    fprintf(c, "Classe suivante %s</A>.\n",
	    u_classe(&les_classes[cl->index+1])) ;
  u_fin(c) ;
}


static void dump_classe(FILE *c, Classe *cl)
{
  static int prof = 0 ;

  fprintf(c, "%*s%s\n", prof, "", cl->nom) ;
  prof += 4 ;
  POUR_LES_CLASSES(cf,
		   {
		   if ( cf->sc != cl )
		     continue ;
		   dump_classe(c, cf) ;
		   }) ;
  prof -= 4 ;
}


static void dump_tl(FILE *c, const Type_logique *tl, const void **t, int nb)
{
  int i ;
  static int prof = 0 ;

  prof += 4 ;
  fprintf(c, "%*s%s", prof, "", tl->nom) ;
  if ( tl->predicat_verifie )
    fprintf(c, ", Prédicat(%s) ", tl->predicat_verifie_nom) ;
  if ( tl->sous_classe )
    fprintf(c, ", Sous_classable(%s)", tl->sous_classe_nom) ;
  fprintf(c, ", %s\n", tl->bulle_d_aide?tl->bulle_d_aide:"") ;

  for(i=0;i<nb;i++)
    {
      if ( ((const Type_logique*)t[i])->surclasse == tl )
	{
	  dump_tl(c, t[i], t, nb) ;
	}
    }
  prof -= 4 ;
}

static void dump_te(FILE *c, const Type_editeur *tl, const void **t, int nb)
{
  int i ;
  static int prof = 0 ;

  prof += 4 ;
  fprintf(c, "%*s%s\n", prof, "", tl->nom) ;
  for(i=0;i<nb;i++)
    {
      if ( ((const Type_editeur*)t[i])->surclasse == tl )
	{
	  dump_te(c, t[i], t, nb) ;
	}
    }
  prof -= 4 ;
}

static void dump_tp(FILE *c, const Codec_informations *tl)
{
  fprintf(c, "%-37s (C: %-25s, %3d octets)\n",
	  tl->nom,
	  tl->nom_structure,
	  tl->taille
	  ) ;
}


static void dump()
{
  const void **tl ;
  int nb ;
  int i ;
  FILE *f ;
  /*
   *
   */
  f = fopen("dump.classe","w") ;
  POUR_LES_CLASSES
    (cl,
     {
       if ( cl->sc == NULL )
	 {
	   fprintf(f, "\\begin{verbatim}\n") ;
	   dump_classe(f, cl) ;
	   fprintf(f, "\\end{verbatim}\n") ;
	 }
     }
     ) ;
  fclose(f) ;
  /*
   *
   */
  f = fopen("dump.tl","w") ;
  creation_table_tl(&tl, &nb) ;

  fprintf(f, "\\begin{verbatim}\n") ;
  for(i=0;i<nb;i++)
    if ( ((const Type_logique*)tl[i])->surclasse == NULL )
      dump_tl(f, tl[i], tl, nb) ;
  free(tl) ;
  fprintf(f, "\\end{verbatim}\n") ;
  fclose(f) ;
  /*
   *
   */
  f = fopen("dump.tp","w") ;
  tl = NULL ;
  nb = 0 ;
  POUR_LES_CLASSES
    (cl,
     POUR_TOUS_LES___CHAMPS(cl, ch,
			    ajoute_a_la_table(&tl, &nb, ch->tp) ;
			    ) ;
     ) ;

  fprintf(f, "\\begin{verbatim}\n") ;
  for(i=0;i<nb;i++)
    dump_tp(f, tl[i]) ;
  free(tl) ;
  fprintf(f, "\\end{verbatim}\n") ;
  fclose(f) ;
  /*
   *
   */
  f = fopen("dump.te","w") ;
  tl = NULL ;
  nb = 0 ;
  POUR_LES_CLASSES
    (cl,
     POUR_TOUS_LES___CHAMPS(cl, ch,
			    ajoute_a_la_table(&tl, &nb, ch->te) ;
			    ) ;
     ) ;

  fprintf(f, "\\begin{verbatim}\n") ;
  for(i=0;i<nb;i++)
    if ( ((const Type_editeur*)tl[i])->surclasse == NULL )
      dump_te(f, tl[i], tl, nb) ;
  free(tl) ;
  fprintf(f, "\\end{verbatim}\n") ;
  fclose(f) ;
}
/*
 *
 */
static void u_page_tp(FILE *c, char *url, void *a, char **args, int fildes)
{
  Buffer nom ;
  int nb, i, j, k ;
  const Codec_informations **tp ;

  sprintf(nom, "Les types physiques (codec)") ;
  u_debut(c, nom) ;

  tp = NULL ;
  nb = 0 ;
  POUR_LES_CLASSES
    (cl,
     POUR_TOUS_LES___CHAMPS(cl, ch,
			    ajoute_a_la_table((const void***)&tp, &nb, ch->tp) ;
			    ) ;
     ) ;

  fprintf(c, "<TABLE BORDER><TR><TH>Nom du codec<TH>Nom du type C<TH>#Oct<TH>Utilisé par\n") ;
  for(i=0;i<nb;i++)
    {
      fprintf(c, "<TR><TD>%s<TD>%s<TD>%d<TD>",
	      tp[i]->nom,
	      tp[i]->nom_structure,
	      tp[i]->taille
	  ) ;
      j = 0 ;
      POUR_LES_CLASSES
	(cl,
	 k = 0 ;
	 POUR_TOUS_LES___CHAMPS(cl, ch,
				if ( k == 1 ) break ;
				if ( ch->tp == tp[i] )
				{
				  k = 1 ;
				  j++ ;
				  fprintf(c, "%s ", u_classe(cl)) ;
				}
				) ;
	 if ( j == 8 )
	 {
	   fprintf(c,"...\n") ;
	   break ;
	 }
	 ) ;
    }
  free(tp) ;
  fprintf(c, "</TABLE>") ;

  u_fin(c) ;
}

static void u_page_action(FILE *c, char *url, void *a, char **args, int fildes)
{
  u_debut(c, "Liste des actions à faire") ;
  fprintf(c, "<PRE>") ;
  u_dump_toutes_les_actions(c) ;
  fprintf(c, "</PRE>") ;
  u_fin(c) ;
}

/*
 *
 */
struct cherche
{
  Buffer nom ;
  int numero ;
  int numero_courant ;
  void *resultat ;
} ;

static Parcour chercher(void *objet, void *d)
{
  struct cherche *cherche = d ;

  if ( strcmp( OBJET(objet)->classe->nom, cherche->nom) == 0 )
    {
      
      if ( cherche->numero == cherche->numero_courant )
	{
	  cherche->resultat = objet ;
	  return(Arrete) ;
	}
      cherche->numero_courant++ ;
    }
  return(Continue) ;
}

static int u_fait_action(FILE *c, void *a, struct cherche *cherche,
			 const char *nom_champ, const char *action,
			 const char *valeur)
{
  Interface_widget iw ;

  cherche->resultat = NULL ;
  cherche->numero_courant = 0 ;
  
  o_parcour(a, chercher, NULL, cherche) ;
  
  POUR(void*, f, CONTENU(&((Modele*)FILS(a,0))->oper),
       if ( cherche->resultat )
	 break ;
              o_parcour(f, chercher, NULL, cherche) ;
       ) ;
 

  fprintf(c,
	  "<PRE>\n"
	  "Nom de la classe    = %s\n"
	  "Numéro d'occurrence = %d\n"
	  "Nom du champ        = %s\n"
	  "Action              = %s\n"
	  "Valeur              = %s\n"
	  "Pointeur objet      = %p\n"
	  ,
	  cherche->nom, cherche->numero, nom_champ, action, valeur,
	  cherche->resultat) ;

  if ( cherche->resultat == NULL )
    return(1) ;

  iw.ch = c_trouve(cherche->resultat, nom_champ) ;
  fprintf(c, "Pointeur champ      = %p\n", iw.ch) ;
  if ( iw.ch == NULL )
    return(1) ;

  fprintf(c, "</PRE>\n") ;

  iw.objet = cherche->resultat ;
  iw.w     = NULL             ;
  iw.texte = valeur           ;
  
  if ( strcmp(action, "te_entree") == 0 )
    {
      (*iw.ch->te->entree)(&iw) ;
    }
  else
  if ( strcmp(action, "te_clavier") == 0 )
    {
      (*iw.ch->te->clavier)(&iw) ;
    }
  else
  if ( strcmp(action, "c_sauve") == 0 )
    {
      c_sauve_ici(iw.objet, iw.ch, iw.texte) ;
    }
  else
    {
      fprintf(c, "Action inconnue\n") ;      
    }
  NOTE_UN_CHAMP_CHANGE(iw.objet);
  evaluation(iw.objet) ;

  return(0) ;
}

static void u_page_appel(FILE *c, char *url, void *a, char **args, int fildes)
{
  Buffer action, valeur, nom_champ ;
  struct cherche cherche ;

  u_debut(c, "Votre requête d'appel de fonction") ;
  MET_A_0(&valeur) ;
  sscanf(url+1, "%s%d%s%s%*c%9999c",
	 cherche.nom, &cherche.numero, nom_champ, action, valeur) ;

  if ( valeur[strlen(valeur)-1] == '\n' )
    valeur[strlen(valeur)-1] = '\0' ;

  if ( cherche.numero >= 0 )
    u_fait_action(c, a, &cherche, nom_champ, action, valeur) ;
  else
    for(cherche.numero = 0 ;
	u_fait_action(c, a, &cherche, nom_champ, action, valeur) == 0 ;
	cherche.numero++)
      /* vide */ ;

  evaluation(a) ;

  if ( 1||OBJET(a)->classe->sc ) /* Si c'est X11_interface, mettre tout à jour */
    while( u_plus_petit_interval_temps() < PRIORITE_ARRIERE_PLAN )
      {
	u_execute_action() ;
      }
  u_fin(c) ;
}

/*
 *
 */

static Parcour dumper(void *objet, void *d)
{
  FILE *c = d ;
  const char *pc ;
  Liste *lo ;

  POUR_TOUS_LES_CHAMPS(objet,ch,
		       {
			 pc = c_recupere_ici_sans_copie(objet, ch) ;
			 if ( ch->tp == &infos_codec_pointeur_extrait ||
			      ch->classe_parametre )
			   {
			     fprintf(c, "%s=%d\n", ch->nom_champ, !!atol(pc));
			     continue ;
			   }
			 if ( strcmp(ch->nom_champ,"nom") == 0 )
			   continue ;
			 if ( ch->tp->generique == codec_void )
			   continue ;

			 if ( strcmp(OBJET(objet)->classe->nom,
				     "modele") == 0
			      && ( strcmp(ch->nom_champ,"numero_sauvegarde")==0
			      || strcmp(ch->nom_champ,"graphe")==0 )
			      )
			   continue ;
			 if ( strcmp(OBJET(objet)->classe->nom,
				     "interface") == 0
			      && ( strcmp(ch->nom_champ,"port")==0 ||
				   strcmp(ch->nom_champ,"argv")==0 )
			      )
			   continue ;
			 if ( ch->tl == &infos_L_liste_objet )
			   {
			     fprintf(c,"%s=", ch->nom_champ) ;
			     lo = T(Liste*, objet, ch) ;
			     if ( lo )
			       POUR(long, li, lo,
				    fprintf(c, " %d", li ? 1 : 0) ;
				    ) ;
			     fprintf(c,"\n") ;
			     continue ;
			   }
			 fprintf(c, "%s=%s\n", ch->nom_champ, pc) ;
		       }) ;
  fprintf(c, "invalide=%llx\n\n", OBJET(objet)->invalide) ;
  return(Continue) ;
}

static void u_page_dump(FILE *c, char *url, void *a, char **args, int fildes)
{
  o_parcour(u_modele(a), dumper, NULL, c) ;
  POUR(void*, o, CONTENU( &((Modele*)u_modele(a))->oper ),
       o_parcour_r(o, dumper, NULL, c) ; ) ;
  POUR(void*, o, CONTENU( &((Modele*)u_modele(a))->oper ),
       o_annule_flag(o) ; ) ;
}

/*
 *
 */

static void u_page_http(FILE *c, char *url, void *a, char **args, int fildes)
{
  u_debut(c, "Page inaccessible pour des raisons de sécurité") ;
  fprintf(c,
	  "Relancez NanoÉdit avec l'option '-http_ecrit\n"
	  "à vos risques et périls.\n"
	  "En effet, avec cette option, il est facile\n"
	  "pour un intrus de pénétrer dans votre compte.\n"
	  ) ;
  u_fin(c);
}

static void u_page_fichier(FILE *c, char *url, void *a, char **args, int fildes)
{
  FILE *f ;
  Buffer tmp ;
  char *pc ;
  const char *nom_fichier ;

  for(pc=url+1; *pc; pc++) if ( *pc == ':' ) *pc = '/' ;
  nom_fichier = u_trouve_fichier(directories_objets, url+1, "/", "") ;
  f = fopen(nom_fichier, "r") ;
  if ( f == NULL )
    {
      sprintf(tmp, "%s%s", DIRECTORY_BASE, url) ;
      f = fopen(tmp, "r") ;
    }
  if ( f == NULL )
    {
      u_mauvaise_url(c,url,a,args,fildes) ;
      return ;
    }
  while( fgets(tmp, sizeof(tmp), f) )
    {
      fputs(tmp, c) ;
    }
  fclose(f) ;
}

/*
 *
 */

struct test_memoire
{
  const Classe *classe_testee ;
  int nb ;
  int memoire[sizeof(Bits)*8] ;
} ;

static Parcour test_memoire_classe(void *o, void *donnee)
{
  Objet *o_ = OBJET(o) ;
  struct test_memoire *tm = donnee ;
  const Champ *ch ;

  if ( tm->classe_testee == o_->classe )
    {
      tm->nb++ ;
      POUR_TOUS_LES_CHAMPS_(o_, ch,
      {
	tm->memoire[i_ch] += strlen(c_recupere_ici_sans_copie(o,ch));
      }
			    ) ;
    }

  
  if ( c_trouve_type((const void**)&o, &ch, &infos_L_operateur) )
    {
      /*
       * Ne pas sauvegarder et récupérer les variables
       * sinon on compte plusieurs fois le même espace mémoire
       * et de plus on en oublie (valeur par défaut des variables
       */
      o_parcour_liste(CONTENU(&T(Operateur, o, ch)), test_memoire_classe, NULL, tm) ;
    }

  return(Continue) ;
}

static void u_page_stat_type(FILE *c,char *url, void *a, char **args, int fildes)
{
  u_debut(c, "Statistique cache du typage dynamique") ;
  c_dump_trouve_type_table(c) ;
  u_fin(c) ;
}

static void u_page_memoire(FILE *c,char *url, void *a, char **args, int fildes)
{
  struct test_memoire tm ;
  int somme, somme_totale, somme_totale_caractere ;

  u_debut(c, "Utilisation de la mémoire") ;

  fprintf(c,
	  "On compte aussi la mémoire des modèles inclus\n"
	  "via des ``opérateurs''."
	  "<P>\n"
	  ) ;
  fprintf(c, "<TABLE BORDER><TH>Classe<TH># instances<TH>Mém. structure<TH>Mém. Caractère\n") ;
  somme_totale = 0 ;
  somme_totale_caractere = 0 ;
  POUR_LES_CLASSES
    (cl,
     {
       MET_A_0(&tm) ;
       tm.classe_testee = cl ;
       o_parcour(u_racine(a), test_memoire_classe, NULL, &tm) ;
       if ( tm.nb )
	 {
	   somme_totale += tm.nb*cl->taille_structure ;
	   fprintf(c, "<TR><TH>%s", u_classe(cl) ) ;
	   fprintf(c, "<TD>%d", tm.nb ) ;
	   fprintf(c, "<TD>%d", tm.nb*cl->taille_structure ) ;
	   somme = 0 ;
	   POUR_TOUS_LES_CHAMPS(a, ch,
				(void)ch ;
				somme += tm.memoire[i_ch] ;
				somme_totale_caractere += tm.memoire[i_ch] ;
				) ;
	   fprintf(c, "<TD>%d\n", somme) ;
	 }
     }
     ) ;
  fprintf(c, "</TABLE>") ;
  fprintf(c, "<P>Taille des structures : %d\n", somme_totale) ;
  fprintf(c, "<P>Taille de la conversion en caractère : %d\n"
	  , somme_totale_caractere) ;

  u_fin(c) ;
}

#define eu(T,F,TYPE,FORKE) enregistre_url(url_alea(o->aleatoire,T), \
					  F,TYPE,o,FORKE,1)
#define EU(T,F,FORKE) eu(T, o->http_ecrit?F:u_page_http, "text/html",FORKE)

static void u_http_enregistre(Interface *o)
{
  if ( o->http_ecrit )
    {
      EU("/creer"        ,u_creer            ,0) ;
      EU("/ecrit"        ,u_ecrit            ,0) ;
      EU("/detruire"     ,u_detruire_objet   ,0) ;
      EU("/active"       ,u_active           ,0) ;
      EU("/edite"        ,u_edite            ,0) ;
      EU("/reinitialise" ,u_page_reinitialise,0) ;
      EU("/exit"         ,u_page_exit        ,0) ;
      EU("/appel"        ,u_page_appel       ,0) ;
    }

  /* Ne fait pas de fork pour synchroniser le script shell "tests" */
  eu("/dump"             ,u_page_dump        ,"text/plain",0);
  eu("/Welcome"          ,u_page_accueil     ,"text/html" ,1);
  eu("/classe"           ,u_page_classe      ,"text/html" ,1);
  eu("/definition"       ,u_page_definition  ,"text/html" ,1);
  eu("/champ"            ,u_page_champ       ,"text/html" ,1);
  eu("/type_logique"     ,u_page_type_logique,"text/html" ,1);
  eu("/objet"            ,u_page_objet       ,"text/html" ,1);
  eu("/hp"               ,u_hier_classe      ,"text/html" ,1);
  eu("/tl"               ,u_hier_tl          ,"text/html" ,1);
  eu("/tp"               ,u_page_tp          ,"text/html" ,1);
  eu("/stattype"         ,u_page_stat_type   ,"text/html" ,1);
  eu("/memoire"          ,u_page_memoire     ,"text/html" ,1);
  eu("/action"           ,u_page_action      ,"text/html" ,1);
  eu("/texte"            ,u_page_fichier     ,"text/plain",1);
  eu("/fichier"          ,u_page_fichier     ,"text/html" ,1);
  eu(""                  ,u_mauvaise_url     ,"text/html" ,1);
}

static void previent_netscape(Interface *o)
{
  goto_racine(o->port, o->welcome) ;
}



static void changement_interface(Interface *o)
{
  int f ;

  if ( o->init == 0 )
    {
      if ( o->dump )
	{
	  dump() ;
	  return ;
	}
      o->init = 1 ;
      o->port = getuid() ;
      http_init(&o->port, &o->socket) ;
      o->aleatoire = aleatoire() ;
      u_http_enregistre(o) ;

      if ( o->demo )
	o->welcome = url_alea(o->aleatoire, "/fichier:DEMO:Welcome.html") ;
      else
	o->welcome = url_alea(o->aleatoire, "/Welcome") ;

      unlink(".secret") ;
      f = creat(".secret", 0600) ;
      write(f, o->welcome+1, 8) ;
      close(f) ;
      if ( o->netscape )
	goto_racine(o->port, o->welcome) ;
      
      evaluation(o) ;	/* Pour faire l'évaluation du modèle */
      if ( OBJET(o)->classe->sc == NULL )	/* Pas une sous-classe de interface */
	while(!o->stop)
	  {
	    while ( u_execute_action() ) /* vide */ ;
	    http(o->socket) ;
	  }
      return ;
    }
}

static void destruction_interface(Interface *o)
{
  http_stop(o->socket) ;
  while ( u_execute_action() ) /* vide */ ;
}

CLASSE(interface, Interface,
       MENU("NON")
       CHAMP(argc                , L_entier      )
       CHAMP(argv                , L_long        )
       CHAMP(port                , L_entier      )
       CHAMP(socket              , L_entier      )
       CHAMP(netscape            , L_entier      )
       CHAMP(stop                , L_entier Edite)
       CHAMP(dump                , L_entier Edite)
       CHAMP(http_ecrit          , L_entier Edite)
       CHAMP(demo                , L_entier Edite)
       CHAMP(ouvre_fenetre_modele, L_entier Edite)
       CHAMP_VIRTUEL(NOM("previent_netscape") L_fonction(previent_netscape))

       CHANGEMENT(changement_interface)
       DESTRUCTION(destruction_interface)
       )

