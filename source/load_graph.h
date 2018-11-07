/**
@author Seyed-Vahid Sanei-Mehri
Email contact: vas@iastate.edu
*/

#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <cstdio>
#include <vector>
#include <cstdlib>
#include <string>
#include <string.h>
#include <sstream>
#include <unordered_set>
#include <set>
#include <unordered_map>
#include <algorithm>
#include <math.h>
#include <random>
#include <time.h>
#include <assert.h>


#define mmin(x, y) ((x)<(y)?(x):(y))
#define mmax(x, y) ((x)>(y)?(x):(y))


class load_graph {
public:
	load_graph();
	~load_graph();
public:
	void start();
	char input_address[2000], output_address[2000];
	double gamma;
	double gamma_prime;
	double k;
	double k_prime;
	int min_size;
	int num_edges();
	int num_vertices();
	int maximum_degree();
	std::vector < std::vector < int > > adj;
	std::unordered_map < int, int > map_idx_to_vertex;
private:
	void read_graph();
	std::vector < std::pair <int, int> > edges;
	std::unordered_map < int, int > map_vertex_to_idx;
	int E;
	int V;
	int max_degree;
	int to_int(std::string &);
	bool all_num(std::string &);
	void add_vertex(int &);
};


