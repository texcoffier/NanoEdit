/*
    NanoÉdit: Un modeleur algébrique interactif.
    Copyright (C) 1996-2004 Thierry EXCOFFIER, Université Claude Bernard, LIRIS

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    Contact: Thierry.EXCOFFIER@liris.univ-lyon1.fr
*/
/*fonction de creation automatique d'objet en cas
de chargement du modele.
Indicateur de non sauvegarde.*/


#ifndef O_OBJET_H
#define O_OBJET_H

#include <stdlib.h>

struct champ;
struct type_editeur;
struct type_logique;
struct classe;
struct objet;
struct interface_widget;

typedef struct champ Champ;
typedef struct type_editeur Type_editeur;
typedef struct type_logique Type_logique;
typedef struct classe Classe;
typedef struct objet Objet;
typedef struct interface_widget Interface_widget;


/*
 *
 */

#include "u_base.h"		/* Pour le type Booleen */
#include "l_liste.h"		/* Pour la macro POUR */
#include "u_geometrie.h"
#include "codec.h"
#include "u_memoire.h"
#include "u_widget.h"

/*
 * **************************************************************************
 * **************************************************************************
 * Types de base et Enum
 * **************************************************************************
 * **************************************************************************
 */
typedef unsigned long long Bits;
/*
 * Pour les champs.
 * On n'utilise pas des champs de bits car l'initialisation
 * statique est plus lourde et moins lisible
 */
enum options
{
  O_Cree = 0x000001,		/* Le Widget sera créé */
  O_Affiche = 0x000002,		/* Le Widget sera visible (--> Cree) */
  O_Edite = 0x000004,		/* Le Widget sera éditable (--> Affiche) */
  O_Label = 0x000008,		/* Un Widget label sera créé (<-- Aff/Titr) */
  O_Modifiable = 0x000010,	/* Le champ est modifiable */

  O_Sauve = 0x000100,		/* On doit sauvegarder ce champ */
  O_Extrait = 0x000200,		/* Ce champ extrait les info. d'un objet */
  O_Obligatoire = 0x000400,	/* A la creation : Extrait obligatoirement */

  O_InitChaine = 0x001000,	/* ------------- : Initialisation chaine */
  O_Init3d = 0x002000,		/* ------------- : Initialisation 3d */

  O_Initialiser = 0x020000,	/* Initialiser le champ à la création */
  O_Virtuel = 0x040000,		/* Pas de bit de changement */

  O_BoiteHomogene = 0x100000,
  O_NePasTester   = 0x200000
};
/*
 * Les mot-clef du langage de description de classe
 */
enum mot_clef
{
  /*
   * Les champs de la structure classe, sauf ceux calculés
   */
  MC_nom,
  MC_Cstruct,
  MC_surclasse,
  MC_taille_structure,
  MC_destruction,
  MC_changement,
  MC_informations,
  MC_destruction_nom,
  MC_changement_nom,
  MC_informations_nom,
  MC_affiche_champs_classe,
  MC_paquet,
  MC_decalage,
  MC_evenement,
  MC_menu,
  MC_fichier,
  MC_ligne,
  /*
   * Classe et champ
   */
  MC_bulle_d_aide,
  /*
   * Les champs de la structure champ
   */
  MC_label,
  MC_herite_pas_des_champs,
  MC_nom_champ,
  MC_Cnom_champ,
  MC_option,
  MC_non_option,
  MC_position,
  MC_taille,
  MC_defaut,
  MC_type_dans_fils,
  MC_numero_fils,
  MC_tl,
  MC_classe_parametre,
  MC_tp,
  MC_tp_ecrit,
  MC_tp_ecrit_nom,
  MC_tp_lit,
  MC_tp_lit_nom,
  MC_affichage,
  MC_te,
  MC_te_texte,
  MC_te_hauteur,
  MC_te_largeur,
  MC_te_ecrit,
  MC_te_ecrit_nom,
  MC_te_lit,
  MC_te_lit_nom,
  MC_te_entree,
  MC_te_entree_nom,
  MC_te_expose,			/* Obsolete */
  MC_te_expose_nom,		/* Obsolete */
  MC_te_mouvement,
  MC_te_mouvement_nom,
  MC_te_clavier,
  MC_te_clavier_nom,
  MC_te_aide,
  MC_te_aide_nom,
  /*
   * Des commandes SANS ARGUMENT car u_definition_classe()
   */
  MC_debut_champ,
  MC_fin_champ,
  MC_fin_definition
};

typedef long Mot_clef;
/*
 * **************************************************************************
 * **************************************************************************
 * Structures
 * **************************************************************************
 * **************************************************************************
 */
struct classe
{
  const char *nom;		/* Nom de la classe */
  const char *Cstruct;		/* Nom du type C */
  const char *surclasse;	/* Nom de la surclasse */
  int taille_structure;		/* Taille d'une instance */
  void (*destruction) (void *);	/* Destruction d'une instance */
  const char *destruction_nom;
  void (*changement) (void *);	/* L'instance a ete modifiee... */
  const char *changement_nom;
  const char *(*informations) (const void *);	/* Infos sur l'objet */
  const char *informations_nom;
  Liste *champs;		/* Les champs de la classe */
  Booleen affiche_champs_classe;	/* Vrai pour les objets graphiq */
  int paquet;			/* Taille paquet a sauter */
  int decalage;			/* Decalage pour le paq. suiv. */
  const Mot_clef *definition;	/* Définition de la classe */
  int index;			/* Dans le tableau de structure */
  const char *evenement;	/* Evenement de creation d'inst. */
  const char *menu;		/* Position dans le menu */
  const char *bulle_d_aide;
  const char *fichier;
  int ligne;
  int id ; /* Pour list GL */
  /*
   * Modifié durant l'exécution
   */
  Liste *fenetres_unmappees;	/* Les fenetres unmappees */
  int distance;			/* Adéquation liste des actifs */
  const Classe *sc;		/* La surclasse ou NULL */
};
/*
 *
 */
struct objet
{
  Bits change;			/* Changement dans les champs */
  Bits invalide;		/* Indique les champs invalides de l'objet */
  const Classe *classe;		/* Informations sur ce type d'objet */
  Liste *parent;		/* Objets contenant celui-ci (ou NULL) */
  Liste *fils;			/* Objets composant celui-ci (ou NULL) */
  Objet *nom;			/* Nom de l'objet pour récuperation/sauve */
  int actif;			/* Mise en évidence de l'objet, Num. ordre */
  NanoWidget edite;		/* Fenêtre d'édition textuelle de l'objet */

  Booleen trace;		/* Trace ce que fait l'objet */
  Booleen trace2;		/* Trace ce que fait le système sur l'objet */
  Booleen ferme;		/* On affiche pas le sous-arbre du graphe */
  Booleen visible;		/* Si vrai, son père l'affiche */
  unsigned int changement:1;	/* Fin de récursion */
  unsigned int flag:1;		/* Fin récursion pour la fonction "parcour" */
  unsigned int flag2:1;		/* Fin récursion pour fonctions utilisateur */
  unsigned int flag3:1;		/* Fin récursion pour gtk widget */
  unsigned int flag4:1;		/* Fin récursion pour coherence graphe */
  unsigned int flag5:1;		/* Fin récursion pour duplication graphe */
  unsigned int urgent:1;
  unsigned int creation:1;	/* L'objet viens d'être créé */
};
/*
 *
 */
struct type_logique
{
  const Type_logique *(*initialisation) (Champ *);	/* En premier */
  const char *nom;		/* Nom du type logique */
  const Type_logique *surclasse;
  /*
   * La fonction suivante retourne vrai si la valeur du champ de l'objet
   * vérifie certaine propriété.
   */
    Booleen (*predicat_verifie) (const void *o, const Champ * c);
  const char *predicat_verifie_nom;
  /*
   * Cette fonction retourne vrai si le champ peut être considéré
   * de type logique 't'.
   * Ceci permet de faire toutes les opérations possible sur
   * les classes de type logique (union, intersection, ...)
   *
   * Dans certain cas, l'objet et le champ sont changés : Variable, Opérateur
   * En effet, pour le sous classé on peut changer l'objet et le champ.
   */
    Booleen (*sous_classe) (const void **o,
			    const Champ ** c, const Type_logique * t);
  const char *sous_classe_nom;
  const char *bulle_d_aide;
};
/*
 *
 */

struct interface_widget
{
  void *objet;			/* L'objet recevant l'événement */
  const Champ *ch;		/* Le champ (Widget) recevant l'événement */
  NanoWidget w;			/* Le Widget recevant l'événement */
  NanoEvent e ;			/* Evenement declencheur */
  int b;			/* 0: Bouton gauche, 1: milieu, 2: droite */
  int x, y;			/* Position du curseur */
  int x_root, y_root;		/* Position du curseur */
  int etat;			/* 1=shift 2=ctrl 4=alt */
  char touche[30];		/* Nom de la touche appuyée */
  char toucheminmaj[30];       /* Nom de la touche appuyée en gardant son etat( minuscule ou majuscule). Added by Houssam on 12 November 2012*/
  int configure_x, configure_y, configure_l, configure_h;
  void *objet_contextuel ;
  int double_clique ;

  const char *texte;		/* Texte (touche clavier, ...) RECODÉE */
};

struct type_editeur
{
  const Type_editeur *(*initialisation) (Champ *);	/* En premier */
  const char *bulle_d_aide;
  const char *nom;
  const Type_editeur *surclasse;
  /*

   */
    NanoWidget (*creer) (const Interface_widget *);
  const char *creer_nom;
  /*
   * Récupération de la valeur courante du Widget
   */
  const char *(*lit) (const Interface_widget *);
  const char *lit_nom;
  /*
   * Changement de la valeur courante du Widget
   */
  void (*ecrit) (const Interface_widget *);
  const char *ecrit_nom;
  /*
   * Réinitialisation du Widget si celui-ci a été UnMappé
   */
  void (*init) (const Interface_widget *);
  const char *init_nom;
  /*
   * Entrée souris, return 1 si on veut traquer le mouvement
   */
    Booleen (*entree) (const Interface_widget *);
  const char *entree_nom;
  /*
   * Mouvement de la souris
   */
  void (*mouvement) (const Interface_widget *);
  const char *mouvement_nom;
  /*
   * Entrée souris, retourne l'aide en fonction de la position x,y
   */
  const char *(*aide) (const Interface_widget *);
  const char *aide_nom;
  /*
   * Entrée clavier
   */
  void (*clavier) (const Interface_widget *);
  const char *clavier_nom;
  /*
   * Affichage
   */
  void (*expose) (const Interface_widget *);	/* Obsolete */
  const char *expose_nom;	/* Obsolete */
  /*            

   */
  void (*start_classe) (const Interface_widget *);
  const char *start_classe_nom;
  void (*stop_classe) (const Interface_widget *);
  const char *stop_classe_nom;
};

typedef enum
{ Toujours_different = 1,	/* Ne pas faire de liste car trop gros
				   ou toujours différent */
  Toujours_identique = 2,	/* Toutes les instances s'affichent
				   de mainère identique */
  Different_si_actif = 4,
  Different_suivant_qualite = 8,
  Different_suivant_type_affichage = 16
}
Affichage;


struct champ
{
  Classe *classe;		/* Classe dont le champ fait partie */
  int index;

  const char *nom_champ;	/* Nom de widget */
  const char *Cnom_champ;	/* Nom dans la structure C */
  const char *label;		/* Le label à mettre à gauche */
  int options;			/* Edite/Affiche/Sauve/... */
  int position;			/* Position du champ dans la structure */
  int taille;			/* Taille du champs */
  const char *defaut;		/* Valeur par defaut du champ */
  const Type_logique *type_dans_fils;	/* Type du champ extrait */
  int numero_fils;		/* Indice dans la liste des composants */
  const char *classe_parametre;	/* Si non NULL, on veut extraire un objet */
  const char *bulle_d_aide;
  const char *te_texte;		/* Pour le widget menu */
  Affichage affichage;		/* Options d'affichage GL pour les listes */
  int te_hauteur;
  int te_largeur;
  /*
   * Les fonctions suivantes ne sont pas celle de l'éditeur
   * Widget, elles n'ont pas les même paramètre,
   * C'est en faite le ``hook''
   */
  void (*tp_ecrit) (void *, const Champ *, const char *v);
  const char *(*tp_lit) (void *, const Champ *);
  void (*te_ecrit) (const Interface_widget *);
  const char *(*te_lit) (const Interface_widget *);
    Booleen (*te_entree) (const Interface_widget *);
  void (*te_clavier) (const Interface_widget *);
  void (*te_mouvement) (const Interface_widget *);
  void (*te_expose) (const Interface_widget *);	/* Obsolete */
  const char *(*te_aide) (const Interface_widget *);
  const char *tp_ecrit_nom;
  const char *tp_lit_nom;
  const char *te_ecrit_nom;
  const char *te_lit_nom;
  const char *te_entree_nom;
  const char *te_clavier_nom;
  const char *te_mouvement_nom;
  const char *te_expose_nom;	/* Obsolete */
  const char *te_aide_nom;
  /*
   * Les classes
   */
  const Type_logique *tl;
  const Type_editeur *te;
  const Codec_informations *tp;

  /*
   * Les parametres variable des classes modifiable dynamiquement.
   * Ce n'est pas propre car normalement, un Champ est une constante.
   */
  int parametre;		/* Utiliser comme numero de "tag" (LignBris) */
};
/*
 * **************************************************************************
 * **************************************************************************
 * Variables globales
 * **************************************************************************
 * **************************************************************************
 */
/*
 * Cette variable contient les champs commun à tous les objets.
 * Elle est utilisée par la macro POUR_LES_CHAMPS
 */
extern Liste *champs_objet;
/*
 * Elle est utilisée par la macro POUR_LES_CLASSES
 */
extern Classe *les_classes;
extern int nombre_de_classes;
/*
 * Pour la génération de la table de description de classe.
 * Ceci réduit considérablement la taille des ".o"
 */
extern int la_taille_de_la_table_globale;
extern int la_position_dans_la_table_globale;
extern Mot_clef *la_table_globale;
/*
 * Pour la verification de l'allocation memoire
 */
extern Gestion_memoire globale_gm_objet;
/*
 *
 */
extern char *directories_objets;
extern int globale_debug;
extern void *globale_interface;	/* Pointeur sur la racine */
/*
 * **************************************************************************
 * **************************************************************************
 * DEFINES
 * **************************************************************************
 * **************************************************************************
 */
/*
 * Le plus important : le cast
 */
/* graphe_coherent(o), \  */

#define OBJET_OU_NULL(o)				\
(							\
 MEMOIRE_VALIDE(globale_gm_objet, (Objet*)o - 1)	\
 ? ((Objet*)o - 1)					\
 : NULL )

#define OBJET(o)					\
(							\
 MEMOIRE_VALIDE(globale_gm_objet, (Objet*)o - 1)	\
 ?							\
 ((Objet*)o - 1)					\
 : ( eprintf("%s:%d\n", __FILE__, __LINE__),		\
     abort(),						\
     (Objet*)NULL					\
     )                                                  \
)

/*
 * Noms de champs très particuliers testés par différentes fonctions
 */
#define CHAMP_ACTIF 	   "actif"
#define CHAMP_FERME    	   "annule_ferme"
#define CHAMP_NUMERO_ACTIF "numero_actif"
#define LABEL_             "label_"
#define NB_ACTIFS     -1	/* Pour le champ : paquet */
#define NB_NON_DEFINI -2	/* Pour les champs : paquet et decalage */
#define NOM_FICHIER_SAUVEGARDE "NanoEdit.ne"
#define FICHIER_CONTENANT_LE_NOM_DU_MODELE "nom_modele"
#define DEBUT_TAG_DEPLACABLE ((int)0x40000000)
#define TAG_DEPLACABLE(X)   (((X)&DEBUT_TAG_DEPLACABLE)==0)

#define EST_OPERATEUR(O) ( strcmp(OBJET(O)->classe->nom, "operateur") == 0 )

/*
 * Ce qui concerne le débuggage
 */
#define INFO_(O) eprintf( "%17s:%-4d %s ",				  \
			  __FILE__,__LINE__,				  \
			  (O)->classe ? (O)->classe->nom : "Classe???")
#define INFO(O) INFO_(OBJET(O))

#define TRACE__(O,T) ((O)->T ? (INFO_(O), Vrai) : Faux )

#define TRACE_(O)  TRACE__(O, trace)
#define TRACE(O)   TRACE_(OBJET(O))

#define TRACE2_(O) TRACE__(O, trace2)
#define TRACE2(O)  TRACE2_(OBJET(O))
/*
 * **************************************************************************
 * Gestion des options
 * **************************************************************************
 */
/*
 * Pour les options, met ensemble les valeur courantes
 */
#define Cree          OPTION(O_Cree         )
#define Affiche       OPTION(O_Affiche      )
#define Edite         OPTION(O_Edite        )
#define Modifiable    OPTION(O_Modifiable   )
#define Label         OPTION(O_Label        )
#define Extrait       OPTION(O_Extrait      )
#define Sauve         OPTION(O_Sauve        )
#define Obligatoire   OPTION(O_Obligatoire  )
#define InitChaine    OPTION(O_InitChaine   )
#define Init3d        OPTION(O_Init3d       )
#define Initialiser   OPTION(O_Initialiser  )
#define Virtuel       OPTION(O_Virtuel      )
#define BoiteHomogene OPTION(O_BoiteHomogene)
#define NePasTester   OPTION(O_NePasTester  )

#define NonAffiche       NON_OPTION(O_Affiche|O_Edite       )
#define NonCree          NON_OPTION(O_Cree|O_Edite|O_Affiche)
#define NonExtrait       NON_OPTION(O_Extrait               )
#define NonModifiable    NON_OPTION(O_Modifiable            )
#define NonEdite         NON_OPTION(O_Edite                 )
#define NonLabel         NON_OPTION(O_Label                 )
#define NonSauve         NON_OPTION(O_Sauve                 )
#define NonObligatoire   NON_OPTION(O_Obligatoire           )
#define NonInitChaine    NON_OPTION(O_InitChaine            )
#define NonInit3d        NON_OPTION(O_Init3d                )
#define NonVirtuel       NON_OPTION(O_Virtuel               )
#define NonInitialiser   NON_OPTION(O_Initialiser           )
#define NonBoiteHomogene NON_OPTION(O_BoiteHomogene         )


#define        A_LABEL(C) ( (C)->options & O_Label                    )
#define    OBLIGATOIRE(C) ( (C)->options & O_Obligatoire              )
#define           CREE(C) ( (C)->options & (O_Cree|O_Edite|O_Affiche) )
#define        EXTRAIT(C) ( (C)->options & O_Extrait                  )
#define          EDITE(C) ( (C)->options & O_Edite                    )
#define     MODIFIABLE(C) ( (C)->options & (O_Edite|O_Modifiable)     )
#define        AFFICHE(C) ( (C)->options & (O_Affiche|O_Edite)        )
#define          SAUVE(C) ( (C)->options & O_Sauve                    )
#define    INIT_CHAINE(C) ( (C)->options & O_InitChaine               )
#define        INIT_3D(C) ( (C)->options & O_Init3d                   )
#define    INITIALISER(C) ( (C)->options & O_Initialiser              )
#define        VIRTUEL(C) ( (C)->options & O_Virtuel                  )
#define BOITE_HOMOGENE(C) ( (C)->options & O_BoiteHomogene            )
#define  NE_PAS_TESTER(C) ( (C)->options & O_NePasTester              )

extern Bits bit_dernier_champ_objet;
#define CHAMP_OBJET(C)  ( BIT_CHAMP(C) <= bit_dernier_champ_objet )

/*
 * **************************************************************************
 * **************************************************************************
 * Gestion des bit de changement
 * **************************************************************************
 * **************************************************************************
 */

/*
 * Retourne le bit correspondant au champ.
 * Cela limite évidemment le nombre de champs....
 * En tenant compte des bits réservés, il reste 29 champs
 */
#define BIT_CHAMP(C)          ( ((Bits)1) << ((C)->index) )
#define BIT_FILS_CHANGE       ( ((Bits)1) << (sizeof(Bits)*8-1) )
#define BIT_DESCENDANT_CHANGE ( ((Bits)1) << (sizeof(Bits)*8-2) )
#define BIT_CA_CHANGE         ( ((Bits)1) << (sizeof(Bits)*8-3) )
/*
 * Fonctions générales sur l'objet
 */
#define          FILS_CHANGE_(O) ((O)->change & BIT_FILS_CHANGE    )
#define    DESCENDANT_CHANGE_(O) ((O)->change & BIT_DESCENDANT_CHANGE    )
#define            CA_CHANGE_(O) ((O)->change & BIT_CA_CHANGE    )
#define      UN_CHAMP_CHANGE_(O) ((O)->change & ~(BIT_FILS_CHANGE|BIT_DESCENDANT_CHANGE)   )
#define UN_VRAI_CHAMP_CHANGE_(O) ((O)->change & ~(BIT_FILS_CHANGE|BIT_DESCENDANT_CHANGE|BIT_CA_CHANGE)   )
#define    UN_CHAMP_INVALIDE_(O) ((O)->invalide                        )
#define     NOTE_FILS_CHANGE_(O) ((O)->change |= BIT_FILS_CHANGE|BIT_DESCENDANT_CHANGE )
#define NOTE_DESCENDANT_CHANGE_(O) ((O)->change |= BIT_DESCENDANT_CHANGE )
#define NOTE_FILS_CHANGE_PAS_(O) ((O)->change &= ~BIT_FILS_CHANGE)
#define  NOTE_RIEN_NE_CHANGE_(O) ((O)->change = 0                )
#define                ACTIF_(O) ((O)->actif                           )
#define    UN_CHAMP_INVALIDE_(O) ((O)->invalide                        )
#define NOTE_UN_CHAMP_CHANGE_(O) ((O)->change |= BIT_CA_CHANGE   )
#define             CREATION_(O) ((O)->creation                       )

#define            FILS_CHANGE(O)            FILS_CHANGE_(OBJET(O))
#define      DESCENDANT_CHANGE(O)      DESCENDANT_CHANGE_(OBJET(O))
#define              CA_CHANGE(O)              CA_CHANGE_(OBJET(O))
#define        UN_CHAMP_CHANGE(O)        UN_CHAMP_CHANGE_(OBJET(O))
#define   UN_VRAI_CHAMP_CHANGE(O)   UN_VRAI_CHAMP_CHANGE_(OBJET(O))
#define       NOTE_FILS_CHANGE(O)       NOTE_FILS_CHANGE_(OBJET(O))
#define NOTE_DESCENDANT_CHANGE(O) NOTE_DESCENDANT_CHANGE_(OBJET(O))
#define   NOTE_FILS_CHANGE_PAS(O)   NOTE_FILS_CHANGE_PAS_(OBJET(O))
#define    NOTE_RIEN_NE_CHANGE(O)    NOTE_RIEN_NE_CHANGE_(OBJET(O))
#define                  ACTIF(O)                  ACTIF_(OBJET(O))
#define      UN_CHAMP_INVALIDE(O)      UN_CHAMP_INVALIDE_(OBJET(O))
#define   NOTE_UN_CHAMP_CHANGE(O)   NOTE_UN_CHAMP_CHANGE_(OBJET(O))
#define               CREATION(O)               CREATION_(OBJET(O))

/*
 * Cas des macros pour lequel ont veut tester un champ particulier
 * en donnant le nom du champ DANS LA STRUCTURE C
 * Ce sont ceux utilisé par le programmeur.
 */
#define ADRESSE_CHAMP_(O, NC) c_trouve_champ(O+1,		\
					     (char*)&((O)->NC),	\
					     sizeof((O)->NC))
#define ADRESSE_CHAMP(O,NC) c_trouve_champ(O,			\
					   (char*)&((O)->NC),	\
					   sizeof((O)->NC))

#define NUMERO_CHAMP(O, NC)   (ADRESSE_CHAMP(O, NC)->index)
#define BIT_DU_CHAMP(O, NC)   (1<<NUMERO_CHAMP(O, NC))

#define CHAMP_INVALIDE(O, NC) (OBJET(O)->invalide   & BIT_DU_CHAMP(O, NC))
#define CHAMP_CHANGE(O, NC)   (OBJET(O)->change & BIT_DU_CHAMP(O, NC))
#define NOTE_CHAMP_INVALIDE_(O,C) (OBJET(O)->invalide|=BIT_CHAMP(C))
#define NOTE_CHAMP_INVALIDE(O,NC) (OBJET(O)->invalide|=BIT_DU_CHAMP(O,NC))
/* Pas nécessaire d'utiliser le suivant dans la fonction "changement" */
#define NOTE_CHAMP_CHANGE_(O,C) OBJET(O)->change |= BIT_CHAMP(C)
#define NOTE_CHAMP_CHANGE(O,NC) NOTE_CHAMP_CHANGE_(O, ADRESSE_CHAMP(O, NC))

/* Change la taille d'une table 1d, 2d, 3d, ...
 * On donne le pointeur sur l'objet, le nom du champs
 * et les nouvelles tailles dans l'ordre du C : profondeur, hauteur, largeur
 */
#ifndef __cplusplus
#define CHANGE_TAILLE(O,NC,...) do					     \
 {									     \
   struct informations_table __ne_it__ ;				     \
   __ne_it__ = codec_informations_table(ADRESSE_CHAMP(O,NC)->tp, &(O)->NC) ; \
   codec_change_taille_table(&__ne_it__, __VA_ARGS__) ;			     \
 } while(0)
#endif

/*
 * Le macros pour lequel on donne le POINTEUR sur le champ
 * C'est utilisé de manière interne.
 */
#define CHAMP_CHANGE_(O,C)     ( O->change & BIT_CHAMP(C) )
#define CHAMP_INVALIDE_(O,C)   ( O->invalide   & BIT_CHAMP(C) )
#define EXTRAYABLE_(O, C)      (EXTRAIT(C) && FILS_(O,(C)->numero_fils)!=NULL)
#define SAUVABLE_(O, C)        ( SAUVE(C) && !EXTRAYABLE_(O,C) )
/*
 * **************************************************************************
 * **************************************************************************
 * Parcours
 * **************************************************************************
 * **************************************************************************
 */
/*
 * Des classes existantes
 */
#define POUR_LES_CLASSES(INDICE, CORPS)			\
{							\
   int CONCATENE(i_,INDICE) ;				\
   for( CONCATENE(i_,INDICE)=0 ;			\
        CONCATENE(i_,INDICE)<nombre_de_classes ;	\
	CONCATENE(i_,INDICE)++)				\
     {							\
       Classe *INDICE ;					\
       INDICE = &les_classes[CONCATENE(i_,INDICE)] ;	\
       CORPS ;						\
     }							\
}
/*
 * D'objet
 */
#define POUR_LES_ELEMENTS(LISTE,INDICE,CORPS)				      \
	POUR( void*, INDICE, LISTE, if ( INDICE ) {CORPS} )
#define POUR_TOUS_LES_ELEMENTS(LISTE,INDICE,CORPS)			      \
	POUR( void*, INDICE, LISTE, {CORPS} )
#define POUR_TOUS_LES_FILS_(O_,INDICE,CORPS)				      \
	POUR_TOUS_LES_ELEMENTS( (O_)->fils, INDICE, CORPS )
#define POUR_TOUS_LES_FILS(O,INDICE,CORPS)		                      \
        POUR_TOUS_LES_FILS_(OBJET(O),INDICE,CORPS)
#define POUR_LES_FILS_(O_,INDICE,CORPS)				              \
	POUR_LES_ELEMENTS( (O_)->fils, INDICE, CORPS )
#define POUR_LES_FILS(O,INDICE,CORPS)		                              \
        POUR_LES_FILS_(OBJET(O),INDICE,CORPS)
#define POUR_TOUS_LES_PARENTS_(O_,INDICE,CORPS)				      \
	POUR_LES_ELEMENTS( (O_)->parent, INDICE, { CORPS } )
#define POUR_LES_PARENTS_(O_,INDICE,CORPS)				      \
	POUR_LES_ELEMENTS( (O_)->parent, INDICE, { if ( !l_trouve(OBJET(INDICE)->fils,(O_)+1) ) continue ; CORPS } )
#define POUR_LES_PARENTS(O,INDICE,CORPS)	                              \
        POUR_LES_PARENTS_(OBJET(O),INDICE,CORPS)
#define FILS_(O_,N) (l_element((O_)->fils,N))
#define FILS(O,N) FILS_(OBJET(O), N)
/*
 * De champs
 */
#define POUR_LES_CHAMPS___CLASSE(CL,INDICE,CORPS)			\
 POUR( Champ*, INDICE, CL->champs, CORPS )
#define POUR_LES_CHAMPS_OBJET(INDICE,CORPS)	\
 POUR( Champ*, INDICE, champs_objet, CORPS )
#define POUR_LES_CHAMPS_CLASSE_(O_,INDICE,CORPS)			\
 POUR( Champ*, INDICE, (O_)->classe->champs, CORPS )
#define POUR_LES_CHAMPS_CLASSE(O,INDICE,CORPS)			\
 POUR_LES_CHAMPS_CLASSE_(OBJET(O),INDICE,CORPS)
#define POUR_LES_CHAMPS_(O_,INDICE,CORPS)		\
{							\
 if ( (O_)->classe->affiche_champs_classe )		\
	      POUR_LES_CHAMPS_OBJET(INDICE,CORPS) ;	\
 POUR_LES_CHAMPS_CLASSE_(O_,INDICE,CORPS) ;		\
}
#define POUR_LES_CHAMPS(O,INDICE,CORPS) \
 POUR_LES_CHAMPS_(OBJET(O),INDICE,CORPS)

#define POUR_TOUS_LES_CHAMPS_(O_,INDICE,CORPS)	\
{							\
POUR_LES_CHAMPS_OBJET(INDICE,CORPS) ;			\
POUR_LES_CHAMPS_CLASSE_(O_,INDICE,CORPS) ;		\
}
#define POUR_TOUS_LES_CHAMPS(O,INDICE,CORPS)	\
 POUR_TOUS_LES_CHAMPS_(OBJET(O),INDICE,CORPS)
#define POUR_TOUS_LES___CHAMPS(CL,INDICE,CORPS)	\
{							\
POUR_LES_CHAMPS_OBJET(INDICE,CORPS) ;			\
POUR_LES_CHAMPS___CLASSE(CL,INDICE,CORPS) ;		\
}


/*
 * **************************************************************************
 * **************************************************************************
 * **************************************************************************
 * PROTOTYPES
 * **************************************************************************
 * **************************************************************************
 * **************************************************************************
 */
/*
 * Les fonctions génériques sur les objets (u_objet.c)
 */
void o_sauve (const void *, FILE * f);
void o_positionne (void *, const Quadruplet *, int);
int o_prend_position (const void *, Quadruplet *, int);
Champ *u_champ_trouve_n (const void **o, int n, int init);
void o_destruction (void *);
void o_destruction_remonte (void *);
void o_annule_flag (void *);
void o_annule_flag2 (void *);
void o_annule_flag3 (void *);
void o_annule_flag4 (void *);
void o_annule_flag5 (void *);
void o_dump (void *objet);
void u_enleve_fils(void *parent, int child) ;
void u_ajoute_fils(void *parent, void *child, int i) ;

typedef enum
{ Continue, Arrete }
Parcour;
Parcour o_parcour (void *,
		   Parcour (*avant) (void *, void *donnee),
		   Parcour (*apres) (void *, void *donnee), void *donnee);
Parcour o_parcour_sauf_racine (void *,
			       Parcour (*avant) (void *, void *donnee),
			       Parcour (*apres) (void *, void *donnee),
			       void *donnee);
Parcour o_parcour_liste (Liste *,
			 Parcour (*avant) (void *, void *donnee),
			 Parcour (*apres) (void *, void *donnee),
			 void *donnee);
Parcour o_parcour_r(void *o,
		    Parcour (*avant)(void *, void*),
		    Parcour (*apres)(void *, void*),
		    void *donnee) ;

void c_monte_valeur(void* o, const Champ *ch);
void o_appel_changement (void *);
void o_appel_changement_liste (Liste *);
void o_appel_changement_sommet (void *);
Booleen un_fils_change (void *);
void *o_objet (const Classe * c);
void *o_recupere (FILE * f);
const char *u_change (void *o);
const char *o_informations (const void *);
void o_mise_a_jour_champs (void *);
void o_positionne_chaine (void *, const char *val);
void o_ils_deviennent_mes_fils (void *objet, Liste * fils, Booleen debut);
void o_il_devient_mon_fils (void *objet, void *fils, Booleen debut);

void u_ajoute_a_table_globale (Mot_clef a);
void u_ajoute_a_table_globale2 (Mot_clef a, Mot_clef b);
void u_ajoute_a_table_globale_string (Mot_clef a, const char *b);
void u_ajoute_a_table_globale_int (Mot_clef a, int b);
void u_ajoute_a_table_globale_affichage (Mot_clef a, Affichage b);

void u_ajoute_a_table_globale_methode_classe (Mot_clef a, void (*b) (void *));
void u_ajoute_a_table_globale_methode_char_classe (Mot_clef a,
						   const char *(*b) (const
								     void *));
void u_ajoute_a_table_globale_methode_widget (Mot_clef a,
					      void (*b) (const
							 Interface_widget *));
void u_ajoute_a_table_globale_tp_ecrit (Mot_clef a,
					void (*b) (void *, const Champ *,
						   const char *v));
void u_ajoute_a_table_globale_tp_lit (Mot_clef a,
				      const char *(*b) (void *,
							const Champ *));
void u_ajoute_a_table_globale_methode_lit_widget (Mot_clef a,
						  const char *(*b) (const
								    Interface_widget
								    *));
void u_ajoute_a_table_globale_methode_entree_widget (Mot_clef a,
						     Booleen (*b) (const
								   Interface_widget
								   *));

#define         u_racine(O)              globale_interface
#define         u_modele(O)              FILS(globale_interface,0)
void *u_operateur_ou_modele (const void *o);
const Liste *u_operateur_ou_modele_liste(const void *o) ;
void o_bascule_sous_actif (void *objet, int i);
void o_bascule_actif (void *objet);
Liste *u_charge (const char *nom, Booleen ouvre_fenetr);
void ecrit_numero_tag (const Champ * ch, int i);
int lit_numero_tag (const Champ * ch);
void u_dump_memoire ();

/*
 * Les fonctions sur les champs
 */
const char *type_vers_char (const Type_logique *);

Booleen tl_inclus (const Type_logique * a, const Type_logique * b);
const Champ *c_trouve (const void *, const char *nom_champ);
const char *c_recupere_ici_sans_copie (const void *, const Champ *);
const char *c_recupere_sans_copie (const void *, const char *nom_champ);
Booleen appartient (const void **o, const Champ ** ch,
		    const Type_logique * t);

void c_sauve (void *, const char *nom_champ, const char *va);
void c_sauve_ici (void *, const Champ *, const char *va);
void c_mise_a_jour (void *, const Champ *);

Champ *c_trouve_champ (const void *objet, const char *position, int taille);
void c_dump_trouve_type_table (FILE * f);
Booleen c_trouve_type (const void **objet,
		       const Champ ** c, const Type_logique * t);
Booleen c_type_existe (const void *o, const Type_logique * t);
const Champ *c_type_exacte_existe (const void *o, const Type_logique * t);
const char *c_recupere_type_sans_copie (const void *objet,
					const Type_logique *);
void c_sauve_type (void *objet, const Type_logique *, const char *valeur);
/*
 * Utilitaires
 */

/* o_objet.c */
Booleen applique (const Interface_widget * iw);
#include "u_etat.h"
#define A_EMPILER(X) o_applique_a_empiler( (void*)(&(X)), sizeof(X) ) ;
const char *decode_evenement (const char *e);
int graphe_coherent (const void *o);

void ajoute_crochet_destruction (void (*fct) (void *o));
void ajoute_crochet_changement (void (*fct) (void *o));

const char *u_nom_classe (const Classe * c);
const char *u_nom_type_logique (const Type_logique * c);
const char *u_nom_type_editeur (const Type_editeur * c);
const char *u_aide_objet (const void *o);
const char *u_aide_type_logique (const Type_logique * c);
const char *u_aide_type_editeur (const Type_editeur * c);
const char *u_aide_champ (const void *o, const Champ * ch);

/*
 * **************************************************************************
 * **************************************************************************
 * LANGAGE DE DEFINITION DE CLASSES ET CHAMPS 
 * **************************************************************************
 * **************************************************************************
 */


#define MCA(MC, V) u_ajoute_a_table_globale2(MC, ((Mot_clef)(V))) ;
#define MCS(MC, V) u_ajoute_a_table_globale_string(MC, V) ;
#define MCI(MC, V) u_ajoute_a_table_globale_int(MC, V) ;
#define MCM(MC, V) u_ajoute_a_table_globale_methode_classe(MC, (void (*)(void *))V) ; MCA(CONCATENE(MC,_nom), GUILLEMET(V))
#define MCC(MC, V) u_ajoute_a_table_globale_methode_char_classe(MC, (const char* (*)(const void *))V) ; MCS(CONCATENE(MC,_nom), GUILLEMET(V)) ;
#define MCW(MC, V) u_ajoute_a_table_globale_methode_widget(MC, V) ;
#define MCWL(MC, V) u_ajoute_a_table_globale_methode_lit_widget(MC, V) ;
#define MCWE(MC, V) u_ajoute_a_table_globale_methode_entree_widget(MC, V) ;
#define MCL(MC, V) u_ajoute_a_table_globale_tp_lit(MC, (const char* (*)(void*, const Champ *))V) ;
#define MCE(MC, V) u_ajoute_a_table_globale_tp_ecrit(MC, (void (*)(void*, const Champ *, const char *v))V) ;
#define MCB(MC)    u_ajoute_a_table_globale(MC) ;
#define MCP(MC, V) MCA(MC,V) MCA(CONCATENE(MC,_nom), GUILLEMET(V))
#define MCa(MC, V) u_ajoute_a_table_globale_affichage(MC, (Affichage)V) ;
/*
 * Classe
 */
#define           NOM_CLASSE(CHAINE) MCS(MC_nom                  ,CHAINE)
#define           NOM_STRUCT(CHAINE) MCS(MC_Cstruct              ,CHAINE)
#define           SUR_CLASSE(CHAINE) MCS(MC_surclasse            ,CHAINE)
#define     TAILLE_STRUCTURE(INT   ) MCI(MC_taille_structure     ,INT   )
#define          DESTRUCTION(MC    ) MCM(MC_destruction          ,MC    )
#define           CHANGEMENT(MC    ) MCM(MC_changement           ,MC    )
#define         INFORMATIONS(MC    ) MCC(MC_informations         ,MC    )
#define AFFICHE_CHAMP_CLASSE(BOOL  ) MCA(MC_affiche_champs_classe,BOOL  )
#define               PAQUET(INT   ) MCI(MC_paquet               ,INT   )
#define             DECALAGE(INT   ) MCI(MC_decalage             ,INT   )
#define            EVENEMENT(CHAINE) MCS(MC_evenement            ,CHAINE)
#define                 MENU(CHAINE) MCS(MC_menu                 ,CHAINE)
#define         BULLE_D_AIDE(CHAINE) MCS(MC_bulle_d_aide         ,CHAINE)
#define              FICHIER(CHAINE) MCS(MC_fichier              ,CHAINE)
#define                LIGNE(INT   ) MCI(MC_ligne                ,INT   )
/*
 * Champs
 */
#define HERITE_PAS_DES_CHAMPS     MCB(MC_herite_pas_des_champs)
#define            LABEL(CHAINE  ) MCS(MC_label           ,CHAINE           )
#define              NOM(CHAINE  ) MCS(MC_nom_champ       ,CHAINE           )
#define             CNOM(CHAINE  ) MCS(MC_Cnom_champ      ,CHAINE           )
#define           OPTION(INT     ) MCI(MC_option          ,INT              )
#define       NON_OPTION(INT     ) MCI(MC_non_option      ,INT              )
#define         POSITION(INT     ) MCI(MC_position        ,INT              )
#define           TAILLE(INT     ) MCI(MC_taille          ,INT              )
#define           DEFAUT(CHAINE  ) MCS(MC_defaut          ,CHAINE           )
#define   TYPE_DANS_FILS(TL      ) MCA(MC_type_dans_fils  ,init_##TL(NULL)  )
#define      NUMERO_FILS(INT     ) MCI(MC_numero_fils     ,INT              )
#define               TL(TL      ) MCA(MC_tl              ,init_##TL(NULL)  )
#define CLASSE_PARAMETRE(CHAINE  ) MCS(MC_classe_parametre,CHAINE           ) TYPE_DANS_FILS(L_void)
#define               TP(PTR_FCT ) MCA(MC_tp              ,&infos_##PTR_FCT )
#define         TP_ECRIT(PTR_FCT ) MCE(MC_tp_ecrit        ,PTR_FCT          )
#define           TP_LIT(PTR_FCT ) MCL(MC_tp_lit          ,PTR_FCT          )
#define               TE(TE      ) MCA(MC_te              ,init_##TE(NULL)  )
#define          TE_AIDE(PTR_FCT ) MCWL(MC_te_aide        ,PTR_FCT          )
#define         TE_TEXTE(CHAINE  ) MCS(MC_te_texte        ,CHAINE           )
#define        AFFICHAGE(A       ) MCa(MC_affichage       ,A                )
#define       TE_LARGEUR(INT     ) MCI(MC_te_largeur      ,INT              )
#define       TE_HAUTEUR(INT     ) MCI(MC_te_hauteur      ,INT              )
#define         TE_ECRIT(MW      ) MCW(MC_te_ecrit        ,MW               )
#define           TE_LIT(MW      ) MCWL(MC_te_lit         ,MW               )
#define        TE_ENTREE(MW      ) MCWE(MC_te_entree      ,MW               )
#define       TE_CLAVIER(MW      ) MCW(MC_te_clavier      ,MW               )
#define     TE_MOUVEMENT(MW      ) MCW(MC_te_mouvement    ,MW               )
#define        TE_EXPOSE(MW      ) MCW(MC_te_expose       ,MW               )	/* Obsolete */

/*
 * Des commandes
 */
#define DEBUT_CHAMP   	  MCB(MC_debut_champ)
#define FIN_CHAMP     	  MCB(MC_fin_champ)
#define FIN_DEFINITION    MCB(MC_fin_definition)
/*
 * Définition de classe
 */
#include "t_base.h"		/* Pour P_ Void */

#define CLASSE(CLS,TYPE_STRUCT,PROG)		\
Mot_clef *CONCATENE(u_classe_,CLS)()			\
{						\
  TYPE_STRUCT m ;				\
  char nom_virtuel[100] ; \
  int virtuel ; \
  const char *cls = GUILLEMET(CLS) ; \
						\
  la_position_dans_la_table_globale = 0 ;	\
  la_taille_de_la_table_globale	= 0 ;		\
  la_table_globale = NULL ;			\
  virtuel = 0 ; \
  nom_virtuel[0] = '\0' ; \
  (void)virtuel ; \
  (void)nom_virtuel ; \
						\
  TAILLE_STRUCTURE(sizeof(m)+sizeof(Objet))	\
  NOM_STRUCT(GUILLEMET(TYPE_STRUCT))		\
  NOM_CLASSE(cls)			\
  SUR_CLASSE("")				\
  FICHIER(__FILE__)				\
  LIGNE(__LINE__)				\
  AFFICHE_CHAMP_CLASSE(Vrai)			\
  L_void					\
  Label						\
  PROG						\
  FIN_DEFINITION				\
  return(la_table_globale) ;			\
}

/*
 * Définition de champ
 */
#define CHAMP(CHP,PROG)						\
     DEBUT_CHAMP						\
     NOM(GUILLEMET(CHP))					\
     CNOM(GUILLEMET(CHP))					\
     POSITION(((char*)(&m.CHP)-(char*)(&m))+sizeof(Objet))	\
     TAILLE(sizeof(m.CHP))					\
     LABEL(GUILLEMET(CHP))					\
     PROG							\
     FIN_CHAMP

#define CHAMP_VIRTUEL(PROG)					\
     DEBUT_CHAMP						\
     sprintf(nom_virtuel, "virtuel_%s_%d", cls, virtuel++) ; \
     NOM(strdup(nom_virtuel)/* fuite memoire */ ) CNOM("virtuel") LABEL("") \
     Virtuel \
     PROG							\
     FIN_CHAMP
/*
 * Utilitaires
 */
#define PTR_(O,C) ( ((char*)O) + (C)->position )
#define PTR(O,C)  PTR_( OBJET(O), C)
#define T(TYPE,O,C) (*((TYPE*)PTR(O,C)))


/* Translate to and from id and pointers */
/* 64 is used because in some part of NanoEdit some data is added */

#if 1

#define OBJECT_ID(M) (64*(unsigned int)INDEX_OBJET(OBJET(M)))
#define OBJECT_PTR(ID) ((Objet*)(globale_gm_objet.objets_alloues[(ID)/64])+1)

#else

#define OBJECT_ID(M) (int)(M)
#define OBJECT_PTR(ID) (void*)(ID)

#endif

#endif

