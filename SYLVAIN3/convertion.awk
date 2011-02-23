#!/usr/bin/awk -f

# Du format LRIFS txt (/home/et/LOGICIELS/LRIFS/rapport.txt)
# Vers Nano…dit.

BEGIN {
  for(i=0;i<256;i++)
    {
      c = sprintf("%c", i) ;
      C[c] = i ;
    }
  TT["translation"] = "t_origine" ;
  TT["dilatation"] = "vecteur" ;
  TT["rotation"] = "angle" ;
  P = 1000 ;
  PP = 1000 ;
  K=10000000 ;
  IPT = ARGV[1] ;
  sub("\.txt", "", IPT) ;
  system("mkdir MODELES/" toupper(IPT)) ;
  FF = "MODELES/" toupper(IPT) "/" IPT ".ne" ;
  print "classe vue" >FF ;
  print "edite 1" >>FF ;
  print "point_de_vue 4 4 4" >>FF ;
  print "" >>FF ;
#  print "classe cube" >FF ;
#  print "" >>FF ;
#  print "classe sphere" >FF ;
#  print "" >>FF ;
  
}

/^ *transformation *\(/ {
  nom = $0 ;
  sub("^.*\(","",nom) ;
  sub(",.*","",nom) ;

  type = $0 ;
  sub("^.*\(" nom ",", "", type)  ;
  sub(",.*","", type) ;

  valeur = $0 ;
  sub("^.*" type ",", "", valeur)  ;
  sub("\)","", valeur) ;
  gsub(",", " ", valeur) ;
  sub("^ *","", valeur) ;

  sub("homothetie", "dilatation", type) ;

  if ( type != "rotation" )
    {
      TEXTE[nom] = "classe " type "\nnom " C[nom] "\n" TT[type] " " valeur "\n\n" ;
    }
  else
    {
      angle = valeur ;
      rx = angle ;
      sub(" .*", "", rx) ;
      sub("[^ ]* ", "", angle) ;

      ry = angle ;
      sub(" .*", "", ry) ;
      sub("[^ ]* ", "", angle) ;

      rz = angle ;
      sub(" .*", "", rz) ;

      TEXTE[nom] = "classe applique\nnom " C[nom] "\nfils " K " " K+1 " " K+2 "\n\nclasse rotation\nnom " K "\nvecteur 1 0 0\nangle " rx "\n\nclasse rotation\nnom " K+1 "\nvecteur 0 1 0\nangle " ry "\n\nclasse rotation\nnom " K+2 "\nvecteur 0 0 1\nangle " rz "\n\n" ;
      K+=3 ;
      
    }

  # print nom "=" TEXTE[nom] ;
  next ;
}

/^ *matrice / {
  nom = $0 ;
  sub("\[.*", "", nom) ;
  sub("^.*  *", "", nom) ;

  taille = $0 ;
  sub("].*","",taille) ;
  sub(".*\[", "", taille) ;

  transfo = $0 ;
  sub(".*\(","",transfo) ;
  if ( transfo == ")" )
    next ;

  F="MODELES/" toupper(IPT) "/" IPT "." nom ".ne" ;

  print "classe cube" >F ;
  print "nom 1" >>F ;
  print "" >>F ;
  fils = "" ;
  while( transfo != "")
    {
      ifs = transfo ;

      sub("\"[),].*","",ifs) ;
      transfo = substr(transfo, length(ifs)+3) ;
      sub("^\"", "", ifs) ;
      p = P ;
      while( ifs != "" )
	{
	  t = ifs ;
	  sub("\+.*","", t) ;
	  ifs = substr(ifs, length(t)+2) ;

	  for(i=1;i<=length(t);i++)
	    {
	      if ( FAIT[substr(t,i,1)] != nom )
		{
		  FAIT[substr(t,i,1)] = nom ;
		  print TEXTE[substr(t,i,1)] >>F ;
		}
	    }

	  print "classe applique" >>F ;
	  print "nom " P++ >>F ;
	  printf "fils (" >>F ;
	  for(i=1;i<=length(t);i++)
	    {
	      printf " " C[substr(t,i,1)] >>F ;
	    }
	  print ")" >>F ;
	  print "" >>F ;
	}

      print "classe fractale" >>F ;
      print "nom " P >>F ;
      print "taille 1" >>F ;
      printf "fils (1" >>F ;
      for(;p<P;p++)
	printf " " p >>F ;
      print ")" >>F ;
      print "" >>F ;


      fils = fils " " P++ ;

    }

  print "classe fractale" >>F ;
  print "taille " taille  >>F ;
  print "fils (1" fils ")" >>F ;
  print "" >>F ;
  print "classe vue" >>F ;
  print "edite 1" >>F ;
  print "point_de_vue 4 4 4" >>F ;
  print "" >>F ;
  print "classe lampe" >>F ;
  print "position 4 7 10" >>F ;
  print "" >>F ;
  close(F) ;

  print "classe vue" >>FF ;
  print "titre " nom >>FF ;
  print "point_de_vue 4 4 4" >>FF ;
  print "fils " PP >>FF ;
  print "" >>FF ;
  print "classe operateur" >>FF ;
  print "nom " PP++ >>FF ;
  print "oper " IPT "." nom >>FF ;
  print ""  >>FF ;
  
  

}
	  
#		 "+": addition
#		 "*": multiplication
#		 "|": union
#		 "#": melange
#		 "&": concatenation
#		 "-": intersection
#
	  
