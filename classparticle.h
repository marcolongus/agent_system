#include "parameters.h"    //Módulo con parametros para la simulación.
using namespace std;

//Generador de números aleatorios en (0,1).
mt19937::result_type seed = 1615996332;
mt19937 gen(seed);                             //Standard mersenne_twister_engine seeded time(0)
uniform_real_distribution<KIND> dis(0., 1.); // dis(gen), número aleatorio real entre 0 y 1.

/*Definimos la clase partículas y sus métodos */
/*Memoria que ocupa cada miembro de la clase: 4 doubbles (32 Bytes) + 1 int (4 Bytes) = 36 Bytes*/

class particle{
	private:
		//Estado interno del agente.
		int state;
	public:
		//Estado dinámico del agente.
		KIND x,y;
		KIND velocity;
		KIND angle;

	//Constuctores:
	particle();
	particle(KIND x1, KIND y1, KIND vel, KIND ang);

	//Métodos:
	bool is_healthy()    {return (state <  1);}
	bool is_infected()   {return (state == 1);}
	bool is_refractary() {return (state == 2);}

	int get_state()      {return state;}

	void set_healthy()    {state = 0;}
	void set_infected()   {state = 1;}
	void set_refractary() {state = 2;}
};


/***************************************************************************************/
/***************************************************************************************/

/* Constructor de partícula genérica*/
particle::particle(){
	velocity = 0;
	angle    = 0;
	x        = 0;
	y        = 0;
}

/*Constructor of a particle in a given phase-state (x,p) of the system */
particle::particle(KIND x1, KIND y1, KIND vel, KIND ang){
	velocity = vel;
	angle    = ang;
	x        = x1;
	y        = y1;
}


/***************************************************************************************/

/**
 * [Create a prticle in a random point (x, p, s) in the phase-state]
 * @return  [particle]
 */
particle create_particle(void){
	KIND x,y,velocity,angle;

	x     = dis(gen)*L;
	y     = dis(gen)*L;
	angle = dis(gen)*dos_Pi;

	//Tres distribuciones para asiganar la velocidad al azar:
	switch(velocity_distribution){
		case(0):
			velocity = -active_velocity*log(1. - dis(gen));
			break;
		case(1):
			velocity = pow( dis(gen)*( pow(v_max, 1- k_powerl) - pow(v_min,1-k_powerl))+pow(v_min,1-k_powerl), 1./(1.-k_powerl));
			break;
		default:
			velocity = active_velocity;
			break;
	}
	//Creación de la partícula:
	particle A(x,y,velocity,angle);
	//Setting del estado interno de la partícula:
	if   (dis(gen) < p_init){ A.set_infected();} //Agrega un porcentaje p_init de partículas infectadas.
	else A.set_healthy();
	//Agrega un pocentaje p_rinit de partículas en estado refractario.
	//No reasigna una previamente infectada.
	if (dis(gen) < p_rinit and !A.is_infected() ){ A.set_refractary();}
	return A;
}


/*FUNCIONES AUXILIARES PARA LA CLASE*/
/*Real boundary condition  and integer boundary condition functions*/
KIND b_condition(KIND a){
    return fmod((fmod(a,L)+L),L);
}

int my_mod(int a, int b){
	return ((a%b)+b)%b;
}

/**
 * [Distance between particles in a topological torus]
 * @param  A [particle]
 * @param  B [particle]
 * @return   [KIND distance between A and B]
 */
KIND distance(particle A, particle B){
        KIND x1,x2,y1,y2,res;
        res = infinity;
        x2 = B.x; y2 = B.y;
        for(int i=-1;i<2;i++) for(int j=-1;j<2;j++){
            x1 = A.x + i*L;
            y1 = A.y + j*L;
            res = min(res, (KIND)pow((x1-x2),2) + (KIND)pow((y1-y2),2));
        }
        return sqrt(res);
}

/**
 * [distance_x difference between coordinates of two particles]
 * @param  A [particle]
 * @param  B [particle]
 * @return   [KIND]
 */
KIND distance_x(particle A, particle B){
		KIND x1, x2, res;
		int j = 0;
		vector<KIND> dx;

		dx.resize(3,0);
		res = infinity;
		x2  = B.x;
		for(int i=-1; i<2; i++){
			x1      = A.x + i*L;
			dx[i+1] = x1 - x2;

			if (abs(dx[i+1]) < res ){
				res = abs(dx[i+1]);
				j = i;
			} //if
		}//for
		return dx[j+1];
}


/**
 * [distance_y same as distance_x]
 * @param  A [particle]
 * @param  B [particle]
 * @return   [KIND]
 */
KIND distance_y(particle A, particle B){
		KIND y1, y2, res;
		int j = 0;
		vector<KIND> dy;

		dy.resize(3,0);
		res = infinity;
		y2  = B.y;
		for(int i=-1; i<2; i++){
			y1      = A.y + i*L;
			dy[i+1] = y1 - y2;

			if (abs(dy[i+1]) < res ) {
				res = abs(dy[i+1]);
				j = i;
			} //if
		}//for
		return dy[j+1];
}

KIND distance1(KIND dx, KIND dy){
    return sqrt(pow(dx,2) + pow(dy,2));
}

/* Interact */
bool interact(particle A, particle B){
	return (distance(A,B) < diameter);
} //repensar esta función


/*INTERACTION FUNCTIONS*/
/* Evolution time step function of the particle */

/***************************************************************************************/

particle evolution(vector<particle> &system, vector<int> &index, bool inter){
	particle Agent = system[index[0]];
	/* DINÁMICA ESPACIAL DEL SISTEMA*/
	if (inter){
		vector<KIND> field, potencial;
		field.resize(2); potencial.resize(2,0); //inicia vector tamaño 2 en 0.

		for(int i=1; i < index.size(); i++){
			KIND dx_0i = distance_x(system[index[0]], system[index[i]]),
				   dy_0i = distance_y(system[index[0]], system[index[i]]),
				   d_0i  = distance1(dx_0i, dy_0i);
			potencial[0] += pow(d_0i,-3)*dx_0i;
			potencial[1] += pow(d_0i,-3)*dy_0i;

		}//for
	    for(int i=0; i<potencial.size(); i++) potencial[i] = gamma_friction*potencial[i];

    	field[0] = system[index[0]].velocity*cos(system[index[0]].angle) + potencial[0];
    	field[1] = system[index[0]].velocity*sin(system[index[0]].angle) + potencial[1];

		Agent.x = b_condition(Agent.x + delta_time*field[0]);
		Agent.y = b_condition(Agent.y + delta_time*field[1]);
	}//if
    else{
		Agent.x = b_condition(Agent.x + Agent.velocity*cos(Agent.angle)*delta_time);
		Agent.y = b_condition(Agent.y + Agent.velocity*sin(Agent.angle)*delta_time);
	}//else
	/*El agente cambia de dirección en A.angle +/- pi/2*/
	if (dis(gen) < p_rotation){
		KIND ruido = (dis(gen)-0.5)*Pi;
		Agent.angle += ruido;
	}

    /*DINÁMICA DE LA EPIDEMIA*/
	bool flag = true; //Flag de infección.
	for (int i=1; i<index.size(); i++){
		if (Agent.is_healthy() && system[index[i]].is_infected()){
			if (dis(gen) < p_transmision){
				Agent.set_infected();
				flag = false; //no puede volverse refractaria en esta instancia de evolución.
			}
		}
	}//for
    if (Agent.is_refractary() && (dis(gen) < p_recfractary) ) Agent.set_healthy(); //SIRS
    if (Agent.is_infected() && flag && (dis(gen) < p_infection) ) Agent.set_refractary();
    return Agent;
}
/***************************************************************************************/
void print_state(vector<int> state_vector){
		cout << "Healthy   : " << state_vector[0] << endl;
		cout << "Infected  : " << state_vector[1] << endl;
		cout << "Refractary: " << state_vector[2] << endl << endl;
}
