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
#include <stdlib.h>
#include "matrice.h"

static int nbt = 0 ;
#define DEBUG 0


/* initialise le tableau d'entier de la matrice a 0 */
void init_matrice(matrice *mat)
{
  int i,j;
	
  for(i=0;i<mat->dim;i++)
    {
      for(j=0;j<mat->dim;j++)
	{
	  mat->m[i][j]=NULL;
	}
    }
}

/* alloue la memoire pour une nouvelle matrice et l'initialise */
matrice *matrice_nouvelle(int dim)
{
  matrice *mat;
  int i;

  mat=(matrice *)malloc(sizeof(matrice));
  mat->dim=dim;
	
  mat->m=(liste_transfo ***)malloc(dim*sizeof(liste_transfo **));
  for(i=0;i<dim;i++)
    {
      mat->m[i]=(liste_transfo **)malloc(dim*sizeof(liste_transfo *));
    }
	
  init_matrice(mat);

  if ( DEBUG ) fprintf(stderr,"Apres alloue matrice : %d\n", ++nbt) ;
	
  return mat;
}

/* desalloue la memoire de la matrice*/
void matrice_libere(matrice *m)
{
  int i,j;

  if ( m == NULL )
    return ;
	
  for (i=0;i<m->dim;i++)
    {
      for (j=0;j<m->dim;j++)
	liste_libere(m->m[i][j]);
      free(m->m[i]);
		
    }
  free(m->m);
  free(m);

  if ( DEBUG ) fprintf(stderr,"Apres libere matrice : %d\n", --nbt) ;
}


/* duplique une matrice : a verifier */

matrice *matrice_duplique(matrice *mat)
{
  matrice *m;
  int i,j;
	
  m=matrice_nouvelle(mat->dim);
  for(i=0;i<mat->dim;i++)
    {
      for(j=0;j<mat->dim;j++)
	{
	  m->m[i][j] = liste_duplique(mat->m[i][j]);
	}
    }
	
  return m;
}


/* addition de 2 matrice */
matrice *matrice_additionne(matrice *mat1,matrice *mat2)
{
  matrice *mat;
  int i,j;
	
  mat=matrice_nouvelle(mat1->dim);
  for(i=0;i<mat->dim;i++)
    {
      for(j=0;j<mat->dim;j++)
	{
	  mat->m[i][j]=liste_additionne(mat1->m[i][j], mat2->m[i][j]);
	}
    }

  return mat;
}

/* multiplication de 2 matrices (mat1*mat2)*/
matrice *matrice_multiplie(matrice *mat1,matrice *mat2)
{
  matrice *mat;
  int i,j,k;
  mat=matrice_nouvelle(mat1->dim);
  for(i=0;i<mat1->dim;i++)
    {
      for(j=0;j<mat1->dim;j++)
	{/* on parcours les elements de la matrice */
	  mat->m[i][j]=NULL;
	  /* et maintenant on fait la multiplication */
	  for(k=0;k<mat1->dim;k++)
	    {
	      liste_concatene(&mat->m[i][j],
			      liste_multiplie(mat1->m[i][k] ,mat2->m[k][j])
			      ) ;
	    }
	}
    }
  return mat;	
}


/* puissance n d'une matrice */
matrice *matrice_puissance(matrice *mat, int n)
{
  matrice *m, *tmp ;
	
  if (n<=1)
    return matrice_duplique(mat) ;
  if (n==2)
    return matrice_multiplie(mat, mat) ;
  if (n==3)
    {
      tmp = matrice_multiplie(mat, mat) ;
      m = matrice_multiplie(tmp, mat) ;
      matrice_libere(tmp) ;
      return m ;
    }
  if (n==4)
    {
      tmp = matrice_multiplie(mat, mat) ;
      m = matrice_multiplie(tmp, tmp) ;
      matrice_libere(tmp) ;
      return m ;
    }
  m=matrice_puissance(mat, n/2);
  tmp = m ;
  m=matrice_multiplie(m,m);
  matrice_libere(tmp) ;
  if (n%2)
    {
      tmp = m ;
      m=matrice_multiplie(m,mat);
      matrice_libere(tmp) ;
    }
  return m;
}

/* union de 2 matrices */
matrice *matrice_union(matrice *mat1,matrice *mat2)
{
  int i,j;
  matrice *m;

  m=matrice_nouvelle(mat1->dim+mat2->dim);
  /* on commence par parcourir la premiere matrice */
  for(i=0;i<mat1->dim;i++)
    for(j=0;j<mat1->dim;j++)
      m->m[i][j]= liste_duplique(mat1->m[i][j]) ;
	
  /* maintenant on parcour la deuxieme matrice */
  for(i=0;i<mat2->dim;i++)
    for(j=0;j<mat2->dim;j++)
      m->m[i+mat1->dim][j+mat1->dim]= liste_duplique(mat2->m[i][j]);

  return m;

}

/* concatenation de 2 matrices */
matrice *matrice_concatenation(matrice *mat1,matrice *mat2)
{
  int i,j;
  matrice *m;
  Transformation *t;
	
  m=matrice_union(mat1,mat2);
  for(i=0;i<mat2->dim;i++)
    {
      for(j=0;j<mat2->dim;j++)
	{
	  t=(*transfo_identite)();
	  matrice_affecte(m, i, j+mat1->dim, t);
	}
    }
  return m;
}


/* melange de 2 matrices */
matrice *matrice_melange(matrice *mat1,matrice *mat2)
{
  int i1,i2,j1,j2;
  matrice *mat;
	
  mat=matrice_nouvelle(mat1->dim*mat2->dim);
  /* on commence par parcourir la premiere matrice */
  for(i1=0;i1<mat1->dim;i1++)
    {
      for(j1=0;j1<mat1->dim;j1++)
	{
	  /* maintenant on parcour la deuxieme matrice */
	  for(i2=0;i2<mat2->dim;i2++)
	    {
	      for(j2=0;j2<mat2->dim;j2++)
		{
		  /* et on rempli */
		  /* je decompose en 4 cas */
		  if (i2==j2 && i1!=j1)
		    {	/* on est sur une petite diagonale mais pas sur la grande */
		      mat->m[i1*mat2->dim+i2][j1*mat2->dim+j2] =
			liste_duplique(mat1->m[i1][j1]) ;
		    }
		  else if (i2==j2 && i1==j1)
		    {	/* on est sur une petite diagonale et aussi sur la grande */
		      mat->m[i1*mat2->dim+i2][j1*mat2->dim+j2] =
			liste_additionne(mat1->m[i1][j1],mat2->m[i2][j2]);
		    }
		  else if (i2!=j2 && i1==j1)
		    {	/* on n'est pas sur une petite diagonale mais sur la grande */
		      mat->m[i1*mat2->dim+i2][j1*mat2->dim+j2] = 
			liste_duplique(mat2->m[i2][j2]) ;
		    }
		  /* ailleur on ne fait rien */

		}
	    }

	}
    }
	

  return mat;
}


/* affiche une matrice */
void matrice_affiche(FILE *f, matrice *mat)
{
  int i,j;
	
  for(i=0;i<mat->dim;i++)
    {
      fprintf(f,"(");
      for(j=0;j<mat->dim;j++)
	{
	  liste_affiche(f, mat->m[i][j]);
	}
      fprintf(f,")\n");
    }
}

/* affecte une case d'une matrice */
void matrice_affecte(matrice *mat, int ligne, int col, Transformation *x)
{
  mat->m[ligne][col]=liste_ajoute(mat->m[ligne][col], x);
}




