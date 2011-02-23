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
#include <errno.h>		/* Pour IRIX */
#include <GL/gl.h>		/* Car il affiche du GL */
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

#include "u_xt_interface.h"
#include "t_image.h"
#include "t_geometrie.h"
#include "u_fichier.h"
#include "u_graphique.h"
#include "u_etat_courant.h"

#define CIV 1

#if CIV
#define CHAMP_IMAGE_VIDEO CHAMP(imag, NOM("gl") L_image_l3_unsigned_char /* P_entier */ E_image TE_HAUTEUR(256) Affiche DEFAUT("[(1)]") )
#else
#define CHAMP_IMAGE_VIDEO
#endif

#define NR_TEXTURES 1000
#define NR_WINDOW 9
static int textures[NR_TEXTURES][NR_WINDOW] = {{1},{1}} ; // 0 si texture libre

int texture_new()
{
  int i ;
  for(i=2;i<NR_TEXTURES;i++)
    if ( textures[i][0] == 0 )
      {
	textures[i][0] = globale_etat_courant.gl ;
	// printf("texture_new ==> %d\n", i) ;
	return i ;
      }
  abort() ;
}

void texture_free(int *i)
{
  int j ; 

  if ( *i )
    {
      for(j=0; j<NR_WINDOW; j++)
	textures[*i][j] = 0 ;
      *i = 0 ;
    }
}

int must_create_texture(int *bindtexture)
{
  int i ;

  if ( *bindtexture == 0 )
    {
      *bindtexture = texture_new() ;
      glBindTexture(GL_TEXTURE_2D, *bindtexture);
      //      printf("MUST CREATE ==> %d (%d)\n", *bindtexture, globale_etat_courant.gl) ;
      return 1 ;
    }
  glBindTexture(GL_TEXTURE_2D, *bindtexture);
  //  printf("BIND TO ==> %d (%d)\n", *bindtexture, globale_etat_courant.gl) ;
  return 0;

  for(i=0; i<NR_WINDOW; i++)
    if ( textures[*bindtexture][i] == globale_etat_courant.gl )
      {
	printf("Yet CREATED ==> %d (%ld)\n", *bindtexture, globale_etat_courant.gl) ;
	return 0 ;
      }
  for(i=0; i<NR_WINDOW; i++)
    if ( textures[*bindtexture][i] == 0 )
      {
	textures[*bindtexture][i] = globale_etat_courant.gl ;
	printf("CREATE IN THIS WINDOW ==> %d (%ld)\n", *bindtexture, globale_etat_courant.gl) ;
	return 1 ;
      }
  abort() ;
}


typedef struct
{
  Image imag ; /* Doit rester en premier champ */
  Booleen eclairage ;
  Flottant ratio ;
  int f ;
  int lar, hau ; /* Ceux désiré par l'utilisateur */
  char *nom_device ;
  Image vignette ;
  int bindtexture ;
} Imag ;

/*
 * Affichage 3D de l'image
 */
static void affiche_image(Imag *im , const Champ *ch, const char *v)
{
  Image *o = &im->imag ;
  Image *vig = &im->vignette ;
  GLint render ;
  int qualite ;

  if ( LAR(o) == 0 )
    return ;

  qualite = atol(v) ;

  glGetIntegerv(GL_RENDER_MODE, &render) ;

  if ( render == GL_RENDER && qualite >= 1 )
    {
      /*
       * Les parametres seront-t-ils a sauvegarder ? (etat courant
       * de texture)
       */
      if ( must_create_texture(&im->bindtexture) )
	{
	  if ( 1 || qualite >= 4 )
	    {
	      if ( o->codec == codec_HL_l4_unsigned_char )
		{
		  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, LAR(o), HAU(o), 0 ,
			       GL_RGBA,
			       GL_UNSIGNED_BYTE,
			       L3_FLOAT(o)) ;
		}
	      else
	      glTexImage2D(GL_TEXTURE_2D, 0, 3, LAR(o), HAU(o), 0 ,
			   o->codec == codec_HL_unsigned_char ? GL_LUMINANCE : GL_RGB,
			   o->codec == codec_HL_l3_float ? GL_FLOAT : GL_UNSIGNED_BYTE,
			   L3_FLOAT(o)) ;
	    }
	  else
	    glTexImage2D(GL_TEXTURE_2D, 0, 3, LAR(vig), HAU(vig), 0 ,
			 vig->codec == codec_HL_unsigned_char ? GL_LUMINANCE : GL_RGB,
			 vig->codec == codec_HL_l3_float ? GL_FLOAT : GL_UNSIGNED_BYTE,
			 L3_FLOAT(vig)) ;
	  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP) ;
	  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP) ;
	  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST) ;
	  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST) ;
	}
      glEnable(GL_TEXTURE_2D) ;
      if ( im->eclairage )
	{
	  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE) ;
	  glEnable(GL_LIGHTING) ;
	}
      else
	{
	  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL) ;
	}
      
    }

  if ( o->codec == codec_HL_l4_unsigned_char )
    {
      glEnable(GL_BLEND) ;
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA) ;
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE) ;
      glEnable(GL_ALPHA_TEST);
      glAlphaFunc(GL_GREATER, 0.f);
    }
  glBegin(GL_QUADS) ;
  glTexCoord2f(0.,0.) ; glVertex2f(0.,0.) ;
  glTexCoord2f(1.,0.) ; glVertex2f(o->ratio,0.) ;
  glTexCoord2f(1.,1.) ; glVertex2f(o->ratio,1.) ;
  glTexCoord2f(0.,1.) ; glVertex2f(0.,1.) ;
  glEnd() ;
  if ( o->codec == codec_HL_l4_unsigned_char )
    {
      glDisable(GL_BLEND);
      glDisable(GL_ALPHA_TEST);
    }


  glDisable(GL_TEXTURE_2D) ;
  if ( im->eclairage )
    glDisable(GL_LIGHTING) ;

  if ( ACTIF(o) )
    {
      glBegin(GL_LINE_LOOP) ;
      glVertex2f(0.,0.) ;	
      glVertex2f(o->ratio,0.) ;	
      glVertex2f(o->ratio,1.) ;	
      glVertex2f(0.,1.) ;	
      glEnd() ;
    }
  glBindTexture(GL_TEXTURE_2D, 1);
}
/*
 *
 */

static int nb_bits(int i)
{
  int n ;

  n = 0 ;
  while( i )
    {
      i /= 2 ;
      n++ ;
    }
  return(n) ;
}

static void retaille_image(Image *o, int hau, int lar, int free_image)
{
  int i, j ;
  
  eprintf("retaille\n") ;
  if ( o->codec == codec_HL_l3_float )
    {
      t_l3_float *image ;
      CODEC_MALLOC(image, hau*lar) ;
      for(j=0;j<hau;j++)
	for(i=0;i<lar;i++)
	  {
	    image[j*lar+i] =
	      L3_FLOAT(o)[((j*HAU(o))/hau)*LAR(o) + (i*LAR(o))/lar ] ;
	  }
      if ( free_image )
	codec_free((char**)&L3_FLOAT(o)) ;
      L3_FLOAT(o) = image ;
    }
  else
    {
      if ( o->codec == codec_HL_unsigned_char )
	{
	  unsigned char *image ;
	  CODEC_MALLOC(image, hau*lar) ;
	  for(j=0;j<hau;j++)
	    for(i=0;i<lar;i++)
	      {
		image[j*lar+i] =
		  UNSIGNED_CHAR(o)[((j*HAU(o))/hau)*LAR(o) + (i*LAR(o))/lar ] ;
	      }
      if ( free_image )
	codec_free((char**)&UNSIGNED_CHAR(o)) ;
      UNSIGNED_CHAR(o) = image ;
	}
      else
	if ( o->codec == codec_HL_l3_unsigned_char )
	  {
	    {
	      t_l3_unsigned_char *image ;
	      CODEC_MALLOC(image, hau*lar) ;
	      for(j=0;j<hau;j++)
		for(i=0;i<lar;i++)
		  {
		    image[j*lar+i] =
		      L3_UNSIGNED_CHAR(o)[((j*HAU(o))/hau)*LAR(o) + (i*LAR(o))/lar ] ;
		  }
	      if ( free_image )
		codec_free((char**)&L3_UNSIGNED_CHAR(o)) ;
	      L3_UNSIGNED_CHAR(o) = image ;
	    }
	  }
      else
	  {
	    {
	      t_l4_unsigned_char *image ;
	      CODEC_MALLOC(image, hau*lar) ;
	      for(j=0;j<hau;j++)
		for(i=0;i<lar;i++)
		  {
		    image[j*lar+i] =
		      L4_UNSIGNED_CHAR(o)[((j*HAU(o))/hau)*LAR(o) + (i*LAR(o))/lar ] ;
		  }
	      if ( free_image )
		codec_free((char**)&L4_UNSIGNED_CHAR(o)) ;
	      L4_UNSIGNED_CHAR(o) = image ;
	    }
	  }
    }
  LAR(o) = lar ;
  HAU(o) = hau ;
}

static void destruction_image(Imag *o)
{
  if ( o->bindtexture )
    texture_free(&o->bindtexture) ;
}

static void changement_image(Imag *o)
{
  if ( LAR(&o->imag) == 0 )
    return ;

  if ((1<<nb_bits(LAR(&o->imag)-1)) != LAR(&o->imag)
      || (1<<nb_bits(HAU(&o->imag)-1)) != HAU(&o->imag)
      )
    {
      if ( 0 )
	retaille_image(&o->imag, HAU(&o->imag), LAR(&o->imag), 1 ) ;
      else
	retaille_image(&o->imag
		       , 1 << nb_bits(HAU(&o->imag)-1)
		       , 1 << nb_bits(LAR(&o->imag)-1)
		       , 1) ;
      o->vignette = o->imag ;
      texture_free(&o->bindtexture) ;
      retaille_image(&o->vignette, 16, 16, 0 ) ; // XXX Fuite memoire
    }
}

/*
 *
 */

static Parcour lecture_video(void *objet, int fildes)
{
  int j ;
  char buf[10000] ;
  Imag *o = objet ;

  for(j=HAU(&o->imag)-1; j>=0;j--)
    {
      read(fildes, &L3_UNSIGNED_CHAR(&o->imag)[j*LAR(&o->imag)]
	   ,  3*LAR(&o->imag)) ;
      read(fildes, buf,  3*(o->lar - LAR(&o->imag))) ;
    }
  for(j=0;j<o->hau-HAU(&o->imag);j++)
    read(fildes, buf,  3*o->lar) ;
  if ( ACTIF(o) )
    {
      evaluation(o) ;
    }

  return(Continue) ;
}

static Booleen video_live(const Interface_widget *iw)
{
  time_t t ;
  Imag *o ;

  time(&t) ;
  o = (Imag*)iw->objet ;
  glDisable(GL_DEPTH_TEST) ;
  do
    {
      lecture_video(o, o->f) ;
      Gl_start(&o->imag.gl) ;
      glRasterPos2i(-1,-1) ;
      glPixelStorei(GL_UNPACK_ALIGNMENT, 1) ;
      glDrawPixels(LAR(&o->imag),HAU(&o->imag),GL_RGB, GL_UNSIGNED_BYTE, L3_UNSIGNED_CHAR(&o->imag)) ;
      Gl_stop(&o->imag.gl) ;
      glFlush() ;
    }
  while( time(0) < t+10 ) ;
  glEnable(GL_DEPTH_TEST) ;
  return(Faux) ;
}

static void ferme_fichier_video(Imag *o)
{
  if ( o->f > 0 )
    {
      xt_enleve_fichier(o, lecture_video, o->f) ;
      close(o->f) ;
      o->f = 0 ;
    }
}

static void destruction_video(Imag *o)
{
  ferme_fichier_video(o) ;
#if ! CIV
  codec_libere(o->imag.codec, &o->imag.image) ;
#endif
}

static void changement_video(Imag *o)
{
  struct informations_table it ;

  ferme_fichier_video(o) ;

  o->imag.codec = codec_HL_l3_unsigned_char ;
  it = codec_informations_table(codec_informations(o->imag.codec)
				, &o->imag.image) ;
  codec_change_taille_table(&it
			    ,(1<<(nb_bits(o->hau)-1))
			    ,(1<<(nb_bits(o->lar)-1))
			    ) ;
  
  o->f = open(o->nom_device, O_RDONLY) ;
  if ( o->f > 0 )
    {
      xt_ajoute_fichier(o, lecture_video, o->f) ;
      codec_change_taille_table(&it
				,(1<<(nb_bits(o->hau)-1))
				,(1<<(nb_bits(o->lar)-1))
				) ;
      o->ratio = LAR(&o->imag) /(float) HAU(&o->imag) ;
      o->imag.ratio = o->ratio ;
    }
  else
    perror(o->nom_device) ;
}

/*
 *
 */
static void ecrit_pov_image(Image *o, const Champ *c, const char *v)
{
  if ( v[0]=='\0' ) return ;
  fprintf((FILE*)atol(v),
	  "polygon { 5, <0,0,0>,<1,0,0>,<1,1,0>,<0,1,0>,<0,0,0>\n"
	  "\tpigment { image_map { ppm \"%s\" } }\n"
	  "\tscale <%g,1,1>\n"
	  "\t}\n",
	  u_trouve_fichier
	  (directories_objets,
	   o->nom_image,"/MODELES/",
	   o->codec == codec_HL_unsigned_char ? ".pgm" : ".ppm"
	   ),
	  o->ratio
	  ) ;
}

static void ecrit_stl_image(Image *o, const Champ *c, const char *v)
{
  if ( v[0]=='\0' ) return ;
  fprintf((FILE*)atol(v),
	"# origine %g %g %g  vx %g %g %g  vy %g %g %g %s\n",
	globale_etat_courant.transfo.origine.x ,
	globale_etat_courant.transfo.origine.y ,
	globale_etat_courant.transfo.origine.z ,
	globale_etat_courant.transfo.v1.x *o->ratio,
	globale_etat_courant.transfo.v1.y *o->ratio,
	globale_etat_courant.transfo.v1.z *o->ratio,
	globale_etat_courant.transfo.v2.x ,
	globale_etat_courant.transfo.v2.y ,
	globale_etat_courant.transfo.v2.z ,
        o->nom_image
 ) ;
}





/*
 *
 */
CLASSE(image, Imag,
       BULLE_D_AIDE("Affiche une image sur le carré [0,1]x[0,1]")
       CHAMP(imag.image.l3_float.lar, LABEL("Largeur") L_entier Affiche NonInitialiser)
       CHAMP(imag.image.l3_float.hau, LABEL("Hauteur") L_entier Affiche NonInitialiser)
       CHAMP(eclairage, L_booleen Edite Sauve DEFAUT("1"))
       CHAMP_VIRTUEL(L_affiche_pov(ecrit_pov_image))
       CHAMP_VIRTUEL(L_affiche_gl(affiche_image)
		     AFFICHAGE(Different_si_actif))
       CHAMP_VIRTUEL(L_affiche_stl(ecrit_stl_image))
       DESTRUCTION(destruction_image)
       CHANGEMENT(changement_image)
       MENU("NON")
       )

CLASSE(image_l3_float, Imag,
       SUR_CLASSE("image")
       MENU("Figures/2d/Image RVB float")

       CHAMP(imag, LABEL("Nom Image")
	     BULLE_D_AIDE("L'image doit être au format PPM,\n"
			  "taper son nom sans l'extension.\n")
	     L_fichier_image_l3_float
	     Extrait Sauve Edite InitChaine
	     )
       CHAMP(imag, NOM("gl") NePasTester E_image Affiche P_image_l3_float TE_HAUTEUR(256))
       EVENEMENT("Ctrl+IF")
       )

CLASSE(image_l3_unsigned_char, Imag,
       SUR_CLASSE("image")
       MENU("Figures/2d/Image RVB unsigned char")

       CHAMP(imag, LABEL("Nom Image")
	     BULLE_D_AIDE("L'image doit être au format PPM,\n"
			  "taper son nom sans l'extension.\n")
	     L_fichier_image_l3_unsigned_char
	     Extrait Sauve Edite InitChaine
	     )
       CHAMP(imag, NOM("gl") NePasTester E_image P_image_l3_unsigned_char Affiche TE_HAUTEUR(256))
       EVENEMENT("Ctrl+IC")
       )

CLASSE(image_l4_unsigned_char, Imag,
       SUR_CLASSE("image")
       MENU("Figures/2d/Image RVBA unsigned char")

       CHAMP(imag, LABEL("Nom Image")
	     BULLE_D_AIDE("L'image doit être au format PNG transparente,\n"
			  "taper son nom sans l'extension.\n")
	     L_fichier_image_l4_unsigned_char
	     DEFAUT("test-png")
	     Extrait Sauve Edite InitChaine
	     )
       CHAMP(imag, NOM("gl") NePasTester E_image P_image_l4_unsigned_char Affiche TE_HAUTEUR(256))
       EVENEMENT("Ctrl+I4")
       )

CLASSE(entree_video, Imag,
       SUR_CLASSE("image")
       MENU("Figures/2d/Entrée Video")

       CHAMP(lar, L_nombre P_entier Edite Sauve DEFAUT("352")) 
       CHAMP(hau, L_nombre P_entier Edite Sauve DEFAUT("288")) 
       CHAMP(nom_device, LABEL("Nom device") L_chaine
	     DEFAUT("/dev/video0")
	     Extrait Sauve Edite InitChaine
	     )
       CHAMP_IMAGE_VIDEO
       CHAMP_VIRTUEL(L_affiche_gl(affiche_image)
		     AFFICHAGE(Toujours_different))
       CHAMP_VIRTUEL(LABEL("Vidéo LIVE") L_bouton(video_live))
       CHANGEMENT(changement_video)
       DESTRUCTION(destruction_video)
       EVENEMENT("Ctrl+EV")
       )

CLASSE(image_unsigned_char, Imag,
       SUR_CLASSE("image")
       MENU("Figures/2d/Image unsigned char")

       CHAMP(imag, LABEL("Nom Image")
	     BULLE_D_AIDE("L'image doit être au format PGM,\n"
			  "taper son nom sans l'extension.\n")
	     L_fichier_image_unsigned_char
	     Extrait Sauve Edite InitChaine
	     )
       CHAMP(imag, NOM("gl") E_image NePasTester P_image_unsigned_char Affiche TE_HAUTEUR(256))
       EVENEMENT("Ctrl+IU")
       )

static void affiche_image_2d(Imag *o, const Champ *ch, const char *v)
{
  glDisable(GL_DEPTH_TEST) ;
  glRasterPos2i(0,0) ;
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1) ;
  glDrawPixels(LAR(&o->imag),HAU(&o->imag)
	       ,GL_RGB, GL_UNSIGNED_BYTE, L3_UNSIGNED_CHAR(&o->imag)) ;
  glEnable(GL_DEPTH_TEST) ;
}

CLASSE(image_2d_l3_unsigned_char, Imag,
       CHAMP_VIRTUEL(L_affiche_pov(NULL))
       CHAMP_VIRTUEL(L_affiche_gl(affiche_image_2d)
		     AFFICHAGE(Toujours_different))
	     
       SUR_CLASSE("image_l3_unsigned_char")
       CHANGEMENT(NULL)
       MENU("Figures/2d/Image 2D RVB unsigned char")

       EVENEMENT("Ctrl+I2C")
       )

