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
/*------------------*/
/* REGLES FORMELLES */ 
/*------------------*/

/* Regles de reecriture formelles */
/* (jg_0,..,jg_np-1) -> (jdi_0,..,jdi_np-1) pour i = 0,..,nb-1
   suites d'indice de points :
   sommets a gauche et a droite
   */
/* inda[k] arc formel = suite d'indices d'operateurs (inda(k)0,...,inda(k)n) 
   ==> IFS extrait \Ti_arc(k) = \Ti o inda(k) ;
   squelette = union des arcs(k) */ 
/*
typedef struct {int np; int *ipg; int nb; int **ipd;} REGLE;
*/
typedef struct {L_INT ipg; int nb; L_INT *ipd; int nba; L_INT *iarc;} REGLE;

/* Points de controle associes */

#define PC L_POINT 

/*------------*/
/* Operations */
/*------------*/

/* Operations informatiques */
REGLE reg_crea(int nb, int nba);
void destru_reg(REGLE reg);
REGLE reg_dupli(REGLE reg);
void ecrit_ind(FILE *fich, char *nom, L_INT ind);
void ecrit_reg(FILE *fich, char *nomreg, REGLE reg);
void tex_reg(FILE *fich, char *nomreg, REGLE reg);

/* Operations unaires */
int max_reg(REGLE reg);
MATH math_pc_ind(SEMIG sem, PC pc, L_INT ind);
IFS ifs_pc_regle(SEMIG sem, PC pc, REGLE reg);
/*
FIGURE fig_ifs_regle(IFS ifs, REGLE reg);
*/
REGLE reg_aj_geom(REGLE reg);

/* Operations intermediaires */
/*
int *ind2(int j0, int j1);
int *ind3(int j0, int j1, int j2);
int *ind4(int j0, int j1, int j2, int j3);
*/
/* Operations binaires */
REGLE reg_conca(REGLE reg1, REGLE reg2);

/*--------------*/
/* Bibliotheque */
/*--------------*/

/* Regles ponctuelles */
REGLE reg_ponc(int m, int nb);

/* Regles filaires */
REGLE reg_arc(int nb);
REGLE reg_arc2_p();
REGLE reg_IL();
REGLE reg_IL_p(L_INT eta0, L_INT eta1, L_INT eta2);
REGLE reg_IY();
REGLE reg_IGamma();
REGLE reg_IGamma_moins();
REGLE reg_ID();
REGLE reg_IDa();
REGLE reg_IB();
REGLE reg_arbre_sierpinski();
REGLE reg_arbre_sierpinski4();
REGLE reg_sierpinski();
REGLE reg_sierpinski_pr012();
REGLE reg_sierpinski_p(L_INT eta0, L_INT eta1, L_INT eta2);
REGLE reg_sierpinski4();
REGLE reg_DdB();
REGLE reg_Tboucle();
