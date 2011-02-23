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
#ifndef U_HLSRVB_H
#define U_HLSRVB_H

#include "u_geometrie.h"

typedef struct { Flottant r,g,b; }      CLR_RGB;
typedef struct { Flottant h,l,s; }      CLR_HLS;

CLR_HLS CLR_rgb_to_hls(const CLR_RGB *rgb) ;
CLR_RGB CLR_hls_to_rgb(const CLR_HLS *hls) ;

#endif
