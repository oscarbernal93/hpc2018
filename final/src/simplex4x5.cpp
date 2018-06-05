/*
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include <vector>
#include <list>
#include <algorithm>
#include <queue>
#include <stack>
#include <set>
#include <map>
#include <complex>
*/
#ifndef INF
#define INF 99999
#endif
#ifndef nop
#define nop nop()
#endif
//used
#include <iostream>
#include <string>
#include <iomanip>

const int numRest = 5;
const int numVars = 4 + numRest;

using namespace std;

template<typename T>
void log(T data, string s = "log")
{
    cout << s << ": " << data << endl;
}

void nop;
int count_negatives(float* V,int size);
int index_of_lesser(float* V,int size);
void print_simplex_state(float *B,float *C,float *P,float COEF[][numVars], float *Z);
void print_simplex_solution(float *B,float *C,float *P);

int main(int argc, char const *argv[])
{
	//cout << fixed;
    //cout << setprecision(2);
	int nn,cp,rp,counter,i,j,k;
	float tmp,pivot;
	float lesser = INF;
	float COEF[numRest][numVars] = {
		{8,0,1,5,1,0,0,0,0},
		{1,1,0,0,0,1,0,0,0},
		{2,1,4,8,0,0,1,0,0},
		{0,3,1,0,0,0,0,1,0},
		{2,0,3,1,0,0,0,0,1}
	};
	float C[numRest]   = {0};
	float B[numRest]   = {4,5,6,7,8};
	float P[numRest+1] = {6,4,24,3,41,0};
	float Z0[numVars]  = {18,24,25,12,0,0,0,0,0};
	float Z[numVars]   = {0};
	for (int i = 0; i < numVars - numRest; ++i)
	{
		Z[i] = -Z0[i];
	}
	nn = count_negatives(Z,numVars);
	while(nn > 0){
		cp = index_of_lesser(Z,numVars);
		counter = 0;
		for (i = 0; i < numRest; ++i)
		{
			if ( P[i] > 0 && COEF[i] > 0 )
			{
				tmp = P[i] / COEF[i][cp];
				if (tmp < lesser && tmp > 0)
				{
					lesser = tmp;
					rp = i;
				}
				counter++;
			}
		}
		//log(rp,"rp");
		//log(cp,"cp");
		if( counter > 0 ){
			pivot = COEF[rp][cp];
			for (i = 0; i < numRest; ++i)
			{
				for (j = 0; j < numVars; ++j)
				{
					if(i == rp || j == cp){
						nop;
					}else{
						COEF[i][j] = COEF[i][j] - ( COEF[i][cp] * COEF[rp][j] / pivot );
					}
				}
			}
			for (i = 0; i < numRest; ++i)
			{
				if(i == rp){
					nop;
				}else{
					C[i] =  C[i] - ( COEF[i][cp] * C[rp]/pivot );
					P[i] =  P[i] - ( COEF[i][cp] * P[rp]/pivot );
					COEF[i][cp] = COEF[i][cp] - ( COEF[i][cp] * COEF[rp][cp] / pivot );

				}
			}
			for (j = 0; j < numVars; ++j)
			{
				if(j == cp){
					nop;
				}else{
					//cout <<"Z["<<j<<"] = "<<Z[j] - ( Z[cp] * COEF[rp][j]/pivot)<< " = " <<Z[j]<<" - "<<Z[cp]<<" * "<<COEF[rp][j]<<" / "<<pivot << endl;
					Z[j] =  Z[j] - ( Z[cp] * COEF[rp][j]/pivot);
					COEF[rp][j] = COEF[rp][j] / pivot;
				}
			}
			//individual calculus for pivot
			P[numRest] =  P[numRest] - ( Z[cp] *  P[rp]/pivot);
			Z[cp] =  Z[cp] - ( Z[cp] * COEF[rp][cp]/pivot);
			P[rp] =  P[rp] /pivot;
			COEF[rp][cp] = COEF[rp][cp] / pivot;
			//index calculus
			B[rp] = cp;
			C[rp] = Z0[cp];
			nn = count_negatives(Z,numVars);
		}else{
			log("solucion no acotada","error");
			nn = 0;
		}
	}
	print_simplex_solution(B,C,P);
	return 0;
}

void nop{
	return;
}
int count_negatives(float* V,int size)
{
	int i,count = 0;
	for (i = 0; i < size; ++i)
	{
		if (V[i] < 0)
		{
			count++;
		}
	}
	return count;
}
int index_of_lesser(float* V,int size)
{
	int i,iol;
	int lesser = INF;
	for (i = 0; i < size; ++i)
	{
		if (V[i] < lesser)
		{
			lesser = V[i];
			iol = i;
		}
	}
	return iol;
}
void print_simplex_state(float *B,float *C,float *P,float COEF[][numVars], float *Z)
{
	string tab = "\t";
	for (int i = 0; i < numRest; ++i)
	{
		for (int j = 0; j < numVars; ++j)
		{
			if (j == 0)
			{
				cout << B[i]+1 << tab << "|" << tab << C[i] << tab << P[i] << tab;
			}
			cout << COEF[i][j] << tab;
		}
		cout << endl;
	}
	cout << tab << tab << tab << P[numRest] << tab;
	for (int k = 0; k < numVars; ++k)
	{
		cout << Z[k] << tab;
	}
	cout << endl<<endl;
}
void print_simplex_solution(float *B,float *C,float *P)
{
	cout << "Z = " << P[numRest]<<endl;
	for (int i = 0; i < numRest; ++i)
	{
		if(C[i] != 0){
			cout << "x_" << B[i]+1 << " = " << P[i]<< endl;
		}
	}
}