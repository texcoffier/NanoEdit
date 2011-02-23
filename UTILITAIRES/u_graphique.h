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

#ifndef U_GRAPHIQUE_H
#define U_GRAPHIQUE_H

#include "u_geometrie.h"

#define t_triplet(T) glVertex3f((T)->x, (T)->y, (T)->z)
#define t_couleur(T) glColor3f((T)->x, (T)->y, (T)->z)

void u_transfo_vers_gl(const Transfo *t, double m[16]) ;

void t_chaine(const char*) ;
void t_chaine_centree(const char *s) ;
void t_caractere(char c) ;
void t_cube() ;
void t_cube_facette() ;
void t_cercle() ;
void t_cercle_centre_rayon(const Triplet *c, Flottant r) ;
void t_disque_centre_rayon(const Triplet *c, Flottant r) ;
void t_disque() ;
void t_cube_ponctuel() ;
void t_fleche(const Triplet *base, const Triplet *sommet, int plein) ;
void t_fleche_proportionnelle(const Triplet *base, const Triplet *sommet, int plein,
			      float taille_relative) ;
void t_triangle(const Triplet *a, const Triplet *b, const Triplet *c) ;

void t_repere() ;
void u_change_repere(const Transfo *t) ;
void u_recupere_matrice_gl_dans_etat_courant() ;
Transfo u_matrice_gl(double m[16]) ;
void t_sol(const Triplet*, float) ;
void t_tag_general(int numero, const Triplet *pt, const Triplet *couleur) ;
void             t_tag(int numero, const Triplet *o ) ;
void     t_tag_origine(int numero, const Triplet *o ) ;
void  t_tag_dilatation(int numero, const Triplet *o ) ;
void    t_tag_rotation(int numero, const Triplet *o ) ;
void t_tag_destruction(int numero, const Triplet *o ) ;
void       t_tag_poids(int numero, const Triplet *o ) ;
void   t_tag_rotation2(int numero, const Triplet *pt) ;
void t_tag_origine_premier(int numero, const Triplet *o) ;
void t_tag_premier(int numero, const Triplet *pt) ;

void u_3d_2d(const Triplet *pt, Flottant *x, Flottant *y) ;
void u_2d_3d(Flottant x, Flottant y, Triplet *pt) ;

void u_multiplie_epaisseur_ligne(float v) ;
void u_multiplie_taille_point(float v) ;

#endif
