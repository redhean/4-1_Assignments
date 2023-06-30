#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include<float.h>

#define BUFFER 100
#define MAX_NODE 3000

char node_index[MAX_NODE][BUFFER] = {0,};
int count = 0;
float **node_arr;
int k_for_kedgeconnected;
int* visited;
int* queue;
FILE *input;

int read(FILE *input);
int find_index(const char* X);
int is_weakly_connected(int cnt);
int is_strongly_connected(int cnt);
void bfs(int cnt, int start);
void init_int(int *ptr, int num);
void init_float(float *ptr, int num);
int find_negative_cycle(int node_cnt);
int is_k_edge_connected(int node_cnt, int k);
float diameter(int node_cnt);
void degree_centrality(int node_cnt);
void closeness_centrality(int node_cnt);
void betweenness_centrality(int node_cnt);

int main(int argc, char* argv[]) {
  int node_cnt, i;
  int negative_cycle_chk = 0;

  if(argc != 2){
    exit(1);
  }

  input = fopen(argv[1], "r");
  // 그래프 입력받아 노드 개수 반환
  node_cnt = read(input);

  // 1. Checking whether a graph is weakly connected
  if (is_weakly_connected(node_cnt))
    printf("True\n");
  else
    printf("False\n");

  // 2. Checking whether a graph is strongly connected
  if (is_strongly_connected(node_cnt))
    printf("True\n");
  else
    printf("False\n");

  // 3. Checking whether a graph is k-edge-connected (process the graph as an undirected graph)
  if(is_k_edge_connected(node_cnt, k_for_kedgeconnected))
    printf("True\n");
  else
    printf("False\n");

  // 4. Checking whether there exists a cycle with negative total weight
  if (find_negative_cycle(node_cnt)){
    printf("True\n");
    negative_cycle_chk = 1;
  }
  else
    printf("False\n");

  // 5. Computing the diameter of a graph
  float result_5 = diameter(node_cnt);
  if(result_5 == -1.0)
    printf("NULL\n");
  else
    printf("%.1lf\n", result_5);

  // 6. Computing the node with the maximum degree centrality
  degree_centrality(node_cnt);

  // 7. Computing the node with the maximum closeness centrality
  if(negative_cycle_chk == 1)
    printf("NULL\n");
  else
    closeness_centrality(node_cnt);

  // 8. Computing the node with the maximum betweenness centrality
  if(negative_cycle_chk == 1)
    printf("NULL\n");
  else
    betweenness_centrality(node_cnt);

  // Free allocated memory
  for (int i = 0; i < node_cnt; i++) {
    free(node_arr[i]);
  }
  free(node_arr);
  free(visited);
  free(queue);

  return 0;
}

int read(FILE *input) {
  char buf[BUFFER] = "";
  int i, cnt = 0;

  // get Node
  while (fgets(buf, sizeof(buf), input) != NULL) {
    if (strchr(buf, ' ') == NULL) {
      if (find_index(buf) != -1)
        continue;
      buf[strcspn(buf, "\n")] = '\0'; 
      strcpy(node_index[cnt], buf);
      cnt++;
      for (i = 0; i < sizeof(buf); i++) {
        buf[i] = 0;
      }
    } else {
      break;
    }
  }
  // allocate cnt*cnt array
  node_arr = (float **)malloc(sizeof(float *) * cnt);
  for (i = 0; i < cnt; i++) {
    node_arr[i] = (float *)malloc(sizeof(float) * cnt);
  }

    char* start_temp = strtok(buf, " ");
    char* end_temp = strtok(NULL, " ");
    char* temp_temp = strtok(NULL, " ");
    float weight_temp = atof(temp_temp);

    node_arr[find_index(start_temp)][find_index(end_temp)] = weight_temp;

  while(strchr(fgets(buf, sizeof(buf), input), ' ') != NULL){
    char* start = strtok(buf, " ");
    char* end = strtok(NULL, " ");
    char* temp = strtok(NULL, " ");
    float weight = atof(temp);

    node_arr[find_index(start)][find_index(end)] = weight;
  }

  k_for_kedgeconnected = atoi(buf);

  // Allocate memory for visited and queue arrays
  visited = (int *)malloc(sizeof(int) * cnt);
  queue = (int *)malloc(sizeof(int) * cnt);

  return cnt;
}

int find_index(const char* node) {
    for (int i = 0; i < MAX_NODE; i++) {
        if (strcmp(node_index[i], node) == 0) {
            return i;
        }
    }
    return -1;
}

void init_int(int *ptr, int num) {
  int i;
  for (i = 0; i < num; i++) {
    ptr[i] = 0;
  }
}

void init_float(float *ptr, int num) {
  int i;
  for (i = 0; i < num; i++) {
    ptr[i] = 0.0;
  }
}

void bfs(int cnt, int start) {
  int front = 0, rear = 0, current = 0, i;

  queue[rear++] = start;
  visited[start] = 1;
  while (front != rear) {
    current = queue[front++];
    for (i = 0; i < cnt; i++) {
      if (visited[i] == 0 && node_arr[current][i] != 0) {
        queue[rear++] = i;
        visited[i] = 1;
      }
    }
  }
}

/*
*  1. Checking whether a graph is weakly connected
*/
int is_weakly_connected(int cnt) {
  int front = 0, rear = 0, current = 0, i;
  init_int(visited, cnt);

  queue[rear++] = 0;
  visited[0] = 1;
  while (front != rear) {
    current = queue[front++];
    for (i = 0; i < cnt; i++) {
      if (visited[i] == 0 && (node_arr[current][i] != 0 || node_arr[i][current] != 0)) {
        queue[rear++] = i;
        visited[i] = 1;
      }
    }
  }

  for (i = 0; i < cnt; i++) {
    if (visited[i] != 1) {
      return 0;
    }
  }
  return 1;
}

/*
* 2. Checking whether a graph is strongly connected
*/
int is_strongly_connected(int cnt) {
  int i, j;

  for (i = 0; i < cnt; i++) {
    init_int(visited, cnt);
    bfs(cnt, i);
    for (j = 0; j < cnt; j++) {
      if (visited[j] != 1) {
        return 0;
      }
    }
  }
  return 1;
}

/*
* 3. Checking whether a graph is k-edge-connected (process the graph as an undirected graph)
*/
int is_k_edge_connected(int cnt, int k) {
    int front = 0, rear = 0, current = 0, i;

    // Initialize 
    init_int(visited, cnt);

    // Perform breadth-first search starting from node 0
    bfs(cnt, 0);

    // Check if all nodes are visited
    for (i = 0; i < cnt; i++) {
        if (visited[i] != 1) {
            return 0;
        }
    }

    // Check if the graph has at least (k-1) edges
    int edge_count = 0;
    for (i = 0; i < cnt; i++) {
        for (int j = i + 1; j < cnt; j++) {
            if (node_arr[i][j] != 0) {
                edge_count++;
            }
        }
    }

    return edge_count >= (k - 1);
}

/*
* 4. Checking whether there exists a cycle with negative total weight
*/
int find_negative_cycle(int node_cnt) {
  float distance[MAX_NODE];
  init_int(visited, MAX_NODE);
  //init_float(distance, MAX_NODE);
  for(int i = 0; i < node_cnt; i++){
    distance[i] = FLT_MAX;
  }

  for (int i = 0; i < node_cnt; i++) {
    distance[i] = 0.0;

    for (int k = 0; k < node_cnt - 1; k++) {
      for (int u = 0; u < node_cnt; u++) {
        for (int v = 0; v < node_cnt; v++) {
          if (distance[u] != FLT_MAX && distance[v] > distance[u] + node_arr[u][v]) {
            distance[v] = distance[u] + node_arr[u][v];
          }
        }
      }
    }
    // check negative cycle
    for (int u = 0; u < node_cnt; u++) {
      for (int v = 0; v < node_cnt; v++) {
        if (distance[u] != FLT_MAX && distance[v] > distance[u] + node_arr[u][v]) {
          return 1;
        }
      }
    }
  }
  return 0;
}

/*
* 5. Computing the diameter of a graph
* Floyd-Warshall algorithm
*/
float diameter(int cnt) {
    // Initialize distance matrix
    float** distance = (float**)malloc(sizeof(float*) * MAX_NODE);
    for(int i = 0; i < MAX_NODE; i++){
        distance[i] = (float*)malloc(sizeof(float) * MAX_NODE);
    }
    for (int i = 0; i < cnt; i++) {
        for (int j = 0; j < cnt; j++) {
            if (i == j)
                distance[i][j] = 0.0;
            else if (node_arr[i][j] != 0.0)
                distance[i][j] = node_arr[i][j];
            else
                distance[i][j] = INFINITY;
        }
    }

    // Calculate all pairs shortest paths using Floyd-Warshall algorithm
    for (int k = 0; k < cnt; k++) {
        for (int i = 0; i < cnt; i++) {
            for (int j = 0; j < cnt; j++) {
                if (distance[i][k] != INFINITY && distance[k][j] != INFINITY && distance[i][k] + distance[k][j] < distance[i][j])
                    distance[i][j] = distance[i][k] + distance[k][j];
            }
        }
    }

    // Find the maximum value in the distance matrix
    float max_distance = 0.0;
    for (int i = 0; i < cnt; i++) {
        for (int j = 0; j < cnt; j++) {
            if (distance[i][j] > max_distance && distance[i][j] != INFINITY)
                max_distance = distance[i][j];
        }
    }
    
    for(int i = 0; i < MAX_NODE; i++){
        free(distance[i]);
    }
    free(distance);
    // Check if the graph is disconnected
    if (max_distance == INFINITY)
        return -1.0; // Graph is disconnected, diameter cannot be calculated

    return max_distance;
}


/*
* 6. Computing the node with the maximum degree centrality
* 7. Computing the node with the maximum closeness centrality
* 8. Computing the node with the maximum betweenness centrality
*
* Floyd-Warshall 알고리즘을 사용했는데 이는 음수 사이클을 체크하지 못 하므로 위에 
* find_negative_cycle 결과에 따라 음수 사이클이 존재하면 NULL 출력하도록 함 
*/

float** create_distance_matrix(int node_cnt) {
    float** distance = (float**)malloc(sizeof(float*) * node_cnt);
    for (int i = 0; i < node_cnt; i++) {
        distance[i] = (float*)malloc(sizeof(float) * node_cnt);
    }
    for (int i = 0; i < node_cnt; i++) {
        for (int j = 0; j < node_cnt; j++) {
            if (i == j)
                distance[i][j] = 0.0;
            else
                distance[i][j] = INFINITY;
        }
    }
    return distance;
}

void free_distance_matrix(float** distance, int node_cnt) {
    for (int i = 0; i < node_cnt; i++) {
        free(distance[i]);
    }
    free(distance);
}

void floyd_warshall(float** distance, int node_cnt) {
    for (int k = 0; k < node_cnt; k++) {
        for (int i = 0; i < node_cnt; i++) {
            for (int j = 0; j < node_cnt; j++) {
                if (distance[i][k] != INFINITY && distance[k][j] != INFINITY &&
                    distance[i][k] + distance[k][j] < distance[i][j])
                    distance[i][j] = distance[i][k] + distance[k][j];
            }
        }
    }
}

float calculate_degree_centrality(float** distance, int node_cnt, int node) {
    float in_degree_sum = 0.0;
    float out_degree_sum = 0.0;
    for (int i = 0; i < node_cnt; i++) {
        if (i != node && !isinf(distance[node][i]))
            in_degree_sum += 1.0;
        if (i != node && !isinf(distance[i][node]))
            out_degree_sum += 1.0;
    }
    return (in_degree_sum + out_degree_sum) / (2 * (node_cnt - 1));
}

float calculate_closeness_centrality(float** distance, int node_cnt, int node) {
    float sum = 0.0;
    for (int i = 0; i < node_cnt; i++) {
        if (i != node)
            sum += 1.0 / distance[node][i];
    }

    if(sum == 0.0)
      return FLT_MAX;
    else
      return 1.0 / sum;
}

void calculate_betweenness_centrality(float** distance, int node_cnt, float* betweenness) {
    for (int k = 0; k < node_cnt; k++) {
        for (int i = 0; i < node_cnt; i++) {
            for (int j = 0; j < node_cnt; j++) {
                if (i != j && i != k && j != k &&
                    distance[i][k] != INFINITY && distance[k][j] != INFINITY &&
                    distance[i][k] + distance[k][j] == distance[i][j])
                    betweenness[k]++;
            }
        }
    }
}

void print_nodes_with_max_centrality(float* centrality, int node_cnt) {
    float max_centrality = 0.0;
    for (int i = 0; i < node_cnt; i++) {
        if (centrality[i] > max_centrality)
            max_centrality = centrality[i];
    }
    for (int i = 0; i < node_cnt; i++) {
        if (centrality[i] == max_centrality)
            printf("%d ",i);
    }
    printf("\n");
}

void degree_centrality(int node_cnt) {
    float** distance = create_distance_matrix(node_cnt);
    for (int i = 0; i < node_cnt; i++) {
        for (int j = 0; j < node_cnt; j++) {
            if (i == j)
                distance[i][j] = 0.0;
            else if (node_arr[i][j] != 0.0)
                distance[i][j] = node_arr[i][j];
        }
    }

    float* centrality = (float*)malloc(sizeof(float) * node_cnt);
    for (int i = 0; i < node_cnt; i++) {
        centrality[i] = calculate_degree_centrality(distance, node_cnt, i);
    }

    print_nodes_with_max_centrality(centrality, node_cnt);

    free_distance_matrix(distance, node_cnt);
    free(centrality);
}

void closeness_centrality(int node_cnt) {
    float** distance = create_distance_matrix(node_cnt);
    for (int i = 0; i < node_cnt; i++) {
        for (int j = 0; j < node_cnt; j++) {
            if (i == j)
                distance[i][j] = 0.0;
            else if (node_arr[i][j] != 0.0){
                distance[i][j] = node_arr[i][j];
            }
        }
    }

    floyd_warshall(distance, node_cnt);

    float* centrality = (float*)malloc(sizeof(float) * node_cnt);
    for (int i = 0; i < node_cnt; i++) {
        centrality[i] = calculate_closeness_centrality(distance, node_cnt, i);
    }

    print_nodes_with_max_centrality(centrality, node_cnt);

    free_distance_matrix(distance, node_cnt);
    free(centrality);
}

void betweenness_centrality(int node_cnt) {
    float** distance = create_distance_matrix(node_cnt);
    for (int i = 0; i < node_cnt; i++) {
        for (int j = 0; j < node_cnt; j++) {
            if (i == j)
                distance[i][j] = 0.0;
            else if (node_arr[i][j] != 0.0)
                distance[i][j] = node_arr[i][j];
        }
    }

    floyd_warshall(distance, node_cnt);

    float* centrality = (float*)malloc(sizeof(float) * node_cnt);
    for (int i = 0; i < node_cnt; i++) {
        centrality[i] = 0.0;
    }

    calculate_betweenness_centrality(distance, node_cnt, centrality);

    print_nodes_with_max_centrality(centrality, node_cnt);

    free_distance_matrix(distance, node_cnt);
    free(centrality);
}












