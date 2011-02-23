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
/*
 * Créé le Mon Feb  8 15:23:31 1999 par unknown
 */

#ifndef U_STL_H
#define U_STL_H

#include "u_geometrie.h"
#include <stdio.h>

/*
 * Triangles
 */

void t_triangle_stl(FILE *f, const Triplet *a, const Triplet *b, const Triplet *c) ;
void t_triangle_stl_(FILE *f, const Triplet *a, const Triplet *b, const Triplet *c) ;

void t_triangle_stl_offset(FILE *f, const Triplet *aa, const Triplet *bb, const Triplet *cc
			   , const Triplet *aao, const Triplet *bbo, const Triplet *cco) ;
void t_triangle_stl_offset_(FILE *f, const Triplet *aa, const Triplet *bb, const Triplet *cc
			   , const Triplet *aao, const Triplet *bbo, const Triplet *cco) ;

void t_triangle_stl_epais_(FILE *f, const Triplet *aa, const Triplet *bb, const Triplet *cc, float epaisseur) ;
void t_triangle_stl_epais(FILE *f, const Triplet *aa, const Triplet *bb, const Triplet *cc, float epaisseur) ;

/*
 * Polygones
 */

void t_debut_polygone_stl(FILE *f) ;
void t_point_stl(const Triplet *pt) ;
void t_point_stl_offset(const Triplet *pt, const Triplet *pt2) ;
void t_fin_polygone_stl() ;
void t_fin_polygone_stl_offset() ;
void t_fin_polygone_stl_offset_() ;
void t_fin_polygone_stl_epais_(float epaisseur) ;

/*
 * Quadrangles
 */

void t_quadrangle_stl(FILE *f, const Triplet *a, const Triplet *b, const Triplet *c, const Triplet *d);
void t_quadrangle_stl_(FILE *f, const Triplet*a, const Triplet *b, const Triplet *c, const Triplet *d);

/*
 * Solides
 */

void t_cylindre_stl(FILE *f, const Triplet *p, const Triplet *q, float rayon, int nb_faces);
void t_cylindre_stl_(FILE *f, const Triplet *p, const Triplet *q, float rayon, int nb_faces);
void t_capped_cylindre_stl(FILE *f, const Triplet *p, const Triplet *q, float rayon, int nb_faces) ;
void t_sphere_capped_cylindre_stl(FILE *f, const Triplet *p, const Triplet *q, float rayon, int nb_faces) ;
void t_sphere_stl(FILE *f, const Triplet *p, float rayon, int nb_lat, int nb_lon);
void t_sphere_stl_(FILE *f, const Triplet *p, float rayon, int nb_lat, int nb_lon);


#endif
