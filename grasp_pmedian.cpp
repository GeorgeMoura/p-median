#include <iostream>
#include <vector>
#include <time.h>
#include <utility>
#include <stdlib.h>
#include <list>
#include <string.h>
#include <limits.h>
#include <algorithm>
#include <iterator>

#define GRASP_MAX_ITER 50 // numero de iterações do grasp
#define GRASP_ALPHA_VALUE 0.5 //porcentagem de aleatoriedade do grasp

using namespace std;

typedef vector<vector<int> > vv;
typedef vector<list<pair<int, int> > > vlp; // <node, cost>

//Auxiliar functions:
void print_solution(pair<int, vector<int> >);
bool not_in(int, vector<int>);
bool compare(pair<int,float> , pair<int, float>);
vector<vector<int> > readInput();

//algorithm functions:
vlp construction(vv &adj, int limit){ // converte a matriz numa lista, setando valores de distancia acima do parametro como intratáveis
	int size = adj.size();
	vlp adj_list(size);

	for(int i=0; i<size; i++)
		for(int j=0; j<size; j++)
			if(i!=j)
				if(adj[i][j] <= limit)
					adj_list[i].push_back(make_pair(j, adj[i][j]));

	return adj_list;
}

//[TODO] armazenar informações de média em linked list para poupar operações
vector<int> grasp_solution(vlp adj, int P, float alpha){

    vector<int> solution;
    vector<pair<int, float> > medias;
    list<int> sol_list;
    float media;
    int size_list = adj.size(), total_part, left_part, random;

    for(int i=0; i<size_list; i++){

        media = 0;
        for(list<pair<int, int> >::iterator it = adj[i].begin(); it!=adj[i].end(); it++)
            media += it->second;
        media /= adj[i].size();
        medias.push_back(make_pair(i, media));
    }

    sort(medias.begin(), medias.end(), compare); 

    left_part = size_list - P;
    total_part = P + (left_part * alpha);

    for(int i=0; i<total_part; i++)
    	sol_list.push_back(medias[i].first);
    

    list<int>::iterator it;

    for(int i=0; i<P; i++){
    	it = sol_list.begin();
    	random = rand() % total_part;
    	advance(it, random);
    	solution.push_back(*it);
    	sol_list.erase(it);
    	total_part--;
    }

    return solution;
}

// soma local com LISTA
int local_sum(vlp adj, vector<int> solution){
	int size_list = adj.size(), size_sol = solution.size(), min, sum=0;
	bool pass;

	for(int i=0; i<size_list; i++){

		min = INT_MAX;
		pass = true;
		for(int g=0; g<size_sol; g++)
			if(i==solution[g])
				pass = false;
			
		if(pass){
			for(list<pair<int, int> >::iterator it = adj[i].begin(); it != adj[i].end(); it++)
				for(int g=0; g<size_sol; g++)
					if(it->first == solution[g])
						if(it->second < min){
							min = it->second;
						}

			if(min != INT_MAX)
				sum += min; // soma os mínimos para cada nó
		}
	}
	return sum; // retorna a soma
}


pair<int, vector<int> > neighborhood_moviments(vlp adj_list, vector<int> initial_solution){ // essa função vai fazer as permutações e calcular as somas

	int best_local_sum = INT_MAX, init_size = initial_solution.size(), partial_sum;
	vector<int> best_local_solution, partial_solution = initial_solution;
	bool improve=true;

	best_local_sum = local_sum(adj_list, partial_solution); // solução inicial
	best_local_solution = partial_solution;
	
	while(improve){

		initial_solution = partial_solution;
		improve = false;

		for(int i=0; i<init_size; i++){
			for(list<pair<int, int> >::iterator it = adj_list[initial_solution[i]].begin(); it != adj_list[initial_solution[i]].end(); it++){

				if(not_in(it->first, partial_solution)){
					partial_solution[i] = it->first;
				}else
					continue;

				partial_sum = local_sum(adj_list, partial_solution);

				if(partial_sum<best_local_sum){
					best_local_sum = partial_sum;
					best_local_solution = partial_solution;
					improve = true;
				}
			}
			partial_solution[i] = initial_solution[i];
		}

		if(improve)
			partial_solution = best_local_solution;
	} 

	return make_pair(best_local_sum, best_local_solution);
}

//função que define como a matriz será podada
int cut_limit(vv adj){
	int size_matrix = adj.size();
	int max = 0, min = INT_MAX;

	for(int i=0; i<size_matrix; i++)
		for(int j=0; j<size_matrix; j++){
			if(adj[i][j] != 0){
				min = min<adj[i][j]?min:adj[i][j];
				max = max>adj[i][j]?max:adj[i][j];
			}
		}
	if(min==INT_MAX)min=0;
	return ((max-min)/2);   //HEURISTICA DE CORTE DA MATRIZ
}

int main()
{
	vv adj_matrix = readInput(); // faz a leitura da matriz de entrada pela entrada padrão cin (usar fluxo do unix)
	vlp original_list, heuristic_list;
	int P, V;
	vector<int> initial_solution;
	pair<int, vector<int> > global_solution, local_solution;
	global_solution.first = INT_MAX;

	//original_list vai receber simplesmente a conversão da matriz original pra lista, isso porque o valor limite é pseudo-infinito
    original_list = construction(adj_matrix, INT_MAX);
    //heuristic_list é a lista podada, vai limitar as conexões baseadas numa constante limite imposta.
    heuristic_list = construction(adj_matrix, cut_limit(adj_matrix));

	do{
		//cout << "defina P ( P < V and P > 0):";
		cin >> P;
	}while(P>=V && P<=0);

    for(int i=0; i<GRASP_MAX_ITER; i++){

        //seleção gulosa-aleatória de medianas
        initial_solution = grasp_solution(heuristic_list, P, GRASP_ALPHA_VALUE);

        //movimentos de vizinhança, utiliza busca local enquanto ela estiver melhorando a solução
        local_solution = neighborhood_moviments(original_list, initial_solution);
        
        cout << "Solução local: ";
        print_solution(local_solution);

        //fica só com a melhor
        global_solution = local_solution.first<global_solution.first?local_solution:global_solution;
    }
    cout << "Melhor resultado da metaheuristica: ";
	print_solution(global_solution);

    return 0;
}

//implementação de funções auxiliares

vector<vector<int> > readInput(){

    int route = 0, numberInput = 0;
    cin >> route;
    vector<vector<int> > mDistance;

  for (int i = 0; i < route; i++){
      vector<int> vTemp;
      for (int j = 0; j < route; j++){
           cin >>  numberInput;
           vTemp.push_back ( numberInput ); 
      }
      mDistance.push_back( vTemp );
  }
   return mDistance;
}

void print_solution(pair<int, vector<int> > result){

	int size = result.second.size();
	cout << "( " << result.first << " , [,";
	for(int i=0; i<size; i++)
		cout << result.second[i] << ",";
	cout << "] )" << endl;
	cout << endl;
}

bool not_in(int test, vector<int> vec){
	int size = vec.size();
	for(int i=0; i<size; i++)
		if(test==vec[i])
			return false;
	return true;
}

bool compare(pair<int, float> p1, pair<int, float> p2){
	return p1.second < p2.second;
}
