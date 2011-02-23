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
#include "util.h"
#include "point.h"
#include "matrice.h"
#include "controle.h"
#include "ifs.h"
#include "regle.h"

/********************************/
/* REGLES ET POINTS DE CONTROLE */ 
/********************************/

/*--------------------------*/
/* Operations informatiques */
/*--------------------------*/

/* Creation de regle */
REGLE reg_crea(int nb, int nba)
 {
  REGLE regr;
  regr.nb = nb;
  regr.nba = nba;
  regr.ipd = (L_INT *) malloc (nb*sizeof(L_INT));
  regr.iarc = (L_INT *) malloc (nba*sizeof(L_INT));
  return regr;
 }
/* Destruction de regle */
void destru_reg(REGLE reg)
 {
  free(reg.ipd);
  if(reg.nba > 0) free(reg.iarc);
 }
/* Duplication de regle */
REGLE reg_dupli(REGLE reg)
 {
  REGLE regr;
  int nb,nba,i;
  nb = reg.nb;
  nba = reg.nba;
  regr = reg_crea(nb,nba);
  regr.ipg = li_dupli(reg.ipg);
  for(i=0;i<nb;i++)
    regr.ipd[i] = li_dupli(reg.ipd[i]);
  for(i=0;i<nba;i++)
    regr.iarc[i] = li_dupli(reg.iarc[i]);
  return regr;
 }
/* Impression de np-uplet d'objets indices */
void ecrit_ind(FILE *fich, char *nom, L_INT ind)
 {
  int npm,j;
  npm = ind.nb-1;
  fprintf(fich,"(");
  for(j=0;j<npm;j++)
    fprintf(fich,"%s%d,",nom,ind.c[j]);
  fprintf(fich,"%s%d)",nom,ind.c[npm]);
 }
/* Impression de regle */
void ecrit_reg(FILE *fich, char *nomreg, REGLE reg)
 {
  int i;
  fprintf(fich,nomreg);
  fprintf(fich," = regle de reecriture\n  -sommets : ");
  ecrit_ind(fich,"q",reg.ipg);
  fprintf(fich," --> ");
  for(i=0;i<reg.nb;i++)
     ecrit_ind(fich,"q",reg.ipd[i]);
  fprintf(fich,"\n  -arcs : ");
  for(i=0;i<reg.nba;i++)
     ecrit_ind(fich,"T",reg.iarc[i]);
  fprintf(fich,"\n");
 }
/* Ecriture de regle en TeX */
void tex_reg(FILE *fich, char *nomreg, REGLE reg)
 {
  int np,i;
/*
  fprintf(fich,nomreg);
  fprintf(fich,"  -r\\'e\\'ecriture des sommets : $");
  fprintf(fich,"R\\`egle : $");
*/
  fprintf(fich,"Rules : $");
  np = (reg.ipg).nb;
  ecrit_ind(fich,"q_",reg.ipg);
  fprintf(fich,"\\;\\rightarrow\\;");
  for(i=0;i<reg.nb;i++)
     ecrit_ind(fich,"q_",reg.ipd[i]);
  fprintf(fich,"$");
  fprintf(fich,"; Minimal arcs : $");
  for(i=0;i<reg.nba;i++)
     ecrit_ind(fich,"T_",reg.iarc[i]);
  fprintf(fich,"$\n");
 }

/*--------------------*/
/* Operations unaires */
/*--------------------*/

/* Indice maximum */
int max_reg(REGLE reg)
 {
  int jr,nb,i;
  nb = reg.nb;
  jr = max_li(reg.ipg);
  for(i=0;i<nb;i++)
    jr = int_max(jr,max_li(reg.ipd[i]));
  return jr;
 }
/* Extraction de transfo a partir d'un semigroupe et d'un np-uplet */
MATH math_pc_ind(SEMIG sem, L_POINT pc, L_INT ind)
 {
  MATH r;
  L_POINT lp;
  lp = lpt_extr(pc,ind);
  r = math_sem_lpt(sem,lp);
  destru_lpt(lp);
  return r;
 }
/* Creation d'IFS */
/* Points de controle (dans espace iteration) : Qc = (q_0,..,q_m)
   Regle formelle : [reg.g0,reg.g1] -> [reg.d0_i,reg.d1_i] pour i = 0,..,nb-1 
   => Regle : Rg = T[q_reg.g0,q_reg.g1] -> R_i = T[q_reg.d0_i,q_reg.d1_i]) 
   => IFS : T_i = Ri*Rg^-1 */ 
IFS ifs_pc_regle(SEMIG sem, L_POINT pc, REGLE reg)
 {
  IFS ifs;
  MATH rg,rginv,rdi;
  int nb,m,i;
  nb = reg.nb; 
  ifs = ifs_crea(nb); 
  rg = math_pc_ind(sem,pc,reg.ipg);
  rginv = math_inverse(rg);
  m = dim_lpt(pc);
  rdi = math_crea(m,m); 
  for(i=0;i<ifs.nb;i++)
    {
     rdi = math_pc_ind(sem,pc,reg.ipd[i]);
     ifs.tf[i] = math_mul(rdi,rginv);
     if(sem == Cis)
        ifs.tf[i] = math_mul(ifs.tf[i],dy2(0.25));
     destru_math(rdi); 
    }
  destru_math(rginv);
  return ifs;
 }
/* Graphe gauche */
/*
FIGURE fig_graph_g(FIGURE gg, FIGURE gd, GIFS ifs, L_POINT pc, REGLE reg)
 {
  FIGURE gr;
  L_INT ai,si;
  na = reg.na;
  gr = fig_crea(2*na);
  proj = gifs.proj;
  for(i=0;i<na;i++)
    {
     ai = reg.iarc[i];
     gr.c[2*i] = prim_gifs(gifs_extr(gifs,ai));
     lpi = lpt_mul(proj,lpt_extr(pc,reg.ipg));
     gr.c[2*i+1] = prim_lpt(lpi);
     destru_lpt(lpi);
    }
  return gr;
 }
*/
/* Ajout d'indices de controle geometrique a une regle */
/* (g||m) -> (d0||m+1)...(dN-1||m+nb) */
REGLE reg_aj_geom(REGLE reg)
 {
  REGLE regc,regr;
  regc = reg_ponc(max_reg(reg)+1,reg.nb);
  regr = reg_conca(reg,regc);
  destru_reg(regc);
  return regr;
 }

/*---------------------------*/
/* Operations intermediaires */
/*---------------------------*/

/*
REGLE reg2(L_INT lig, L_INT lid0, L_INT lid1)
 {
  REGLE reg;
  reg = reg_crea(3);
  reg.ipg = lig;
  reg.ipd[0] = lid0;
  reg.ipd[1] = lid1;
  return reg;
 }
REGLE reg3(L_INT lig, L_INT lid0, L_INT lid1, L_INT lid2)
 {
  REGLE reg;
  reg = reg_crea(3);
  reg.ipg = lig;
  reg.ipd[0] = lid0;
  reg.ipd[1] = lid1;
  reg.ipd[2] = lid2;
  return reg;
 }
REGLE reg4(L_INT lig, L_INT lid0, L_INT lid1, L_INT lid2, L_INT lid3)
 {
  REGLE reg;
  reg = reg_crea(3);
  reg.ipg = lig;
  reg.ipd[0] = lid0;
  reg.ipd[1] = lid1;
  reg.ipd[2] = lid2;
  reg.ipd[3] = lid3;
  return reg;
 }
*/

/*---------------------*/
/* Operations binaires */
/*---------------------*/

/* concatenation */
/* g1||g2 -> d1_0||d2_0 ... d1_N||d2_N  et  ia1||ia2
   condition : reg1.nb = reg2.nb */ 
REGLE reg_conca(REGLE reg1, REGLE reg2)
 {
  REGLE regr;
  int nb,nba1,nba,i;
  nb = int_min(reg1.nb,reg2.nb);
  nba1 = reg1.nba;
  nba = nba1 + reg2.nba;
  regr = reg_crea(nb,nba);
  regr.ipg = li_conca(reg1.ipg,reg2.ipg);
  for(i=0;i<nb;i++)
    regr.ipd[i] = li_conca(reg1.ipd[i],reg2.ipd[i]);
  for(i=0;i<nba1;i++)
    regr.iarc[i] = li_dupli(reg1.iarc[i]);
  for(i=nba1;i<nba;i++)
    regr.iarc[i] = li_dupli(reg2.iarc[i-nba1]);
  return regr;
 }

/*------------------------*/
/* Bibliotheque de regles */
/*------------------------*/

/* Equivalences de description :
   a partie gauche fixee g = (i0,..,inp)
     T_di = T(g,di)   Sigma = Ud ensemble des np-uplets de partie droite
   Hypergraphe (geometrie) Hg -> Hd
   - sommets (geom) :
     j figurant dans g et di a la meme place, c.a.d. tel que :
     il existe k : j = g_k = di_k 
     => T_di(p_j) = p_j 
     => p_j = c_i centre de contraction de Ti = T_di 
   Graphe (topologie) Gg -> Gd
   - arcs de Ag :
     (g_k,g_l) = (di_k,di_l)
     a = (j,k) dans g 
     et (j0,...,jn-1) avec j0 = j et jn-1 = k chemin dans graphe Gd 
     => (j,k) dans Ag 
   - arcs de Ad : (i,a) dans Sigma x Ag  
   - sommets : bords des arcs 
     j tels que (j,k) ou (k,j) arc
*/ 

/*--------------------*/
/* Regles ponctuelles */
/*--------------------*/

/* Ajout de points de controle geometriques */
/* (m) -> (m+1)...(m+nb) */
REGLE reg_ponc(int m, int nb)
 {
  REGLE regr;
  int m1,j;
  regr = reg_crea(nb,0);
  regr.ipg = li1(m);
  m1 = m+1;
  for(j=0;j<nb-1;j++)
    {
    regr.ipd[j] = li1(m1+j);
    }
  return regr;
 }

/*-----------------*/
/* Regles filaires */
/*-----------------*/

/* Couples de points */
/* -Geometrie : Semig = Sim(2) ou Hyp(2) */

/* Arcs de courbe subdivises en nb : 
	      0-------nb -> 0-1-...-nb 
*/
REGLE reg_arc(int nb)
 {
  REGLE reg;
  L_INT iarc;
  int j;
  reg = reg_crea(nb,1);
  reg.ipg = li2(0,nb);
  iarc = li_crea(nb-1);
  for(j=0;j<nb-1;j++)
    {
    reg.ipd[j] = li2(j,j+1);
    iarc.c[j] = j;
    }
  reg.iarc[0] = iarc; 
  return reg;
 }
/* Variante permutee des arcs binaires */
/*          + - 
   0--2 -> 0-1-2 */ 
REGLE reg_arc2_p()
 {
  REGLE reg;
  L_INT *it,*ia;
  reg = reg_crea(2,1);
  reg.ipg = li2(0,2);
  it = reg.ipd;
  it[0] = li2(0,1);
  it[1] = li2(2,1);
  ia = reg.iarc;
  ia[0] = li2(0,1);
  return reg;
 }

/* Arbres avec un arc et 3 branches */ 

/* Arbre IL : 2    2
              |    | 
	      | -> 1 
	      |    |
	      0    0-->3 
*/
REGLE reg_IL()
 {
  REGLE reg;
  L_INT *it,*ia;
  reg = reg_crea(3,1);
  reg.ipg = li2(0,2);
  it = reg.ipd;
  it[0] = li2(0,1);
  it[1] = li2(1,2);
  it[2] = li2(0,3);
  ia = reg.iarc;
  ia[0] = li2(0,1);
  return reg;
 }
/* Variantes permutees */
REGLE reg_IL_p(L_INT eta0, L_INT eta1, L_INT eta2)
 {
  REGLE reg;
  L_INT *it,*ia;
  reg = reg_crea(3,1);
  reg.ipg = li2(0,2);
  it = reg.ipd;
  it[0] = li_extr(li2(0,1),eta0);
  it[1] = li_extr(li2(1,2),eta1);
  it[2] = li_extr(li2(0,3),eta2);
  ia = reg.iarc;
  ia[0] = li2(0,1);
  return reg;
 }

/* Arbre IY : 2    2
              |    | 
	      | -> 1-->3 
	      |    |
	      0    0    
*/
REGLE reg_IY()
 {
  REGLE reg;
  reg = reg_crea(3,1);
  reg.ipg = li2(0,2);
  reg.ipd[0] = li2(0,1);
  reg.ipd[1] = li2(1,2);
  reg.ipd[2] = li2(1,3);
  reg.iarc[0] = li2(0,1);
  return reg;
 }

/* Arbre IGamma : 
	      2    2-->3
              |    | 
	      | -> 1 
	      |    |
	      0    0    
*/
REGLE reg_IGamma()
 {
  REGLE reg;
  reg = reg_crea(3,1);
  reg.ipg = li2(0,2);
  reg.ipd[0] = li2(0,1);
  reg.ipd[1] = li2(1,2);
  reg.ipd[2] = li2(2,3);
  reg.iarc[0] = li2(0,1);
  return reg;
 }

/* Arbre IGamma- : 
	      2    2<--3
              |    | 
	      | -> 1 
	      |    |
	      0    0   
   Igamma- = IL o sigma  
   avec sigma permutation : 0 <-> 2, 1 <-> 1, 3 <-> 3
   dans ce cas il y a inversion des couples :
   (sig(j),sig(k)) = (k,j)
   et invariance des contractions :
   T((sig(i),sig(j)),(sig(k),sig(l))) = T((j,i),(l,k)) = T((i,j),(k,l))
   Formule generale  : sig(g) = g o tau et sig(dj) = dj o tau 
    - permutation globale des indices :
      sig(j_0,...,j_n) = (sig(j_0),...,sig(j_n))
      sig : {0,..,m} -> {0,..,m}
      => isomorphie des IFS
    - permutation locale simultanee des n+1-uplets :
      u o tau = (j_tau(0),...,j_tau(n))
      tau : {0,..,n} -> {0,...,n} 
      => invariance des contractions et des IFS :
      T(g o u,di o tau) = T(g,di)
    T'i = T(sig(g),di)
*/
REGLE reg_IGamma_moins()
 {
  REGLE reg;
  reg = reg_crea(3,1);
  reg.ipg = li2(0,2);
  reg.ipd[0] = li2(0,1);
  reg.ipd[1] = li2(1,2);
  reg.ipd[2] = li2(3,2);
  reg.iarc[0] = li2(0,1);
  return reg;
 }

/* Graphe serie-parallele :
              2    2
              |    |\ 
	      | -> 1 3
	      |    |/
	      0    0 
    ID = IL + Igamma-
*/
REGLE reg_ID()
 {
  REGLE reg;
  reg = reg_crea(4,2);
  reg.ipg = li2(0,2);
  reg.ipd[0] = li2(0,1);
  reg.ipd[1] = li2(1,2);
  reg.ipd[2] = li2(0,3);
  reg.ipd[3] = li2(3,2);
  reg.iarc[0] = li2(0,1);
  reg.iarc[1] = li2(2,3);
  return reg;
 }

/* Variante asymetrique : IL + Igamma = IL + (2,3) */
REGLE reg_IDa()
 {
  REGLE reg;
  reg = reg_crea(4,1);
  reg.ipg = li2(0,2);
  reg.ipd[0] = li2(0,1);
  reg.ipd[1] = li2(1,2);
  reg.ipd[2] = li2(0,3);
  reg.ipd[3] = li2(2,3);
  reg.iarc[0] = li2(0,1);
  reg.iarc[1] = li2(2,3);
  return reg;
 }

/* Graphe avec pont :
              2    2
              |    |\ 
	      | -> 1-3
	      |    |/
	      0    0 
    IB = ID + IY = ID + (1,3)
*/
REGLE reg_IB()
 {
  REGLE reg;
  reg = reg_crea(5,1);
  reg.ipg = li2(0,2);
  reg.ipd[0] = li2(0,1);
  reg.ipd[1] = li2(1,2);
  reg.ipd[2] = li2(0,3);
  reg.ipd[3] = li2(3,2);
  reg.ipd[4] = li2(1,3);
  reg.iarc[0] = li2(0,1);
  return reg;
 }

/* Triplets de points */
/* - Geometrie : Semig = Aff(2) 
   - Topologie : couples extraits */

/* Arbre de Sierpinski : 
	   0----2      0---4-2
	   |           |   |
	   |      ->   |   6 
           |           3-5 
	   |           |
	   1           1  
*/
REGLE reg_arbre_sierpinski()
 {
  REGLE reg;
  reg = reg_crea(3,2);
  reg.ipg = li3(0,1,2);
  reg.ipd[0] = li3(0,3,4);
  reg.ipd[1] = li3(3,1,5);
  reg.ipd[2] = li3(4,6,2);
  reg.iarc[0] = li2(0,1);
  reg.iarc[1] = li2(0,2);
  return reg;
 }

/* Variante de l'arbre avec composante supplementaire */
REGLE reg_arbre_sierpinski4()
 {
  REGLE reg;
  reg = reg_crea(4,2);
  reg.ipg = li3(0,1,2);
  reg.ipd[0] = li3(0,3,4);
  reg.ipd[1] = li3(3,1,5);
  reg.ipd[2] = li3(4,6,2);
  reg.ipd[3] = li3(3,4,5);
  reg.iarc[0] = li2(0,1);
  reg.iarc[1] = li2(0,2);
  return reg;
 }

/* Triangle de Sierpinski: 
	   0-----2    0--4--2
	   |    /     | /| /
	   |   /   -> |/ |/ 
           |  /       3--5 
	   | /        | /
	   |/         |/
	   1          1   
*/
REGLE reg_sierpinski()
 {
  REGLE reg;
  reg = reg_crea(3,3);
  reg.ipg = li3(0,1,2);
  reg.ipd[0] = li3(0,3,4);
  reg.ipd[1] = li3(3,1,5);
  reg.ipd[2] = li3(4,5,2);
  reg.iarc[0] = li2(0,1);
  reg.iarc[1] = li2(0,2);
  reg.iarc[2] = li2(1,2);
  return reg;
 }

/* Variante permutee du Triangle de Sierpinski: 
	   0-----2    0--4--2
	   |    /     |0/|-/
	   |   /   -> |/ |/ 
           |  /       3--5 
	   | /        |+/
	   |/         |/
	   1          1   
*/
REGLE reg_sierpinski_pr012()
 {
  REGLE reg;
  reg = reg_crea(3,3);
  reg.ipg = li3(0,1,2);
  reg.ipd[0] = li3(0,3,4);
  reg.ipd[1] = li3(1,5,3);
  reg.ipd[2] = li3(2,4,5);
  /* arcs non correct */
  reg.iarc[0] = li2(0,1);
  reg.iarc[1] = li2(0,2);
  reg.iarc[2] = li2(1,2);
  return reg;
 }

REGLE reg_sierpinski_p(L_INT eta0, L_INT eta1, L_INT eta2)
 {
  REGLE reg;
  reg = reg_crea(3,3);
  reg.ipg = li3(0,1,2);
  reg.ipd[0] = li_extr(li3(0,3,4),eta0);
  reg.ipd[1] = li_extr(li3(3,1,5),eta1);
  reg.ipd[2] = li_extr(li3(4,5,2),eta2);
  /* arcs non correct */
  reg.iarc[0] = li2(0,1);
  reg.iarc[1] = li2(0,2);
  reg.iarc[2] = li2(1,2);
  return reg;
 }

/* Triangle de Sierpinski avec composante supplementaire 
	   0-----2    0--4--2
	   |    /     |2/|1/
	   |   /   -> |/3|/ 
           |  /       3--5 
	   | /        |0/
	   |/         |/
	   1          1   
*/
REGLE reg_sierpinski4()
 {
  REGLE reg;
  reg = reg_crea(4,3);
  reg.ipg = li3(0,1,2);
  reg.ipd[0] = li3(0,3,4);
  reg.ipd[1] = li3(3,1,5);
  reg.ipd[2] = li3(4,5,2);
  reg.ipd[3] = li3(3,4,5);
  reg.iarc[0] = li2(0,1);
  reg.iarc[1] = li2(0,2);
  reg.iarc[2] = li2(1,2);
  return reg;
 }

/* Arc dedouble : 
	   0-----2    4--0--5
	   |    /      \ | /
	   |   /   ->   \|/  
           |  /          3--6 
	   | /           | /
	   |/            |/
	   1             1
   cas degenere de IL (3=2)
   0,1,3 points d'interpolation 
	 sommets topologiques et points de controle geometriques   
   2,4,5,6 uniquement points de controle geometriques  
   arcs : {T(034),T(316)} et {T(035).T(316)}
   raccord commun : (0,1) -> (0,3)(3,1)
*/
REGLE reg_DdB()
 {
  REGLE reg;
  reg = reg_crea(3,2);
  reg.ipg = li3(0,1,2);
  reg.ipd[0] = li3(0,3,4);
  reg.ipd[1] = li3(0,3,5);
  reg.ipd[2] = li3(3,1,6);
  reg.iarc[0] = li2(0,2);
  reg.iarc[1] = li2(1,2);
  return reg;
 }

/* Arc avec boucle 
      0
      |
      1==2
      |
      3 
*/
REGLE reg_Tboucle()
 {
  REGLE reg;
  reg = reg_crea(4,1);
  reg.ipg = li2(0,3);
  reg.ipd[0] = li2(0,1);
  reg.ipd[1] = li2(1,2);
  reg.ipd[2] = li2(2,1);
  reg.ipd[3] = li2(1,3);
  reg.iarc[0] = li2(0,3);
  return reg;
 }
