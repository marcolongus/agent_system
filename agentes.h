#include "classparticle.h"
#define forn(i,a,b) for(int i=a; i<b; i++)

using namespace std;


void
init_system( vector<particle>            &system       ,  
			 vector<size_t>              &state_vector , 
			 vector<vector<set<size_t>>> &grid            )
{
	for(int p = 0; p < N; p++){
		particle Agent;
		bool accepted = false;
		while(!accepted){
			accepted = true;
			Agent = create_particle();
			int i_index = floor(Agent.x),
				j_index = floor(Agent.y);
				//Si interactúa con otra partícula cambiamos la condición a no aceptada.
			forn(l,-2,3){
				forn(m,-2,3){
					int i = b_condition(i_index + l),
						j = b_condition(j_index + m);
					if (!grid[i][j].empty()){
						for (auto element: grid[i][j]){
							if (interact(Agent,system[element])) accepted = false;
						}//for auto
					}//if not empty
				}//for m
			}//for l
			if (accepted) grid[i_index][j_index].insert(p);
		}//while
		system.push_back(Agent);
		state_vector[Agent.get_state()]++;
	}//for N	
}


void 
update_system(  vector<particle>            &system      , 
				vector<particle>            &system_new  ,
				vector<size_t>              &state_vector, 
				vector<vector<set<size_t>>> &grid         ,
				vector<bool>                &inter       ,
				int                         &TimeStep     ,
				ofstream                    &anim          )
{
	size_t healthy=0, infected=0, refract=0;
	state_vector = {0UL,0UL,0UL};
	#pragma omp parallel for schedule(guided) reduction(+:healthy,infected,refract) num_threads(12)
	for (size_t p=0; p<N; p++){
			vector<int> index;
			index.push_back(p);
			inter[p] = false;
			/*chequeamos interacciones*/
			forn(l,-2,3) forn(m,-2,3){
				size_t i_index = b_condition(floor(system[p].x)+l),
					   j_index = b_condition(floor(system[p].y)+m);
				if(!grid[i_index][j_index].empty()){
					for(auto element: grid[i_index][j_index]){
						if (element !=p && interact(system[p],system[element])){
							inter[p] = true;
							index.push_back(element);
						}
					}//for
				}//if not empty
			} //for m, l
			/*fin de chequeo de interacciones*/
			system_new[p] = evolution(system, index, inter[p]);
			switch(system_new[p].get_state()){
				case(0):
					healthy++;
					break;
				case(1):
					infected++;
					break;
				default:
				refract++;
					break;
			}
		}//for p
		state_vector = {healthy,infected,refract};
		//Animacion:
		if (animation and TimeStep % anim_step == 0){
			for(size_t p=0; p < N; p++){
				anim << system_new[p].x           << " ";
				anim << system_new[p].y           << " ";
				anim << TimeStep*delta_time       << " ";
				anim << system_new[p].get_state() << endl;
			}
		}//if animacion
		/*Estabilzamos el set*/

		for(size_t p=0; p<N; p++){
			int i_new = floor(system_new[p].x),
				j_new = floor(system_new[p].y);
			int i_old = floor(system[p].x),
				j_old = floor(system[p].y);

			if (grid[i_new][j_new].find(p) == grid[i_new][j_new].end()){
				grid[i_old][j_old].erase(p);
				grid[i_new][j_new].insert(p);
			}//if
		}//cirra el for p set.
		system = system_new;
}



//Print functiones

void print_header(int n_simulaciones)
{
	cout << "--------------------------------------------------------"   << endl;
	cout << "Simulacion: " << n_simulaciones << endl;
	cout << "--------------------------------------------------------"   << endl;
	cout << endl;
}


void print_mem_info(void)
{
	int   system_memory  = 2*(5*sizeof(KIND)*N);
	float space_memory   = 48.f*L*L;
	cout << "Memoria del sistema      : " << system_memory                 << " Bytes" << endl;
	cout << "Memoria del sistema      : " << (float)system_memory/1000000. << " Mb"    << endl;  
	cout << "Memoria de una particula : " << (float)system_memory/(float)N << " Bytes" << endl;
	cout << "Memoria del espacio      : " <<  space_memory/1e06f     	   << " Mb" << endl;
	cout << "Memoria de un grid        : " <<  48                          << endl;  
}

void print_epidemic_tofile(ofstream &file, vector<size_t> &state_vector, int &TimeStep)
{
	file << state_vector[0] << " ";
        file << state_vector[1] << " ";
	file << state_vector[2] << " ";
	file << delta_time*(double)TimeStep << endl;
}
