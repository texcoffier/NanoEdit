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
#include "io.h"


/* cree un fichier en ecriture a partir d'une chaine de caracteres */
FILE *fichier_dest(char *name)
{
        FILE *res;
        
        res=fopen(name, "w");
        if (res==NULL)
	{
                printf("Impossible de creer le fichier destination.\n");
		exit(1);
	}
	return res;
}           


void inc_transfo(FILE *sortie, Transformation *t)
{
	fprintf(sortie, "Separator{\n");
	fprintf(sortie, "MatrixTransformationrm { matrix\n");
	transfo_affiche_pov(t, sortie) ;
	fprintf(sortie, "}\nCube{\nwidth 1\nheight 1\ndepth 1\n}}\n");	
}

void inc_liste_transfo(FILE *sortie, liste_transfo *l)
{
	if(l!=NULL)
	{
		inc_liste_transfo(sortie, l->suc);
		inc_transfo(sortie, l->mat);
	}
}

/* sortie d'une matrice dans un fichier inventor inc */
void io_inc(char *fichier, matrice *mat)
{
	FILE *sortie;
	int i,j;
	
	sortie=fichier_dest(fichier);
	fprintf(sortie,"#Inventor V2.0 ascii\n");
	for(i=0;i<mat->dim;i++)
		for(j=0;j<mat->dim;j++)
			inc_liste_transfo(sortie, mat->m[i][j]);
	fclose(sortie);
}

void mat_liste_transfo(FILE *sortie, liste_transfo *l)
{
	if(l!=NULL)
	{
		mat_liste_transfo(sortie, l->suc);
		transfo_affiche_pov(l->mat, sortie);
	}
}

/* sortie de la matrice dans un fichier */
void io_mat(char *fichier, matrice *mat)
{
	FILE *sortie;
	int i,j;
	
	sortie=fichier_dest(fichier);
	printf("sauve matrice\n");
	fprintf(sortie,"%i\n",mat->dim);
	for(i=0;i<mat->dim;i++)
		for(j=0;j<mat->dim;j++)
		{
			mat_liste_transfo(sortie, mat->m[i][j]);
			fprintf(sortie,"\n");
		}
	fclose(sortie);
}

