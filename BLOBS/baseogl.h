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
#ifndef __BASE__
#define __BASE__

#include "list.h"
#include "triangle.h"

#define Vertex3D(P) glVertex3d(P[0],P[1],P[2]) 
#define Normal3D(P) glNormal3d(P[0],P[1],P[2])

void initOgl(int argc, char** argv);
void draw(const List<Triangle>&);
void draw(const List<SmoothTriangle>&);

void Smooth(const List<Triangle>&,List<SmoothTriangle>&);

void AddColorTriangle(const Triangle&,const Vector&);
#endif
