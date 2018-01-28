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
include Makefile.dep
