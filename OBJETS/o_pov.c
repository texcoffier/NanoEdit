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
#ifndef _INCLUDE__STDC__
#define _INCLUDE__STDC__ 1
#endif

#include <math.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include "u_xt_interface.h"
#include "t_geometrie.h"
#include "t_image.h"
#include "u_vue.h"
#include "s_string.h"


/*
pas_de_nom.pov:45: warning: Refraction value unnecessary to turn on refraction.
To attenuate, the fade_power and fade_distance keywords 
should be specified in 'interior{...}' statement.
Use of this syntax may not be backwards compatable with earlier versions
of POV-Ray. The #version directive or +MV switch will not help.
*/

typedef struct
{
  Image imag ;

  int            pid ;		/* Pour pouvoir tuer POV */
  int            pipe[2] ;	/* To read status from POV */

  int lar, hau ;
  int qualite ;
  int qualite_vue ;
  t_observateur observateur ;
  Booleen antialiasing ;
  Booleen auto_changement ;
  Booleen auto_si_observateur_change ;
  Booleen auto_si_modele_change ;
  Booleen animation ;
  int lar_max, hau_max ;
  int numero_image ;
} Pov ;

static Parcour pov_a_fini(void *o, int fildes) ;

static char ** argument_pov(Pov *o)
{
  Buffer buf ;
  int i ;
  static char *arg[20] ;

  i = 0 ;
  arg[i++] = strdup("povray") ;
  sprintf(buf, "+I%s.pov", o->imag.nom_image) ; arg[i++] = strdup(buf) ;
  sprintf(buf, "+O%s"    , o->imag.nom_image) ; arg[i++] = strdup(buf) ;
  sprintf(buf, "+W%d"    , o->lar           ) ; arg[i++] = strdup(buf) ;
  sprintf(buf, "+H%d"    , o->hau           ) ; arg[i++] = strdup(buf) ;
  sprintf(buf, "+Q%d"    , o->qualite       ) ; arg[i++] = strdup(buf) ;
  arg[i++] = strdup("+FP") ;
  arg[i++] = strdup(o->antialiasing ? "+A" : "-A") ;
  arg[i++] = NULL ;
  return(arg) ;
}

static void libere_argument(char **arg)
{
  int i ;
  for(i=0; arg[i]; i++)
    free(arg[i]) ;
}

static char* ligne_de_commande(Pov *o)
{
  static Chaine s ;
  char **arg ;
  int i ;

  arg = argument_pov(o) ;
  s_efface(&s) ;
  for(i=0; arg[i]; i++)
    s_printf(&s, " %s", arg[i]) ;
  libere_argument(arg) ;
  return( s_char(&s) ) ;
}


static void lance_pov(Pov *o)
{
  pipe(o->pipe) ;
  o->pid = u_fork(o->pipe[1]) ;
  if ( o->pid < 0 )
    {
	perror("fork() pour lancer POV RAY") ;
	close(o->pipe[0]) ;
	close(o->pipe[1]) ;
	o->pid = 0 ;
	return ;
    }

  if ( o->pid )
    {
      /* Pere */
      xt_ajoute_fichier(o, pov_a_fini, o->pipe[0]) ;
    }
  else
    {
      /* fils */
      dup2(o->pipe[1], 2) ;
      execvp("povray", argument_pov(o)) ;
      perror(__FILE__) ;
      eprintf("\007BUG BUG BUG BUG BUG BUG BUG BUG BUG\007") ; 
      exit(0) ;
    }
}


static void destruction_pov(Pov *o)
{
  int status ;

  if ( o->pid )
    {
      if ( kill(o->pid,SIGTERM) == 0 )
	{
	  /* waitpid(o->pid,&status,WNOHANG) ; */
	  waitpid(-1,&status,WNOHANG) ;
	}
      xt_enleve_fichier(o, pov_a_fini, o->pipe[0]) ;
      close(o->pipe[0]) ;
      close(o->pipe[1]) ;
      o->pid = 0 ;
    }
}

static Booleen arreter_calcul(const Interface_widget *iw)
{
  destruction_pov((Pov*)iw->objet) ;
  return(0) ;
}


static Parcour pov_a_fini(void *oo, int fildes)
{
  Buffer tmp ;
  int i,j ;
  Pov *o = oo ;

  i = read(o->pipe[0],tmp,sizeof(tmp)-1) ;
  tmp[sizeof(tmp)-1] = '\0' ;
  if ( TRACE2(o) )
    write(1,tmp,i) ;
  for(j=0;j<i;j++)
    if ( tmp[j]=='\007' )
      break ;
  if ( j==i )
    {
      if ( strstr(tmp, "Total Time") == NULL
	   && strstr(tmp, "Peak memory used") == NULL )
	return(Continue) ;
    }
    


  if ( TRACE(o) ) eprintf( "POV a fini\n") ;
  destruction_pov(o) ;
  /*
   * Affiche l'image : pas très propre....
   */
  recharge_image(&o->imag) ;
  if ( o->lar < o->lar_max )
    {
      o->lar *= 2 ;
      o->hau *= 2 ;
      lance_pov(o) ;
    }

  o->auto_changement = Vrai ;
  evaluation(o) ;
  o->auto_changement = Faux ;
  return(Continue) ;		/* Car déjà détruit par "destruction_pov" */
}

static void genere_fichier_pov(Pov *o)
{
  FILE *f ;
  NomFichier fn,file ;

  if ( o->animation )
    {
      mkdir(o->imag.nom_image, 0755) ;
      sprintf(fn, "%s/%05d.pov", o->imag.nom_image, o->numero_image) ;
    }
  else
    sprintf(fn, "%s.pov", o->imag.nom_image) ;
  f = fopen(fn,"w") ;
  if ( TRACE(o) )
    eprintf("Ouverture du fichier POV %s : FILE=%ld\n", fn, (long)f) ;
  if ( f==NULL )
    {
      NOTE_CHAMP_INVALIDE(o, imag) ;
      return ;
    }

  o->lar = o->lar_max ;
  o->hau = o->hau_max ;

  fprintf(f,
	  "// Ligne de commande pour faire l'image\n"
	  "// %s\n"
	  "// Modèle généré par NanoÉdit à partir de :\n"
	  "// %s\n"
	  , ligne_de_commande(o)
	  , c_recupere_sans_copie(u_modele(o), "nom_modele")
	  ) ;

  fprintf(f,
	  "#include \"chars.inc\"\n"
	  "camera {\n"
	  "location <%g,%g,%g>\n"
	  "sky <%g,%g,%g>\n"
	  "right <%g,0,0>\n"
	  "up y\n"
	  "look_at <%g,%g,%g>\n"
	  "angle %g\n"
	  "}\n"
	  "union {\n"
	  "union {\n"
	  ,o->observateur.point_de_vue.x
	  ,o->observateur.point_de_vue.y
	  ,-o->observateur.point_de_vue.z
	  ,o->observateur.haut.x
	  ,o->observateur.haut.y
	  ,-o->observateur.haut.z
	  ,o->lar_max/(double)o->hau_max
	  ,o->observateur.point_vise.x
	  ,o->observateur.point_vise.y
	  ,-o->observateur.point_vise.z
	  ,(atan(o->lar_max/(double)o->hau_max*o->observateur.focale))*180./M_PI
	  ) ;

  sprintf(file,"%ld %d",(long)f, o->qualite_vue) ;

 POUR_LES_ELEMENTS(fils_a_afficher(o), f,
		   c_sauve_type(f, &infos_L_affiche_pov, file) ; ) ;

  fprintf(f,
	  "}\n"
	  "pigment {red 1 green 1 blue 1}\n"
	  "matrix <1, 0, 0,  0, 1, 0,  0, 0, -1,  0, 0, 0>\n"
	  "}") ;

  fclose(f) ;

  if ( TRACE(o) )
    eprintf("Fermeture du fichier POV\n") ;
}

static void changement_pov(Pov *o)
{
  if ( CREATION(o) || o->auto_changement )
    {
      if ( TRACE(o) )
	eprintf("Ne fait rien : création ou auto_changement\n") ;
      return ;
    }

  if ( CHAMP_CHANGE(o, observateur.ratio) || CHAMP_CHANGE(o, observateur) )
    {
      o->hau_max = o->lar_max/o->observateur.ratio ;
    }
  else
    {
      o->observateur.ratio = o->lar_max/(float)o->hau_max ;
    }

  if ( o->animation )
    {
      if ( DESCENDANT_CHANGE(o) )
	{
	  genere_fichier_pov(o) ;
	  o->numero_image++ ;
	}
      return ;
    }

  if ( !UN_VRAI_CHAMP_CHANGE(o) )
    {
      if ( TRACE(o) )
	eprintf("Aucun champ de POV ne change\n") ;
      if ( !DESCENDANT_CHANGE(o) )
	return ;
      if ( !o->auto_si_modele_change )
	return ;
    }
  else
    {
      if ( CHAMP_CHANGE(o, observateur) && !o->auto_si_observateur_change )
	return ;
    }

  destruction_pov(o) ;

  genere_fichier_pov(o) ;	/* Pour indiquer la taille max dans "?.pov" */

  o->lar = o->lar_max ;
  o->hau = o->hau_max ;
  while ( o->lar > 32 )
    {
      o->lar /= 2 ;
      o->hau /= 2 ;
    }

  if ( OBJET(o)->edite )
    lance_pov(o) ;
}


CLASSE(pov, Pov,
       Affiche L_entier
       BULLE_D_AIDE("Il faut que vous modifiez l'un des champs\n"
		    "de l'objet afin de lancer le calcul de\n"
		    "l'image de synthèse en lancé de rayons.\n"
		    "\n"
		    "La scène contiendra les fils de l'objet,\n"
		    "s'il y en a aucun, tous les fils du modèle.\n"
		    )
       CHAMP(observateur , L_observateur NonAffiche Extrait Sauve)
       DEBUT_BOITE(HORIZONTALE)
       CHAMP(imag.image.l3_float.lar, LABEL("Largeur") TE_LARGEUR(50) L_entier NonInitialiser)
       CHAMP(imag.image.l3_float.hau, LABEL("Hauteur") TE_LARGEUR(50) L_entier NonInitialiser)
       CHAMP(lar_max, L_entier Edite Sauve TE_LARGEUR(50) DEFAUT("512"))
       CHAMP(hau_max, L_entier Edite Sauve TE_LARGEUR(50) DEFAUT("512"))
       CHAMP(observateur.ratio, L_nombre LABEL("ratio") Edite P_flottant DEFAUT("1"))
       FIN_BOITE
       CHAMP(imag, LABEL("Nom Image")
	     BULLE_D_AIDE("L'image doit être au format PPM,\n"
			  "taper son nom sans l'extension.\n")
	     L_fichier_image_l3_unsigned_char
	     DEFAUT("pas_de_nom")
	     Sauve Edite
	     )
       DEBUT_BOITE(HORIZONTALE)
       CHAMP(observateur.point_de_vue, NOM("oeil") NonLabel L_point3 Edite Init3d DEFAUT("3 3 3"))
       CHAMP(observateur.point_vise  , NOM("pt_vise") NonLabel  Edite L_point3 Init3d DEFAUT("0 0 0"))
       CHAMP(observateur.haut        , L_point3  NonLabel Init3d Edite DEFAUT("0 0 1"))
       FIN_BOITE
       DEBUT_BOITE(HORIZONTALE)
       CHAMP(observateur.focale      , L_flottant LABEL("Focale") TE_LARGEUR(50) Edite Init3d DEFAUT("1"))
       CHAMP(pid, L_entier NonEdite Affiche TE_LARGEUR(50))
       Edite Sauve
       CHAMP(qualite,  L_entier    TE_LARGEUR(50) LABEL("Qualité POV")        DEFAUT("4")
	     BULLE_D_AIDE("Entre 0 et 9, plus c'est grand mieux c'est.\n")
	     )
       CHAMP(qualite_vue,  L_entier    TE_LARGEUR(50) LABEL("Qualité Vue")       DEFAUT("5")
	     BULLE_D_AIDE("Même valeur que dans la fenêtre vue.\n"
			  "Cette qualité n'agit que sur les solides filaires\n"
			  )
	     )
       FIN_BOITE
       DEBUT_BOITE(HORIZONTALE)
       CHAMP(antialiasing, L_booleen)
       CHAMP(auto_si_modele_change, L_booleen)
       CHAMP(auto_si_observateur_change, L_booleen)
       FIN_BOITE
       DEBUT_BOITE(HORIZONTALE)
       CHAMP_VIRTUEL(LABEL("Arrêter calcul") L_bouton(arreter_calcul))
       CHAMP(animation, L_booleen)
       CHAMP(numero_image, NonSauve L_nombre P_entier DEFAUT("0"))
       FIN_BOITE
       CHAMP(imag.gl, NOM("gl") L_void Edite E_image
	     TE_HAUTEUR(512) TE_LARGEUR(512))
       CHAMP_VIRTUEL( L_prevenir_si_modele_change(me_prevenir))

       CHANGEMENT(changement_pov)
       DESTRUCTION(destruction_pov)
       PAQUET(NB_ACTIFS)
       EVENEMENT("0p0r") ;
       MENU("Divers/Pov Ray")
       )
