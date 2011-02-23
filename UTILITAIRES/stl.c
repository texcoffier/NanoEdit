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

#include "o_objet.h"
#include "u_etat_courant.h"
#include "u_geometrie.h"
#include "stl.h"

/*
 * Ceux qui sont souligné ne travaillent pas dans le repère courant.
 * Ils son utile si la transformation à été faites au niveau du dessus.
 */

/*
 * C'est très mal programmé (incrémentalement)
 * Cela ne sera pas modifié, car le futur utilisera un serveur
 * d'affichage.
 */


void t_triangle_stl_(FILE *f, const Triplet *a, const Triplet *b, const Triplet *c)
{
  Triplet n ;
  static FILE *old_f ;
  static Triplet old_color ;

  n = u_normale_triangle(a, b, c) ;
  n = u_normalise_triplet(&n) ;

  if ( globale_etat_courant.stl_couleur )
    if ( old_f != f
	 || globale_etat_courant.couleur.x != old_color.x
	 || globale_etat_courant.couleur.y != old_color.y
	 || globale_etat_courant.couleur.z != old_color.z
	 )
      {
	fprintf(f, "color %g %g %g\n"
		
		,globale_etat_courant.couleur.x < 0
		? 0
		: ( globale_etat_courant.couleur.x > 1
		    ? 1
		    : globale_etat_courant.couleur.x )
		
		,globale_etat_courant.couleur.y < 0
		? 0
		: ( globale_etat_courant.couleur.y > 1
		    ? 1
		    : globale_etat_courant.couleur.y )
		
		,globale_etat_courant.couleur.z < 0
		? 0
		: ( globale_etat_courant.couleur.z > 1
		    ? 1
		    : globale_etat_courant.couleur.z )
		) ;
	old_f = f ;
	old_color = globale_etat_courant.couleur ;
      }

  fprintf(f, "facet normal %g %g %g\n", n.x,n.y,n.z) ;
  fprintf(f, "outer loop\n") ;
  fprintf(f, "vertex %g %g %g\n", a->x,a->y,a->z) ;
  fprintf(f, "vertex %g %g %g\n", b->x,b->y,b->z) ;
  fprintf(f, "vertex %g %g %g\n", c->x,c->y,c->z) ;
  fprintf(f, "endloop\n") ;
  fprintf(f, "endfacet\n") ;
}

void t_triangle_stl(FILE *f, const Triplet *aa, const Triplet *bb, const Triplet *cc)
{
  Triplet a, b, c ;

  a = u_transforme(&globale_etat_courant.transfo, aa) ;
  b = u_transforme(&globale_etat_courant.transfo, bb) ;
  c = u_transforme(&globale_etat_courant.transfo, cc) ;
  t_triangle_stl_(f, &a, &b, &c) ;
}

void t_quadrangle_stl_(FILE *f, const Triplet *a, const Triplet *b, const Triplet *c, const Triplet *d)
{
  t_triangle_stl_(f, a, b, c) ;
  t_triangle_stl_(f, a, c, d) ;
}

void t_quadrangle_stl(FILE *f, const Triplet *a, const Triplet *b, const Triplet *c, const Triplet *d)
{
  t_triangle_stl(f, a, b, c) ;
  t_triangle_stl(f, a, c, d) ;
}

void t_triangle_stl_offset_(FILE *f, const Triplet *aa, const Triplet *bb, const Triplet *cc
			   , const Triplet *aao, const Triplet *bbo, const Triplet *cco)
{
  t_triangle_stl_(f, aa,bb,cc) ;
  if ( globale_etat_courant.stl_offset )
    t_triangle_stl_(f, cco,bbo,aao) ;
}

void t_triangle_stl_epais_general(FILE *f
				  , const Triplet *aa
				  , const Triplet *bb
				  , const Triplet *cc
				  , float epaisseur
				  , void (*triangle)(FILE*
						     ,const Triplet*
						     ,const Triplet*
						     ,const Triplet*)
				  , void (*quadrangle)(FILE*
						       ,const Triplet*
						       ,const Triplet*
						       ,const Triplet*
						       ,const Triplet*)
				  )
{
  Triplet n, a1, b1, c1, a2, b2, c2 ;

  n = u_normale_triangle(aa, bb, cc) ;
  n = u_normalise_triplet(&n) ;
  n = u_triplet_fois_flottant(&n, epaisseur/2) ;
  a1 = u_ajoute_triplet(aa, &n) ;
  b1 = u_ajoute_triplet(bb, &n) ;
  c1 = u_ajoute_triplet(cc, &n) ;
  n = u_triplet_fois_flottant(&n, -1) ;
  a2 = u_ajoute_triplet(aa, &n) ;
  b2 = u_ajoute_triplet(bb, &n) ;
  c2 = u_ajoute_triplet(cc, &n) ;

  (*triangle)(f, &a1, &b1, &c1) ;
  (*triangle)(f, &c2, &b2, &a2) ;
  (*quadrangle)(f, &a1, &a2, &b2, &b1) ;
  (*quadrangle)(f, &b1, &b2, &c2, &c1) ;
  (*quadrangle)(f, &c1, &c2, &a2, &a1) ;
}

void t_triangle_stl_epais_(FILE *f, const Triplet *aa, const Triplet *bb, const Triplet *cc, float epaisseur)
{
  t_triangle_stl_epais_general(f,aa,bb,cc,epaisseur,t_triangle_stl_, t_quadrangle_stl_) ;
}

void t_triangle_stl_epais(FILE *f, const Triplet *aa, const Triplet *bb, const Triplet *cc, float epaisseur)
{
  t_triangle_stl_epais_general(f,aa,bb,cc,epaisseur,t_triangle_stl, t_quadrangle_stl) ;
}

void t_triangle_stl_offset(FILE *f, const Triplet *aa, const Triplet *bb, const Triplet *cc
			   , const Triplet *aao, const Triplet *bbo, const Triplet *cco)
{
  t_triangle_stl(f, aa,bb,cc) ;
  if ( globale_etat_courant.stl_offset )
    t_triangle_stl(f, cco,bbo,aao) ;
}


static FILE *global_stl_fichier = NULL ;
static int global_stl_nb_points = 0 ;
static int global_stl_nb_max_points = 0 ;
static Triplet *global_stl_points = NULL ;

void t_debut_polygone_stl(FILE *f)
{
  if ( global_stl_fichier || global_stl_nb_points )
    {
      eprintf("Vous avez fait 2 't_debut_polygone_stl' sans terminer\n") ;
    }
  global_stl_fichier = f ;
  global_stl_nb_points = 0 ;
}
void t_point_stl(const Triplet *pt)
{
  if ( global_stl_nb_points == global_stl_nb_max_points )
    {
      global_stl_nb_max_points = 1.5*global_stl_nb_max_points + 10 ;
      REALLOUER(global_stl_points, global_stl_nb_max_points) ;
    }
  global_stl_points[global_stl_nb_points++] = *pt ;
}

void t_point_stl_offset(const Triplet *pt, const Triplet *pt2)
{
  t_point_stl(pt) ;
  t_point_stl(pt2) ;
}

void t_fin_polygone_stl()
{
  Triplet milieu ;
  int i ;

  if ( global_stl_nb_points )
    {
      u_annule_triplet(&milieu) ;
      for(i=0;i<global_stl_nb_points; i++)
	milieu = u_ajoute_triplet(&milieu, &global_stl_points[i]) ;
      milieu = u_triplet_fois_flottant(&milieu, 1./global_stl_nb_points) ;
      
      for(i=0;i<global_stl_nb_points; i++)
	{
	  t_triangle_stl(global_stl_fichier
			 , &global_stl_points[i]
			 , &global_stl_points[(i+1)%global_stl_nb_points]
			 , &milieu
			 ) ;
	}
    }

  global_stl_fichier = NULL ;
  global_stl_nb_points = 0 ;
}

void t_fin_polygone_stl_epais_(float epaisseur)
{
  Triplet milieu ;
  int i ;

  if ( global_stl_nb_points )
    {
      u_annule_triplet(&milieu) ;
      for(i=0;i<global_stl_nb_points; i++)
	milieu = u_ajoute_triplet(&milieu, &global_stl_points[i]) ;
      milieu = u_triplet_fois_flottant(&milieu, 1./global_stl_nb_points) ;
      
      for(i=0;i<global_stl_nb_points; i++)
	{
	  t_triangle_stl_epais_(global_stl_fichier
				, &global_stl_points[i]
				, &global_stl_points[(i+1)%global_stl_nb_points]
				, &milieu
				, epaisseur
				) ;
	}
    }

  global_stl_fichier = NULL ;
  global_stl_nb_points = 0 ;
}

void t_fin_polygone_stl_offset()
{
  Triplet milieu, milieu2 ;
  int i ;

  if ( global_stl_nb_points )
    {
      u_annule_triplet(&milieu) ;
      for(i=0;i<global_stl_nb_points; i+=2)
	milieu = u_ajoute_triplet(&milieu, &global_stl_points[i]) ;
      milieu = u_triplet_fois_flottant(&milieu, 2./global_stl_nb_points) ;

      u_annule_triplet(&milieu2) ;
      for(i=1;i<global_stl_nb_points; i+=2)
	milieu2 = u_ajoute_triplet(&milieu2, &global_stl_points[i]) ;
      milieu2 = u_triplet_fois_flottant(&milieu2, 2./global_stl_nb_points) ;
      
      for(i=0;i<global_stl_nb_points; i+=2)
	{
	  t_triangle_stl(global_stl_fichier
			 , &global_stl_points[i]
			 , &global_stl_points[(i+2)%global_stl_nb_points]
			 , &milieu
			 ) ;
	  if ( globale_etat_courant.stl_offset )
	    t_triangle_stl(global_stl_fichier
			   , &global_stl_points[(i+3)%global_stl_nb_points]
			   , &global_stl_points[(i+1)%global_stl_nb_points]
			   , &milieu2
			   ) ;
	}
    }

  global_stl_fichier = NULL ;
  global_stl_nb_points = 0 ;
}

/* Copier/coller appel t_triangle_stl_ au lieu de t_triangle_stl */

void t_fin_polygone_stl_offset_()
{
  Triplet milieu, milieu2 ;
  int i ;

  if ( global_stl_nb_points )
    {
      u_annule_triplet(&milieu) ;
      for(i=0;i<global_stl_nb_points; i+=2)
	milieu = u_ajoute_triplet(&milieu, &global_stl_points[i]) ;
      milieu = u_triplet_fois_flottant(&milieu, 2./global_stl_nb_points) ;

      u_annule_triplet(&milieu2) ;
      for(i=1;i<global_stl_nb_points; i+=2)
	milieu2 = u_ajoute_triplet(&milieu2, &global_stl_points[i]) ;
      milieu2 = u_triplet_fois_flottant(&milieu2, 2./global_stl_nb_points) ;
      
      for(i=0;i<global_stl_nb_points; i+=2)
	{
	  t_triangle_stl_(global_stl_fichier
			 , &global_stl_points[i]
			 , &global_stl_points[(i+2)%global_stl_nb_points]
			 , &milieu
			 ) ;
	  if ( globale_etat_courant.stl_offset )
	    t_triangle_stl_(global_stl_fichier
			    , &global_stl_points[(i+3)%global_stl_nb_points]
			    , &global_stl_points[(i+1)%global_stl_nb_points]
			    , &milieu2
			    ) ;
	}
    }

  global_stl_fichier = NULL ;
  global_stl_nb_points = 0 ;
}

/*
 *
 */
static Triplet pt_cyl(const Transfo *t, float lat, float lon, float rayon)
{
  Triplet x,y,z ;

  if ( lon > 2*M_PI - 0.001 )
    lon = 0 ;

  z = u_triplet_fois_flottant(&t->v3, lat) ;
  y = u_triplet_fois_flottant(&t->v2, sin(lon)*rayon) ;
  x = u_triplet_fois_flottant(&t->v1, cos(lon)*rayon) ;

  x = u_ajoute_triplet(&x, &y) ;
  x = u_ajoute_triplet(&x, &z) ;
  x = u_ajoute_triplet(&x, &t->origine) ;

  return(x) ;
}

void t_general_cylindre_stl(FILE *f, const Triplet *p, const Triplet *q
			    , float rayon, int nb_faces, Booleen capped
			    , void (*trace_triangle)(FILE *, const Triplet *, const Triplet *, const Triplet *)
			    )
{
  Transfo t ;
  Triplet a, b, c, d ;
  int j ;
  static int n ;

  t.v3 = u_soustrait_triplet(q, p) ; 

  if ( capped )
    {      
      t.v3 = u_normalise_triplet(&t.v3) ;
      t.v3 = u_triplet_fois_flottant(&t.v3, rayon) ;
      a = u_soustrait_triplet(p, &t.v3) ;
      b = u_ajoute_triplet(q, &t.v3) ;
      t_general_cylindre_stl(f, &a, &b, rayon, nb_faces, Faux, trace_triangle) ;
      return ;
    }

  u_repere_de_vecteur(&t.v3, &t.v1, &t.v2) ;
  t.v1 = u_normalise_triplet(&t.v1) ;
  t.v2 = u_normalise_triplet(&t.v2) ;
  t.origine = *p ;
	  
  for(j=0;j<nb_faces;j++)
    {
      a = pt_cyl(&t ,0 ,j    *2*M_PI/nb_faces, rayon) ;
      b = pt_cyl(&t ,0 ,(j+1)*2*M_PI/nb_faces, rayon) ;
      c = pt_cyl(&t ,1 ,(j+1)*2*M_PI/nb_faces, rayon) ;
      d = pt_cyl(&t ,1 ,j    *2*M_PI/nb_faces, rayon) ;
	      
      (*trace_triangle)(f, &a, &b, &c) ;
      (*trace_triangle)(f, &a, &c, &d) ;
      (*trace_triangle)(f, &b, &a, &t.origine) ;
      (*trace_triangle)(f, &d, &c, q) ;
    }
  n++ ;
}

void t_cylindre_stl_(FILE *f, const Triplet *p, const Triplet *q, float rayon, int nb_faces)
{
  t_general_cylindre_stl(f, p, q, rayon, nb_faces, Faux, t_triangle_stl_) ;
}

void t_cylindre_stl(FILE *f, const Triplet *p, const Triplet *q, float rayon, int nb_faces)
{
  t_general_cylindre_stl(f, p, q, rayon, nb_faces, Faux, t_triangle_stl) ;
}

void t_capped_cylindre_stl(FILE *f, const Triplet *p, const Triplet *q, float rayon, int nb_faces)
{
  t_general_cylindre_stl(f, p, q, rayon, nb_faces, Vrai, t_triangle_stl) ;  
}

void t_sphere_capped_cylindre_stl(FILE *f, const Triplet *p, const Triplet *q, float rayon, int nb_faces)
{
  t_general_cylindre_stl(f, p, q, rayon, nb_faces, Faux, t_triangle_stl) ;  
  t_sphere_stl(f,p,rayon,nb_faces/2,nb_faces) ;
  t_sphere_stl(f,q,rayon,nb_faces/2,nb_faces) ;
}

static Triplet pt_sphere(const Triplet *centre, float rayon, float lat, float lon)
{
  Triplet pt ;

  if ( lon > 2*M_PI - 0.001 )
    lon = 0 ;

  pt.x = centre->x + rayon*cos(lat)*cos(lon) ;
  pt.y = centre->y + rayon*cos(lat)*sin(lon) ;
  pt.z = centre->z + rayon*sin(lat) ;
  return(pt) ;
}

void t_sphere_stl_general(FILE *f, const Triplet *p, float rayon, int nb_lat, int nb_lon, void (*triangle)(FILE*,const Triplet*,const Triplet*,const Triplet*))
{
  float lat, lon, d_lat, d_long ;
  Triplet p1, p2, p3, p4 ;

  d_lat = M_PI/nb_lat ;
  d_long = 2*M_PI/nb_lon ;

  p1 = pt_sphere(p, rayon, -M_PI/2, 0) ;
  for( lon = 0; lon < 2*M_PI - d_long/2 ; lon += d_long )
    {
      p3 = pt_sphere(p, rayon, -M_PI/2+d_lat, lon+d_long) ;
      p4 = pt_sphere(p, rayon, -M_PI/2+d_lat, lon) ;
      (*triangle)(f, &p1, &p3, &p4) ;
    }

  for( lat = -M_PI/2 + d_lat ; lat < M_PI/2 - d_lat - d_lat/2 ; lat += d_lat )
    {
      for( lon = 0; lon < 2*M_PI - d_long/2 ; lon += d_long )
	{
	  p1 = pt_sphere(p, rayon, lat, lon) ;
	  p2 = pt_sphere(p, rayon, lat, lon+d_long) ;
	  p3 = pt_sphere(p, rayon, lat+d_lat, lon+d_long) ;
	  p4 = pt_sphere(p, rayon, lat+d_lat, lon) ;
	  (*triangle)(f, &p1, &p2, &p3) ;
	  (*triangle)(f, &p1, &p3, &p4) ;
	}
    }

  p1 = pt_sphere(p, rayon, M_PI/2, 0) ;
  for( lon = 0; lon < 2*M_PI - d_long/2 ; lon += d_long )
    {
      p3 = pt_sphere(p, rayon, M_PI/2-d_lat, lon+d_long) ;
      p4 = pt_sphere(p, rayon, M_PI/2-d_lat, lon) ;
      (*triangle)(f, &p1, &p4, &p3) ;
    }
}

void t_sphere_stl(FILE *f, const Triplet *p, float rayon, int nb_lat, int nb_lon)
{
  t_sphere_stl_general(f, p, rayon, nb_lat, nb_lon, t_triangle_stl) ;
}

void t_sphere_stl_(FILE *f, const Triplet *p, float rayon, int nb_lat, int nb_lon)
{
  t_sphere_stl_general(f, p, rayon, nb_lat, nb_lon, t_triangle_stl_) ;
}
