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
#define EPSILON   1.e-8
/*
#define DEBUG
*/
/*-----------------------------------------------------------*/
/*-------------------------------------------------------------------------*/
/*                                                                         */
/*  initialise                                                             */
/*                                                                         */
/*-------------------------------------------------------------------------*/
void initialise(a, b ,c, alp,base, h_base, m , nn)
int m,*nn;
double a[100][100], *b, *c;
int base[100][100], h_base[100][100];
double *alp;
{
int i, j,k,nh,n;
double alpha;

n=*nn;

#ifdef DEBUG
 printf("m=%d n=%d\n", m, n) ;
for(i=0;i<m;i++)
  {
    printf("%g -", b[i]) ;
    for(j=0;j<n;j++)
      printf(" %g", a[i][j]) ;
    printf("\n") ; 
  }
#endif

alpha=0;

for ( i=0; i<n; i++)
   {
    h_base[i][0]=i;
    h_base[i][1]=0;
   }
for (i=0; i<m;i++)
   {
    for(j=n; j<m+n; j++) a[i][j]=0.;
   }

for (i=0; i<m+n;i++)
    c[i]=0.;

nh=n;
k=0;
for ( i=0; i<m; i++)
   {
    if ( b[i] >= 0.)
       {
	for (j=0; j<n; j++)a[i][j]= -a[i][j];
	base[k][0]=i;
	base[k][1]=1;
       }
    else
       {
	h_base[nh][0]=i;
	h_base[nh][1]=1;
        base[k][0]=i;
	base[k][1]=2;
	b[i] = -b[i];
	alpha +=b[i];
	a[i][nh]=1.;
	nh++;
	for (j=0; j<nh; j++)c[j] += a[i][j];
	}
    k++;
   }

*nn=nh;
*alp=alpha;

#ifdef DEBUG
 printf("m=%d nh=%d alhpa=%g \n", m, nh, alpha) ;
for(i=0;i<m;i++)
  {
    printf("%g (%g) -", b[i], c[i]) ;
    for(j=0;j<=nh;j++)
      printf(" %g", a[i][j]) ;
    printf("\n") ; 
  }
#endif

return;
}


/*-------------------------------------------------------------------------*/
/*                                                                         */
/*  transform                                                              */
/*                                                                         */
/*-------------------------------------------------------------------------*/

void transform(m,n,i0,j0,a,b,alpha,c)
int m,n,i0,j0;
double a[100][100], *b, *c;
double *alpha;
{
int l , i ;
double alp;

alp=*alpha;

 a[i0][j0] = 1./a[i0][j0] ;
 b[i0] *= -a[i0][j0] ;
 for ( l=0; l<n ; l++)
   {
    if ( l != j0 ) a[i0][l] *= -a[i0][j0] ;
   }

 for ( i=0; i<m ; i++ )
   {
    if ( i != i0 )
      {
       if ( a[i][j0] != 0 )
         {
          b[i] +=a[i][j0]*b[i0] ;
	  for ( l=0; l<n ; l++)
	    {
             if ( l != j0 ) a[i][l] += a[i][j0]*a[i0][l] ;
            }
          a[i][j0] *= a[i0][j0];
	 }
      }
   }
 if ( c[j0] != 0 )
   {
    alp +=c[j0]* b[i0] ;
    for ( l=0; l<n ; l++)
      {
       if ( l != j0 )c[l] += c[j0]*a[i0][l] ;
      }
    c[j0] *= a[i0][j0] ;
   }

#ifdef DEBUG
 printf("\n DANS TRANSFORM: i0=%d j0=%d",i0,j0);
 printf("\n");
#endif

*alpha=alp;
return;
}



/*-------------------------------------------------------------------------*/
/*                                                                         */
/*  tetamax                                                                */
/*                                                                         */
/*-------------------------------------------------------------------------*/

int tetamax ( a, b, base, j0, i00, teta, m)

int j0,*i00,m;
int base[100][100];
double a[100][100], *b;
double *teta;
{
int i ,i0;
double S, Smax;

 i0=-1;
 Smax=-1.E+9;
 for (i=0; i<m; i++)
   {
    if ( a[i][j0] >EPSILON && base[i][1] > 0 )
      {
       S= -b[i]/a[i][j0] ;
       if ( S > Smax )
         {
	  i0=i;
	  Smax=S;
         }
      }
   }

#ifdef DEBUG
    printf(" \n DANS TETAMAX: j0=%d m=%d Smax=%f i0=%d",j0,m,Smax,i0);
    printf("\n");
#endif

*teta=Smax;
*i00=i0;
return(0) ;
}

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*  tetamin                                                                */
/*                                                                         */
/*-------------------------------------------------------------------------*/

int tetamin ( a, b, base, j0, i00, teta, m)

int j0,*i00,m;
int base[100][100];
double a[100][100], *b;
double *teta;
{
int i ,i0;
double S, Smin;

 i0=-1;
 Smin=+1.E+9;
 for (i=0; i<m; i++)
   {
    if ( a[i][j0] < -EPSILON && base[i][1] > 0 )
      {
       S= -b[i]/a[i][j0] ;
       if ( S < Smin )
         {
          i0=i;
          Smin=S;
         }
      }
    }

#ifdef DEBUG
    printf("\n DANS TETAMIN: j0=%d m=%d Smin=%f i0=%d",j0,m,Smin,i0);
    printf("\n");
#endif

*teta=Smin;
*i00=i0;
return(0) ;
}


/*-------------------------------------------------------------------------*/
/*                                                                         */
/*  affiche                                                                */
/*                                                                         */
/*-------------------------------------------------------------------------*/

void affiche(a,b,c,alpha,base,h_base,m,n)
int m,n;
double a[100][100], *b, *c;
int base[100][100], h_base[100][100];
double alpha;
{
int i,k;
static char *decode[]={"x","y","e"};

 printf("\n");
 printf ("\n  H-BA:  cste");
  for(i=0; i<n; i++)printf("      %s[%d]",decode[h_base[i][1]],h_base[i][0]);
  printf("\n  Z   = %f",alpha);
  for( i=0; i<n; i++)printf (" %f" ,c[i]);
  for( i=0; i<m; i++)
   {
     printf ("\n  %s[%d]= %f" , decode[base[i][1]],base[i][0],b[i] );
     for( k=0; k<n ; k++ ) printf (" %f" , a[i][k]);
   }
  printf("\n");
return;
}

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*  inter_espace                                                           */
/*   Calcul de l'intersection de 1/2 espaces                               */
/*                                                                         */
/*-------------------------------------------------------------------------*/

int inter_espace(a, b ,mm,nn)
int mm, nn;
double a[100][100], *b;
{
double alpha,teta;
int base[100][100], h_base[100][100];
double c[100];
int m,n,test,j,i0,k,i;

#ifdef DEBUG
 printf("mm=%d nn=%d\n", mm, nn) ;
for(i=0;i<mm;i++)
  {
    printf("%g -", b[i]) ;
    for(j=0;j<nn;j++)
      printf(" %g", a[i][j]) ;
    printf("\n") ; 
  }
#endif

 m=mm;
 n=nn;

 initialise(a, b ,c, &alpha,base, h_base, m , &n);

#ifdef DEBUG
 affiche(a,b,c,alpha,base,h_base,m,n);
#endif
test=1;
 while ( fabs(alpha) > EPSILON  && test==1 )
  {
   test=0;
   for (j=0; j<n && test==0; j++)
     {
      if ( fabs(c[j]) <= EPSILON ) continue;
      i0=-1;
      if ( c[j] > 0. )
       {
        if (h_base [j][1] == 0 ) tetamax ( a, b, base, j, &i0, &teta, m);
       }
      else
       {
        tetamin( a, b, base, j, &i0, &teta, m);
       }
      if ( i0 == -1 ) continue;
#ifdef DEBUG
  printf ("\n Z va baisser de:%f",teta*c[j]);
#endif
      transform(m,n,i0,j,a,b,&alpha,c);

#ifdef DEBUG
 affiche(a,b,c,alpha,base,h_base,m,n);
#endif

      for (k=0; k<2; k++)
        {
         i=h_base [j][k];
         h_base [j][k]=base[i0][k];
         base[i0][k]=i;
        }
      test=1;
     }
   }

 test=0;
 if ( fabs(alpha) <= EPSILON ) test=1;
return(test);
}


/*-------------------------------------------------------------------------*/
/*                                                                         */
/*  inter2_espace                                                          */
/*   Calcul de l'intersection de 1/2 espaces                               */
/*  Difference avec inter_espace: On calcule le pivot maximum              */
/*-------------------------------------------------------------------------*/

int inter2_espace(a, b ,mm,nn)
int mm, nn;
double a[100][100], *b;
{
double alpha,teta;
int base[100][100], h_base[100][100];
double c[100];
int m,n,test,j,i0,k,i;
double cout, S;
int j1,i1;

 i1 = j1 = 0 ; // To please the compiler
 m=mm;
 n=nn;

 initialise(a, b ,c, &alpha,base, h_base, m , &n);

#ifdef DEBUG
 affiche(a,b,c,alpha,base,h_base,m,n);
#endif
test=1;
 while ( fabs(alpha) > EPSILON  && test==1 )
  {
   test=0;
   cout=0.;
   for (j=0; j<n ; j++)
     {
      if ( fabs(c[j]) <= EPSILON ) continue;
      i0=-1;
      if ( c[j] > 0. )
       {
        if (h_base [j][1] == 0 ) tetamax ( a, b, base, j, &i0, &teta, m);
       }
      else
       {
        tetamin( a, b, base, j, &i0, &teta, m);
       }
      if ( i0 == -1 ) continue;
#ifdef DEBUG
  printf ("\n Z va baisser de:%f",teta*c[j]);
#endif
      S= fabs(teta*c[j]);
      if ( S >cout )
       {
        cout=S;
        j1=j;
        i1=i0;
       }
    }
#ifdef DEBUG
    printf ("\n Choix: i0=%d j0=%d cout=%f",i1,j1,cout);
#endif
    if ( cout == 0. ) test=0;
    else
     {
      j=j1; i0=i1;
      transform(m,n,i0,j,a,b,&alpha,c);

#ifdef DEBUG
 affiche(a,b,c,alpha,base,h_base,m,n);
#endif

      for (k=0; k<2; k++)
        {
         i=h_base [j][k];
         h_base [j][k]=base[i0][k];
         base[i0][k]=i;
        }
      test=1;
     }
   }

 test=0;
 if ( fabs(alpha) <= EPSILON ) test=1;
return(test);
}
