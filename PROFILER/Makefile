


export USER_CFLAGS=-I../UTILITAIRES
export NE_PAS_INSTRUMENTER=1

include ../Makefile.commun

default:

default_:OBJ/$(ARCH)/prof_dump


OBJ/$(ARCH)/prof_dump:fnccheck.c .prof_dump.c
	mkdir -p OBJ/$(ARCH)
	$(CC) $(CFLAGS) .prof_dump.c fnccheck.c -lm \
	 -o OBJ/$(ARCH)/prof_dump

recupere:
	if [ -d /home/yperret/. ] \
	then \
	cp /home/yperret/funC/fnccheck-devel-1.1.5/src/fnccheck.[ch] . ; \
	cp /home/yperret/funC/fnccheck-devel-1.1.5/src/fncdump.c .prof_dump.c;\
	fi

include Makefile.dep
