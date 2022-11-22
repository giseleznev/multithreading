#include "main.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define BILLION 1000000000L;

const int block_size = 16;
int matrix_size;
int threads_number;
int blocks_num;

int** MakeMatrix()
{
    int **matrix = (int**)malloc(matrix_size * sizeof(int*));
    for (int i = 0; i < matrix_size; i++)
        matrix[i] = (int*)malloc(matrix_size * sizeof(int));

    for (int i = 0; i < matrix_size; i++)
        for (int j = 0; j < matrix_size; j++)
            matrix[i][j] = rand() % 10;
    return matrix;
}

void PrintMatrix(int** matrix)
{
    for (int i = 0; i < matrix_size; i++) {
        for (int j = 0; j < matrix_size; j++) {
            printf("%4d", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

int main(int argc, char **argv) {
    
    // if (argc != 3) {
    //     printf("Wrong argument: ./block_mult threads_number matrix_size");
    //     return 1;
    // }
    // char *p;
    // errno = 0;
    threads_number = 0;
    // threads_number = strtol(argv[1], &p, 10);
    // if (errno != 0 || *p != '\0') {
    //     printf("Wrong argument: ./block_mult threads_number matrix_size");
    //     return 1;
    // }
    // matrix_size = strtol(argv[2], &p, 10);
    // if (errno != 0 || *p != '\0') {
    //     printf("Wrong argument: ./block_mult threads_number matrix_size");
    //     return 1;
    // }

    srand(time(NULL));
    std::ofstream graph1;
    graph1.open ("./log/graph1.txt");
    std::ofstream graph2;
    graph2.open ("./log/graph2.txt");
    struct timespec mt1, mt2;
    long int tt;

    for ( matrix_size = 5; matrix_size < 200; matrix_size += 5 ) {
        graph1 << matrix_size << " ";
        graph2 << matrix_size << " ";

        int** MatrixA = MakeMatrix();
        int** MatrixB = MakeMatrix();
        int** MatrixC = MakeMatrix();

        blocks_num = matrix_size / block_size;
        
        clock_gettime (CLOCK_REALTIME, &mt1);
        MultiplyNaive(MatrixC, MatrixA, MatrixB);
        clock_gettime (CLOCK_REALTIME, &mt2);
        tt=1000000000*(mt2.tv_sec - mt1.tv_sec)+(mt2.tv_nsec - mt1.tv_nsec);
        graph1 << tt << std::endl;

        clock_gettime (CLOCK_REALTIME, &mt1);
        MultiplyBlock(MatrixC, MatrixA, MatrixB);
        clock_gettime (CLOCK_REALTIME, &mt2);
        tt=1000000000*(mt2.tv_sec - mt1.tv_sec)+(mt2.tv_nsec - mt1.tv_nsec);

        graph2 << tt << std::endl;

        for (int i = 0; i < matrix_size; i++) {
            free(MatrixA[i]); free(MatrixB[i]); free(MatrixC[i]); //free(MatrixD[i]);
        }
        free(MatrixA); free(MatrixB); free(MatrixC); //free(MatrixD);
    }
    graph1.close();
    graph2.close();

    matrix_size = 200;
    std::ofstream graph3;
    graph3.open ("./log/graph3.txt");
    std::ofstream graph4;
    graph4.open ("./log/graph4.txt");

    for ( threads_number = 1; threads_number < 20; threads_number += 1 ) {
        graph3 << threads_number << " ";
        graph4 << threads_number << " ";

        int** MatrixA = MakeMatrix();
        int** MatrixB = MakeMatrix();
        int** MatrixC = MakeMatrix();

        blocks_num = matrix_size / block_size;
        
        clock_gettime (CLOCK_REALTIME, &mt1);
        MultiplyThreadedNotBlock(MatrixC, MatrixA, MatrixB);
        clock_gettime (CLOCK_REALTIME, &mt2);
        tt=1000000000*(mt2.tv_sec - mt1.tv_sec)+(mt2.tv_nsec - mt1.tv_nsec);
        graph3 << tt << std::endl;

        clock_gettime (CLOCK_REALTIME, &mt1);
        MultiplyThreadedBlock(MatrixC, MatrixA, MatrixB);
        clock_gettime (CLOCK_REALTIME, &mt2);
        tt=1000000000*(mt2.tv_sec - mt1.tv_sec)+(mt2.tv_nsec - mt1.tv_nsec);
        graph4 << tt << std::endl;

        for (int i = 0; i < matrix_size; i++) {
            free(MatrixA[i]); free(MatrixB[i]); free(MatrixC[i]); //free(MatrixD[i]);
        }
        free(MatrixA); free(MatrixB); free(MatrixC); //free(MatrixD);
    }
    graph1.close();
    graph2.close();
}