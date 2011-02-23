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
#ifndef DELAUNAY_H
#define DELAUNAY_H

typedef int Germ ;
struct delaunay ;
#define Region REgion /* To avoid a nme clash with X11 */
/*
 * A region is defined as a pair-of-germ set.
 */
typedef struct region
{
  int nb_pairs ;		/* Number of germs couple */
  struct pair
  {
    Germ in ;			/* Inside germ */
    Germ out ;			/* Outside germ */
  } *pairs ;
} Region ;
/*
 * The node of the graphe contains two part.
 *   - The first part is a classical Delaunay node with the Germ information,
 *     the list of connected germ (neighbor region) and a flag for
 *     recursive algorithms.
 *   - The second part is set when the region is updated by a new germ.
 *     It points on the region updated and the region that create this update.
 */
typedef struct dnode
{
  Germ value ;			/* Informations on the germ */
  int nb_connected ;		/* Number of neighbors */
  struct dnode **germs ;	/* The neighbors */
  int flag ;			/* Recursion stop flag */
  struct delaunay *delaunay ;	/* Stored here to remove a parameter in fcts */

  struct dnode *shrinked ;	/* This region updated */
  struct dnode *shrinker ;	/* The region that reduced this one */
} DNode ;
/*
 * A Delaunay structure
 */
typedef struct delaunay
{
  int (*empty_region)(const Region*) ;/* To test if a region is empty */
  void (*print_germ)(Germ) ;	/* To print germ informations */
  DNode *root ;			/* Root of the hierarchical delaunay */
  int flag ;			/* Current value of the recursion flag */
} Delaunay ;
/*
 * Initialisations
 */
void initialise_delaunay(Delaunay *,
			 int (*empty_region)(const Region*),
			 void (*print)(Germ)) ;
/*
 * Add a new germ to Delaunay
 */
void add_a_new_germ(Delaunay *, Germ n) ;
/*
 * Printing the Delaunay and Hierarchical Delaunay
 */
void dump_delaunay(Delaunay *) ;
void dump_history_delaunay(Delaunay *) ;
/*
 * Utilities to navigate throught the Delaunay
 */
void reset_flag(Delaunay*) ;	/* Unmark all nodes */
int flagged(DNode *) ;		/* True if marked, and mark it if not */
DNode *root_node(Delaunay*) ;	/* The root of the hierarchical Delaunay */
DNode *a_region(Delaunay*) ;	/* A region of the current Delaunay */
Region* create_region(DNode *g);/* Create a region from the node */
void dump_DNode(DNode *g) ;	/* For debugging */
void free_delaunay(Delaunay *d) ;
/*
 *
 */
#define MALLOC(X,NB) if ( (X = malloc(sizeof(*(X)) * (NB) )) == 0 )\
                             fprintf( stderr, "No more memory\n") ;
#define REALLOC(X,NB) if ( (X = (X) ? realloc(X,sizeof(*(X)) * (NB)) : malloc(sizeof(*(X)) * (NB)))  == 0 )\
                            fprintf( stderr,"No more memory\n")

#endif
