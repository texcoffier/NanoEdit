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
#include "o_objet.h"
#include "t_geometrie.h"
#include "u_graphique.h"
#include "u_table.h"
#include "s_string.h"
#include "u_etat_courant.h"
#include <GL/gl.h>
#include "stl.h"

/*
 * Ceci doit être complètement réécrit.
 * Il faut pouvoir travailler de manière transparente
 * au niveau de la facette et non du sommet.
 */

/*
 * Je suppose dans le code que FACETTE_HAUT ==> RACCORDE_GD
 * sinon c'est vraiment trop compliqué dans l´état actuel
 */


/*
 ******************************************************************************
 * Affichage générique de table de points
 ******************************************************************************
 */

const char* recupere_poids(struct informations_table *it)
{
  int i ;
  const char *pc ;

  pc = codec_recupere_element_courant(it) ;
  for(i=strlen(pc)-1; i>=0; i--)
    if ( pc[i] == ' ' )
      break ;
  if ( i == 0 )
    return( pc ) ;
  else
    return( &pc[i+1] ) ;
}

const char * lit_tag_table(const void *o, const Champ *ch)
{
  int i, j ;
  static Chaine s ;
  struct informations_table it ;
  const Champ *c ;

  i = lit_numero_tag(ch) ;
  j = i%NB_TAG_PLAGE ;
  i -= j ;
  c = c_trouve(o,"t") ;
  if ( c == NULL )
    return("L'objet n'est pas éditable (Voir option de 'operateur'") ;

  it = codec_informations_table(c->tp, PTR(o,c)) ;
  if ( j>=0 && j<it.taille[0] )
    {
      codec_positionne_element_table(&it, j) ;
      switch(i)
	{
	case TAG_DEPLACEMENT:
	  return( codec_recupere_element_courant(&it) ) ;
	case TAG_POIDS:
	  return( recupere_poids(&it) ) ;
	case TAG_DESTRUCTION:
	  s_efface(&s) ;
	  s_printf(&s,"Cliquer dessus pour\n"
		   "détruire le point numéro %d",
		   i) ;
	  return(s_char(&s)) ;
	}
    }
  return("") ;
}

/*
 * Change la dernière valeur dans la chaine
 */

void change_poids(struct informations_table *it, const char *v)
{
  struct informations_table it2 ;
  const char *pt ;
  char *nouvelle ;
  int pos_pt ;

  it2 = *it ;
  pt = codec_recupere_element_courant(it) ;
  pos_pt = strlen(pt) ;
  /*
   * La '(' à cause de la notation du poids ou homogène
   */
  while ( pos_pt>0 && pt[pos_pt]!=' ' && pt[pos_pt]!='(' )
    pos_pt-- ;


  ALLOUER(nouvelle, strlen(pt) + strlen(v) + 3 ) ;
  memcpy(nouvelle, pt, pos_pt+1 ) ;
  strcpy(nouvelle + pos_pt + 1, v) ;
  if ( pt[pos_pt]=='(' )
    strcat(nouvelle, ")") ;
  codec_sauve_element_courant(&it2, nouvelle) ;
  free(nouvelle) ;
}

/*
 * change les première valeurs de la chaine
 */

void change_debut(struct informations_table *it, const char *v)
{
  struct informations_table it2 ;
  const char *pt ;
  char *nouvelle ;
  int nb_mots ;

  /*
   * compte le nombre de mots
   */
  for(pt=v, nb_mots=1 ; pt ; )
    {
      pt = strchr(pt, ' ') ;
      if ( pt )
	{
	  pt += strspn(pt, " \t") ;
	  if ( *pt != '\0' )
	    nb_mots++ ;
	}
    }
  /*
   * Récupère ancienne valeur
   */
  it2 = *it ;
  pt = codec_recupere_element_courant(it) ;
  while(nb_mots-- && pt)
    {
      pt = strchr(pt, ' ') ;
      if ( pt )
	pt += strspn(pt, " \t") ;
    }
  /*
   * Sauvegarde nouvelle
   */
  if ( pt )
    {
      ALLOUER(nouvelle, strlen(pt) + strlen(v) + 2 ) ;
      sprintf(nouvelle, "%s %s", v, pt) ;
      codec_sauve_element_courant(&it2, nouvelle) ;
      free(nouvelle) ;
    }
  else
    {
      /*
       * tout est remplacé par la nouvelle valeur
       */
      codec_sauve_element_courant(&it2, v) ;
    }
}


void ecrit_tag_table(void *o, const Champ *ch, const char *vv)
{
  int i, j, k, num_fils ;
  struct informations_table it ;
  const Champ *c ;
  char *v ;

  v = strdup(vv) ;
  if ( strstr(v," (") )
    strstr(v," (")[0] = '\0' ; /* Enlève direction regard */

  i = lit_numero_tag(ch) ;
  j = i%NB_TAG_PLAGE ;
  i -= j ;
  c = c_trouve(o,"t") ;
  if ( c == NULL )
    return ;
  it = codec_informations_table(c->tp, PTR(o,c)) ;
  if ( i == TAG_SPLIT )
    {
      if ( j>=0 && j<=it.taille[0] )
	{
	  Triplet p, q ;

	  if ( j != 0 && j != it.taille[0])
	    {
	      codec_positionne_element_table(&it, j-1) ;
	      p = char_vers_triplet(codec_recupere_element_courant(&it)) ;
	      q = char_vers_triplet(codec_recupere_element_courant(&it)) ;
	      p = u_milieu_2_triplet(&p, &q) ;
	      k = j ;
	    }
	  else if ( j == 0 )
	    {
	      codec_positionne_element_table(&it, 0) ;
	      p = char_vers_triplet(codec_recupere_element_courant(&it)) ;
	      q = char_vers_triplet(codec_recupere_element_courant(&it)) ;
	      p = u_barycentre_2_triplet(&p, -0.5, &q) ;
	      k = 0 ;
	    }
	  else
	    {
	      codec_positionne_element_table(&it, it.taille[0] - 2) ;
	      p = char_vers_triplet(codec_recupere_element_courant(&it)) ;
	      q = char_vers_triplet(codec_recupere_element_courant(&it)) ;
	      p = u_barycentre_2_triplet(&p, 1.5, &q) ;
	      k = it.taille[0] ;
	    }
	  
	  codec_positionne_element_table(&it,
					 j == it.taille[0] ?it.taille[0]-1:j) ;
	  codec_duplique_element_courant(&it) ;
	  codec_positionne_element_table(&it, k) ;
	  codec_sauve_element_courant(&it, triplet_vers_char(&p)) ;
	  NOTE_CHAMP_CHANGE_(o,c) ;
	}
    }
  else
  if ( j>=0 && j<it.taille[0] )
    {
      codec_positionne_element_table(&it, j) ;
      switch(i)
	{
	case TAG_DEPLACEMENT:
	  if ( TRACE(o) )
	    eprintf("ecrit tag numéro %d, valeur %s\n", j, v) ;
	  change_debut(&it, v) ;
	  break ;
	case TAG_POIDS:
	  change_poids(&it, v) ;
	  break ;
	case TAG_DESTRUCTION:
	  codec_detruit_element_courant(&it) ;
	  /*
	   * Enleve le fils s'il y a
	   */
	  num_fils = 0 ;
	  POUR_LES_FILS(o, f,
			{
			  if ( num_fils == j )
			    {
			      o_destruction(f) ;
			      break ;
			    }
			  num_fils++ ;
			}
			) ;
	  break ;
	}
      NOTE_CHAMP_CHANGE_(o,c) ;
    }
  free(v) ;
}

int recupere_point_poids(struct informations_table *it, Triplet *p, Flottant *poids)
{
  const float *pf ;
  int i, nb, old[2] ;

  old[0] = it->courant[0] ;
  old[1] = it->courant[1] ;
  pf = codec_recupere_element_courant_float(it, &nb) ;
  if ( nb==1 )
    {
      for(i=0;i<2;i++)
	{
	  if ( it->taille[i]>1 )
	    (&p->x)[1-i] = old[i]/(float)(it->taille[i]-1) ;
	  else
	    (&p->x)[1-i] = 0 ;
	}
      (&p->x)[2] = pf[0] ;
    }
  else
    {
      p->x = pf[0] ;
      p->y = pf[1] ;
      p->z = pf[2] ;
      if ( poids && nb>=4 )
	*poids = pf[3] ;
    }
  return(nb) ;
}

int recupere_point(struct informations_table *it, Triplet *p)
{
  return(recupere_point_poids(it, p, NULL)) ;
}

void t_table_general(const void *t, const Codec_informations *tp, int comment)
{
  int i, nb ;
  struct informations_table it ;
  Triplet p1, p2, p3, n ;

  it = codec_informations_table(tp, t) ;
  if ( it.taille[0] == 0 )
    return ;

  if ( comment & T_FACETTE )
    {
      codec_positionne_element_table(&it,0) ;
      if ( comment & T_CULL_FACE )
	glEnable(GL_CULL_FACE) ;
      if ( it.taille[0] >= 3 )
	{
	  recupere_point(&it, &p1) ;
	  recupere_point(&it, &p2) ;
	  recupere_point(&it, &p3) ;
	  n = u_normale_triangle(&p1, &p2, &p3) ;

	  glEnable(GL_LIGHTING) ;
	  glBegin(GL_POLYGON) ;
	  glNormal3dv((GLdouble*)&n) ;
	  codec_positionne_element_table(&it,0) ;
	  for(i=0; i<it.taille[0]; i++)
	    {
	      recupere_point(&it, &p1) ;
	      glVertex3dv((GLdouble*)&p1) ;
	    }
	  glEnd() ;
	  glDisable(GL_LIGHTING) ;
	}
      if ( comment & T_CULL_FACE )
	glDisable(GL_CULL_FACE) ;
    }
  if ( comment & T_POLYGONE )
    {
      codec_positionne_element_table(&it,0) ;
      glBegin(GL_LINE_LOOP) ;
      for(i=0; i<it.taille[0]; i++)
	{
	  recupere_point(&it, &p1) ;
	  glVertex3dv((GLdouble*)&p1) ;
	}
      glEnd() ;
    }
  if ( comment & T_FILAIRE )
    {
      codec_positionne_element_table(&it,0) ;
      glBegin(GL_LINE_STRIP) ;
      for(i=0; i<it.taille[0]; i++)
	{
	  recupere_point(&it, &p1) ;
	  glVertex3dv((GLdouble*)&p1) ;
	}
      glEnd() ;
    }
  if ( comment & T_PONCTUEL )
    {
      codec_positionne_element_table(&it,0) ;
      glBegin(GL_POINTS) ;
      for(i=0; i<it.taille[0]; i++)
	{
	  recupere_point(&it, &p1) ;
	  glVertex3dv((GLdouble*)&p1) ;
	}
      glEnd() ;
    }
  if ( comment & (T_TAG|T_TAG_DESTRUCTION|T_TAG_POIDS) )
    {
      codec_positionne_element_table(&it,0) ;
      for(i=0; i<it.taille[0]; i++)
	{
	  nb = recupere_point(&it, &p1) ;

	  if ( comment & T_TAG )
	    {
	      if ( i == 0 )
		t_tag_origine_premier(TAG_DEPLACEMENT, &p1) ;
	      else
		t_tag_origine(TAG_DEPLACEMENT+i, &p1) ;
	    }
	  if ( comment & T_TAG_DESTRUCTION )
	    t_tag_destruction(TAG_DESTRUCTION+i, &p1) ;
	  if ( nb>=4 && (comment & T_TAG_POIDS) )
	    t_tag_poids(TAG_POIDS+i, &p1) ;
	  if ( i != 0 && (comment & T_TAG_SPLIT) )
	    {
	      p3 = u_milieu_2_triplet(&p1, &p2) ;
	      t_tag_dilatation(TAG_SPLIT+i, &p3) ;
	      if ( i == 1 )
		{
		  p3 = u_barycentre_2_triplet(&p1, 1.5, &p2) ;
		  t_tag_dilatation(TAG_SPLIT, &p3) ;
		}
	      if ( i == it.taille[0]-1 )
		{
		  p3 = u_barycentre_2_triplet(&p1, -0.5, &p2) ;
		  t_tag_dilatation(TAG_SPLIT+i+1, &p3) ;
		}
	    }
	  p2 = p1 ;
	}
    }
}

void t_table(const void *o, const Champ *ch, int comment)
{
  t_table_general(&T(Table, o, ch), ch->tp, comment) ;
}

void t_table_pov_general(const void *t, const Codec_informations *tp
			 ,int comment , FILE *f)
{
  int i ;
  struct informations_table it ;
  Triplet pt, pt2 ;

  it = codec_informations_table(tp, t) ;
  if ( it.taille[0] == 0 )
    return ;

  if ( comment & T_FACETTE )
    {
      if ( it.taille[0] >= 3 )
	{
	  codec_positionne_element_table(&it,0) ;
	  fprintf(f, "polygon { %d", it.taille[0]+1) ;
	  for(i=0;i<=it.taille[0] ; i++) /* OUI <= pour le fermer */
	    {
	      recupere_point(&it, &pt) ;
	      fprintf(f, ",<%g,%g,%g>\n"
		      , pt.x, pt.y, pt.z
		      ) ;
	    }
	  fprintf(f,"}\n") ;
	}
    }
  if ( comment & (T_FILAIRE|T_POLYGONE) )
    {
      for(i=0  ;  i< it.taille[0] - ((comment&T_POLYGONE)?0:1)  ;  i++)
	{
	  codec_positionne_element_table(&it,i) ;
	  recupere_point(&it, &pt) ;
	  recupere_point(&it, &pt2) ;
	  fprintf(f, "cylinder { <%g,%g,%g>, <%g,%g,%g>, %g }\n"
		  , pt.x, pt.y, pt.z
		  , pt2.x, pt2.y, pt2.z
		  , globale_etat_courant.epaisseur_trait
		  ) ;
	}
    }
  if ( comment & T_PONCTUEL )
    {
      codec_positionne_element_table(&it,0) ;
      for(i=0;i<it.taille[0] ; i++)
	{
	  recupere_point(&it, &pt) ;
	  fprintf(f, "sphere { <%g,%g,%g>, %g }\n"
		  , pt.x, pt.y, pt.z
		  , globale_etat_courant.epaisseur_trait
		  ) ;
	}
    }
}

void t_table_pov(const void *o, const Champ *ch, int comment, const char *urgent)
{
  t_table_pov_general(&T(Table, o, ch)
		      , ch->tp
		      , comment
		      , (FILE*)atol(urgent)
		      ) ;
}


void t_table_stl_general(const void *t, const Codec_informations *tp
			 ,int comment , FILE *f)
{
  int i ;
  struct informations_table it ;
  Triplet p1, p2, p3, pt, pt2, n, m ;

  it = codec_informations_table(tp, t) ;
  if ( it.taille[0] == 0 )
    return ;

  if ( comment & T_FACETTE )
    {
      if ( it.taille[0] >= 3 )
	{
	  codec_positionne_element_table(&it,0) ;
	  recupere_point(&it, &p1) ;
	  recupere_point(&it, &p2) ;
	  recupere_point(&it, &p3) ;
	  n = u_normale_triangle(&p1, &p2, &p3) ;
	  n = u_normalise_triplet(&n) ;
	  n = u_triplet_fois_flottant(&n, globale_etat_courant.stl_offset/2);
	  m = u_triplet_fois_flottant(&n, -1) ;
	  codec_positionne_element_table(&it,0) ;
	  t_debut_polygone_stl(f) ;
	  for(i=0;i<=it.taille[0] ; i++) /* OUI <= pour le fermer */
	    {
	      recupere_point(&it, &p1) ;
	      p2 = u_ajoute_triplet(&p1, &n) ;
	      p3 = u_ajoute_triplet(&p1, &m) ;
	      t_point_stl_offset(&p2, &p3) ;
	    }
	  t_fin_polygone_stl_offset() ;
	}
    }
  if ( comment & (T_FILAIRE|T_POLYGONE) )
    {
      for(i=0  ;  i< it.taille[0] - ((comment&T_POLYGONE)?0:1)  ;  i++)
	{
	  codec_positionne_element_table(&it,i) ;
	  recupere_point(&it, &pt) ;
	  recupere_point(&it, &pt2) ;
	  t_cylindre_stl(f, &pt, &pt2, globale_etat_courant.stl_offset/2
			  , 8) ;
	}
    }
  if ( comment & T_PONCTUEL )
    {
      codec_positionne_element_table(&it,0) ;
      for(i=0;i<it.taille[0] ; i++)
	{
	  recupere_point(&it, &pt) ;
	  t_sphere_stl(f, &pt, globale_etat_courant.stl_offset/2, 4, 8) ;
	}
    }
}

void t_table_stl(const void *o, const Champ *ch, int comment, const char *urgent)
{
  t_table_stl_general(&T(Table, o, ch)
		      , ch->tp
		      , comment
		      , (FILE*)atol(urgent)
		      ) ;
}

/*
 *
 */

void changement_table(void *o, const Champ *ch)
{
  int i ;
  const Champ *c ;
  struct informations_table it ;
  void *table ;
  char *temp ;

  table = PTR(o, ch) ;
  it = codec_informations_table(ch->tp, table) ;

  if ( FILS_CHANGE(o) )
    {
      if ( TRACE(o) )
	eprintf("Prend les valeurs des fils et les met dans la table\n"
		"Ancienne = (%s)\n", c_recupere_sans_copie(o,"t")
		) ;
      i = 0 ;
      POUR_LES_FILS
	(o, f,
	 if ( c_trouve_type((const void**)&f, &c, ch->type_dans_fils) )
	 i++ ;
	 ) ;
      codec_change_taille_table(&it, i) ;
      POUR_LES_FILS
	(o, f,
	 if ( c_trouve_type((const void**)&f, &c, ch->type_dans_fils) )
	    codec_sauve_element_courant(&it, c_recupere_ici_sans_copie(f,c)) ;
	 ) ;
      if ( TRACE(o) )
	eprintf("Nouvelle = (%s)\n", c_recupere_sans_copie(o,"t")) ;
    }
  else
  if ( CHAMP_CHANGE_(OBJET(o), ch) || CA_CHANGE(o) )
    {
      if ( TRACE(o) )
	eprintf("Prend la table et met à jour les valeurs des fils\n") ;
      i = 0 ;
      POUR_LES_FILS
	(o, f,
	 {
	   if ( c_trouve_type((const void**)&f, &c, ch->type_dans_fils) )
	     {
	       if ( i++==it.taille[0] ) break ;
	       /* Duplication obligatoire car c_sauve_ici fait une
		* relecture pour savoir si cela a changé */
	       temp = strdup(codec_recupere_element_courant(&it)) ;
	       c_sauve_ici(f, c, temp) ;
	       free(temp) ;
	     }
	   }) ;
    }
}
  
/*
 ******************************************************************************
 * Affichage générique de grille de points
 ******************************************************************************
 */
int recupere_point_ji(struct informations_table *it, int j, int i, Triplet *p)
{
  codec_positionne_element_table(it
				 , (j+it->taille[0])%it->taille[0]
				 , (i+it->taille[1])%it->taille[1]
				 ) ;
  return( recupere_point(it, p) ) ;
}

static int globale_moyenne_nb ;
static Triplet globale_moyenne ;

static void debut_moyenne()
{
  globale_moyenne_nb = 0 ;
  u_annule_triplet(&globale_moyenne) ;
}

static void ajoute_moyenne_normale_facette_poids(const Triplet *a
					   , const Triplet *b
					   , const Triplet *c
					   , Flottant poids
					   )
{
  Triplet u, v, n ;

  u = u_soustrait_triplet(b, a) ;
  v = u_soustrait_triplet(c, a) ;
  n = u_pv_triplet(&u, &v) ;
  n = u_normalise_triplet(&n) ;
  n = u_triplet_fois_flottant(&n, poids) ;
  globale_moyenne = u_ajoute_triplet(&globale_moyenne, &n) ;
  globale_moyenne_nb++ ;
}

static void ajoute_moyenne_normale_facette(const Triplet *a
					   , const Triplet *b
					   , const Triplet *c
					   )
{
  ajoute_moyenne_normale_facette_poids(a, b, c, 1.) ;
}

static Triplet fin_moyenne()
{
  Triplet n ;

  if ( globale_moyenne_nb )
    n = u_triplet_fois_flottant(&globale_moyenne, 1./globale_moyenne_nb) ;
  else
    u_annule_triplet(&n) ;
  return(n) ;
}


int recupere_point_ji_(struct informations_table *it, int j, int i, Triplet *p)
{
  int res ;

  res = recupere_point_ji(it, j, i, p) ;
  *((Triplet*)p) = u_transforme(&globale_etat_courant.transfo, (Triplet*)p) ;

  return( res ) ;
}

static Triplet normale_moyenne(struct informations_table *mt, int j, int i
			    , int comment)
{
  Triplet centre, gauche, droite, haut, bas ;
  const Flottant pdg = 1.7 ;	/* Poids de la normales des facettes GDHB */
  int k ;

  j = (j+mt->taille[0])%mt->taille[0] ;
  i = (i+mt->taille[1])%mt->taille[1] ;

  debut_moyenne() ;

  recupere_point_ji_(mt, j  ,i  , &centre) ;
  recupere_point_ji_(mt, j  ,i+1, &droite) ;
  recupere_point_ji_(mt, j  ,i-1, &gauche) ;
  recupere_point_ji_(mt, j+1,i  , &haut) ;
  recupere_point_ji_(mt, j-1,i  , &bas) ;

  if ( j+1 == mt->taille[0] )
    {
      if ( comment&T_DEGENERE_HAUT )
	{
	  recupere_point_ji_(mt, j-1, 0, &bas) ;
	  for(k=1; k<=mt->taille[1]; k++)
	    {
	      recupere_point_ji_(mt, j-1, k, &droite) ;
	      ajoute_moyenne_normale_facette(&centre, &bas, &droite) ;
	      bas = droite ;
	    }
	  return( fin_moyenne() ) ;
	}
      if ( comment&T_FACETTE_HAUT )
	{
	  ajoute_moyenne_normale_facette(&centre, &gauche, &bas   ) ;
	  ajoute_moyenne_normale_facette(&centre, &bas   , &droite) ;
	  ajoute_moyenne_normale_facette_poids(&centre, &droite, &gauche,pdg) ;
	  return( fin_moyenne() ) ;
	}
      if ( ! (comment&T_RACCORDE_HB) )
	{
	  if ( i+1 != mt->taille[1] )
	    ajoute_moyenne_normale_facette(&centre, &bas, &droite  ) ;
	  if ( i != 0 )
	    ajoute_moyenne_normale_facette(&centre, &gauche, &bas  ) ;
	  return( fin_moyenne() ) ;
	}
    }
  if ( i+1 == mt->taille[1] )
    {
      if ( comment&T_DEGENERE_DROITE )
	{
	  recupere_point_ji_(mt, 0, i-1, &gauche) ;
	  for(k=1; k<=mt->taille[0]; k++)
	    {
	      recupere_point_ji_(mt, k, i-1, &haut) ;
	      ajoute_moyenne_normale_facette(&centre, &haut, &gauche) ;
	      gauche = haut ;
	    }
	  return( fin_moyenne() ) ;
	}
      if ( comment&T_FACETTE_DROITE )
	{
	  ajoute_moyenne_normale_facette(&centre, &haut  , &gauche) ;
	  ajoute_moyenne_normale_facette(&centre, &gauche, &bas   ) ;
	  ajoute_moyenne_normale_facette_poids(&centre, &bas   , &haut,pdg  ) ;
	  return( fin_moyenne() ) ;
	}
      if ( ! (comment&T_RACCORDE_GD) )
	{
	  if ( j+1 != mt->taille[0] )
	    ajoute_moyenne_normale_facette(&centre, &haut, &gauche  ) ;
	  if ( j != 0 )
	    ajoute_moyenne_normale_facette(&centre, &gauche, &bas  ) ;
	  return( fin_moyenne() ) ;
	}
    }
  if ( j == 0 )
    {
      if ( comment&T_DEGENERE_BAS )
	{
	  recupere_point_ji_(mt, 1, 0, &haut) ;
	  for(k=1; k<=mt->taille[1]; k++)
	    {
	      recupere_point_ji_(mt, 1, k, &droite) ;
	      ajoute_moyenne_normale_facette(&centre, &droite, &haut) ;
	      haut = droite ;
	    }
	  return( fin_moyenne() ) ;
	}
      if ( comment&T_FACETTE_BAS )
	{
	  ajoute_moyenne_normale_facette(&centre, &droite, &haut  ) ;
	  ajoute_moyenne_normale_facette(&centre, &haut  , &gauche) ;
	  ajoute_moyenne_normale_facette_poids(&centre, &gauche, &droite,pdg) ;
	  return( fin_moyenne() ) ;
	}
      if ( ! (comment&T_RACCORDE_HB) )
	{
	  if ( i+1 != mt->taille[1] )
	    ajoute_moyenne_normale_facette(&centre, &droite, &haut  ) ;
	  if ( i != 0 )
	    ajoute_moyenne_normale_facette(&centre, &haut, &gauche  ) ;
	  return( fin_moyenne() ) ;
	}
    }
  if ( i == 0 )
    {
      if ( comment&T_DEGENERE_GAUCHE )
	{
	  recupere_point_ji_(mt, 0, 1, &droite) ;
	  for(k=1; k<=mt->taille[0]; k++)
	    {
	      recupere_point_ji_(mt, k, 1, &haut) ;
	      ajoute_moyenne_normale_facette(&centre, &droite, &haut) ;
	      droite = haut ;
	    }
	  return( fin_moyenne() ) ;
	}
      if ( comment&T_FACETTE_GAUCHE )
	{
	  ajoute_moyenne_normale_facette(&centre, &droite, &haut  ) ;
	  ajoute_moyenne_normale_facette(&centre, &bas   , &droite) ;
	  ajoute_moyenne_normale_facette_poids(&centre, &haut  , &bas,pdg   ) ;
	  return( fin_moyenne() ) ;
	}
      if ( ! (comment&T_RACCORDE_GD) )
	{
	  if ( j+1 != mt->taille[0] )
	    ajoute_moyenne_normale_facette(&centre, &droite, &haut  ) ;
	  if ( j != 0 )
	    ajoute_moyenne_normale_facette(&centre, &bas, &droite  ) ;
	  return( fin_moyenne() ) ;
	}
    }
  ajoute_moyenne_normale_facette(&centre, &droite, &haut  ) ;
  ajoute_moyenne_normale_facette(&centre, &haut  , &gauche) ;
  ajoute_moyenne_normale_facette(&centre, &gauche, &bas   ) ;
  ajoute_moyenne_normale_facette(&centre, &bas   , &droite) ;
  return( fin_moyenne() ) ;
}

void point_et_normale(struct informations_table *mt,
		      struct informations_table *mtnor,
		      int j, int i, int comment)
{
  Triplet p ;

  if ( mtnor->dimension )
    recupere_point_ji(mtnor, j, i, &p) ;
  else
    p = normale_moyenne(mt, j, i, comment) ;

  glNormal3dv((GLdouble*)&p) ;
  recupere_point_ji(mt, j, i, &p) ;
  t_triplet(&p) ;
}

void t_grille_general(const void *t, const Codec_informations *tp
		      , int comment, const char *urgent,
		      const void *nor, const Codec_informations *tpnor)
{
  int i, j, k, nb ;
  struct informations_table mt, mtnor ;
  Triplet pt, p1, p2, p3, p4,  n ;

  if ( comment == 0 )
    return ;

  mt = codec_informations_table(tp, t) ;
  if ( comment & T_LISSAGE )
    {
      if ( nor && tpnor )
	mtnor = codec_informations_table(tpnor, nor) ;
      else
	mtnor.dimension = 0 ;
    }

  if ( mt.taille[0] == 0 || mt.taille[1] == 0 )
    return ;

  k = '5' - urgent[0] ;
  if ( k < 0 )
    k = 0 ;
  k = 1<<(2*k) ; /* Normale 1,Mauvais 4,Tres mauvais 16,Inacceptable 64 */

  if ( comment & T_FACETTE )
    {
      if ( comment & T_CULL_FACE )
	glEnable(GL_CULL_FACE) ;
      glEnable(GL_LIGHTING) ;
      if ( comment & T_LISSAGE )
	{
	  for(j=0;j<mt.taille[0]-k;j+=k)
	    {
	      glBegin(GL_TRIANGLE_STRIP) ;
	      for(i=0;i<mt.taille[1];i+=k)
		{
		  point_et_normale(&mt, &mtnor, j+k, i, comment) ;
		  point_et_normale(&mt, &mtnor, j, i, comment) ;
		}
	      if ( comment & T_RACCORDE_GD )
		{
		  point_et_normale(&mt, &mtnor, j+k, 0, comment) ;
		  point_et_normale(&mt, &mtnor, j, 0, comment) ;
		}
	      glEnd() ;
	    }
	  if ( comment & T_RACCORDE_HB )
	    {
	      glBegin(GL_TRIANGLE_STRIP) ;
	      for(i=0;i<mt.taille[1];i+=k)
		{
		  point_et_normale(&mt, &mtnor, 0, i, comment) ;
		  point_et_normale(&mt, &mtnor, mt.taille[0]-1, i, comment) ;
		}
	      if ( comment & T_RACCORDE_GD )
		{
		  point_et_normale(&mt, &mtnor, 0, 0, comment) ;
		  point_et_normale(&mt, &mtnor, mt.taille[0]-1, 0, comment) ;
		}
	      glEnd() ;
	    }
	}
      else
	{
	  glBegin(GL_TRIANGLES) ;
	  for(j=0;j<mt.taille[0]-k;j+=k)
	    {
	      for(i=0;i<mt.taille[1]-k;i+=k)
		{
		  recupere_point_ji(&mt, j  , i+k, &p1) ;
		  recupere_point_ji(&mt, j+k, i+k, &p2) ;
		  recupere_point_ji(&mt, j  , i  , &p3) ;
		  recupere_point_ji(&mt, j+k, i  , &p4) ;
		  
		  if ( (!(comment&T_DEGENERE_DROITE) || i!=mt.taille[1]-2 )
		       && (!(comment&T_DEGENERE_BAS) || j!=0))
		    t_triangle(&p1, &p2, &p3) ;
		  
		  if ( (!(comment&T_DEGENERE_GAUCHE) || i!=0)
		       && (!(comment&T_DEGENERE_HAUT) || j!=mt.taille[0]-2)
		       )
		    t_triangle(&p2, &p4, &p3) ;
		}
	      if ( comment & T_RACCORDE_GD )
		{
		  recupere_point_ji(&mt, j  , 0  , &p1) ;
		  recupere_point_ji(&mt, j+k, 0  , &p2) ;
		  recupere_point_ji(&mt, j  , i  , &p3) ;
		  recupere_point_ji(&mt, j+k, i  , &p4) ;
		  t_triangle(&p1, &p2, &p3) ;
		  t_triangle(&p2, &p4, &p3) ;
		}	      
	    }
	  if ( comment & T_RACCORDE_HB )
	    {
	      for(i=0;i<mt.taille[1]-k;i+=k)
		{
		  recupere_point_ji(&mt, -1, i+k, &p1) ;
		  recupere_point_ji(&mt,  0, i+k, &p2) ;
		  recupere_point_ji(&mt, -1, i  , &p3) ;
		  recupere_point_ji(&mt,  0, i  , &p4) ;
		  
		  if ( (!(comment&T_DEGENERE_DROITE) || i!=mt.taille[1]-2 )
		       && (!(comment&T_DEGENERE_BAS) || j!=0))
		    t_triangle(&p1, &p2, &p3) ;
		  
		  if ( (!(comment&T_DEGENERE_GAUCHE) || i!=0)
		       && (!(comment&T_DEGENERE_HAUT) || j!=mt.taille[0]-2)
		       )
		    t_triangle(&p2, &p4, &p3) ;
		}
	      if ( comment & T_RACCORDE_GD )
		{
		  recupere_point_ji(&mt, -1, 0  , &p1) ;
		  recupere_point_ji(&mt,  0, 0  , &p2) ;
		  recupere_point_ji(&mt, -1, i  , &p3) ;
		  recupere_point_ji(&mt,  0, i  , &p4) ;
		  t_triangle(&p1, &p2, &p3) ;
		  t_triangle(&p2, &p4, &p3) ;
		}	      
	    }
	  glEnd() ;
	}
      if ( (comment & T_FACETTE_HAUT) && mt.taille[1]>2 )
	{
	  recupere_point_ji(&mt, -1, 0, &p1) ;
	  recupere_point_ji(&mt, -1, 1, &p2) ;
	  recupere_point_ji(&mt, -1, 2, &p3) ;
	  n = u_normale_triangle(&p1, &p2, &p3) ;
	  glNormal3f(n.x, n.y, n.z) ;
	  glBegin(GL_POLYGON) ;
	  for(i=0;i<mt.taille[1];i+=k)
	    {
	      recupere_point_ji(&mt, -1, i, &p4) ;
	      t_triplet(&p4) ;
	    }
	  if ( comment & T_RACCORDE_GD )
	    {
	      recupere_point_ji(&mt, -1, 0, &p4) ;
	      t_triplet(&p4) ;
	    }
	  glEnd() ;
	}
      if ( (comment & T_FACETTE_BAS) && mt.taille[1]>2 )
	{
	  recupere_point_ji(&mt, 0, 2, &p1) ;
	  recupere_point_ji(&mt, 0, 1, &p2) ;
	  recupere_point_ji(&mt, 0, 0, &p3) ;
	  n = u_normale_triangle(&p1, &p2, &p3) ;
	  glNormal3f(n.x, n.y, n.z) ;
	  glBegin(GL_POLYGON) ;
	  for(i=mt.taille[1]-1;i>=0;i-=k)
	    {
	      recupere_point_ji(&mt, 0, i, &p4) ;
	      t_triplet(&p4) ;
	    }
	  if ( comment & T_RACCORDE_GD )
	    {
	      recupere_point_ji(&mt, 0, 0, &p4) ;
	      t_triplet(&p4) ;
	    }
	  glEnd() ;
	}
      if ( (comment & T_FACETTE_GAUCHE) && mt.taille[0]>2 )
	{
	  recupere_point_ji(&mt, 0, 0, &p1) ;
	  recupere_point_ji(&mt, 1, 0, &p2) ;
	  recupere_point_ji(&mt, 2, 0, &p3) ;
	  n = u_normale_triangle(&p1, &p2, &p3) ;
	  glNormal3f(n.x, n.y, n.z) ;
	  glBegin(GL_POLYGON) ;
	  for(j=0;j<mt.taille[0];j+=k)
	    {
	      recupere_point_ji(&mt, j, 0, &p4) ;
	      t_triplet(&p4) ;
	    }
	  if ( comment & T_RACCORDE_HB )
	    {
	      recupere_point_ji(&mt, 0, 0, &p4) ;
	      t_triplet(&p4) ;
	    }
	  glEnd() ;
	}
      if ( (comment & T_FACETTE_DROITE) && mt.taille[0]>2 )
	{
	  recupere_point_ji(&mt, 2, -1, &p1) ;
	  recupere_point_ji(&mt, 1, -1, &p2) ;
	  recupere_point_ji(&mt, 0, -1, &p3) ;
	  n = u_normale_triangle(&p1, &p2, &p3) ;
	  glNormal3f(n.x, n.y, n.z) ;
	  glBegin(GL_POLYGON) ;
	  for(j=mt.taille[0]-1; j>=0 ;j-=k)
	    {
	      recupere_point_ji(&mt, j, -1, &p4) ;
	      t_triplet(&p4) ;
	    }
	  if ( comment & T_RACCORDE_HB )
	    {
	      recupere_point_ji(&mt, 0, -1, &p4) ;
	      t_triplet(&p4) ;
	    }
	  glEnd() ;
	}
      glDisable(GL_LIGHTING) ;
      if ( comment & T_CULL_FACE )
	glDisable(GL_CULL_FACE) ;
    }

  if ( comment & T_FILAIRE )
    {
      for (j=(comment&T_DEGENERE_BAS)?1:0 ;
	   j<mt.taille[0]-((comment&T_DEGENERE_HAUT)?1:0) ; j+=k)
	{
	  glBegin(GL_LINE_STRIP) ;
	  for (i=0; i<mt.taille[1]; i+=k)
	    {
	      recupere_point_ji(&mt, j, i, &pt) ;
	      glVertex3dv((GLdouble*)&pt) ;
	    }
	  if ( comment & T_RACCORDE_GD )
	    {
	      recupere_point_ji(&mt, j, 0, &pt) ;
	      glVertex3dv((GLdouble*)&pt) ;
	    }
	  glEnd() ;
	}
      for (i=(comment&T_DEGENERE_GAUCHE)?1:0;
	   i<mt.taille[1]-((comment&T_DEGENERE_DROITE)?1:0) ; i+=k)
	{
	  glBegin(GL_LINE_STRIP) ;
	  for (j=0 ; j<mt.taille[0] ; j+=k)
	    {
	      recupere_point_ji(&mt, j, i, &pt) ;
	      glVertex3dv((GLdouble*)&pt) ;
	    }
	  if ( comment & T_RACCORDE_HB )
	    {
	      recupere_point_ji(&mt, 0, i, &pt) ;
	      glVertex3dv((GLdouble*)&pt) ;
	    }
	  glEnd() ;
	}
    }
  if ( comment & T_PONCTUEL )
    {
      glBegin(GL_POINTS) ;
      for (j=(comment&T_DEGENERE_BAS)?1:0 ;
	   j<mt.taille[0]-((comment&T_DEGENERE_HAUT)?1:0) ; j++)
	for (i=(comment&T_DEGENERE_GAUCHE)?1:0;
	     i<mt.taille[1]-((comment&T_DEGENERE_DROITE)?1:0) ; i++)
	  {
	    recupere_point_ji(&mt, j, i, &pt) ;
	    glVertex3dv((GLdouble*)&pt) ;
	  }
      if ( comment&(T_DEGENERE_BAS|T_DEGENERE_GAUCHE) )
	{
	  recupere_point_ji(&mt, 0, 0, &pt) ;
	  glVertex3dv((GLdouble*)&pt) ;
	}
      if ( comment&(T_DEGENERE_HAUT|T_DEGENERE_DROITE) )
	{
	  recupere_point_ji(&mt, -1 ,-1, &pt) ;
	  glVertex3dv((GLdouble*)&pt) ;
	}
      glEnd() ;
    }
  if ( comment & (T_TAG|T_TAG_DESTRUCTION|T_TAG_POIDS) )
    {
      for (j=0 ; j<mt.taille[0] ; j++)
	for (i=0 ; i<mt.taille[1] ; i++)
	  {
	    nb = recupere_point_ji(&mt, j, i, &pt) ;

	    if ( comment & T_TAG )
	      t_tag_origine(TAG_DEPLACEMENT +i+j*mt.taille[1], &pt) ;
	    if ( comment & T_TAG_DESTRUCTION )
	      t_tag_destruction(TAG_DESTRUCTION+i+j*mt.taille[1],&pt);
	    if ( nb>=4 && (comment & T_TAG_POIDS) )
	      t_tag_poids(TAG_POIDS+ i + j*mt.taille[1], &pt) ;
	  }
    }
}

void t_grille(const void *o, const Champ *ch, int comment, const char *urgent,
	      const Champ *chn)
{
  if ( comment & T_LISSAGE )
    t_grille_general(&T(Grille, o, ch), ch->tp, comment,
		     urgent, &T(Grille, o, chn), chn->tp) ;
  else
    t_grille_general(&T(Grille, o, ch), ch->tp, comment, urgent, NULL, NULL) ;
}


const char * lit_tag_grille(const void *o, const Champ *ch)
{
  int i, j ;
  struct informations_table it ;
  static Chaine s ;
  const Champ *c ;

  i = lit_numero_tag(ch) ;
  j = i%NB_TAG_PLAGE ;
  i -= j ;
  c = c_trouve(o,"t") ;
  if ( c == NULL )
    return("L'objet n'est pas éditable (Voir option de 'operateur'") ;
  it = codec_informations_table(c->tp, PTR(o,c)) ;
  if ( j>=0 && j<it.taille[0]*it.taille[1] )
    {
      codec_positionne_element_table(&it
				     , j/it.taille[1]
				     , j%it.taille[1]
				     ) ;
      switch(i)
	{
	case TAG_DEPLACEMENT:
	  return( codec_recupere_element_courant(&it) ) ;
	case TAG_POIDS:
	  return( recupere_poids(&it) ) ;

	case TAG_DESTRUCTION:
	  s_efface(&s) ;
	  s_printf(&s,"Cliquer dessus pour\n"
		   "détruire le point numéro %d",
		   j) ;
	  return(s_char(&s)) ;
	}

    }
  return("") ;
}

void ecrit_tag_grille(void *o, const Champ *ch, const char *vv)
{
  int i, j ;
  struct informations_table it ;
  const Champ *c ;
  char *v ;

  v = strdup(vv) ;
  if ( strstr(v," (") )
    strstr(v," (")[0] = '\0' ; /* Enlève direction regard */

  i = lit_numero_tag(ch) ;
  j = i%NB_TAG_PLAGE ;
  i -= j ;
  c = c_trouve(o,"t") ;
  if ( c == NULL )
    return ;
  it = codec_informations_table(c->tp, PTR(o,c)) ;

  if ( j >= 0 && j < it.taille[0]*it.taille[1] )
    {
      codec_positionne_element_table(&it, j/it.taille[1], j%it.taille[1]) ;
      switch(i)
	{
	case TAG_DEPLACEMENT:
	  change_debut(&it, v) ;
	  break ;
	case TAG_POIDS:
	  change_poids(&it, v) ;
	  break ;
	}
      NOTE_CHAMP_CHANGE_(o,c) ;
    }
  free(v) ;
}
/*
 *
 */
void t_grille_pov_general(const void *t, const Codec_informations *tp,
			  int comment, FILE *f,
			  const void *nor, const Codec_informations *tpnor)
{
  int i, j ;
  struct informations_table mt, mtnor ;
  Triplet p1, p2, p3, p4, np1, np2, np3, np4 ;

  mt = codec_informations_table(tp, t) ;
  if ( comment & T_LISSAGE )
    mtnor = codec_informations_table(tpnor, nor) ;

  if ( mt.taille[0] == 0 || mt.taille[1] == 0 )
    return ;
  
  if ( comment & T_FACETTE )
    {
      for(i=0;i<mt.taille[1]-1+!!(comment&T_RACCORDE_GD);i++)
	for(j=0;j<mt.taille[0]-1+!!(comment&T_RACCORDE_HB);j++)
	  {	    
	    recupere_point_ji(&mt, j  , i+1, &p1) ;
	    recupere_point_ji(&mt, j+1, i+1, &p2) ;
	    recupere_point_ji(&mt, j  , i  , &p3) ;
	    recupere_point_ji(&mt, j+1, i  , &p4) ;
	    if ( comment & T_LISSAGE )
	      {
		recupere_point_ji(&mtnor, j  , i+1, &np1) ;
		recupere_point_ji(&mtnor, j+1, i+1, &np2) ;
		recupere_point_ji(&mtnor, j  , i  , &np3) ;
		recupere_point_ji(&mtnor, j+1, i  , &np4) ;
	      }

	    if ( (!(comment&T_DEGENERE_DROITE) || i!=mt.taille[1]-2 )
		 && (!(comment&T_DEGENERE_BAS) || j!=0))
	      {
		if ( comment & T_LISSAGE )
		  fprintf(f
			  , "smooth_triangle { <%g,%g,%g>,<%g,%g,%g>,<%g,%g,%g>,<%g,%g,%g>,<%g,%g,%g>,<%g,%g,%g> }\n"
			  , p3.x, p3.y, p3.z
			  , np3.x, np3.y, np3.z
			  , p1.x, p1.y, p1.z
			  , np1.x, np1.y, np1.z
			  , p2.x, p2.y, p2.z
			  , np2.x, np2.y, np2.z
			  ) ;
		else
		  fprintf(f
			  , "polygon { 3,<%g,%g,%g>,<%g,%g,%g>,<%g,%g,%g> }\n"
			  , p3.x, p3.y, p3.z
			  , p1.x, p1.y, p1.z
			  , p2.x, p2.y, p2.z
			  ) ;
	      }
	    if ( (!(comment&T_DEGENERE_GAUCHE) || i!=0)
		 && (!(comment&T_DEGENERE_HAUT) || j!=mt.taille[0]-2))
	      {
		if ( comment & T_LISSAGE )
		  fprintf(f
			  , "smooth_triangle { <%g,%g,%g>,<%g,%g,%g>,<%g,%g,%g>,<%g,%g,%g>,<%g,%g,%g>,<%g,%g,%g>}\n"
			  , p3.x, p3.y, p3.z
			  , np3.x, np3.y, np3.z
			  , p2.x, p2.y, p2.z
			  , np2.x, np2.y, np2.z
			  , p4.x, p4.y, p4.z
			  , np4.x, np4.y, np4.z
			  ) ;
		else
		  fprintf(f
			  , "polygon { 3,<%g,%g,%g>,<%g,%g,%g>,<%g,%g,%g> }\n"
			  , p3.x, p3.y, p3.z
			  , p2.x, p2.y, p2.z
			  , p4.x, p4.y, p4.z
			  ) ;
	      }
	  }
      if ( (comment & T_FACETTE_HAUT) && mt.taille[1]>2 )
	{
	  fprintf(f, "polygon { %d", mt.taille[1]+!!(comment&T_RACCORDE_GD)) ;
	  for(i=0;i<mt.taille[1]+!!(comment&T_RACCORDE_GD);i++)
	    {
	      recupere_point_ji(&mt,-1, i, &p4) ;
	      fprintf(f, ",<%g,%g,%g>", p4.x, p4.y, p4.z) ;
	    }
	  fprintf(f, "}\n") ;
	}
      if ( (comment & T_FACETTE_BAS) && mt.taille[1]>2 )
	{
	  fprintf(f, "polygon { %d", mt.taille[1]+!!(comment&T_RACCORDE_GD)) ;
	  for(i=mt.taille[1]-1+!!(comment&T_RACCORDE_GD);i>=0;i--)
	    {
	      recupere_point_ji(&mt, 0, i, &p4) ;
	      fprintf(f, ",<%g,%g,%g>", p4.x, p4.y, p4.z) ;
	    }
	  fprintf(f, "}\n") ;
	}
      if ( (comment & T_FACETTE_GAUCHE) && mt.taille[0]>2 )
	{
	  fprintf(f, "polygon { %d", mt.taille[0]+!!(comment&T_RACCORDE_HB)) ;
	  for(j=0;j<mt.taille[0]+!!(comment&T_RACCORDE_HB);j++)
	    {
	      recupere_point_ji(&mt, j, 0, &p4) ;
	      fprintf(f, ",<%g,%g,%g>", p4.x, p4.y, p4.z) ;
	    }
	  fprintf(f, "}\n") ;
	}
      if ( (comment & T_FACETTE_DROITE) && mt.taille[0]>2 )
	{
	  fprintf(f, "polygon { %d", mt.taille[0]+!!(comment&T_RACCORDE_HB)) ;
	  for(j=mt.taille[0]-1+!!(comment&T_RACCORDE_HB); j>=0 ;j--)
	    {
	      recupere_point_ji(&mt, j, -1, &p4) ;
	      fprintf(f, ",<%g,%g,%g>", p4.x, p4.y, p4.z) ;
	    }
	  fprintf(f, "}\n") ;
	}
    }
  if ( comment & T_PONCTUEL )
    {
      for (j=(comment&T_DEGENERE_BAS)?1:0 ;
	   j<mt.taille[0]-((comment&T_DEGENERE_HAUT)?1:0) ; j++)
	for (i=(comment&T_DEGENERE_GAUCHE)?1:0;
	     i<mt.taille[1]-((comment&T_DEGENERE_DROITE)?1:0) ; i++)
	  {	    
	    recupere_point_ji(&mt, j  , i  , &p3) ;

	    fprintf(f, "sphere { <%g,%g,%g>, %g }\n"
		    , p3.x, p3.y, p3.z,
		    globale_etat_courant.epaisseur_trait) ;
	  }
      if ( comment&(T_DEGENERE_BAS|T_DEGENERE_GAUCHE) )
	{
	  recupere_point_ji(&mt, 0, 0, &p3) ;
	  fprintf(f, "sphere { <%g,%g,%g>, %g }\n"
		  , p3.x, p3.y, p3.z,
		  globale_etat_courant.epaisseur_trait) ;
	}
      if ( comment&(T_DEGENERE_HAUT|T_DEGENERE_DROITE) )
	{
	  recupere_point_ji(&mt, -1, -1, &p3) ;
	  fprintf(f, "sphere { <%g,%g,%g>, %g }\n"
		  , p3.x, p3.y, p3.z,
		  globale_etat_courant.epaisseur_trait) ;
	}
    }
  if ( comment & T_FILAIRE )
    {
      for (i=(comment&T_DEGENERE_GAUCHE)?1:0;
	   i<mt.taille[1]-((comment&T_DEGENERE_DROITE)?1:0); i++)
	{
	  for(j=0;j<mt.taille[0]-1+!!(comment&T_RACCORDE_HB);j++)
	    {	    
	      recupere_point_ji(&mt, j  , i  , &p3) ;
	      recupere_point_ji(&mt, j+1, i  , &p4) ;
	      
	  fprintf(f, "cylinder { <%g,%g,%g>, <%g,%g,%g>, %g }\n"
		  , p3.x, p3.y, p3.z
		  , p4.x, p4.y, p4.z
		  , globale_etat_courant.epaisseur_trait
		  ) ;
	    }
	}
      for(i=0;i<mt.taille[1]-1+!!(comment&T_RACCORDE_GD);i++)
	{
	  for (j=(comment&T_DEGENERE_BAS)?1:0 ;
	       j<mt.taille[0]-((comment&T_DEGENERE_HAUT)?1:0) ; j++)
	    {	    
	      recupere_point_ji(&mt, j  , i  , &p3) ;
	      recupere_point_ji(&mt, j  , i+1, &p4) ;
	      
	      fprintf(f, "cylinder { <%g,%g,%g>, <%g,%g,%g>, %g }\n"
		      , p3.x, p3.y, p3.z
		      , p4.x, p4.y, p4.z
		      , globale_etat_courant.epaisseur_trait
		      ) ;
	    }
	}
    }
}

void t_grille_pov(const void *o, const Champ *ch, int comment, const char *urgent, const Champ *chn)
{
if ( comment & T_LISSAGE )
     t_grille_pov_general(&T(Grille, o, ch), ch->tp
     , comment, (FILE*)atol(urgent), &T(Grille, o, chn), chn->tp) ;
else
t_grille_pov_general(&T(Grille, o, ch), ch->tp
, comment, (FILE*)atol(urgent), NULL, NULL) ;
}

/*
 *
 */

void changement_grille(void *o, const Champ *ch)
{
  static Chaine s ;
  const Champ *c ;
  int i, j ;
  char *tmp ;
  void *table ;
  struct informations_table it ;

  if ( FILS_CHANGE(o) )
    {
      if ( TRACE(o) )
	eprintf("Récupère la valeur des fils\n") ;
      s_efface(&s) ;
      s_putchar(&s, '[') ;
      POUR_LES_FILS(o,f,
		    if(c_trouve_type((const void**)&f, &c, ch->type_dans_fils))
		    {
		      s_strcat(&s, c_recupere_ici_sans_copie(f,c)) ;
		      s_putchar(&s, '\n') ;
		    }) ;
      s_putchar(&s, ']') ;
      if ( TRACE(o) )
	eprintf("ancienne=(%s) nouvelle=(%s)\n"
                , c_recupere_ici_sans_copie(o, ch)
                , s_char(&s) 
		) ;
      c_sauve_ici(o, ch, s_char(&s)) ;
    }
  else
    if ( CHAMP_CHANGE_(OBJET(o), ch) || CA_CHANGE(o) )
      {
	if ( TRACE(o) )
	  eprintf("Descend ma valeur chez les fils\n") ;
	table = PTR(o, ch) ;
	it = codec_informations_table(ch->tp, table) ;
	j = 0 ;
	POUR_LES_FILS
	  (o, f,
	   {
	     if (c_trouve_type((const void**)&f, &c, ch->type_dans_fils))
	       {
		 if ( j++==it.taille[0] ) break ;

		 codec_debut_recupere_texte() ;
		 codec_ouvre_parenthese_recupere_texte() ;

		 for(i=0;i<it.taille[1]; i++)
		   {
		     putstr(codec_recupere_element_courant_avec_lf(&it)) ;
		   }

		 codec_ferme_parenthese_recupere_texte() ;
		 tmp = strdup(codec_fin_recupere_texte()) ;
		 if ( TRACE(o) )
		   eprintf("[%d] descend :%s\n",i, tmp) ;
		 c_sauve_ici(f, c, tmp) ;
		 free(tmp) ;
	       }
	   }
	   ) ;
      }
}



int recupere_point_ji_offset(struct informations_table *it, int j, int i, Triplet *p, Triplet *po, int comment)
{
  int res ;
  Triplet ppo ;

  res = recupere_point_ji_(it, j, i, p) ;
  ppo = normale_moyenne(it, j, i, comment) ;
  ppo = u_triplet_fois_flottant(&ppo, -globale_etat_courant.stl_offset) ;
  *po = u_ajoute_triplet(p, &ppo) ;
  return( res ) ;
}



/*
 * Le calcul de l'offset est complètement FAUX.
 * En effet, le polyèdre offset à l'intérieur n'a pas
 * la même topologie que l'extérieur.
 * Exemple : au sommet il y a 2 plans très proche
 * l'un de l'autre et 2 autres perpendiculaire et éloignés.
 * Le polyèdre intérieur à un segment au lieu d'un sommet.
 */

void t_grille_stl_general(const void *t, const Codec_informations *tp,
			  int comment, FILE *f)
{
  int i, j ;
  struct informations_table mt ;
  Triplet p1, p2, p3, p4, p1o, p2o, p3o, p4o ;
  Booleen facette_epaisse ;

  mt = codec_informations_table(tp, t) ;

  if ( mt.taille[0] < 2 || mt.taille[1] < 2 )
    return ;
  
  facette_epaisse = (comment & T_FACETTE) && !(comment & T_CULL_FACE) ;

  if ( (comment & T_FACETTE) && !facette_epaisse )
    {
      for(i=0;i<mt.taille[1]-1+!!(comment&T_RACCORDE_GD);i++)
	for(j=0;j<mt.taille[0]-1+!!(comment&T_RACCORDE_HB);j++)
	  {	    
	    recupere_point_ji_offset(&mt, j  , i+1, &p1, &p1o, comment) ;
	    recupere_point_ji_offset(&mt, j+1, i+1, &p2, &p2o, comment) ;
	    recupere_point_ji_offset(&mt, j  , i  , &p3, &p3o, comment) ;
	    recupere_point_ji_offset(&mt, j+1, i  , &p4, &p4o, comment) ;

	    if ( (!(comment&T_DEGENERE_DROITE) || i!=mt.taille[1]-2 )
		 && (!(comment&T_DEGENERE_BAS) || j!=0))
	      t_triangle_stl_offset_(f, &p3, &p1, &p2, &p3o, &p1o, &p2o) ;
	    
	    if ( (!(comment&T_DEGENERE_GAUCHE) || i!=0)
		 && (!(comment&T_DEGENERE_HAUT) || j!=mt.taille[0]-2))
	      t_triangle_stl_offset_(f, &p3, &p2, &p4, &p3o, &p2o, &p4o) ;
	  }
      if ( (comment & T_FACETTE_HAUT) && mt.taille[1]>2 )
	{
	  t_debut_polygone_stl(f) ;
	  for(i=0;i<mt.taille[1];i++)
	    {
	      recupere_point_ji_offset(&mt,-1, i, &p4, &p4o, comment) ;
	      t_point_stl_offset(&p4, &p4o) ;
	    }
	  t_fin_polygone_stl_offset_() ;
	}
      if ( (comment & T_FACETTE_BAS) && mt.taille[1]>2 )
	{
	  t_debut_polygone_stl(f) ;
	  for(i=mt.taille[1]-1;i>=0;i--)
	    {
	      recupere_point_ji_offset(&mt, 0, i, &p4, &p4o, comment) ;
	      t_point_stl_offset(&p4, &p4o) ;
	    }
	  t_fin_polygone_stl_offset_() ;
	}
      if ( (comment & T_FACETTE_GAUCHE) && mt.taille[0]>2 )
	{
	  t_debut_polygone_stl(f) ;
	  for(j=0;j<mt.taille[0];j++)
	    {
	      recupere_point_ji_offset(&mt, j, 0, &p4, &p4o, comment) ;
	      t_point_stl(&p4) ;
	    }
	  t_fin_polygone_stl_offset_() ;
	}
      if ( (comment & T_FACETTE_DROITE) && mt.taille[0]>2 )
	{
	  t_debut_polygone_stl(f) ;
	  for(j=mt.taille[0]-1; j>=0 ;j--)
	    {
	      recupere_point_ji_offset(&mt, j, -1, &p4, &p4o, comment) ;
	      t_point_stl_offset(&p4, &p4o) ;
	    }
	  t_fin_polygone_stl_offset_() ;
	}



      if ( !(comment & (T_FACETTE_DROITE | T_DEGENERE_DROITE | T_RACCORDE_GD)))
	{
	  for(j=mt.taille[0]-2+!!(comment&T_RACCORDE_HB); j>=0 ;j--)
	    {
	      recupere_point_ji_offset(&mt, j, -1, &p1, &p1o, comment) ;
	      recupere_point_ji_offset(&mt, j+1, -1, &p2, &p2o, comment) ;
	      t_quadrangle_stl_(f, &p2, &p1, &p1o, &p2o) ;
	    }
	}

      if ( !(comment & (T_FACETTE_GAUCHE | T_DEGENERE_GAUCHE | T_RACCORDE_GD)))
	{
	  for(j=mt.taille[0]-2+!!(comment&T_RACCORDE_HB); j>=0 ;j--)
	    {
	      recupere_point_ji_offset(&mt, j+1, 0, &p1, &p1o, comment) ;
	      recupere_point_ji_offset(&mt, j, 0, &p2, &p2o, comment) ;
	      t_quadrangle_stl_(f, &p2, &p1, &p1o, &p2o) ;
	    }
	}

      if ( !(comment & (T_FACETTE_BAS | T_DEGENERE_BAS | T_RACCORDE_HB)))
	{
	  for(i=mt.taille[1]-2+!!(comment&T_RACCORDE_GD); i>=0 ;i--)
	    {
	      recupere_point_ji_offset(&mt, 0, i, &p1, &p1o, comment) ;
	      recupere_point_ji_offset(&mt, 0, i+1, &p2, &p2o, comment) ;
	      t_quadrangle_stl_(f, &p2, &p1, &p1o, &p2o) ;
	    }
	}

      if ( !(comment & (T_FACETTE_HAUT | T_DEGENERE_HAUT | T_RACCORDE_HB)))
	{
	  for(i=mt.taille[1]-2+!!(comment&T_RACCORDE_GD); i>=0 ;i--)
	    {
	      recupere_point_ji_offset(&mt, -1, i+1, &p2, &p2o, comment) ;
	      recupere_point_ji_offset(&mt, -1, i, &p1, &p1o, comment) ;
	      t_quadrangle_stl_(f, &p2, &p1, &p1o, &p2o) ;
	    }
	}
    }

  if ( facette_epaisse )
    {
      for(i=0;i<mt.taille[1]-1+!!(comment&T_RACCORDE_GD);i++)
	for(j=0;j<mt.taille[0]-1+!!(comment&T_RACCORDE_HB);j++)
	  {	    
	    recupere_point_ji_(&mt, j  , i+1, &p1) ;
	    recupere_point_ji_(&mt, j+1, i+1, &p2) ;
	    recupere_point_ji_(&mt, j  , i  , &p3) ;
	    recupere_point_ji_(&mt, j+1, i  , &p4) ;

	    if ( (!(comment&T_DEGENERE_DROITE) || i!=mt.taille[1]-2 )
		 && (!(comment&T_DEGENERE_BAS) || j!=0))
	      t_triangle_stl_epais_(f, &p3, &p1, &p2, globale_etat_courant.stl_offset) ;
	    
	    if ( (!(comment&T_DEGENERE_GAUCHE) || i!=0)
		 && (!(comment&T_DEGENERE_HAUT) || j!=mt.taille[0]-2))
	      t_triangle_stl_epais_(f, &p3, &p2, &p4, globale_etat_courant.stl_offset) ;
	  }
      if ( (comment & T_FACETTE_HAUT) && mt.taille[1]>2 )
	{
	  t_debut_polygone_stl(f) ;
	  for(i=0;i<mt.taille[1];i++)
	    {
	      recupere_point_ji_(&mt,-1, i, &p4) ;
	      t_point_stl(&p4) ;
	    }
	  t_fin_polygone_stl_epais_(globale_etat_courant.stl_offset) ;
	}
      if ( (comment & T_FACETTE_BAS) && mt.taille[1]>2 )
	{
	  t_debut_polygone_stl(f) ;
	  for(i=mt.taille[1]-1;i>=0;i--)
	    {
	      recupere_point_ji_(&mt, 0, i, &p4) ;
	      t_point_stl(&p4) ;
	    }
	  t_fin_polygone_stl_epais_(globale_etat_courant.stl_offset) ;
	}
      if ( (comment & T_FACETTE_GAUCHE) && mt.taille[0]>2 )
	{
	  t_debut_polygone_stl(f) ;
	  for(j=0;j<mt.taille[0];j++)
	    {
	      recupere_point_ji_(&mt, j, 0, &p4) ;
	      t_point_stl(&p4) ;
	    }
	  t_fin_polygone_stl_epais_(globale_etat_courant.stl_offset) ;
	}
      if ( (comment & T_FACETTE_DROITE) && mt.taille[0]>2 )
	{
	  t_debut_polygone_stl(f) ;
	  for(j=mt.taille[0]-1; j>=0 ;j--)
	    {
	      recupere_point_ji_(&mt, j, -1, &p4) ;
	      t_point_stl(&p4) ;
	    }
	  t_fin_polygone_stl_epais_(globale_etat_courant.stl_offset) ;
	}
    }

  if ( (comment & T_FILAIRE) || facette_epaisse )
    {
      for (i=(comment&T_DEGENERE_GAUCHE)?1:0;
	   i<mt.taille[1]-((comment&T_DEGENERE_DROITE)?1:0); i++)
	{
	  for(j=0;j<mt.taille[0]-1+!!(comment&T_RACCORDE_HB);j++)
	    {	    
	      recupere_point_ji_(&mt, j  , i  , &p3) ;
	      recupere_point_ji_(&mt, j+1, i  , &p4) ;
	      t_cylindre_stl_(f, &p3, &p4, globale_etat_courant.stl_offset/2
			     , 8) ;
	    }
	}
      for(i=0;i<mt.taille[1]-1+!!(comment&T_RACCORDE_GD);i++)
	{
	  for (j=(comment&T_DEGENERE_BAS)?1:0 ;
	       j<mt.taille[0]-((comment&T_DEGENERE_HAUT)?1:0) ; j++)
	    {	    
	      recupere_point_ji_(&mt, j  , i  , &p3) ;
	      recupere_point_ji_(&mt, j  , i+1, &p4) ;
	      t_cylindre_stl_(f, &p3, &p4, globale_etat_courant.stl_offset/2
			     , 8) ;
	    }
	}
      /*
      for(i=0;i<mt.taille[1]-1+!!(comment&T_RACCORDE_GD);i++)
	for(j=0;j<mt.taille[0]-1+!!(comment&T_RACCORDE_HB);j++)
	  {	    
	    recupere_point_ji_(&mt, j+1, i+1, &p2) ;
	    recupere_point_ji_(&mt, j  , i  , &p3) ;
	    t_cylindre_stl_(f, &p2, &p3, globale_etat_courant.stl_offset/2
			    , 8) ;
	  }
      */
    }
  if ( (comment & T_PONCTUEL) ||  facette_epaisse )
    {
      for (j=(comment&T_DEGENERE_BAS)?1:0 ;
	   j<mt.taille[0]-((comment&T_DEGENERE_HAUT)?1:0) ; j++)
	for (i=(comment&T_DEGENERE_GAUCHE)?1:0;
	     i<mt.taille[1]-((comment&T_DEGENERE_DROITE)?1:0) ; i++)
	  {	    
	    recupere_point_ji_(&mt, j  , i  , &p3) ;
	    t_sphere_stl_(f, &p3, globale_etat_courant.stl_offset/2, 4, 8) ;
	  }
      if ( comment&(T_DEGENERE_BAS|T_DEGENERE_GAUCHE) )
	{
	  recupere_point_ji_(&mt, 0, 0, &p3) ;
	  t_sphere_stl_(f, &p3, globale_etat_courant.stl_offset/2, 4, 8) ;
	}
      if ( comment&(T_DEGENERE_HAUT|T_DEGENERE_DROITE) )
	{
	  recupere_point_ji_(&mt, -1, -1, &p3) ;
	  t_sphere_stl_(f, &p3, globale_etat_courant.stl_offset/2, 4, 8) ;
	}
    }
}

void t_grille_stl(const void *o, const Champ *ch, int comment, const char *urgent)
{
  t_grille_stl_general(&T(Grille, o, ch), ch->tp
		       , comment, (FILE*)atol(urgent)) ;
}

/*
 * Tenseur
 */


int recupere_point_kji(struct informations_table *it,
		       int k, int j, int i, Triplet *p)
{
  codec_positionne_element_table(it
				 , (k+it->taille[0])%it->taille[0]
				 , (j+it->taille[1])%it->taille[1]
				 , (i+it->taille[2])%it->taille[2]
				 ) ;
  return( recupere_point(it, p) ) ;
}

const char * lit_tag_tenseur(const void *o, const Champ *ch)
{
  int i, j ;
  struct informations_table it ;
  static Chaine s ;
  const Champ *c ;

  i = lit_numero_tag(ch) ;
  j = i%NB_TAG_PLAGE ;
  i -= j ;
  c = c_trouve(o,"t") ;
  if ( c == NULL )
    return("L'objet n'est pas éditable (Voir option de 'operateur'") ;

  it = codec_informations_table(c->tp, PTR(o,c)) ;
  if ( j>=0 && j<it.taille[0]*it.taille[1]*it.taille[2] )
    {
      codec_positionne_element_table(&it
				     , j/it.taille[1]/it.taille[2]
				     , (j%(it.taille[1]*it.taille[2]))
				     / it.taille[1]
				     , (j%(it.taille[1]*it.taille[2]))
				     % it.taille[1]
				     ) ;
      switch(i)
	{
	case TAG_DEPLACEMENT:
	  return( codec_recupere_element_courant(&it) ) ;
	case TAG_POIDS:
	  return( recupere_poids(&it) ) ;

	case TAG_DESTRUCTION:
	  s_efface(&s) ;
	  s_printf(&s,"Cliquer dessus pour\n"
		   "détruire le point numéro %d",
		   j) ;
	  return(s_char(&s)) ;
	}

    }
  return("") ;
}

void ecrit_tag_tenseur(void *o, const Champ *ch, const char *vv)
{
  int i, j ;
  struct informations_table it ;
  const Champ *c ;
  char *v ;

  v = strdup(vv) ;
  if ( strstr(v," (") )
    strstr(v," (")[0] = '\0' ; /* Enlève direction regard */

  i = lit_numero_tag(ch) ;
  j = i%NB_TAG_PLAGE ;
  i -= j ;
  c = c_trouve(o,"t") ;
  if ( c == NULL )
    return ;
  it = codec_informations_table(c->tp, PTR(o,c)) ;

  if ( j >= 0 && j < it.taille[0]*it.taille[1]*it.taille[2] )
    {
      codec_positionne_element_table(&it
				     , j/it.taille[1]/it.taille[2]
				     , (j%(it.taille[1]*it.taille[2]))
				     / it.taille[1]
				     , (j%(it.taille[1]*it.taille[2]))
				     % it.taille[1]
				     ) ;
      switch(i)
	{
	case TAG_DEPLACEMENT:
	  change_debut(&it, v) ;
	  break ;
	case TAG_POIDS:
	  change_poids(&it, v) ;
	  break ;
	}
      NOTE_CHAMP_CHANGE_(o,c) ;
    }
  free(v) ;
}

/*
 *
 */

void changement_tenseur(void *o, const Champ *ch)
{
  static Chaine s ;
  const Champ *c ;
  int i, j, k ;
  char *tmp ;
  void *table ;
  struct informations_table it ;

  if ( FILS_CHANGE(o) )
    {
      if ( TRACE(o) )
	eprintf("Récupère la valeur des fils\n") ;
      s_efface(&s) ;
      s_putchar(&s, '{') ;
      POUR_LES_FILS(o,f,
		    if(c_trouve_type((const void**)&f, &c, ch->type_dans_fils))
		    {
		      s_strcat(&s, c_recupere_ici_sans_copie(f,c)) ;
		      s_putchar(&s, '\n') ;
		    }) ;
      s_putchar(&s, '}') ;
      if ( TRACE(o) )
	eprintf("ancienne=(%s) nouvelle=(%s)\n"
                , c_recupere_ici_sans_copie(o, ch)
                , s_char(&s) 
		) ;
      c_sauve_ici(o, ch, s_char(&s)) ;
    }
  else
    if ( CHAMP_CHANGE_(OBJET(o), ch) || CA_CHANGE(o) )
      {
	if ( TRACE(o) )
	  eprintf("Descend ma valeur chez les fils\n") ;
	table = PTR(o, ch) ;
	it = codec_informations_table(ch->tp, table) ;
	j = 0 ;
	POUR_LES_FILS
	  (o, f,
	   {
	     if (c_trouve_type((const void**)&f, &c, ch->type_dans_fils))
	       {
		 if ( j++==it.taille[0] ) break ;

		 codec_debut_recupere_texte() ;
		 codec_ouvre_crochet_recupere_texte() ;

		 for(i=0;i<it.taille[1]; i++)
		   {
		     codec_ouvre_parenthese_recupere_texte() ;
		     for(k=0;k<it.taille[2]; k++)
		       {
			 putstr(codec_recupere_element_courant_avec_lf(&it)) ;
		       }
		     codec_ferme_parenthese_recupere_texte() ;
		   }

		 codec_ferme_crochet_recupere_texte() ;
		 tmp = strdup(codec_fin_recupere_texte()) ;
		 if ( TRACE(o) )
		   eprintf("[%d] descend :%s\n",i, tmp) ;
		 c_sauve_ici(f, c, tmp) ;
		 free(tmp) ;
	       }
	   }
	   ) ;
      }
}




void t_tenseur(const void *t, const Codec_informations *tp
	       , int comment, const char *urgent)
{
  int i, j, k, nb, d ;
  struct informations_table mt ;
  Triplet pt ;

  mt = codec_informations_table(tp, t) ;

  if ( mt.taille[0] == 0 || mt.taille[1] == 0 || mt.taille[2] == 0 )
    return ;

  d = '5' - urgent[0] ;
  if ( d < 0 )
    d = 0 ;
  d = 1<<(2*d) ; /* Normale 1,Mauvais 4,Tres mauvais 16,Inacceptable 64 */

  if ( comment & T_FILAIRE )
    {
      for(i=0; i<mt.taille[2]; i+= d)
	for (j=0 ; j<mt.taille[1] ; j+=d)
	  {
	    glBegin(GL_LINE_STRIP) ;
	    for (k=0 ; k<mt.taille[0] ; k+=d)
	      {
		recupere_point_kji(&mt, k, j, i, &pt) ;
		glVertex3dv((GLdouble*)&pt) ;
	      }
	    glEnd() ;
	  }
      for(i=0; i<mt.taille[2]; i+= d)
	for (k=0 ; k<mt.taille[0] ; k+=d)
	  {
	    glBegin(GL_LINE_STRIP) ;
	    for (j=0 ; j<mt.taille[1] ; j+=d)
	      {
		recupere_point_kji(&mt, k, j, i, &pt) ;
		glVertex3dv((GLdouble*)&pt) ;
	      }
	    glEnd() ;
	  }
      for (k=0 ; k<mt.taille[0] ; k+=d)
        for (j=0 ; j<mt.taille[1] ; j+=d)
        {
          glBegin(GL_LINE_STRIP) ;
          for(i=0; i<mt.taille[2]; i+= d)
            {
	      recupere_point_kji(&mt, k, j, i, &pt) ;
	      glVertex3dv((GLdouble*)&pt) ;
            }
          glEnd() ;
        }
    }
  if ( comment & T_PONCTUEL )
    {
      glBegin(GL_POINTS) ;
      for (k=0 ; k<mt.taille[0] ; k+=d)
	for (j=0 ; j<mt.taille[1] ; j+=d)
	  for(i=0; i<mt.taille[2]; i+= d)
	      {
		recupere_point_kji(&mt, k, j, i, &pt) ;
		glVertex3dv((GLdouble*)&pt) ;
	      }
      glEnd() ;
    }
  if ( comment & (T_TAG|T_TAG_DESTRUCTION|T_TAG_POIDS) )
    {
      for (k=0 ; k<mt.taille[0] ; k+=d)
	for (j=0 ; j<mt.taille[1] ; j+=d)
	  for(i=0; i<mt.taille[2]; i+= d)
	  {
	    nb = recupere_point_kji(&mt, k, j, i, &pt) ;

	    if ( comment & T_TAG )
	      t_tag_origine(TAG_DEPLACEMENT
			    +i+j*mt.taille[2]+k*mt.taille[2]*mt.taille[1], &pt) ;
	    if ( comment & T_TAG_DESTRUCTION )
	      t_tag_destruction(TAG_DESTRUCTION
			    +i+j*mt.taille[2]+k*mt.taille[2]*mt.taille[1], &pt) ;
	    if ( nb>=4 && (comment & T_TAG_POIDS) )
	      t_tag_poids(TAG_POIDS
			    +i+j*mt.taille[2]+k*mt.taille[2]*mt.taille[1], &pt) ;
	  }
    }
}

/*
 *
 */

#define V(J,I) t_grille_obj_point_start + \
  ((J+mt.taille[0]) % mt.taille[0]) * mt.taille[1] +	\
  (I+mt.taille[1]) % mt.taille[1]

int t_grille_obj_point_start ;

void t_grille_obj_general(const void *t, const Codec_informations *tp,
			  int comment, FILE *f)
{
  int i, j ;
  struct informations_table mt ;
  Triplet p ;

  mt = codec_informations_table(tp, t) ;

  if ( mt.taille[0] < 2 || mt.taille[1] < 2 )
    return ;


#define mm globale_etat_courant

  fprintf(f, "# newmtl m%d\n", t_grille_obj_point_start) ;
  fprintf(f, "#  Ka %g %g %g\n", mm.ambiante.x, mm.ambiante.y, mm.ambiante.z);
  fprintf(f, "#  Kd %g %g %g\n",
	  mm.couleur.x*mm.diffuse,
	  mm.couleur.y*mm.diffuse,
	  mm.couleur.z*mm.diffuse);
  fprintf(f, "#  Ks %g %g %g\n",
	  mm.couleur.x*mm.speculaire,
	  mm.couleur.y*mm.speculaire,
	  mm.couleur.z*mm.speculaire);
  fprintf(f, "usemtl m%d\n", t_grille_obj_point_start);
  
  if ( comment & T_FACETTE )
    {
      for(j=0;j<mt.taille[0];j++)
	for(i=0;i<mt.taille[1];i++)
	  {	    
	    recupere_point_ji_(&mt, j, i, &p) ;
	    fprintf(f, "v %g %g %g\n", p.x, p.y, p.z) ;
	  }

      for(i=0;i<mt.taille[1]-1+!!(comment&T_RACCORDE_GD);i++)
	for(j=0;j<mt.taille[0]-1+!!(comment&T_RACCORDE_HB);j++)
	  {
	    fprintf(f, "f %d %d %d %d\n",
		    V(j, i+1), V(j+1, i+1), V(j+1, i), V(j, i)) ;
	  }
      if ( (comment & T_FACETTE_HAUT) && mt.taille[1]>2 )
	{
	  fprintf(f, "f") ;
	  for(i=0;i<mt.taille[1];i++)
	    fprintf(f, " %d", V(-1, i)) ;
	  fprintf(f, "\n") ;
	}
      if ( (comment & T_FACETTE_BAS) && mt.taille[1]>2 )
	{
	  fprintf(f, "f") ;
	  for(i=0;i<mt.taille[1];i++)
	    fprintf(f, " %d", V(0, mt.taille[1]-i-1)) ;
	  fprintf(f, "\n") ;
	}
      if ( (comment & T_FACETTE_GAUCHE) && mt.taille[0]>2 )
	{
	  fprintf(f, "f") ;
	  for(j=0;j<mt.taille[0];j++)
	    fprintf(f, " %d", V(j, 0)) ;
	  fprintf(f, "\n") ;
	}
      if ( (comment & T_FACETTE_DROITE) && mt.taille[0]>2 )
	{
	  fprintf(f, "f") ;
	  for(j=0;j<mt.taille[0];j++)
	    fprintf(f, " %d", V(j, -1)) ;
	  fprintf(f, "\n") ;
	}
    }
  t_grille_obj_point_start += mt.taille[0] * mt.taille[1] ;
}

void t_grille_obj(const void *o, const Champ *ch, int comment, const char *urgent)
{
  t_grille_obj_general(&T(Grille, o, ch), ch->tp
		       , comment, (FILE*)atol(urgent)) ;
}
