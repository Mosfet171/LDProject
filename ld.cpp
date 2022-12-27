#include <stdlib.h>
#include <stdio.h>
#include <array>
#include <random>
#include <algorithm>
#include <iterator>
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

float run_execution_on_graph(int* graph[], int N, int n_gen, float payoff_matrix[2][2]) {
	unsigned int seed = 0;
	int population[N];
	int next_population[N];
	int payoffs[N];
	int full_sums[n_gen];
	

	for (int i = 0; i<N; i++) {
		population[i] = i < 50 ? 0 : 1;
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
			len_neighbours = *(&graph[j] + 1) - graph[j];
			for (int k = 0; k<len_neighbours; k++) {
				payoffs[j] = payoffs[j] + payoff_matrix[population[j]][population[graph[j][k]]];
			}
		}// End population

		// Second loop over population
		for (int j = 0; j<N; j++) {
			len_neighbours = *(&graph[j] + 1) - graph[j];
			int random_neighbour_id = rand() % len_neighbours;
			int random_neighbour_strat = graph[j][random_neighbour_id];
			int px = payoffs[j]; int py = payoffs[random_neighbour_strat];
			int ky = *(&graph[random_neighbour_strat] + 1) - graph[random_neighbour_strat];
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

		int sum = 0;
		for (int k = 0; k<N; k++) {
			population[k] = next_population[k];
			sum = sum + population[k];
		}
		full_sums[i] = sum;
	} // End outermost
	int thesum = 0;
	for (int i=0;i<n_gen;i++) {
		thesum = thesum + full_sums[i];
	}
	return thesum/n_gen;
}

float exec_with_fixed_params(float P, float T, float S, float R, int n_real, int n_runs, int N, int n_gen) {
	float payoff_matrix[2][2] = {{P,T},{S,R}};
	float sums_real_av;
	float sums_av = 0;

	for (int i=0; i<n_real; i++) {
		int* graph[N];
		retrieve_graph(i, graph)
		for (int j=0; j<n_runs; j++) {
			float av_sum = run_execution_on_graph(graph, N, n_gen, payoff_matrix);
			sums_av = sums_av + av_sum;
		}
		sums_av = sums_av/n_runs;
		sums_real_av = sums_real_av + sums_av;
	}
	sums_real_av = sums_real_av/n_real
	return sums_real_av;
}

void retrieve_graph(int i, int* graph[]) {
	
}

int main() {
	printf("%d",doIreplace(1,2,3,4,5,6));
}