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
#include <limits.h>
#include "u_geometrie.h"

#ifndef FLT_MAX

#ifndef MAXFLOAT
#define MAXFLOAT (1e9)
#endif

#define FLT_MAX MAXFLOAT

#endif


/*
 *
 */

Triplet u_ajoute_triplet(const Triplet *a, const Triplet *b)
{
  Triplet c ;

  c.x = a->x + b->x ;
  c.y = a->y + b->y ;
  c.z = a->z + b->z ;
  return(c) ;
}

Quadruplet u_ajoute_quadruplet(const Quadruplet *a, const Quadruplet *b)
{
  Quadruplet c ;

  c.x = a->x + b->x ;
  c.y = a->y + b->y ;
  c.z = a->z + b->z ;
  c.h = a->h + b->h ;
  return(c) ;
}
/*
 *
 */

Triplet u_soustrait_triplet(const Triplet *a, const Triplet *b)
{
  Triplet c ;

  c.x = a->x - b->x ;
  c.y = a->y - b->y ;
  c.z = a->z - b->z ;
  return(c) ;
}

/*
 *
 */

Flottant u_norme2_triplet(const Triplet *a)
{
  return( a->x*a->x + a->y*a->y + a->z*a->z ) ;
}

Flottant u_norme_triplet(const Triplet *a)
{
  return( sqrt(u_norme2_triplet(a)) ) ;
}

Flottant u_distance_triplet(const Triplet *a, const Triplet *b)
{
  Triplet pt ;
  pt = u_soustrait_triplet(a,b) ;
  return( u_norme_triplet(&pt) ) ;
}

Triplet u_pv_triplet(const Triplet *a, const Triplet *b)
{
  Triplet pt ;
  pt.x = a->y*b->z - a->z*b->y ;
  pt.y = a->z*b->x - a->x*b->z ;
  pt.z = a->x*b->y - a->y*b->x ;
  return( pt ) ;
}

Flottant u_ps_triplet(const Triplet *a, const Triplet *b)
{
  return( a->x*b->x + a->y*b->y + a->z*b->z ) ;
}

Triplet u_normalise_triplet(const Triplet *a)
{
  Flottant n ;
  Triplet t ;

  n = u_norme_triplet(a) ;
  if ( n>1e-30 )
    {
      t = u_triplet_fois_flottant(a, 1/n ) ;
      if ( t.x > 1 )
	t.x = 1 ;
      else
      if ( t.x < -1 )
	t.x = -1 ;

      if ( t.y > 1 )
	t.y = 1 ;
      else
      if ( t.y < -1 )
	t.y = -1 ;

      if ( t.z > 1 )
	t.z = 1 ;
      else
      if ( t.z < -1 )
	t.z = -1 ;
      return( t ) ;
    }
  else
    {
      eprintf("Erreur normalisation triplet\n") ;
      t.x = 1 ;
      t.y = 0 ;
      t.z = 0 ;
      return(t) ;
    }
}

Triplet u_triplet_fois_flottant(const Triplet *a, Flottant f)
{
  Triplet p ;
  p.x = a->x * f ;
  p.y = a->y * f ;
  p.z = a->z * f ;
  return(p) ;
}

Quadruplet u_quadruplet_fois_flottant(const Quadruplet *a, Flottant f)
{
  Quadruplet p ;
  p.x = a->x * f ;
  p.y = a->y * f ;
  p.z = a->z * f ;
  p.h = a->h * f ;
  return(p) ;
}

Booleen u_triplet_invalide(const Triplet *a)
{
#if 1
  return( !( finite(a->x) && finite(a->y) && finite(a->z)) ) ;
#else
  return ( a->x < -1e30 || a->x > 1e30 
	   || a->y < -1e30 || a->y > 1e30 
	   || a->z < -1e30 || a->z > 1e30 
	   ) ;
#endif
}

void u_annule_triplet(Triplet *a)
{
  a->x = a->y = a->z = 0 ;
}

void u_annule_quadruplet(Quadruplet *a)
{
  a->x = a->y = a->z = 0 ;
  a->h = 1 ;
}

void u_annule_boite_min_max(t_boite_min_max *a)
{
  a->min.x = a->min.y = a->min.z = FLT_MAX ;
  a->max.x = a->max.y = a->max.z = -FLT_MAX ;
}

void u_agrandir_boite_min_max(t_boite_min_max *a, const Triplet *b)
{
  if ( b->x < a->min.x )
    a->min.x = b->x ;
  if ( b->y < a->min.y )
    a->min.y = b->y ;
  if ( b->z < a->min.z )
    a->min.z = b->z ;

  if ( b->x > a->max.x )
    a->max.x = b->x ;
  if ( b->y > a->max.y )
    a->max.y = b->y ;
  if ( b->z > a->max.z )
    a->max.z = b->z ;
}

Triplet u_vecteur_min_boite_min_max(const t_boite_min_max *a)
{
  Triplet diagonale ;

  diagonale = u_soustrait_triplet(&a->max, &a->min) ;

  if ( diagonale.x > diagonale.y )
    {
      diagonale.x = 0 ;
      if ( diagonale.y > diagonale.z )
	diagonale.y = 0 ;
      else
	diagonale.z = 0 ;
    }
  else
    {
      diagonale.y = 0 ;
      if ( diagonale.x > diagonale.z )
	diagonale.x = 0 ;
      else
	diagonale.z = 0 ;
    }
  return(diagonale) ;
}

Triplet u_vecteur_max_boite_min_max(const t_boite_min_max *a)
{
  Triplet diagonale ;

  diagonale = u_soustrait_triplet(&a->max, &a->min) ;

  if ( diagonale.x > diagonale.y )
    {
      diagonale.y = 0 ;
      if ( diagonale.x > diagonale.z )
	diagonale.z = 0 ;
      else
	diagonale.x = 0 ;
    }
  else
    {
      diagonale.x = 0 ;
      if ( diagonale.y > diagonale.z )
	diagonale.z = 0 ;
      else
	diagonale.y = 0 ;
    }
  return(diagonale) ;
}

void u_observateur_de_boite_min_max(const t_boite_min_max *a, t_observateur *o)
{
  Triplet regard, maxi ;

  o->point_vise = u_milieu_2_triplet(&a->min, &a->max) ;

  regard = u_vecteur_min_boite_min_max(a) ;
  regard = u_normalise_triplet(&regard) ;
  maxi = u_vecteur_max_boite_min_max(a) ;
  regard = u_triplet_fois_flottant(&regard, u_norme_triplet(&maxi));

  o->point_de_vue = u_ajoute_triplet(&o->point_vise, &regard) ;
  o->haut = u_pv_triplet(&regard, &maxi) ;
  o->focale = 1 ;
}

/*
 * Pas optimum, a ameliorer
 */
Triplet u_rotation_triplet(const Triplet *v,
			   const Triplet *axe,
			   Flottant angle)
{
  Triplet v_norme, vx, vy, vz ;
  Flottant x, y, z, norme ;

  norme     = u_norme_triplet(v) ;
  if ( norme < 1.e-4 )
    return(*v) ;      
  v_norme   = u_triplet_fois_flottant(v, 1/norme) ;
  /*
   * trouve le repere norme
   */
  norme     = u_norme_triplet(axe) ;
  if ( norme < 1.e-4 )
    return(*v) ;      
  vz        = u_triplet_fois_flottant(axe, 1/norme) ;
  vy        = u_pv_triplet(&vz,&v_norme) ;
  if ( u_norme_triplet(&vy) < 1.e-4 )
    return(*v) ;
  vy        = u_normalise_triplet(&vy) ;
  vx        = u_pv_triplet(&vy,&vz) ; 
  vx        = u_normalise_triplet(&vx) ;
  /*
   * Trouve les coordonnees du vecteur dans ce repere
   */
  z 	    = u_ps_triplet(&vz, v) ;
  x 	    = u_ps_triplet(&vx, v) ;
  /*
   * Rotation des coordonnees
   */
  y  = x*sin(angle) ;
  x *= cos(angle) ;
  /*
   * Creation du vecteur resultat
   */
  vx = u_triplet_fois_flottant(&vx, x) ;
  vy = u_triplet_fois_flottant(&vy, y) ;
  vz = u_triplet_fois_flottant(&vz, z) ;

  vx = u_ajoute_triplet(&vx, &vy) ;
  vx = u_ajoute_triplet(&vx, &vz) ;
  return(vx) ;
}

/*
 *
 */

Triplet u_transforme(const Transfo *t, const Triplet *p)
{
  Triplet r ;

  r.x = t->v1.x*p->x + t->v2.x*p->y + t->v3.x*p->z + t->origine.x ;
  r.y = t->v1.y*p->x + t->v2.y*p->y + t->v3.y*p->z + t->origine.y ;
  r.z = t->v1.z*p->x + t->v2.z*p->y + t->v3.z*p->z + t->origine.z ;

  return(r) ;
}

void u_annule_transfo(Transfo *a)
{
  u_annule_triplet(&a->origine) ;
  u_annule_triplet(&a->v1) ;
  u_annule_triplet(&a->v2) ;
  u_annule_triplet(&a->v3) ;
}

void u_identite_transfo(Transfo *a)
{
  u_annule_triplet(&a->origine) ;
  u_annule_triplet(&a->v1) ;
  u_annule_triplet(&a->v2) ;
  u_annule_triplet(&a->v3) ;
  a->v1.x = 1 ;
  a->v2.y = 1 ;
  a->v3.z = 1 ;
}

Transfo u_produit_matrice_transfo(const Transfo *g, const Transfo *d)
{
  Transfo r ;

  r.origine.x = r.origine.y = r.origine.z = 0. ; // To please compiler
  r.v1.x = g->v1.x*d->v1.x + g->v2.x*d->v1.y + g->v3.x*d->v1.z ;
  r.v1.y = g->v1.y*d->v1.x + g->v2.y*d->v1.y + g->v3.y*d->v1.z ;
  r.v1.z = g->v1.z*d->v1.x + g->v2.z*d->v1.y + g->v3.z*d->v1.z ;
  r.v2.x = g->v1.x*d->v2.x + g->v2.x*d->v2.y + g->v3.x*d->v2.z ;
  r.v2.y = g->v1.y*d->v2.x + g->v2.y*d->v2.y + g->v3.y*d->v2.z ;
  r.v2.z = g->v1.z*d->v2.x + g->v2.z*d->v2.y + g->v3.z*d->v2.z ;
  r.v3.x = g->v1.x*d->v3.x + g->v2.x*d->v3.y + g->v3.x*d->v3.z ;
  r.v3.y = g->v1.y*d->v3.x + g->v2.y*d->v3.y + g->v3.y*d->v3.z ;
  r.v3.z = g->v1.z*d->v3.x + g->v2.z*d->v3.y + g->v3.z*d->v3.z ;

  return r ;
}
Transfo u_produit_transfo(const Transfo *g, const Transfo *d)
{
  Transfo r ;

  r = u_produit_matrice_transfo(g, d) ;
  r.origine = u_transforme(g, &d->origine) ;
  /* 19/11/2000
  r.origine = u_ajoute_triplet(&r.origine, &g->origine) ;
  */
  return(r) ;
}

Triplet u_ponderation_2_triplet(const Triplet *a, Flottant pa,
				const Triplet *b, Flottant pb)
{
  Triplet ap, bp ;

  ap = u_triplet_fois_flottant(a, pa) ;
  bp = u_triplet_fois_flottant(b, pb) ;
  return( u_ajoute_triplet(&ap, &bp) ) ;
}

Triplet u_barycentre_2_triplet(const Triplet *a, Flottant pa,
				const Triplet *b)
{
  return( u_ponderation_2_triplet(a, 1-pa, b, pa) ) ;
}

Triplet u_milieu_2_triplet(const Triplet *a, const Triplet *b)
{
  return( u_ponderation_2_triplet(a, .5, b, .5) ) ;
}


Quadruplet u_ponderation_2_quadruplet(const Quadruplet *a, Flottant pa,
				const Quadruplet *b, Flottant pb)
{
  Quadruplet ap, bp ;

  ap = u_quadruplet_fois_flottant(a, pa) ;
  bp = u_quadruplet_fois_flottant(b, pb) ;
  return( u_ajoute_quadruplet(&ap, &bp) ) ;
}

Quadruplet u_barycentre_2_quadruplet(const Quadruplet *a, Flottant pa,
				const Quadruplet *b)
{
  return( u_ponderation_2_quadruplet(a, 1-pa, b, pa) ) ;
}

Quadruplet u_milieu_2_quadruplet(const Quadruplet *a, const Quadruplet *b)
{
  return( u_ponderation_2_quadruplet(a, .5, b, .5) ) ;
}


/*
 * La valeur retournée est une abscisse curviligne sur le vecteur.
 * Donc 1 indique que l'on se projète sur le bout du vecteur.
 * On projète le vecteur défini par pt-depart
 */
Flottant u_projete_triplet(Triplet *pt, Triplet *depart, Triplet *vecteur)
{
  Triplet v ;
  float n2 ;

  n2 =  u_norme2_triplet(vecteur) ;
  if ( n2 < 1e-8 )
    return(0) ;

  v = u_soustrait_triplet(pt, depart) ;
  return( u_ps_triplet(vecteur, &v) / n2 ) ;
}
/*
 *
 */
Triplet u_normale_triangle(const Triplet *a,const Triplet *b,const Triplet *c)
{
  Triplet u, v ;

  u = u_soustrait_triplet(c,b) ;
  v = u_soustrait_triplet(a,b) ;
  return( u_pv_triplet(&u,&v) ) ;
}
/*
 *
 */
/*
 * Calcul de la transformation inverse.
 */
static void enleve_ligne_colonne(Flottant in[4][4], int n, int y, int x,
				 Flottant out[4][4])
{
  int i, j, ii, jj ;

  for(i=0, ii=0 ; i<n; i++)
    if ( i != x )
      {
	for(j=0, jj=0 ; j<n; j++)
	  if ( j != y )
	    out[jj++][ii] = in[j][i] ;
	ii++ ;
      }
}
static int signe(int i)
{
  if ( i&1 )
    return(-1) ;
  else
    return(1) ;
}
static Flottant determinant(Flottant m[4][4], int n)
{
  int j ;
  Flottant tmp[4][4], s ;

  if ( n == 1 )
    return( m[0][0] ) ;

  if ( n == 2 )
    return( m[0][0]*m[1][1] - m[1][0]*m[0][1] ) ;

  s = 0 ;
  for(j=0; j<n; j++)
    {
      enleve_ligne_colonne(m, n, j, 0, tmp) ;
      s = s + m[j][0]*determinant(tmp, n-1)*signe(j) ;
    }
  return(s) ;
}
/*
 * L'inversion de matrice de taille maxi 4x4
 */
static void inversion(Flottant in[4][4], int n, Flottant out[4][4])
{
  int i, j ;
  Flottant det, tmp[4][4] ;

  det = determinant(in, n) ;

  for(j=0;j<n;j++)
    for(i=0;i<n;i++)
      {
	enleve_ligne_colonne(in, n, j, i, tmp) ;
	out[i][j] = signe(i+j)*determinant(tmp, n-1) / det ;
      }
}

void inversion4x4(const double in[16], double out[16])
{
  inversion((Flottant(*)[4])in, 4, (Flottant(*)[4])out) ;
}

Flottant determinant_transfo(const Transfo *in)
{
  return(
	 in->v1.x*(in->v2.y*in->v3.z-in->v3.y*in->v2.z) -
	 in->v1.y*(in->v2.x*in->v3.z-in->v3.x*in->v2.z) +
	 in->v1.z*(in->v2.x*in->v3.y-in->v2.y*in->v3.x)
	 ) ;
}


Transfo inversion_transfo(const Transfo *in)
{
  Transfo out ;
  Flottant det ;

  det = determinant_transfo(in) ;
  out.v1.x = (in->v2.y*in->v3.z-in->v3.y*in->v2.z)/det ;
  out.v2.x = (in->v2.z*in->v3.x-in->v2.x*in->v3.z)/det ;
  out.v3.x = (in->v2.x*in->v3.y-in->v3.x*in->v2.y)/det ;
  out.v1.y = (in->v3.y*in->v1.z-in->v1.y*in->v3.z)/det ;
  out.v2.y = (in->v1.x*in->v3.z-in->v3.x*in->v1.z)/det ;
  out.v3.y = (in->v1.y*in->v3.x-in->v1.x*in->v3.y)/det ;
  out.v1.z = (in->v1.y*in->v2.z-in->v2.y*in->v1.z)/det ;
  out.v2.z = (in->v2.x*in->v1.z-in->v1.x*in->v2.z)/det ;
  out.v3.z = (in->v2.y*in->v1.x-in->v2.x*in->v1.y)/det ;

  u_annule_triplet(&out.origine) ;

  out.origine = u_transforme(&out, &in->origine) ;
  out.origine = u_triplet_fois_flottant(&out.origine, -1) ;

  return(out) ;
}

/*
 * Trouve un repere en fonction de son vecteur z
 */
void u_repere_de_vecteur(const Triplet *z,
			 Triplet *x,
			 Triplet *y)
{
  Flottant norme_z ;

  x->x = 0 ;
  x->y = 0 ;
  x->z = 1 ;
  *x = u_pv_triplet(z,x) ;
  if ( u_norme_triplet(x) < .0001 )
    {
      /*
       * Pas de chance, c'etait colineaire
       */
      x->y = 1 ;
      x->z = 0 ;
      *x = u_pv_triplet(z,x) ;
    }

  *y = u_pv_triplet(z,x) ;

  /* 31/01/2003
  *y = u_triplet_fois_flottant(y, 1./u_norme_triplet(z)) ;
  */
  norme_z = u_norme_triplet(z) ;

  *y = u_normalise_triplet(y) ;
  *y = u_triplet_fois_flottant(y, norme_z) ;

  *x = u_normalise_triplet(x) ;
  *x = u_triplet_fois_flottant(x, norme_z) ;
}

/*
 *  0 < alpha < 2*PI     -PI/2 < beta < PI/2
 */

Triplet u_polaires_vers_cartesiennes(Flottant alpha, Flottant beta)
{
  Triplet p ;

  p.x = cos(alpha)*cos(beta) ;
  p.y = sin(alpha)*cos(beta) ;
  p.z = sin(beta) ;

  return(p) ;
}

void u_cartesiennes_vers_polaires(const Triplet *p, Flottant *alpha, Flottant *beta)
{
  Triplet pt ;
  Flottant f ;

  pt = u_normalise_triplet(p) ;

  *beta = asin(pt.z) ;

  f = cos(*beta) ;
  if ( fabs(f) < 1e-10 )
    {
      *alpha = 0 ;
      return ;
    }
  pt.x /= f ;
  pt.y /= f ;
  *alpha = atan2(pt.y, pt.z) ;
  if ( *alpha < 0 )
    *alpha += 2*M_PI ;
}

/*
 *  Intersection de 3 plans donnés par leurs équations
 */

Triplet u_intersection_3_plans(const Quadruplet *a
			       , const Quadruplet *b
			       , const Quadruplet *c)
{
  double t[4][4], inv[4][4] ;
  Triplet res ;
  int i ;
  
  for(i=0;i<4;i++)
    {
      t[i][0] = (&a->x)[i] ;
      t[i][1] = (&b->x)[i] ;
      t[i][2] = (&c->x)[i] ;
      t[i][3] = (i==3) ;
    }

  inversion4x4((double*)t, (double*)inv) ;
  res.x = inv[3][0] ;
  res.y = inv[3][1] ;
  res.z = inv[3][2] ;
  return(res) ;
}

Quadruplet u_creation_plan_lie(const Triplet *normale, const Triplet *point)
{
  Quadruplet equation ;

  equation.x = normale->x ;
  equation.y = normale->y ;
  equation.z = normale->z ;
  equation.h = -equation.x*point->x -equation.y*point->y -equation.z*point->z ;

  return(equation) ;
}

Flottant u_distance_plan_triplet_signee(const Quadruplet *p, const Triplet *a)
{
  return( p->x*a->x + p->y*a->y + p->z*a->z + p->h ) ;
}

Triplet u_point_du_plan(const Quadruplet *a)
{
  Triplet pt ;

  u_annule_triplet(&pt) ;

  if ( fabs(a->x) > fabs(a->y) )
    {
      if ( fabs(a->x) > fabs(a->z) )
	pt.x = -a->h / a->x ;
      else
	pt.z = -a->h / a->z ;
    }
  else
    {
      if ( fabs(a->y) > fabs(a->z) )
	pt.y = -a->h / a->y ;
      else
	pt.z = -a->h / a->z ;
    }
  return(pt) ;
}

Quadruplet u_plan_inverse(const Quadruplet *a)
{
  Quadruplet q ;

  q.x = -a->x ;
  q.y = -a->y ;
  q.z = -a->z ;
  q.h = -a->h ;
  return(q) ;
}
