#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <cstdio>
#include <vector>
/**
@author Seyed-Vahid Sanei-Mehri
Email contact: vas@iastate.edu
*/

#include <cstdlib>
#include <string>
#include <string.h>
#include <sstream>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <math.h>
#include <random>
#include <time.h>
#include <assert.h>
#include "load_graph.h"

const int LIMIT_ON_SIZE = 1 << 17;

#define mmin(x, y) ((x)<(y)?(x):(y))
#define mmax(x, y) ((x)>(y)?(x):(y))


class quickM {
public:
	quickM(load_graph&, double);
	~quickM();
	std::set < std::pair < int, std::set <int> > > get_all_quasi_cliques();
	void extract_all_quasi_cliques();
	void expand(std::vector<int>&);
private:
	std::vector < std::vector <int> > adj;
	std::vector <int> degrees;
	std::vector <int> degrees_in_ex;
	std::vector <int> aux;
	std::vector <int> indeg_X;
	std::vector <int> exdeg_X;
	std::vector <int> in_common;
	std::vector <int> X;
	std::vector <int> Q;
	std::vector <int> candidate_set;
	std::vector <int> idx_in_candidate;
	std::vector <int> next_id_candidate;
	std::vector <int> prev_id_candidate;
	std::vector <int> candidates_in_common_with_A;
	std::vector <int> candidates_in_common_with_B;
	std::vector <int> aux_sum;


	std::vector <bool> should_be_in_X;
	std::vector <bool> is_candidate;
	std::vector <bool> is_in_current_quasi_clique;
	std::vector <bool> is_adacent_to_A;
	std::vector <bool> is_adjacent_to_B;
	std::vector <bool> is_two_hop_neighborhood;
	std::vector <bool> in_the_search_space;
	std::vector <bool> is_reachable_by_2_hops;

	int idx_aux;

	std::vector < std::unordered_set <int> > two_hop_neighbors;
	std::set < std::pair <int, int> > edges;

	int topK;
	std::set < std::pair < int, std::set <int> > > all_quasi_cliques;
	std::set < std::pair < int, std::set <int> > > top_k_set;
	bool changeInTheSet;

	int V;
	int min_size;
	int beg_idx_cand_X;
	int end_idx_cand_X;
	int sz_cand_X;
	int adjacent_to_A_in_candidate;
	int adjacent_to_B_in_candidate;
	int non_adjacent_to_A_in_X;
	int non_adjacent_to_B_in_X;
	int should_be_connected_to_AB;
	int n_neighbors_A;
	int n_neighbors_B;
	int min_degree_start;
	int U_X, L_X;
	int l_i, h_i;
	int idx_X, min_degree, deg, sum_indeg_X, xxx;
	int current_min_degree_X;
	int current_min_exIn_degree_X;
	int final_answer;
	int quasi_cliques;

	double gamma;


	bool is_quasi_clique();
	int ceiling(double);
	void upper_bound();
	void lower_bound();
	void insert_to_quasi_clique_from_candidates(int &);
	void remove_from_quasi_clique_return_back_to_candidates(int &);
	bool remove_from_candidates(int &, std::vector<int> &);
	void reutrn_back_to_candidates(std::vector<int>&);
	void insert_to_candidate_from_nowhere(int &);
	void remove_from_candX_to_null(int &);
	void add_to_in_common(int &, int &, int &, std::set<int> &, std::vector<int> &);
	void get_two_hop_neighbors(std::vector<int> &Set, std::set<int> &);
	void add_to_in_common(int &, int &, int, std::vector<int> &, std::unordered_set<int> &);
	void get_two_hop_neighbors(int &x, std::vector<int> &);
	bool prune_min_degree(int &, std::vector<int> &);
	bool prune_diam(int &, std::vector<int> &);
	void prune_critical(int &, std::vector<int> &);
	bool prune_lowerUpperbounds(std::vector <int> &, std::vector<int> &);
	void back_track();
	void get_quasi_clique();
};


