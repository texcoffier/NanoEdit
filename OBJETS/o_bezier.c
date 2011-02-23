/*
    NanoÉdit: Un modeleur algébrique interactif.
    Copyright (C) 2006 Thierry EXCOFFIER, Université Claude Bernard, LIRIS

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

#include "o_objet.h"		/* Car il faut definir un objet */
#include "t_geometrie.h"

typedef struct
{
  Table_triplet out ;
  Table_triplet in ;
  int depth ;
} Tbl ;


static void castel(Table_triplet *t, int nb_in, int in, int nb_out)
{
  int i, j, out ;
  Triplet nouveau_milieu, milieu ;

  if ( nb_in == nb_out )
    return ;
  
  out = in + nb_out/2 ;

  if ( TRACE(t) )
    fprintf(stderr, "castel nB_in=%d in=%d nb_out=%d out=%d\n",
	    nb_in, in, nb_out, out) ;

  i = 0 ; // Pour faire plaisir au compilo
  for(j=in; j < in + nb_in - 1; j++)
    {
      milieu = t->table[j] ;
      for(i = j + 1; i < in + nb_in; i++)
	{
	  nouveau_milieu = u_milieu_2_triplet(&t->table[i-1], &t->table[i]) ;
	  t->table[i-1] = milieu ;
	  milieu = nouveau_milieu ;
	}
      t->table[out+nb_in-(j-in+1)] = t->table[i-1] ;
      t->table[i-1] = milieu ;
    }
  t->table[out+nb_in-(j-in+1)] = t->table[i-1] ;

  castel(t, nb_in, in, nb_out/2) ;
  castel(t, nb_in, out, nb_out/2) ;
}

static void changement_bezier(void *oo)
{
  int i ;
  Tbl *o = oo ;

  o->out.nb = o->in.nb << o->depth ;
  REALLOUER(o->out.table, o->out.nb) ;
  for(i=0;i<o->in.nb; i++)
    o->out.table[i] = o->in.table[i] ;

  if ( TRACE(o) )
    fprintf(stderr, "depth=%d in=%d out=%d\n", o->depth, o->in.nb, o->out.nb) ;
  
  castel(&o->out, o->in.nb, 0, o->out.nb) ;

  if ( TRACE(o) )
    fprintf(stderr, "fin\n") ;
}

CLASSE(bezier, Tbl,
       SUR_CLASSE("table")
       CHAMP(out, L_table_point P_table_triplet Affiche)
       CHAMP(in, L_table_point P_table_triplet Extrait Edite Sauve)
       CHAMP(depth, L_nombre P_entier Extrait Edite Sauve DEFAUT("1"))
       MENU("Figures/1d/Bézier")
       EVENEMENT("Shft+BE")
       CHANGEMENT(changement_bezier)
       )



typedef struct
{
  Table_quadruplet out ;
  Table_quadruplet in ;
  int nb ;
  Booleen fermee ;
} Spline2 ;

static void changement_spline2(void *oo)
{
  Spline2 *o = oo ;
  int i, j, w, fermee ;
  Quadruplet pa, pb, p1, p2 ;
  float f ;

  if ( o->in.nb < 3 )
    {
      NOTE_CHAMP_INVALIDE(o, in) ;
      return ;
    }

  if ( o->fermee )
    fermee = 2 ;
  else
    fermee = 0 ;

  o->out.nb = (o->in.nb - 2 + fermee) * o->nb + 1 ;
  REALLOUER(o->out.table, o->out.nb) ;

  for(i=1, w=0; i<o->in.nb - 1 + fermee; i++)
    {
      pa = u_milieu_2_quadruplet(&o->in.table[(i-1)%o->in.nb],
			      &o->in.table[i%o->in.nb]) ;
      pb = u_milieu_2_quadruplet(&o->in.table[i%o->in.nb],
			      &o->in.table[(i+1)%o->in.nb]) ;
      for(j=0; j<o->nb; j++)
	{
	  f = j / (float)o->nb ;
	  p1 = u_barycentre_2_quadruplet(&pa, f, &o->in.table[i%o->in.nb]) ;
	  p2 = u_barycentre_2_quadruplet(&o->in.table[i%o->in.nb], f, &pb) ;
	  o->out.table[w++] = u_barycentre_2_quadruplet(&p1, f, &p2) ;
	}
    }
  o->out.table[w++] = pb ;
}

CLASSE(spline2, Spline2,
       SUR_CLASSE("table")
       CHAMP(out, L_table_point P_table_quadruplet Affiche)
       CHAMP(in, L_table_point P_table_quadruplet Extrait Edite Sauve)
       CHAMP(nb, L_nombre P_entier Extrait Edite Sauve DEFAUT("10"))
       CHAMP(fermee, L_booleen Extrait Edite Sauve DEFAUT("0"))
       MENU("Figures/1d/Spline degré 2")
       EVENEMENT("Shft+S2")
       CHANGEMENT(changement_spline2)
       )




