#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h>
#include <cstdio>
#include <fstream>

extern const int block_size;
extern int matrix_size;
extern int threads_number;
extern int blocks_num;

void MultiplyNaive(int** res, int** first, int** second) {
    for (int i = 0; i < matrix_size; i++) {
        for (int j = 0; j < matrix_size; j++) {
            res[i][j] = 0;
            for (int k = 0; k < matrix_size; k++) {
                res[i][j] += first[i][k] * second[k][j];
            }
        } 
    }
}

void MultiplyOneBlock(int** res, int** first, int** second, int raw_block_num, int column_block_num) {
    for (int i = raw_block_num * block_size; i < raw_block_num * block_size + block_size; i++) {
        for (int j = column_block_num * block_size; j < column_block_num * block_size + block_size; j++) {
            res[i][j] = 0;
            for (int k = 0; k < matrix_size; k++) {
                res[i][j] += first[i][k] * second[k][j];
            }
        } 
    }
}

void MultiplyBlock(int** res, int** first, int** second) {
    for (int i = 0; i < blocks_num; i++) {
        for (int j = 0; j < blocks_num; j++) {
            MultiplyOneBlock( res, first, second, i, j);
        } 
    }
}

struct ThreadNotBlockData {
    int start; int end;
    int **res; int **first; int **second;
};

void* MultiplyOneThreadNotBlock(void* args) {
    struct ThreadNotBlockData *data = (struct ThreadNotBlockData*)args;
    for (int i = data->start; i < data->end; i++) {
        for (int j = 0; j < matrix_size; j++) {
            data->res[i][j] = 0;
            for (int k = 0; k < matrix_size; k++) {
                data->res[i][j] += data->first[i][k] * data->second[k][j];
            }
        }
    }
    free(data);
}

void MultiplyThreadedNotBlock(int** res, int** first, int** second) {

    pthread_t tid[threads_number];

    for (int i = 0; i < threads_number; i++) {
        struct ThreadNotBlockData *data = ( struct ThreadNotBlockData *)malloc(sizeof(struct ThreadNotBlockData));
        data->start = i * matrix_size / threads_number;
        data->end = (i + 1 == threads_number) ? matrix_size : (i + 1) * matrix_size / threads_number;
        data->res = res;
        data->first = first;
        data->second = second;
        pthread_create(&tid[i], NULL, MultiplyOneThreadNotBlock, data);
    }
    for (int i = 0; i < threads_number; i++)
        pthread_join(tid[i], NULL);

}

struct ThreadBlockData {
    int raw_first_block_num; int raw_last_block_num; int column_first_block_num; int column_last_block_num;
    int **res; int **first; int **second;
};

void* MultiplyOneThreadBlock(void* args) {
    struct ThreadBlockData *data = (struct ThreadBlockData*)args;
    printf("MultiplyOneThreadBlock: [%d, %d], [%d, %d]\n", data->raw_first_block_num, data->raw_last_block_num, data->column_first_block_num, data->column_last_block_num);

    if (data->raw_first_block_num == data->raw_last_block_num) {
        for (int j = data->column_first_block_num; j < data->column_last_block_num; j++) {
            MultiplyOneBlock( data->res, data->first, data->second, data->raw_first_block_num, j);
        }
    } else {
        for (int i = data->raw_first_block_num; i < data->raw_last_block_num; i++) {
            if ( i == data->raw_first_block_num ) {
                for (int j = data->column_first_block_num; j < blocks_num; j++) {
                    MultiplyOneBlock( data->res, data->first, data->second, i, j);
                }
                continue;
            }
            if (i == data->raw_last_block_num ) {
                for (int j = 0; j < data->raw_last_block_num; j++) {
                    MultiplyOneBlock( data->res, data->first, data->second, i, j);
                }
                continue;
            }
            for (int j = 0; j < blocks_num; j++) {
                MultiplyOneBlock( data->res, data->first, data->second, i, j);
            }
        }
    }
    free(data);
}

void MultiplyThreadedBlock(int** res, int** first, int** second){

    pthread_t tid[threads_number];

    if (matrix_size % block_size != 0){
        printf("such matrix size can't be multiplied with blocks of size %d", block_size);
        return;
    };

    int blocks_per_thread = blocks_num * blocks_num / threads_number;
    int current_block = 0;  // 123
                            // 456

    for (int i = 0; i < threads_number; i++) {
        printf("current_block is: %d, bloch num: %d\n", current_block, blocks_num);
        struct ThreadBlockData *data = ( struct ThreadBlockData *)malloc(sizeof(struct ThreadBlockData));
        data->raw_first_block_num = current_block / blocks_num;
        data->column_first_block_num = current_block % blocks_num;
        current_block = (i + 1 == threads_number) ? blocks_num * blocks_num : current_block + blocks_per_thread; 
        data->raw_last_block_num = current_block / blocks_num;
        data->column_last_block_num = current_block % blocks_num;
        data->res = res;
        data->first = first;
        data->second = second;
        pthread_create(&tid[i], NULL, MultiplyOneThreadBlock, data);
    }
    for (int i = 0; i < threads_number; i++)
        pthread_join(tid[i], NULL);

}