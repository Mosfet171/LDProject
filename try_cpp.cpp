#include <stdlib.h>
#include <stdio.h>


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

int run_execution_on_graph(array)

int main() {
	printf("%d",doIreplace(1,2,3,4,5,6));
}