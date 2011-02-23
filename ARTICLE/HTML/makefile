architecture.gif:architecture.fig makefile
	fig2dev -L ppm -m 2 <$(@:.gif=.fig) | pnmscale .5 | ppmquant 256 | ppmtogif -transparent "#FFF" >$@