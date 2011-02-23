#include <stdio.h>
#include <stdlib.h>

int position(int i, int lar, int rep)
{
  int size ;

  size = lar/(rep*4) ;

  switch( (i/size) % 4 )
    {
    case 0:
      return i % size + size*(i/(4*size)) ;
    case 3:
      return i % size + size*(i/(4*size)) + 3*(lar/4) ;
    default:
      return (i % (4*size)) + (2*size)*(i/(4*size)) + lar/4 - size ;
    }
}

int main(int argc, char **argv)
{
  int rep ;
  char *image ;
  int i, j, x, y, lar, hau, tlar, thau ;
  char buf[99] ;

  if ( argc != 2 )
    {
      fprintf(stderr, "Donner le nombre de répétitions\n") ;
      exit(1) ;
    }

  rep = atoi(argv[1]) ;

  fgets(buf, sizeof(buf), stdin) ;

  scanf("%d%d%d%*c", &lar, &hau, &x) ;

  if ( x != 255 )
    {
      fprintf(stderr, "Probleme 255\n") ;
      exit(1) ;
    }

  if ( lar != hau || lar % (4*rep) != 0 )
    {
      fprintf(stderr, "Problème résolution\n") ;
      exit(1) ;
    }
  image = malloc(lar*hau*3) ;
  for(j=0; j<hau; j++)
    {
      y = position(j, hau, rep) * lar * 3 ;
      for(i=0; i<lar; i++)
	{
	  x = y + 3*position(i, lar, rep) ;
	  image[x] = getchar() ;
	  image[x+1] = getchar() ;
	  image[x+2] = getchar() ;
	}
    }
  if ( hau > 900 )
    {
      thau = 900*(hau/900) ;
      tlar = 900*(hau/900) ;
      printf("P6\n%d %d\n255\n", tlar, thau) ;
      for(j=(hau%900)/2; j<hau-(hau%900)/2; j++)
	fwrite(image + j*lar*3 + 3*(lar%900)/2, 3*tlar, 1, stdout) ;
    }
  else
    {
      printf("P6\n%d %d\n255\n", lar, hau) ;
      for(j=0; j<hau; j++)
	fwrite(image + j*lar*3 + 3*lar, 3*lar, 1, stdout) ;
    }
  return 0;
}
