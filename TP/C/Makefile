# -*- makefile -*-
# Recopier ce fichier sous le nom "makefile" dans tous les directory
# ou vous comptez stocker des objets


# Indiquez la liste des directories (separe par des blanc) ou vous voulez aller
# chercher les classes (bibliotheques d'objets).
# En cas de conflit de code clavier pour la creation, c'est l'ordre
# des directory qui souleve l'ambiguite.
DIRECTORY_A_AJOUTER = .

# Par defaut,
#   - on compile le contenu du directory courant
#   - on fait l'edition des liens
#   - on lance nanoedit.
but: prepare default Nanoedit lance

R=/^CLASSE[(]/   { sub("[(]","(tp_") ; } \
  /MENU[(]/      { sub("\".*/","\"TP Infographie/") ; } \
  /EVENEMENT[(]/ { next ; } \
  /EXCOIN/   	 {I=1;} \
  I!=1       	 {print $$0;} \
  /EXCOOUT/  	 {I=0;}

prepare:o_ex_rotation.c o_ex_translation.c o_ex_bresenham.c o_ex_plan.c o_ex_decoupage.c o_ex_arbre.c

o_ex_rotation.c:o_x_rotation.c
	awk '$(R)' <$? >$@
o_ex_translation.c:o_x_translation.c
	awk '$(R)' <$? >$@
o_ex_bresenham.c:o_bresenham.c
	awk '$(R)' <$? >$@
o_ex_plan.c:../../OBJETS/o_plan.c
	awk '$(R)' <$? >$@
o_ex_decoupage.c:o_decoupage.c
	awk '$(R)' <$? >$@
o_ex_arbre.c:o_arbre.c
	awk '$(R)' <$? >$@

# DEBUGGAGE=oui

include ../../Makefile.commun
# DO NOT DELETE

o_affiche_octree.o: /home/exco/NE/2.0/COEUR/o_objet.h
o_affiche_octree.o: /home/exco/NE/2.0/UTILITAIRES/u_base.h
o_affiche_octree.o: /home/exco/NE/2.0/UTILITAIRES/eprintf.h
o_affiche_octree.o: /home/exco/NE/2.0/UTILITAIRES/l_liste.h
o_affiche_octree.o: /home/exco/NE/2.0/UTILITAIRES/codec_base.h
o_affiche_octree.o: /home/exco/NE/2.0/UTILITAIRES/u_geometrie.h
o_affiche_octree.o: /home/exco/NE/2.0/UTILITAIRES/codec.h
o_affiche_octree.o: /home/exco/NE/2.0/UTILITAIRES/u_memoire.h
o_affiche_octree.o: /home/exco/NE/2.0/UTILITAIRES/u_widget.h
o_affiche_octree.o: /home/exco/NE/2.0/UTILITAIRES/u_etat.h
o_affiche_octree.o: /home/exco/NE/2.0/TYPES/t_base.h
o_ajoute_pt.o: /home/exco/NE/2.0/COEUR/o_objet.h
o_ajoute_pt.o: /home/exco/NE/2.0/UTILITAIRES/u_base.h
o_ajoute_pt.o: /home/exco/NE/2.0/UTILITAIRES/eprintf.h
o_ajoute_pt.o: /home/exco/NE/2.0/UTILITAIRES/l_liste.h
o_ajoute_pt.o: /home/exco/NE/2.0/UTILITAIRES/codec_base.h
o_ajoute_pt.o: /home/exco/NE/2.0/UTILITAIRES/u_geometrie.h
o_ajoute_pt.o: /home/exco/NE/2.0/UTILITAIRES/codec.h
o_ajoute_pt.o: /home/exco/NE/2.0/UTILITAIRES/u_memoire.h
o_ajoute_pt.o: /home/exco/NE/2.0/UTILITAIRES/u_widget.h
o_ajoute_pt.o: /home/exco/NE/2.0/UTILITAIRES/u_etat.h
o_ajoute_pt.o: /home/exco/NE/2.0/TYPES/t_base.h
o_ajoute_pt.o: /home/exco/NE/2.0/TYPES/t_geometrie.h
o_arbre.o: /home/exco/NE/2.0/COEUR/o_objet.h
o_arbre.o: /home/exco/NE/2.0/UTILITAIRES/u_base.h
o_arbre.o: /home/exco/NE/2.0/UTILITAIRES/eprintf.h
o_arbre.o: /home/exco/NE/2.0/UTILITAIRES/l_liste.h
o_arbre.o: /home/exco/NE/2.0/UTILITAIRES/codec_base.h
o_arbre.o: /home/exco/NE/2.0/UTILITAIRES/u_geometrie.h
o_arbre.o: /home/exco/NE/2.0/UTILITAIRES/codec.h
o_arbre.o: /home/exco/NE/2.0/UTILITAIRES/u_memoire.h
o_arbre.o: /home/exco/NE/2.0/UTILITAIRES/u_widget.h
o_arbre.o: /home/exco/NE/2.0/UTILITAIRES/u_etat.h
o_arbre.o: /home/exco/NE/2.0/TYPES/t_base.h
o_arbre.o: /home/exco/NE/2.0/TYPES/t_geometrie.h
o_arbre.o: /home/exco/NE/2.0/OBJETS/u_vue.h
o_bezier.o: /home/exco/NE/2.0/COEUR/o_objet.h
o_bezier.o: /home/exco/NE/2.0/UTILITAIRES/u_base.h
o_bezier.o: /home/exco/NE/2.0/UTILITAIRES/eprintf.h
o_bezier.o: /home/exco/NE/2.0/UTILITAIRES/l_liste.h
o_bezier.o: /home/exco/NE/2.0/UTILITAIRES/codec_base.h
o_bezier.o: /home/exco/NE/2.0/UTILITAIRES/u_geometrie.h
o_bezier.o: /home/exco/NE/2.0/UTILITAIRES/codec.h
o_bezier.o: /home/exco/NE/2.0/UTILITAIRES/u_memoire.h
o_bezier.o: /home/exco/NE/2.0/UTILITAIRES/u_widget.h
o_bezier.o: /home/exco/NE/2.0/UTILITAIRES/u_etat.h
o_bezier.o: /home/exco/NE/2.0/TYPES/t_base.h
o_bezier.o: /home/exco/NE/2.0/TYPES/t_geometrie.h
o_bresenham.o: /home/exco/NE/2.0/COEUR/o_objet.h
o_bresenham.o: /home/exco/NE/2.0/UTILITAIRES/u_base.h
o_bresenham.o: /home/exco/NE/2.0/UTILITAIRES/eprintf.h
o_bresenham.o: /home/exco/NE/2.0/UTILITAIRES/l_liste.h
o_bresenham.o: /home/exco/NE/2.0/UTILITAIRES/codec_base.h
o_bresenham.o: /home/exco/NE/2.0/UTILITAIRES/u_geometrie.h
o_bresenham.o: /home/exco/NE/2.0/UTILITAIRES/codec.h
o_bresenham.o: /home/exco/NE/2.0/UTILITAIRES/u_memoire.h
o_bresenham.o: /home/exco/NE/2.0/UTILITAIRES/u_widget.h
o_bresenham.o: /home/exco/NE/2.0/UTILITAIRES/u_etat.h
o_bresenham.o: /home/exco/NE/2.0/TYPES/t_base.h
o_bresenham.o: /home/exco/NE/2.0/TYPES/t_geometrie.h
o_convexe.o: /home/exco/NE/2.0/COEUR/o_objet.h
o_convexe.o: /home/exco/NE/2.0/UTILITAIRES/u_base.h
o_convexe.o: /home/exco/NE/2.0/UTILITAIRES/eprintf.h
o_convexe.o: /home/exco/NE/2.0/UTILITAIRES/l_liste.h
o_convexe.o: /home/exco/NE/2.0/UTILITAIRES/codec_base.h
o_convexe.o: /home/exco/NE/2.0/UTILITAIRES/u_geometrie.h
o_convexe.o: /home/exco/NE/2.0/UTILITAIRES/codec.h
o_convexe.o: /home/exco/NE/2.0/UTILITAIRES/u_memoire.h
o_convexe.o: /home/exco/NE/2.0/UTILITAIRES/u_widget.h
o_convexe.o: /home/exco/NE/2.0/UTILITAIRES/u_etat.h
o_convexe.o: /home/exco/NE/2.0/TYPES/t_base.h
o_convexe.o: /home/exco/NE/2.0/TYPES/t_geometrie.h
o_decoupage.o: /home/exco/NE/2.0/COEUR/o_objet.h
o_decoupage.o: /home/exco/NE/2.0/UTILITAIRES/u_base.h
o_decoupage.o: /home/exco/NE/2.0/UTILITAIRES/eprintf.h
o_decoupage.o: /home/exco/NE/2.0/UTILITAIRES/l_liste.h
o_decoupage.o: /home/exco/NE/2.0/UTILITAIRES/codec_base.h
o_decoupage.o: /home/exco/NE/2.0/UTILITAIRES/u_geometrie.h
o_decoupage.o: /home/exco/NE/2.0/UTILITAIRES/codec.h
o_decoupage.o: /home/exco/NE/2.0/UTILITAIRES/u_memoire.h
o_decoupage.o: /home/exco/NE/2.0/UTILITAIRES/u_widget.h
o_decoupage.o: /home/exco/NE/2.0/UTILITAIRES/u_etat.h
o_decoupage.o: /home/exco/NE/2.0/TYPES/t_base.h
o_decoupage.o: /home/exco/NE/2.0/TYPES/t_geometrie.h
o_decoupage.o: /home/exco/NE/2.0/UTILITAIRES/u_graphique.h
o_ex_arbre.o: /home/exco/NE/2.0/COEUR/o_objet.h
o_ex_arbre.o: /home/exco/NE/2.0/UTILITAIRES/u_base.h
o_ex_arbre.o: /home/exco/NE/2.0/UTILITAIRES/eprintf.h
o_ex_arbre.o: /home/exco/NE/2.0/UTILITAIRES/l_liste.h
o_ex_arbre.o: /home/exco/NE/2.0/UTILITAIRES/codec_base.h
o_ex_arbre.o: /home/exco/NE/2.0/UTILITAIRES/u_geometrie.h
o_ex_arbre.o: /home/exco/NE/2.0/UTILITAIRES/codec.h
o_ex_arbre.o: /home/exco/NE/2.0/UTILITAIRES/u_memoire.h
o_ex_arbre.o: /home/exco/NE/2.0/UTILITAIRES/u_widget.h
o_ex_arbre.o: /home/exco/NE/2.0/UTILITAIRES/u_etat.h
o_ex_arbre.o: /home/exco/NE/2.0/TYPES/t_base.h
o_ex_arbre.o: /home/exco/NE/2.0/TYPES/t_geometrie.h
o_ex_arbre.o: /home/exco/NE/2.0/OBJETS/u_vue.h
o_ex_bresenham.o: /home/exco/NE/2.0/COEUR/o_objet.h
o_ex_bresenham.o: /home/exco/NE/2.0/UTILITAIRES/u_base.h
o_ex_bresenham.o: /home/exco/NE/2.0/UTILITAIRES/eprintf.h
o_ex_bresenham.o: /home/exco/NE/2.0/UTILITAIRES/l_liste.h
o_ex_bresenham.o: /home/exco/NE/2.0/UTILITAIRES/codec_base.h
o_ex_bresenham.o: /home/exco/NE/2.0/UTILITAIRES/u_geometrie.h
o_ex_bresenham.o: /home/exco/NE/2.0/UTILITAIRES/codec.h
o_ex_bresenham.o: /home/exco/NE/2.0/UTILITAIRES/u_memoire.h
o_ex_bresenham.o: /home/exco/NE/2.0/UTILITAIRES/u_widget.h
o_ex_bresenham.o: /home/exco/NE/2.0/UTILITAIRES/u_etat.h
o_ex_bresenham.o: /home/exco/NE/2.0/TYPES/t_base.h
o_ex_bresenham.o: /home/exco/NE/2.0/TYPES/t_geometrie.h
o_ex_decoupage.o: /home/exco/NE/2.0/COEUR/o_objet.h
o_ex_decoupage.o: /home/exco/NE/2.0/UTILITAIRES/u_base.h
o_ex_decoupage.o: /home/exco/NE/2.0/UTILITAIRES/eprintf.h
o_ex_decoupage.o: /home/exco/NE/2.0/UTILITAIRES/l_liste.h
o_ex_decoupage.o: /home/exco/NE/2.0/UTILITAIRES/codec_base.h
o_ex_decoupage.o: /home/exco/NE/2.0/UTILITAIRES/u_geometrie.h
o_ex_decoupage.o: /home/exco/NE/2.0/UTILITAIRES/codec.h
o_ex_decoupage.o: /home/exco/NE/2.0/UTILITAIRES/u_memoire.h
o_ex_decoupage.o: /home/exco/NE/2.0/UTILITAIRES/u_widget.h
o_ex_decoupage.o: /home/exco/NE/2.0/UTILITAIRES/u_etat.h
o_ex_decoupage.o: /home/exco/NE/2.0/TYPES/t_base.h
o_ex_decoupage.o: /home/exco/NE/2.0/TYPES/t_geometrie.h
o_ex_decoupage.o: /home/exco/NE/2.0/UTILITAIRES/u_graphique.h
o_ex_plan.o: /home/exco/NE/2.0/COEUR/o_objet.h
o_ex_plan.o: /home/exco/NE/2.0/UTILITAIRES/u_base.h
o_ex_plan.o: /home/exco/NE/2.0/UTILITAIRES/eprintf.h
o_ex_plan.o: /home/exco/NE/2.0/UTILITAIRES/l_liste.h
o_ex_plan.o: /home/exco/NE/2.0/UTILITAIRES/codec_base.h
o_ex_plan.o: /home/exco/NE/2.0/UTILITAIRES/u_geometrie.h
o_ex_plan.o: /home/exco/NE/2.0/UTILITAIRES/codec.h
o_ex_plan.o: /home/exco/NE/2.0/UTILITAIRES/u_memoire.h
o_ex_plan.o: /home/exco/NE/2.0/UTILITAIRES/u_widget.h
o_ex_plan.o: /home/exco/NE/2.0/UTILITAIRES/u_etat.h
o_ex_plan.o: /home/exco/NE/2.0/TYPES/t_base.h
o_ex_plan.o: /home/exco/NE/2.0/OBJETS/u_vue.h
o_ex_plan.o: /home/exco/NE/2.0/OBJETS/u_etat_courant.h
o_ex_rotation.o: /home/exco/NE/2.0/COEUR/o_objet.h
o_ex_rotation.o: /home/exco/NE/2.0/UTILITAIRES/u_base.h
o_ex_rotation.o: /home/exco/NE/2.0/UTILITAIRES/eprintf.h
o_ex_rotation.o: /home/exco/NE/2.0/UTILITAIRES/l_liste.h
o_ex_rotation.o: /home/exco/NE/2.0/UTILITAIRES/codec_base.h
o_ex_rotation.o: /home/exco/NE/2.0/UTILITAIRES/u_geometrie.h
o_ex_rotation.o: /home/exco/NE/2.0/UTILITAIRES/codec.h
o_ex_rotation.o: /home/exco/NE/2.0/UTILITAIRES/u_memoire.h
o_ex_rotation.o: /home/exco/NE/2.0/UTILITAIRES/u_widget.h
o_ex_rotation.o: /home/exco/NE/2.0/UTILITAIRES/u_etat.h
o_ex_rotation.o: /home/exco/NE/2.0/TYPES/t_base.h
o_ex_rotation.o: /home/exco/NE/2.0/TYPES/t_geometrie.h
o_ex_rotation.o: /home/exco/NE/2.0/UTILITAIRES/s_string.h
o_ex_translation.o: /home/exco/NE/2.0/COEUR/o_objet.h
o_ex_translation.o: /home/exco/NE/2.0/UTILITAIRES/u_base.h
o_ex_translation.o: /home/exco/NE/2.0/UTILITAIRES/eprintf.h
o_ex_translation.o: /home/exco/NE/2.0/UTILITAIRES/l_liste.h
o_ex_translation.o: /home/exco/NE/2.0/UTILITAIRES/codec_base.h
o_ex_translation.o: /home/exco/NE/2.0/UTILITAIRES/u_geometrie.h
o_ex_translation.o: /home/exco/NE/2.0/UTILITAIRES/codec.h
o_ex_translation.o: /home/exco/NE/2.0/UTILITAIRES/u_memoire.h
o_ex_translation.o: /home/exco/NE/2.0/UTILITAIRES/u_widget.h
o_ex_translation.o: /home/exco/NE/2.0/UTILITAIRES/u_etat.h
o_ex_translation.o: /home/exco/NE/2.0/TYPES/t_base.h
o_ex_translation.o: /home/exco/NE/2.0/TYPES/t_geometrie.h
o_ex_translation.o: /home/exco/NE/2.0/UTILITAIRES/s_string.h
o_gpao_piece.o: /home/exco/NE/2.0/COEUR/o_objet.h
o_gpao_piece.o: /home/exco/NE/2.0/UTILITAIRES/u_base.h
o_gpao_piece.o: /home/exco/NE/2.0/UTILITAIRES/eprintf.h
o_gpao_piece.o: /home/exco/NE/2.0/UTILITAIRES/l_liste.h
o_gpao_piece.o: /home/exco/NE/2.0/UTILITAIRES/codec_base.h
o_gpao_piece.o: /home/exco/NE/2.0/UTILITAIRES/u_geometrie.h
o_gpao_piece.o: /home/exco/NE/2.0/UTILITAIRES/codec.h
o_gpao_piece.o: /home/exco/NE/2.0/UTILITAIRES/u_memoire.h
o_gpao_piece.o: /home/exco/NE/2.0/UTILITAIRES/u_widget.h
o_gpao_piece.o: /home/exco/NE/2.0/UTILITAIRES/u_etat.h
o_gpao_piece.o: /home/exco/NE/2.0/TYPES/t_base.h
o_gpao_piece.o: /home/exco/NE/2.0/UTILITAIRES/u_graphique.h
o_gpao_reserve.o: /home/exco/NE/2.0/COEUR/o_objet.h
o_gpao_reserve.o: /home/exco/NE/2.0/UTILITAIRES/u_base.h
o_gpao_reserve.o: /home/exco/NE/2.0/UTILITAIRES/eprintf.h
o_gpao_reserve.o: /home/exco/NE/2.0/UTILITAIRES/l_liste.h
o_gpao_reserve.o: /home/exco/NE/2.0/UTILITAIRES/codec_base.h
o_gpao_reserve.o: /home/exco/NE/2.0/UTILITAIRES/u_geometrie.h
o_gpao_reserve.o: /home/exco/NE/2.0/UTILITAIRES/codec.h
o_gpao_reserve.o: /home/exco/NE/2.0/UTILITAIRES/u_memoire.h
o_gpao_reserve.o: /home/exco/NE/2.0/UTILITAIRES/u_widget.h
o_gpao_reserve.o: /home/exco/NE/2.0/UTILITAIRES/u_etat.h
o_gpao_reserve.o: /home/exco/NE/2.0/TYPES/t_base.h
o_gpao_reserve.o: /home/exco/NE/2.0/UTILITAIRES/u_graphique.h
o_gpao_reserve.o: /home/exco/NE/2.0/UTILITAIRES/s_string.h
o_gpao_reserve.o: /home/exco/NE/2.0/TYPES/t_geometrie.h
o_gpao_transporteur.o: /home/exco/NE/2.0/COEUR/o_objet.h
o_gpao_transporteur.o: /home/exco/NE/2.0/UTILITAIRES/u_base.h
o_gpao_transporteur.o: /home/exco/NE/2.0/UTILITAIRES/eprintf.h
o_gpao_transporteur.o: /home/exco/NE/2.0/UTILITAIRES/l_liste.h
o_gpao_transporteur.o: /home/exco/NE/2.0/UTILITAIRES/codec_base.h
o_gpao_transporteur.o: /home/exco/NE/2.0/UTILITAIRES/u_geometrie.h
o_gpao_transporteur.o: /home/exco/NE/2.0/UTILITAIRES/codec.h
o_gpao_transporteur.o: /home/exco/NE/2.0/UTILITAIRES/u_memoire.h
o_gpao_transporteur.o: /home/exco/NE/2.0/UTILITAIRES/u_widget.h
o_gpao_transporteur.o: /home/exco/NE/2.0/UTILITAIRES/u_etat.h
o_gpao_transporteur.o: /home/exco/NE/2.0/TYPES/t_base.h
o_gpao_transporteur.o: /home/exco/NE/2.0/TYPES/t_geometrie.h
o_gpao_transporteur.o: /home/exco/NE/2.0/UTILITAIRES/u_graphique.h
o_inter_bez_seg.o: /home/exco/NE/2.0/COEUR/o_objet.h
o_inter_bez_seg.o: /home/exco/NE/2.0/UTILITAIRES/u_base.h
o_inter_bez_seg.o: /home/exco/NE/2.0/UTILITAIRES/eprintf.h
o_inter_bez_seg.o: /home/exco/NE/2.0/UTILITAIRES/l_liste.h
o_inter_bez_seg.o: /home/exco/NE/2.0/UTILITAIRES/codec_base.h
o_inter_bez_seg.o: /home/exco/NE/2.0/UTILITAIRES/u_geometrie.h
o_inter_bez_seg.o: /home/exco/NE/2.0/UTILITAIRES/codec.h
o_inter_bez_seg.o: /home/exco/NE/2.0/UTILITAIRES/u_memoire.h
o_inter_bez_seg.o: /home/exco/NE/2.0/UTILITAIRES/u_widget.h
o_inter_bez_seg.o: /home/exco/NE/2.0/UTILITAIRES/u_etat.h
o_inter_bez_seg.o: /home/exco/NE/2.0/TYPES/t_base.h
o_inter_bez_seg.o: /home/exco/NE/2.0/TYPES/t_geometrie.h
o_inter_octree.o: /home/exco/NE/2.0/COEUR/o_objet.h
o_inter_octree.o: /home/exco/NE/2.0/UTILITAIRES/u_base.h
o_inter_octree.o: /home/exco/NE/2.0/UTILITAIRES/eprintf.h
o_inter_octree.o: /home/exco/NE/2.0/UTILITAIRES/l_liste.h
o_inter_octree.o: /home/exco/NE/2.0/UTILITAIRES/codec_base.h
o_inter_octree.o: /home/exco/NE/2.0/UTILITAIRES/u_geometrie.h
o_inter_octree.o: /home/exco/NE/2.0/UTILITAIRES/codec.h
o_inter_octree.o: /home/exco/NE/2.0/UTILITAIRES/u_memoire.h
o_inter_octree.o: /home/exco/NE/2.0/UTILITAIRES/u_widget.h
o_inter_octree.o: /home/exco/NE/2.0/UTILITAIRES/u_etat.h
o_inter_octree.o: /home/exco/NE/2.0/TYPES/t_base.h
o_interseg.o: /home/exco/NE/2.0/COEUR/o_objet.h
o_interseg.o: /home/exco/NE/2.0/UTILITAIRES/u_base.h
o_interseg.o: /home/exco/NE/2.0/UTILITAIRES/eprintf.h
o_interseg.o: /home/exco/NE/2.0/UTILITAIRES/l_liste.h
o_interseg.o: /home/exco/NE/2.0/UTILITAIRES/codec_base.h
o_interseg.o: /home/exco/NE/2.0/UTILITAIRES/u_geometrie.h
o_interseg.o: /home/exco/NE/2.0/UTILITAIRES/codec.h
o_interseg.o: /home/exco/NE/2.0/UTILITAIRES/u_memoire.h
o_interseg.o: /home/exco/NE/2.0/UTILITAIRES/u_widget.h
o_interseg.o: /home/exco/NE/2.0/UTILITAIRES/u_etat.h
o_interseg.o: /home/exco/NE/2.0/TYPES/t_base.h
o_interseg.o: /home/exco/NE/2.0/TYPES/t_geometrie.h
o_iso_surf.o: /home/exco/NE/2.0/COEUR/o_objet.h
o_iso_surf.o: /home/exco/NE/2.0/UTILITAIRES/u_base.h
o_iso_surf.o: /home/exco/NE/2.0/UTILITAIRES/eprintf.h
o_iso_surf.o: /home/exco/NE/2.0/UTILITAIRES/l_liste.h
o_iso_surf.o: /home/exco/NE/2.0/UTILITAIRES/codec_base.h
o_iso_surf.o: /home/exco/NE/2.0/UTILITAIRES/u_geometrie.h
o_iso_surf.o: /home/exco/NE/2.0/UTILITAIRES/codec.h
o_iso_surf.o: /home/exco/NE/2.0/UTILITAIRES/u_memoire.h
o_iso_surf.o: /home/exco/NE/2.0/UTILITAIRES/u_widget.h
o_iso_surf.o: /home/exco/NE/2.0/UTILITAIRES/u_etat.h
o_iso_surf.o: /home/exco/NE/2.0/TYPES/t_base.h
o_iso_surf.o: /home/exco/NE/2.0/TYPES/t_geometrie.h
o_iso_surf.o: /home/exco/NE/2.0/UTILITAIRES/u_graphique.h
o_iso_surf.o: /home/exco/NE/2.0/OBJETS/u_etat_courant.h
o_primitives.o: /home/exco/NE/2.0/COEUR/o_objet.h
o_primitives.o: /home/exco/NE/2.0/UTILITAIRES/u_base.h
o_primitives.o: /home/exco/NE/2.0/UTILITAIRES/eprintf.h
o_primitives.o: /home/exco/NE/2.0/UTILITAIRES/l_liste.h
o_primitives.o: /home/exco/NE/2.0/UTILITAIRES/codec_base.h
o_primitives.o: /home/exco/NE/2.0/UTILITAIRES/u_geometrie.h
o_primitives.o: /home/exco/NE/2.0/UTILITAIRES/codec.h
o_primitives.o: /home/exco/NE/2.0/UTILITAIRES/u_memoire.h
o_primitives.o: /home/exco/NE/2.0/UTILITAIRES/u_widget.h
o_primitives.o: /home/exco/NE/2.0/UTILITAIRES/u_etat.h
o_primitives.o: /home/exco/NE/2.0/TYPES/t_base.h
o_primitives.o: /home/exco/NE/2.0/OBJETS/u_etat_courant.h
o_triangulation.o: /home/exco/NE/2.0/COEUR/o_objet.h
o_triangulation.o: /home/exco/NE/2.0/UTILITAIRES/u_base.h
o_triangulation.o: /home/exco/NE/2.0/UTILITAIRES/eprintf.h
o_triangulation.o: /home/exco/NE/2.0/UTILITAIRES/l_liste.h
o_triangulation.o: /home/exco/NE/2.0/UTILITAIRES/codec_base.h
o_triangulation.o: /home/exco/NE/2.0/UTILITAIRES/u_geometrie.h
o_triangulation.o: /home/exco/NE/2.0/UTILITAIRES/codec.h
o_triangulation.o: /home/exco/NE/2.0/UTILITAIRES/u_memoire.h
o_triangulation.o: /home/exco/NE/2.0/UTILITAIRES/u_widget.h
o_triangulation.o: /home/exco/NE/2.0/UTILITAIRES/u_etat.h
o_triangulation.o: /home/exco/NE/2.0/TYPES/t_base.h
o_triangulation.o: /home/exco/NE/2.0/TYPES/t_geometrie.h
o_x_rotation.o: /home/exco/NE/2.0/COEUR/o_objet.h
o_x_rotation.o: /home/exco/NE/2.0/UTILITAIRES/u_base.h
o_x_rotation.o: /home/exco/NE/2.0/UTILITAIRES/eprintf.h
o_x_rotation.o: /home/exco/NE/2.0/UTILITAIRES/l_liste.h
o_x_rotation.o: /home/exco/NE/2.0/UTILITAIRES/codec_base.h
o_x_rotation.o: /home/exco/NE/2.0/UTILITAIRES/u_geometrie.h
o_x_rotation.o: /home/exco/NE/2.0/UTILITAIRES/codec.h
o_x_rotation.o: /home/exco/NE/2.0/UTILITAIRES/u_memoire.h
o_x_rotation.o: /home/exco/NE/2.0/UTILITAIRES/u_widget.h
o_x_rotation.o: /home/exco/NE/2.0/UTILITAIRES/u_etat.h
o_x_rotation.o: /home/exco/NE/2.0/TYPES/t_base.h
o_x_rotation.o: /home/exco/NE/2.0/TYPES/t_geometrie.h
o_x_rotation.o: /home/exco/NE/2.0/UTILITAIRES/s_string.h
o_x_translation.o: /home/exco/NE/2.0/COEUR/o_objet.h
o_x_translation.o: /home/exco/NE/2.0/UTILITAIRES/u_base.h
o_x_translation.o: /home/exco/NE/2.0/UTILITAIRES/eprintf.h
o_x_translation.o: /home/exco/NE/2.0/UTILITAIRES/l_liste.h
o_x_translation.o: /home/exco/NE/2.0/UTILITAIRES/codec_base.h
o_x_translation.o: /home/exco/NE/2.0/UTILITAIRES/u_geometrie.h
o_x_translation.o: /home/exco/NE/2.0/UTILITAIRES/codec.h
o_x_translation.o: /home/exco/NE/2.0/UTILITAIRES/u_memoire.h
o_x_translation.o: /home/exco/NE/2.0/UTILITAIRES/u_widget.h
o_x_translation.o: /home/exco/NE/2.0/UTILITAIRES/u_etat.h
o_x_translation.o: /home/exco/NE/2.0/TYPES/t_base.h
o_x_translation.o: /home/exco/NE/2.0/TYPES/t_geometrie.h
o_x_translation.o: /home/exco/NE/2.0/UTILITAIRES/s_string.h
