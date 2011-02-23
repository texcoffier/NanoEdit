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
/********************************************/
/* POTENTIEL D'ECHAPPEMENT ASSOCIE A UN IFS */
/********************************************/

/* La Fonction Echappement depend d'un IFS et d'un ellipsoide 
   englobant l'attracteur.
   L'ellipsoide est defini comme image de la boule unitaire 
   (centree a l'origine et de rayon 1) par une transformation affine.
   On a : 
     Echap(conj(t,ifs),t*ellip,t*p) = Echap(ifs,ellip,p)
   d'ou : 
     Echap(ifs,te*bu,p) = Echap(conj(te^-1,ifs),bu,te^-1*p) 
   */

/* Calcul des potentiels */
Flottant echap(int m, POINT q);
void deb_pot_gifs(TIMA tima, GIFS gifs);
void fin_pot_gifs();
Flottant val_pot2(int m, Flottant x, Flottant y);
Flottant val_pot3(int m, Flottant x, Flottant y, Flottant z);
/* Visualisation */
void tima_echap_v(TIMA tima, GIFS gifs, int m);
void tima_echap(TIMA tima, Flottant lfen, GIFS gifs, int m);

