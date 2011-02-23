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
/******************************************/
/* PRODUCTION D'IMAGES ET DE DESCRIPTIONS */
/******************************************/

/* Type de productions */
/* 
   Dispositif de production : Camera 
   - format d'image (pellicule)
   - type de fichier de sortie : 
     image directe (tableau, image.pgm, image.ppm), 
     image calculee (scene.pov, image.tga),
     fichier de points (.pt),
     descriptions (.desc, .tex),
   - graphisme, rendu :
     gamme : nombre -> couleur,
     lampe : eclairage, 
   - matrice de prise de vue : Espace modelisation -> Espace visualisation 
   Parametre exterieur :
     nom de la production => fichiers d'enregistrement :
     <nom>.desc, <nom>.pgm, <nom>.pov, <nom>_g.pgm, <nom>_d.pgm, ...
   Appel general :
     camera = cam(format,type_fichier,prise de vue)
     fichier = clic(nom,camera,rendu,scene)
     production (ensemble de fichiers) = (nom,regie,scene) 
     */

/* Stockage des productions (images, descriptions) */

#define PROD conca(MOD_FRAC,mot(5,"PROD/"))
#define DIR_FILAIRE conca(PROD,mot(8,"FILAIRE/"))
#define DIR_COURBE conca(PROD,mot(7,"COURBE/"))
#define DIR_SURFACE conca(PROD,mot(8,"SURFACE/"))
#define DIR_POV conca(PROD,mot(4,"POV/"))

/* anciennes adresses */
/*
#define DIR_FILAIRE mot(18,"../IMAGES/FILAIRE/")
*/

/*--------------------------*/
/* Operations informatiques */
/*--------------------------*/

MOT deb_prod(MOT ir, char *nom);
void fin_prod(MOT iprod);
FILE *deb_desc(MOT iprod);
void fin_desc(FILE *f);
FILE *deb_tex(MOT iprod);
void fin_tex(FILE *f);

/*------*/
/* Vues */
/*------*/

void vue_pgm(MOT iprod, CAMERA cam, FIGIT fig);
void vue_pov(MOT iprod, CAMERA cam, FIGIT fig, POINT pl);
void vue_tga(MOT iprod, CAMERA cam, FIGIT fig, POINT pl);

/*--------------*/
/* Bibliotheque */
/*--------------*/

/* Productions 2D */
void pgm_attrac_gifs(MOT idfich, float lfen, GAMME gam, GIFS gifs);
void prod_2d(MOT ir, char *nom, IFS ifs);

/* Productions filaires */
void prod_fil(char *nom, SEMIG sem, PC pc, REGLE reg);
/*
void graphI_gifs(MOT idfich, float lfen, GAMME gam, GIFS gifs); 
*/
void pgm_graph_gifs(MOT idfich, float lfen, GAMME gam, GIFS gifs, L_POINT pc, REGLE reg); 
