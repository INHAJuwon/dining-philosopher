#include<stdio.h>
#include<pthread.h>
#include<semaphore.h>
#include<stdbool.h>
#define NUM 5

sem_t forks[NUM];
sem_t m;
int V[NUM]={1,1,1,1,1};
int claim[NUM][NUM]={{1,1,0,0,0},{0,1,1,0,0},{0,0,1,1,0},{0,0,0,1,1},{1,0,0,0,1}};
int allocation[NUM][NUM]={0};
int need[NUM][NUM]={{1,1,0,0,0},{0,1,1,0,0},{0,0,1,1,0},{0,0,0,1,1},{1,0,0,0,1}};

void pickup(int philosopher_num){
	sem_wait(&forks[philosopher_num%NUM]);
}

void putdown(int philosopher_num){
	sem_post(&forks[philosopher_num%NUM]);
}

bool check(int philosopher_num){
	sem_wait(&m);
	for(int i=0;i<NUM;i++){
		if(need[philosopher_num][i]>V[i]){
			sem_post(&m);
			return false;
		}
	}
	for(int i=0;i<NUM;i++){
		V[i]-=need[philosopher_num][i];
		allocation[philosopher_num][i]+=need[philosopher_num][i];
		need[philosopher_num][i]=0;
	}
	sem_post(&m);
	pickup(philosopher_num);
	printf("philosopher %d picks up the fork %d\n",philosopher_num, philosopher_num);
	pickup(philosopher_num+1);
	printf("philosopher %d picks up the fork %d\n",philosopher_num,(philosopher_num+1)%NUM);
	return true;
}

void release(int philosopher_num){
	sem_wait(&m);
	for(int i=0;i<NUM;i++){
		V[i]+=need[philosopher_num][i];
		need[philosopher_num][i]-=need[philosopher_num][i];
		allocation[philosopher_num][i]=0;
	}
	sem_post(&m);
	putdown(philosopher_num+1);
	printf("philosopher %d puts down the fork %d\n",philosopher_num, (philosopher_num+1)%NUM);
	putdown(philosopher_num);
	printf("philosopher %d puts down the fork %d\n",philosopher_num, philosopher_num);
}

void thinking(int philosopher_num){
	printf("philosopher %d is thinking\n", philosopher_num);
	return;
}
void eating(int philosopher_num){
	printf("philosopher %d is eating\n", philosopher_num);
	return;
}
void* philosopher(void* arg){
	int philosopher_num;
	philosopher_num=(unsigned long int)arg;
	while(true){
		if(check(philosopher_num)){
			eating(philosopher_num);
			release(philosopher_num);
		}
		thinking(philosopher_num);
	}
	return NULL;
}

int main(){
	pthread_t threads[NUM];

	for(int i=0;i<NUM;i++)
		sem_init(&forks[i],0,1);

	sem_init(&m,0,1);

	for(unsigned long int i=0;i<NUM;i++){
		pthread_create(&threads[i],NULL,philosopher,(void*)i);
	}

	for(int i=0;i<NUM;i++){
		pthread_join(threads[i],NULL);
	}
	printf("NO DEADLOCK\n");
	return 0;
}
