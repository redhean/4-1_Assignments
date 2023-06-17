#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define LINE 50000

// swap string
void swap(char** a, char** b){
    char* temp = *a;
    *a = *b;
    *b = temp;
}

// arr1 = keys, arr2 = input
int partition(char** arr1, char** arr2, int left, int right){
    char* pivot = arr1[right];
    int i = left - 1;

    for(int j = left; j <= right - 1; j++){
        if(strcmp(arr1[j], pivot) < 0){
            i++;
            swap(&arr1[i], &arr1[j]);
            swap(&arr2[i], &arr2[j]);
        }
    }
    swap(&arr1[i + 1], &arr1[right]);
    swap(&arr2[i + 1], &arr2[right]);
    return i + 1;
}

void quickSort(char** arr1, char** arr2, int left, int right){
    if(left < right){
        int pi = partition(arr1, arr2, left, right);

        quickSort(arr1, arr2, left, pi - 1);
        quickSort(arr1, arr2, pi + 1, right);
    }
}

int main(){

    // make FILE variable for reading and wrting txt file.
    FILE *ifp, *ofp;
    ifp = fopen("hw1_input.txt", "r");
    ofp = fopen("hw1_output.txt", "w");

    // store n
    char n_char[LINE];
    fgets(n_char,LINE,ifp);
    int n = atoi(n_char);

    // read $ 
    char read_part[LINE];
    fgets(read_part, LINE, ifp);

    // store the imformation of attributes
    char attribute[LINE];
    fgets(attribute, LINE, ifp);
    
    int t = 0; // store the number of attributes
    int key = 0; // store the place of key attribute
    int idx = 0; 

    while(attribute[idx] != '\0'){
        if(attribute[idx] == '*'){
            key = t;
            idx++;
        }
        else if(attribute[idx] == ':'){
            t++;
            idx++;
        }
        else{
            idx++;
        }
    }

    // read $ again
    fgets(read_part, LINE, ifp);
 
    // two dimensional array for storing input data
    char **input = (char**)malloc(sizeof(char*) * n);
    for(int i = 0; i < n; i++){
        input[i] = (char*)malloc(sizeof(char) * LINE);
    }

    // two dimensional array for storing key value
    char **keys = (char**)malloc(sizeof(char*) * n);
    for(int i = 0 ; i < n; i++){
        keys[i] = (char*)malloc(sizeof(char) * 12);
    }

    // extract key's value
    for(int i = 0; i < n; i++){
        fgets(input[i],LINE,ifp);

        if(i == n - 1){
            int last = strlen(input[i]);
            input[i][last] = '\n';
        }

        char input_copy[LINE];
        strcpy(input_copy, input[i]);
        char* result;

        result = strtok(input_copy, ":");
        for(int i = 0; i < key; i++){
            result = strtok(NULL, ":");
        }
        
        strcpy(keys[i], result);
    }

    // Using quickSort
    quickSort(keys, input, 0, n-1);

    // write into output file
    for(int i = 0; i < n; i++){
        fwrite(input[i], sizeof(char), LINE, ofp);
    }

    // close the file
    fclose(ifp);
    fclose(ofp);
    
    return 0;
}
