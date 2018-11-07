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
#include <set>
#include <unordered_map>
#include <algorithm>
#include <math.h>
#include <random>
#include <time.h>
#include <assert.h>


#define mmin(x, y) ((x)<(y)?(x):(y))
#define mmax(x, y) ((x)>(y)?(x):(y))


class list_top_k {
public:
	list_top_k();
	~list_top_k();
	std::vector < std::pair<int, std::set<int> > > extract_top_k(int k, std::set < std::pair < int, std::set <int> > >& quasi_cliques);
private:
	bool is_excluded(std::set <int>&, std::set < std::pair <int, std::set <int> > >&);
};


