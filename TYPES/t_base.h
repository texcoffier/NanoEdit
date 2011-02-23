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
#ifndef T_BASE_H
#define T_BASE_H

#include "u_geometrie.h" /* Pour le type Flottant */
#include "codec.h"

#define DEFINITION_TYPE_LOGIQUE(NOM, SURCLASSE, AIDE, CORPS)	\
Type_logique infos_##NOM ;					\
const Type_logique* init_##NOM(Champ *c)			\
{								\
  Type_logique *tl = &infos_##NOM ;				\
  if ( c )							\
     {								\
  	init_##SURCLASSE(c) ;					\
  	c->tl              = tl ;				\
        *tl = infos_##SURCLASSE ;				\
  	tl->surclasse      = &infos_##SURCLASSE ;		\
  	tl->nom            = #NOM ;				\
  	tl->bulle_d_aide      = AIDE ;				\
  	{							\
  	   CORPS ;						\
  	}							\
     }								\
  tl->initialisation = init_##NOM ;				\
  return(tl) ;							\
}

#define DEFINITION_TYPE_EDITEUR(NOM, SURCLASSE, AIDE, CORPS)	\
Type_editeur infos_##NOM  ;					\
const Type_editeur* init_##NOM(Champ *c)			\
{								\
  Type_editeur *te = &infos_##NOM ;				\
  if ( c )							\
    {								\
  	init_##SURCLASSE(c) ;					\
  	c->te              = te ;				\
        *te = infos_##SURCLASSE ;				\
  	te->surclasse      = &infos_##SURCLASSE ;		\
  	te->nom            = #NOM ;				\
  	te->bulle_d_aide      = AIDE ;				\
  	{							\
  	  CORPS ;						\
  	}							\
    }								\
  te->initialisation = init_##NOM ;				\
  return(te) ;							\
}

#define FONCTION_EDITEUR(A,F) A=F ; A##_nom = #F
#define PREDICAT(X) FONCTION_EDITEUR(tl->predicat_verifie, X)
#define SOUS_CLASSE(X) FONCTION_EDITEUR(tl->sous_classe, X)
#define PROTOTYPE_TYPE_PHYSIQUE CODEC_PROTOTYPE
#define PROTOTYPE_TYPE_PHYSIQUE_VOID CODEC_PROTOTYPE_VOID

#define PROTOTYPE_TYPE_EDITEUR(NOM)		\
extern Type_editeur  infos_##NOM ;		\
const Type_editeur* init_##NOM(Champ*) ;

#define PROTOTYPE_TYPE_LOGIQUE(NOM)		\
extern Type_logique  infos_##NOM ;		\
const Type_logique* init_##NOM(Champ*) ;

struct fenetre_menu
{
  char *chemin ;		/* Chemin d'accès au menu, "" pour la racine */
  int index ;			/* Index du premier choix du menu */
  NanoWidget widget ;		/* Fenêtre menu */
  const Champ *ch ;		/* Le champ du menu */
  void *objet ;
  void *objet_contextuel ;
} ;

typedef struct t_menu
{
  char *selection   ;  /* Texte de la dernière sélection */
  char *description ;  /* Si le menu est dynamique       */
  char index        ;  /* Index derniere selection       */
  char option_menu  ;  /* Vrai si menu option            */
  Liste *menus      ;  /* Liste de "struct fenetre_menu" */
} Menu ;


typedef struct l_liste_referenced
{
  Liste *modeles ; // Les modeles utilisant cette liste
  struct l_liste *contenu ;
  char *fichier ;		/* Nom du fichier contenant la fonction */
  char *fichier_complet ;       /* Nom complet */
} Liste_Referenced ;


typedef struct t_operateur
{
  Liste_Referenced *liste ;	/* Valeur une fois chargee */
  char *noms_variables ;	/* Numero=Nom\n pour toutes les variables */
} Operateur ;

#define CONTENU(O) (O)->liste->contenu
#define NOMFICHIER(O) (O)->liste->fichier
#define NOMCOMPLET(O) (O)->liste->fichier_complet


#define CODEC_OBJET (*((Objet**)&globale_traduction->objet))
#define CODEC_CHAMP (*((const Champ**)&globale_traduction->champ))

/*
 * Utilitaires exportés
 */

int u_initialiser_iw(Interface_widget *iw, NanoWidget w,void* p,NanoEvent *e) ;
PROTOTYPE_CALLBACK_ACTION(u_callback_bouton, w, p) ;
PROTOTYPE_CALLBACK(u_callback_expose, w, p) ;
PROTOTYPE_CALLBACK_EVENT(u_ev_motion        , w, p, e) ;
PROTOTYPE_CALLBACK_EVENT(u_ev_motion_na     , w, p, e) ;
PROTOTYPE_CALLBACK_EVENT(u_ev_button_press  , w, p, e) ;
PROTOTYPE_CALLBACK_EVENT(u_ev_button_release, w, p, e) ;
PROTOTYPE_CALLBACK_EVENT(u_ev_key_press     , w, p, e) ;

void u_vrai_affiche_menu(NanoWidget w, void *objet, const Champ *ch,
			 NanoEvent *event, void *objet_contextuel) ;

void        Editepas_ecrit    (const Interface_widget*) ;
const char *Jamais_pareil_lit (const Interface_widget* iw            ) ;

const Classe *char_vers_classe       (const char *nom    ) ;
const char   *int_vers_char          (long f             ) ;
const char   *flottant_vers_char     (Flottant f         ) ;
const char   *ptr_flottant_vers_char (Flottant *f        ) ;
const char   *triplet_vers_char      (const Triplet* p   ) ;
const char   *quadruplet_vers_char   (const Quadruplet* p) ;
Triplet    char_vers_triplet         (const char* t      ) ;
Quadruplet char_vers_quadruplet      (const char* t      ) ;

void revert_modele(Operateur *op, const char* nom_fichier, int ouvre_fenetre) ;

void  Variable_recupere  (const void       *o, const Champ *c);
void  Variable_sauve     (const void       *o, const Champ *c);
void  *Variable_contenu  (const void *o, const Champ *c);
Liste *Operateur_contenu (const void *o, const Champ *c);
void widget_Graphe_trace(const Interface_widget *iw) ;
void w_expose(void *o) ;



/*
 * Types physique
 */
PROTOTYPE_TYPE_PHYSIQUE_VOID(codec_void)
PROTOTYPE_TYPE_PHYSIQUE_VOID(codec_virtuel)
PROTOTYPE_TYPE_PHYSIQUE_VOID(codec_champ_texte)
PROTOTYPE_TYPE_PHYSIQUE(codec_pointeur_extrait, int      )
PROTOTYPE_TYPE_PHYSIQUE(codec_actif           , int      )
PROTOTYPE_TYPE_PHYSIQUE(codec_classe          , Classe*  )
PROTOTYPE_TYPE_PHYSIQUE(codec_adresse_objet   , void*    )
PROTOTYPE_TYPE_PHYSIQUE(codec_operateur       , Operateur)
PROTOTYPE_TYPE_PHYSIQUE(codec_commande_menu   , Menu     )
PROTOTYPE_TYPE_PHYSIQUE(codec_option_menu     , Menu     )
PROTOTYPE_TYPE_PHYSIQUE(codec_modele          , Liste_Referenced*)

#define infos_codec_entier   infos_codec_int
#define infos_codec_entier8  infos_codec_char_decimal
#define infos_codec_chaine   infos_codec_char_etoile
#define infos_codec_chaine_sans_echappement infos_codec_char_etoile_sans_echappement
#define infos_codec_liste    infos_codec_liste_unsigned_long

#define P_operateur               TP(codec_operateur              )
#define P_entier                  TP(codec_int                    )
#define P_long                    TP(codec_long                   )
#define P_entier8                 TP(codec_char_decimal           )
#define P_chaine                  TP(codec_chaine                 )
#define P_chaine_sans_echappement TP(codec_chaine_sans_echappement)
#define P_float                   TP(codec_float                  )
#define P_double                  TP(codec_double                 )
#define P_angle                   TP(codec_angle                  )
#define P_liste                   TP(codec_liste                  )
#define P_void                    TP(codec_void                   )
#define P_actif                   TP(codec_actif                  )
#define P_adresse_objet           TP(codec_adresse_objet          )
#define P_modele                  TP(codec_modele                 )
#define P_classe                  TP(codec_classe                 )
#define P_option_menu             TP(codec_option_menu            )
#define P_commande_menu           TP(codec_commande_menu          )
#define P_graphe_N_l3_float       TP(codec_graphe_N_l3_float      )
#define P_graphe_N_l2_l3_float    TP(codec_graphe_N_l2_l3_float   )
#define P_graphe_N_l6_float       TP(codec_graphe_N_l6_float      )
#define P_graphe_N_l3_float_chaine_A_chaine TP(codec_graphe_N_l3_float_chaine_A_chaine)
#define P_graphe_N_pcc_A_chaine TP(codec_graphe_N_pcc_A_chaine)
#define P_pointeur_extrait        TP(codec_pointeur_extrait       )
#define P_champ_texte             TP(codec_champ_texte            )
#define P_table_float             TP(codec_L_float                )
#define P_table_double            TP(codec_L_double               )
#define P_virtuel(OUT,IN)  TP(codec_virtuel      ) TP_ECRIT(OUT) TP_LIT(IN) NonInitialiser Virtuel
#define P_double_borne            TP(codec_flottant_borne)
/*
 * Type Éditeurs
 */
PROTOTYPE_TYPE_EDITEUR(E_void          )
PROTOTYPE_TYPE_EDITEUR(E_texte         )
PROTOTYPE_TYPE_EDITEUR(E_long_texte    )
PROTOTYPE_TYPE_EDITEUR(E_angle         )
PROTOTYPE_TYPE_EDITEUR(E_booleen       )
PROTOTYPE_TYPE_EDITEUR(E_geometrie     )
PROTOTYPE_TYPE_EDITEUR(E_titre         )
PROTOTYPE_TYPE_EDITEUR(E_bouton        )
PROTOTYPE_TYPE_EDITEUR(E_commande_menu )
PROTOTYPE_TYPE_EDITEUR(E_option_menu   )
PROTOTYPE_TYPE_EDITEUR(E_drawing       )
PROTOTYPE_TYPE_EDITEUR(E_graphe_modele )
PROTOTYPE_TYPE_EDITEUR(E_ascenseur     )

#define E_void          TE(E_void         )
#define E_texte         TE(E_texte        )
#define E_long_texte    TE(E_long_texte   )
#define E_angle         TE(E_angle        )
#define E_booleen       TE(E_booleen      )
#define E_geometrie     TE(E_geometrie    )
#define E_titre         TE(E_titre        )
#define E_bouton        TE(E_bouton       )
#define E_commande_menu TE(E_commande_menu)
#define E_option_menu   TE(E_option_menu  )
#define E_drawing       TE(E_drawing      )
#define E_graphe_modele TE(E_graphe_modele)
#define E_ascenseur     TE(E_ascenseur    )

/*
 * Type logique
 */
PROTOTYPE_TYPE_LOGIQUE(L_void_void                )
PROTOTYPE_TYPE_LOGIQUE(L_metamorphe               )
PROTOTYPE_TYPE_LOGIQUE(L_void                     )
PROTOTYPE_TYPE_LOGIQUE(L_fonction                 )
PROTOTYPE_TYPE_LOGIQUE(L_tag                      )
PROTOTYPE_TYPE_LOGIQUE(L_affiche_pov              )
PROTOTYPE_TYPE_LOGIQUE(L_affiche_stl              )
PROTOTYPE_TYPE_LOGIQUE(L_affiche_obj              )
PROTOTYPE_TYPE_LOGIQUE(L_affiche_gl               )
PROTOTYPE_TYPE_LOGIQUE(L_applique_gl              )
PROTOTYPE_TYPE_LOGIQUE(L_applique                 )
PROTOTYPE_TYPE_LOGIQUE(L_raccourci                )
PROTOTYPE_TYPE_LOGIQUE(L_prevenir_si_modele_change)
PROTOTYPE_TYPE_LOGIQUE(L_bouton                   )
PROTOTYPE_TYPE_LOGIQUE(L_geometrie                )
PROTOTYPE_TYPE_LOGIQUE(L_titre                    )
PROTOTYPE_TYPE_LOGIQUE(L_drawing                  )
PROTOTYPE_TYPE_LOGIQUE(L_graphe_modele            )
PROTOTYPE_TYPE_LOGIQUE(L_graphe                   )
PROTOTYPE_TYPE_LOGIQUE(L_graphe_de_points         )
PROTOTYPE_TYPE_LOGIQUE(L_graphe_de_points_vecteurs)
PROTOTYPE_TYPE_LOGIQUE(L_valeur                   )
PROTOTYPE_TYPE_LOGIQUE(L_menu                     )
PROTOTYPE_TYPE_LOGIQUE(L_commande_menu            )
PROTOTYPE_TYPE_LOGIQUE(L_option_menu              )
PROTOTYPE_TYPE_LOGIQUE(L_nombre                   )
PROTOTYPE_TYPE_LOGIQUE(L_nombre_positif           )
PROTOTYPE_TYPE_LOGIQUE(L_entier                   )
PROTOTYPE_TYPE_LOGIQUE(L_long                     )
PROTOTYPE_TYPE_LOGIQUE(L_flottant                 )
PROTOTYPE_TYPE_LOGIQUE(L_booleen                  )
PROTOTYPE_TYPE_LOGIQUE(L_classe                   )
PROTOTYPE_TYPE_LOGIQUE(L_liste                    )
PROTOTYPE_TYPE_LOGIQUE(L_liste_objet              )
PROTOTYPE_TYPE_LOGIQUE(L_sous_actif               )
PROTOTYPE_TYPE_LOGIQUE(L_chaine                   )
PROTOTYPE_TYPE_LOGIQUE(L_attribut_pov             )
PROTOTYPE_TYPE_LOGIQUE(L_variable                 )
PROTOTYPE_TYPE_LOGIQUE(L_operateur                )
PROTOTYPE_TYPE_LOGIQUE(L_solide                   )
PROTOTYPE_TYPE_LOGIQUE(L_pointeur_extrait         )
PROTOTYPE_TYPE_LOGIQUE(L_flottant_borne           )
PROTOTYPE_TYPE_LOGIQUE(L_debut_boite_horizontale  )
PROTOTYPE_TYPE_LOGIQUE(L_debut_boite_verticale    )
PROTOTYPE_TYPE_LOGIQUE(L_fin_boite                )
PROTOTYPE_TYPE_LOGIQUE(L_prog                     )
PROTOTYPE_TYPE_LOGIQUE(L_plan                     )



#define L_void_void           TL(L_void_void)
#define L_void                TL(L_void)
#define L_metamorphe          TL(L_metamorphe)
#define L_fonction(FCT)       TL(L_fonction)      TP_ECRIT(FCT)
#define L_fonction2(OUT,IN)   TL(L_fonction)      TP_ECRIT(OUT) TP_LIT(IN)
#define L_tag(OUT,IN)         TL(L_tag) TP_ECRIT(OUT) TP_LIT(IN)
#define L_affiche_pov(FCT)    TL(L_affiche_pov)   TP_ECRIT(FCT)
#define L_affiche_stl(FCT)    TL(L_affiche_stl)   TP_ECRIT(FCT)
#define L_affiche_obj(FCT)    TL(L_affiche_obj)   TP_ECRIT(FCT)
#define L_affiche_gl(FCT)     TL(L_affiche_gl)    TP_ECRIT(FCT)
#define L_applique_gl(FCT)    TL(L_applique_gl)   TP_ECRIT(FCT)
#define L_applique_gl_rapide(FCT,IN) L_applique_gl(FCT) TP_LIT(IN)
#define L_applique(FCT)       TL(L_applique)      TP_ECRIT(FCT)
#define L_raccourci(FCT)      TL(L_raccourci)     TP_ECRIT(FCT)
#define L_bouton(FCT)         TL(L_bouton)        TE_ENTREE(FCT)
#define L_geometrie           TL(L_geometrie)
#define L_titre               TL(L_titre)
#define L_drawing             TL(L_drawing)
#define L_graphe_modele       TL(L_graphe_modele) TE_ECRIT(widget_Graphe_trace)
#define L_graphe              TL(L_graphe)
#define L_graphe_de_points    TL(L_graphe_de_points)
#define L_valeur              TL(L_valeur)
#define L_menu                TL(L_menu)
#define L_commande_menu(FCT)  TL(L_commande_menu) TE_ENTREE(FCT)
#define L_option_menu         TL(L_option_menu)
#define L_nombre              TL(L_nombre)
#define L_nombre_positif      TL(L_nombre_positif)
#define L_entier              TL(L_entier)
#define L_long                TL(L_long)
#define L_flottant            TL(L_flottant)
#define L_booleen             TL(L_booleen)
#define L_classe              TL(L_classe)
#define L_liste               TL(L_liste)
#define L_liste_objet         TL(L_liste_objet)
#define L_sous_actif          TL(L_sous_actif)
#define L_chaine              TL(L_chaine)
#define L_attribut_pov        TL(L_attribut_pov)
#define L_variable            TL(L_variable)
#define L_operateur           TL(L_operateur)
#define L_solide              TL(L_solide)
#define L_pointeur_extrait    TL(L_pointeur_extrait)
#define L_flottant_borne      TL(L_flottant_borne)
#define L_prog                TL(L_prog)
#define L_plan                TL(L_plan)
#define L_prevenir_si_modele_change(IN) TL(L_prevenir_si_modele_change) TP_LIT(IN)

#define HORIZONTALE    TL(L_debut_boite_horizontale)
#define VERTICALE      TL(L_debut_boite_verticale)
#define DEBUT_BOITE(P) CHAMP_VIRTUEL(P)
#define FIN_BOITE      CHAMP_VIRTUEL(TL(L_fin_boite))

#endif

