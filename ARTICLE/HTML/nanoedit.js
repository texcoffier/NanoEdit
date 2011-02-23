function eurl(url, texte)
{
if ( url == ici )
   document.write( '<LI CLASS="petit"><BIG><B>'+texte+'</B></BIG></LI>' ) ;
else
   document.write('<LI CLASS="petit"><A HREF="'+url+'">'+texte+'</A></LI>') ;
}


function haut(titre)
{
ici = window.location.pathname.replace('.*/','') ;


document.write('<html><head><title>' + titre + '</title><link href="nanoedit.css" rel="STYLESHEET"></head><body>') ;
document.write('<TABLE WIDTH="100%"><TR><TD WIDTH="25%" VALIGN=TOP>') ;
document.write('<A HREF="../../nanoedit.html">NanoÉdit</A>/<A HREF="http://ligim.univ-lyon1.fr/">LIGIM</A>') ;
document.write('<UL>') ;
eurl('Welcome.html', 'Index') ;
eurl('histoire.html', 'Histoire') ;
eurl('architecture.html', 'Architecture') ;
eurl('avantages.html', 'Avantages') ;
eurl('concepts.html', 'Concepts') ;
	document.write('<UL>') ;
	eurl('classe.html', 'Classe') ;
	eurl('etat_courant.html', 'État courant') ;
	eurl('graphe.html', 'Graphe') ;
	eurl('extraction.html', 'Extraction') ;
	eurl('typage.html', 'Typage') ;
	eurl('repartiteur.html', 'Tâche') ;
	eurl('neutre.html', 'Neutre') ;
	eurl('xdr.html', 'Stockage') ;
	document.write('</UL>') ;
eurl('classes_particulieres.html', 'Classes Particulières') ;
	document.write('<UL>') ;
	eurl('operateur.html', 'Opérateur') ;
	eurl('vue.html', 'Vue') ;
	eurl('interface.html', 'Interface') ;
	eurl('modeleur.html', 'Modeleur') ;
	eurl('racine.html', 'Racine') ;
	document.write('</UL>') ;
eurl('exemples.html', 'Exemples') ;
eurl('futur.html', 'Futur') ;
document.write('</UL>') ;
document.write('<TD CLASS="droite"><h1>&nbsp;' + titre + '&nbsp;</h1>') ;
}


function bas()
{	
document.write('</TD></TR></TABLE>') ;
}



