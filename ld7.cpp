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
//#include <pthread.h>
#include <thread>
//#include <bits/stdc++.h>

#define N 1000
#define T_MAX 2
#define T_MIN 0
#define S_MAX 1
#define S_MIN (-1)
#define N_THREADS 8

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

float run_execution_on_graph(short int graph[N][N], short int neighbours_sizes[], int n_gen, float payoff_matrix[2][2], int transient) {
	unsigned int seed = 0;
	int population[N];
	int next_population[N];
	int payoffs[N];
	int full_sums[n_gen];
	
	for (int i = 0; i<N; i++) {
		population[i] = i < N/2 ? 0 : 1;
	}
	for (int i = 0; i<5; i++) {
		shuffle(population, population+N, std::default_random_engine(seed));
	}
	for (int i = 0; i<N; i++) {
		next_population[i] = population[i];
	}

	// Outermost Loop: over generations
	for (int i = 0; i<n_gen; i++) {

		int len_neighbours; 
		// Loop over population
		for (int j = 0; j<N; j++) {
			payoffs[j] = 0;
			len_neighbours = neighbours_sizes[j];
			for (int k = 0; k<len_neighbours; k++) {
				payoffs[j] = payoffs[j] + payoff_matrix[population[j]][population[graph[j][k]]];
			}
		}// End population

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
				float prob = ((float)py-(float)px)/((float)kg*(float)Dg)*100;
				int random = (rand())%100;
				if (100*prob > random) {
					next_population[j] = population[random_neighbour_strat];
				}
			}
		}// End population

		if(transient == 0) { 
			int sum = 0;
			for (int k = 0; k<N; k++) {
				population[k] = next_population[k];
				sum = sum + population[k];
			}
			full_sums[i] = sum;
		}
	} // End outermost
 
 	if (transient == 0) {
		float thesum = 0;
		for (int i=0;i<n_gen;i++) {
			thesum = thesum + full_sums[i];
		}
		return thesum/n_gen;
	} else {
		return 0;
	}
}

void retrieve_graph(int i, short int graph[N][N], short int neighbours_sizes[]) {
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
		//short int subgraph[n_neighbours];
		for(int j=0;j<content[i].size();j++) {
			//subgraph[j] = stoi(content[i][j]);
			//graph[i] = subgraph;
			graph[i][j] = stoi(content[i][j]);
		}
	}
}

float exec_with_fixed_params(float P, float T, float S, float R, int n_real, int n_runs, int n_gen, int n_transient) {
	float payoff_matrix[2][2] = {{P,T},{S,R}};
	float sums_real_av = 0;
	float sums_av = 0;

	for (int i=0; i<n_real; i++) {
		short int graph[N][N];
		short int neighbours_sizes[N];
		retrieve_graph(i, graph, neighbours_sizes);

		for (int j=0; j<n_runs; j++) {
			run_execution_on_graph(graph, neighbours_sizes, n_transient, payoff_matrix, 1);
			float av_sum = run_execution_on_graph(graph, neighbours_sizes, n_gen, payoff_matrix, 0);
			sums_av = sums_av + av_sum;
		}
		sums_av = sums_av/n_runs;
		sums_real_av = sums_real_av + sums_av;
		sums_av = 0;
	}
	sums_real_av = sums_real_av/n_real;
	return sums_real_av; 
}

void exec_on_thread(int tid) {
	int P = 0; int R = 1; float T = 1; float S = -1; 
	int n_real = 10; int n_runs = 10;
	int n_transient = 1000;
	int n_gen = 1000; 
	int granularity = 8;

	/*
	int* pptid = (int*) p_tid;
	int tid = *pptid;
	*/

	// Loop Over S's

	for (int i=tid*(granularity/N_THREADS);i<(tid+1)*(granularity/N_THREADS);i++) {
		float S = (S_MAX-S_MIN)/(float)granularity * (float)i + (float)S_MIN;
		cout << "[Thread " << tid << "] ---------- S = " << S << " ---------- #" << endl;

		string outname = "out_S_" + to_string(i) + ".txt";
		fstream outfile (outname, ios::app);
		// Loop over T's
		for (int j=0;j<granularity;j++) {
			float T = (T_MAX-T_MIN)/(float)granularity * (float)i + (float)T_MIN;
			cout << "[Thread " << tid << "] ----- T = " << T << " ----- #" << endl;
			float thesum = exec_with_fixed_params(P,T,S,R,n_real,n_runs,n_gen,n_transient);
			cout << "[Thread " << tid << "] Sum: " << thesum << endl;
			outfile << thesum << endl;
		}
	}	
}

void progressBar(float progress) {
    int barWidth = 48;

    /*
    cout << "[";
    int pos = barWidth * progress;
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    */
    std::cout << int(progress * 100.0) << " %\r";
    cout.flush();
    

}



int main(int argc, char** argv) {
	int P = 0; int R = 1; 
	int n_real = 10; int n_runs = 10;
	int n_transient = stoi(argv[1]);
	int n_gen = stoi(argv[2]);
	int granularity = 64;

	int tid = 7;

	// Loop Over S's

	for (int i=tid*(granularity/N_THREADS);i<(tid+1)*(granularity/N_THREADS);i++) {
		float S = ((float)S_MAX-(float)S_MIN)/(float)granularity * (float)i + (float)S_MIN;
		cout << "[Thread " << tid << "] ---------- S = " << S << " ---------- #" << endl;

		string outname = "out" + to_string(stoi(argv[3])) + "/out_S_" + to_string(i) + ".txt";
		fstream outfile (outname, ios::app);
		// Loop over T's
		for (int j=0;j<granularity;j++) {
			float T = ((float)T_MAX-(float)T_MIN)/(float)granularity * (float)j + (float)T_MIN;
			progressBar((float)j/(float)granularity);
			//cout << "[Thread " << tid << "] ----- T = " << T << " ----- #" << endl;
			float thesum = exec_with_fixed_params(P,T,S,R,n_real,n_runs,n_gen,n_transient);
			outfile << thesum << endl;
		}
	}	
	
	
	return 0;

}