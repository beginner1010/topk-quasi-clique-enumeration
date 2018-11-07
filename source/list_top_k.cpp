/**
@author Seyed-Vahid Sanei-Mehri
Email contact: vas@iastate.edu
*/

#include "list_top_k.h"

list_top_k::list_top_k() {}
list_top_k::~list_top_k() {}

// This function determines if a current quasi-clique is included in any previously seen quasi clique.
bool list_top_k::is_excluded(std::set <int>& current_quasi_clique, std::set < std::pair <int, std::set <int> > > &quasi_cliques) {
	for (auto pair : quasi_cliques) {
		std::set<int> top = pair.second;
		bool ok = false;
		if (current_quasi_clique.size() >= top.size())
			ok = true;
		else {
			for (auto node : current_quasi_clique) {
				if (top.find(node) == top.end()) {
					ok = true;
					break;
				}
			}
		}
		if (ok == false) {
			return false;
		}
	}
	return true;
}

// Here, we extract top-k quasi cliques which are the k largest maximal quasi cliques among all quasi cliques, i.e. among quasi cliques in "quasi_cliques"
std::vector < std::pair<int, std::set<int> > > list_top_k::extract_top_k(int top_k, std::set < std::pair < int, std::set <int> > >& quasi_cliques) {
	int cnt = 0;
	std::set< std::pair< int, std::set<int> > > top_k_set;
	std::vector < std::pair<int, std::set<int> > > res;
	for (auto p : quasi_cliques) {
		std::set <int> cur_quasi_clique = p.second;
		if (this->is_excluded(cur_quasi_clique, top_k_set) == false) 
			continue;
		top_k_set.insert(make_pair(-((int)cur_quasi_clique.size()), cur_quasi_clique));
		res.push_back(make_pair ((int)cur_quasi_clique.size(), cur_quasi_clique));
		cnt++;
		if (cnt == top_k) 
			break;
	}
	return res;
}