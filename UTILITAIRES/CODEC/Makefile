CC=gcc
CFLAGS=-Wall -I.. -g

run:run_


run_:codec_base.c codec.c codec_main
	time codec_main >xxx.html

codec_base.c codec.c:
	ln -s ../$*.c .

OBJS=codec_main.o codec.o codec_base.o u_memoire.o

codec_main:$(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

codec_main.o:liste_codec codec_main.c
	$(CC) $(CFLAGS) -DLISTE=`tr <liste_codec "[\012]" "[,]"` -c $(@:.o=.c)


liste_codec:Makefile .
	pwd
	grep "^CODEC_PROTOTYPE" ../codec*.h | fgrep -v "#" | sed -e 's/^.*(//' -e 's/[  ,)].*//' >$@

soft_clean_::
	rm *.o xxx.html codec_main *~


