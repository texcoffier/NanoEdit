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
#include <math.h>
#include <sys/time.h>
#include "o_objet.h"
#include "s_string.h"
#include "u_fichier.h"
#include "t_geometrie.h"

typedef struct
{
  char *nom_fichier ;
  Booleen joue_a_la_creation ;
  Booleen joue_si_modifie ;
} Audio ;

static void joue(Audio *o)
{
  static Chaine s ;
  const char *f ;

  f = u_trouve_fichier(directories_objets, o->nom_fichier, "/MODELES/", "") ;
  if ( f )
    {
      s_efface(&s) ;
      s_printf(&s, "auplay %s || aplay %s || play %s &", f, f, f) ;
      system(s_char(&s)) ;
    }
}

static Booleen joue_maintenant(const Interface_widget *iw)
{
  joue((Audio*)iw->objet) ;
  return(Faux) ;
}

static void changement(Audio *o)
{
  if ( (CREATION(o) && o->joue_a_la_creation)
       || ( UN_CHAMP_CHANGE(o) && !CREATION(o) && o->joue_si_modifie )
       )
    joue(o) ;
}



CLASSE(audio, Audio,
       MENU("Divers/Sortie Audio")
       EVENEMENT("SA")
       BULLE_D_AIDE("Fait péter le son un max.")
       CHAMP(nom_fichier, L_chaine Edite Sauve Extrait InitChaine
	     DEFAUT("/usr/share/sounds/login.wav"))
       CHAMP(joue_a_la_creation, L_booleen Edite Sauve Extrait)
       CHAMP(joue_si_modifie, L_booleen Edite Sauve Extrait)
       CHAMP_VIRTUEL(LABEL("Joue maintenant") L_bouton(joue_maintenant))
       CHANGEMENT(changement)
       )

/*
 * Générateur de son.
 */

#define DUREE_DU_BLOC 10 /* En milli secondes */

typedef struct
{
  char *nom ;
  int nb ;
  char *donnees ;
} Son ;

void son_charge(Son *s)
{
  const char *nom ;
  int i ;

  if ( s->nb )
    {
      free(s->donnees) ;
      s->nb = 0 ;
    }

  nom = u_trouve_fichier(directories_objets, s->nom, "/MODELES/", "") ;
  if ( nom == NULL )
    return ;

  lit_fichier_complet(nom, &s->donnees, &s->nb) ;

  if ( s->nb > 128 )
    {
      memmove(s->donnees, s->donnees+128, s->nb-128) ;
      s->nb -= 128 ;
    }

  if ( 0 )
    for(i=0 ;i<s->nb; i++)
      s->donnees[i] -= 128 ;

  eprintf("%s %d %p\n", nom, s->nb, s->donnees) ;
}

typedef struct
{
  FILE *sortie_audio ;
  Booleen onoff ;
  int timout ;
  struct timeval depart ;
  int temps_deja_genere ; /* micro sec depuis depart */
  int frequence_echantillonnage ;
  Menu generateur ;

  Quadruplet q ;

  Son x, y, z, h ;
} Audio_generator ;

static Booleen genere_ag(void *oo)
{
  Audio_generator *o = oo ;
  int i, di ;
  struct timeval maintenant ;
  int present, futur ;

  gettimeofday(&maintenant, NULL) ;
  present = (maintenant.tv_sec - o->depart.tv_sec)*1000000
    + (maintenant.tv_usec - o->depart.tv_usec) ;

  if ( !o->onoff )
    {
      return(Vrai) ;
    }

  /*
   * Si la fonction a été lancée en retard
   * on laisse un trou dans le son
   */
  if ( present > o->temps_deja_genere )
    {
      if ( TRACE(o) )
	eprintf("retard de %d microsecs\n", o->temps_deja_genere-present) ;
      o->temps_deja_genere = present ;
    }

  /* Le +15000 représente le temps d'erreur de déclenchement.
   */
  futur = present + DUREE_DU_BLOC*1000 + 15000 ;

  di = 1000000 / o->frequence_echantillonnage ;

  if ( 0 )
  eprintf("deja=%g present=%g futur=%g nbe=%d file=%p\n",
	  o->temps_deja_genere/1000000.,
	  present/1000000.,
	  futur/1000000.,
	  (futur-o->temps_deja_genere)/di,
	  o->sortie_audio
	  ) ;

  switch( o->generateur.index)
    {
    case 0:
      for(i = o->temps_deja_genere; i<futur; i += di)
	putc(
	     64*cos(i*o->q.x/1000) +
	     64*cos(i*o->q.y/1000) +
	     64*cos(i*o->q.z/1000) +
	     64*cos(i*o->q.h/1000)
	     ,o->sortie_audio) ;
      break ;
    case 1:
      for(i = o->temps_deja_genere; i<futur; i += di)
	putc(
	     10*o->q.x*cos(i/100) +
	     10*o->q.y*cos(i/300) +
	     10*o->q.z*cos(i/900) +
	     10*o->q.h*cos(i/2700)
	     ,o->sortie_audio) ;
      break ;
    case 2:
      for(i = o->temps_deja_genere; i<futur; i += di)
	putc(
	     (
	     o->q.x*(o->x.nb ? o->x.donnees[((i*8000)/1000000) % o->x.nb] : 0 ) +
	     o->q.y*(o->y.nb ? o->y.donnees[((i*8000)/1000000) % o->y.nb] : 0 ) +
	     o->q.z*(o->z.nb ? o->z.donnees[((i*8000)/1000000) % o->z.nb] : 0 ) +
	     o->q.h*(o->h.nb ? o->h.donnees[((i*8000)/1000000) % o->h.nb] : 0 ))
	    / 10
	     ,o->sortie_audio) ;
      break ;
    }

  o->temps_deja_genere = futur ;

  fflush(o->sortie_audio) ;

  return(Vrai) ;
}

static void destruction_ag(Audio_generator *o)
{
  if ( o->x.nb ) free( o->x.donnees ) ;
  if ( o->y.nb ) free( o->y.donnees ) ;
  if ( o->z.nb ) free( o->z.donnees ) ;
  if ( o->h.nb ) free( o->h.donnees ) ;


  if ( o->sortie_audio )
    {
      pclose(o->sortie_audio) ;
      o->sortie_audio = NULL ;
      widget_enleve_timeout(o->timout) ;
    }
}

static void changement_ag(Audio_generator *o)
{
  char buf[99] ;

  if ( CHAMP_CHANGE(o, frequence_echantillonnage) )
    destruction_ag(o) ;

  if ( CHAMP_CHANGE(o, x.nom) )
    son_charge(&o->x) ;
  if ( CHAMP_CHANGE(o, y.nom) )
    son_charge(&o->y) ;
  if ( CHAMP_CHANGE(o, z.nom) )
    son_charge(&o->z) ;
  if ( CHAMP_CHANGE(o, h.nom) )
    son_charge(&o->h) ;

  if ( o->sortie_audio )
    return ;
  gettimeofday(&o->depart, NULL) ;
  o->temps_deja_genere = 0 ;
  sprintf(buf, "aplay -r %d -f S8 -c 1", o->frequence_echantillonnage) ;
  o->sortie_audio = popen(buf , "w") ;

  o->timout = widget_ajoute_timeout( DUREE_DU_BLOC, genere_ag, o) ;
}

CLASSE(generateur_son, Audio_generator,
       CHAMP(frequence_echantillonnage, L_nombre P_entier Edite Sauve DEFAUT("8000"))
       CHAMP(q, L_point P_quadruplet Extrait Edite Sauve DEFAUT("1 2 -1 -2"))
       CHAMP(generateur , L_option_menu Edite Sauve
             TE_TEXTE("Frequences\n"
		      "Amplitudes\n"
		      "Fichiers\n"
		      )
	     DEFAUT("Amplitudes")
	     )
       CHAMP(x.nom, L_chaine Edite Sauve DEFAUT("x.uLAW"))
       CHAMP(y.nom, L_chaine Edite Sauve DEFAUT("y.uLAW"))
       CHAMP(z.nom, L_chaine Edite Sauve DEFAUT("z.uLAW"))
       CHAMP(h.nom, L_chaine Edite Sauve DEFAUT("h.uLAW"))
       CHAMP(onoff, L_booleen Edite Sauve)
       CHANGEMENT(changement_ag)
       DESTRUCTION(destruction_ag)
       MENU("Divers/Générateur de son")
       EVENEMENT("GS")
       )
