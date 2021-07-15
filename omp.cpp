#include <bits/stdc++.h>

using namespace std;

int main(void){

	vector<int> state_vector = {0,0,0};
	int healthy  = 0;
	int refract  = 0;
	int infected = 0;
	#pragma omp parallel for schedule(dynamic) num_threads(12) reduction(+:healthy,refract,infected)
	for (int i = 0; i < 10000000; ++i) 
	{
		healthy++;
		refract++;
		infected++;
	}
	state_vector = {healthy,refract,infected};
	printf("results %i, %i, %i \n",healthy,refract,infected);

	return 0;
}