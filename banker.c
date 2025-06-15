#include<stdio.h>
#include<pthread.h>
#include<semaphore.h>
#include<stdbool.h>
#define NUM 5

sem_t forks[NUM];
sem_t m;
int V[NUM]={1,1,1,1,1}; //사용가능한 남은 자원
int claim[NUM][NUM]={{1,1,0,0,0},{0,1,1,0,0},{0,0,1,1,0},{0,0,0,1,1},{1,0,0,0,1}}; // 각 philosopher마다 요구하는 자원량
int allocation[NUM][NUM]={0}; //할당된 자원
int need[NUM][NUM]={{1,1,0,0,0},{0,1,1,0,0},{0,0,1,1,0},{0,0,0,1,1},{1,0,0,0,1}}; // C-A

void pickup(int philosopher_num){
	sem_wait(&forks[philosopher_num%NUM]);
}

void putdown(int philosopher_num){
	sem_post(&forks[philosopher_num%NUM]);
}

bool check(int philosopher_num){
	sem_wait(&m); // 상호 배제
	for(int i=0;i<NUM;i++){
		if(need[philosopher_num][i]>V[i]){ //만약 C-A가 남은 자원보다 크다면 할당할 수 없기에 false
			sem_post(&m);
			return false;
		}
	}
	for(int i=0;i<NUM;i++){ //할당한다면 남은 자원은 C-A만큼 감소, 할당은 C-A만큼 증강, 할당 받았기에 C-A=0
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
	for(int i=0;i<NUM;i++){ //할당했던 자원들을 다시 원상복구
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
		if(check(philosopher_num)){ // 할당 가능한 자원이라면 eating 가능
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
