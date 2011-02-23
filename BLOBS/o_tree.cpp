#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "implicit.h"
#include "triangle.h"
#include "list.h"
#include "cube.h"
#include <GL/gl.h>
#include "vector.h"
#include "baseogl.h"
#include "blobtree.h"

extern "C" {
#include "o_objet.h"
#include "t_blob.h" /* Type Logique du Blob */
#include "t_geometrie.h"	  
}

struct essai
{
  TreeNode *resultat;
  TreeNode *pat;                   /* pat et bobby serve pour les fonction qui sont en cours de developpement */
  TreeNode *bobby;
  Implicit *blob ;                          /* Pr affichage de meme qu'avant */
  List<Triangle> *facettes[10] ;            /* Pr affichage de meme qu'avant*/   
  int nb;                                   /* qualite de meme qu'avant*/
  double seuil,type,coef,a,b,c;                             /* Sert de coefficient que je ne connait pas */
  Triplet coord,coord2,coord3;              /* Sert a la construction des vecteurs,donne les coord de celui-ci */
};

/*-----------------------------------------------------------------------------------------------------------------------*/
/*                                                  INCHANGER                                                            */
/*-----------------------------------------------------------------------------------------------------------------------*/

/* AFFICHAGE (De meme que pour un blob dans o_blob.cpp) */
static void affiche_blob(struct essai *o, const Champ *ch, const char *urgent)
{
  int n, nb ;

  n = *urgent - '0' ;
  if ( n<0 || n>9 )
    return ;
  // creation des facettes du blobtree

  if ( o->facettes[n] == NULL )
    {
      o->facettes[n] = new List<Triangle>() ;
      nb = (int)(o->nb * pow(sqrt(2.), n-5)) ;
      Polygonize(o->blob
		 ,o->blob->GetBox()
		 ,nb,*o->facettes[n]
		 ,Cube::Lookup
		 ) ;
    }

  glEnable(GL_LIGHTING) ;
  glBegin(GL_TRIANGLES);
  
for (ListNode<Triangle>* e=o->facettes[n]->Node();e!=NULL;e=e->Next())
    {
      Normal3D(e->E().Normal());
      Vertex3D(e->E()[0]);
      Vertex3D(e->E()[1]);
      Vertex3D(e->E()[2]);
    }
  glEnd();
  glDisable(GL_LIGHTING) ;
}

/*-----------------------------------------------------------------------------------------------------------------------*/
/*                                                  INCHANGER                                                            */
/*-----------------------------------------------------------------------------------------------------------------------*/

static void detruit_blob(struct essai *o)
{/* DESTRUCTION des objets */
  int i ;
  for(i=0;i<10;i++)
    if ( o->facettes[i] )
      {
	delete o->facettes[i] ;
	o->facettes[i] = NULL ;
      }
  if ( o->blob )
    delete o->blob ;
}
/*-----------------------------------------------------------------------------------------------------------------------*/
/*  ICI  EXEMPLE du calcul de l'hyperblend  pour un arbre binaire                                        */
/*-----------------------------------------------------------------------------------------------------------------------*/
// static void calcul_union(struct essai *o)
// {      
//   detruit_blob(o) ; 
//   Set<TreeNode*> set; /* Creation du noeud a l'aide des 2 arbres qu'on a recuperer */
//   set.Insert(o->pat); 
//   set.Insert(o->bobby);
//   o->resultat = new TreeNodeHyperBlend(set,o->a);  /* Realisation de l'hyperblend */
//   BlobTree *blobtree=new BlobTree(o->resultat,o->b); 
//   o->blob = blobtree;  
// }

/*-----------------------------------------------------------------------------------------------------------------------*/
/* La methode de construction de tout les autres formes est base sur le meme principe */
/*-----------------------------------------------------------------------------------------------------------------------*/

static void calcul_blend2(struct essai *o)                      /* Version 2 des fonctions => Multibranche */
{/* Melange de blob */                                                             
  const Champ *ch ;
  Set<TreeNode*> set;
  detruit_blob(o);
  POUR_LES_FILS(o, f,
		{
		  if ( c_trouve_type((const void**)&f, &ch, &infos_L_blob) )
		    set.Insert(T(TreeNode*, f, ch)) ;
		}) ;
  o->resultat = new TreeNodeBlend(set); 
  o->blob = new BlobTree(o->resultat,o->seuil) ;
}

static void calcul_union2(struct essai *o)
{/* Melange hyperbolique */
  const Champ *ch ;
  Set<TreeNode*> set;  
  detruit_blob(o);
  POUR_LES_FILS(o, f,
		{
		  if ( c_trouve_type((const void**)&f, &ch, &infos_L_blob) )
		    set.Insert(T(TreeNode*, f, ch)) ;
		}) ;
  o->resultat = new TreeNodeHyperBlend(set,o->type);  /* Realisation de l'hyperblend */
  BlobTree *blobtree=new BlobTree(o->resultat,o->seuil); 
  o->blob = blobtree;    
}

static void calcul_unif2(struct essai *o)
{/* Union de blob */    
  const Champ *ch ;
  Set<TreeNode*> set;  
  detruit_blob(o);
  POUR_LES_FILS(o, f,
		{
		  if ( c_trouve_type((const void**)&f, &ch, &infos_L_blob) )
		    set.Insert(T(TreeNode*, f, ch)) ;
		}) ;
  o->resultat = new TreeNodeUnion(set);  
  BlobTree *blobtree=new BlobTree(o->resultat,o->seuil); 
  o->blob = blobtree;
}

static void calcul_inters2(struct essai *o)
{/* Intersection de blob */
  const Champ *ch ;
  Set<TreeNode*> set;  
  detruit_blob(o);
  POUR_LES_FILS(o, f,
		{
		  if ( c_trouve_type((const void**)&f, &ch, &infos_L_blob) )
		    set.Insert(T(TreeNode*, f, ch)) ;
		}) ;
  o->resultat = new TreeNodeIntersection(set);  
  BlobTree *blobtree=new BlobTree(o->resultat,o->seuil); 
  o->blob = blobtree;
}

static void calcul_alpha(struct essai *o)
{/* Grossissement de Blob */    
  detruit_blob(o) ;
  o->resultat = new TreeNodeAlpha(o->coef,o->pat);  
  BlobTree *blobtree=new BlobTree(o->resultat,o->seuil); 
  o->blob = blobtree;    
}

/*--------------------------------------------------------------------------------------*/
/* En Cours de construction ces fonctions => j`ai realiser la suite pour test           */
/*--------------------------------------------------------------------------------------*/
static void calcul_difference(struct essai *o)
  /* Difference de blob */
{ /* En Cours de travail => pour debug */
  detruit_blob(o) ;
  Set<TreeNode*> set;
  set.Insert(o->pat);
  set.Insert(o->bobby);
  o->resultat = new TreeNodeDifference(set);
  BlobTree *blobtree=new BlobTree(o->resultat,o->seuil); 
  o->blob = blobtree;    
}

static void calcul_twist(struct essai *o)
  /* Twist */
{    /* En Cours de travail => pour debug */
  detruit_blob(o) ;
  Set<TreeNode*> set;  
  o->resultat = new TreeTwist(o->pat,
			      Vector(o->coord.x,o->coord.y,o->coord.z),
			      Vector(o->coord2.x,o->coord2.y,o->coord2.z),
			      Vector(o->coord3.x,o->coord3.y,o->coord3.z),
			      o->a
			      );
  BlobTree *blobtree=new BlobTree(o->resultat,o->seuil); 
  o->blob = blobtree;
}
static void calcul_taper(struct essai *o)
  /* Taper */
{    /* En Cours de travail => pour debug */
  detruit_blob(o) ;
  Set<TreeNode*> set;  
  o->resultat = new TreeTaper(o->pat,
			      Vector(o->coord.x,o->coord.y,o->coord.z),
			      Vector(o->coord2.x,o->coord2.y,o->coord2.z),
			      o->a,
			      o->b
			      );
  BlobTree *blobtree=new BlobTree(o->resultat,o->seuil); 
  o->blob = blobtree;
}

/*-----------------------------------------------------------------------------------------------------------------------*/
/* CLASSE */
/*-----------------------------------------------------------------------------------------------------------------------*/

extern "C" {
/* MELANGE */
CLASSE(blend,		
       struct essai, 
       CHAMP(resultat,L_blob P_entier /*Obligatoire*/) /* Arbre renvoye pour la realisation du blobtree */
       CHAMP(nb, L_entier Edite Sauve DEFAUT("30")/* Pour le reste des variable voir au debut (structure) */
	     BULLE_D_AIDE("Taille de la discrétisation en mode d'affichage\n"
			  "qualité normale.\n"))
       CHAMP(seuil, L_flottant Edite Sauve DEFAUT("1")	     
	     BULLE_D_AIDE(" Seuil"))
       CHANGEMENT(calcul_blend2)
       DESTRUCTION(detruit_blob)
       CHAMP_VIRTUEL(L_affiche_gl(affiche_blob))
       MENU("Blobs/Noeuds/Blend")
       PAQUET(NB_ACTIFS)
      )

/* MELANGE HYPERBOLIC */
CLASSE(hyperblend,		
       struct essai, 
       CHAMP(resultat,L_blob P_entier)
       CHAMP(nb, L_entier Edite Sauve DEFAUT("30")
	     BULLE_D_AIDE("Taille de la discrétisation en mode d'affichage\n"
			  "qualité normale.\n"))
       CHAMP(type, L_flottant Edite Sauve DEFAUT("1")
	     BULLE_D_AIDE("Type de l'hyperbole ne doit jamais etre nul car sinon pas d'affichage"))  
       CHAMP(seuil, L_flottant Edite Sauve DEFAUT("1")
	     BULLE_D_AIDE("Seuil \n"))
       CHANGEMENT(calcul_union2)
       DESTRUCTION(detruit_blob)
       CHAMP_VIRTUEL(L_affiche_gl(affiche_blob))
       MENU("Blobs/Noeuds/HyperBlend")
       PAQUET(NB_ACTIFS)
       )

/*UNION */
CLASSE(unifie,		
       struct essai, 
       CHAMP(resultat,L_blob P_entier)
       CHAMP(nb, L_entier Edite Sauve DEFAUT("30")
	     BULLE_D_AIDE("Taille de la discrétisation en mode d'affichage\n"
			  "qualité normale.\n"))
       CHAMP(seuil, L_flottant Edite Sauve DEFAUT("1")
	     BULLE_D_AIDE(" Seuil\n")) 
       CHANGEMENT(calcul_unif2)
       DESTRUCTION(detruit_blob)
       CHAMP_VIRTUEL(L_affiche_gl(affiche_blob))
       MENU("Blobs/Noeuds/Union")
       PAQUET(NB_ACTIFS)
      )

/* INTERSECTION */
CLASSE(intersecte,		
       struct essai, 
       CHAMP(resultat,L_blob P_entier)
       CHAMP(nb, L_entier Edite Sauve DEFAUT("30")
	     BULLE_D_AIDE("Taille de la discrétisation en mode d'affichage\n"
			  "qualité normale.\n"))
       CHAMP(a, L_flottant Edite Sauve DEFAUT("1")
	     BULLE_D_AIDE(" Seuil\n"))       
       CHANGEMENT(calcul_inters2)
       DESTRUCTION(detruit_blob)
       CHAMP_VIRTUEL(L_affiche_gl(affiche_blob))
       MENU("Blobs/Noeuds/Intersection")
       PAQUET(NB_ACTIFS)
      )

/* ALPHA */
CLASSE(alpha,		
       struct essai, 
       CHAMP(resultat,L_blob P_entier)
       CHAMP(pat,L_blob P_entier Extrait Obligatoire
	     BULLE_D_AIDE(" Blob a traiter"))
       CHAMP(nb, L_entier Edite Sauve DEFAUT("30")
	     BULLE_D_AIDE("Taille de la discrétisation en mode d'affichage\n"
			  "qualité normale.\n"))
       CHAMP(seuil, L_flottant Edite Sauve DEFAUT("1")
	     BULLE_D_AIDE(" Seuil\n")) 
       CHAMP(coef, L_flottant Edite Sauve DEFAUT("1")
	     BULLE_D_AIDE(" Coefficient de grossisement du rayon\n"
			  " TreeNodeAlpha(b,pat)")) 
       CHANGEMENT(calcul_alpha)
       DESTRUCTION(detruit_blob)
       CHAMP_VIRTUEL(L_affiche_gl(affiche_blob))
       MENU("Blobs/Noeuds/Alpha")
      )

/*-----------------------------------------------------------------------------------------------------------------------*/
/* CLASSE des fonctions en cours de developpement  */
/*-----------------------------------------------------------------------------------------------------------------------*/

/* TEST pour DIFFERENCE*/
CLASSE(diff,		
       struct essai, 
       CHAMP(resultat,L_blob P_entier)
       CHAMP(pat,L_blob P_entier Extrait Obligatoire
	     BULLE_D_AIDE(" Blob a traiter"))  
       CHAMP(bobby,L_blob P_entier Extrait Obligatoire
	     BULLE_D_AIDE(" Blob a traiter"))
       CHAMP(nb, L_entier Edite Sauve DEFAUT("30")
	     BULLE_D_AIDE("Taille de la discrétisation en mode d'affichage\n"
			  "qualité normale.\n"))
       CHAMP(seuil, L_flottant Edite Sauve DEFAUT("1")
	     BULLE_D_AIDE("Seuil")) 
       CHANGEMENT(calcul_difference)
       DESTRUCTION(detruit_blob)
       CHAMP_VIRTUEL(L_affiche_gl(affiche_blob))
       MENU("Blobs/En test/Difference")
      )

/* TEST pour TWIST */
CLASSE(tounicoton,		
       struct essai, 
       CHAMP(resultat,L_blob P_entier)
       CHAMP(pat,L_blob P_entier Extrait Obligatoire
	     BULLE_D_AIDE(" Blob a traiter") )  
       CHAMP(nb, L_entier Edite Sauve DEFAUT("30")
	     BULLE_D_AIDE("Taille de la discrétisation en mode d'affichage\n"
			  "qualité normale.\n"))
       CHAMP(coord, L_point3 Edite Sauve
	     BULLE_D_AIDE("Coord. du vecteur pour realiser\n"
			  "TreeTwist(pat,Vector(coord),Vector(coord2),Vector(coord3),a)"))
       CHAMP(coord2,L_point3 Edite Sauve
	     BULLE_D_AIDE("Coord. du vecteur pour realiser\n"
			  "TreeTwist(pat,Vector(coord),Vector(coord2),Vector(coord3),a)"))
       CHAMP(coord3,L_point3 Edite Sauve
	     BULLE_D_AIDE("Coord. du vecteur pour realiser\n"
			  "TreeTwist(pat,Vector(coord),Vector(coord2),Vector(coord3),a)"))
       CHAMP(a, L_flottant Edite Sauve DEFAUT("1")
	     BULLE_D_AIDE("Coefficient double a pour realiser\n"
			  "TreeTwist(pat,Vector(coord),Vector(coord2),Vector(coord3),a)"))
       CHAMP(seuil, L_flottant Edite Sauve DEFAUT("1")
	     BULLE_D_AIDE(" Seuil\n"))
       CHANGEMENT(calcul_twist)
       DESTRUCTION(detruit_blob)
       CHAMP_VIRTUEL(L_affiche_gl(affiche_blob))
       MENU("Blobs/En test/Twist")
      )

/* TEST pour TAPER */
CLASSE(jesepa,		
       struct essai, 
       CHAMP(resultat,L_blob P_entier)
       CHAMP(pat,L_blob P_entier Extrait Obligatoire
	     BULLE_D_AIDE(" Blob a traiter"))  
       CHAMP(nb, L_entier Edite Sauve DEFAUT("30")
	     BULLE_D_AIDE("Taille de la discrétisation en mode d'affichage\n"
			  "qualité normale.\n"))
       CHAMP(coord, L_point3 Edite Sauve
	     BULLE_D_AIDE("Coord. du vecteur pour realiser\n"
			  "TreeTaper(pat,Vector(coord),Vector(coord2),a,b)"))
       CHAMP(coord2,L_point3 Edite Sauve
	     BULLE_D_AIDE("Coord. du vecteur pour realiser\n"
			  "TreeTaper(pat,Vector(coord),Vector(coord2),a,b)"))      
       CHAMP(a, L_flottant Edite Sauve DEFAUT("1")
	      BULLE_D_AIDE("Coef. double a pour realiser\n"
			  "TreeTaper(pat,Vector(coord),Vector(coord2),a,b)")) 
       CHAMP(b, L_flottant Edite Sauve DEFAUT("1")
	     BULLE_D_AIDE("Coef. double b pour realiser\n"
			  "TreeTaper(pat,Vector(coord),Vector(coord2),a,b)"))
       CHAMP(seuil, L_flottant Edite Sauve DEFAUT("1")
	     BULLE_D_AIDE("Seuil")) 
       CHANGEMENT(calcul_taper)
       DESTRUCTION(detruit_blob)
       CHAMP_VIRTUEL(L_affiche_gl(affiche_blob))
       MENU("Blobs/En test/Taper")
      )
}

