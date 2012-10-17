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
#include <GL/gl.h>
#include <string.h>
#include "t_image.h"
#include "o_objet.h"
#include "u_fichier.h"
#include "s_string.h"

#define USEPNG 1

#if USEPNG
#include "png.h"

/*
 * Guillaume Cottenceau (gc at mandrakesoft.com)
 *
 * Copyright 2002 MandrakeSoft
 *
 * This software may be freely redistributed under the terms of the GNU
 * public license.
 *
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#define PNG_DEBUG 3
#include <png.h>



int read_png_file(const char* file_name, Image *p)
{
  int y;
  
  png_byte color_type;
  png_byte bit_depth;
  
  png_structp png_ptr;
  png_infop info_ptr;
  int number_of_passes;
  png_bytep * row_pointers;

  unsigned char header[8];	// 8 is the maximum size that can be checked
  unsigned char *image ;
  
  /* open file and test for it being a png */
  FILE *fp = fopen(file_name, "rb");
  if (!fp)
    return 1;
  fread(header, 1, 8, fp);
  if (png_sig_cmp(header, 0, 8))
    {
      fclose(fp) ;
      return 1 ;
    }

  /* initialize stuff */
  png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  info_ptr = png_create_info_struct(png_ptr);
  png_init_io(png_ptr, fp);
  png_set_sig_bytes(png_ptr, 8);

  png_read_info(png_ptr, info_ptr);

  // Modified by Houssam on 17/10/2012 to be able to work with libpng1.5
  // because libpng1.5 prevents direct access to png_struct and png_info structure
//   LAR(p) = info_ptr->width;
  LAR(p) = png_get_image_width(png_ptr,info_ptr);
//   HAU(p) = info_ptr->height;
  HAU(p) = png_get_image_height(png_ptr,info_ptr);
  p->ratio = LAR(p)/(double)HAU(p) ;

//   color_type = info_ptr->color_type;
  color_type = png_get_color_type(png_ptr,info_ptr);
//   bit_depth = info_ptr->bit_depth;
  bit_depth = png_get_bit_depth(png_ptr,info_ptr);
  
  // End Houssam Modification
  
  if ( color_type != PNG_COLOR_TYPE_RGB_ALPHA )
    {
      eprintf("Ne supporte que les PNG avec transparence\n") ;
      return 1 ;
    }
  if ( bit_depth != 8 )
    {
      eprintf("Ne supporte que 8 bits par composantes\n") ;
      return 1 ;
    }


  number_of_passes = png_set_interlace_handling(png_ptr);
  png_read_update_info(png_ptr, info_ptr);

  CODEC_MALLOC(image, LAR(p)*HAU(p)*4) ;

  row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * HAU(p));
  for (y=0; y<HAU(p); y++)
    row_pointers[y] = (png_byte*) image + (HAU(p) - y - 1)*LAR(p)*4 ;

  png_read_image(png_ptr, row_pointers);

  fclose(fp);

  free(row_pointers) ;
  L4_UNSIGNED_CHAR(p) = (t_l4_unsigned_char*)image ;

  png_destroy_read_struct(&png_ptr, &info_ptr, NULL) ;
  return 0 ;
}

#endif


DEFINITION_TYPE_LOGIQUE(L_fichier_image, L_chaine,
			"Image sans type physique",
			c->defaut = "doua" ;
			)

/*
 *
 */

static int image_libere(Image *i)
{
  return( codec_libere(i->codec, &i->image) ) ;
}

static int image_fichier_lit(Image *p)
{
  return( codec_char_etoile_recupere_texte((const char**)&p->nom_image) ) ;
}

static int lecture_image(Image *p)
{
  FILE *f ;
  char c ;
  unsigned char *image ;
  int i ;
  const char *nom ;
  char *suffix ;

  image_libere(p) ;

  if ( p->codec == codec_HL_unsigned_char )
    suffix = ".pgm" ;
  else if ( p->codec == codec_HL_l3_unsigned_char )
    suffix = ".ppm" ;
  else if ( p->codec == codec_HL_l4_unsigned_char )
    suffix = ".png" ;


  nom = u_trouve_fichier(directories_objets
			 , p->nom_image
			 , "/MODELES/"
			 , suffix) ;

  if ( nom == NULL )
    {
      if ( strcmp(suffix, ".ppm") == 0 )
	{
	  suffix = ".png" ;
	  nom = u_trouve_fichier(directories_objets
				 , p->nom_image
				 , "/MODELES/"
				 , suffix) ;
	  if ( nom == NULL )
	    {
	      suffix = ".jpg" ;
	      nom = u_trouve_fichier(directories_objets
				     , p->nom_image
				     , "/MODELES/"
				     , suffix) ;
	    }
	}
      if ( nom == NULL )
	return(1) ;
    }
  if ( p->codec == codec_HL_l4_unsigned_char )
    return read_png_file(nom, p) ;

  if ( strcmp(suffix, ".ppm") == 0 )
    f = fopen( nom, "r" ) ;
  else
    {
      Chaine tmp = s_ouvre_ecriture() ;
      if ( strcmp(suffix, ".jpg") == 0 )
	  s_printf(&tmp, "djpeg -pnm <%s", nom) ;
	else
	  s_printf(&tmp, "pngtopnm <%s", nom) ;
      f = popen(s_char(&tmp) , "r") ;
      s_ferme(&tmp) ;
    }
  if ( f==NULL )
    {
      return(1) ;
    }
  fscanf(f,"%*[^\n]\n") ;	/* Saute la premiere ligne (P6) */
  for( c = fgetc(f) ; c == '#' ; c = fgetc(f) )
    fscanf(f,"%*[^\n]\n") ;
  ungetc(c,f) ;
  if ( fscanf(f, "%d %d\n", &LAR(p), &HAU(p)) != 2 )
    {
      if ( strcmp(suffix, ".ppm") == 0 )
	fclose(f) ;
      else
	pclose(f) ;
      return(1) ;
    }
  /*
   * Allocation place lecture
   */
  if ( p->codec == codec_HL_unsigned_char )
    CODEC_MALLOC(image, LAR(p)*HAU(p)) ;
  else
    CODEC_MALLOC(image, LAR(p)*HAU(p)*3) ;
  /*
   * Saute le 255 et commentaires
   */
  fgets((char*)image,LAR(p)*HAU(p)*3,f) ;
  /*
   * Lecture en inversant haut et bas
   */
  for(i=0;i<HAU(p);i++)
    if ( p->codec == codec_HL_unsigned_char )
      fread(image+LAR(p)*(HAU(p)-i-1), LAR(p), 1, f) ;
    else
      fread(image+3*LAR(p)*(HAU(p)-i-1), LAR(p), 3, f) ;
  /*
   * Transformations si nécessaire
   */  
  if ( p->codec == codec_HL_l3_float )
    {
      CODEC_MALLOC(L3_FLOAT(p), LAR(p)*HAU(p)*3);
      for(i=0;i<LAR(p)*HAU(p)*3;i++)
	((float*)L3_FLOAT(p))[i] = image[i]/256. ;
      CODEC_FREE(image) ;
    }
  else
    {
      *((unsigned char**)&L3_UNSIGNED_CHAR(p)) = image ;
    }
  if ( strcmp(suffix, ".ppm") == 0 )
    fclose(f) ;
  else
    pclose(f) ;
  /*
   *
   */
  if ( HAU(p) )
    p->ratio = LAR(p)/(double)HAU(p) ;
  else
    p->ratio = 1 ;
  return(0) ;
}

static int image_fichier_ecrit(Image *p, Codec codec) 
{
  codec_char_etoile_sauve_texte(&p->nom_image) ;
  p->codec = codec ;
  return( lecture_image(p) ) ;
}

static int image_fichier_l3_float_ecrit(Image *p, int type) 
{
  return( image_fichier_ecrit(p, codec_HL_l3_float)) ;
}

static int image_fichier_l3_unsigned_char_ecrit(Image *p, int type) 
{
  return( image_fichier_ecrit(p, codec_HL_l3_unsigned_char)) ;
}

static int image_fichier_l4_unsigned_char_ecrit(Image *p, int type) 
{
  return( image_fichier_ecrit(p, codec_HL_l4_unsigned_char)) ;
}

static int image_fichier_unsigned_char_ecrit(Image *p, int type) 
{
  return( image_fichier_ecrit(p, codec_HL_unsigned_char)) ;
}


TYPE_DE_BASE(codec_fichier_image_l3_float, Image,
	     image_fichier_lit,
	     image_fichier_l3_float_ecrit,
	     image_libere,
	     codec_rien
	     )
DEFINITION_TYPE_LOGIQUE(L_fichier_image_l3_float, L_fichier_image,
			"Image de triplet de float",
			c->tp = &infos_codec_fichier_image_l3_float ;
			)


TYPE_DE_BASE(codec_fichier_image_l3_unsigned_char, Image,
	     image_fichier_lit,
	     image_fichier_l3_unsigned_char_ecrit,
	     image_libere,
	     codec_rien
	     )
DEFINITION_TYPE_LOGIQUE(L_fichier_image_l3_unsigned_char, L_fichier_image,
			"Image de triplet unsigned char",
			c->tp = &infos_codec_fichier_image_l3_unsigned_char ;
			)

TYPE_DE_BASE(codec_fichier_image_l4_unsigned_char, Image,
	     image_fichier_lit,
	     image_fichier_l4_unsigned_char_ecrit,
	     image_libere,
	     codec_rien
	     )
DEFINITION_TYPE_LOGIQUE(L_fichier_image_l4_unsigned_char, L_fichier_image,
			"Image de quadruplet unsigned char",
			c->tp = &infos_codec_fichier_image_l4_unsigned_char ;
			)


TYPE_DE_BASE(codec_fichier_image_unsigned_char, Image,
	     image_fichier_lit,
	     image_fichier_unsigned_char_ecrit,
	     image_libere,
	     codec_rien
	     )
DEFINITION_TYPE_LOGIQUE(L_fichier_image_unsigned_char, L_fichier_image,
			"Image de triplet unsigned char",
			c->tp = &infos_codec_fichier_image_unsigned_char ;
			)

/*
 *
 */

int recharge_image(Image *o)
{
  int erreur ;

  if ( o->nom_image )
    {
      erreur = lecture_image(o) ;
      /*      OBJET(o)->mettre_a_jour_widget = Vrai ; 25/02/2000 */
    }
  else
    erreur = 1 ;

  return(erreur) ;
}

static Booleen u_image_recharge(const Interface_widget *iw)
{
  recharge_image(&T(Image, iw->objet, iw->ch)) ;
  return(0) ;
}


static void u_image_trace(const Interface_widget *iw)
{
  Image *o ;

  o = iw->objet ;
  if ( L3_FLOAT(o)==NULL ) return ;

  Gl_start(&o->gl) ;
  /*  widget_resize(o->gl.gl, LAR(o), HAU(o)) ; */
  glViewport(0, 0, LAR(o), HAU(o)) ;
  glClearColor(0.5,0.5,0.5,0.) ;
  glClear(GL_COLOR_BUFFER_BIT) ;
  glDisable(GL_DEPTH_TEST) ;
  glRasterPos2i(-1,-1) ;
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1) ;
  if ( o->codec == codec_HL_l3_float )
    glDrawPixels(LAR(o),HAU(o),GL_RGB, GL_FLOAT, L3_FLOAT(o)) ;
  else
    if ( o->codec == codec_HL_unsigned_char )
      glDrawPixels(LAR(o),HAU(o),GL_LUMINANCE, GL_UNSIGNED_BYTE, UNSIGNED_CHAR(o)) ;
    else if ( o->codec == codec_HL_l3_unsigned_char )
      glDrawPixels(LAR(o),HAU(o),GL_RGB, GL_UNSIGNED_BYTE, L3_UNSIGNED_CHAR(o)) ;
    else
      {
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA) ;
	glEnable(GL_BLEND) ;
	glDrawPixels(LAR(o),HAU(o),GL_RGBA, GL_UNSIGNED_BYTE, L4_UNSIGNED_CHAR(o)) ;
	glDisable(GL_BLEND) ;
      }
  
  glEnable(GL_DEPTH_TEST) ;
  Gl_stop(&o->gl) ;

  if ( TRACE(o) )
    eprintf("Affichage d'image : fin\n") ;
}

DEFINITION_TYPE_EDITEUR(E_image, E_gl,
			"Zone d'affichage d'image en GL",
			{
			  c->te_entree = u_image_recharge ;
			  c->te_ecrit  = u_image_trace ;
			}
			)




static int image_lit(Image *p)
{
  return( (*p->codec)(&p->image) ) ;
}

static int image_l3_float_ecrit(Image *p)
{
  p->codec = codec_HL_l3_float ;  
  return( (*p->codec)(&p->image) ) ;
}

static int image_l3_unsigned_char_ecrit(Image *p)
{
  p->codec = codec_HL_l3_unsigned_char ;  
  return( (*p->codec)(&p->image) ) ;
}

static int image_l4_unsigned_char_ecrit(Image *p)
{
  p->codec = codec_HL_l4_unsigned_char ;  
  return( (*p->codec)(&p->image) ) ;
}

static int image_unsigned_char_ecrit(Image *p)
{
  p->codec = codec_HL_unsigned_char ;  
  return( (*p->codec)(&p->image) ) ;
}


TYPE_DE_BASE(codec_image, Image,
	     codec_vide,
	     codec_rien,
	     image_libere,
	     codec_rien
	     )

TYPE_DE_BASE(codec_image_l3_float, Image,
	     image_lit,
	     image_l3_float_ecrit,
	     image_libere,
	     codec_rien
	     )

TYPE_DE_BASE(codec_image_l3_unsigned_char, Image,
	     image_lit,
	     image_l3_unsigned_char_ecrit,
	     image_libere,
	     codec_rien
	     )

TYPE_DE_BASE(codec_image_l4_unsigned_char, Image,
	     image_lit,
	     image_l4_unsigned_char_ecrit,
	     image_libere,
	     codec_rien
	     )

TYPE_DE_BASE(codec_image_unsigned_char, Image,
	     image_lit,
	     image_unsigned_char_ecrit,
	     image_libere,
	     codec_rien
	     )

static Booleen image_l3_float(const void *o, const Champ *c)
{
  return( c->tp == &infos_codec_image_l3_float ) ;
}
static Booleen image_l3_unsigned_char(const void *o, const Champ *c)
{
  return( c->tp == &infos_codec_image_l3_unsigned_char ) ;
}
static Booleen image_l4_unsigned_char(const void *o, const Champ *c)
{
  return( c->tp == &infos_codec_image_l4_unsigned_char ) ;
}
static Booleen image_unsigned_char(const void *o, const Champ *c)
{
  return( c->tp == &infos_codec_image_unsigned_char ) ;
}

DEFINITION_TYPE_LOGIQUE(L_image, L_valeur,
			"Image",
			init_E_image(c)
			)

DEFINITION_TYPE_LOGIQUE(L_image_l3_float, L_image,
			"Image de triplet de float",
			c->tp = &infos_codec_image_l3_float ;
			PREDICAT(image_l3_float) ;
			)

DEFINITION_TYPE_LOGIQUE(L_image_l3_unsigned_char, L_image,
			"Image de triplet de unsigned char",
			c->tp = &infos_codec_image_l3_unsigned_char ;
			PREDICAT(image_l3_unsigned_char) ;
			)
DEFINITION_TYPE_LOGIQUE(L_image_l4_unsigned_char, L_image,
			"Image de quadruplet de unsigned char",
			c->tp = &infos_codec_image_l4_unsigned_char ;
			PREDICAT(image_l4_unsigned_char) ;
			)

DEFINITION_TYPE_LOGIQUE(L_image_unsigned_char, L_image,
			"Image de triplet de unsigned char",
			c->tp = &infos_codec_image_unsigned_char ;
			PREDICAT(image_unsigned_char) ;
			)

