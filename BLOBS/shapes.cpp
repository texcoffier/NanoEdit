#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "implicit.h"
#include "blob.h"


AnaBlob* CreateBear()
{   
   AnaBlob *blob=new AnaBlob;
   blob->SetThreshold(0.6525);  
   
   AnalElem *e;
   
   e=new AnalElemSphere(Vector( 0.0,  0.0,-1.00), PolynomialBlend(7.00, 1.00,0)); // body
   blob->SetElem(e);
   
   e=new AnalElemSphere(Vector( 0.0,  0.0, 4.25), PolynomialBlend(3.00, 1.75,0)); // head
   blob->SetElem(e);
   
   e=new AnalElemSphere(Vector(-2.0,  0.0, 5.75), PolynomialBlend(1.75, 0.75,0)); // ears
   blob->SetElem(e);
   
   e=new AnalElemSphere(Vector( 2.0,  0.0, 5.75), PolynomialBlend(1.75, 0.75,0));
   blob->SetElem(e);
   
   e=new AnalElemSphere(Vector( 0.0,-1.75, 4.25), PolynomialBlend(1.00, 1.50,0)); // nose
   blob->SetElem(e);
   
   e=new AnalElemSphere(Vector(-2.0, -3.0,-3.00), PolynomialBlend(2.00, 1.75,0)); // legs
   blob->SetElem(e);

   e=new AnalElemSphere(Vector( 2.0, -3.0,-3.00), PolynomialBlend(2.00, 1.75,0));
   blob->SetElem(e);

   e=new AnalElemSphere(Vector(-3.75, -1.0, 0.00), PolynomialBlend(1.75, 1.5,0)); // arms
   blob->SetElem(e);

   e=new AnalElemSphere(Vector( 3.75, -1.0, 0.00), PolynomialBlend(1.75, 1.5,0));
   blob->SetElem(e);
   
   return blob;   
}

AnaBlob* CreateBird()
{   
   AnaBlob *blob=new AnaBlob;
   blob->SetThreshold(0.6525);  
   
   AnalElem *e;
   e=new AnalElemSphere(Vector(0.0,-1.0,4.0), PolynomialBlend(2.0, 0.75,0));
   blob->SetElem(e);
   e=new AnalElemSphere(Vector(0.0,-0.5,3.0), PolynomialBlend(3.0, 0.55,0)); // head
   blob->SetElem(e);
   
   Vector u[5];
   u[0]=Vector(0.0,0.0,3.0);
   u[1]=Vector(0.0,-1.0,4.25);
   u[2]=Vector(0.0,-3.0,2.75);
   
   e=new AnalElemPoly(u,3,PolynomialBlend(1.0,0.75,0)); // head
   blob->SetElem(e);
   
   // Body
   e=new AnalElemCylinder(Vector(0.0,1.0,0.0),Vector(0.0,3.0,-4.0),PolynomialBlend(3.0, 0.75,0));
   blob->SetElem(e);
   
   e=new AnalElemSphere(Vector(0.0,1.0,0.0),PolynomialBlend(8.0, 0.25,0));
   blob->SetElem(e);

   // Wing
   
   u[0]=Vector(-1.5, 2.0,-1.0);
   u[1]=Vector(-1.5, 2.0, 0.0);
   u[2]=Vector(-3.5, 2.0, 3.0);
   u[3]=Vector(-5.5, 2.0, 3.0);
   u[4]=Vector(-8.5, 2.0,-2.0);
   
   e=new AnalElemPoly(u,5,PolynomialBlend(0.5, 0.75,0));
   blob->SetElem(e);
   
   e=new AnalElemSphere(Vector(-3.00,2.0,-3.25), PolynomialBlend(3.5, -0.5,0));
   blob->SetElem(e);
   e=new AnalElemSphere(Vector(-6.75,2.0,-3.25), PolynomialBlend(3.0, -0.5,0));
   blob->SetElem(e);
   // Wing
   
   u[0]=Vector( 1.5, 2.0,-1.0);
   u[1]=Vector( 1.5, 2.0, 0.0);
   u[2]=Vector( 3.5, 2.0, 3.0);
   u[3]=Vector( 5.5, 2.0, 3.0);
   u[4]=Vector( 8.5, 2.0,-2.0);
   
   e=new AnalElemPoly(u,5,PolynomialBlend(0.5, 0.75,0));
   blob->SetElem(e);
   
   e=new AnalElemSphere(Vector( 3.00,2.0,-3.25), PolynomialBlend(3.5, -0.5,0));
   blob->SetElem(e);
   e=new AnalElemSphere(Vector( 6.75,2.0,-3.25), PolynomialBlend(3.0, -0.5,0));
   blob->SetElem(e);
      
   return blob;   
}

AnaBlob* CreateElk()
{
   Vector u[3];
   
   AnaBlob *blob=new AnaBlob;
   blob->SetThreshold(0.6525);  
   
   AnalElem *e;
   
   e=new AnalElemSphere(Vector( 0.0,-1.0,4.0), PolynomialBlend(2.25, 0.75,0)); // head
   blob->SetElem(e);
   
   e=new AnalElemSphere(Vector( 0.0,-0.5,3.0), PolynomialBlend(3.25, 0.55,0)); // body
   blob->SetElem(e);
   
   u[0]=Vector(0.0,0.0,3.0);
   u[1]=Vector(0.0,-1.0,4.25);
   u[2]=Vector(0.0,-2.5,2.75);   
   e=new AnalElemPoly(u,3,PolynomialBlend(1.5, 2.0,0));
   blob->SetElem(e);
   
   e=new AnalElemSphere(Vector( 0.75,-2.5,2.75), PolynomialBlend(2.0, 0.25,0));
   blob->SetElem(e);
   
   e=new AnalElemSphere(Vector(-0.75,-2.5,2.75), PolynomialBlend(2.0, 0.25,0));
   blob->SetElem(e);
   
   // END OF HEAD
   
   // NECK
   
   e=new AnalElemCylinder(Vector(0.0, 0.0,2.0),Vector(0.0,2.0,-1.0), PolynomialBlend(1.5, 0.75,0));
   blob->SetElem(e);
   
   e=new AnalElemSphere(Vector(0.0,4.5,-1.0), PolynomialBlend(3.0, 0.75,0));
   blob->SetElem(e);
   e=new AnalElemSphere(Vector(0.0,3.5,-1.0), PolynomialBlend(8.0, 0.25,0));
   blob->SetElem(e);
   e=new AnalElemSphere(Vector(0.0,2.5,-1.0), PolynomialBlend(4.0, 0.55,0));
   blob->SetElem(e);
   e=new AnalElemSphere(Vector(0.0,6.5,-1.0), PolynomialBlend(4.0, 0.55,0));
   blob->SetElem(e);
   
   // forward legs
   e=new AnalElemCylinder(Vector( 3.5,2.25,1.5),Vector( 4.5,1.5,-1.5),PolynomialBlend(0.95,0.85,0));
   blob->SetElem(e);
   
   e=new AnalElemCylinder(Vector( 1.0,3.00,-1.0),Vector(3.5,2.25,1.5),PolynomialBlend(0.95,1.0,0));
   blob->SetElem(e);
   
   e=new AnalElemCylinder(Vector(-3.5,2.25,1.5),Vector(-4.5,1.5,-1.5),PolynomialBlend(0.95,0.85,0));
   blob->SetElem(e);

   e=new AnalElemCylinder(Vector(-1.0,3.00,-1.0),Vector(-3.5,2.25,1.5),PolynomialBlend(0.95,1.0,0));
   blob->SetElem(e);
   
   
   // back legs
   
   e=new AnalElemCylinder(Vector( 1.0,5.5,-1.0),Vector( 3.5,5.0,1.5),PolynomialBlend(0.95,1.0,0));
   blob->SetElem(e);
   e=new AnalElemCylinder(Vector( 3.5,5.0,1.5),Vector( 4.75,4.5,-1.5),PolynomialBlend(0.95,0.85,0));
   blob->SetElem(e);
   
   e=new AnalElemCylinder(Vector(-1.0,5.5,-1.0),Vector(-3.5,5.0,1.5),PolynomialBlend(0.95,1.0,0));
   blob->SetElem(e);
   e=new AnalElemCylinder(Vector(-3.5,5.0,1.5),Vector(-4.75,4.5,-1.5),PolynomialBlend(0.95,0.85,0));
   blob->SetElem(e);
   
   // HORNS
   
   e=new AnalElemSphere(Vector(-0.75,-0.95, 4.50), PolynomialBlend(1.00, 0.25,0)); // start
   blob->SetElem(e);
   
   // ramiform +z*.05 inc x2
   e=new AnalElemCylinder(Vector(-0.75,-0.95, 4.50),Vector(-2.35,-0.70, 5.05), PolynomialBlend(0.55, 1.50,0));
   blob->SetElem(e);
   e=new AnalElemCylinder(Vector(-2.35,-0.70, 5.15),Vector(-3.25,-0.50, 6.35), PolynomialBlend(0.50, 1.50,0));
   blob->SetElem(e);
   e=new AnalElemCylinder(Vector(-3.25,-0.50, 6.45),Vector(-3.80,-0.35, 7.75), PolynomialBlend(0.45, 1.50,0));
   blob->SetElem(e);
   
   
   // others level 1 +x*0.05 non inc x2
   e=new AnalElemCylinder(Vector(-0.85,-0.95, 4.50),Vector(-1.60,-0.90, 6.05), PolynomialBlend(0.50, 1.25,0));
   blob->SetElem(e);
   
   // others level 2
   e=new AnalElemCylinder(Vector(-2.45,-0.70, 5.00),Vector(-3.35,-0.60, 4.60), PolynomialBlend(0.45, 1.25,0));
   blob->SetElem(e);
   
   // others level 2
   e=new AnalElemCylinder(Vector(-3.35,-0.50, 6.20),Vector(-4.35,-0.45, 6.10), PolynomialBlend(0.40, 1.25,0));
   blob->SetElem(e);
   
   // symetrie
   
   e=new AnalElemSphere(Vector(0.75,-0.95, 4.50), PolynomialBlend(1.00, 0.25 ,0));
   blob->SetElem(e);
   
   e=new AnalElemCylinder(Vector(0.75,-0.95, 4.50),Vector(2.35,-0.70, 5.05), PolynomialBlend(0.55, 1.50,0));
   blob->SetElem(e);
   e=new AnalElemCylinder(Vector(2.35,-0.70, 5.15),Vector(3.25,-0.50, 6.35), PolynomialBlend(0.50, 1.50,0));
   blob->SetElem(e);
   e=new AnalElemCylinder(Vector(3.25,-0.50, 6.45),Vector(3.80,-0.35, 7.75), PolynomialBlend(0.45, 1.50,0));
   blob->SetElem(e);
   
   e=new AnalElemCylinder(Vector(0.85,-0.95, 4.50),Vector(1.60,-0.90, 6.05), PolynomialBlend(0.50, 1.25,0));
   blob->SetElem(e);
   
   e=new AnalElemCylinder(Vector(2.45,-0.70, 5.00),Vector(3.35,-0.60, 4.60), PolynomialBlend(0.45, 1.25,0));
   blob->SetElem(e);
   
   e=new AnalElemCylinder(Vector(3.35,-0.50, 6.20),Vector(4.35,-0.45, 6.10), PolynomialBlend(0.40, 1.25,0));
   blob->SetElem(e);
   
   return blob;   
}

#include "blobtree.h"

/*
Implicit* CreateShape()
{
  // Create blending functions
  PolynomialBlend *function=new PolynomialBlend(1.5,1.0,1);
  PolynomialBlend *negative=new PolynomialBlend(1.0,-2.0,1);
  PolynomialBlend *offset=new PolynomialBlend(0.5,1.0,1);
  
  // Create a blending group with two cylinders pierced by two line segments
  Set<TreeNode*> set;
  set.Insert(new TreeCylinder(Vector(2.0,0.0,-1.0),Vector(2.0,0.0,2.0),2.0,function));
  set.Insert(new TreeCylinder(Vector(-2.0,0.0,0.0),Vector(-2.0,0.0,1.0),2.0,function));
  set.Insert(new TreeEdge(Vector(2.0,0.0,-2.0),Vector(2.0,0.0,3.0),negative));
  set.Insert(new TreeEdge(Vector(-2.0,0.0,-1.0),Vector(-2.0,0.0,2.0),negative));

  TreeNodeBlend *blendnode=new TreeNodeBlend(set);
  
  // Create a set of two elements, using the previous blending
  // group and inserting a box that will be used to cut the shape  
  Set<TreeNode*> set2;
  set2.Insert(blendnode);
  set2.Insert(new TreeBox(Vector(-5.0,-4.0,0.5),Vector(5.0,0.0,4.0),offset));
  
  TreeNodeDifference *difference=new TreeNodeDifference(set2);

  // Create the blob-tree, with threshold 0.5
  BlobTree *blobtree=new BlobTree(difference,0.5);
  return blobtree;
}
*/

Implicit* CreateShape()
{
  // Create blending functions
  PolynomialBlend *function=new PolynomialBlend(1.5,1.0,1);
  
  // Create a blending group with two cylinders pierced by two line segments
  Set<TreeNode*> set;
  set.Insert(new TreeBox(Vector(-2.0,-2.0,-2.0),Vector(2.0,2.0,2.0),function));
  set.Insert(new TreeBox(Vector(1.0,-3.0,-3.0),Vector(3.0,1.0,3.0),function));

  TreeNode *difference=new TreeNodeDifference(set);
  
  Set<TreeNode*> set2;
  set.Insert(difference);
  set.Insert(new TreeVertex(Vector(-3.0),function));
  TreeNode *blend=new TreeNodeBlend(set2);
  
  // Create the blob-tree, with threshold 0.5
  BlobTree *blobtree=new BlobTree(blend,0.5);
  return blobtree;
}

Implicit* CreateCandle()
{
  Set<TreeNode*> set1;
  set1.Insert(new TreeVertex(Vector(0.0,0.0,7.5), new PolynomialBlend(3.0,0.75,1)));
  set1[0]->Scale(Vector(1.5,1.5,0.75));
  set1.Insert(new TreeVertex(Vector(0.0,0.0,9.0), new PolynomialBlend(2.0,0.75,1)));
  set1[1]->Scale(Vector(2.00,2.00,0.75));

  TreeNodeHyperBlend *hyper_1=new TreeNodeHyperBlend(set1,2.0);

  Set<TreeNode*> set2;
  set2.Insert(new TreeEdge(Vector( 2.5 , 0.0 ,-7.0), Vector(1,0,7), new PolynomialBlend(1.75,1.0,1)));
  set2.Insert(new TreeEdge(Vector(-1.25, 2.16,-7.0), Vector(-0.5,0.87,7), new PolynomialBlend(1.75,1.0,1)));
  set2.Insert(new TreeEdge(Vector(-1.25,-2.16,-7.0), Vector(-0.5,-0.87,7), new PolynomialBlend(1.75,1.0,1)));

  TreeNodeHyperBlend *hyper_2=new TreeNodeHyperBlend(set2,2.0);

  TreeTwist *twist_1=new TreeTwist(hyper_2,Vector(0.0),Vector(0.0,0.0,1.0),Vector(1.0,0.0,0.0),0.5);
  //taper { <0,0,-7>, <0,0,7>,0.85,1.15,<-5.0,-5.0,-8.0>, <5.0,5.0,8.0> quadratic_spline}
 
  // Total
  Set<TreeNode*> set0;
  set0.Insert(hyper_1);
  set0.Insert(twist_1);
  
  set0.Insert(new TreeVertex(Vector(0,0,-7.5), new PolynomialBlend(3.0,1.0,1)));
  set0[2]->Scale(Vector(3.75,3.75,0.80));

  TreeNode *blend_1=new TreeNodeBlend(set0);
  
  blend_1->Scale(Vector(0.85));
  
  Set<TreeNode*> set3;
  set3.Insert(new TreeVertex(Vector(0,0,-2), new PolynomialBlend(4.0, 1.0,1)));
  set3[0]->Scale(Vector(2.5,2.5,0.5));
  set3[0]->Translate(Vector(0,0,10));
  set3.Insert(new TreeVertex(Vector(0,0,0), new PolynomialBlend(5.5, 1.0,1)));
  set3[1]->Translate(Vector(0,0,10));
  set3.Insert(new TreeVertex(Vector(0,0,0), new PolynomialBlend(5.0,-1.0,1)));
  set3[2]->Scale(Vector(0.65, 0.65, 1.00));
  set3[2]->Translate(Vector(0,0,10));

  TreeNodeHyperBlend *hyper_3=new TreeNodeHyperBlend(set3,1.5);
  
  Set<TreeNode*> set4;
  set4.Insert(blend_1);
  set4.Insert(hyper_3);

  TreeNodeBlend *blend_2=new TreeNodeBlend(set4);
  // Create the blob-tree, with threshold 0.5
  BlobTree *blobtree=new BlobTree(blend_2,0.5);
  return blobtree;
}

Implicit* CreateCube()
{   
  Set<TreeNode*> set;  
  set.Insert(new TreeVertex(Vector(0.0,0.0,0.0), new PolynomialBlend(9.0,1.0,1)));
  set.Insert(new TreeBox(Vector(-8.0,-0.5,-0.5), Vector(8.0,0.5,0.5),new PolynomialBlend(2.15,-0.75,1)));
  set.Insert(new TreeBox(Vector(-0.5,-8.0,-0.5), Vector(0.5,8.0,0.5),new PolynomialBlend(2.15,-0.75,1)));
  set.Insert(new TreeBox(Vector(-0.5,-0.5,-8.0), Vector(0.5,0.5,8.0),new PolynomialBlend(2.15,-0.75,1)));

  TreeNodeBlend *blend=new TreeNodeBlend(set);
  // Create the blob-tree, with threshold 0.5
  BlobTree *blobtree=new BlobTree(blend,0.5);
  return blobtree;
}
