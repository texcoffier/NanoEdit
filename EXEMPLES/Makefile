# -*- makefile -*-
# Recopier ce fichier sous le nom "makefile" dans tous les repertoires
# ou vous comptez stocker des objets.


# Indiquez la liste des répertoires (separe par des blanc) ou vous voulez aller
# chercher les classes (bibliothèques d'objets).
# En cas de conflit de code clavier pour la création, c'est l'ordre
# des répertoires qui soulève l'ambiguïté.
DIRECTORY_A_AJOUTER = .

# Pour ajouter vos propre bibliothèques
# export USER_LIB = -ltoto

# Pour ajouter vos propre Flags de compilation
# export USER_CFLAGS =  -Wno-unused -Wno-format

# Par defaut,
#   - on compile le contenu du directory courant
#   - on fait l'edition des liens
#   - on lance nanoedit.
but:
but_: default_ Nanoedit_ lance_

# Pour avoir l'option -g et la table des symbols, decommentez la ligne suivante
# DEBUGGAGE=oui

include ../Makefile.commun
include Makefile.dep
