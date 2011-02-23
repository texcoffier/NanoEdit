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
#ifndef IODEF
#define IODEF

#include <stdio.h>
/*#include <stdlib.h>*/
/*#include <malloc.h>*/
/* fonction d'entrees sorties */
/*#include "liste.h"*/
#include "matrice.h"
/*#include "transfo.h"*/

/* sortie d'une matrice dans un fichien inventor iv */
void io_inc(char *fichier, matrice *mat);

/* sortie de la matrice dans un fichier */
void io_mat(char *fichier, matrice *mat);
/* 
organisation de fichier :
taille de la matrice\n
(une ligne par element avec pour chaque ligne :)
une suite de nombres flottants separes par un espace (16 nombres forment une matrice homogene)

 */

#endif
