#ifndef __AQ_SYMBOLE_H__
#define __AQ_SYMBOLE_H__

namespace aq
{

/// \brief symbole use by aq engine and aq loader
/// \deprecated should be split into several enum
typedef enum 
{
	faux, vrai, vide, unaire, binaire, 
	scalaire, vecteur, liste, r_et, 
	r_ou, r_feuille, r_frere, mini_mot, maxi_mot, liste_mot,
	r_liste, inf_egal, egal, sup_egal,
	hp, tuples, r_tag, fils_gauche, fils_droit,
	// les types des variables
	t_int, /* t_float, */ t_double, 
	t_date1, t_date2, t_date3,
	t_char , t_long_long, t_raw ,
	// mouvement dans l'arbre pour arriver au noeud en cours
	m_up, m_down,
	// nature des feuille dans tree
	n_contenu, n_table,
	// retour de fonciton de lecture de fichiers
	t_continue ,  t_done , t_eof, t_file_read_error , 
	// remplissage de la structure requete
	r_jeq, r_between, r_sup, r_inf, r_leq, r_seq, r_in, r_equal ,
	// element lien vkg : source, pivot  ou cible
	l_source,  l_pivot, l_cible,
	// etape calcul 
	ec_requete, ec_jointure, ec_etape_1, ec_etape_2, ec_etape_3 ,
	ec_etape_4, ec_hp, ec_tuple,
	// nature du vdg pendant les calculs 
	c_neutre, c_calcul,
	// rattrapage d'erreurs ou manques
	string, integer, d_nulle, comma,
	// partie d'énumÈration normalement devenue inutile le 26/04/04 ****
	my_eof, une_table, column, 
	copy/*, type le 17/10/04 */ , vdg, troncat/*trunc*/, name,
	file, t_row_id, precision, t_star, last_symbole
} symbole;

}

#endif
