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
#include "codec.h"
#include "codec_base.h"
#include <stdlib.h>
#include <string.h>
#include "eprintf.c"
#include "s_string.c"


void chaine(const char *val)
{
  if ( *val == '\0' )
    printf(" ") ;
  else
    for(; *val ; val++)
      {
	if ( *val == '\n' )
	  printf("§<BR>\n") ;
	else
	  if ( *val == ' ' )
	    printf("·") ;
	  else
	    printf("%c", *val) ;
      }
  printf("\n") ;
}
/*
 * Tests de base
 */

struct test
{
  char *texte ;
  Codec codec ;
  char *attendue ;
} ;

void erreur(struct test *t, char *message, char *recue)
{
  fprintf(stderr,
	  "%s\n"
	  "Valeur inattendue pour `%s' de type `%s'\n"
	  "On attendait `%s', on recoit `%s'\n"
	  ,message
	  ,t->texte
	  ,codec_informations(t->codec)->nom
	  ,t->attendue
	  ,recue
	  ) ;
}

int nbzm()
{
#ifdef DEBUG_CODEC
  return(nb_zones_memoires(&globale_gm_codec)) ;
#else
  return(globale_codec_malloc) ;
#endif
}

char* essaye(struct test *t, float *p, char *message, char *in, int libere)
{
  char *out ;

  codec_sauve_texte(t->codec, p, in) ;
  out = (char*)codec_recupere_texte(t->codec, p) ;
  if ( strcmp(out, t->attendue) )
    erreur(t, message, out) ;
  if ( libere )
    {
      codec_libere(t->codec, p) ;
      if ( nbzm() )
	{
	  fprintf(stderr, "%s : %s\n",
		  codec_informations(t->codec)->nom,message) ;
	  fprintf(stderr, "globale_codec_malloc = %d\n", nbzm());
	  exit(1) ;
	}
    }
  return(out) ;
}

void test_basique()
{
  int i ;
  float p[10000] ;
  char *in, *out, *tmp ;

  struct test t[] =
  {
    { ""     			, codec_float 	 , "0" },
    { ""     			, codec_float1	 , "1" },
    { "5.6"    			, codec_float	 , "5.6" },
    { "5.6"    			, codec_float1	 , "5.6" },
    { "toto" 			, codec_float 	 , "0" },
    { "toto" 			, codec_float1	 , "1" },
    { "1  2.7  3 "	   , codec_l3_int       , "1 2 3"},
    { "1  2  3 "	   , codec_l3_float     , "1 2 3"},
    { "1  2  3 "	   , codec_l3_float1    , "1 2 3"},
    { "1  2  "  	   , codec_l3_float     , "1 2 0"},
    { "1  2  "  	   , codec_l3_float1    , "1 2 1"},
    { "1  2 a"  	   , codec_l3_float     , "1 2 0"},
    { "1  2 a"  	   , codec_l3_float1    , "1 2 1"},
    { "1  2a"   	   , codec_l3_float     , "1 2 0"},
    { "1  2a"   	   , codec_l3_float1    , "1 2 1"},
    { "1  2"    	   , codec_L_double     , "(1 2)"},
    { "1 2 3\n4 5\n6 7 8 9", codec_L_l3_double  , "(1 2 3\n4 5 0\n6 7 8)"},
    { "1 2 3\n4 5\n6 7 8 9", codec_H_L_double   , "[(1 2 3)\n(4 5 0)\n(6 7 8)]"},
    { "", codec_H_L_double   , "[]"},
    { "1 2 3\n4 5\n6 7 8 9", codec_PHL_float    , "{[(1 2 3)\n(4 5 0)\n(6 7 8)]}"},
    { "1 2 3\n4 5"         , codec_PHL_float    , "{[(1 2 3)\n(4 5 0)]}"},
    { "1 2\n4 5\n6 7 8 9"  , codec_PHL_float    , "{[(1 2)\n(4 5)\n(6 7)]}"},
    { "1 2 3\n4 5\n6 7 8 9\n\n999",
      codec_PHL_float    ,
      "{[(1 2 3)\n(4 5 0)\n(6 7 8)]\n[(999 0 0)\n(0 0 0)\n(0 0 0)]}"},
    { "1 2 3\n4 5\n6 7 8 9\n\n999\n\n888",
      codec_PHL_float    ,
      "{[(1 2 3)\n(4 5 0)\n(6 7 8)]\n[(999 0 0)\n(0 0 0)\n(0 0 0)]\n[(888 0 0)\n(0 0 0)\n(0 0 0)]}"},
    { "1 2 3\n4 5\n6 7 8 9\n\n999\n9\n8\n7\n\n888",
      codec_PHL_float    ,
      "{[(1 2 3)\n(4 5 0)\n(6 7 8)]\n[(999 0 0)\n(9 0 0)\n(8 0 0)]\n[(888 0 0)\n(0 0 0)\n(0 0 0)]}"},
    { "[(1 2 3\n4 5\n6 7 8 9)\n(11 111 1111 11111\n2)\n(3 33\n4 44 444\n5 55\n6 66 666 6666)]",
      codec_H_L_l3_double
      , "[(1 2 3\n4 5 0\n6 7 8)\n(11 111 1111\n2 0 0\n0 0 0)\n(3 33 0\n4 44 444\n5 55 0)]"},
    { "1 2 3\n4 5\n6 7 8 9\n\n11 111 1111 11111\n2\n\n3 33\n4 44 444\n5 55\n6 66 666 6666",
      codec_H_L_l3_double
      , "[(1 2 3\n4 5 0\n6 7 8)\n(11 111 1111\n2 0 0\n0 0 0)\n(3 33 0\n4 44 444\n5 55 0)]"},
    { "{[(1 2 3)]}",
      codec_P_H_L_l3_float,
      "{[(1 2 3)]}"},
    { "{[(1 2 3\n4 5 6)]}",
      codec_P_H_L_l3_float,
      "{[(1 2 3\n4 5 6)]}"},
    { "{[(1 2 3\n4 5 6)(7 8 9)]}",
      codec_P_H_L_l3_float,
      "{[(1 2 3\n4 5 6)\n(7 8 9\n0 0 0)]}"},
    { "{[(1 2 3\n4 5 6)(7 8 9)][(10 11 12)]}",
      codec_P_H_L_l3_float,
      "{[(1 2 3\n4 5 6)\n(7 8 9\n0 0 0)]\n[(10 11 12\n0 0 0)\n(0 0 0\n0 0 0)]}"},
  } ;

  for(i=0; i<sizeof(t)/sizeof(t[0]); i++)
    {
      memset(p, '\0', sizeof(p)) ;

      in = strdup(essaye(&t[i], p, "Sauvegarde simple", t[i].texte, 1)) ;

      out = essaye(&t[i], p, "Sauvegarde double", in, 1) ;

      in = realloc(in, strlen(in) + 100) ;
      strcat(in, "\n\n\n\n") ;
      essaye(&t[i], p, "Sauvegarde en ajoutant des linefeeds", in, 1) ;

      strcpy(in + strlen(in) - 4, "))))") ;
      essaye(&t[i], p, "Sauvegarde en ajoutant des )", in, 1) ;

      strcpy(in + strlen(in) - 4, "]]]]") ;
      essaye(&t[i], p, "Sauvegarde en ajoutant des ]", in, 1) ;

      while((tmp=strstr(in,")\n"))) strcpy(tmp+1, tmp+2) ;
      while((tmp=strstr(in,"}\n"))) strcpy(tmp+1, tmp+2) ;
      while((tmp=strstr(in,"]\n"))) strcpy(tmp+1, tmp+2) ;
      essaye(&t[i], p, "Sauvegarde en enlevant les \\n apres )}]", in, 0) ;
      essaye(&t[i], p, "Sauvegarde sans liberation avant", in, 1) ;
     
      free(in) ;
    }
}

struct pt { int nb ; double x, y, z ; } ;

struct bidon
{
  int pro, hau, lar ;
  struct pt **triplet ;
  float **poids ;
  char c[100000] ;
} ;

TYPE_PHYSIQUE(codec_bidon, struct bidon,
	      {
		return( codec_H_L_pp((void***)&a->triplet, codec_l3_double_nolf,
				     (void***)&a->poids, codec_float1_lf,
				     &a->hau, &a->lar) ) ;
	      }
	      )

TYPE_PHYSIQUE(codec_pt, struct pt,
	      {
		return( codec_l(&a->x, 3, codec_double) ) ;
	      }
	      )
TYPE_PHYSIQUE(codec_pt2, struct pt,
	      {
		return( codec_L((void**)&a->x, &a->nb, codec_double) ) ;
	      }
	      )

TYPE_PHYSIQUE(codec_table2, struct bidon,
	      {
		return( codec_H_L((void***)&a->triplet, &a->hau,
				  &a->lar, codec_l3_double) ) ;
	      }
	      )
TYPE_PHYSIQUE(codec_table3, struct bidon,
	      {
		return( codec_HL((void**)&a->triplet, &a->hau,
				  &a->lar, codec_l3_double) ) ;
	      }
	      )
TYPE_PHYSIQUE(codec_table3bis, struct bidon,
	      {
		return( codec_HH((void**)&a->triplet, &a->hau,
				  codec_l3_double) ) ;
	      }
	      )
TYPE_PHYSIQUE(codec_table3ter, struct bidon,
	      {
		return( codec_h_L((void**)&a->triplet, 2, &a->lar,
				  codec_l3_double) ) ;
	      }
	      )
TYPE_PHYSIQUE(codec_table3bisbis, struct bidon,
	      {
		return( codec_L_h((void**)&a->triplet, 2, &a->lar,
				  codec_l3_double) ) ;
	      }
	      )
TYPE_PHYSIQUE(codec_tablePHL, struct bidon,
	      {
		return( codec_PHL((void*)&a->triplet, &a->pro, &a->hau, &a->lar
				  ,codec_double) ) ;
	      }
	      )
TYPE_PHYSIQUE(codec_table4, struct bidon,
	      {
		return( codec_hl((void*)&a->triplet, 2, 2, codec_l3_double) ) ;
	      }
	      )

void test_table3()
{
  struct bidon pt ;
  int i , j, k, l;
  int c ;
  const char *s ;
  static Codec cc[] = { codec_tablePHL } ;
  struct informations_table it ;

  for(c=0;c<sizeof(cc)/sizeof(cc[0]);c++)
    {
      MET_A_0(&pt) ;
      codec_sauve_texte(cc[c], &pt, "{[(1 2 3 4)(5 6 7 8)(9 10 11 12)][(13 14 15 16)(17 18 19 20)(21 22 23 24)]}") ;
      it = codec_informations_table(codec_informations(cc[c]), &pt) ;
      for(k=0;k<2;k++)
	for(j=0;j<3;j++)
	  for(i=0;i<4;i++)
	    {
	      codec_positionne_element_table(&it, k, j, i) ;
	      s = codec_recupere_element_courant(&it) ;
	      l = 1 + i + 4*j + 12*k ;
	      if ( atoi(s)!=l )
	      {
		fprintf(stderr,"c=%d l=%d recupere=(%s)\n", c, l, s) ;
		fprintf(stderr,"recupere tout=(%s)\n", codec_recupere_texte(cc[c], &pt)) ;
		fprintf(stderr,"Pas bonne table3\n") ;
		exit(1) ;
	      }
	  }
      for(k=0;k<2;k++)
	for(j=0;j<3;j++)
	  for(i=0;i<4;i++)
	    {
	      s = codec_recupere_element_courant(&it) ;
	      l = 1 + i + 4*j + 12*k ;
	      if ( atoi(s)!=l )
	      {
		fprintf(stderr,"Pas bonne table3 bis\n") ;
		exit(1) ;
	      }
	  }
      for(k=0;k<2;k++)
	for(j=0;j<3;j++)
	  for(i=0;i<4;i++)
	    {
	      codec_positionne_element_table(&it, k, j, i) ;
	      codec_sauve_element_courant(&it, "1") ;
	      codec_positionne_element_table(&it, k, j, i) ;
	    }
      if ( strcmp(codec_recupere_texte(cc[c], &pt)
		  , "{[(1 1 1 1)\n(1 1 1 1)\n(1 1 1 1)]\n[(1 1 1 1)\n(1 1 1 1)\n(1 1 1 1)]}")!=0
	   )
	{
	  fprintf(stderr,"[%d] Pas bonne écriture table3 : %s != table 1 1 1\n"
		  ,c, codec_recupere_texte(cc[c], &pt)) ;
	  exit(1) ;
	}
      codec_libere(cc[c], &pt) ;
    }
}

void test_table2()
{
  struct bidon pt ;
  t_l3_double l3 ;
  int i , j, k;
  int c ;
  const char *s ;
  static Codec cc[] = { codec_table2, codec_table3, codec_table4, codec_table3bis, codec_table3ter, codec_table3bisbis } ;
  struct informations_table it ;

  for(c=0;c<sizeof(cc)/sizeof(cc[0]);c++)
    {
      MET_A_0(&pt) ;
      switch(c)
	{
	case 2:
	  codec_sauve_texte(cc[c], &pt, "(1 1 1\n2 2 2)\n(3 3 3\n4 4 4)") ;
	  break ;
	case 5:
	  codec_sauve_texte(cc[c], &pt, "1 1 1\n3 3 3\n\n2 2 2\n4 4 4") ;
	  break ;
	default:
	  codec_sauve_texte(cc[c], &pt, "1 1 1\n2 2 2\n\n3 3 3\n4 4 4") ;
	  break ;
	}
      it = codec_informations_table(codec_informations(cc[c]), &pt) ;
      for(j=0;j<2;j++)
	for(i=0;i<2;i++)
	  {
	    codec_positionne_element_table(&it, j, i) ;
	    s = codec_recupere_element_courant(&it) ;
	    codec_sauve_texte(codec_l3_double, &l3, s) ;
	    k = 1 + i + 2*j ;
	    if ( l3.table[0]!=k || l3.table[1]!=k || l3.table[2]!=k )
	      {
		fprintf(stderr,"c=%d k=%d recupere=(%s)\n", c, k, s) ;
		fprintf(stderr,"recupere tout=(%s)\n", codec_recupere_texte(cc[c], &pt)) ;
		fprintf(stderr,"Pas bonne table2\n") ;
		exit(1) ;
	      }
	  }
      for(j=0;j<2;j++)
	for(i=0;i<2;i++)
	  {
	    s = codec_recupere_element_courant(&it) ;
	    codec_sauve_texte(codec_l3_double, &l3, s) ;
	    k = 1 + i + 2*j ;
	    if ( l3.table[0]!=k || l3.table[1]!=k || l3.table[2]!=k )
	      {
		fprintf(stderr,"Pas bonne table2 bis\n") ;
		exit(1) ;
	      }
	  }
      for(j=0;j<2;j++)
	for(i=0;i<2;i++)
	  {
	    codec_positionne_element_table(&it, j, i) ;
	    codec_sauve_element_courant(&it, "1 1 1") ;
	  }
      s = codec_recupere_texte(cc[c], &pt) ;
      if ( strcmp(s, "[(1 1 1\n1 1 1)\n(1 1 1\n1 1 1)]")!=0
	   && strcmp(s, "(1 1 1\n1 1 1)\n(1 1 1\n1 1 1)")!=0
	   && strcmp(s, "(1 1 1\n1 1 1\n1 1 1\n1 1 1)")!=0
	   )
	{
	  fprintf(stderr,"[%d] Pas bonne écritu-e table : %s != table 1 1 1\n"
		  ,c, codec_recupere_texte(cc[c], &pt)) ;
	  exit(1) ;
	}
      codec_libere(cc[c], &pt) ;
    }
}

void test_table()
{
  struct pt pt ;
  int i ;
  int c ;
  static Codec cc[] = { codec_pt, codec_pt2 } ;
  struct informations_table it ;

  for(c=0;c<sizeof(cc)/sizeof(cc[0]);c++)
    {
      MET_A_0(&pt) ;
      codec_sauve_texte(cc[c], &pt, "6 7 8") ;
      it = codec_informations_table(codec_informations(cc[c]), &pt) ;
      for(i=0;i<3;i++)
	{
	  codec_positionne_element_table(&it, i) ;
	  if ( i+6 != atoi( codec_recupere_element_courant(&it)) )
	    {
	      fprintf(stderr,"Pas bonne table\n") ;
	      exit(1) ;
	    }
	}
      for(i=0;i<3;i++)
	{
	  if ( i+6 != atoi( codec_recupere_element_courant(&it)) )
	    {
	      fprintf(stderr,"Pas bonne table bis\n") ;
	      abort() ;
	    }
	}
      for(i=0;i<3;i++)
	{
	  codec_positionne_element_table(&it, i) ;
	  codec_sauve_element_courant(&it, "5.5") ;
	}
      if ( strcmp(codec_recupere_texte(cc[c], &pt), "5.5 5.5 5.5")!=0 &&
	   strcmp(codec_recupere_texte(cc[c], &pt), "(5.5 5.5 5.5)")!=0
	   )
	    {
	      fprintf(stderr,"[%d] Pas bonne écritue table : %s != %s\n"
		      ,c, codec_recupere_texte(cc[c], &pt), "5.5 5.5 5.5") ;
	      exit(1) ;
	    }
      codec_libere(cc[c], &pt) ;
    }
}

void test_H_L()
{
  t_H_L_l3_double t ;
  struct informations_table it ;
  int i, j, n ;

  MET_A_0(&t) ;
  codec_sauve_texte(codec_H_L_l3_double, &t, "[(6 7 8\n9)\n(1 2 3\n)]") ;
  n = nbzm() ;
  codec_sauve_texte(codec_H_L_l3_double, &t, "[(6 7 8\n9)]") ;
  if ( nbzm() - n != -1 )
    {
      fprintf(stderr, "Oublie liberation dans H_L : \n") ;
      exit(1) ;
    }

  codec_sauve_texte(codec_H_L_l3_double, &t, "[(0 1 0\n0 1 0\n0 1 0)\n(1 0 0\n-1 0 0\n1 0 0)\n(0 -1 0\n0 -1 0\n0 -1 0)]") ;
  if ( t.lar != 3 || t.hau != 3 )
    {
      fprintf(stderr, "Mauvaissse taille H_L : %d %d\n",t.hau, t.lar ) ;
      exit(1) ;
    }
  n = nbzm() ;
  codec_sauve_texte(codec_H_L_l3_double, &t, "[(0 1 0\n0 1 0)\n(0 -1 0\n0 -1 0)]") ;
  if ( nbzm() - n != -1 )
    {
      fprintf(stderr, "Oublie liberation dans H_L : \n") ;
      exit(1) ;
    }
  if ( t.lar != 2 || t.hau != 2 )
    {
      fprintf(stderr, "Mauvasie taille H_L : %d %d\n",t.hau, t.lar ) ;
      exit(1) ;
    }



  it = codec_informations_table(&infos_codec_H_L_l3_double, &t) ;

  codec_change_taille_table(&it, 100, 100) ;
  for(j=0; j<t.hau; j++)
    for(i=0; i<t.lar; i++)
      t.table[j][i].table[0] = 10 ;


  codec_libere(codec_H_L_l3_double, &t) ;
}


/*
*/

void test_P_H_L()
{
  t_P_H_L_l3_double t ;

  MET_A_0(&t) ;
  codec_sauve_texte(codec_P_H_L_l3_double, &t, "{[(0 0 0\n1 0 0)\n(0 1 0\n1 1 0)]}") ;
  codec_sauve_texte(codec_P_H_L_l3_double, &t, "{[(0 0 0\n1 0 0)\n(0 1 0\n1 1 0)]\n[(0 0 1\n1 0 1)\n(0 1 1\n1 1 1)]}") ;

  codec_libere(codec_P_H_L_l3_double, &t) ;
}

/*
 *
 */
struct test_table_L_hh
{
  int lar ;
  double *a ;
  char pad1[10000] ;
  int *b ;
  char pad2[10000] ;
} ;

TYPE_PHYSIQUE(codec_test_table_L_hh, struct test_table_L_hh,
	      {
		return( codec_L_hh((void**)&a->a, codec_double,
				   (void**)&a->b, codec_int,
				   &a->lar
				   ) ) ;
	      }
	      )
TYPE_PHYSIQUE(codec_test_table_l_hh, struct test_table_L_hh,
	      {
		return( codec_l_hh((void*)&a->a, codec_double,
				   (void*)&a->b, codec_int,
				   3
				   ) ) ;
	      }
	      )

void test_table_L_hh()
{
  struct test_table_L_hh t ;
  struct informations_table it ;
  static Codec cc[] = { codec_test_table_L_hh, codec_test_table_l_hh, NULL } ;
  Codec c ;
  int i ;

  for(i=0; cc[i]; i++)
    {
      c = cc[i] ;
      MET_A_0(&t) ;

      if ( i==0 )
	codec_sauve_texte(c, &t, "(1.1 2.2 3.3 4.4 5.5 6.6)") ;
      else
	codec_sauve_texte(c, &t, "1.1 2.2 3.3 4.4 5.5 6.6") ;

      if ( i==0 && t.lar != 3 )
	EXIT ;
      if ( i==0 && strcmp("(1.1 2 3.3 4 5.5 6)", codec_recupere_texte(c, &t)) )
	EXIT ;
      if ( i==1 && strcmp("1.1 2 3.3 4 5.5 6", codec_recupere_texte(c, &t)) )
	EXIT ;
      
      
      it = codec_informations_table(codec_informations(c), &t) ;
      if ( strcmp( codec_recupere_element_courant(&it), "1.1 2") )
	EXIT ;
      if ( strcmp( codec_recupere_element_courant(&it), "3.3 4") )
	EXIT ;
      if ( strcmp( codec_recupere_element_courant(&it), "5.5 6") )
	EXIT ;
      codec_positionne_element_table(&it, 1) ;
      codec_sauve_element_courant(&it, "10.1 20.2") ;
      if (i==0 && strcmp("(1.1 2 10.1 20 5.5 6)",codec_recupere_texte(c, &t)) )
	EXIT ;
      if (i==1 && strcmp("1.1 2 10.1 20 5.5 6", codec_recupere_texte(c, &t)) )
	EXIT ;

      codec_libere(c, &t) ;
    }
}
/*
 *
 */
struct test_table_H_L_pp
{
  int hau, lar ;
  double **a ;
  int **b ;
} ;

TYPE_PHYSIQUE(codec_test_table_H_L_pp, struct test_table_H_L_pp,
	      {
		return( codec_H_L_pp((void***)&a->a, codec_double,
				     (void***)&a->b, codec_int,
				     &a->hau, &a->lar
				   ) ) ;
	      }
	      )

void test_table_H_L_pp()
{
  struct test_table_H_L_pp t ;
  struct informations_table it ;
  static Codec cc[] = { codec_test_table_H_L_pp, NULL } ;
  Codec c ;
  int i ;

  for(i=0; cc[i]; i++)
    {
      c = cc[i] ;
      MET_A_0(&t) ;

      codec_sauve_texte(c, &t, "[(1.1 2.2 3.3 4.4 5.5 6.6)\n(11.1 12.2 13.3 14.4 15.5 16.6)]") ;

      if ( t.lar != 3 )
	EXIT ;
      if ( t.hau != 2 )
	EXIT ;
      if ( strcmp("[(1.1 2 3.3 4 5.5 6)\n(11.1 12 13.3 14 15.5 16)]", codec_recupere_texte(c, &t)) )
	EXIT ;
      
      it = codec_informations_table(codec_informations(c), &t) ;
      if ( strcmp( codec_recupere_element_courant(&it), "1.1 2") )
	EXIT ;
      if ( strcmp( codec_recupere_element_courant(&it), "3.3 4") )
	EXIT ;
      if ( strcmp( codec_recupere_element_courant(&it), "5.5 6") )
	EXIT ;
      if ( strcmp( codec_recupere_element_courant(&it), "11.1 12") )
	EXIT ;
      if ( strcmp( codec_recupere_element_courant(&it), "13.3 14") )
	EXIT ;

      codec_positionne_element_table(&it, 1,2) ;
      codec_sauve_element_courant(&it, "111.1 222.2") ;
      if (strcmp("[(1.1 2 3.3 4 5.5 6)\n(11.1 12 13.3 14 111.1 222)]",codec_recupere_texte(c, &t)) )
	EXIT ;

      codec_libere(c, &t) ;
    }
}
/*
 *
 */

void test_float()
{
  int a ;
  float b ;
  int nb ;


  codec_sauve_texte(codec_float, &b, "6.5") ;
  if ( b != 6.5 )
    EXIT ;
  if ( codec_recupere_float(codec_float, &b, &nb)[0] != 6.5 )
    EXIT ;
  if ( nb != 1 )
    EXIT ;
  
  codec_sauve_texte(codec_int, &a, "6") ;
  if ( a != 6 )
    EXIT ;
  if ( codec_recupere_float(codec_int, &a, &nb)[0] != 6 )
    EXIT ;
  if ( nb != 1 )
    EXIT ;
}

/*
 *
 */

void test_graphe()
{
  t_graphe_N_l6_float g ;
  int i, j, k, nb_noeuds, nb_aretes ;
  Chaine s ;

  memset(&g, 0, sizeof(g)) ;
  codec_sauve_texte(codec_graphe_N_l6_float, &g, "") ;
  s = s_ouvre_ecriture() ;
  for(i=0;i<100;i++)
    {
      s_efface(&s) ;
      s_printf(&s, "(") ;
      nb_noeuds = rand()%16 ;

      for(j=0;j<nb_noeuds;j++)
	{
	  nb_aretes = rand()%16 ;
	  s_printf(&s, "(") ;
	  for(k=0;k<nb_aretes;k++)
	    s_printf(&s, " %d", rand()%nb_noeuds) ;
	  s_printf(&s, ")\n") ;
	  s_printf(&s, "%g %g %g", (float)rand(), (float)rand(), (float)rand()) ;
	  if ( rand()&1 )
	  s_printf(&s, " %g %g %g", (float)rand(), (float)rand(), (float)rand()) ;
	  s_printf(&s, "\n") ;
	}
      s_printf(&s, ")\n") ;

      codec_sauve_texte(codec_graphe_N_l6_float, &g, s_char(&s)) ;
      codec_sauve_texte(codec_graphe_N_l6_float, &g, s_char(&s)) ;
    }
  codec_libere(codec_graphe_N_l6_float, &g) ;
}

/*
 *
 */

int main()
{
  float p[10000] ;
  int i, j ;
  char *t[] = {
    "1 2.7 3 4\n5 6 7 8 9 10 11 12\n9 sfhkgj 10\n\n11",
    "1 2 3 4 5\n11 22 33 44\n111 222 333\n111 222\n1111\n\n1",
    "[(1 2\n11 22)(3 4)(7 8",
    "1 2\n5 6 7\n8 9 10 11\n\n11 12\n13 14 15\n16 17 18 19\n20 21 22 23 24\n\n100 101\n102",
    "NOEUD(5 FEUILLE(8)\nNOEUD(\n11\nFEUILLE(88) FEUILLE(889)))",
    "(0101(00000000)(11111111)0(1))",
  } ;
  Codec ttt[] = { LISTE } ;
  char *tmp ;
  const Codec_informations *ci ;

  if (0)
    {
      struct bidon bidon ;
      char *tb =
	"["
	"(1 2 3 4\n5 6 7 8\n9 10 11 12\n13 14 15 16\n)\n"
	"(1.1 2.1 3.1 4.1\n5.1 6.1 7.1 8.1\n9.1 10.1 11.1 12.1)\n"
	"(1.2 2.2 3.2 4.2\n5.2 6.2 7.2 8.2\n9.2 10.2 11.2 12.2)\n"
	"]" ;
      const char *xx ;
      int i, j ;

      memset(&bidon, '\0', sizeof(bidon)) ;
      codec_sauve_texte(codec_bidon, &bidon,tb) ;

      for(j=0;j<bidon.hau;j++)
	for(i=0;i<bidon.lar;i++)
	  fprintf(stderr,"[%d,%d] %g %g %g  %g\n",
		  j,i,
		  bidon.triplet[j][i].x,
		  bidon.triplet[j][i].y,
		  bidon.triplet[j][i].z,
		  bidon.poids[j][i] ) ;
		  


      xx = codec_recupere_texte(codec_bidon, &bidon) ;
      fprintf(stderr, "\n%s\n\n", xx) ;
      codec_sauve_texte(codec_bidon, &bidon,xx) ;
      xx = codec_recupere_texte(codec_bidon, &bidon) ;
      fprintf(stderr, "%s", xx) ;
      exit(0) ;
    }
	
  if (0)
    {
      memset(p, '\0', sizeof(p)) ;
      for(i=0;i<10000;i++)
	{
	  codec_sauve_texte(codec_PHL_l3_float, p,"1 2\n5 6 7\n8 9 10 11\n\n11 12\n13 14 15\n16 17 18 19\n20 21 22 23 24\n\n100 101\n102") ;
	  codec_recupere_texte(codec_PHL_l3_float, p) ;
	  
	}
      exit(0) ;
    }

  if (0)
    {
      memset(p, '\0', sizeof(p)) ;
      codec_sauve_texte(codec_L_h4l3_double, p,"([(1 2 0)\n(8 9 10)\n(11 12 0)\n(16 17 18)]\n[(102 0 0)\n(0 0 0)\n(0 0 0)\n(0 0 0)])") ;
      printf("%s\n", codec_recupere_texte(codec_L_h4l3_double, p) ) ;
      exit(0) ;
    }


	

  if (0)
    {
      memset(p, '\0', sizeof(p)) ;
      codec_sauve_texte(codec_H_L_double, p,"1 2\n5 6 7\n8 9 10 11\n\n11 12\n13 14 15\n16 17 18 19\n20 21 22 23 24\n\n100 101\n102" ) ;
      fprintf(stderr,"--------------------------------\n") ;
      memset(p, '\0', sizeof(p)) ;
      codec_sauve_texte(codec_H_L_l4_double, p,"1 2\n5 6 7\n8 9 10 11\n\n11 12\n13 14 15\n16 17 18 19\n20 21 22 23 24\n\n100 101\n102" ) ;
      exit(0) ;
    }

  test_basique() ;
  test_table() ;
  test_table2() ;
  test_table3() ;
  test_table_L_hh() ;
  test_table_H_L_pp() ;
  test_H_L() ;
  test_P_H_L() ;
  test_float() ;
  test_graphe() ;

  printf
    ("<STYLE>\n"
     "TD { font-size: 50%% ; font-family: times; font-style: fixed ; vertical-align: left ; }\n"
     "P { background: #FFFF00 ; }\n"
     "</STYLE>\n"
     ) ;

  printf("<TABLE BORDER=1 CELLPADDING=0 CELLSPACING=1>\n<TR><TD>\n") ;
  for( j = 0 ; j < sizeof(t)/sizeof(t[0]) ; j++ )
    {

      printf("<TD><B>") ;
      chaine(t[j]) ;
    }

  for( i = 0 ; i < sizeof(ttt)/sizeof(ttt[0]) ; i++ )
    {
      ci = codec_informations(ttt[i]) ;
      printf("<TR><TD>%03d %s</TD>\n", ci->taille, ci->nom) ;

      memset(p, '\0', sizeof(p)) ;	/* A cause du changement de type */
      for( j = 0 ; j < sizeof(t)/sizeof(t[0]) ; j++ )
	{
	  printf("<TD>") ;
	  /*
	   * Sauvegarde recuperation du texte
	   */
	  codec_sauve_texte(ttt[i], p, t[j]) ;
	  tmp = strdup(codec_recupere_texte(ttt[i], p)) ;
	  chaine(tmp) ;
	  /*
	   * Sauvegarde recuperation du texte modifie
	   * Erreur sir probleme
	   */
	  codec_libere(ttt[i], p) ;
	  if ( nbzm() != 0 )
	      fprintf(stderr, "globale_codec_malloc = %d dans %s\n",
		      nbzm(),
		      ci->nom) ;
	  codec_sauve_texte(ttt[i], p, tmp) ;
	  if ( strcmp( codec_recupere_texte(ttt[i], p), tmp ) )
	    {
	      fprintf(stderr, "Erreur 1\n") ;
	      printf("<P>") ;
	      chaine(codec_recupere_texte(ttt[i], p)) ;
	      printf("</P>\n") ;
	    }
	  /*
	   * Idem mais sans liberation du champ avant
	   */
	  codec_sauve_texte(ttt[i], p, tmp) ;
	  if ( strcmp( codec_recupere_texte(ttt[i], p), tmp ) )
	    {
	      fprintf(stderr, "Erreur 2 <!-- orig = %s -->\n", t[j]) ;
	      printf("<P>") ;
	      chaine(codec_recupere_texte(ttt[i], p)) ;
	      printf("</P>\n") ;
	    }
	  codec_libere(ttt[i], p) ;
	  if ( nbzm() != 0 )
	      fprintf(stderr, "globale_codec_malloc = %d dans %s\n",
		      nbzm(),
		      ci->nom) ;

	  free(tmp) ;
	  printf("</TD>\n") ;
	}
      printf("</TR>\n") ;
    }
  printf("</TABLE>") ;




  return(0) ;
}


