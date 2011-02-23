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
#include "courbe.h"

/********************/
/* SURFACES A POLES */
/********************/

/*
   Carreau defini par :
   - espace d'iteration E = B(R^J) avec J grille triangulaire ou quadrangulaire
     j numero de noeud de la grille 
   - IFS de subdivision T = (T_i) avec i dans Sigma
     i indice d'operateur 
   Arcs k definis par des parties de Sigma : 
   - IFS extrait T_k = (T_k0,T_k1) 
     avec k = (k0,k1) dans Sigma x Sigma
     Phi_k(s) adressage associe avec s dans {0,1}^\omega 
   - sous-espace d'iteration 
     Ek = <Im(Phi_k)> = <q_0,..,q_nk>
     avec : 
      * Phi_k(0^omega) = Fk(0) = q_0
      * t -> j_t tel que FkJ_t(t) = max_j Fkj(t)
	fonction croissante
      * Phi_k(1^omega) = Fk(1) = q_nk 
     en posant :
      * E~k = <e_0,..,e_jnk> 
      * Qk plongement : E~k = B(R^nk) -> E
     on a : 
      * Ek = Qk E~k
      * Phi_k = Qk Phi~k
	avec Phi~k adressage associe a l'IFS  T~k = (T~k_0,T~k_1)
	tel que :
        T_ki Qk = Qk T~k_i 
   Arcs definis sur des parties de la grille :
     q_i = e_ji et Ek = B(R^Jk) avec Jk = (j0,...,jnk)
     Qk = C_Jk
   Arc inverse :
   - -k = (1,0) => parcours en sens inverse de la courbe
       * Phi~-k(s) = Phi~_k(-s) avec -0=1 et -1=0 
       * F~-k(t) = F~k(1-t)
   - Phi~k(-s) = (Qk M)(M Phi~k(-s))
     en posant :		
       * (M Q).j = Q.n-j  et M_i,j = delta_i,n-i 
       * Sym(Psi)(s) = M Psi(-s)
       * \^Sym(T_i) = M T_-i M 
     on a : 
     Phi~-k = (Qk M)Sym(Phi~k)
   - -Jk = (jnk,...,j0)
     on pose : J-k = -Jk
   - arc symetrique :
       * Fk(1-t) = Sym(Fk)(t)
       * T_k = \^Sym(T_k)
   Bordure des carreaux :
     arcs [0,1] -> B(R^Jk) plonges dans A
     k arc de bord :   
         Phi_k = Qk Phi~k
     <=> T_ki Qk = Qk T~k_i 
   Raccords des carreaux le long des arcs de bord :
         T_i Phi_k = T_j Phi_l
     <=> T_i Qk Phi~k = T_j Ql Phi~l
     <=> T_i Qk = T_j Ql et Phi~k = Phi~l
     <=> T_i Qk = T_j Ql et T~k = T~l
     raccord en sens inverse :
         T_i Phi_k = T_j Phi_-l
     <=> T_i Qk Phi~k = T_j (Ql M) Sym(Phi~l)
     <=> T_i Qk = T_j (Q_l M) et T~k = Sym^(T~l)
   Contraintes sur les arcs initiaux :
   - Phi~k = Phi~l
     => Nk = Nl et T~k_i = T~l_i pour 0<=i<=Nk-1 
     => types de bordure de carreaux
   Contraintes sur les T_i :
   - contraintes sur les aretes de bord :
     T_ki Qk = Qk T~k_i  
   - contraintes sur les aretes de raccord :
     T_i Qk = T_j Ql
     => donnee : T_i Qk = Qjl(q_internes)
*/

/*-------------*/
/* Utilitaires */
/*-------------*/

#define ke li_vide()
#define T0 ifsr.tf[0] 
#define T1 ifsr.tf[1] 
#define T2 ifsr.tf[2] 
#define T3 ifsr.tf[3] 

/* Ti de coin : (ju,kv,jw) <- (tu_iu,lq_ki,tw_iw) */
/* 
      |
    w | v
      *---
	u

   Ti Qu <- Qu Tu_iu;
   Ti Qv <- L Qki  
*/
MATH coin_tq(int m, 
	     L_INT ju, IFS ifsu, int iu, 
	     L_INT kv, MATH lqv, L_INT ki,
	     L_INT jw, IFS ifsw, int iw)
 {
  MATH tr;
  tr = math_crea(m,m);
  col_math_li_math(tr,ju,ifsu.tf[iu]);
  ecrit_li(stdout,"ju",ju);
  ecrit_li(stdout,"kv",kv);
  ecrit_li(stdout,"ki",ki);
  ecrit_math(stdout,"lqv",lqv);
  col_math_li_math_li(tr,kv,lqv,ki);
  ecrit_li(stdout,"jw",jw);
  col_math_li_math(tr,jw,ifsw.tf[iw]);
  ecrit_li(stdout,"jw",jw);
  return tr;
 }

/*------------------------*/
/* Carreaux triangulaires */
/*------------------------*/
/*
	   * 
        a / \ c 
         *---*
	   b 
*/  
/* Carreaux subdivises en 4 */
/* Arcs subdivises en 2

	   * 
       a0 /0\ c1
         *---*
     a1 /1\3/2\ c0
       *---*---*
	b0  b1

   Carte initiale C0 :
     * brins : arcs orientes
               a = (0,1); b = (1,2); c = (2,0);
     * aretes : {u,-u}
       alpha : u -> -u
     * faces :  (a,b,c),(-c,-b,-a) 
       phi :   a -> b -> c -> a ; -a -> -c -> -b -> -a 
     * sommets : {u,-w}
   Carte subdibisee C1 :
     * brins : arcs subdivises en deux
               ui = (u,i) avec u dans {a,b,c} et i dans {0,q,1}
	       brins de deux types :
               - frontiere : ui = (u,i) avec i = 0,1 
               - raccord : ui = (u,q)  
     * aretes : {(u,i),(-u,i)}
       alpha : (u,i) -> (-u,i)
     * faces : (a0,bq,c1) (a1,b0,cq) (aq,b1,c0)
       phi : (u,i) -> (phi(u),succ(i)) avec succ : 0 -> q -> 1 -> 0 
     * sommets : {(u,i),(-w,i)}
   Definition de l'IFS :
   - T0, T1, T2 fixes par les 3 coins :
       (a,b,c) -> (a0,bq,c1) (a1,b0,cq) (aq,b1,c0)
     * (0,a) = a0; (0,b) = bq; (0,c) = c1;
     * (1,a) = a1; (1,b) = b0; (1,c) = cq;
     * (2,a) = aq; (2,b) = b1; (2,c) = c0;
   - il y a 6 = 2 x 3 cas pour T3 :
       (a,b,c) -> (u,v,w)  avec (u,v,w) permutation du circuit 
       (aq,bq,cq) = ((2,a),(0,b),(1,c))
     (+) orientation sens trigo 
       => symetries des arcs 
     * T3 (a,b,c) = (-aq,-bq,-cq)
       => Phi~_u = Sym(Phi~_u) pour u = a,b,c
       condition : arcs symetriques
     * T3 (a,b,c) = (-bq,-cq,-aq)
       => Phi~_u = Sym(Phi~_u) = Phi~v pour u,v = a,b,c
       condition : arcs symetriques et egaux
     * T3 (a,b,c) = (-cq,-aq,-bq)
       condition : arcs symetriques et egaux
     (-) orientation sens inverse 
       => egalites de deux arcs 
     * T3 (a,b,c) = (cq,bq,aq)
       => Phi~_a = Phi~_c
       condition : a et c memes arcs 
     * T3 (a,b,c) <- (bq,aq,cq)
       => Phi~_a = Phi~_b
     * T3 (a,b,c) <- (aq,cq,bq)
       => Phi~_b = Phi~_c
*/
/* Grilles triangulaires */
/*
   Parties de la grille :
   - Ja cote a; 
   - Ka interieur du cote a + interieur du carreau;
   - K0 = coin du carreau;
   - K3 = milieu;
           
	   K0
          *K3* 
         K1**K2

*/
/* T du milieu */
/* Affectation generale : (u,v,w) <- (u',v',w') */
MATH milieu_tq(L_INT ju, MATH tu, L_INT jtu, 
	       L_INT jv, MATH tv, L_INT jtv, 
	       L_INT jw, MATH tw, L_INT jtw,
	       L_INT km, MATH lq, L_INT k3)
 {
  MATH tr;
  int m;
  m = dima_math(tu);
  tr = math_crea(m,m);
  col_math_li_math_li(tr,ju,tu,jtu);
  col_math_li_math_li(tr,jv,tv,jtv);
  col_math_li_math_li(tr,jw,tw,jtw);
  col_math_li_math_li(tr,km,lq,k3);
  return tr;
 }
/* fin de grille */
void fin_tq(L_INT ja, L_INT jb, L_INT jc,
	    L_INT ka, L_INT kb, L_INT kc, L_INT km,
	    L_INT k0, L_INT k1, L_INT k2, L_INT k3)
 {
  destru_li(ja);
  destru_li(jb);
  destru_li(jc);
  destru_li(ka);
  destru_li(kb);
  destru_li(kc);
  destru_li(km);
  destru_li(k0);
  destru_li(k1);
  destru_li(k2);
  destru_li(k3);
 }
#define T0_abc coin_tq(m,ja,ifsa,0,kb,lq,k0,jc,ifsc,1) 
#define T1_bca coin_tq(m,jb,ifsb,0,kc,lq,k1,ja,ifsa,1) 
#define T2_cab coin_tq(m,jc,ifsc,0,ka,lq,k2,jb,ifsb,1) 
#define mq3 km,lq,k3
#define aqi T2,li_inv(ja) 
#define bqi T0,li_inv(jb) 
#define cqi T1,li_inv(jc) 
#define T3_abc milieu_tq(ja,aqi,jb,bqi,jc,cqi,mq3)
#define aq T2,ja 
#define bq T0,jb 
#define cq T1,jc 
#define T3_cba milieu_tq(ja,cq,jb,bc,jc,aq,mq3)
#define fin_grille fin_tq(ja,jb,jc,ka,kb,kc,km,k0,k1,k2,k3)

/* Grille degre 1 */
/*
	   0
        a / \ c 
         1---2
	   b 
   
   Ja = (0,1); Jb = (1,2); Jc = (2,0);
*/
IFS surf_qt1_abc()
 {
  IFS ifsr, ifsa, ifsb,ifsc;
  MATH lq;
  int m;
  L_INT ja,jb,jc,ka,kb,kc,km,k0,k1,k2,k3;
  ifsa = arc_b1(0.5);
  ifsb = arc_b1(0.5);
  ifsc = arc_b1(0.5);
  m = 2;
  lq = math_crea(0,0);
  ja = li2(0,1);
  jb = li2(1,2);
  jc = li2(2,0);
  ka = ke;
  kb = ke;
  kc = ke;
  km = ke;
  k0 = ke;
  k1 = ke;
  k2 = ke;
  k3 = ke;
  ifsr = ifs_crea(4);
  T0 = T0_abc;
  T1 = T1_bca;
  T2 = T2_cab;
  T3 = T3_abc;
  fin_grille;
  return ifsr;
 }

/* Grille degre 2 */
/*
	   0
          / \  
       a 1---2 c
	/ \ / \
       3---4---5
	   b
  
  nb de noeuds = 6; nb de points en parametre = 3; 
  arcs  Ja = (0,1,3); Ka = (1); 
	Jb = (3,4,5); Kb = (4); 
	Jc = (5,2,0); Kc = (2); Km = ();
  Noeuds interieurs :
   I0 = (0); I1 = (1); I2 = (2); I3 = ();
*/
IFS surf_qt2_abc(IFS ifsa, IFS ifsb, IFS ifsc, VECH q0, VECH q1, VECH q2)
 {
  IFS ifsr;
  int m;
  L_INT ja,jb,jc,ka,kb,kc,km,k0,k1,k2,k3;
  MATH lq;
  m = 5;
  ja = li3(0,1,3);
  jb = li3(3,4,5);
  jc = li3(5,2,0);
  ka = li1(1);
  kb = li1(4);
  kc = li1(2);
  km = ke;
  lq = math_pt3(q0,q1,q2); 
  k0 = li1(0);
  k1 = li1(1);
  k2 = li1(2);
  k3 = ke;
  ifsr = ifs_crea(4);
  T0 = T0_abc;
  T1 = T1_bca;
  T2 = T2_cab;
  T3 = T3_abc;
  fin_grille;
  return ifsr;
 }

/*--------------------------------*/
/* Autre subdivision triangulaire */
/*--------------------------------*/
/*
     * 
    /|\
   /1|2\
  /0\|/3\
 *---*---*

*/

/*--------------------------*/
/* Carreaux quadrangulaires */
/*--------------------------*/

/* IFS
     a0 a1
     -	-
    *--*--*
 b1 | 1| 3| b1
    *--*--*
 b0 | 0| 2| b0
    *--*--*
     a0 a1

  T0:a<-a0,bc0,-b0; T1:b0, 
*/
/*
  Rot : 0 -> 2 -> 3 -> 1 -> 0
*/
#define T0_abcd coin_tq(m,ja,ifsa,0,kbc,lq,k0,jdi,ifsb,0) 
#define T2_bcda coin_tq(m,jb,ifsb,0,kcd,lq,k2,ja,ifsa,1) 
#define T3_cdab coin_tq(m,jci,ifsa,1,kda,lq,k3,jb,ifsb,1) 
#define T1_dabc coin_tq(m,jdi,ifsb,1,kab,lq,k1,jci,ifsa,0) 

/* Grille 1x1
     c
   1--3
  d|  |b
   0--2
    a
   Ja = (0,2); Jb = (2,3); Jc = (3,1); Jd = (1,0);
   Kbc = (3); Kcd = (1); Kda = (0); Kab = (2);
   K0 = K1 = K2 = K3 = (0);
*/
IFS surf_qq1x1(IFS ifsa, IFS ifsb, VECH q0)
 {
  IFS ifsr;
  L_INT ja,jb,jci,jdi,kab,kbc,kcd,kda,k0,k1,k2,k3;
  MATH lq;
  int m;
  m = (dim_ifs(ifsa)+1)*(dim_ifs(ifsb)+1)-1;
  ecrit_int(stdout,"m",m);
  ja = li2(0,2);
  ecrit_li(stdout,"ja",ja);
  jb = li2(2,3);
  jci = li2(1,3);
  jdi = li2(0,1);
  kbc = li1(3);
  kcd = li1(1);
  kda = li1(0);
  kab = li1(2);
  k0 = li1(0);
  k2 = li1(0);
  k3 = li1(0);
  k1 = li1(0);
  lq = math_pt1(q0); 
  ecrit_math(stdout,"lq",lq);
  ifsr = ifs_crea(4);
  T0 = T0_abcd;
  ecrit_math(stdout,"T0",T0);
  T2 = T2_bcda;
  T3 = T3_cdab;
  T1 = T1_dabc;
  return ifsr;
 }

/* Grille 2x2
      c
   2--5--8
   |  |  |
 d 1--4--7 b
   |  |  |
   0--3--6
      a
   Ja = (0,3,6); Jb = (6,7,8); Jc = (8,5,2); Jd = (2,1,0);
   Kbc = (4,5,7,8); Kcd = (1,2,4,5); Kda = (0,1,3,4); Kab = (3,4,6,7);
   K0 = (0,1,3,4); K1 = (1,2,4,5); K2 = (3,4,6,7); K3 = (4,5,7,8); 
*/

#undef ke
#undef fin_grille 
