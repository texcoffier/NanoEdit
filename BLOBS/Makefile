# -*- makefile -*-
DIRECTORY_A_AJOUTER = . ./BLOB/
export USER_LIB = BLOB/$(ARCH)/libIs.so BLOB/$(ARCH)/libCore.so -lstdc++
export USER_CFLAGS = -I$(BASE)/BLOBS/BLOB


but:
but_: default_ Nanoedit_ lance_

recupere:
	-mkdir -p BLOB/LINUX386
	cp /home/egalin/Stable/* BLOB
	cd BLOB ; a2sl libIs.a ; a2sl libCore.a ; mv *.so LINUX386


include ../Makefile.commun
include Makefile.dep
