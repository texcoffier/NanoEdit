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
#include <math.h>               /* floor function */
#include "t_gl.h"
#include <GL/glu.h>             /* Utilitaires GL */
#include "t_geometrie.h"
#include "u_graphique.h"           /* Pour t_repere */
#include "o_objet.h"            /* Car il faut pouvoir definir la classe */
#include "u_xt_interface.h"     /* Pour widget vers objet */
#include "s_string.h"
#include "u_vue.h"
#include "u_repartiteur.h"
#include "u_etat_courant.h"
#include "u_fichier.h"
#include <sys/times.h>
#include <sys/stat.h>
#include <time.h>
#include <ctype.h>
#include "u_modele.h"

#define BUFSIZE         1000



typedef struct vue
{
  Menu b_persp ;
  Booleen b_entier ;
  Booleen b_local ;
  Booleen b_repere ;
  Booleen b_grille ;
  Booleen b_stereo ;
  t_observateur observateur ;
  Triplet regard, droite ;	/* Ceux-ci sont calcule */
  char *titre ;
  Gl gl ;
  Flottant dts ;		/* Cote du carre (en pixel) de designation */
  Flottant ratio ;
  Flottant pas_vue ;
  Flottant angle_vue ;
  Flottant zmin, zmax ;
  Menu menu_observateur ;
  Menu menu_qualite ;
  Menu menu_contextuel ;
  Interface_widget menu_contextuel_iw ;
  char *geometrie ;
  int x_init, y_init ;		/* Position curseur debut de déplacement */
  int x__init, y__init ;	/* Position curseur debut de déplacement */
  int x_init_root, y_init_root ;
  int mosaicf, mosaicx, mosaicy ;
  Triplet couleur_fond ;
  int temps_en_clockticks ;
  Booleen bouton_2_et_3_deviennent_1 ;
  Booleen deplacement_observateur_a_la_souris ;
  Booleen double_buffer ;
  Booleen vue_boite ;
  int zoomin, zoomout ; // Pour dump hemi

  int nb ;			/* Nombre d'objet cliqués */
  /*
   * Les objets cliqués.
   * BUFSIZE/4 car BUFSIZE est la taille du buffer de sélection
   * hors celui-ci contient au moins 4 entiers par objet : 
   *    Taille_Nom ZMin ZMax Nom
   */
  /* Le champs position indique celle Dans l'arbre du modèle
   * en effet quand on déplace un tag d'un objet translaté
   * il faut retrouver quel translation lui a été faite
   * dans le cas ou il y en a plusieurs
   */
  struct selection
  {
    void *objet ;
    int sous_objet ;
    unsigned int position ;
    struct globale_etat_courant etat_courant ;
    struct vue *vue ;
    int zmin, zmax ;
  } designes[BUFSIZE/4] ;

  int width, height ; // Last size of the GL widget

} Vue ;



typedef struct
{
  Vue vue ;
  int xmin, xmax, ymin, ymax ;
  Booleen efface ;
  Booleen affiche_contenu ;
  Booleen est_designable ;
} Vue_incluse ;


/*
 *
 */


static void u_vue_matrice_suite(const Vue *vue)
{
  float regard, angle ;

  if ( vue->ratio < 0.01 )
    ((Vue*)vue)->ratio = 0.01 ;

  if ( vue->b_persp.index )
    {
      angle = atan(vue->observateur.focale) ;
      gluPerspective(angle/M_PI*360,            /* Angle perspective */
                     vue->ratio,        /* Largeur/Hauteur */
                     vue->zmin,               /* Z min */
                     vue->zmax       /* Z max */
                     ) ;
    }
  else
    {
      /*
      if ( fabs(vue->regard.x) > fabs(vue->regard.y)
           && fabs(vue->regard.x) > fabs(vue->regard.z) )
        regard = vue->observateur.point_de_vue.x ;
      else
        {
          if ( fabs(vue->regard.y) > fabs(vue->regard.x)
               && fabs(vue->regard.y) > fabs(vue->regard.z) )
            regard = vue->observateur.point_de_vue.y ;
          else
            regard = vue->observateur.point_de_vue.z ;
        }
      regard *= vue->observateur.focale ;
      */
      regard = vue->observateur.focale ;
      glOrtho(-regard*vue->ratio, regard*vue->ratio,
              -regard, regard,
              1, 1000) ;
    }

  gluLookAt( vue->observateur.point_de_vue.x, vue->observateur.point_de_vue.y, vue->observateur.point_de_vue.z,
             vue->observateur.point_vise.x  , vue->observateur.point_vise.y  , vue->observateur.point_vise.z  ,
             vue->observateur.haut.x        , vue->observateur.haut.y        , vue->observateur.haut.z   ) ;

  glMatrixMode(GL_MODELVIEW) ;
}

void u_vue_matrice_init(const Vue *vue)
{
  glMatrixMode(GL_MODELVIEW) ;
  glLoadIdentity() ;
  glMatrixMode(GL_PROJECTION) ;
  glLoadIdentity() ;
  if ( vue->mosaicf )
    {
      glTranslatef(-(-vue->mosaicf+1+2*vue->mosaicx),
		   -(-vue->mosaicf+1+2*vue->mosaicy),
		   0) ;
      glScalef(vue->mosaicf, vue->mosaicf, 1) ;
    }
}

static void u_vue_matrice(const Vue *vue)
{
  u_vue_matrice_init(vue) ;
  u_vue_matrice_suite(vue) ;
}


void regarder(const Interface_widget *iw,Vue *vue, struct selection *selection,
	      int surface)
{
  GLdouble mv[16], pr[16] ;
  GLint vp[4] = { 0, 0, vue->gl.lar, vue->gl.hau }, dt ;
  GLdouble xx, yy, zz, z ;

  // Pourquoi cela ne retourne pas 32 mais 24 ???
  // glGetIntegerv(GL_DEPTH_BITS, &dt) ;
  dt = 32;

  if ( surface )
    z = (unsigned int)selection->zmin/pow(2,dt) ;
  else
    z = ((unsigned int)selection->zmin/pow(2,dt)
	 +  (unsigned int)selection->zmax/pow(2,dt)) / 2 ;
    

  u_vue_matrice(vue) ;
  glGetDoublev(GL_MODELVIEW_MATRIX,mv) ;
  glGetDoublev(GL_PROJECTION_MATRIX,pr) ;
  if ( gluUnProject( iw->x, vue->gl.hau - iw->y, z,
		     mv, pr, vp, &xx, &yy, &zz)
       != GL_TRUE )
    eprintf("gluUnProject error\n") ;

  vue->observateur.point_vise.x = xx ;
  vue->observateur.point_vise.y = yy ;
  vue->observateur.point_vise.z = zz ;
  vue->regard = u_soustrait_triplet(&vue->observateur.point_vise,
				    &vue->observateur.point_de_vue) ;
}


void u_vue_2d_3d(Vue* vue, int x, int y, Triplet *pt, int hau)
{
  GLdouble mv[16], pr[16] ;
  GLint vp[4] ;
  GLdouble xx, yy, zz, d, d2 ;

  if ( strcmp("vue_incluse", OBJET(vue)->classe->nom) == 0 )
    {
      Vue_incluse *vue_incluse = (Vue_incluse*)vue ;
      glPushAttrib(GL_VIEWPORT_BIT) ;
      glViewport(vue_incluse->xmin
		 , vue_incluse->ymin
		 , vue_incluse->xmax-vue_incluse->xmin
		 , vue_incluse->ymax-vue_incluse->ymin) ;
    }

  u_vue_matrice(vue) ;

  glGetDoublev(GL_MODELVIEW_MATRIX,mv) ;
  glGetDoublev(GL_PROJECTION_MATRIX,pr) ;
  glGetIntegerv(GL_VIEWPORT,vp) ;

  gluUnProject( (float)x,
		(float)(hau-y),
		(float)1.,
		mv,
		pr,
		vp,
		&xx, &yy, &zz) ;
  if ( !vue->b_persp.index )
    {
      if ( vue->regard.y==0 && vue->regard.z==0 ) xx = 0 ;
      if ( vue->regard.x==0 && vue->regard.z==0 ) yy = 0 ;
      if ( vue->regard.x==0 && vue->regard.y==0 ) zz = 0 ;
    }
  else
    {
      /*
       * Ramène à une distance raisonnable
       */
      d = u_distance_triplet(&vue->observateur.point_de_vue,
			     &vue->observateur.point_vise) ;

      pt->x = xx ;
      pt->y = yy ;
      pt->z = zz ;
      d2 = u_distance_triplet(&vue->observateur.point_de_vue, pt) ;

      if ( 0 )
	{
	  /*
	   * Distance Euclidienne
	   */
	  *pt = u_ponderation_2_triplet(pt
					, d/d2
					, &vue->observateur.point_de_vue
					, 1-d/d2
					) ;
	}
      else
	{
	  /*
	   * Distance plan projection
	   */
	  Triplet v_regard, v_point ;
	  float dp ;

	  v_regard = u_soustrait_triplet(&vue->observateur.point_vise
					 , &vue->observateur.point_de_vue) ;
	  v_regard = u_normalise_triplet(&v_regard) ;

	  v_point = u_soustrait_triplet(pt, &vue->observateur.point_de_vue) ;

	  dp = u_ps_triplet(&v_regard, &v_point) ;

	  v_point = u_triplet_fois_flottant(&v_point, d/dp) ;
	  *pt = u_ajoute_triplet(&vue->observateur.point_de_vue, &v_point) ;
	}
      xx = pt->x ;
      yy = pt->y ;
      zz = pt->z ;
    }
      
  if ( vue->b_entier )
    {
      xx = rint(10*xx)/10. ;
      yy = rint(10*yy)/10. ;
      zz = rint(10*zz)/10. ;
    }
  
  pt->x = xx ;
  pt->y = yy ;
  pt->z = zz ;

  if ( strcmp("vue_incluse", OBJET(vue)->classe->nom) == 0 )
    {
      glPopAttrib() ;
    }
}

static Booleen applique_vue(const Interface_widget *iw)
{
  applique_fenetre(iw->objet) ;
  return(0) ;
}

static void affiche_gl_qualite(const Vue *vue, const Liste *a_afficher)
{
  char qualite_et_urgent[2] ;

  qualite_et_urgent[0] = vue->menu_qualite.selection[0] ;
  qualite_et_urgent[1] = '\0' ;
  affiche_gl(a_afficher, qualite_et_urgent) ;
}

/*
 * Verifie si le haut est perpendiculaire au regard
 */

static void changement_vue(Vue *o)
{
  if ( CREATION(o) )
    {
      o->zoomout = 1 ;
      o->zoomin = 1 ;
      o->width = o->gl.lar ;
      o->height = o->gl.hau ;
    }
  if ( FILS_CHANGE(o) )
    recupere_fenetre(o) ;
  
  if ( !UN_CHAMP_CHANGE(o) )
    return ;

  if ( OBJET(o)->edite )
    {
      /* Gl_start(&o->gl) ; 18/10/2001 car inutile ? */
      o->ratio = o->gl.lar / (float)o->gl.hau ;
    }
  else
    o->ratio = o->observateur.ratio ;

  /*
  if ( CHAMP_CHANGE(o, observateur) && o->ratio != o->observateur.ratio &&
       OBJET(o)->edite && !CREATION(o) )
    {
      o->gl.hau = o->gl.lar/o->observateur.ratio ;
      widget_resize(o->gl.gl, o->gl.lar, o->gl.hau) ;
    }
  else
  */
      o->observateur.ratio = o->ratio ;

  o->regard = u_soustrait_triplet(&o->observateur.point_vise,
				  &o->observateur.point_de_vue) ;
  o->droite = u_pv_triplet( &o->regard, &o->observateur.haut) ;
  
  if ( fabs(u_ps_triplet(&o->observateur.haut, &o->regard)) > 1e-4)
    {
      /*
       * Ne teste pas le cas nul... BEURK
       */
      o->observateur.haut = u_pv_triplet(&o->observateur.haut, &o->regard) ;
      o->observateur.haut = u_pv_triplet(&o->regard, &o->observateur.haut) ;
      o->observateur.haut = u_normalise_triplet(&o->observateur.haut) ;
    }
}


static void u_mouvement_observateur_tp(Vue *vue, Flottant teta, Flottant phi)
{
  Triplet tmp[3] ;  
  int i ;

  tmp[0] = vue->regard ;
  tmp[1] = vue->observateur.haut ;
  tmp[2] = vue->droite ;
  for(i=0;i<3;i++)
    {
      tmp[i] = u_rotation_triplet(&tmp[i], &vue->observateur.haut,  teta/100) ;
      tmp[i] = u_rotation_triplet(&tmp[i], &vue->droite, phi/100) ;
    }
  vue->regard = tmp[0] ;
  vue->observateur.haut   = tmp[1] ;
  vue->droite = tmp[2] ;
  
  vue->observateur.point_de_vue =
    u_soustrait_triplet(&vue->observateur.point_vise, &vue->regard) ;
}

void t_triangle__(const Triplet *a, const Triplet *b, const Triplet *c)
{
  Triplet n ;

  n = u_normale_triangle(a, b, c) ;
  glNormal3f(n.x,n.y,n.z) ;
  t_triplet(a) ;
  t_triplet(b) ;
  t_triplet(c) ;
}

void figure_de_test()
{
  int i, j ;
  Triplet p1, p2, p3 ;

  if ( 0 )
    {
      int i ;
      Triplet pt[4] ;
      
      u_2d_3d(-1, -1, &pt[0]) ;
      u_2d_3d(1, -1, &pt[1]) ;
      u_2d_3d(1, 1, &pt[2]) ;
      u_2d_3d(-1, 1, &pt[3]) ;
      ICI ;
      for(i=0; i<4; i++)
	eprintf("%s\n", triplet_vers_char(&pt[i])) ;
      
      glFlush() ;
      glBegin(GL_QUADS) ;
      t_triplet(&pt[0]) ;
      t_triplet(&pt[1]) ;
      t_triplet(&pt[2]) ;
      t_triplet(&pt[3]) ;
      glEnd() ;      
      glFlush() ;
    }
  
  if ( 0 )
    {
      glEnable(GL_LIGHT0) ;
      glEnable(GL_LIGHTING) ;
      glBegin(GL_TRIANGLES) ;
      glNormal3f(0,0,1) ;
      
      for(j=0;j<10;j++)
	for(i=0;i<10;i++)
	  {
	    p1.x = i/5. -1 ;
	    p1.y = j/5. -1 ;
	    p1.z = 0 ;
	    p2.x = (i+1)/5. -1 ;
	    p2.y = j/5. -1 ;
	    p2.z = 0 ;
	    p3.x = (i+1)/5. -1 ;
	    p3.y = (j+1)/5. -1 ;
	p3.z = 0 ;
	
	
	t_triangle__(&p1, &p2, &p3) ;
	
	  }
      
      glEnd() ;
      
      glDisable(GL_LIGHTING) ; 
    }
}


static int taille_hemi(Vue *o)
{
  return (MIN(o->gl.lar, o->gl.hau) / (3*16))*(3*16) ;
}

static void u_vue_trace_reel(Vue *o)
{
  Flottant d ;

  if ( o->width != o->gl.lar || o->height != o->gl.hau )
    {
      o->width = o->gl.lar ;
      o->height = o->gl.hau ;
      eprintf("Taille image : %d %d\n", o->width, o->height) ;
    }

  if ( TRACE(o) )
    eprintf("u_vue_trace_reel debut\n") ;

  u_vue_matrice(o) ;
  
  if ( o->b_grille )
    {
      d = u_norme_triplet(&o->regard) ;
      t_sol(&o->regard, d>10 ? 10. : d) ;
    }
  if ( o->b_repere )
    t_repere() ;
  
  glColor3f(1.,1.,1.) ;

  figure_de_test();

  affiche_gl_qualite(o, fils_a_afficher(o)) ;
}

static void dump_image_reel(Vue *o, int zoomin, int zoomout,FILE *f) ;

const Flottant global_teta = 4 ; /* Écart des yeux pour la stéréo */


static Parcour trouve_seconde_(void *o, void *data)
{
  if ( strcmp(OBJET(o)->classe->nom, "seconde") == 0)
    {
      *(void**)data = o ;
      return Arrete ;
    }
  return Continue ;
}

static Flottant trouve_seconde(Vue *o)
{
  void *seconde = NULL ;
  o_parcour_liste((void*)fils_a_afficher(o), trouve_seconde_, NULL, &seconde) ;

  if ( seconde == NULL )
    return ((Modele*)u_operateur_ou_modele(o))->numero_sauvegarde ;

  return atof( c_recupere_sans_copie(seconde, "temps") ) ;
}


static void u_vue_trace(const Interface_widget *iw)
{
  Vue *o ;
  Modele *op ;
  struct tms tms_debut, tms_fin ;

  o = iw->objet ;
  op = u_operateur_ou_modele(o) ;
  if ( op == NULL )
    return ;



  if ( op->animation )
    {
      char nom[999] ;
      FILE *f ;
      int animation = op->animation ;
      int ieme ;
      op->animation = 0 ;
      ieme = rint(1000*trouve_seconde(o)) ;
      mkdir("ANIMATION", 0755) ;
      /*      sprintf(nom, "SHELL/hemi %d|convert ppm:- png:ANIMATION/%02dm%02ds%03d &", */
      sprintf(nom, "convert ppm:- -quality 100 jpg:ANIMATION/%02dm%02ds%03d",
	      ieme/(1000*60),
	      (ieme/1000)%60,
	      ieme%1000
	      ) ;
	      
      f = popen(nom, "w") ;
      eprintf("%s\n", nom) ;
      if ( f )
	{
	  time_t t1, t2 ;
	  time(&t1) ;
	  dump_image_reel(o, o->zoomin, o->zoomout, f) ;
	  pclose(f) ;
	  time(&t2) ;
	  eprintf("Temps calcul cette image : %d secondes\n", t2-t1) ;
	}
      op->animation = animation ;
    }



  times(&tms_debut) ;
  /*
  if ( o->gl.gl == 0 )
    return ;
  */
  globale_etat_courant.gl = Gl_start(&o->gl) ;

  if ( o->double_buffer )
    glDrawBuffer(GL_BACK) ;
  else
    glDrawBuffer(GL_FRONT) ;

  o->ratio = o->gl.lar / (float)o->gl.hau ;

  if ( TRACE(o) )
    {
      o_dump(o) ;
    }

  glDepthMask(1) ;
  glEnable(GL_DEPTH_TEST) ;     /* Elimine les faces cachees */
  glEnable(GL_NORMALIZE) ;
  glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, o->b_local) ;
  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1) ;
  glClearColor(o->couleur_fond.x,o->couleur_fond.y,o->couleur_fond.z,0) ;
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) ; /* Efface TOUT */

  if ( TRACE(o) )
    eprintf("Initialisations GL terminé\n") ;

  if ( o->b_stereo )
    {
      o->ratio /= 2 ;
      glViewport(o->gl.lar/2, 0, o->gl.lar/2, o->gl.hau) ; /* BUG GL ? */
      glViewport(o->gl.lar/2, 0, o->gl.lar/2, o->gl.hau) ;
      u_mouvement_observateur_tp(o, -global_teta/2, 0.) ;
      u_vue_trace_reel(o) ;
      glViewport(0, 0, o->gl.lar/2, o->gl.hau) ;
      u_mouvement_observateur_tp(o, 2*global_teta/2, 0.) ;
      u_vue_trace_reel(o) ;
      o->ratio *= 2 ;
      u_mouvement_observateur_tp(o, -global_teta/2, 0.) ;
      glViewport(0, 0, o->gl.lar, o->gl.hau) ;
    }
  else
    {
      if ( o->b_persp.index == 2 ) // Hémisphère
	{
	  Triplet v ;
	  t_observateur tmp ;
	  int lar, hau ;
	  Flottant ratio ;

	  lar = o->gl.lar ;
	  hau = o->gl.hau ;
	  ratio = o->ratio ;
	  o->gl.lar = o->gl.hau = taille_hemi(o) ;

	  o->ratio = 1 ;

	  tmp = o->observateur ;
	  v = u_soustrait_triplet(&o->observateur.point_vise
				  ,&o->observateur.point_de_vue) ; 

	  /* Haut */
	  glViewport(o->gl.lar/3, o->gl.hau/3, o->gl.lar/3, o->gl.hau/3) ;
	  u_vue_trace_reel(o) ;


	  /* Gauche */
	  glViewport(0, o->gl.hau/3, o->gl.lar/3, o->gl.hau/3) ;
	  o->observateur.point_vise =
	    u_pv_triplet(&o->observateur.haut, &v) ;
	  o->observateur.point_vise =
	    u_ajoute_triplet(&o->observateur.point_vise
			     , &o->observateur.point_de_vue);
	  u_vue_trace_reel(o) ;

	  /* Droite */
	  glViewport((2*o->gl.lar)/3, o->gl.hau/3, o->gl.lar/3, o->gl.hau/3) ;
	  o->observateur.point_vise =
	    u_pv_triplet(&v, &o->observateur.haut) ;
	  o->observateur.point_vise =
	    u_ajoute_triplet(&o->observateur.point_vise
			     , &o->observateur.point_de_vue);
	  u_vue_trace_reel(o) ;

	  /* Derrière */
	  glViewport(o->gl.lar/3, (2*o->gl.hau)/3, o->gl.lar/3, o->gl.hau/3);
	  o->observateur.point_vise =
	    u_ajoute_triplet(&o->observateur.point_de_vue
			     , &o->observateur.haut);
	  o->observateur.haut = u_triplet_fois_flottant(&v, -1) ;
	  u_vue_trace_reel(o) ;
	  o->observateur = tmp ;

	  /* Devant */
	  glViewport(o->gl.lar/3, 0, o->gl.lar/3, o->gl.hau/3) ;
	  o->observateur.point_vise =
	    u_soustrait_triplet(&o->observateur.point_de_vue
			     , &o->observateur.haut);
	  o->observateur.haut = v ;
	  u_vue_trace_reel(o) ;

	  /* Bas */
	  glViewport(0, 0, o->gl.lar/3, o->gl.hau/3) ;
	  o->observateur.point_vise =
	    u_soustrait_triplet(&tmp.point_de_vue, &v);
	  o->observateur.haut = tmp.haut ;
	  u_vue_trace_reel(o) ;

	  
	  o->observateur = tmp ;
	  o->gl.lar = lar ;
	  o->gl.hau = hau ;
	  o->ratio = ratio ;
	}
      else
	{
	  u_vue_trace_reel(o) ;
	}
    }

  if ( TRACE(o) )
   eprintf("Fin affichage\n") ;
  if ( o->double_buffer )
    Gl_stop(&o->gl) ;
  else
    Gl_stop_sans_swapping(&o->gl) ;

  times(&tms_fin) ;
  o->temps_en_clockticks = tms_fin.tms_utime - tms_debut.tms_utime ;

  /*
  if ( OBJET(o)->urgent )
    OBJET(o)->mettre_a_jour_widget = Vrai ;
  */
}

static Vue *globale_vue_courante ;

static void trouve_etat_courant(void *o, unsigned int pos)
{
  int i ;

  for(i=0; i<globale_vue_courante->nb; i++)
    {
      if ( globale_vue_courante->designes[i].vue )
	continue ; /* Travail déjà fait dans la vue incluse */
      if ( TRACE(o) )
	eprintf("[%d/%d] %p(%s)/%d == %p(%s)/%d ?\n", i , globale_vue_courante->nb
		, o
		, OBJET(o)->classe->nom
		, pos
		, globale_vue_courante->designes[i].objet
		, OBJET(globale_vue_courante->designes[i].objet)->classe->nom
		, globale_vue_courante->designes[i].position
		) ;

      if ( globale_vue_courante->designes[i].objet == o
	   && globale_vue_courante->designes[i].position == pos )
	{
	  globale_vue_courante->designes[i].etat_courant =
	    globale_etat_courant ;
	  globale_vue_courante->designes[i].vue = globale_vue_courante ;
	}
    }
}

static int m_trouve_objet(Vue *o, int x, int y, struct selection *trouve)
{
  GLuint selectBuf[BUFSIZE] ;
  GLint vp[4] ;
  int hits ;
  int i, j ,position, taille ;
  int nb, nb2 ;
  Vue_incluse *vue_incluse, *vi ;
  Liste *vue_incluses ;
  
  if ( TRACE(o) )
   eprintf("Designation o=%p (%s) x=%d y=%d\n"
	   , o, OBJET(o)->classe->nom, x,y) ;

  o->x_init  = x ;
  o->y_init  = y ;
  o->x__init = x ;
  o->y__init = y ;
  vue_incluse = ( strcmp(OBJET(o)->classe->nom, "vue_incluse") == 0 )
    ? (Vue_incluse*)o : NULL ;

  if ( o->b_stereo )
    {
      o->ratio /= 2 ;
      if ( x < o->gl.lar/2 )
	{
	  u_mouvement_observateur_tp(o, -global_teta, 0.) ;
	  glViewport(0, 0, o->gl.lar/2, o->gl.hau) ;
	}
      else
	{
	  u_mouvement_observateur_tp(o, global_teta, 0.) ;
	  glViewport(o->gl.lar/2, 0, o->gl.lar/2, o->gl.hau) ;
	}
    }

  if ( vue_incluse )
    {
      glViewport(vue_incluse->xmin
		 , vue_incluse->ymin
		 , vue_incluse->xmax-vue_incluse->xmin
		 , vue_incluse->ymax-vue_incluse->ymin) ;
      
    }
  glGetIntegerv(GL_VIEWPORT,vp) ;
  glSelectBuffer(BUFSIZE, selectBuf ) ;
  glRenderMode(GL_SELECT) ;

  u_vue_matrice_init(o) ;

  gluPickMatrix( (float)x, (float)(vp[3]-y), o->dts, o->dts, vp ) ;
  glViewport(x-o->dts/2, (vp[3]-y)-o->dts/2, o->dts, o->dts) ;
  u_vue_matrice_suite(o) ;
  globale_etat_courant.designable = Vrai ;
  if ( vue_incluse && TRACE(o) )
    {
      eprintf("Fils a afficher : ") ;
      POUR(void*, f, fils_a_afficher(o), eprintf(" %s", OBJET(f)->classe->nom););
      eprintf("\n") ;
    }
  affiche_gl_qualite(o, fils_a_afficher(o)) ;
  globale_etat_courant.designable = Faux ;
  hits = glRenderMode(GL_RENDER) ;


  if ( o->b_stereo )
    {
      o->ratio *= 2 ;
      if ( x < o->gl.lar/2 )
	{
	  u_mouvement_observateur_tp(o, global_teta, 0.) ;
	}
      else
	{
	  u_mouvement_observateur_tp(o, -global_teta, 0.) ;
	}
    }
  glViewport(vp[0], vp[1], vp[2], vp[3]) ;
  nb = 0 ;

  position = 0 ;
  if ( TRACE(o) )
   eprintf("hits=%d\n",hits) ;
  vue_incluses = NULL ;
  for(i=0;i<hits;i++)
    {
      taille = selectBuf[position] ;
      if ( TRACE(o) )
        eprintf("taille=%d min=%d max=%d position=%d objet=%p sous_objet=%p\n",
               taille, selectBuf[position+1], selectBuf[position+2],
               selectBuf[position+3], selectBuf[position+4]
		,selectBuf[position+5]) ;
      if ( selectBuf[position+4] )
        {
	  trouve[nb].vue = NULL ;
	  trouve[nb].zmin = selectBuf[position+1] ;
	  trouve[nb].zmax = selectBuf[position+2] ;
	  trouve[nb].position = (unsigned int) selectBuf[position+3] ;
	  trouve[nb].objet = OBJECT_PTR(selectBuf[position+4]) ;
	  if ( taille>=3 )
	    trouve[nb].sous_objet = (int) selectBuf[position+5] ;
	  else
	    trouve[nb].sous_objet = -1 ;
	  
	  for(j=0;j<nb;j++)
	    if ( trouve[j].objet == trouve[nb].objet
		 && trouve[j].sous_objet == trouve[nb].sous_objet )
	      {
		nb-- ;
		break ;
	      }
	  nb++ ;
	  if ( strcmp(OBJET(trouve[nb-1].objet)->classe->nom,"vue_incluse")==0 )
	    {
	      if ( TRACE(o) )
		eprintf("Vue incluse touchée : cherche contenu\n") ;
	      vi = trouve[nb-1].objet ;
	      vi->affiche_contenu = Vrai ;
	      l_ajoute(&vue_incluses, vi) ;
	      nb2 = m_trouve_objet(&vi->vue, x, (vi->ymax-vi->ymin)-(vp[3]-y)
				   , vi->vue.designes) ;
	      if ( TRACE(o) )
		eprintf("Vue incluse touchée : fin affichage contenu\n") ;
	      /* 10/03/2003 */
	      // vi->affiche_contenu = Faux ;
	      if ( nb2 || !vi->est_designable )
		nb-- ;
	      for(j=0; j<nb2; j++)
		{
		  trouve[nb++] = vi->vue.designes[j] ;
		  if ( TRACE(o) )
		    eprintf("Trouve %s\n", OBJET(trouve[nb-1].objet)->classe->nom) ;
		}
	    }
        }
      position += 3 + taille ;
    }
  if ( TRACE(o) )
    eprintf("Tout les objets cliqués ont été parcouru\n") ;

  /*
   * Retrouve les états courants
   */
  globale_vue_courante = o ;
  o->nb = nb ;
  globale_etat_courant.detourne_affichage = trouve_etat_courant ;
  glRenderMode(GL_SELECT) ;
  globale_etat_courant.designable = Vrai ;
  glViewport(vp[0], vp[1], vp[2], vp[3]) ;
  if ( vue_incluse && TRACE(o)  )
    {
      eprintf("Fils a afficher pour retrouver etat courant : ") ;
      POUR(void*, f, fils_a_afficher(o), eprintf(" %s", OBJET(f)->classe->nom););
      eprintf("\n") ;
    }
  if ( TRACE(o) )
    eprintf("Affichage pour chercher l'état courant\n") ;
  affiche_gl_qualite(o, fils_a_afficher(o)) ;

  /* 10/03/2003 */
  POUR(Vue_incluse*, vue_inc, vue_incluses,vue_inc->affiche_contenu = Faux ;) ;
  l_detruit(&vue_incluses) ;

  globale_etat_courant.designable = Faux ;
  glRenderMode(GL_RENDER) ;
  globale_etat_courant.detourne_affichage = NULL ;
  
  /*
   * Ceci est là car dans le cas d'un opérateur c'est l'opérateur
   * que l'on désigne et le contenu de l'opérateur qui passe
   * dans le detournement affichage et ne sont donc pas trouvé.
   */
  for(i=0;i<nb;i++)
    if ( trouve[i].vue == NULL )
      trouve[i].vue = o ;

  if ( TRACE(o) )
    {
      eprintf("Ce qui a été désigné dans la vue %p(%s) :\n", o,
	      OBJET(o)->classe->nom) ;
      for(i=0;i<nb;i++)
	{
	  eprintf("objet=%p(%s) position=%8d sous_objet=%8x zmin=%d zmax=%d\n"
		  , trouve[i].objet, OBJET(trouve[i].objet)->classe->nom
		  , trouve[i].position, trouve[i].sous_objet
		  , trouve[i].zmin, trouve[i].zmax
		  ) ;
	  eprintf("TRANSFO = %s\n"
		  , codec_recupere_texte(codec_transfo
					 , &trouve[i].etat_courant.transfo
					 )) ;
	}
      eprintf("Fin liste\n") ;
    }

  /*
   * Ne sélectionne que le plus proche si l'on ne clique pas sur un TAG
   */
  for(i=0; i<nb; i++)
    if ( trouve[i].sous_objet != -1 )
      break ;
  if ( i == nb )
    {
      int m ;
      
      if ( TRACE(o) )
	eprintf("On a pas cliqué sur un tag : on laisse le plus proche\n") ;
      m = 1<<30 ;
      j = -1 ;
      for(i=0; i<nb; i++)
	if ( trouve[i].zmin < m )
	  {
	    m = trouve[i].zmin ;
	    j = i ;
	  }
      nb2 = 0 ;
      if ( j >= 0 )
	{
	  for(i=0; i<nb; i++)
	    if ( trouve[i].objet == trouve[j].objet
		 || trouve[i].zmin < m+10 )
	      {
		trouve[nb2++] = trouve[i] ;
	      }
	}
      nb = nb2 ;
    }
  

  return(nb) ;
}

/*
 * Trouve la coordonnée qu'il ne faut pas changer
 * par ce que cela n' pas de sens (vue X Y ou Z).
 * Si on peut toute les changer, cela retourne "4"
 */

static int numero_mauvaise_coordonnee(const Vue *vue)
{
  int i, j ;

  j = 5 ;
  for(i=0;i<3;i++)
    if ( abs( (&vue->observateur.point_de_vue.x)[i] - (&vue->observateur.point_vise.x)[i]) > 1e-4 )
      {
	if ( j == 5 )
	  j = i ;		/* Cette coordonnée est mauvaise */
	else
	  {
	    j = 4 ;		/* NON : elle sont toutes bonne (persp) */
	    break ;
	  }
      }
  return(j) ;
}


static void u_mouvement_observateur(Vue *vue, int x, int y, Triplet vecteur)
{
  if ( !vue->b_persp.index )
    {
      vue->observateur.point_de_vue = u_ajoute_triplet( &vue->observateur.point_de_vue, &vecteur) ;
      vue->observateur.point_vise = u_ajoute_triplet( &vue->observateur.point_vise, &vecteur) ;
    }
  else
    {
      u_mouvement_observateur_tp(vue, vue->x_init - x, vue->y_init - y) ;
    }
}

static const Champ *trouve_tag(const void *o, int n)
{
  const Champ *ch ;

  ch = u_champ_trouve_n(&o, n, O_Init3d) ;
  if ( ch == NULL )
    {
      c_trouve_type(&o, &ch, &infos_L_tag) ;
      if ( ch==NULL )
	{
	  eprintf("Ne trouve pas le tag %d dans l'objet\n", n) ;
	  o_sauve(o, stderr) ;		
	  return(NULL) ;
	}
      if ( TRACE2(o) )
	eprintf("Tag trouve : %x\n", n) ;
      ecrit_numero_tag(ch,n) ;
    }
  return(ch) ;
}

static Triplet repositionne_point(const Transfo *inverse
				  , const Triplet *cible
				  , const Triplet *orig
				  , int mauvaise_coordonnee
				  )
{
  /*
    3 équation de la forme, avec une seule inconnue : cible->[xyz]
    v1.x*cible->x + v2.x*cible->y + v3.x*cible->z + o.x = nouveau.x

    Dérivée de la Distance entre "orig" et "nouveau" qui doit être
    nulle si l'on veut être au plus près du point :
    orig->x-nouveau.x + orig->y-nouveau.y + orig->y-nouveau.z = 0

Petit Programme Maple
---------------------

nx := v1x*cx+v2x*cy+v3x*cz+ox ;
ny := v1y*cx+v2y*cy+v3y*cz+oy ;
nz := v1z*cx+v2z*cy+v3z*cz+oz ;
solve(diff((nx-px)^2 + (ny-py)^2 + (nz-pz)^2, cx) = 0, cx );

Bizarrement, il faut inverser le signe donné par Mapple
pour intégrer cela dans le programme.
JE NE COMPREND PAS.

  */
  Triplet c ;

#define v1x (inverse->v1.x)
#define v1y (inverse->v1.y)
#define v1z (inverse->v1.z)
#define v2x (inverse->v2.x)
#define v2y (inverse->v2.y)
#define v2z (inverse->v2.z)
#define v3x (inverse->v3.x)
#define v3y (inverse->v3.y)
#define v3z (inverse->v3.z)
#define ox (inverse->origine.x)
#define oy (inverse->origine.y)
#define oz (inverse->origine.z)
#define cx (c.x)
#define cy (c.y)
#define cz (c.z)
#define px (orig->x)
#define py (orig->y)
#define pz (orig->z)

  c = *cible ;
  switch(mauvaise_coordonnee)
    {
    case 0:
      cx = -(-v1x*px+v1x*v2x*cy+v1x*v3x*cz+v1x*ox-v1y*py+v1y*v2y*cy+v1y*v3y*cz+v1y*oy-v1z*pz+v1z*v2z*cy+v1z*v3z*cz+v1z*oz)/(v1x*v1x+v1y*v1y+v1z*v1z) ;
      break ;
    case 1:
      cy = -(v2x*v1x*cx+v2x*v3x*cz+v2x*ox-v2x*px+v2y*v1y*cx+v2y*v3y*cz+v2y*oy-v2y*py+v2z*v1z*cx+v2z*v3z*cz+v2z*oz-v2z*pz)/(v2x*v2x+v2y*v2y+v2z*v2z) ;
      break ;
    case 2:
      cz = -(v3x*v1x*cx+v3x*v2x*cy+v3x*ox-v3x*px+v3y*v1y*cx+v3y*v2y*cy+v3y*oy-v3y*py+v3z*v1z*cx+v3z*v2z*cy+v3z*oz-v3z*pz)/(v3x*v3x+v3y*v3y+v3z*v3z) ;
      break ;
    }

#undef v1x
#undef v1y
#undef v1z
#undef v2x
#undef v2y
#undef v2z
#undef v3x
#undef v3y
#undef v3z
#undef ox
#undef oy
#undef oz
#undef cx
#undef cy
#undef cz
#undef px
#undef py
#undef pz
  
  return(u_transforme(inverse, &c)) ;
}

static int u_decoupe_chaine(const char *v, Quadruplet *q)
{
  int i ;

  *q = char_vers_quadruplet(v) ;
  for(i=0; i<5; i++)
    {
      v = strchr(v, ' ') ;
      if ( v == NULL )
	return(i) ;
      v += strspn(v, " ") ;
    }
  return(i) ;
}

static void u_deplace_tag_profondeur(const Vue *vue, struct selection *s,
				     Flottant d)
{
  const Champ *ch ;
  Quadruplet q ;
  Triplet p ;

  ch = trouve_tag(s->objet, s->sous_objet) ;
  if ( ch )
    switch( u_decoupe_chaine(c_recupere_ici_sans_copie(s->objet, ch), &q) )
      {
      case 0:
	c_sauve_ici(s->objet, ch, flottant_vers_char(q.x + d)) ;
	break ;
      case 1:
	break ;
      case 2:
	p = u_soustrait_triplet((Triplet*)&q, &vue->observateur.point_de_vue) ;
	p = u_triplet_fois_flottant(&p, vue->pas_vue*d) ;
	p = u_ajoute_triplet((Triplet*)&q, &p) ;
	c_sauve_ici(s->objet, ch, triplet_vers_char(&p)) ;
	break ;
      case 3:
	q.h += d ;
	c_sauve_ici(s->objet, ch, quadruplet_vers_char(&q)) ;
	break ;
      }
}

static void u_deplace_tag(const Vue *vue, struct selection *s
			  , const Triplet *p
			  , int x, int y)
{
  const Champ *ch ;
  Triplet pt, regard, pt_de_vue ;
  int i ;
  Transfo t ;
  struct globale_etat_courant ec ;
  static Chaine st ;
  Flottant poids ;
  Quadruplet q ;
  Flottant d_ancien, d_nouveau ;

  ch = trouve_tag(s->objet, s->sous_objet) ;
  if ( ch )
    {
      /*
       * Calcul du vecteur regard
       */
      t = inversion_transfo(&s->etat_courant.transfo) ;
      pt = u_transforme(&t, p) ;
      
      if ( vue->b_persp.index )
	{
	  pt_de_vue = u_transforme(&t, &vue->observateur.point_de_vue);
	  regard = u_soustrait_triplet(&pt,&pt_de_vue);
	}
      else
	{
	  regard = u_transforme(&t, &vue->regard);
	  /* Beurk, il faut transfo*vecteur */
	  regard = u_soustrait_triplet(&regard, &t.origine) ;
	}
      /*
       *
       */
      poids = 0 ;
      switch( u_decoupe_chaine(c_recupere_ici_sans_copie(s->objet, ch), &q) )
	{
	case 0:
	  u_annule_triplet(&pt) ;
	  pt.x = x/100. ;
	  break ;
	case 1:
	  u_annule_triplet(&pt) ;
	  pt.y = y/100. ;
	  pt.x = x/100. ;
	  break ;
	case 3:
	  poids = q.h ;
	case 2:
	  i = numero_mauvaise_coordonnee(vue) ;
	  /*
	   * Je met le 1|| en commentaire le 22/10/2001
	   * car cela empêche de déplacer plusieurs tag superposés
	   * sans changer leur profondeur (voir les grilles)
	   */
	  if ( /* 1 || */ i >= 3 )
	    {
	      /*
	       * Changement position dans ``vue 3d''
	       * On conserve le point à distance constant de l'oeil
	       */
	      d_nouveau = u_distance_triplet(&pt_de_vue, &pt) ;
	      d_ancien = u_distance_triplet(&pt_de_vue, (Triplet*)&q) ;
	      pt = u_soustrait_triplet(&pt, &pt_de_vue) ;
	      pt = u_triplet_fois_flottant(&pt,
					   d_ancien/d_nouveau
					   ) ;
	      pt = u_ajoute_triplet(&pt, &pt_de_vue) ;
	    }
	  else
	    {
	      /*
	       * Changement position dans vue ``orthogonale''.
	       * Le but est de ne pas changer l'une des coordonnées
	       * alors que le point a subit des transformations.
	       * C'est non trivial et difficile à expliquer sans faire
	       * des dessins.
	       */
	      pt.x = q.x ;
	      pt.y = q.y ;
	      pt.z = q.z ;
	      pt = repositionne_point(&t, p, &pt, i) ;
	    }
	}

      s_efface(&st) ;
      s_printf(&st, "%s", triplet_vers_char(&pt)) ;
      if ( poids )
	s_printf(&st, " %s", flottant_vers_char(poids)) ;	
      /*
       * Vecteur Regard dans le repère de l'objet
       */
      s_printf(&st, " (%s)", triplet_vers_char(&regard)) ;

      /*
       * La restauration de l'état courant avant la sauvegarde
       * a été introduit parce que c'était nécessaire dans le cas
       * ou le fonctionnement dépendait des variables courantes.
       * Exemple :
       *    - Un opérateur contenant un "segment" paramétré par des variables.
       *      Ce qui doit être déplacé par le tag est le vrai point
       *      et non celui par défaut.
       */
      ec = globale_etat_courant ;
      globale_etat_courant = s->etat_courant ;

      /*
       * Il faut récupérer les valeurs des fils car ce
       * sont peut-être des variables
       */
      POUR_LES_CHAMPS_CLASSE(s->objet, ch,
			     if ( EXTRAIT(ch) )
			       c_monte_valeur(s->objet, ch) ;
			     ) ;
      c_sauve_ici(s->objet, ch, s_char(&st) ) ;
      o_appel_changement(s->objet) ;
      globale_etat_courant = ec ;

      /* A cause des OPERATEUR pour remonter le changement */
      evaluation(s->objet) ;
    }
}

static NanoWidget globale_bulle = NULL ;
static int globale_bulle_date = 0 ;

char *trouve_bulle_d_aide(void *o)
{
  char *t ;

  if ( strcmp(OBJET(o)->classe->nom, "bulle_d_aide") == 0 )
    return( *(char**)o ) ;
  POUR_LES_PARENTS(o, p,
  {
    t = trouve_bulle_d_aide(p) ;
    if ( t ) 
      return(t) ;
  }) ;
  return(NULL) ;
}

static Booleen u_bulle(void *objet)
{
  int i ;
  Vue *vue = objet ;
  char *t ;
  static Chaine s ;

  globale_etat_courant.gl = Gl_start(&vue->gl) ;

  vue->nb      = m_trouve_objet( vue, vue->x_init, vue->y_init, vue->designes);

  s_efface(&s) ;
  if ( vue->nb )
    {
      for(i=0;i<vue->nb;i++)
	{
	  t = trouve_bulle_d_aide(vue->designes[i].objet) ;
	  if ( t )
	    {
	      s_printf(&s, "%s", t) ;
	      break ;
	    }
	}
      if ( globale_bulle == NULL && s_char(&s)[0] )
	{
	  globale_bulle = widget_vrai_bulle_d_aide(OBJET(vue)->edite
						   , s_char(&s)) ;
	  widget_positionne(globale_bulle
			    , vue->x_init_root+50, vue->y_init_root) ;
	  widget_popup(globale_bulle) ;
	}
    }

  Gl_stop_sans_swapping(&vue->gl) ;

  return(0) ;
}

static int global_timer = 0 ;

static void destruction_vue(Vue *vue)
{
  if ( global_timer )
    {
      widget_enleve_timeout(global_timer) ;  
      global_timer = 0 ;
    }
}

static void destruction_bulle()
{
  if ( global_timer )
    widget_enleve_timeout(global_timer) ;
  if ( globale_bulle )
    {
      widget_detruit(globale_bulle) ;
      globale_bulle = NULL ;
    }
}

/*
 * Deplacement de l'oeil
 */

static void e_rotation(Vue* o, int dir)
{
  Triplet axe ;

  NOTE_UN_CHAMP_CHANGE(o) ;

  switch( dir )
    {
    case 0 : axe = u_triplet_fois_flottant(&o->observateur.haut, -1.)   ; break ;
    case 1 : axe = o->observateur.haut                                  ; break ;
    case 2 : axe = o->droite                                ; break ;
    case 3 : axe = u_triplet_fois_flottant(&o->droite, -1.) ; break ;
    case 4 : axe = u_triplet_fois_flottant(&o->regard, -1.) ; break ;
    case 5 : axe = o->regard                                ; break ;
    }

  if ( dir>=2 )
    o->observateur.haut = u_rotation_triplet(&o->observateur.haut, &axe, o->angle_vue) ;

  if ( dir<4 )
    {
      o->regard = u_rotation_triplet(&o->regard, &axe, o->angle_vue) ;
      o->observateur.point_vise = u_ajoute_triplet(&o->observateur.point_de_vue, &o->regard) ;
    }
}

static void e_rotation_origine(Vue* o, int dir)
{
  Triplet axe ;

  NOTE_UN_CHAMP_CHANGE(o) ;

  switch( dir )
    {
    case 0 : axe = o->observateur.haut ; break ;
    case 1 : axe = u_triplet_fois_flottant(&o->observateur.haut, -1.) ; break ;
    case 2 : axe = u_triplet_fois_flottant(&o->droite, -1.) ; break ;
    case 3 : axe = o->droite ; break ;
    }
  o->regard = u_rotation_triplet(&o->regard, &axe, o->angle_vue) ;
  o->observateur.point_de_vue = u_soustrait_triplet(&o->observateur.point_vise, &o->regard) ;
  if ( dir<2 )
    {
      o->droite = u_rotation_triplet(&o->droite, &axe, o->angle_vue) ;
      o->observateur.haut = u_pv_triplet(&o->droite, &o->regard) ;
    }
  else
    {
      o->observateur.haut = u_rotation_triplet(&o->observateur.haut, &axe, o->angle_vue) ;
      o->droite = u_pv_triplet(&o->regard, &o->observateur.haut) ;
    }

  o->droite = u_normalise_triplet(&o->droite) ;
  o->observateur.haut = u_normalise_triplet(&o->observateur.haut) ;
}
/*
 *
 */

static void e_deplace(Vue *o, int dire)
{
  Triplet regard, dir, haut ;

  NOTE_UN_CHAMP_CHANGE(o) ;

  if ( !o->b_persp.index )
    {
      if ( dire == 5 )
	{
	  o->observateur.focale += o->pas_vue ;
	  return ;
	}
      if ( dire == 4 )
	{
	  o->observateur.focale -= o->pas_vue ;
	  return ;
	}
    }

  o->regard = u_normalise_triplet(&o->regard) ;
  o->droite = u_normalise_triplet(&o->droite) ;
  regard = u_triplet_fois_flottant(&o->regard, o->pas_vue) ;
  haut = u_triplet_fois_flottant(&o->observateur.haut, o->pas_vue) ;

  switch( dire )
    {
    case 0 : dir = u_triplet_fois_flottant(&o->droite,  o->pas_vue) ; break ;
    case 1 : dir = u_triplet_fois_flottant(&o->droite, -o->pas_vue) ; break ;
    case 2 : dir = haut                                         ; break ;
    case 3 : dir = u_triplet_fois_flottant(&haut, -1.)          ; break ;
    case 4 : dir = regard                                       ; break ;
    case 5 : dir = u_triplet_fois_flottant(&regard, -1.)        ; break ;
    }

  o->observateur.point_de_vue = u_ajoute_triplet( &o->observateur.point_de_vue, &dir) ;

  // XXX 13/4/2010
  //  if ( dire!=4 && dire!=5 )
  o->observateur.point_vise = u_ajoute_triplet( &o->observateur.point_vise, &dir) ;
}

static void u_vue_mouvement(const Interface_widget *iw)
{
  Triplet depart, arrivee, vecteur ;  
  Vue *vue ;
  int i, j ;

  vue          = iw->objet ;

  if ( iw->b == -1 )
    {
      globale_bulle_date = time(0L) ;
      destruction_bulle() ;

      vue->x_init = iw->x ;
      vue->y_init = iw->y ;
      vue->x_init_root = iw->x_root ;
      vue->y_init_root = iw->y_root ;
      global_timer = widget_ajoute_timeout(1000, u_bulle, vue) ;
      return ;
    }

  globale_etat_courant.gl = Gl_start(&vue->gl) ;
      
  if ( vue->nb == 0 )
    {
      if ( !vue->deplacement_observateur_a_la_souris )
	return ;

      u_vue_2d_3d(vue, iw->x, iw->y, &arrivee, vue->gl.hau) ;
      u_vue_2d_3d(vue, vue->x_init, vue->y_init, &depart, vue->gl.hau) ;
      vecteur = u_soustrait_triplet(&depart,&arrivee) ;
      u_mouvement_observateur(vue, iw->x, iw->y, vecteur) ;
      NOTE_UN_CHAMP_CHANGE(vue);
    }
  else
    {
      for(i=0;i<vue->nb;i++)
	if ( vue->designes[i].sous_objet != -1 )
	  {
	    for(j=0;j<i;j++)
	      if ( vue->designes[i].objet==vue->designes[j].objet)
		break ;
	    if ( 1 || i==0 || j == i )
	      {
		u_vue_2d_3d(vue->designes[i].vue, iw->x, iw->y, &arrivee,
			    vue->gl.hau) ;
		u_deplace_tag(vue, &vue->designes[i], &arrivee,
			      iw->x-vue->x__init, iw->y-vue->y__init) ;
	      }
	  }
    }

  vue->x_init = iw->x ;
  vue->y_init = iw->y ;

  Gl_stop_sans_swapping(&vue->gl) ;
}

#define FSI(T,C) if ( strcmp(iw->texte,T)==0 ) { C ; }

static Booleen menu_contextuel(const Interface_widget *iw)
{
  int i, j ;
  void *o ;
  Vue *vue = iw->objet ;

  FSI("Aide",
  {
    u_popup_help_reel(&vue->menu_contextuel_iw) ;
  }) ;

  for(i=0;i<vue->nb;i++)
    {
      /* Pas faire en double l'action */
      for(j=0; j<i; j++)
	if ( vue->designes[i].objet == vue->designes[j].objet )
	  break ;
      if ( j < i )
	continue ;

      o = vue->designes[i].objet ;

      if ( OBJET_OU_NULL(o) )
	{
	  FSI("Regarder surface", regarder(&vue->menu_contextuel_iw,
				   vue, &vue->designes[i], 1) ; ) ;
	  FSI("Regarder dedans", regarder(&vue->menu_contextuel_iw,
				   vue, &vue->designes[i], 0) ; ) ;
	  FSI("Detruire", detruire_objet_remonte_fils(o) ; ) ;
	  FSI("Editer",  ouvrir_fenetre(o) ;  ) ;
	  FSI("Enleve DETRUIT", l_enleve(&OBJET(o)->fils, NULL) ;
	      NOTE_FILS_CHANGE(o) ;
	      ) ;
	  FSI("Duplique",
	      modele_ajoute_fils((Modele*)u_operateur_ou_modele(iw->objet)
				 , duplique_objet(o)) ;
	      ) ;
	}
    }

  return(Faux) ;
}

static Booleen u_vue_entree(const Interface_widget *iw)
{
  int i, j ;
  Triplet v ;
  Booleen un_non_deplacable, un_sous_objet ;
  Vue *vue ;
  int bouton ;

  destruction_bulle() ;

  vue          = iw->objet                                       ;
  globale_etat_courant.gl = Gl_start(&vue->gl) ;
  vue->nb      = m_trouve_objet( vue, iw->x, iw->y, vue->designes) ;
  Gl_stop_sans_swapping(&vue->gl) ;

  if ( vue->bouton_2_et_3_deviennent_1 )
    bouton = 0 ;
  else
    bouton = iw->b ;

  if ( iw->b == 2 && vue->nb )
    {
      vue->menu_contextuel_iw = *iw ;

      widget_arrete_emission_button_press(iw->w) ;

      u_vrai_affiche_menu(iw->w, iw->objet
			  , c_trouve(iw->objet, "menu_contextuel")
			  , iw->e, (void*)iw->ch) ;

      return(0) ;
    }



  if ( vue->nb )
    {
      u_annule_triplet(&v) ;
      u_vue_2d_3d(vue, iw->x, iw->y, &v, vue->gl.hau) ; /* Ajout du 09/01/2001 */

      /*
       * Traite les TAG en premier si c'est le bouton de gauche
       */
      if ( TRACE(vue) )
	{
	  eprintf("b=%d\n", bouton) ;
	  for(i=0;i<vue->nb;i++)
	    eprintf("objet=%p, sous-tag=%x\n",
		    vue->designes[i].objet
		    , vue->designes[i].sous_objet) ;
	}
      if ( bouton==0 || bouton==3 || bouton==4 )
	{
	  un_non_deplacable = Faux ;
	  un_sous_objet = Faux ;
	  for(i=0;i<vue->nb;i++)
	    {
	      if ( vue->designes[i].sous_objet != -1 )
		{
		  /*
		   * 24/12/2000 Ajoute le 1 || pour
		   * que les TAGS soient aussi gérés même si l'objet
		   * n'est pas actif
		   */
		  if ( 1 || ACTIF(vue->designes[i].objet) )
		    {
		      un_sous_objet = Vrai ;
		      if ( !TAG_DEPLACABLE(vue->designes[i].sous_objet) )
			un_non_deplacable = Vrai ;
		    }
		}
	    }
	  if ( TRACE(vue) )
	    {
	      eprintf("un_non_deplacable = %d\n", un_non_deplacable) ;
	    }
	  if ( 1/*22/10/2001*/ || un_non_deplacable )
	    {
	      for(i=0;i<vue->nb;i++)
		{
		  if ( vue->designes[i].sous_objet != -1 )
		    if ( 1/*22/10/2001*/
			 || !TAG_DEPLACABLE(vue->designes[i].sous_objet) )
		      {
			for(j=0;j<i;j++)
			  if ( vue->designes[i].objet==vue->designes[j].objet
			       && vue->designes[j].sous_objet != -1)
			    break ;
			if ( i==0 || j == i )
			  {
			    if ( bouton == 0 )
			      u_deplace_tag(vue, &vue->designes[i],
					    &v,
					    iw->x-vue->x__init,
					    iw->y-vue->y__init);
			    else
			      u_deplace_tag_profondeur(vue, &vue->designes[i],
						       bouton-3.5) ;
			    if ( TRACE(vue) )
			      eprintf("Déplace objet=%p, sous-tag=%x\n",
				      vue->designes[i].objet
				      , vue->designes[i].sous_objet) ;
			  }
		      }
		}
	      /*
	       * Le 14/01/2001 c'était return(0)
	       * Le but est de mélanger tag non déplacable et déplacable
	       */
	      /*
	       * J'enlève le return 22/10/2001
	       */
	      /* return(0) ; */
	    }
	  /*
	   * Ajoute if :  22/10/2001
	   */
	  if ( un_non_deplacable )
	    return(0) ;
	  if ( un_sous_objet )
	    return(1) ;
	  }
      /*
       * Si aucun TAG : active/inactive objet ou sous-objet
       */
      for(i=0;i<vue->nb;i++)
	{
	  if ( bouton==0 )
	    {
	      o_bascule_actif(vue->designes[i].objet) ;
	      evaluation(vue->designes[i].objet) ; /* A cause des OPERATEUR pour remonter le changement */
	    }
	  else
	    o_bascule_sous_actif( vue->designes[i].objet
				  , vue->designes[i].sous_objet ) ;
        }
      return(0) ;
    }
  else
    {
      if ( bouton == 3 || bouton == 4 )
	{
	  e_deplace(vue,  bouton+1) ;
	  return(0) ;
	}
      return(1) ;
    }
}






static void e_reinit_vue(Vue *o, int dire)
{
  static const struct
  {
    char *titre ;
    char *persp ;
    Triplet point_de_vue, point_vise ;
    Triplet haut ;
    Flottant focale ;
  } t[] =
    {
      {"Vue Perspective"  , "1:Persp", {4,4,4},	{0,0,0}, {0,0,1}, 1 },
      {"Vue X"            , "0:Ortho", {4,0,0},	{0,0,0}, {0,0,1}, 4 },
      {"Vue Y"            , "0:Ortho", {0,4,0},	{0,0,0}, {0,0,1}, 4 },
      {"Vue Z"            , "0:Ortho", {0,0,4},	{0,0,0}, {0,1,0}, 4 },
      {"Vue Hémisphérique", "2:Hémis", {0,0,0},	{0,0,1}, {1,0,0}, 1 },
      {"Vue Naturelle"    , "1:Persp", {0,-4,2},{0,0,2.0001}, {0,0,1}, 1 },
      {"Vue Persp. Mart." , "1:Persp", {4,-4,4},{0,0,0}, {0,0,1}, 1 },

    } ;
  
  c_sauve(o, "b_persp", t[dire].persp) ;
  o->observateur.point_de_vue = t[dire].point_de_vue ;
  o->observateur.haut = t[dire].haut ;
  o->observateur.focale = t[dire].focale ;
  o->observateur.point_vise = t[dire].point_vise ;
  c_sauve(o, "titre", t[dire].titre) ;

  if ( dire == 4 )
    {
      c_sauve(o, "geometrie", "0 0 1000 1082") ;
    }

  o->b_grille = Vrai ;
  o->b_repere = Vrai ;
}

#define TOUCHE(S) ( strncmp(S, v+2, strlen(S)) == 0 )

static void dump_image(Vue *o, const Champ *ch, const char *v) ;

static Booleen menu_observateur(const Interface_widget *iw)
{
  static const struct
  {
    void (*fct)(Vue*o,int dir) ;
    int dir ;
    char *arg ;
  } t[] =
    {
      { e_deplace ,0 }, { e_deplace ,1 }, { e_deplace ,2 },{ e_deplace ,3 },
      { e_rotation,0 }, { e_rotation,1 }, { e_rotation,2 },{ e_rotation,3 },
      { e_rotation,5 }, { e_rotation,4 }, { e_deplace ,4 },{ e_deplace ,5 },
      { e_rotation_origine ,0 }, { e_rotation_origine ,1 },
      { e_rotation_origine ,2 },{ e_rotation_origine ,3 },
      { e_reinit_vue, 0 },{ e_reinit_vue, 1 },
      { e_reinit_vue, 2 },{ e_reinit_vue, 3 },
      { e_reinit_vue, 4 },{ e_reinit_vue, 5 },{ e_reinit_vue, 6 },

      { (void (*)(Vue*,int))dump_image, -1, "1 2"},
      { (void (*)(Vue*,int))dump_image, -1, "1 1"},
      { (void (*)(Vue*,int))dump_image, -1, "2 2"},
      { (void (*)(Vue*,int))dump_image, -1, "2 1"},
      { (void (*)(Vue*,int))dump_image, -1, "3 1"},
      { (void (*)(Vue*,int))dump_image, -1, "4 4"},
      { (void (*)(Vue*,int))dump_image, -1, "4 2"},
      { (void (*)(Vue*,int))dump_image, -1, "4 1"},
      { (void (*)(Vue*,int))dump_image, -1, "8 4"},
      { (void (*)(Vue*,int))dump_image, -1, "8 2"},
      { (void (*)(Vue*,int))dump_image, -1, "16 2"},
    } ;

  Vue *o ;
  int i ;
  char *arg ;

  o = iw->objet ;
  i = t[T(Menu, o, iw->ch).index].dir ;
  arg = t[T(Menu, o, iw->ch).index].arg ;
   
  if ( i >= 0 )
    (*t[T(Menu, o, iw->ch).index].fct)(o, i) ;
  else
    {
      (*(void (*)(void*,const Champ*, const char*))t[T(Menu, o, iw->ch).index].fct)(o, iw->ch, arg) ;
    }
  return(0) ;
}

/* Numero tag contient 0 si le curseur n'est pas sur l'objet */
static void qui_veut_le_raccourci(void *o, char *v, Vue *vue)
{
  const Champ *ch ;
  const void *ff ;
  int i ;

  if ( o == NULL || v[0] == '\0' )
    return ;

  ff = o ;
  if ( c_trouve_type( &ff, &ch, &infos_L_raccourci ) )
    {
      ecrit_numero_tag(ch, 0) ;
      for(i=0; i<vue->nb; i++)
	if ( vue->designes[i].objet == ff )
	  {
	    ecrit_numero_tag(ch, 1) ;
	    break ;
	  }
      c_sauve_ici((void*)ff, ch, v) ;
    }

  POUR_LES_FILS(o, f, qui_veut_le_raccourci(f, v, vue) ; ) ;
}

static void clavier(const Interface_widget *iw)
{
  Vue *vue ;
  char *t ;
  const char *p, *v ;
  Triplet pt ;
  int x, y ;
  const Champ *ch ;
  const void *ff ;

  vue = iw->objet ;

  v = iw->texte ;
  if ( TRACE(vue) )
    eprintf("Touche = (%s)\n", v) ;

  /*
   * Création de la chaine de caractère à envoyer
   * avec toutes les informations
   */
  p = v + strlen(v) - 1 ;
  while( *p != ' ' ) p-- ;
  p-- ;
  while( *p != ' ' ) p-- ;
  sscanf(p, "%d%d", &x, &y) ;
  
  globale_etat_courant.gl = Gl_start(&vue->gl) ;
  u_vue_2d_3d(vue, x, y, &pt, vue->gl.hau) ;

  ALLOUER(t, strlen(v)+200) ;
  sprintf(t
	  , "%s %d %s!"
	  , v
	  , numero_mauvaise_coordonnee(vue)
	  , triplet_vers_char(&pt)
	  ) ;
  /*
   * Recherche de raccourci clavier
   */

  /* Pour gagner du temps on ne fait la sélection que
   * s'il y a un raccourci clavier
   */

  POUR(void*, f, fils_a_afficher(vue),
       if ( f == NULL )
       continue ;
       ff = f ;
       if ( c_trouve_type( &ff, &ch, &infos_L_raccourci ) )
  {
    vue->nb      = m_trouve_objet( vue, iw->x, iw->y, vue->designes) ;
    break ;
  }
       ) ;

  Gl_stop_sans_swapping(&vue->gl) ;

  if ( TRACE(vue) )
    eprintf("Cherche les raccourcis\n") ;
  POUR(void*, f, fils_a_afficher(vue), qui_veut_le_raccourci(f, t, vue) ) ;
  if ( TRACE(vue) )
    eprintf("raccourcis = %s\n", t) ;
  if ( t[0] == '\0' )
    return ;



  if ( v[1] == ' ' )
    {
      /*
       * Pas une touche standard : 123..abc..AB..,;
       * Donc F1, Left, ...
       * On intercepte seulement celle qui nous intéressent
       * Le reste remonte au modeleur.
       */
      switch( v[0] )
        {
        case '0':
          if ( TOUCHE("Right") ) e_deplace(vue, 0) ; else
            if ( TOUCHE("Left") ) e_deplace(vue, 1) ; else
              if ( TOUCHE("Up") ) e_deplace(vue, 2) ; else      
                if ( TOUCHE("Down") ) e_deplace(vue, 3) ; else
                  break ;
          return ;
        case '1':
          if ( TOUCHE("Right") ) e_rotation(vue, 0) ; else
            if ( TOUCHE("Left") ) e_rotation(vue, 1) ; else
              if ( TOUCHE("Up") ) e_rotation(vue, 2) ; else     
                if ( TOUCHE("Down") ) e_rotation(vue, 3) ; else
                  break ;
          return ;
        case '2':
          if ( TOUCHE("Right") ) e_rotation(vue, 5) ; else
            if ( TOUCHE("Left") ) e_rotation(vue, 4) ; else
              if ( TOUCHE("Up") ) e_deplace(vue, 4) ; else      
                if ( TOUCHE("Down") ) e_deplace(vue, 5) ; else
                  break ;
          return ;
        case '3':
          if ( TOUCHE("Right") ) e_rotation_origine(vue, 0) ; else
            if ( TOUCHE("Left") ) e_rotation_origine(vue, 1) ; else
              if ( TOUCHE("Up") ) e_rotation_origine(vue, 2) ; else     
                if ( TOUCHE("Down") ) e_rotation_origine(vue, 3) ; else
                  break ;
          return ;
        }
      c_sauve( FILS(u_racine(vue),0), "u_ajoute_touche", v) ;
      return ;
    }


  /*
   * Trouve l'objet à qui filer la chaine de caractères
   */
  c_sauve( u_operateur_ou_modele(vue), "u_ajoute_touche", t) ;

  free(t) ;
}

/*
 *
 */

static const char * vue_aide(const Interface_widget *iw)
{
  static Chaine s ;
  int i, j ;
  void *o ;
  const char *v ;
  const Champ *ch ;
  Vue *vue ;

  vue = iw->objet ;
  if ( vue->bouton_2_et_3_deviennent_1 )
    return( "" ) ;


  globale_etat_courant.gl = Gl_start(&vue->gl) ;


  s_efface(&s) ;

  vue->nb = m_trouve_objet( vue, iw->x, iw->y, vue->designes) ;

  if ( TRACE(vue) )
    eprintf("Début aide sur vue nb=%d\n", vue->nb) ;
  if ( vue->nb )
    for(i=0;i<vue->nb;i++)
      {
	o = vue->designes[i].objet ;
	for(j=i+1;j<vue->nb;j++)
	  if ( o == vue->designes[j].objet )
	    break ;
	if ( j == vue->nb )
	  {
	    s_printf(&s,"----------- %s ------------\n",
		     OBJET(o)->classe->nom) ;
	    POUR_LES_CHAMPS_CLASSE
	      (o, ch,
	       {
		 if ( AFFICHE(ch) )
		   {
		     v = c_recupere_ici_sans_copie(o, ch) ;
		     if ( v[0] )
		       s_printf(&s, "%17s ====> %s\n", ch->nom_champ, v) ;
		   }
	       }
	       ) ;
	    for(j=0;j<=i;j++)
	      if ( o == vue->designes[j].objet
		   && vue->designes[j].sous_objet!=-1 )
		{
		  ch = trouve_tag(o, vue->designes[j].sous_objet) ;
		  s_printf(&s, "TAG 0x%x = %s\n",
			   vue->designes[j].sous_objet,
			   ch ? c_recupere_ici_sans_copie(o, ch) : "Pas de valeur"
			   ) ;
		}
	  }
      }
  else
    {
      if ( TRACE(vue) )
	eprintf("u_aide_champ\n") ;
      s_strcat(&s, u_aide_champ(iw->objet, iw->ch)) ;
    }

#ifndef CLK_TCK
#define CLK_TCK 100
#endif

  s_printf(&s,
	   "\nTemps mis pour le dernier affichage = %g seconde\n"
	   "Soit %g images par seconde.\n"
	   ,vue->temps_en_clockticks/(float)CLK_TCK
	   ,CLK_TCK/(float)vue->temps_en_clockticks
	   ) ;

  s_printf(&s, "GL Vendor = %s\n", glGetString(GL_VENDOR)) ;
  s_printf(&s, "GL Renderer = %s\n", glGetString(GL_RENDERER)) ;
  s_printf(&s, "GL Version = %s\n", glGetString(GL_VERSION)) ;

  if ( TRACE(vue) )
    eprintf("Fin aide sur vue\n") ;

  /*  s_printf(&s, "\nFenêtre GL %d/%d = %g\n", vue->gl.lar, vue->gl.hau, vue->ratio); */

  return(s_char(&s)) ;
}

/*
 *
 */

struct observateur
{
  t_observateur observateur ;
} ;

CLASSE(observateur, struct observateur,
       MENU("Divers/Observateur")
       BULLE_D_AIDE(
		    "Les champs ont la même signification\n"
		    "que dans l'objet `vue'.\n"
		    "\n"
		    "En attendant l'interface graphique,\n"
		    "vous pouvez utilisez les touche 1,2,3,4\n"
		    "pour déplacer interactivement l'observateur\n"
		    "dans l'espace.\n"
		    "\n"
		    "Vous pouvez paramétrer l'observateur par une\n"
		    "vue, dans ce cas le déplacement de l'observateur\n"
		    "change l'observateur de la vue!.\n"
		    )
       EVENEMENT("OB")
       CHAMP(observateur, NOM("camera") L_observateur)
       Edite Sauve Extrait Init3d
       CHAMP(observateur.point_de_vue, L_point3   DEFAUT("1 0 0"))
       CHAMP(observateur.point_vise  , L_point3   DEFAUT("0 0 0"))
       CHAMP(observateur.haut        , L_point3   DEFAUT("0 0 1"))
       CHAMP(observateur.focale      , L_flottant DEFAUT("1"))
       CHAMP(observateur.ratio       , L_flottant DEFAUT("1"))
)

struct observateur_repere
{
  t_observateur observateur ;
  Transfo t ;
} ;

static void changement_observateur_repere(struct observateur_repere *o)
{
  Triplet pt ;
  /*
  if ( CREATION(o) )
    return ;
  */
  if ( FILS_CHANGE(o) )
    {
      o->observateur.point_de_vue = o->t.origine                              ;
      o->observateur.point_vise   = u_ajoute_triplet(&o->t.origine, &o->t.v1) ;
      o->observateur.haut         = o->t.v3                                   ;
      o->observateur.focale       = u_norme_triplet(&o->t.v2)
	/ u_norme_triplet(&o->t.v1)     ;
    }
  else
    if ( CHAMP_CHANGE(o, observateur) )
      {
	o->t.origine = o->observateur.point_de_vue ;
	o->t.v1 = u_soustrait_triplet(&o->observateur.point_vise,
				      &o->observateur.point_de_vue) ;
	o->t.v3 = o->observateur.haut ;
	pt = u_pv_triplet(&o->t.v3, &o->t.v1) ;
	pt = u_normalise_triplet(&pt) ;
	o->t.v2 = u_triplet_fois_flottant(&pt,
					  o->observateur.focale
					  *u_norme_triplet(&o->t.v1)) ;
      }
}

CLASSE(observateur_repere, struct observateur_repere,
       SUR_CLASSE("observateur") HERITE_PAS_DES_CHAMPS
       BULLE_D_AIDE(
		    "Transformation d'un repère en un observateur\n"
		    )
       CHAMP(observateur, NOM("camera") L_observateur Edite TE_HAUTEUR(100))
       CHAMP(t, L_transfo Extrait Edite Obligatoire)
       CHANGEMENT(changement_observateur_repere)
)

/*
 *
 */
     /*
static void offscreen(int quoi)
{
  static OSMesaContext osmc ;

  if ( quoi == 0 )
    {
      osmc = OSMesaCreateContext(OSMESA_RGBA, NULL) ;
      OSMesaMakeCurrent(osmc, 
    }
}
*/


#if 0
#define EPRINTF eprintf
#else
#define EPRINTF if(0) eprintf
#endif

static void dump_image_reel(Vue *o, int zoomin, int zoomout,FILE *f)
{
  unsigned char *image, *p ;
  Interface_widget iw ;
  int i, j, x, y, r, v, b, largeur, decalage, nouveau_decalage ;
  int nb_mosaic_a_faire, nb_mosaic_faits, temps_restant ;
  time_t temps_demarrage ;
  float  temps_moyen ;
  int lar, hau ;

  /* bug driver GL ? */
  /*
  Gl_start(&o->gl) ;
  Gl_stop(&o->gl) ;
  */

  o->mosaicf = zoomin ;
  largeur = o->mosaicf * o->gl.lar * 3 ;
  ALLOUER(image, largeur*(o->gl.hau+zoomout)) ;
  glPixelStorei(GL_PACK_ALIGNMENT, 1) ;
  glPixelStorei(GL_PACK_ROW_LENGTH, o->mosaicf*o->gl.lar) ;
  iw.objet = o ;

  glPushAttrib(GL_LINE_BIT|GL_POINT_BIT) ;
  u_multiplie_epaisseur_ligne(o->mosaicf) ;
  u_multiplie_taille_point(o->mosaicf) ;

  if ( o->b_persp.index == 2 ) // Hémisphère
    {
      lar = hau = taille_hemi(o) ;
    }
  else
    {
      lar = o->gl.lar ;
      hau = o->gl.hau ;
    }


  fprintf(f, "P6\n%d %d\n255\n"
	  , (o->mosaicf*lar)/zoomout
	  , (o->mosaicf*hau)/zoomout
	  ) ;
  nb_mosaic_a_faire = o->mosaicf * o->mosaicf ;
  nb_mosaic_faits = 0 ;
  time(&temps_demarrage) ;

  EPRINTF("hauteur image generee=%d hauteur fenetre %d\n",
	  (o->mosaicf*hau)/zoomout, hau) ;

  decalage = 0 ;
  for(o->mosaicy = o->mosaicf-1; o->mosaicy>=0 ; o->mosaicy--)
    {
      for(o->mosaicx = 0 ; o->mosaicx<o->mosaicf; o->mosaicx++)
	{
	  if ( zoomin != 1 )
	    {
	      o->double_buffer = 0 ;
	      u_vue_trace(&iw) ;
	      glFlush() ;
	      o->double_buffer = 1 ;
	      glReadBuffer(GL_FRONT) ;
	    }
	  glReadPixels(0, 0, lar, hau, GL_RGB, GL_UNSIGNED_BYTE,
		       image + o->mosaicx*3*lar
		       ) ;
	  nb_mosaic_faits++ ;
	  if ( time(0) - temps_demarrage > 1 )
	    {
	      temps_moyen = (time(0) - temps_demarrage)/(float)nb_mosaic_faits ;
	      temps_restant = temps_moyen
		* (nb_mosaic_a_faire - nb_mosaic_faits) ;
	      fprintf(stderr
		      , "Temps de calcul APPROXIMATIF restant %4dh%02dm%02ds\r"
		      , temps_restant/3600
		      , (temps_restant%3600)/60
		      , temps_restant%60
		      ) ;
	      fflush(stdout) ;
	      if ( nb_mosaic_a_faire == nb_mosaic_faits )
		printf("\n") ;
	    }
	}

      if(0)
	{
	  FILE *g ;
	  g = popen("xv -", "w") ;
	  fprintf(g, "P6\n%d %d\n255\n"
		  , (o->mosaicf*lar)
		  , hau
		  ) ;
	  fwrite(image,hau, 3*o->mosaicf*lar, g) ;
	  pclose(g) ;	
	}
      
      nouveau_decalage = (hau + decalage)%zoomout ;
      EPRINTF("Y=%d decalage=%d nouveau+decalage=%d zoomout=%d\n"
	      , o->mosaicy, decalage, nouveau_decalage, zoomout);

      for(j=hau+decalage-zoomout; j>=nouveau_decalage; j -= zoomout)
	for(i=0; i<=lar*o->mosaicf-zoomout; i += zoomout)
	  {
	    p = image + 3*i + j*largeur ;
	    if ( zoomout != 1 )
	      {
		r = v = b = 0 ;
		for(y=0; y<zoomout; y++)
		  {
		    for(x=0; x<zoomout; x++)
		      {
			r += *p++ ;
			v += *p++ ;
			b += *p++ ;
		      }
		    p += -zoomout*3 + largeur ;
		  }
		putc(r/zoomout/zoomout, f) ;
		putc(v/zoomout/zoomout, f) ;
		putc(b/zoomout/zoomout, f) ;
	      }
	    else
	      {
		putc(*p++, f) ;
		putc(*p++, f) ;
		putc(*p++, f) ;
	      }
	  }
      if ( ((hau+decalage-zoomout-nouveau_decalage)%zoomout) != 0 )
	ICI ;
      EPRINTF("genere %d lignes\n"
	      , (hau+decalage-zoomout-nouveau_decalage)/zoomout) ;
      memcpy(image + hau*largeur, image, nouveau_decalage*largeur) ;
      decalage = nouveau_decalage ;
    }

  glPopAttrib() ;

  o->mosaicf = 0 ;
  free(image) ;
  glPixelStorei(GL_PACK_ROW_LENGTH, 0) ;
}

#undef EPRINTF

void calcul_nom_fichier(Vue *o, NomFichier fichier)
  {
    int i ;
    char *nom ;

    nom = my_basename((char*)c_recupere_sans_copie(u_modele(o),"nom_modele")) ;
    if ( isdigit(nom[0])  && isdigit(nom[1]) && isdigit(nom[2])
	 &&  isdigit(nom[3]) && isdigit(nom[4]) )
      sprintf(fichier, "%s.ppm", o->titre) ;
    else
      sprintf(fichier, "%s_%s.ppm", nom, o->titre) ;
    for(i=0; fichier[i]; i++)
      if ( fichier[i]==' ' )
	fichier[i] = '_' ;
  }

static void dump_image(Vue *o, const Champ *ch, const char *vv)
{
  int zoomin, zoomout ;
  NomFichier fichier, commande ;
  FILE *f ;
  int pour_icone ;

  pour_icone = ( strncmp(vv, "icone ", 6) == 0 ) ;
  if ( pour_icone )
    {
      vv += 6 ;
      zoomin = 1 ;
      zoomout = o->gl.lar / atol(vv) ;
      vv = strchr(vv, ' ') ;
      if ( vv == NULL )
	return ;

      sprintf(commande, "ppmnorm >%s 2>/dev/null", vv+1) ;
      f = popen(commande, "w") ;
    }
  else
    {
      int i ;
      calcul_nom_fichier(o, fichier) ;
      i = strlen(fichier) ;
      strcpy(fichier+i-4, ".png") ;
      
      sprintf(commande, "pnmtopng >%s", fichier) ;
      f = popen(commande, "w") ;
      if ( f==NULL )
	{
	  perror(fichier) ;
	  return ;
	}
      sscanf(vv, "%d%d", &zoomin, &zoomout) ;
      if ( zoomin < 1 )
	zoomin = 1 ;
      if ( zoomout < 1 )
	zoomout = 1 ;
      o->zoomin = zoomin ;
      o->zoomout = zoomout ;
    }

  if ( OBJET(u_modele(o))->edite && OBJET(o)->edite && !pour_icone )
    {
      widget_raise(OBJET(o)->edite) ;
    }


  dump_image_reel(o, zoomin, zoomout, f) ;
  pclose(f) ;
  if ( !pour_icone && !((Interface*)u_racine(o))->http_ecrit )
    {
      /*
       * On n'affiche que dans le cas interactif
       */
      sprintf(commande, "xv '%s' || /usr/liris/bin/xv '%s' || display '%s' || gthumb '%s' &", fichier, fichier, fichier, fichier) ;
      system(commande) ;
    }
}

static void print_image(Vue *o, const Champ *ch, const char *vv)
{
  int zoomin, zoomout ;
  FILE *f ;
  NomFichier commande ;
  int x, y, l, h ;

  zoomin = zoomout = 1 ;
  sscanf(vv, "%d%d", &zoomin, &zoomout) ;

  x = 0 ;
  y = 0 ;
  l = (zoomin*o->gl.lar)/zoomout ;
  h = (zoomin*o->gl.hau)/zoomout ;
  sscanf(vv, "%*d%*d%d%d%d%d", &x, &y, &l, &h) ;

  sprintf(commande, "print_image %d %d %d %d", x, y, l, h) ;
  f = popen(commande, "w") ;
  dump_image_reel(o, zoomin, zoomout, f) ;
  pclose(f) ;
}


CLASSE(vue, Vue,
       MENU("Divers/Vue")
       EVENEMENT("0v0u")
       AFFICHE_CHAMP_CLASSE(Faux)
       CHAMP(observateur , L_observateur Extrait /*OPTION(O_Edite)*/) /* 2/2/2000 Met Extrait */
       Edite Sauve
       CHAMP(titre    , L_titre InitChaine DEFAUT("vue_utilisateur"))
       DEBUT_BOITE(HORIZONTALE BoiteHomogene)
       CHAMP(double_buffer, LABEL("DblBuf")  L_booleen DEFAUT("1")
	     BULLE_D_AIDE("Active ou non le double buffering")
	     )
       CHAMP(pas_vue     , LABEL("Pas")    L_flottant DEFAUT(".1")
	     TE_LARGEUR(30)
             BULLE_D_AIDE
             ("Indique la distance que vous parcourez lorsque\n"
              "l'observateur se déplace.\n"
              )
             )
       CHAMP(angle_vue   , LABEL("DØ")     L_flottant DEFAUT(".1")
	     TE_LARGEUR(30)
             BULLE_D_AIDE
             ("Indique le changement d'angle de vue (en radian)\n"
              "lorsque l'observateur tourne ou penche la tête.\n"
              )
             )
       CHAMP(observateur.focale  , NOM("taille_vue") LABEL("Focale") L_flottant DEFAUT("1")
	     TE_LARGEUR(30)
             BULLE_D_AIDE
             ("Cette valeur représente l'angulaire\n"
              "si votre vue est avec perspective.\n"
              "Dans le dessin suivant : `o' est l'oeil\n"
              "La focale est égale à b/a\n"
              "\n"
              "         /| ^    \n"
              "        / | |    \n"
              "       /  | |    \n"
              "      /   | |    \n"
              "     /    | |b   \n"
              "    /     | |    \n"
              "   /      | |    \n"
              "  /       | |    \n"
              " o--------+ v    \n"
              "  <------->      \n"
              "      a\n"
              "\n"
	      "Sans perspective, c'est la hauteur de ce qui est\n"
	      "vu sur l'écran\n"
              )
             )
       CHAMP(dts         , LABEL("Pix")    L_flottant DEFAUT("6")
	     TE_LARGEUR(20)
             BULLE_D_AIDE
             ("Lorsque vous cliquez dans la vue, les objets qui\n"
              "se trouvent à une distance pixel inférieure à celle\n"
              "indiquée voit leur sélection changer d'état.\n"
              )
             )
       CHAMP(zmin         , LABEL("Zmin")    L_flottant DEFAUT("0.1")
	     TE_LARGEUR(20)
             BULLE_D_AIDE
             ("Si les objets sont plus près, ils sont coupés")
             )
       CHAMP(zmax         , LABEL("Zmax")    L_flottant DEFAUT("1000")
	     TE_LARGEUR(20)
             BULLE_D_AIDE
             ("Si les objets sont plus loin, ils sont coupés")
             )
       FIN_BOITE
       DEBUT_BOITE(HORIZONTALE BoiteHomogene)
       NonLabel
       CHAMP(observateur.point_de_vue, NOM("point_de_vue")                L_point3   DEFAUT("1 1 1") LABEL("Observateur") Edite
             BULLE_D_AIDE
             ("La position de l'observateur.\n")
             )
       CHAMP(observateur.point_vise  , NOM("point_vise")                 L_point3   DEFAUT("0 0 0") LABEL("Point vise") Edite
             BULLE_D_AIDE
             ("Le point regardé (ou point visé).\n")
             )
       CHAMP(observateur.haut        , NOM("haut")                L_point3   DEFAUT("0 0 1") LABEL("Haut") Edite
             BULLE_D_AIDE
             ("Le vecteur indique le haut pour l'observateur.\n"
              "Pour avoir l'axe Z vers le haut : (0 0 1)\n")
             )
       CHAMP(couleur_fond, L_couleur P_triplet DEFAUT("0 0 0") Edite Sauve LABEL("Couleur de fond"))
       FIN_BOITE
       Label
       /*
       CHAMP(filtre      , NonLabel        L_chaine   DEFAUT("*")
             BULLE_D_AIDE
             ("Non utilisé pour l'instant : Filtre/calque\n")
              )
       */
       DEBUT_BOITE(HORIZONTALE BoiteHomogene NOM("vue_ligne"))
       CHAMP(b_persp     , L_option_menu DEFAUT("1:Persp")
	     NonLabel
	     TE_TEXTE("0:Ortho\n"
		      "1:Persp\n"
		      "2:Hémis\n")
             BULLE_D_AIDE
             ("Choix entre la projection perspective, orthogonale et hémisphérique (dessus + 4 demis cotés).\n")
             )
       CHAMP(b_entier    , LABEL("Entier") L_booleen  DEFAUT("0")
             BULLE_D_AIDE
             ("Si actif, alors les coordonnées saisies sont des\n"
              "valeur entières en non flottantes.\n")
             )
       CHAMP(b_grille    , LABEL("Grille") L_booleen  DEFAUT("1")
             BULLE_D_AIDE
             ("Faire apparaître ou non la grille\n")
             )
       CHAMP(b_repere    , LABEL("Repère") L_booleen  DEFAUT("1")
             BULLE_D_AIDE
             ("Faire apparaître ou non le repère\n")
             )
       CHAMP(b_local     , LABEL("Local")  L_booleen  DEFAUT("0")
             BULLE_D_AIDE
             ("Changement d'algo d'affichage de GL")
             )
       CHAMP(b_stereo     , LABEL("Stéréo")  L_booleen  DEFAUT("0")
             BULLE_D_AIDE
             ("Affichage stéréo")
             )
       CHAMP(menu_qualite , L_option_menu
	     BULLE_D_AIDE("Qualité d'affichage dans cette vue")
	     LABEL("Qualité")
             TE_TEXTE(
		      "2: Inacceptable\n"
		      "3: Très mauvaise\n"
		      "4: Mauvaise\n"
		      "5: Normale\n"
		      "6: Bonne\n"
		      "7: Très bonne\n"
		      "8: Parfaite\n"
                      )
	     DEFAUT("5: Normale")
	     NonLabel
             )
       CHAMP(menu_contextuel , L_commande_menu(menu_contextuel) NonCree
             TE_TEXTE("Editer\n"
		      "Regarder surface\n"
		      "Regarder dedans\n"
		      "Enleve DETRUIT\n"
		      "Detruire\n"
		      "Duplique\n"
		      "Aide\n"
		      )
       )
       CHAMP(menu_observateur , L_commande_menu(menu_observateur)
	     LABEL("Observ")
             /*
              * Ne pas changer l'ordre du menu
              */
             TE_TEXTE(
                      "va à droite                 <Right>\n"
                      "va à gauche                 <Left>\n"
                      "va en haut                  <Up>\n"
                      "va en bas                   <Down>\n"
                      "regarde à droite            Shift<Right>\n"
                      "regarde à gauche            Shift<Left>\n"
                      "regarde en haut             Shift<Up>\n"
                      "regarde en bas              Shift<Down>\n"
                      "penche à droite             Control<Right>\n"
                      "penche à gauche             Control<Left>\n"
                      "avance                      Control<Up>\n"
                      "recule                      Control<Down>\n"
                      "va à droite du point visé   Shift-Control<Right>\n"
                      "va à gauche du point visé   Shift-Control<Left>\n"
                      "va au dessus du point visé  Shift-Control<Up>\n"
                      "va au dessous du point visé Shift-Control<Down>\n"
		      "Devient vue Perspective\n"
		      "Devient vue X\n"
		      "Devient vue Y\n"
		      "Devient vue Z\n"
		      "Devient vue Hémisphérique\n"
		      "Devient vue Naturelle\n"
		      "Devient vue Persp. Mart.\n"
		      "Dump image fois 1 div 2\n"
		      "Dump image fois 1 div 1\n"
		      "Dump image fois 2 div 2\n"
		      "Dump image fois 2 div 1\n"
		      "Dump image fois 3 div 1\n"
		      "Dump image fois 4 div 4\n"
		      "Dump image fois 4 div 2\n"
		      "Dump image fois 4 div 1\n"
		      "Dump image fois 8 div 4\n"
		      "Dump image fois 8 div 2\n"
		      "Dump image fois 16 div 2\n"
                      )
             )
       CHAMP(b_local  , LABEL("Applique") NOM("ooook") L_bouton(applique_vue) 
             BULLE_D_AIDE
             ("Appuyez ici pour valider les modifications que vous\n"
              "avez fait sur les champs.\n"
	      )
             )
       FIN_BOITE
       CHAMP(gl,
	     L_gl
	     TE_ENTREE(u_vue_entree)
	     TE_ECRIT(u_vue_trace)
	     TE_MOUVEMENT(u_vue_mouvement)
             TE_CLAVIER(clavier) 
	     TE_AIDE(vue_aide)
       BULLE_D_AIDE("Interface 3D OpenGL pour NanoÉdit.\n"
                    "S'il n'a pas de fils, le contenu du modèle est\n"
                    "affiché sinon seul les fils le sont.\n"
                    "\n"
                    "On clique avec le bouton de gauche dans\n"
                    "la zone graphique pour activer/inactiver\n"
                    "les objets graphiques.\n"
		    "Si l'on clique et déplace :\n"
		    "  - Sur rien : rotation ou translation de la vue.\n"
		    "  - Sur un tag : déplace le tag.\n"
		    "\n"
		    "Le bouton du milieu permet d'activer/inactiver\n"
		    "un sous-objet d'un objet complexe.\n"
		    "\n"
		    "Le bouton de droite fait apparaître un menu permettant\n"
		    "de manipuler l'ensemble des objets pointés.\n"
                    )
	     )
       CHAMP_VIRTUEL( L_prevenir_si_modele_change(me_prevenir))
       CHAMP(bouton_2_et_3_deviennent_1, L_booleen NonAffiche DEFAUT("0"))
       CHAMP(deplacement_observateur_a_la_souris, L_booleen NonAffiche DEFAUT("1"))

       CHANGEMENT(changement_vue)
       DESTRUCTION(destruction_vue)
       PAQUET(NB_ACTIFS)
       CHAMP(geometrie   , L_geometrie Cree /* Modifiable */ Sauve DEFAUT("0 0 400 400"))
       //CHAMP(couleur_fond, L_couleur P_triplet DEFAUT("0 0 0") NonAffiche Sauve)
       CHAMP_VIRTUEL(NOM("dump_image") L_fonction(dump_image))
       CHAMP_VIRTUEL(NOM("print_image") L_fonction(print_image))

       ajoute_crochet_changement(destruction_liste_objet) ;
       ajoute_crochet_destruction(destruction_liste_objet) ;
       )

static void affiche_vue_incluse(Vue_incluse *o, const Champ *c
				, const char *qualite)
{
  Triplet pt[4] ;

  if ( o->xmax == 0 || o->ymax == 0 )
    return ;

  o->vue.ratio = (o->xmax-o->xmin)/(o->ymax-o->ymin) ;

  if ( globale_etat_courant.designable )
      {
	if ( !o->affiche_contenu )
	  {
	    if ( TRACE(o) )
	      eprintf("Affiche le cadre de Vue incluse\n") ;

	    u_2d_3d(o->xmin, o->ymin, &pt[0]) ;
	    u_2d_3d(o->xmax, o->ymin, &pt[1]) ;
	    u_2d_3d(o->xmax, o->ymax, &pt[2]) ;
	    u_2d_3d(o->xmin, o->ymax, &pt[3]) ;
	    
	    glBegin(GL_QUADS) ;
	    t_triplet(&pt[0]) ;
	    t_triplet(&pt[1]) ;
	    t_triplet(&pt[2]) ;
	    t_triplet(&pt[3]) ;
	    glEnd() ;
	  }
	else
	  {
	    if ( TRACE(o) )
	      eprintf("Affiche en selection Vue incluse\n") ;
	    affiche_gl_qualite((Vue*)o, fils_a_afficher(o)) ;
	  }
      }
    else
      {
	if ( TRACE(o) )
	  eprintf("Affiche Vue incluse\n") ;
	glPushAttrib(GL_VIEWPORT_BIT) ;

	glViewport(o->xmin, o->ymin, o->xmax-o->xmin, o->ymax-o->ymin) ;
	
	glMatrixMode(GL_PROJECTION) ;
	glPushMatrix() ;
	glMatrixMode(GL_MODELVIEW) ;
	glPushMatrix() ;
	
	if ( o->efface )
	  {
	    glDepthFunc(GL_ALWAYS) ;
	    glDepthRange(1,1) ;
	    glColor3f(o->vue.couleur_fond.x
		      ,o->vue.couleur_fond.y
		      ,o->vue.couleur_fond.z
		      ) ;
	    glBegin(GL_QUADS) ;
	    glVertex2f(-1,-1) ;
	    glVertex2f(1,-1) ;
	    glVertex2f(1,1) ;
	    glVertex2f(-1,1) ;
	    glEnd() ;
	    glDepthRange(-1,1) ;
	    glDepthFunc(GL_LESS) ;
	  }
	
	u_vue_trace_reel(&o->vue) ;
	
	glPopMatrix() ;
	glMatrixMode(GL_PROJECTION) ;
	glPopMatrix() ;
	glMatrixMode(GL_MODELVIEW) ;

	glPopAttrib() ;
      }
}



CLASSE(vue_incluse, Vue_incluse,
       DEBUT_BOITE(HORIZONTALE BoiteHomogene)
       CHAMP(xmin,L_nombre P_entier Edite Sauve Init3d	DEFAUT("100"))
       CHAMP(xmax,L_nombre P_entier Edite Sauve Init3d	DEFAUT("200"))
       CHAMP(ymin,L_nombre P_entier Edite Sauve Init3d	DEFAUT("100"))
       CHAMP(ymax,L_nombre P_entier Edite Sauve Init3d	DEFAUT("200"))
       CHAMP(est_designable, L_booleen Edite Sauve DEFAUT("1"))
       CHAMP(efface, L_booleen DEFAUT("0"))
       FIN_BOITE
       SUR_CLASSE("vue")
       MENU("Divers/Vue Incluse")
       EVENEMENT("VI")
       CHAMP_VIRTUEL(L_affiche_gl(affiche_vue_incluse)
		     AFFICHAGE(Toujours_different))

       CHAMP_VIRTUEL(Obligatoire L_void)
       )



typedef struct viewport
{
  Triplet centre ;
  Triplet taille ;
  Booleen efface ;
} Viewport ;

/*
static void viewport_tags(Viewport *o, Triplet *centre, Triplet *coin)
{
  GLint vp[4] ;

  glGetIntegerv(GL_VIEWPORT,vp) ;

  u_2d_3d(o->centre.x*vp[2], o->centre.y*vp[3], centre) ;
  u_2d_3d(  (o->centre.x+o->taille.x/2)*vp[2]
	  , (o->centre.y+o->taille.y/2)*vp[3]
	    , coin) ;
}
*/

static void affiche_viewport(Viewport *o, const Champ *c
				  , const char *qualite)
{
  GLint vp[4] ;

  glGetIntegerv(GL_VIEWPORT,vp) ;

  glEnable(GL_SCISSOR_TEST) ;
  glScissor((o->centre.x-o->taille.x/2)*vp[2]
	    , (o->centre.y-o->taille.y/2)*vp[3]
	    , o->taille.x * vp[2]
	    , o->taille.y * vp[3]
	    ) ;
  if ( o->efface )
    {
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) ;
    }
  POUR_LES_FILS(o, f, u_affiche_objet(f,qualite) ;) ;
  glDisable(GL_SCISSOR_TEST) ;
}

static void ecrit_tag_vp(Viewport *o,const Champ *c, const char *v)
{
  GLint vp[4] ;
  Triplet pt ;
  Flottant x, y ;

  glGetIntegerv(GL_VIEWPORT,vp) ;
  pt = char_vers_triplet(v) ;
  u_3d_2d(&pt, &x, &y) ;
  o->centre.x = x/vp[2] ;
  o->centre.y = y/vp[3] ;
}

static const char* lit_tag_vp(Viewport *o,const Champ *c, const char *v)
{
  static Chaine s ;
  
  s_efface(&s) ;
  s_printf(&s, "%g %g", o->centre.x, o->centre.y) ;
  return( s_char(&s) ) ;
}




CLASSE(viewport, Viewport,
       CHAMP(centre, L_point P_triplet Edite Sauve DEFAUT("0.5 0.5"))
       CHAMP(taille, L_point P_triplet Edite Sauve DEFAUT("0.2 0.2"))
       CHAMP(efface, L_booleen DEFAUT("1")
	     BULLE_D_AIDE("Efface la fenêtre avant d'afficher")
	     )
       CHAMP_VIRTUEL(Init3d L_tag(ecrit_tag_vp, lit_tag_vp))
       CHAMP_VIRTUEL(L_affiche_gl(affiche_viewport)
		     AFFICHAGE(Toujours_different))
       PAQUET(NB_ACTIFS)       
       EVENEMENT("VP")
       MENU("Divers/Viewport (clipping)")
       )

