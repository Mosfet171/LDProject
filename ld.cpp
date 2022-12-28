#include <stdlib.h>
#include <stdio.h>
#include <array>
#include <random>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
//#include <bits/stdc++.h>

using namespace std;

int doIreplace(int px, int py, int kx, int ky, float T, float S) {
	if (px < py) {
		int kg = kx > ky ? kx : ky;
		int maxT = T > 1 ? T : 1;
		int minS = S < 0 ? S : 0;
		int Dg = maxT - minS;
		float prob = (py-px)/(kg*Dg);
		int random = (rand())%100;
		if (100*prob > random) {
			return(1);
		}
	}
	return(0);
}

float run_execution_on_graph(short int* graph[], short int neighbours_sizes[], int N, int n_gen, float payoff_matrix[2][2]) {
	unsigned int seed = 0;
	int population[N];
	int next_population[N];
	int payoffs[N];
	int full_sums[n_gen];
	
	cout << "Creating Population" << endl;
	for (int i = 0; i<N; i++) {
		population[i] = i < N/2 ? 0 : 1;
	}
	for (int i = 0; i<5; i++) {
		shuffle(population, population+N, std::default_random_engine(seed));
	}
	for (int i = 0; i<N; i++) {
		next_population[i] = population[i];
		cout<<population[i];
	}
	cout <<endl;

	cout << "Starting loop over generations" << endl; 
	// Outermost Loop: over generations
	for (int i = 0; i<n_gen; i++) {

		int len_neighbours; 
		cout << "Starting loop over population" << endl; 
		// Loop over population
		for (int j = 0; j<N; j++) {
			payoffs[j] = 0;
			len_neighbours = neighbours_sizes[j];
			cout << "Individual: " << j << ", n_neighbours: " << len_neighbours <<endl;
			cout << "Starting loop over neighbours" << endl; 
			for (int k = 0; k<len_neighbours; k++) {
				cout << "payoffsj: " <<payoffs[j] <<endl;
				cout<<"populationj: "<<population[j]<<endl;
				cout<<"graphjk: "<<graph[j][k]<<endl;
				cout<<"populationgraphjk: "<<population[graph[j][k]]<<endl;
				payoffs[j] = payoffs[j] + payoff_matrix[population[j]][population[graph[j][k]]];
			}
		}// End population

		cout << "Starting second loop over population" << endl; 
		// Second loop over population
		for (int j = 0; j<N; j++) {
			len_neighbours = neighbours_sizes[j];
			int random_neighbour_id = rand() % len_neighbours;
			int random_neighbour_strat = graph[j][random_neighbour_id];
			int px = payoffs[j]; int py = payoffs[random_neighbour_strat];
			int ky = neighbours_sizes[random_neighbour_strat];
			int kx = len_neighbours;
			int S = payoff_matrix[1][0]; int T = payoff_matrix[0][1];
			// DO I REPLACE REPLACEMENT:
			if (px < py) {
				int kg = kx > ky ? kx : ky;
				int maxT = T > 1 ? T : 1;
				int minS = S < 0 ? S : 0;
				int Dg = maxT - minS;
				float prob = (py-px)/(kg*Dg);
				int random = (rand())%100;
				if (100*prob > random) {
					next_population[j] = population[random_neighbour_strat];
				}
			}
		}// End population

		cout << "Finalizing sums" << endl; 
		int sum = 0;
		for (int k = 0; k<N; k++) {
			population[k] = next_population[k];
			sum = sum + population[k];
		}
		full_sums[i] = sum;
	} // End outermost

	cout << "Finalizing last sum" << endl; 
	int thesum = 0;
	for (int i=0;i<n_gen;i++) {
		thesum = thesum + full_sums[i];
	}
	return thesum/n_gen;
}

void retrieve_graph(int i, short int* graph[], short int neighbours_sizes[]) {
	string fname = "py_graph_"+to_string(i)+".txt";
	fstream thefile (fname, ios::in);

	vector<vector<string> > content;
	vector<string> row;
	string line, word;

	if (thefile.is_open()) {
		while(getline(thefile, line)) {
			row.clear();
			stringstream str(line);
			while(getline(str, word, ',')) {
				row.push_back(word);
			}
			content.push_back(row);
		}
	} else {
		cout<<"Could not open the file\n";
	}

	for(int i=0;i<content.size();i++) {
		int n_neighbours = content[i].size();
		neighbours_sizes[i] = n_neighbours;
		int graph[i][n_neighbours];
		for(int j=0;j<content[i].size();j++) {
			graph[i][j] = stoi(content[i][j]);
			cout<<graph[i][j]<<" ";
		}
		cout<<""<<endl;
	}
}

float exec_with_fixed_params(float P, float T, float S, float R, int n_real, int n_runs, int N, int n_gen) {
	float payoff_matrix[2][2] = {{P,T},{S,R}};
	float sums_real_av;
	float sums_av = 0;

	for (int i=0; i<n_real; i++) {
		short int* graph[N];
		short int neighbours_sizes[N];
		retrieve_graph(i, graph, neighbours_sizes);

		cout<< "Neighbour sizes:"<<endl;
		for (int k=0;k<N;k++) {
			cout << neighbours_sizes[k] << endl;
		}


		for (int j=0; j<n_runs; j++) {
			float av_sum = run_execution_on_graph(graph, neighbours_sizes, N, n_gen, payoff_matrix);
			sums_av = sums_av + av_sum;
		}
		sums_av = sums_av/n_runs;
		sums_real_av = sums_real_av + sums_av;
	}
	sums_real_av = sums_real_av/n_real;
	return sums_real_av;
}



int main() {
	int P = 0; int R = 1; int T = 1; int S = 0; 
	int n_real = 1; int n_runs = 1;
	int n_gen = 10; int N = 10;

	string outname = "outtest1.txt";
	fstream outfile (outname, ios::out);

	float thesum = exec_with_fixed_params(P,T,S,R,n_real,n_runs,N,n_gen);

	outfile << thesum << endl;

}