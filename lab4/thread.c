#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/time.h>

/* Author 1: Reece Holl, holl.13@osu.edu */
/* Author 2: Andrew Cheng, cheng.620@osu.edu */
/* The code can be compiled and run normally, as described in the lab document */

#define N 2400
#define M 2000
#define P 500
int matrix_A[N][M];           /* Matrix A */
int matrix_B[M][P];           /* Matrix B */
int matrix_C1[N][P];          /* Matrix C1 */
int matrix_C[N][P];           /* Matrix C */

struct arguments {
  int thrs;
  int place;
};

void initialize_matrix_A(){
  int i;
  int j;
  for(i = 0; i < N; i++){
    for(j = 0; j < M; j++){
      matrix_A[i][j] = j-i+2;
    }
  }
}

void initialize_matrix_B(){
  int i;
  int j;
  for(i = 0; i < M; i++){
    for(j = 0; j < P; j++){
      matrix_B[i][j] = i-j+1;
    }
  }
}

void multiply_matrices_for_C1(){
  int i;
  int j;
  int k;
  for (i = 0; i < N; i++){
    for (j = 0; j < P; j++){
      matrix_C1[i][j] = 0;
      for (k = 0; k < M; k++){
	matrix_C1[i][j] += matrix_A[i][k]*matrix_B[k][j];
      }
    }
  }
}

void *multiply_matrices(void* thr_args){
  int i;                  /* A counter */
  int j;                  /* A counter */
  int k;                  /* A counter */
  struct arguments *args; /* The arguments */
  int part;               /* The place that should be multiplied */
  int num;                /* The number of threads */
  int start_row;          /* The row to start multiplying */
  int end_row;            /* The row to end multiplying */

  args = (struct arguments *)thr_args; 
  part = args->place;
  num = args->thrs;
  start_row = (part * N)/num;
  end_row = ((part + 1)* N)/num;
  for (i = start_row; i < end_row; i++){
    for (j = 0; j < P; j++){
      matrix_C[i][j] = 0;
      for (k = 0; k < M; k++){
	matrix_C[i][j] += matrix_A[i][k]*matrix_B[k][j];
      }
    }
  }
  return 0;
}

int check_matrix_equality(){
  int i;                  /* A counter */
  int j;                  /* A counter */
  for (i = 0; i < N; i++){
    for(j = 0; j < P; j++){
      if(matrix_C[i][j] != matrix_C1[i][j]){
	printf("%d\t%d\t%d\n", i, j, matrix_C[i+1][j]);
	return 0;
      }
    }
  }
  return 1;
}

int main(int argc, char*argv[]){
  int num_threads;            /* Number of threads */
  int i;
  int j;
  struct timeval start_time;
  struct timeval end_time;
  double time;

  /* Warning for not enough or too many arguments */
  if(argc != 2){
    printf("Please enter two arguments only.\n");
    exit(-1);
  }

  /* Get arguments */
  num_threads = atoi(argv[1]);

  /* Warning for too many threads */
  if(num_threads < 0){
    printf("Please enter a number between 0 and n on the command line.\n");
    exit(-1);
  }

  /* Print header */
    printf("Threads\tSeconds\n");

  if(num_threads == 0){
    exit(0);
  }

  /* Initialize A and B */
  initialize_matrix_A();
  initialize_matrix_B();

  gettimeofday(&start_time, NULL);
  multiply_matrices_for_C1();
  gettimeofday(&end_time, NULL);
  time = (end_time.tv_sec - start_time.tv_sec) + ((end_time.tv_usec - start_time.tv_usec)/1000000.0);
  printf("1\t%.2f\n", time);
  
  
  /* Create threads */
  for(i = 1; i < num_threads; i++){
    gettimeofday(&start_time, NULL);
    int result = 0;               /* Integer to check if thread creation failed */
    int equality;                 /* Equals 1 if matrices are equal, 0 otherwise */
    pthread_t* threads;
    struct arguments args_first;
    threads = (pthread_t*) malloc(num_threads*sizeof(pthread_t));
    for(j = 1; j < i + 1; j++){
      struct arguments thr_args[7];
      thr_args[j-1].thrs = i + 1;
      thr_args[j-1].place = j;
      result = pthread_create(&threads[j], NULL, multiply_matrices,(void *)&thr_args[j-1]);
    }
    if(result != 0){
      printf("Error creating producer threads.\n");
      exit(-1);
    }
    args_first.thrs = i + 1;
    args_first.place = 0;
    multiply_matrices((void *)&args_first);
    for(j = 1; j < i+1; j++){
      pthread_join(threads[j], NULL);
    }
    gettimeofday(&end_time, NULL);
    time = (end_time.tv_sec - start_time.tv_sec) + ((end_time.tv_usec - start_time.tv_usec)/1000000.0);
    printf("%d\t%.2f\n",i+1,time);
    equality = check_matrix_equality();
    if(equality == 1){
      printf("No error\n");
    }
    else{
      printf("Something's gone terribly wrong\n");
    }
    memset(matrix_C, 0, sizeof(matrix_C[0])*P);
    memset(threads, 0, num_threads*sizeof(pthread_t));
  }
  
  exit(0);
}
