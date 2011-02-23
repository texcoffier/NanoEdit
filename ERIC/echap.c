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
#include <math.h> 
#include "util.h"
#include "point.h"
#include "matrice.h"
#include "controle.h"
#include "ifs.h"
#include "gifs.h"
#include "graphisme.h"
#include "image.h"
#include "echap.h"

/********************************************/
/* FONCTIONS POTENTIELLES ASSOCIEES AUX IFS */
/********************************************/

/* Fonction potentielle dependant de :
     -IFS indice
     -boule englobant l'attracteur
   Valeur de potentiel :   
     q dans Em -> echap(m,q) fonction recursive sur m, 
          m = longueur d'echappement maximum,
     (x,y) -> potentiel(m,x,y) champs de valeurs 
   */
/* Interpretation ensembliste
   Deterministe :
    - P projection :    E_I -> E_M
    - T contraction :   E_I -> E_I 
    - B englobant (ou approximant initial) de l'attracteur A_I
    - englobant transforme : T B
    - approximant de niveau n de PA_I : K_n = U |u|=n PT_u B
      ou encore : K_n = U |u|=n Fig(PT_u)
    - condensation : U j=<n K_j 
   Echappement : 
    - P^-1 plongement : E_M -> E_I
    - coupe dans E_I : P^-1 E_M ^ T B 
    - projection dans E_M de la coupe : P(P^-1 E_M ^ T B)
    - PT projection contractee : E_I -> E_M
    - restriction de projection 
      P(P^-1 E_M ^ T B) = P T(T^-1 P^-1 E_M ^ B) 
                        = (PT)((PT)^-1 E_M ^ B)
	                <_ (PT) B
    en posant : Res(Q) = Q(Q^-1 E_M ^ B)
    - visualisation : p dans Res(Q) <==> Q^-1p dans B
      B domaine defini par des inegalites dans E_I
    - approximant de niveau n : 
      K'_n = U |u|=n Res(PT_u)
      => nouvelle figure : projection de coupe d'attracteur ?  
         lim K'_n = P(P^-1 E_M ^ A_I) ? 
      U |u|=n Res(PT_u) = U P(P^-1 E_M ^ T_u B)
                        = P(U (P^-1 E_M ^ T_u B))
                        = P(P^-1 E_M ^ (U T_u B))
      donc :
      K'_n = P(P^-1 E_M ^ A_n)
      et :
      lim K'_n = P lim (P^-1 E_M ^ A_n)
	       = P lim (V ^ A_n)
      Soit : A = lim A_n = lim A'_n avec A_n <_ A <_ A'_n 
       on suppose que V^A_n =/= 0 pour tout n 
       on a :       V^A_n <_ V^A <_ V^A'_n
       et :   lim (V^A_n) <_ V^A <_ lim(V^A'_n)
       est-ce que lim (V^A_n) = lim(V^A'_n) ?   
    - condensation : U j=<n K'_j 
   */

/* Constantes globales */
/* IFS :
     - ifsinv IFS inverse,
     - dim dimension de l'espace d'iteration,
     - ntf nombre de transformations,
     - tfinv[j] avec j=0,..,ntf-1 tableau des transformations inverses,
   Ellipsoide englobant :
     - teinv transfo inverse de te,
     - plong = Te^-1*P^-1*E^-1 
       plongement du pixel (i,j) dans l'espace d'englobement,
     - Boule centree unitaire : r^2 = r = 1, log(r) = log(1),
   me : longeur maximum de trajectoire (= nombre max d'iterations),
   */
/* Variables globales */
/* p, pu points courants de l'espace d'iteration */

/* variables d'echappement mD */
   int dim;
   int ntf;
   IFS ifsinv;
   MATH *tfinv;
   MATH plong;
   Flottant log1;
   POINT p,pu;

/*----------------------------------*/
/* Fonction recursive d'echappement */
/*----------------------------------*/
/* arguments de la fonction :
   q point courant,
   m longueur maximum de trajectoire 
     fonction appelee avec m > 0 */
/* variables liees au point courant 
   dq distance carree de q a l'origine,
   logdq, num */
/* variables liees au point suivant :
   i indice de transfo, 
   qi=tfinv[i]*q point suivant selon i,
   dqi distance carree de qi au centre,
   ech valeur d'echappement */ 
  
Flottant echap(int m, POINT q)   
 {
  Flottant dq,logdq,num;
  int i;
  POINT qi; 
  Flottant ech,dqi;

  dq = norme2_pt(q);
  if (dq > 1) return 0;  /* q exterieur */
  /* sinon q interieur */ 
  logdq = log(sqrt(dq));
  num = log1 - logdq;
  qi = pt_crea(dim); 
  i = 0;
  ech = 0.;
  while (i<ntf)
    {
     mul_math_pt(qi,tfinv[i],q);
     dqi = norme2_pt(qi);
     if (dqi <= 1) break;
     ech = max(ech, num/(log(sqrt(dqi)) - logdq));
     i = i+1;
    }
  if (i==ntf) /* tous les qi sont exterieurs => echap < 1 */
    {
     destru_pt(qi);
     return ech;
    }
  /* sinon on a le premier qi interieur : 0=< i =<ntf-1 => echap >= 1 */
  if (m==1)  /* = 1+echap(0,qi) = 1+0 = 1 */ 
    {
     destru_pt(qi); 
     return 1;
    }
  ech = echap(m-1,qi);  /* premier qi interieur */
  i = i+1;
  while(i < ntf)
    {
     mul_math_pt(qi,tfinv[i],q);
     ech = max(ech,echap(m-1,qi));
     i = i+1;
    }
  destru_pt(qi);
  return 1+ech;
 }

/*-----------------------*/
/* Calcul des potentiels */
/*-----------------------*/

/* arguments :
   (plong, ifs, te) plongement, IFS et ellipsoide englobant l'attracteur,
   m longueur maximum de trajectoire, 
   (x,y) ou (x,y,z) point ou le potentiel est calcule. 
   */

/* initialisation du calcul */
void deb_pot_gifs(TIMA tima, GIFS gifsv)
 {
  /*
  GIFS gifsv;
  MATH echel,proj;
  */
  log1 = log(1);                       /* boule rayon 1 */
  /*
  gifsv = gifs_normal(gifs);          
  echel = echel_larg(tima,lfen);
  proj = math_mul(echel,gifsv.proj);
  */
  plong = math_inverse_d(gifsv.proj);         /* inversion */ 
  ifsinv = ifs_inverse(gifsv.ifs);      
  ntf = ifsinv.nb;               
  tfinv = ifsinv.tf;
  p = pt_crea(plong.n.dim);           /* creation point courant image */
  dim = plong.m.dim ;                  /* dimension espace d'iteration */
  pu = pt_crea(dim);              /* creation point courant espace englobe */ 
  return;
 }

/* fin du calcul */
void fin_pot_gifs()
  {
   destru_pt(p); 
   destru_pt(pu);
   destru_ifs(ifsinv); 
   destru_math(plong); 
  }

/* calcul de potentiel 2D */
Flottant val_pot2(int m, Flottant x, Flottant y)
  {
   if(m == 0) return 0;
   /* sinon me >= 1 */
   p.c[0] = 1;
   p.c[1] = x;
   p.c[2] = y;                
   mul_math_pt(pu,plong,p);  /* plongement dans espace englobe */
   return echap(m,pu);   
  }

/* calcul de potentiel 3D */
Flottant val_pot3(int m, Flottant x, Flottant y, Flottant z)
  {
   if(m == 0) return 0;
   /* sinon me >= 1 */
   p.c[0] = 1;
   p.c[1] = x;
   p.c[2] = y;           
   p.c[3] = z;               
   mul_math_pt(pu,plong,p);  /* plongement dans espace englobe */
   return echap(m,pu);   
  }

/*--------------------------------*/
/* Remplissage de tableau d'image */
/*--------------------------------*/

/* Coupe d'attracteur d'IFS dans espace de visualisation */
void tima_echap_v(TIMA tima, GIFS gifs, int m)
 {
  GIFS gifsv;
  int i,j;
  gifsv = gifs_normal(gifs);           /* normalisation */
  deb_pot_gifs(tima, gifsv);
  for(j=tima.n.dim;j>=0;j--)
    {
     for(i=0;i<=tima.m.dim;i++)
        tima.c[i][j] = val_pot2(m,i,j);
    }
  fin_pot_gifs();
 }

/* Coupe quelconque d'attracteur d'IFS */
void tima_echap(TIMA tima, Flottant lfen, GIFS gifs, int m)
 {
  MATH echel; 
  GIFS gifsv;
  echel = echel_larg(tima,lfen);
  gifsv = gifs_mul(echel,gifs);
  tima_echap_v(tima, gifsv, m);
  destru_gifs(gifsv);
  destru_math(echel);
 }
/*
void tima_echap(TIMA tima, Flottant lfen, GIFS gifs, int m)
 {
  int i,j;
  deb_pot_gifs(tima, lfen, gifs);
  for(j=tima.n;j>=0;j--)
    {
     for(i=0;i<=tima.m;i++)
        tima.c[i][j] = val_pot2(m,i,j);
    }
  fin_pot_gifs();
 }
*/
