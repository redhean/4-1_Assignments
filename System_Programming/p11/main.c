#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

int* result;
int** matrix;
int* vector;
int N;
int M;
int result_value;

void *thread(void *arg){
	long idx = (long)arg;
	
	for(int i = 0; i < M; i++){
		result_value = 0;
		for(int j = 0; j < N; j++){
			result_value += matrix[idx][j] * vector[j];
		}
		result[idx] = result_value;
	}

	pthread_exit(NULL);	
}

int main(int argc, char* argv[]){
		
	if(argc != 3)
		exit(1);
	
	srand(time(NULL));
	M = atoi(argv[1]);
	N = atoi(argv[2]);
	
	matrix = (int**)malloc(sizeof(int*) * M);
	for(int i = 0; i < M; i++){
		matrix[i] = (int*)malloc(sizeof(int) * N);
	}

	for(int i = 0; i < M; i++){
		for(int j = 0; j < N; j++){
			int value = rand() % 10;
			matrix[i][j] = value;
		}
	}

	printf("*** Matrix ***\n");
	for(int i = 0; i < M; i++){
		for(int j = 0; j < N; j++){
			printf("[ %d ] ",matrix[i][j]);
		}
		printf("\n");
	}

	printf("*** Vector ***\n");
	vector = (int*)malloc(sizeof(int) * N);
	for(int i = 0; i < N; i++){
		int value = rand() % 10;
		vector[i] = value;
		printf("[ %d ]\n", vector[i]);
	}

	printf("*** Result ***\n");
	result = (int*)malloc(sizeof(int) * N);
	pthread_t tid[M];
	long t;
	for(t = 0; t < M; t++){
		if(pthread_create(&tid[t], NULL, thread, (void*)t)){
			exit(1);
		}
	}
	for(t = 0; t < M; t++){
		pthread_join(tid[t], NULL);
	}

	for(int i = 0; i < M; i++){
		printf("[ %d ]\n", result[i]);
	}
	return 0;
}
