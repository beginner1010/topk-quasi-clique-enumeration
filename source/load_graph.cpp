/*
	@author Seyed-Vahid Sanei-Mehri
	Email contact: vas@iastate.edu
*/

#include "load_graph.h"

load_graph::load_graph() {
	this->adj.clear();
}
load_graph::~load_graph() {}

void load_graph::start_exact() {
	std::cerr << " Insert the input (graph) file location" << std::endl;
	std::cerr << " >>> "; std::cin >> input_address;
	assert(strlen(input_address) < 2000);
	std::cerr << " Insert the output file" << std::endl;
	std::cerr << " >>> "; std::cin >> output_address;
	assert(strlen(output_address) < 2000);
	std::cerr << " Insert gamma value [0.5, 1)" << std::endl;
	std::cerr << " >>> "; std::cin >> gamma;
	assert(gamma >= 0.5 && gamma < 1);
	std::cerr << " Insert a positive integer k" << std::endl;
	std::cerr << " >>> "; std::cin >> k;
	assert(k > 0);
	std::cerr << " Insert a positive integer as minimum size threshold" << std::endl;
	std::cerr << " >>> "; std::cin >> min_size;
	assert(0 < min_size);
	std::cerr << " ---------------------------------------------------------------------------------------------------------------------- \n";
	std::cerr << " Preprocessing the graph (We ignore texts, self-loops, and multiple edges in the input file) ..." << std::endl;
	this->read_graph();
	fprintf(stderr, " There are %d edges, and %d vertices\n", this->E, this->V);
	freopen(output_address, "w", stdout);
	fflush(stdout), fflush(stderr);
}

void load_graph::start() {
	std::cerr << " Insert the input (graph) file location" << std::endl;
	std::cerr << " >>> "; std::cin >> input_address;
	assert(strlen(input_address) < 2000);
	std::cerr << " Insert the output file" << std::endl;
	std::cerr << " >>> "; std::cin >> output_address;
	assert(strlen(output_address) < 2000);
	std::cerr << " Insert gamma value [0.5, 1)" << std::endl;
	std::cerr << " >>> "; std::cin >> gamma;
	assert(gamma >= 0.5 && gamma < 1);
	std::cerr << " Insert gamma prime value (gamma, 1]" << std::endl;
	std::cerr << " >>> "; std::cin >> gamma_prime;
	assert(gamma < gamma_prime && gamma_prime <= 1);
	std::cerr << " Insert a positive integer k" << std::endl;
	std::cerr << " >>> "; std::cin >> k;
	assert(k > 0);
	std::cerr << " Insert a positive integer k_prime" << std::endl;
	std::cerr << " >>> "; std::cin >> k_prime;
	assert(k < k_prime);
	std::cerr << " Insert a positive integer as minimum size threshold" << std::endl;
	std::cerr << " >>> "; std::cin >> min_size;
	assert(0 < min_size);
	std::cerr << " ---------------------------------------------------------------------------------------------------------------------- \n";
	std::cerr << " Preprocessing the graph (We ignore texts, self-loops, and multiple edges in the input file) ..." << std::endl;
	this->read_graph();
	fprintf(stderr, " There are %d edges, and %d vertices\n", this->E, this->V);
	freopen(output_address, "w", stdout);
	fflush(stdout), fflush(stderr);
}

int load_graph::to_int(std::string &x) { std::stringstream aux; aux << x; int res; aux >> res; return res; }

bool load_graph::all_num(std::string &s) {
	for (int i = 0; i < (int)s.size(); i++) if ((s[i] >= '0' && s[i] <= '9') == false) return false;
	return true;
}

void load_graph::add_vertex(int &A) {
	if (this->map_vertex_to_idx.find(A) == map_vertex_to_idx.end()) {
		map_vertex_to_idx[A] = V;
		map_idx_to_vertex[V++] = A;
		this->adj.push_back(std::vector<int>());
	}
	A = map_vertex_to_idx[A];
}

// Here, we read the graph. The first two integers are admitted as the endpoints of an edge.
void load_graph::read_graph() {
	FILE *f_in;
	f_in = freopen(input_address, "r", stdin);

	std::string s;

	this->E = 0;
	this->V = 0;
	this->map_vertex_to_idx.clear();
	this->map_idx_to_vertex.clear();

	std::set < std::pair <int, int> > seen_edges;

	this->max_degree = 0;

	while (std::getline(std::cin, s)) {
		std::stringstream ss; ss << s;
		std::vector <std::string> vec_str;
		for (std::string z; ss >> z; vec_str.push_back(z));
		if (((int)vec_str.size()) >= 2) {
			bool is_all_num = true;
			for (int i = 0; i < mmin(2, (int)vec_str.size()); i++) is_all_num &= this->all_num(vec_str[i]);
			if (is_all_num) {
				int A = to_int(vec_str[0]), B = to_int(vec_str[1]);
				// ignore a self loop
				if (A == B)
					continue;

				this->add_vertex(A);
				this->add_vertex(B);

				// the graph is undirected
				if (A > B) 
					std::swap(A, B);

				// remove self loops or multiple edges 
				if (seen_edges.find(std::make_pair(A, B)) != seen_edges.end())
					continue;
				
				seen_edges.insert(std::make_pair(A, B));
				adj[A].push_back(B);
				adj[B].push_back(A);
				this->max_degree = mmax(this->max_degree, mmax((int)adj[A].size(), (int)adj[B].size()));
				E++;
			}
		}
	}
	fclose(f_in);
}

// Here, we read the graph. The first two integers are admitted as the endpoints of an edge.
void load_graph::read_graph_test() {
	std::string s;

	this->E = 0;
	this->V = 0;
	this->map_vertex_to_idx.clear();
	this->map_idx_to_vertex.clear();

	std::set < std::pair <int, int> > seen_edges;

	this->max_degree = 0;

	while (std::getline(std::cin, s)) {
		std::stringstream ss; ss << s;
		std::vector <std::string> vec_str;
		for (std::string z; ss >> z; vec_str.push_back(z));
		if (((int)vec_str.size()) >= 2) {
			bool is_all_num = true;
			for (int i = 0; i < mmin(2, (int)vec_str.size()); i++) is_all_num &= this->all_num(vec_str[i]);
			if (is_all_num) {
				int A = to_int(vec_str[0]), B = to_int(vec_str[1]);
				// ignore a self loop
				if (A == B)
					continue;

				this->add_vertex(A);
				this->add_vertex(B);

				// the graph is undirected
				if (A > B) 
					std::swap(A, B);

				// remove self loops or multiple edges 
				if (seen_edges.find(std::make_pair(A, B)) != seen_edges.end())
					continue;
				
				seen_edges.insert(std::make_pair(A, B));
				adj[A].push_back(B);
				adj[B].push_back(A);
				this->max_degree = mmax(this->max_degree, mmax((int)adj[A].size(), (int)adj[B].size()));
				E++;
			}
		}
	}
}


int load_graph::num_edges() {
	return this->E;
}

int load_graph::num_vertices() {
	return this->V;
}

int load_graph::maximum_degree() {
	return this->max_degree;
}
