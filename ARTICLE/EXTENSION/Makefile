#--------------------------------------------------------
# Les lignes suivantes doivent etre inclues dans tous les
# makefile utilisant TeX et les outils G,G0,RAYTEX,...
#--------------------------------------------------------
# Les suffixes necessaires pour les differents traitements
.SUFFIXES: .dvi .tex .ps .g0 .b .g .a .bib .bbl .fig .sc .ind .idx .aux .pstex_t

# Transformation d'un fichier .tex en .dvi
.tex.dvi:
	if [ -f $*.idx ] ; then cp $*.idx $*.idx2 ; fi
	. texenv ; latex $<
# Transformation d'un fichier .dvi en .ps
.dvi.ps:
	. texenv ; dvips -t a4 -f $< >$*.ps
# Il est possible en executant un fichier .g d'obtenir un
# fichier .a ou un fichier .g0
# On obtient le .a  par : !IMPCSG("machin.a",scene)
# On obtient le .g0 par : !IMPG0("machin.g0",scene,"G0")
.g.g0:
	$<
.g.a:
	$<
# Transformation d'un fichier .a (Description CSG)
# En un fichier .b, on suppose que le fichier
# .a contient le bon nom de fichier .b
.a.b:
	RAYTEX2 i=$< 2>&1 | tee $*.b.t
# Transformation d'un fichier .b en .ps
.b.ps:
	btopgm <$< | pnmtops >$*.ps
	# Mettre "btoppm" pour avoir de la couleur.
# Transformation d'un fichier .g0 en .ps
.g0.ps:
	G0PS $< >$*.ps
# Creation de la bibliographie
.aux.bbl:
	# -if [ -f $(<:.tex=.aux) -a -f $(<:.tex=.blg) ] ;
	-if [ -f $(<:.tex=.aux) ] ; \
		then . texenv ; bibtex $(<:.aux=) ; fi
#Transformation de fichiers fig en postscript ou en latex
.fig.ps:
	fig2dev -L ps $< > $*.ps
.fig.tex:
	fig2dev -L latex $< > $*.tex
#
# Regle compliquee car le ".pstex_t" contient du texte qui doit etre
# transforme en formules TeX. Pour ce faire il faut ajouter
# un script "sed" demandant la transformation.
# Ce script peut contenir par exemple:
# s/toto/$tot^o$/g
# s/titi/$\\alpha$/g
# N'oubliez pas d'anti-slasher les caracteres speciaux.
# N'oubliez pas la ligne avec le /.*/
#
# Il y a une methode beaucoup plus simple faisant intervenir
# une option "-special" mais je ne l'ai pas essaye.
.fig.pstex_t:
	echo "Xbegin{picture}(0,0)Xepsfig{file=$*.pstex}Xend{picture}" | \
		sed 's/X/\\/g' >$*.pstex_t
	fig2dev -L pstex_t $< | \
	if [ -f $*.sed ] ; \
		then sed -f $*.sed ; \
		else cat ; \
		fi >> $*.pstex_t
	fig2dev -L pstex $< > $*.pstex
# Transformation d'un fichier sc
.sc.tex:
	echo "Stblstyle=latex\nT$@\nQ" | sc $< >/dev/null
# TRansformation d'un index bete en index trie.
.idx.ind:
	. texenv ; makeindex $<
# La ligne suivante ne sert qu'a creer le fichier .idx la premiere fois
# Ensuite on n'y touche plus
.tex.idx:
	if [ ! -f $*.idx ] ; then touch $*.idx ; fi
#--------------------------------------------------------
# Les lignes suivantes permettent decrire mon document
#--------------------------------------------------------

par_default:extension.dvi

extension.dvi: ../../dump.tl o_s1.c.tex

../../dump.tl:
	cd ../.. ; $(MAKE) lib Nanoedit dump

o_s1.c.tex:
	. texenv ; \
	for I in ../../EXEMPLES/o_*.c ../../EXEMPLES/t_*.[ch] ../../OBJETS/o_particule.c ; \
	 do \
	 unexpand $$I | \
	 lgrind -lc -i - | \
	 sed -e 's/\$$\\alpha\$$/à/g' >`basename $$I`.tex ; \
	 done

extension.ps.gz:extension.ps
	gzip -9 <$? >$@

###############################################################################

repere.ps:
	echo "\n\
/carre {\n\
  newpath\n\
  -5 0   moveto\n\
  -5 100 lineto\n\
   5 100 lineto\n\
   5 0   lineto\n\
   0 0   lineto\n\
  closepath\n\
  stroke\n\
} def\n\
\n\
/arbre\n\
{\n\
  dup 0 ne\n\
  {\n\
    1 sub dup\n\
    carre\n\
    gsave  0 100 translate  20 rotate .5 .5 scale arbre grestore\n\
    gsave  0 100 translate -40 rotate .7 .7 scale arbre grestore\n\
  }\n\
  {\n\
    add\n\
  }\n\
  ifelse\n\
} def\n\
\n\
200 0 translate\n\
2 2 scale\n\
9 arbre\n\
       " >$@


# Effacement des fichiers inutiles
clean::
	rm -f *.a *.g0 *.b *.dvi *.log *.aux *.bak *.b.t *.toc *.lof *~ \
	      *.idx *.idx2 *.bbl *.blg core mon.out o_*.c.tex \
	      sphere.ps exco.ps makefile.tex *.c.tex \
	stockage2.tex logique.tex logique2.tex
	rm -f `echo *.fig | sed "s/fig/ps/g"`
	rm -f `echo *.gif | sed "s/gif/ps/g"`
	rm -f `echo *.tex | sed "s/tex/ps/g"`

soft_clean_::
	rm *.aux *.log extension.ps

include ../../Makefile.commun
# DO NOT DELETE
