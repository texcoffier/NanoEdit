expose.ps:expose.dvi
	. texenv ; dvips -t a4r -f $? >$@

dvi:expose.dvi
	xdvi -paper a3 -geometry 1800x1200-0-0 -s 5 expose.dvi

expose.ps.gz:expose.ps
	gzip -9 <$? >$@

include $(HOME)/TEX/makefile
