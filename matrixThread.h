#include<pthread.h>
#include<stdio.h>
#include<time.h>

#define M 8
#define K 6
#define N 10

int A[M][K];
int B[K][N];
int C[M][N];
void *calc(void * arg);

typedef struct v
{
    int i;/*row*/
    int j;/*column*/
}v;

void gene_matrix()
{   int i,j;
    srand((unsigned)time(NULL));
    printf("A = \n");
    for(i = 0; i < M; i++)
    {   
        for(j = 0; j < K; j++)
        {
            A[i][j] = rand() % 10;
            printf("%5d",A[i][j]);
        }
        printf("\n");
    }
    printf("B = \n");
    for(i = 0; i < K; i++)
    {   
        for(j = 0; j < N; j++)
        {
            B[i][j] = rand() % 10;
            printf("%5d",B[i][j]);
        }
        printf("\n");
    }
}

int main(){
  //  pthread_attr_t attr[M * N];
    pthread_t tids[M * N];

    int i, j;
    gene_matrix();
    clock_t start = clock();
    for(i = 0; i < M; i++)
    {   for(j = 0; j < N; j++){
            v *data = (v *)malloc(sizeof(v));
            data -> i = i;
            data -> j = j;
            //pthread_attr_init(&attr[i * N + j]);
            if(pthread_create(&tids[i * N + j],NULL,calc,(void *)data))
            {
                perror("pthread_create\n");
                exit(1);
            } 
        }
    }
    for(i = 0; i < M; i++)
        for(j = 0; j < N; j++)
        {
            pthread_join(tids[i * N + j], NULL);
        }
        clock_t finish = clock();
        printf("Total time: %0.2f s\n",(long)(finish - start)/1E6);
    printf("C =\n"); 
    for(i = 0; i < M; i++)
    {
        for(j = 0; j < N; j++)
        {
            printf("%5d",C[i][j]);
        }
        printf("\n");
    }
        exit(0);
}

void *calc(void * arg)
{
    v * data = (v * )arg;
    C[data->i][data->j] = 0;
    int i;
    for(i = 0; i < K; i++)
        C[data->i][data->j] += A[data->i][i] * B[i][data->j];
    pthread_exit(NULL);
}

