/**
@author Seyed-Vahid Sanei-Mehri
Email contact: vas@iastate.edu
*/

#include "quickM.h"

const int INF = 1 << 29;
#define eps 1e-10

quickM::quickM(load_graph& G, double used_gamma) {
	this->V = G.num_vertices();
	this->min_size = G.min_size;
	this->gamma = used_gamma;
	this->adj = G.adj;

	this->two_hop_neighbors.resize(V);

	this->degrees.resize(V);
	this->degrees_in_ex.resize(V);
	this->aux.clear();
	this->indeg_X.resize(V);
	this->exdeg_X.resize(V);
	this->in_common.resize(V);
	this->X.resize(V);
	this->Q.resize(V);
	this->candidate_set.resize(V << 1);
	this->idx_in_candidate.resize(V << 1);
	this->next_id_candidate.resize(V << 1);
	this->prev_id_candidate.resize(V << 1);
	this->candidates_in_common_with_A.resize(V);
	this->candidates_in_common_with_B.resize(V);
	this->aux_sum.resize(V);
	this->is_reachable_by_2_hops.resize(V);

	this->should_be_in_X.resize(V);
	this->is_candidate.resize(V);
	this->is_in_current_quasi_clique.resize(V);
	this->is_adacent_to_A.resize(V);
	this->is_adjacent_to_B.resize(V);
	this->is_two_hop_neighborhood.resize(V);
	this->in_the_search_space.resize(V);

	this->idx_aux = 0;

	all_quasi_cliques.clear();
	top_k_set.clear();

	this->beg_idx_cand_X = 0;
	this->end_idx_cand_X = 0;
	this->sz_cand_X = 0;
	this->adjacent_to_A_in_candidate = 0;
	this->adjacent_to_B_in_candidate = 0;
	this->non_adjacent_to_A_in_X = 0;
	this->non_adjacent_to_B_in_X = 0;
	this->should_be_connected_to_AB = 0;
	this->n_neighbors_A = 0;
	this->n_neighbors_B = 0;
	this->min_degree_start = 0;
	this->U_X = 0;
	this->L_X = 0;
	this->l_i = 0;
	this->h_i = 0;
	this->idx_X = 0;
	this->min_degree = 0;
	this->deg = 0;
	this->sum_indeg_X = 0;
	this->current_min_degree_X = 0;
	this->current_min_exIn_degree_X = 0;
	this->final_answer = 0;
	this->quasi_cliques = 0; 
}

quickM::~quickM() {}

bool quickM::is_quasi_clique() {
	return idx_X >= min_size && current_min_degree_X >= ceil(gamma * (idx_X - 1));
}

int quickM::ceiling(double x) {
	return (int)(ceil(x) + eps);
}

// find an upper bound on the number of vertices that can be added from candidate list
void quickM::upper_bound() {
	int idx = 0;
	aux.clear();
	U_X = (int)(current_min_exIn_degree_X / gamma + eps) + 1 - idx_X;
	for (int iter = beg_idx_cand_X; iter < end_idx_cand_X; iter = next_id_candidate[iter]) {
		int x = candidate_set[iter];
		if (is_candidate[x]) {
			idx++;
			aux.push_back(indeg_X[x]);
		}
	}
	idx_aux = idx;
	std::sort(aux.begin(), aux.end());
	U_X = mmin(U_X, idx);
	// tight upper bound further 
	int sum = 0;
	int mx = -1;
	for (int t = 1; idx > 0 && t <= U_X; t++) {
		sum += aux[--idx];
		if (sum_indeg_X + sum >= idx_X * ceiling(gamma * (idx_X + t - 1))) {
			mx = t;
		}
	}
	if (mx != -1) U_X = mx;
}

// find a lower bound on the number of vertices that must be added to the current quasi clique using minimum degree and minimum size thresholds.
void quickM::lower_bound() {
	int sz_cand = sz_cand_X;
	L_X = 1;
	for (int t = 1; t <= sz_cand; t++) {
		if (current_min_degree_X + t >= ceiling(gamma * (idx_X + t - 1))) {
			L_X = t;
			break;
		}
	}
	int sum = 0;
	int idx = idx_aux;
	if (idx == 0) {
		L_X = 0;
		return;
	}
	for (int i = 0; i < L_X; i++) sum += aux[--idx];
	for (int t = L_X;; t++) {
		if (sum_indeg_X + sum >= idx_X * ceiling(gamma * (idx_X + t - 1))) {
			L_X = t;
			break;
		}
		if (idx <= 0) {
			L_X = sz_cand_X + 1;
			break;
		}
		sum += aux[--idx];
	}
}

// here we add a vertex to quasi clique from candidate list
void quickM::insert_to_quasi_clique_from_candidates(int &x) {
	if (is_in_current_quasi_clique[x] == true) return;
	is_candidate[x] = false; is_in_current_quasi_clique[x] = true;
	int id = idx_in_candidate[x];
	next_id_candidate[prev_id_candidate[id]] = next_id_candidate[id];
	prev_id_candidate[next_id_candidate[id]] = prev_id_candidate[id];
	sz_cand_X--;
	X[idx_X++] = x;
	indeg_X[x] = exdeg_X[x] = 0;
	for (int i = 0; i < (int)adj[x].size(); i++) {
		int y = adj[x][i];
		if (is_in_current_quasi_clique[y] == true) {
			deg = indeg_X[y];
			degrees[deg] --;
			degrees[deg + 1] ++;
			if (current_min_degree_X == deg && degrees[deg] == 0) {
				current_min_degree_X++;
			}
			indeg_X[x] ++;
			sum_indeg_X += 2;
		}
		else if (is_candidate[y] == true) exdeg_X[x] ++;
		indeg_X[y] ++;
		exdeg_X[y] --;
	}
	if (indeg_X[x] < current_min_degree_X)
		current_min_degree_X = indeg_X[x];
	if (indeg_X[x] + exdeg_X[x] < current_min_exIn_degree_X)
		current_min_exIn_degree_X = indeg_X[x] + exdeg_X[x];
	degrees[indeg_X[x]] ++;
	degrees_in_ex[indeg_X[x] + exdeg_X[x]] ++;
}

// we remove a vertex from quasi clique and put it back in the candidate list
void quickM::remove_from_quasi_clique_return_back_to_candidates(int &x) {
	if (is_in_current_quasi_clique[x] == false) return;
	is_candidate[x] = true; is_in_current_quasi_clique[x] = false;
	sz_cand_X++;
	int id = idx_in_candidate[x];
	next_id_candidate[prev_id_candidate[id]] = id;
	prev_id_candidate[next_id_candidate[id]] = id;
	idx_X--;
	for (int i = 0; i < (int)adj[x].size(); i++) {
		int y = adj[x][i];
		if (is_in_current_quasi_clique[y] == true) {
			deg = indeg_X[y];
			degrees[deg] --;
			degrees[deg - 1]++; // current_min_degree_X is updated in the main recursive function. No worries about that!!
			sum_indeg_X -= 2;
		}
		indeg_X[y] --;
		exdeg_X[y] ++;
	}
	degrees[indeg_X[x]] --;
	degrees_in_ex[indeg_X[x] + exdeg_X[x]] --;
}

// a vertex v is removed from candidate list
bool quickM::remove_from_candidates(int &v, std::vector<int> & removed_vertices) {
	if (is_in_current_quasi_clique[v] == false && is_candidate[v] == false) return true; //nor in X neither in cand X means it's already been removed from graph
	if (is_in_current_quasi_clique[v] == true) return false;
	bool bad = false;
	int begID = 0, endID = 0;
	Q[endID++] = v;
	while (begID < endID) {
		int x = Q[begID++];
		if (is_candidate[x] == true) {
			removed_vertices.push_back(x);
			is_candidate[x] = false; // remove from G
			int id = idx_in_candidate[x];
			next_id_candidate[prev_id_candidate[id]] = next_id_candidate[id];
			prev_id_candidate[next_id_candidate[id]] = prev_id_candidate[id];
			sz_cand_X--;
			for (int i = 0; i < (int)adj[x].size(); i++) {
				int y = adj[x][i];
				exdeg_X[y] --; // as x has been in the candidate ext list
				if (is_in_current_quasi_clique[y] == true || is_candidate[y] == true) {
					if (is_in_current_quasi_clique[y] == true) {
						int preDeg = indeg_X[y] + exdeg_X[y] + 1;
						degrees_in_ex[preDeg]--;
						if (preDeg == current_min_exIn_degree_X)
							current_min_exIn_degree_X--;
						degrees_in_ex[preDeg - 1]++;
					}
					if (indeg_X[y] + exdeg_X[y] < min_degree) {
						Q[endID++] = y; // if the degree just right now dropped to min_degree - 1
						if (is_in_current_quasi_clique[y] == true) bad = true;
					}
				}
			}
			if (bad == true) return false;
		}
	}
	return true;
}

// we insert back all vertices in "removed_vertices" to our candidate list
void quickM::reutrn_back_to_candidates(std::vector<int>& removed_vertices) { // we have the indices of removed vertices, meaning the vertices from null to the graph
	for (int r = 0; r < (int)removed_vertices.size(); r++) {
		int x = removed_vertices[r];
		for (int i = 0; i < (int)adj[x].size(); i++) {
			int y = adj[x][i];
			exdeg_X[y] ++;
			if (is_in_current_quasi_clique[y] == true) {
				int preDeg = indeg_X[y] + exdeg_X[y] - 1;
				degrees_in_ex[preDeg]--;
				if (preDeg == current_min_exIn_degree_X && degrees_in_ex[preDeg] == 0)
					current_min_exIn_degree_X++;
				degrees_in_ex[preDeg + 1]++;
			}
		}
		is_candidate[x] = true;
		sz_cand_X++;
		int id = idx_in_candidate[x];
		next_id_candidate[prev_id_candidate[id]] = id;
		prev_id_candidate[next_id_candidate[id]] = id;
	}
}

// a vertex x is inserted to a candidate list
void quickM::insert_to_candidate_from_nowhere(int &x) {
	if (is_candidate[x] == true) return;
	is_candidate[x] = true; is_in_current_quasi_clique[x] = false;
	sz_cand_X++;
	candidate_set[end_idx_cand_X] = x;
	idx_in_candidate[x] = end_idx_cand_X;

	prev_id_candidate[end_idx_cand_X] = end_idx_cand_X - 1;
	next_id_candidate[end_idx_cand_X] = end_idx_cand_X + 1;
	end_idx_cand_X++;

	for (int i = 0; i < (int)adj[x].size(); i++) {
		int y = adj[x][i];
		exdeg_X[y] ++;
		if (is_in_current_quasi_clique[y] == true) {
			int preDeg = indeg_X[y] + exdeg_X[y] - 1;
			degrees_in_ex[preDeg]--;
			if (preDeg == current_min_exIn_degree_X && degrees_in_ex[preDeg] == 0)
				current_min_exIn_degree_X++;
			degrees_in_ex[preDeg + 1]++;
		}
	}
}

// Here, we remove a vertex from candidate list
void quickM::remove_from_candX_to_null(int &x) {// remove it from G !
	if (is_candidate[x] == false) return;
	is_candidate[x] = is_in_current_quasi_clique[x] = false;
	sz_cand_X--;
	for (int i = 0; i < (int)adj[x].size(); i++) {
		int y = adj[x][i];
		exdeg_X[y] --;
		if (is_in_current_quasi_clique[y] == true) {
			int preDeg = indeg_X[y] + exdeg_X[y] + 1;
			degrees_in_ex[preDeg]--;
			if (preDeg == current_min_exIn_degree_X)
				current_min_exIn_degree_X--;
			degrees_in_ex[preDeg - 1]++;
		}
	}
}

void quickM::add_to_in_common(int &z, int &idx, int &sz_set, std::set<int> &res, std::vector<int> &all) {
	if (in_common[z] == idx) {
		in_common[z] ++;
		all.push_back(z);
		if (in_common[z] == sz_set) {
			res.insert(z);
		}
	}
}

// this function finds a list nodes that are in two hops of all vertices in Set
void quickM::get_two_hop_neighbors(std::vector<int> &Set, std::set<int> &res) { // intersection of two hop neighborhood of vertices in Set
	int idx = 0;
	std::vector <int> all;
	int sz_set = (int)Set.size();
	for (auto x : Set) {
		for (int i = 0; i < (int)adj[x].size(); i++) {
			int y = adj[x][i];
			for (int j = 0; j < (int)adj[y].size(); j++) {
				int z = adj[y][j];
				add_to_in_common(z, idx, sz_set, res, all);
			}
			add_to_in_common(y, idx, sz_set, res, all);
		}
		idx++;
	}
	for (auto node : all) 
		in_common[node] = 0;// all cells of inCommon should become zero for the next use
}

void quickM::add_to_in_common(int &z, int &idx, int sz_set, std::vector<int> &res, std::unordered_set<int> &all) {
	if (in_common[z] == idx) {
		in_common[z] ++;
		all.insert(z);
		if (in_common[z] == sz_set) {
			res.push_back(z);
		}
	}
}

// find two neighbor hood of a vertex x
void quickM::get_two_hop_neighbors(int &x, std::vector <int> &res) { // intersection of two hop neighborhood of vertices in Set
	int idx = 0;
	std::unordered_set <int> all;
	for (int i = 0; i < (int)adj[x].size(); i++) {
		int y = adj[x][i];
		for (int j = 0; j < (int)adj[y].size(); j++) {
			int z = adj[y][j];
			add_to_in_common(z, idx, 1, res, all);
		}
		add_to_in_common(y, idx, 1, res, all);
	}
	// all cells of inCommon should become zero for the next use
	for (std::unordered_set <int> ::iterator iter = all.begin(); iter != all.end(); iter++) 
		in_common[*iter] = 0;
	std::sort(res.begin(), res.end());
}

bool quickM::prune_min_degree(int &x, std::vector <int> &rem) {
	if (exdeg_X[x] + indeg_X[x] < min_degree) {
		if (remove_from_candidates(x, rem) == false) return false;
	}
	return true;
}

// We remove vertices from candidate list based on the diameter threshold, for more details on this pruning technique, please look at quick algorithm's paper
bool quickM::prune_diam(int &x, std::vector <int> & rem) { // two hop neighborhood
	if (is_two_hop_neighborhood[x] == false) {
		is_two_hop_neighborhood[x] = true;
		two_hop_neighbors[x].clear();
		for (int i = 0; i < (int)adj[x].size(); i++) {
			int y = adj[x][i];
			for (int j = 0; j < (int)adj[y].size(); j++) {
				int z = adj[y][j];
				if (in_the_search_space[z]) two_hop_neighbors[x].insert(z);
			}
			if (in_the_search_space[y]) two_hop_neighbors[x].insert(y);
		}
	}
	for (int iter = beg_idx_cand_X; iter < end_idx_cand_X; iter = next_id_candidate[iter]) {
		int y = candidate_set[iter];
		if (is_candidate[y] && two_hop_neighbors[x].find(y) == two_hop_neighbors[x].end()) {
			if (remove_from_candidates(y, rem) == false) return false;
		}
	}
	return true;
}

// If a vertex must be added to a quasi clique we call it critical vertex. This function performs pruning based on this definition.
void quickM::prune_critical(int &x, std::vector<int> &new_in_X) {
	for (int i = 0; i < (int)adj[x].size(); i++) {
		int y = adj[x][i];
		if (is_candidate[y] == true && should_be_in_X[y] == false) {
			should_be_in_X[y] = true;
			new_in_X.push_back(y);
		}
	}
}

// Pruning based on the lower bound and upper bounds on the number of vertices of the candidate list
bool quickM::prune_lowerUpperbounds(std::vector <int> &rem, std::vector <int> &new_in_X) {
	bool change = true;
	int lx = -1, ux = -1, pre_lx;
	while (change && sz_cand_X > 0) {
		change = false;
		upper_bound(), lower_bound();
		if (L_X > U_X) return false;
		pre_lx = lx;
		lx = ceiling(gamma * (idx_X + L_X - 1));
		ux = ceiling(gamma * (idx_X + U_X - 1));
		int min_deg_in_X = current_min_exIn_degree_X;
		if (min_deg_in_X < lx) return false;
		if (current_min_degree_X + U_X < ux)	return false;
		if (pre_lx != lx && min_deg_in_X == lx) { // there is critical vertex
			for (int i = 0; i < idx_X; i++) {
				int v = X[i];
				if (exdeg_X[v] > 0 && indeg_X[v] + exdeg_X[v] == lx) {
					prune_critical(v, new_in_X);
				}
			}
		}
		idx_aux = 0;
		for (int iter = beg_idx_cand_X; iter < end_idx_cand_X; iter = next_id_candidate[iter]) {
			int v = candidate_set[iter];
			if (is_candidate[v] == true) {
				if (indeg_X[v] + exdeg_X[v] < ceiling(gamma * (idx_X + exdeg_X[v]))
					|| indeg_X[v] + U_X - 1 < ux
					|| indeg_X[v] + exdeg_X[v] < lx
					|| indeg_X[v] + exdeg_X[v] < min_degree
					) {
					if (remove_from_candidates(v, rem) == false) return false;
					change = true;
				}
			}
		}
	}
	return true;
}

// When we find a quasi clique, we keep it here.
void quickM::get_quasi_clique() {
	std::set <int> curQ;
	for (int i = 0; i < idx_X; i++) { curQ.insert(X[i]); }
	this->all_quasi_cliques.insert(make_pair(-((int)curQ.size()), curQ));
	if (LIMIT_ON_SIZE < (int)all_quasi_cliques.size()) {
		all_quasi_cliques.erase(prev(all_quasi_cliques.end()));
	}
}

// This is the recursive function using backtracking method to explore the search space and extract quasi cliques.
void quickM::back_track() {
	if ((idx_X + sz_cand_X < min_size) || (sz_cand_X == 0))
		return;

	//add current vertex "v" to C 
	int pre_begId_cand_X = beg_idx_cand_X;
	while (is_candidate[candidate_set[beg_idx_cand_X]] == false) 
		beg_idx_cand_X = next_id_candidate[beg_idx_cand_X];
	int v = candidate_set[beg_idx_cand_X];
	this->beg_idx_cand_X = next_id_candidate[beg_idx_cand_X];

	// current min degree maintains the minimum degree inside X
	int fixed_current_min_degree = current_min_degree_X;
	int fixed_current_min_exIn_degree_X = current_min_exIn_degree_X;
	bool isPossibleToExtendX;

	int fixed_min_degree = min_degree;
	int fixed_min_size = min_size;
	if (indeg_X[v] + exdeg_X[v] >= min_degree) {

		this->insert_to_quasi_clique_from_candidates(v);
		std::vector<int> rem;

		if (is_quasi_clique()) {
			quasi_cliques++;
			get_quasi_clique();
		}

		isPossibleToExtendX = prune_diam(v, rem);
		std::vector <int> new_in_X;
		if (isPossibleToExtendX == true)
			isPossibleToExtendX = this->prune_lowerUpperbounds(rem, new_in_X);

		if (isPossibleToExtendX == true)
			back_track();

		for (auto x : new_in_X) 
			this->should_be_in_X[x] = false;
		this->reutrn_back_to_candidates(rem);
		this->remove_from_quasi_clique_return_back_to_candidates(v);
	}

	this->beg_idx_cand_X--;
	this->current_min_degree_X = fixed_current_min_degree;
	this->min_degree = fixed_min_degree;
	this->min_size = fixed_min_size;
	this->current_min_exIn_degree_X = fixed_current_min_exIn_degree_X;

	//move backward one step
	//do not add v to X (meaning remove it from G)
	if (this->should_be_in_X[v] == false) {
		std::vector <int> remB;
		isPossibleToExtendX = remove_from_candidates(v, remB);
		std::vector <int> new_in_X;
		if (isPossibleToExtendX == true)
			isPossibleToExtendX = this->prune_lowerUpperbounds(remB, new_in_X);

		if (isPossibleToExtendX == true)
			back_track();

		for (auto x : new_in_X) 
			this->should_be_in_X[x] = false;
		this->reutrn_back_to_candidates(remB);
		this->beg_idx_cand_X--;
	}
	this->beg_idx_cand_X = pre_begId_cand_X;
}

// This function starts off with one vertex and tries to expand it to find all quasi cliques containing the starting vertex.
void quickM::extract_all_quasi_cliques() {

	int fixed_min_degree = this->ceiling(this->gamma * (this->min_size - 1));
	int fixed_min_size = this->min_size;

	for (int start_node = 0; start_node < this->V ; start_node++) {
		std::vector <int> two_hop_neighbors;

		this->get_two_hop_neighbors(start_node, two_hop_neighbors);

		this->idx_X = 0;
		this->sum_indeg_X = 0;
		this->current_min_exIn_degree_X = INF;
		this->current_min_degree_X = INF;
		this->beg_idx_cand_X = 1;
		this->end_idx_cand_X = 1;
		this->sz_cand_X = 0;

		for (auto neighbor : two_hop_neighbors) if (neighbor > start_node) {
			this->insert_to_candidate_from_nowhere(neighbor);
			this->in_the_search_space[neighbor] = true;
		}
		this->insert_to_candidate_from_nowhere(start_node);
		this->in_the_search_space[start_node] = true;
		this->insert_to_quasi_clique_from_candidates(start_node);

		this->L_X = min_size - idx_X;
		this->U_X = sz_cand_X;

		std::vector <int> rem;
		bool chk = prune_min_degree(start_node, rem);

		for (auto neighbor : two_hop_neighbors) {
			if (!chk) break;
			chk = prune_min_degree(neighbor, rem);
		}


		if (chk == true) {
			if (this->is_quasi_clique()) {
				get_quasi_clique();
				this->quasi_cliques++;
			}
			this->back_track();
		}

		min_degree = fixed_min_degree;
		min_size = fixed_min_size;

		this->remove_from_quasi_clique_return_back_to_candidates(start_node);
		this->remove_from_candX_to_null(start_node);
		this->is_two_hop_neighborhood[start_node] = false;
		this->in_the_search_space[start_node] = false;

		for (auto neighbor : two_hop_neighbors) {
			this->remove_from_candX_to_null(neighbor);
			is_two_hop_neighborhood[neighbor] = false;
			in_the_search_space[neighbor] = false;
		}
	}
	return;
}

// When we have a couple of vertices, we can expand the subgraph and find all quasi cliques containing the subgraph using this fucntion.
void quickM::expand(std::vector <int> &cur_quasi_clique) {
	std::set <int> cand_x;
	this->get_two_hop_neighbors(cur_quasi_clique, cand_x);
	int fixed_min_size = min_size;
	int fixed_min_degree = min_degree;

	this->sum_indeg_X = 0;
	this->current_min_exIn_degree_X = current_min_degree_X = INF ;
	this->beg_idx_cand_X = 1, this->end_idx_cand_X = 1, this->sz_cand_X = 0;

	min_size = (int)cur_quasi_clique.size();
	min_degree = this->ceiling(this->gamma * (this->min_size - 1));

	for (auto node : cur_quasi_clique) {
		this->insert_to_candidate_from_nowhere(node);
		this->insert_to_quasi_clique_from_candidates(node);
		this->in_the_search_space[node] = true;
	}

	for (auto node : cand_x) {
		if (is_in_current_quasi_clique[node] == false) {
			this->insert_to_candidate_from_nowhere(node);
			this->in_the_search_space[node] = true;
		}
	}

	if (this->is_quasi_clique()) {
		get_quasi_clique();
	}

	this->back_track();

	for (auto node : cand_x) {
		if (this->is_in_current_quasi_clique[node] == false) {
			this->remove_from_candX_to_null(node);
			this->is_two_hop_neighborhood[node] = false;
			this->in_the_search_space[node] = false;
		}
	}

	for (auto node : cur_quasi_clique) {
		this->remove_from_quasi_clique_return_back_to_candidates(node);
		this->remove_from_candX_to_null(node);
		this->is_two_hop_neighborhood[node] = false;
		this->in_the_search_space[node] = false;
	}

	this->min_degree = fixed_min_degree;
	this->min_size = fixed_min_size;
}

std::set < std::pair < int, std::set <int> > > quickM::get_all_quasi_cliques() {
	return this->all_quasi_cliques; 
}

