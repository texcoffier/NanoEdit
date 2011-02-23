DEBUGGAGE=oui
DIRECTORY_A_AJOUTER = `pwd`/BLOBS `pwd`/SYLVAIN3 .
DIRECTORY_A_AJOUTER = .


export USER_LIB = BLOBS/BLOB/$(ARCH)/libIs.so BLOBS/BLOB/$(ARCH)/libCore.so
export USER_LIB =

en_premier:all
all_: verif compile_directory_ Nanoedit lance_

verif:
	@if [ `ls -id         | (read A B ; echo $$A)` != \
              `ls -id $(BASE) | (read A B ; echo $$A)` ] ; \
		then \
		echo "Vous devez modifier la variable BASE" ; \
		echo "dans le fichier Makefile.commun et Makefile.prototype" ;\
		exit 1 ;\
		fi >&2

include Makefile.commun

# DO NOT DELETE
