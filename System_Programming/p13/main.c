#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int count;
int points;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

void *montecarlo(void *arg) {
    double x, y;
    unsigned int seed = (unsigned int)(long)arg;

    for(int i = 0; i < points; i++){
    	x = (double)rand_r(&seed) / RAND_MAX;
    	y = (double)rand_r(&seed) / RAND_MAX;

	if((x * x) + (y * y) < 1){
		pthread_mutex_lock(&m);
		count++;
		pthread_mutex_unlock(&m);
	}
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("usage: ./p13 [num_threads] [points per thread]\n");
        exit(1);
    }
    int n = atoi(argv[1]);
    points = atoi(argv[2]);
    /* your code here */
    
    pthread_t tid[n];
    long i;
    for(i = 0; i < n; i++){
	if(pthread_create(&tid[i], NULL, montecarlo, (void*)i)){
		exit(1);
	}
    }

    for(i = 0; i < n; i++){
	pthread_join(tid[i], NULL);
    }
				
    double pi = (double)(4 * count) / (double)(n * points);
    printf("pi: %lf\n", pi);
    return 0;
}
