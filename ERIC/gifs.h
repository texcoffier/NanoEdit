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
/*******************/
/* IFS GENERALISES */ 
/*******************/

/* Modele : P,(T_i),Te ;
   - projection P : E_I -> E_M  
   - plongement Q = P^-1 : E_M -> E_I 
   - contraction T_i : E_I -> E_I
   - englobant (ou motif) : image de figure standard Te*B0 ;
     K(P,(T_i),Te) = P*T_u*Te*B0
     Ech(Q,(T_i),Te)(p) = max{ |u| : T_u^-1*Q*p dans Te*B0 }
   Relation : 
     K_n = { p : Ech(p) >= n }
	 = P*(Q*E_M ^\T^n*Te*B_0) 
     K_inf = P(Q*E_M ^ A(T_i)) <_ P*A(T_i) 
   Visualisation : 
   - echelle E : E_M -> E_V ;
   - Motif itere : B(u1...un) = P*T_u*Te*B0 ;
     avec |u| = n et T_u = T_u1*...T_un ; 
   - (E*P)*T_u*Te : B0 -> E_V
   Equivalences :
     K(P*R^1,R*(T_i)*R^-1,R*Te) = K(P,(T_i),Te)
       P*R^1*R*T_u*R^-1*R*Te*B0 = P*T_u*Te*B0
     Ech(R*Q,R*(T_i)*R^-1,R*Te) = Ech(Q,(T_i),Te)
       Ech(Q,(T_i),Te)(p) = max{ |u| : T_u^-1*Q*p dans Te*B0 }
       = max{ |u| : R*T_u^-1*Q*p dans R*Te*B0 }
       = max{ |u| : R*T_u^-1*R^-1*R*Q*p dans R*Te*B0 }
       = Ech(R*Q,R*T_i*R^-1,R*Te)(p)
   Operations :
   - conjugaison : 
     K(P,R*(T_i)*R^-1,R*Te) = K(P*R,(T_i),Te)
      P*A(R*(T_i)*R^-1) = P*R*A(T_i) ;
     P(Q*E_M :^: A(R*(T_i)*R^-1)) 
       = P*(Q*E_M :^: R*A(T_i)) 
       = (P*R)*(R^-1*Q*E_M :^: A(T_i)) 
     Ech(Q,R*(T_i)*R^-1,R*Te) = Ech(R^-1*Q,(T_i),Te)
      Ech(Q,R*(T_i)*R^-1,R*Te)(p) 
      = max{ |u| : R*T_u^-1*R^-1*Q*p dans R*Te*B0 }
      = max{ |u| : T_u^-1*R^-1*Q*p dans Te*B0 }
      = Ech(R^-1*Q,T_i,Te)(p)
   - transformation : 
     K(R*P,(T_i),Te) 
      = R*P*T_u*Te*B0 = R*K(P,T_i,Te)
     Ech(Q*R^-1,(T_i),Te)(p) 
      = max{ |u| : T_u^-1*Q*R^-1*p dans Te*B0 }
      = Ech(Q,(T_i),Te)(R^-1*p) 
   - Normalisation :
     (P,T_i,Te) <=> (P*Te,Te^-1*T_i*Te,Id) 
*/ 
/* Algo Echappement :
   coupe d'attracteur : P(Q*E_M :^: A(T_i))
   projection de la coupe d'attracteur :
    P(Q*E_M :^: A(T_i))
   p dans E_V -> q = (P*T_u*Te)^-1*p -> q dans B0 ?  
    (P*T_u1*...*T_un*Te)^-1 = Te^-1*(T_un^-1*...*T_u1^-1*P^-1) 
    dans espace englobe :
    = (Te^-1*T_un^-1*...*T_u1^-1*Te)*(Te^-1*P^-1) 
    = (T'_un^-1*...*T'_u1^-1)*(Te^-1*P^-1)
    avec T'_i = Te^-1*T_i*Te et P'^-1 = Te^-1*P^-1
    Echap(Q,(T_i),Te) = Echap(Te^-1*Q,(Te^-1*T_i*Te),Id)
*/
/* Algo Deterministe : 
   projection d'attracteur : P*A(T_i) <_ P*Te*B0 
    afficher (P*T_u1*...*T_un)*(Te*B0)
    dans espace englobe :
    = (P*Te)*(Te^-1*T_u*Te)*B0
    (P,(T_i),Te) <==> (P*Te,(Te^-1*T_i*Te),Id)
*/ 
/* Chaos dynamique : P,(T_i) et Tc
    afficher P*T_un*...*T_u1*q0 avec q0 dans A
*/

/* Grilles */
/* produits cartesiens de simplexes : S_n1(m1,N1) x ... x S_nk(mk,Nk) avec :
   - ni dimension du simplexe
   - mi degre (nombre d'aretes du cote) sommets : indices espace d'iteration
   - Ni degre (nombre d'aretes du cote) hyperfaces : indices de l'IFS 
*/

typedef struct {int dim; int deg_esp; int deg_ifs;} SIMPLEXE;

typedef struct {int nb; SIMPLEXE *c;} GRILLE;

/* Generalised IFS */
/* (P,(T_i),T_englobant,J_extremites) */

typedef struct {MATH proj; IFS ifs; MATH te; L_INT extrem;} GIFS;

/*
typedef struct {MATH plong; MATH proj; IFS ifs; AUTOMATE autom; MATH te;} GIFS;
*/


/*---------*/
/* Grilles */
/*---------*/

GRILLE gril_crea();
void destru_gril(GRILLE g);
GRILLE gril_dupli(GRILLE g);
void ecrit_gril(FILE *f, char *nom, GRILLE g);

int dim_gril(GRILLE g);
/*
int nb_pc_simpl(SIMPLEXE s);
int nb_pc_gril(GRILLE g);
int nb_ifs_simpl(SIMPLEXE s);
int nb_ifs_gril(GRILLE g);
*/
GRILLE gril_simpl(int n, int m, int nb);
GRILLE gril_pcar(GRILLE g1, GRILLE g2);
GRILLE gril_S0();
GRILLE gril_S1(int m, int nb);
GRILLE gril_S1xS1(int m1, int nb1, int m2, int nb2);
GRILLE gril_S2(int m, int nb);
/*
GRILLE gril_S3(int m, int nb);
*/

/* Operations informatiques */
/*
GIFS gifs_crea();
*/
void destru_gifs(GIFS gifs);
GIFS gifs_dupli(GIFS gifs);
void ecrit_gifs(FILE *f, char *nomgifs, GIFS gifs);

/* Operations 0-aires */
GIFS gifs_proj_ifse(MATH proj, IFS ifs, MATH te);
GIFS gifs_plong_ifse(MATH plong, IFS ifs, MATH te);
GIFS gifs_ifse(int m, IFS ifs, MATH te);
GIFS gifs_proj_ifs(MATH proj, IFS ifs);
GIFS gifs_ifs(int m, IFS ifs);

/* Operations unaires */
int dim_gifs(GIFS gifs);
int nb_gifs(GIFS gifs);
L_INT lie_gifs(GIFS gifs);
L_POINT centre_gifs(GIFS gifs);
MATH extrem_gifs(GIFS gifs);
L_VECH lvh_extrem_gifs(GIFS gifs);

GIFS gifs_2(GIFS gifs1, int i, int j);
GIFS gifs_extr(GIFS gifs1, L_INT ind);
GIFS gifs_conj(MATH t,GIFS gifs1);
GIFS gifs_mul(MATH t,GIFS gifs1);
GIFS gifs_normal(GIFS gifs1);
GIFS gifs_it(int i, GIFS gifs1);

