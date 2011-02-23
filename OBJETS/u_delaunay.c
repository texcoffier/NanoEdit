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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "u_delaunay.h"
/*
 * Utilities to navigate throught the Delaunay
 */
void reset_flag(Delaunay *d)
{
  d->flag++ ;
}

int flagged(DNode *g)
{
  if ( g->flag == g->delaunay->flag )
    {
      return(1) ;
    }
  else
    {
      g->flag = g->delaunay->flag ;
      return(0) ;
    }
}

DNode *root_node(Delaunay *d)
{
  return(d->root) ;
}

/*
 * Return a region of the current Delaunay.
 * It is a leaf of the hierarchical Delaunay.
 */
DNode *a_region(Delaunay *d)
{
  DNode *g ;
  g = root_node(d) ;
  if ( g )
    while( g->shrinked )
      {
	g = g->shrinked ;
      }
  return(g) ;
}
/*
 * Create a temporary region.
 * The region will be destroyed on the next call.
 * There is a free slot to allow you to add a pair of germ.
 */
Region* create_region(DNode *g)
{
  static Region r = { 0 } ;
  int i ;

  r.nb_pairs = g->nb_connected ;
  REALLOC(r.pairs, r.nb_pairs+1) ;
  for(i=0;i<g->nb_connected;i++)
    {
      r.pairs[i].in  = g->value ;
      r.pairs[i].out = g->germs[i]->value ;
    }
  return(&r) ;
}
/*
 ******************************************************************************
 * Printing the hierarchical Delaunay or the Delaunay
 ******************************************************************************
 */
void dump_DNode(DNode *g)
{
  int i ;

  fprintf(stderr,"GERM %p[%d] ", g, g->value) ;
  if ( g->delaunay->print_germ )
    (*g->delaunay->print_germ)(g->value) ;
  if ( g->shrinker )
    fprintf(stderr,", shrinked: %lx[%d], shrinker: %lx[%d]"
	    ,(long)g->shrinked, g->shrinked ? g->shrinked->value : 0
	    ,(long)g->shrinker, g->shrinker->value) ;
  fprintf(stderr,"\n\tConnected to :") ;
  for(i=0;i<g->nb_connected;i++)
    fprintf(stderr," %p[%d]", g->germs[i], g->germs[i]->value) ;
  fprintf(stderr,"\n") ;
}

static void dump_delaunay_(DNode *g, int print_only_leaves)
{
  int i ;

  if ( g==NULL || flagged(g) )
    return ;

  if ( print_only_leaves )
    {
      if ( g->shrinker == NULL )
	{
	  fprintf(stderr,"%d :", g->value) ;
	  for(i=0;i<g->nb_connected;i++)
	    fprintf(stderr, " %d", g->germs[i]->value) ;
	  fprintf(stderr,"\n") ;
	}
    }
  else
    {
      dump_DNode(g) ;
    }
  dump_delaunay_(g->shrinked, print_only_leaves) ;
  dump_delaunay_(g->shrinker, print_only_leaves) ;
}

void dump_history_delaunay(Delaunay *d)
{
  reset_flag(d) ;
  dump_delaunay_(root_node(d), 0) ;
}

void dump_delaunay(Delaunay *d)
{
  reset_flag(d) ;
  dump_delaunay_(root_node(d), 1) ;
}

/*
 * Free Delaunay
 */
void free_delaunay_1(DNode *g)
{
  if ( g == NULL )
    return ;

  if ( g->germs )
    {
      g->value = 0 ;
      free(g->germs) ;
      free_delaunay_1(g->shrinked) ;
      free_delaunay_1(g->shrinker) ;
    }
  else
    g->value++ ;
}

void free_delaunay_2(DNode *g)
{
  if ( g == NULL )
    return ;

  if ( g->value == 0 )
    {
      free_delaunay_2(g->shrinked) ;
      free_delaunay_2(g->shrinker) ;
      free(g) ;
    }
  else
    g->value-- ;
}

void free_delaunay(Delaunay *d)
{
  free_delaunay_1(root_node(d)) ;
  free_delaunay_2(root_node(d)) ;
  d->root = NULL ;
}


/*
 ******************************************************************************
 * Utilities to compute the hierarchical Delaunay
 ******************************************************************************
 */

/*
 * Aloccate and initialise a new node.
 */
static DNode* allocate_DNode(Delaunay *d, Germ n)
{
  DNode *g ;

  MALLOC(g,1) ;
  g->value        = n ;
  g->nb_connected = 0 ;
  g->shrinked     = NULL ;
  g->shrinker     = NULL ;
  g->germs        = NULL ;
  g->flag         = 0 ;
  g->delaunay     = d ;
  return(g) ;
}
/*
 * Given "g" a node and "i" the neighbor index.
 * It returns "j" so :
 *                      g->germs[i]->germs[j] = g ;
 */
static int reverse_link(DNode *g, int i)
{
  DNode *c ;
  int j ;

  c = g->germs[i] ;
  for(j=0;j<c->nb_connected;j++)
    if ( c->germs[j] == g )
      return(j) ;

  fprintf(stderr,"Bug !, link not symetric\n") ;
  dump_DNode(g) ;
  dump_DNode(c) ;
  return(0) ;
}
/*
 * Remove a neighbor from a node.
 * The graph arc is removed from the two sides (the node and its neighbor)
 */
static void remove_this_connection(DNode *g, int i)
{
  DNode *c ;
  int j ;
  
  c = g->germs[i] ;
  j = reverse_link(g,i) ;

  memmove(&c->germs[j], &c->germs[j+1], (c->nb_connected-j-1)*sizeof(DNode*));
  c->nb_connected-- ;
  memmove(&g->germs[i], &g->germs[i+1], (g->nb_connected-i-1)*sizeof(DNode*));
  g->nb_connected-- ;
}
/*
 * Shrink a region.
 *    (1) Duplicate the region and add the shrinker as neighbor
 *    (2) Make the neighbors point on the shrinked region
 *        in place of the original
 *    (3) Add the shrinked region as neighbor of shrinker
 * Remark :
 *    The links to the neighbors are not symetrics
 *    on nodes that are not leaf of the hierarchical Delaunay.
 *    These upper level links are only used to create "region".
 */

static void update_node(DNode *g, DNode *shrinker)
{
  int i,j ;
  /* 1 */
  g->shrinked = allocate_DNode(g->delaunay, g->value) ;
  MALLOC(g->shrinked->germs, g->nb_connected+1) ;
  memcpy(g->shrinked->germs, g->germs, g->nb_connected*sizeof(g->germs[0]));
  g->shrinked->germs[g->nb_connected] = shrinker ;
  g->shrinked->nb_connected = g->nb_connected + 1 ;
  /* 2 */
  for(i=0;i<g->nb_connected;i++)
    {
      j = reverse_link(g,i) ;
      g->shrinked->germs[i]->germs[j] = g->shrinked ;
    }
  /* 3 */
  g->shrinker = shrinker ;
  REALLOC(shrinker->germs, ++shrinker->nb_connected) ;
  shrinker->germs[shrinker->nb_connected-1] = g->shrinked ;
}
/*
 * Try to shrink a region by a given node.
 */
static void try_to_shrink(DNode *g, DNode *ng)
{
  Region *r ;
  
  if ( flagged(g) )		/* Stop recursion */
    return ;
  /*
   * Create the intersection region of the germ and the new germ
   */
  r = create_region(g) ;
  r->pairs[r->nb_pairs].out = g->value ;
  r->pairs[r->nb_pairs].in  = ng->value ;
  r->nb_pairs++ ;
  if ( (*g->delaunay->empty_region)(r) )
    return ;			/* The germ doesn't modify the region */

  if ( g->shrinker )
    {
      /*
       * If we are on a non terminal node continue recursion
       */
      try_to_shrink(g->shrinked, ng) ;
      try_to_shrink(g->shrinker, ng) ;
    }
  else
    { 
      /*
       * If we are on a leaf, the leaf is updated to become a node
       */
      update_node(g,ng) ;
    }
}
/*
 * Test if for a region, the neighbor "i" is useful.
 * If it is useful, it reduces the region.
 * So, we test if the trimmed part is empty or not.
 */
static int is_not_a_shrinker(DNode *g, int i)
{
  Region *r ;
  Germ tmp ;

  r = create_region(g) ;

  tmp = r->pairs[i].out ;
  r->pairs[i].out = r->pairs[i].in ;
  r->pairs[i].in = tmp ;

  return( (*g->delaunay->empty_region)(r) ) ;
}
/*
 * Remove the unnecessary neighbor of a region.
 * So test if each neighbor is a shrinker or shrinkered.
 * THE TWO TESTS MUST BE DONE.
 */
static void remove_connection(DNode *g)
{
  int i ;
  
  (void)flagged(g) ;
  for(i=g->nb_connected-2;i>=0;i--) /* -2 because the last one is OK */
    {
      if ( g->germs[i]->flag == g->delaunay->flag )
	continue ;
      
      if (    is_not_a_shrinker(g,i)
	      && is_not_a_shrinker(g->germs[i],reverse_link(g,i)) )
	remove_this_connection(g,i) ;
    }
}
/*
 * Add a new germ to hierarchical Delaunay
 */
void add_a_new_germ(Delaunay *d, Germ n)
{
  DNode *new_germ ;		/* The DNode to add into the Delaunay */
  int i ;

  new_germ = allocate_DNode(d, n) ;
  if ( d->root==NULL )
    {
      d->root = new_germ ;	/* First germ of the Delaunay */
      return ;
    }
  reset_flag(d) ;		/* Reset all markers (constant time) */
  try_to_shrink(d->root,new_germ); /* Update hierarchical Delaunay */

  if ( new_germ->nb_connected == 0 )
    {
      free(new_germ) ;		/* The new germ doesn't shrink any region */
    }
  else
    {
      /*
       * For all the regions shrinked by the new germ,
       * We try to remove neighbor that are no more neighbor.
       */
      reset_flag(d) ;
      for(i=new_germ->nb_connected-1;i>=0;i--)
	{
	  remove_connection(new_germ->germs[i]) ;
	}
    }
}
/*
 *
 */
void initialise_delaunay(Delaunay *d,
			 int (*empty_region)(const Region*),
			 void (*print)(Germ))
{
  d->empty_region    = empty_region ;
  d->print_germ      = print ;
  d->flag            = 0 ;
  d->root            = NULL ;
}


