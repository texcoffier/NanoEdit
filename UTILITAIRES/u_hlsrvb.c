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
/*
 * C code for converting
 * between HLS and RGB and back, which came from Graphics Gems, p. 764. 
 */

#include <math.h>
#include "u_base.h"
#include "u_hlsrvb.h"


#define HUE_UNDEFINED (0.5)

CLR_HLS CLR_rgb_to_hls(const CLR_RGB *rgb)
{
    CLR_HLS hls;
    double  max,min,delta;

    max = MAX3(rgb->r, rgb->g, rgb->b);
    min = MIN3(rgb->r, rgb->g, rgb->b);

    /* calculate the lightness */
    hls.l = (max + min)/2.0;
    
    if (max == min)
    {
        /* Achromatic case, r=b=g */
        hls.s = 0.0;
        hls.h = HUE_UNDEFINED;
    } 
    else 
    {
        /* Chromatic case.  First, the saturation */
        if (hls.l <= 0.5)
            hls.s = (max - min)/(max + min);
        else
            hls.s = (max - min)/(2 - max - min);

        /* Next, the hue */
        delta = max - min;
        if (rgb->r == max)
            hls.h = (rgb->g - rgb->b)/delta;
        else if (rgb->g == max)
            hls.h = 2 + (rgb->b - rgb->r)/delta;
        else if (rgb->b == max)
            hls.h = 4 + (rgb->r - rgb->g)/delta;
	else
	  hls.h = 0 ; // To please compiler
        hls.h *= 60.0;
        
        if (hls.h < 0.0)
            hls.h += 360.0;
    }
    hls.h *= M_PI/180 ;

    return hls;
}

CLR_RGB CLR_hls_to_rgb(const CLR_HLS *hls)
{
  CLR_RGB rgb = {0,0,0} ; // Initialized to please compiler
    double value;

    /* the algorithm calls for hue in the range [0,1], while we are
     * using values in the range [0,360], so scale it
     */
    value = (hls->l <= 0.5) ? (hls->l * (1.0 + hls->s)) : 
        (hls->l + hls->s - hls->l * hls->s);

    if (value <= 0)
    {
        rgb.r = rgb.g = rgb.b = 0.0;
    }
    else
    {
        double m;
        double sv;
        int sextant;
        double fract, vsf, mid1, mid2;

        m = hls->l + hls->l - value;
        sv = (value - m)/value;
        sextant = 6*hls->h/M_PI/2 ;
        fract =   6*hls->h/M_PI/2 - sextant;
        vsf = value * sv * fract;
        mid1 = m + vsf;
        mid2 = value - vsf;

        switch (sextant%6)
        {
          case 0: 
            rgb.r = value;
            rgb.g = mid1;
            rgb.b = m;
            break;
          case 1:
            rgb.r = mid2;
            rgb.g = value;
            rgb.b = m;
            break;
          case 2:
            rgb.r = m;
            rgb.g = value;
            rgb.b = mid1;
            break;
          case 3:
            rgb.r = m;
            rgb.g = mid2;
            rgb.b = value;
            break;
          case 4:
            rgb.r = mid1;
            rgb.g = m;
            rgb.b = value;
            break;
          case 5:
            rgb.r = value;
            rgb.g = m;
            rgb.b = mid2;
            break;
        }
    }
    return rgb;
}

