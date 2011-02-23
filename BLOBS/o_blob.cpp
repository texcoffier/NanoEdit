#include <stdio.h>		// Comme d'hab
#include <stdlib.h>
#include <math.h>

#include "implicit.h"		//  Demander a celui qui a creer ces points H
#include "triangle.h"
#include "list.h"
#include "cube.h"
#include <GL/gl.h>
#include "vector.h"
#include "baseogl.h"
#include "blobtree.h"            /* J'insere les differentes fonctions pour l'arbre */

extern "C" {
#include "o_objet.h"		/* Car il faut définir un objet */
#include "t_blob.h"                  /* Je dois changer le nom */
#include "t_geometrie.h"
}

struct bob 		  
{
  Implicit *blob ;                          /* Pr affichage de meme qu'avant */
  List<Triangle> *facettes[10] ;            /* Pr affichage de meme qu'avant*/  
  Triplet coord;                            /* Sert a la construction des vecteurs,donne les coord de celui-ci */
  Triplet coord2;                           /* Voir au-dessus */
  int nb;                                   /* qualite de meme qu'avant*/
  TreeNode *pat;                            /* Arbre renvoyer par le programme ~ une feuille */
  double rayon,intensite,rayon2,seuil;           /* Sert de coefficient pour les fonctions */
  int type;                                  /* Pareille */
} ;

/*-----------------------------------------------------------------------------------------------------------------------*/
/*                                                  INCHANGER                                                            */
/*-----------------------------------------------------------------------------------------------------------------------*/

/* AFFICHAGE DES BLOBS */

static void affiche_blob(struct bob *o, const Champ *ch, const char *urgent)
{
  int n, nb ;

  n = *urgent - '0' ;
  if ( n<0 || n>9 ) /* Qualite d'affichage gere dans Nanoedit */
    return ;

  /* creation des facettes du blobtree */
  /* Pour plus d'info pour la suite demander a son createur*/

  if ( o->facettes[n] == NULL )
    {
      o->facettes[n] = new List<Triangle>() ;
      nb = (int)(o->nb * pow(sqrt(2.), n-5)) ;
      Polygonize(o->blob
		 ,o->blob->GetBox()
		 ,nb,*o->facettes[n]
		 ,Cube::Lookup
		 );
    }

 /* Affichage */

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

static void detruit_blob(struct bob *o)
{
  /* Desturction des objets */
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
/*  Calcul d'un blob spherique                                                                                        */
/*-----------------------------------------------------------------------------------------------------------------------*/
static void calcul_blob_sphere(struct bob *o)
{   
				/* Sphere */
  detruit_blob(o) ;
  PolynomialBlend *function=new PolynomialBlend(o->rayon,o->intensite,o->type);
  Set<TreeNode*> set;       
  set.Insert(new TreeVertex(Vector(o->coord.x,o->coord.y,o->coord.z),function)); /*Creation de 2 breanche d'un arbre*/
  set.Insert(new TreeVertex(Vector(o->coord.x,o->coord.y,o->coord.z),function)); /*Si 1 seule branche => pbs d'affichage*/
  o->pat =new TreeNodeBlend(set); 
  o->blob = new BlobTree(o->pat,o->seuil); 
}

/*-----------------------------------------------------------------------------------------------------------------------*/
/* La methode de construction de tout les autres formes est base sur le meme principe */
/*-----------------------------------------------------------------------------------------------------------------------*/

static void calcul_blob_box(struct bob *o)
{ /* Boite */    
  detruit_blob(o) ;
  PolynomialBlend *function=new PolynomialBlend(o->rayon,o->intensite,o->type);
  Set<TreeNode*> set;    
  set.Insert(new TreeBox(Vector(o->coord.x,o->coord.y,o->coord.z),
			 Vector(o->coord2.x,o->coord2.y,o->coord2.z),
			 function));
  set.Insert(new TreeBox(Vector(o->coord.x,o->coord.y,o->coord.z),
			 Vector(o->coord2.x,o->coord2.y,o->coord2.z),
			 function));
  o->pat =new TreeNodeBlend(set); 
  o->blob = new BlobTree(o->pat,o->seuil); 
}

static void calcul_blob_cylindric2(struct bob *o)
{ /* Cylindre */
  detruit_blob(o) ;
  Set<TreeNode*> set;  
  PolynomialBlend *function=new PolynomialBlend(o->rayon,o->intensite,o->type);
  set.Insert(new TreeCylinder(Vector(o->coord.x,o->coord.y,o->coord.z), 
			      Vector(o->coord2.x,o->coord2.y,o->coord2.z), 
			      o->rayon2,
			      function));
  set.Insert(new TreeCylinder(Vector(o->coord.x,o->coord.y,o->coord.z), 
			      Vector(o->coord2.x,o->coord2.y,o->coord2.z), 
			      o->rayon2,
			      function));
  o->pat =new TreeNodeBlend(set); 
  o->blob = new BlobTree(o->pat,o->seuil); 
}

static void calcul_blob_cylindric1(struct bob *o)
{ /* Arete */
  detruit_blob(o) ;
  Set<TreeNode*> set;  
  PolynomialBlend *function=new PolynomialBlend(o->rayon,o->intensite,o->type);
  set.Insert(new TreeEdge(Vector(o->coord.x,o->coord.y,o->coord.z), 
			  Vector(o->coord2.x,o->coord2.y,o->coord2.z), 
			  function));
  set.Insert(new TreeEdge(Vector(o->coord.x,o->coord.y,o->coord.z), 
			  Vector(o->coord2.x,o->coord2.y,o->coord2.z), 
			  function));
  o->pat =new TreeNodeBlend(set); 
  o->blob = new BlobTree(o->pat,o->seuil); 
}

static void calcul_blob_disque(struct bob *o)
{ /* Disque ( on dirai plus une meule a fromage */
  detruit_blob(o) ;
  Set<TreeNode*> set;  
  PolynomialBlend *function=new PolynomialBlend(o->rayon,o->intensite,o->type);
  set.Insert(new TreeCircle(Vector(o->coord.x,o->coord.y,o->coord.z), 
				     Vector(o->coord2.x,o->coord2.y,o->coord2.z),
				     o->rayon2,
				     function));
  set.Insert(new TreeCircle(Vector(o->coord.x,o->coord.y,o->coord.z), 
				     Vector(o->coord2.x,o->coord2.y,o->coord2.z),
				     o->rayon2,
				     function));
  o->pat =new TreeNodeBlend(set); 
  o->blob = new BlobTree(o->pat,o->seuil);
}

static void calcul_blob_tore(struct bob *o)
{ /* Tore ou donuts */
  detruit_blob(o) ;
  Set<TreeNode*> set;  
  PolynomialBlend *function=new PolynomialBlend(o->rayon,o->intensite,o->type);
  set.Insert(new TreeTorus(Vector(o->coord.x,o->coord.y,o->coord.z), 
				     Vector(o->coord2.x,o->coord2.y,o->coord2.z),
				     o->rayon2,
				     function));
  set.Insert(new TreeTorus(Vector(o->coord.x,o->coord.y,o->coord.z), 
				     Vector(o->coord2.x,o->coord2.y,o->coord2.z),
				     o->rayon2,
				     function));
  o->pat =new TreeNodeBlend(set); 
  o->blob = new BlobTree(o->pat,o->seuil);
}
/*-----------------------------------------------------------------------------------------------------------------------*/
/* CLASSE */
/*-----------------------------------------------------------------------------------------------------------------------*/

/* TORE */ 

extern "C" {
CLASSE(blobtore,		
       struct bob, 
       CHAMP(pat,L_blob P_entier)       
       CHAMP(nb, L_entier Edite Sauve DEFAUT("30")
	     BULLE_D_AIDE("Taille de la discrétisation en mode d'affichage\n"
			  "qualité normale.\n"))	         
       CHAMP(coord, L_point3 Edite Sauve /* Pour le reste des variable voir au debut (structure) */
	     BULLE_D_AIDE("Position du Centre "))
       CHAMP(coord2, L_point3 Edite Sauve /* Pour le reste des variable voir au debut (structure) */
	     BULLE_D_AIDE("Direction de la normale "))
       CHAMP(rayon, L_flottant Edite Sauve DEFAUT("1.0")
	     BULLE_D_AIDE("Rayon du Blob\n ")) 
       CHAMP(intensite, L_flottant Edite Sauve DEFAUT("1.0")
	     BULLE_D_AIDE("Intensite du Blob\n")) 
       CHAMP(type, L_entier Edite Sauve
	     BULLE_D_AIDE("Type de la fonction \n "))       
       CHAMP(rayon2, L_flottant Edite Sauve DEFAUT("1.0"))
       CHAMP(seuil, L_flottant Edite Sauve DEFAUT("1.0")
	     BULLE_D_AIDE(" Seuil \n ")) 
       CHANGEMENT(calcul_blob_tore)
       DESTRUCTION(detruit_blob)
       CHAMP_VIRTUEL(L_affiche_gl(affiche_blob))
       MENU("Blobs/En test/Tore")
       )

/* DISQUE */

CLASSE(blobdisque,		
       struct bob, 
       CHAMP(pat,L_blob P_entier)       
       CHAMP(nb, L_entier Edite Sauve DEFAUT("30")
	     BULLE_D_AIDE("Taille de la discrétisation en mode d'affichage\n"
			  "qualité normale.\n"))	         
       CHAMP(coord, L_point3 Edite Sauve /* Pour le reste des variable voir au debut (structure) */
	     BULLE_D_AIDE("Position du Centre "))
       CHAMP(coord2, L_point3 Edite Sauve /* Pour le reste des variable voir au debut (structure) */
	     BULLE_D_AIDE("Direction de la normale "))
       CHAMP(rayon, L_flottant Edite Sauve DEFAUT("1.0")
	     BULLE_D_AIDE("Rayon du Blob\n ")) 
       CHAMP(intensite, L_flottant Edite Sauve DEFAUT("1.0")
	     BULLE_D_AIDE("Intensite du Blob\n")) 
       CHAMP(type, L_entier Edite Sauve
	     BULLE_D_AIDE("Type de la fonction \n "))       
       CHAMP(rayon2, L_flottant Edite Sauve DEFAUT("1.0")
	     BULLE_D_AIDE("Rayon du disque"))
       CHAMP(seuil, L_flottant Edite Sauve DEFAUT("1.0")
	      BULLE_D_AIDE(" Seuil \n ")) 
       CHANGEMENT(calcul_blob_disque)
       DESTRUCTION(detruit_blob)
       CHAMP_VIRTUEL(L_affiche_gl(affiche_blob))
       MENU("Blobs/En test/Disque")
       )

/* SPHERE */

CLASSE(blobsphere,		
       struct bob, 
       CHAMP(pat,L_blob P_entier)       
       CHAMP(nb, L_entier Edite Sauve DEFAUT("30")
	     BULLE_D_AIDE("Taille de la discrétisation en mode d'affichage\n"
			  "qualité normale.\n"))	         
       CHAMP(coord, L_point3 Edite Sauve Extrait Init3d /* Pour le reste des variable voir au debut (structure) */
	     BULLE_D_AIDE("Position du Blob "))
       CHAMP(rayon, L_flottant Edite Sauve DEFAUT("1.0")
	     BULLE_D_AIDE("Rayon du Blob\n ")) 
       CHAMP(intensite, L_flottant Edite Sauve DEFAUT("1.0")
	     BULLE_D_AIDE("Intensite du Blob\n")) 
       CHAMP(type, L_entier Edite Sauve
	     BULLE_D_AIDE("Type de la fonction \n "))       
       CHAMP(seuil, L_flottant Edite Sauve DEFAUT("1.0")
	      BULLE_D_AIDE(" Seuil \n ")) 
       CHANGEMENT(calcul_blob_sphere)
       DESTRUCTION(detruit_blob)
       CHAMP_VIRTUEL(L_affiche_gl(affiche_blob))
       MENU("Blobs/Primitives/Sphere")
       )

 /* BOITE */

CLASSE(blobcube,		
       struct bob, 
       CHAMP(pat,L_blob P_entier)                     
       CHAMP(nb, L_entier Edite Sauve DEFAUT("30")
	     BULLE_D_AIDE("Taille de la discrétisation en mode d'affichage\n"
			  "qualité normale.\n"))	         
       CHAMP(coord, L_point3 Edite Sauve
	     BULLE_D_AIDE("Position d'un coin pour le Blob "))
       CHAMP(coord2, L_point3 Edite Sauve
	     BULLE_D_AIDE("Position du coin opposé pour le Blob "))
       CHAMP(rayon, L_flottant Edite Sauve DEFAUT("1.0")
	     BULLE_D_AIDE("Rayon du Blob\n ")) 
       CHAMP(intensite, L_flottant Edite Sauve DEFAUT("1.0")
	     BULLE_D_AIDE("Intensite du Blob\n")) 
       CHAMP(type, L_entier Edite Sauve
	     BULLE_D_AIDE("Type de la fonction \n "))             
       CHAMP(seuil, L_flottant Edite Sauve DEFAUT("1.0")
	      BULLE_D_AIDE(" Seuil \n ")) 
       CHANGEMENT(calcul_blob_box)
       DESTRUCTION(detruit_blob)
       CHAMP_VIRTUEL(L_affiche_gl(affiche_blob))
       MENU("Blobs/Primitives/Cube")
       )

/* ARETE */

CLASSE(blobarete,		
       struct bob, 
       CHAMP(pat,L_blob P_entier)                     
       CHAMP(nb, L_entier Edite Sauve DEFAUT("30")
	     BULLE_D_AIDE("Taille de la discrétisation en mode d'affichage\n"
			  "qualité normale.\n"))	         
       CHAMP(coord, L_point3 Edite Sauve
	     BULLE_D_AIDE("Position pour le Blob ")
	     )
       CHAMP(coord2, L_point3 Edite Sauve
	     BULLE_D_AIDE("Position pour le Blob ")
	     )      
       CHAMP(rayon, L_flottant Edite Sauve DEFAUT("1.0")   
	     BULLE_D_AIDE("Rayon du Blob\n ")) 
       CHAMP(intensite, L_flottant Edite Sauve DEFAUT("1.0")   
	     BULLE_D_AIDE("Intensite du Blob\n")) 
       CHAMP(type, L_entier Edite Sauve
	     BULLE_D_AIDE("Type de la fonction \n "))                
       CHAMP(seuil, L_flottant Edite Sauve DEFAUT("1.0")
	     BULLE_D_AIDE(" Seuil \n ")) 
       CHANGEMENT(calcul_blob_cylindric1)
       DESTRUCTION(detruit_blob)
       CHAMP_VIRTUEL(L_affiche_gl(affiche_blob))      
       MENU("Blobs/Primitives/Arete")
       )

/* CYLINDRE */  

CLASSE(blobcylindre,		
       struct bob, 
       CHAMP(pat,L_blob P_entier)                     
       CHAMP(nb, L_entier Edite Sauve DEFAUT("30")
	     BULLE_D_AIDE("Taille de la discrétisation en mode d'affichage\n"
			  "qualité normale.\n"))	         
       CHAMP(coord, L_point3 Edite Sauve Extrait Init3d
	     BULLE_D_AIDE("Position  d'un point de l'arete pour le Blob\n"))
       CHAMP(coord2, L_point3 Edite Sauve Extrait Init3d
	     BULLE_D_AIDE("Position de l'autre point de l'arete\n"))
       CHAMP(rayon, L_flottant Edite Sauve DEFAUT("1.0")
	     BULLE_D_AIDE("Rayon du Blob \n ")) 
       CHAMP(intensite, L_flottant Edite Sauve DEFAUT("1.0")
	     BULLE_D_AIDE("Intensite du Blob \n ")) 
       CHAMP(type, L_entier Edite Sauve   
	     BULLE_D_AIDE("Type de la fonction \n " )) 
       CHAMP(rayon2, L_flottant Edite Sauve DEFAUT("1.0") 
	     BULLE_D_AIDE("Rayon interne du cylindre\n"))     
       CHAMP(seuil, L_flottant Edite Sauve DEFAUT("1.0")
	     BULLE_D_AIDE(" Seuil \n ")) 
       CHANGEMENT(calcul_blob_cylindric2)
       DESTRUCTION(detruit_blob)
       CHAMP_VIRTUEL(L_affiche_gl(affiche_blob))
       MENU("Blobs/Primitives/Cylindre")
      )
   } 
