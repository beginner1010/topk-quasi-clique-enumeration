/**
@author Seyed-Vahid Sanei-Mehri
Email contact: vas@iastate.edu
*/

#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include "load_graph.h"
#include "quickM.h"
#include "list_top_k.h"

void print_quasi_cliques(std::vector< std::pair< int, std::set<int> > > quasi_cliques, load_graph& G) {
	freopen(G.output_address, "w", stdout);
	int cnt = 1;
	for (auto pair : quasi_cliques) {
		auto quasi_clique = pair.second;
		printf("%d. size = %d,\t", cnt++, (int)quasi_clique.size());
		bool flg = true;
		printf("Vertex set = {");
		for (auto node : quasi_clique) {
			if (flg == false) printf(", ");
			flg = false;
			printf("%d", G.map_idx_to_vertex[node]);
		}
		printf("}\n");
	}
}

int main() {
	std::ios::sync_with_stdio(false);

	load_graph* graph = new load_graph();
	graph->start();

	// quickm extracts quasi cliques with the parameter gamma_prime
	quickM* quickm = new quickM(*graph, graph->gamma_prime);
	quickm->extract_all_quasi_cliques();
	auto all_gamma_prime_quasi_cliques = quickm->get_all_quasi_cliques();
	list_top_k* top_k_prime = new list_top_k();
	auto top_k_gamma_prime_quasi_cliques = top_k_prime->extract_top_k(graph->k_prime, all_gamma_prime_quasi_cliques);

	// quickm_2 uses gamma
	quickM* quickm_2 = new quickM(*graph, graph->gamma);
	for (auto pair : top_k_gamma_prime_quasi_cliques) {
		auto quasi_clique = std::vector<int> (pair.second.begin(), pair.second.end());
		quickm_2->expand(quasi_clique);
	}
	auto gamma_quasi_cliques = quickm_2->get_all_quasi_cliques();
	
	list_top_k* top_k = new list_top_k();
	auto top_k_gamma_quasi_cliques = top_k->extract_top_k(graph->k, gamma_quasi_cliques);

	print_quasi_cliques(top_k_gamma_quasi_cliques, *graph);
	return 0;
}
