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
#include "liste.h"
#include <stdlib.h>

/* libere la memoire d'une liste */
void liste_libere(liste_transfo *lt)
{
  liste_transfo *suc ;

  while (lt!=NULL)
    {
      suc = lt->suc ;
      (*transfo_libere)(lt->mat);
      free(lt);
      lt = suc ;
    }
}


/* ajoute la transformation t au debut de la liste liste */
liste_transfo *liste_ajoute(liste_transfo *liste, Transformation *t)
{
  liste_transfo *l;
	
  l=(liste_transfo *)malloc(sizeof(liste_transfo));
  l->mat=t;
  l->suc=liste;

  return l ;
}

/* duplique l'element */
liste_transfo *liste_duplique(liste_transfo *l)
{
  liste_transfo *lt;
	
  if(l!=NULL)
    {
      lt=liste_duplique(l->suc);
      lt=liste_ajoute(lt,(*transfo_duplique)(l->mat));
      return lt;
    }
  else
    return NULL;
}

/* Inverse ordre de la liste */
liste_transfo *liste_inverse(liste_transfo *l)
{
  liste_transfo *pred, *suc ;
 
  pred = NULL ;
  
  while( l )
    {
      suc = l->suc ;
      l->suc = pred ;
      pred = l ;
      l = suc ;
    }
  return( pred ) ;
}

/* ajoute 2 elements */
/* (a,b)+(c,d) = (a,b,c,d) */
liste_transfo *liste_additionne(liste_transfo *e1, liste_transfo *e2)
{
  e2 = liste_duplique(e2) ;
  for( ; e1; e1 = e1->suc)
    e2 = liste_ajoute(e2, (*transfo_duplique)(e1->mat)) ;
  return e2 ;
}

void liste_concatene(liste_transfo **e1, liste_transfo *e2)
{
  liste_transfo *e ;

  if ( *e1 == NULL )
    {
      *e1 = e2 ;
      return ;
    }
  e = *e1 ;
  while( e->suc )
    e = e->suc ;

  e->suc = e2 ;
}

/* multiplie 2 elements */
/* (a,b)*(c,d) = (ac,ad,bc,bd) */
liste_transfo *liste_multiplie(liste_transfo *e1, liste_transfo *e2)
{
  liste_transfo *ret,*tmp;
  ret=NULL;
  /* on parcours le 1er element */
  while(e1!=NULL)
    {
      /* on parcours le 2eme element */
      tmp=e2;
      while(tmp!=NULL)
	{
	  /* on compose les 2 transfo */
	  /* et on la met dans la liste */
	  ret=liste_ajoute(ret,(*transfo_multiplie)(e1->mat,tmp->mat));
	  tmp=tmp->suc;
	}
      e1=e1->suc;
    }
  return ret;
}


/* affiche la liste des transformation de l'element */
void liste_affiche(FILE *f, liste_transfo *e)
{
  fprintf(f,"{");
  while(e!=NULL)
    {
      if (e->mat!=NULL)
	transfo_affiche_pov(e->mat, f);
      else
	fprintf(f,"(null)");
      printf(" ");
		
      e=e->suc;
    }
	
  fprintf(f,"}");
}


