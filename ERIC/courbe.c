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
/* #include <math.h> */
#include "util.h"
#include "point.h"
#include "matrice.h"
#include "controle.h"
#include "ifs.h"

/*******************/
/* COURBES A POLES */
/*******************/

/*
   matrices definies en coordonnees barycentriques 
   arcs controles par des polygones e0,e1,..,em 
   m = dimension de l'espace d'iteration
       (= nombre de points de controle - 1)
       (~ degre de polynome)
   cas les plus simples :
   m = 2 ==> triangle de controle e0,e1,e2 (degre 2)
   m = 3 ==> quadrilatere de controle e0,e1,e2.e3 (degre 3)
*/
/*
   nb = nombre de subdivisions de l'arc
        (= nombre de transfo dans l'ifs) 
   cas les plus courants :
   nb = 2 ==> subdivisions binaires
   nb = 3 ==> subdivisions ternaires 
*/
/* 
   transfo Ti definies par l'image des vecteurs homogenes de base ek 
   Ti ek = qik
   Cas binaire Tk k=0,1 :
   - e0,em extremites de l'arc : 
     T0 e0 = e0, T1 em = em ==> F(0) = e0, F(1) = em 
   qi point de raccord et d'interpolation : 
     T0 em = T1 e0 = qi ==> F(0.5) = qi 
   qk1,..,qkm-1 k=0.1 : images des sommets intermediaires e1,..,em-1 :
     Tk e1 = qk1,..., Tk em-1 = qkm-1 ==> controle aspect de la courbe
   arc_bm depend de 2(m-1)+1 points : 
     - 1 point d'interpolation qi
     - 2(m-1) points de controle internes qk1,...,qkm-1 
*/

/*-----------*/
/* Courbe 1D */
/* (segment) */
/*-----------*/

/* Arcs binaires 1D */
IFS arc_b1(Flottant r)
 {
  IFS ifsr;
  MATH t0,t1;
  t0 = math2x2(1,1-r,0,r);
  t1 = math2x2(1-r,0,r,1);
  ifsr = ifs_math2(t0,t1);
  destru_math(t0);
  destru_math(t1);
  return ifsr;
 }

/*------------*/
/* Courbes 2D */
/*------------*/

/* Arcs binaires 2D */
/* 
   e0,e2 extremites de l'arc : T0 e0 = e0, T1 e2 = e2 
   qi = qi point de raccord : T0 e2 = T1 e0 = qi 
   q01,q11 images du sommet du triangle e1 : T0 e1 = q0,  T1 e1 = q1 
*/
IFS arc_b2(VECH q01, VECH qi, VECH q11)
 {
  IFS ifsr;
  MATH t0,t1;
  VECH e0,e2;
  e0 = vh3(1,0,0);
  e2 = vh3(0,0,1);
  t0 = math_pt3(e0,q01,qi);
  t1 = math_pt3(qi,q11,e2);
  ifsr = ifs_math2(t0,t1);
  destru_pt(e0);
  destru_pt(e2);
  destru_math(t0);
  destru_math(t1);
  return ifsr;
 }
/* Arc "tangent" aux extremites (cisaillements) */
/*
   conservation des droites <e0,e1>, <e1,e2>
   T0<e0,e1> <_ <e0,e1> et T1<e2,e1> <_ <e2,e1>
   q01 sur <e0,e1>, q11 sur <e2,e1> : 
   T0 e1 = q01 = (1-r01) e0 + r01 e1
   T1 e1 = q11 = (1-r12) e1 + r12 e2
*/
IFS arc_b2_tg(Flottant r01, VECH qi, Flottant r12)
 {
  IFS ifsr;
  VECH q01, q11;
  q01 = vh3(1-r01,r01,0);
  q11 = vh3(0,1-r12,r12);
  ifsr = arc_b2(q01,qi,q11);
  destru_pt(q01);
  destru_pt(q11);
  return ifsr;
 }
/* Arc lisse C1 (schema de Bezier) */
/* 
   q01 sur <e0,e1>, q11 sur <e2,e1> et qi sur <q01,q11> : 
   q11 = (1-r012)q01 + r012 q11
*/
IFS arc_b2_c1(Flottant r01, Flottant r12, Flottant r012) 
 {
  IFS ifsr;
  VECH q01, qi, q11;
  q01 = vh3(1-r01,r01,0);
  q11 = vh3(0,1-r12,r12);
  qi = vh_caf(q01,q11,r012);
  ifsr = arc_b2(q01, qi, q11);
  destru_pt(q01);
  destru_pt(qi);
  destru_pt(q11);
  return ifsr;
 }
/* Arc de parabole (Bezier de degre 2) */
/* 0 < r < 1 
   subdivision [0,r],[r,1] : T0 t = r t, T1 t = r + (1-r)t */
IFS arc_b2_para(Flottant r)
 {
  return arc_b2_c1(r,r,r);
 }
/* Arc en V (schema de B-spline) */
/* qi = e1 => [e0,e1] U [e1,e2] */
IFS arc_b2_c1s(Flottant r01, Flottant r12) 
 {
  IFS ifsr;
  VECH qi;
  qi = vh3(0,1,0);
  ifsr = arc_b2_tg(r01,qi,r12);
  destru_pt(qi);
  return ifsr;
 }
/* Arc en V (B-spline de degre 1) */
/* noeuds (0,r,1) ou (0,1/2,1) ? */
IFS arc_b2_bsp(Flottant r) 
 {
  return arc_b2_c1s(r,r);
 }
/* Arc symetrique */
/* r=0 => segment [e0,e2] */ 
IFS arc_b2_sym(VECH q01, Flottant r10)
 {
  IFS ifsr;
  VECH qi,q11;
  Flottant s;
  q11 = pt_crea(2); 
  q11.c[0] = q01.c[2];
  q11.c[1] = q01.c[1];
  q11.c[2] = q01.c[0];
  s = (1-r10)*0.5;
  qi = vh3(s,r10,s);
  ifsr = arc_b2(q01,qi,q11);
  destru_pt(qi);
  destru_pt(q11);
  return ifsr;
 }
/* Arc symetrique C1 */
/* r=0 => segment [e0,e2], r=0.5 => parabole, r=1 => ligne brisee [e0,e1,e2] */ 
IFS arc_b2_sym_c1(Flottant r)
 {
  return arc_b2_c1(r, 0.5, 1-r);
 }

/*------------*/
/* Courbes 3D */
/*------------*/

/* Arcs binaires 3D */
/* e0,e3 extremites de l'arc : T0 e0 = e0, T1 e3 = e3 
   q03 point de raccord : T0 e3 = T1 e0 = qi 
   q01,q11 images du sommet e1, q02,q12 images du sommet e2 :
     T0 e1 = q01, T1 e1 = q11; T0 e2 = q02, T1 e2 = q12 
*/
IFS arc_b3(VECH q01, VECH q02, VECH qi, VECH q11, VECH q12)
 {
  IFS ifsr;
  MATH t0,t1;
  VECH e0,e3;
  e0 = vh4(1,0,0,0);
  e3 = vh4(0,0,0,1);
  t0 = math_pt4(e0,q01,q02,qi);
  t1 = math_pt4(qi,q11,q12,e3);
  ifsr = ifs_math2(t0,t1);
  destru_pt(e0);
  destru_pt(e3);
  destru_math(t0);
  destru_math(t1);
  return ifsr;
 }
/* Arc "tangent" aux extremites (axes invariants) */
/* 
   <eo,e1> (resp <e2,e3>) invariants par T0 (resp T1)
*/
IFS arc_b3_tg(Flottant s01, VECH q02, VECH qi, VECH q11, Flottant s12)
 {
  IFS ifsr;
  VECH q01,q12;
  q01 = vh4(1-s01,s01,0,0);
  q12 = vh4(0,0,1-s12,s12);
  ifsr = arc_b3(q01,q02,qi,q11,q12);
  destru_pt(q01);
  destru_pt(q12);
  return ifsr;
 }
/* Arc C1 */
/* 
   tangentes : en e0 <e0,e1>, en qi <q02,q11>, en e3 <e2,e3> 
*/ 
IFS arc_b3_c1(Flottant s01, VECH q02, Flottant si, VECH q11, Flottant s12)
 {
  IFS ifsr;
  VECH q01,qi,q12;
  q01 = vh4(1-s01,s01,0,0);
  q12 = vh4(0,0,1-s12,s12);
  qi = vh_caf(q02,q11,si);
  ifsr = arc_b3(q01,q02,qi,q11,q12);
  destru_pt(q01);
  destru_pt(q12);
  destru_pt(qi);
  return ifsr;
 } 
/* Arc C1 (schema de Bezier) */
/* 
   q01 = p01 sur <e0,e1>, p12 sur <e1,e2>, q12 = p23 sur <e2,e3>;
   q02 = p012 sur <p01,p12>, q11 = p123 sur <p12,p23>;
   qi = p0123 sur <p012,p123> 
*/
IFS arc_b3_c1b(Flottant r01, Flottant r12, Flottant r23, 
	       Flottant r012, Flottant r123, Flottant r0123) 
 {
  IFS ifsr;
  VECH p01,p12,p23,p012,p123,p0123;
  p01 = vh4(1-r01,r01,0,0);
  p12 = vh4(0,1-r12,r12,0);
  p23 = vh4(0,0,1-r23,r23);
  p012 = vh_caf(p01,p12,r012);
  p123 = vh_caf(p12,p23,r123);
  p0123 = vh_caf(p012,p123,r0123);
  ifsr = arc_b3(p01,p012,p0123,p123,p23);
  destru_pt(p01);
  destru_pt(p12);
  destru_pt(p23);
  destru_pt(p123);
  destru_pt(p0123);
  return ifsr;
 }
/* Arc de cubique (Bezier de degre 3) */
IFS arc_b3_cub(Flottant r)
 {
  return arc_b3_c1b(r,r,r,r,r,r);
 }
/* Arc C1 a schema de B-spline */
/* 
   c1b sur <e0,e1,e2>, c1b sur <e1,e2,e3>, raccord C1 sur <e2,e3>;
   tangentes : en e0 <e0,e1>, en qi <q02,q11>=<e2,e3>, en e3 <e2,e3> 
*/
/*
IFS arc_b3_c1s(Flottant r01, Flottant r12, Flottant r23, 
	       Flottant r112, Flottant r122) 
 {
  IFS ifsr;
  VECH e1,e2,p12,p112,p122;
  p12 = vh4(0,1-r12,r12,0);
  p112 = vh_caf(e1,p12,r112);
  p122 = vh_caf(p12,e2,r122);
  ifsr = arc_b3_tg(r01,p112,p12,p122,r23);
  destru_pt(p01);
  destru_pt(p12);
  destru_pt(p23);
  destru_pt(p112);
  destru_pt(p122);
  return ifsr;
 }
*/
/* Arc B-spline de degre 2 */
/* 
   parabole(r01): [0,ti] -> <e0,e1,qi>, 
   parabole(r23): [ti,1] -> <qi,e2,e3>, 
   raccord C1 qi = F(ti) sur <e2,e3>;
   parametres : (r01,r12,r23)  
   noeuds (0,r01,r12,r23,1) ? 
*/
/*
IFS arc_b3_bsp(Flottant r, Flottant r12, Flottant r23)
 {
  return arc_b3_c1s(r01,r12,r23,r01,r12);
 }
*/
/* Arc symetrique */
IFS arc_b3_sym(VECH q01, VECH q02, Flottant r)
 {
  IFS ifsr;
  VECH e0,qi;
  MATH t0,t1;
  Flottant s03,s12;
  e0 = vh4(1,0,0,0);
  s03 = (1-r)*0.5;
  s12 = r*0.5;
  qi = vh4(s03,s12,s12,s03);
  t0 = math_pt4(e0,q01,q02,qi);
  t1 = math_reflex(t0); 
  ifsr = ifs_math2(t0,t1);
  destru_pt(e0);
  destru_pt(qi);
  destru_math(t0);
  destru_math(t1);
  return ifsr;
 }

/*------------*/
/* Courbes mD */ 
/*------------*/
/*
IFS arc_bi_md(L_VECH lq) 
*/
