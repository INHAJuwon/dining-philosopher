#include<stdio.h>
#include<pthread.h>
#include<semaphore.h>
#define NUM 5

sem_t forks[NUM];

void pickup(int philosopher_num){
	sem_wait(&forks[philosopher_num%NUM]);
}

void putdown(int philosopher_num){
	sem_post(&forks[philosopher_num%NUM]);
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
	while(1){
		if(philosopher_num<4){ //0~3 philosopher들은 자신의 좌, 우 순서대로 fork를 픽업하며 한 방향으로 진행된다.
			pickup(philosopher_num);
			printf("philosopher %d picks up the fork %d\n",philosopher_num, philosopher_num);
			pickup(philosopher_num+1);
			printf("philosopher %d picks up the fork %d\n",philosopher_num, (philosopher_num+1)%NUM);
		}else{ // 4 philosopher마저 좌, 우순으로 선택하면 순환 대기가 생기기 때문에 4번만 순서를 반대로 하여 순환 대기가 생기지 않도록 하였다.
			pickup(philosopher_num+1);
			printf("philosopher %d picks up the fork %d\n",philosopher_num, (philosopher_num+1)%NUM);
			pickup(philosopher_num);
			printf("philosopher %d picks up the fork %d\n",philosopher_num, philosopher_num);
		}
		eating(philosopher_num);
		putdown(philosopher_num+1);
		printf("philosopher %d puts down the fork %d\n",philosopher_num, (philosopher_num+1)%NUM);
		putdown(philosopher_num);
		printf("philosopher %d puts down the fork %d\n",philosopher_num, philosopher_num);
		thinking(philosopher_num);
	}
	return NULL;
}

int main(){
	pthread_t threads[NUM];

	for(int i=0;i<NUM;i++){
		sem_init(&forks[i],0,1);
	}

	for(unsigned long int i=0;i<NUM;i++){
		pthread_create(&threads[i],NULL,philosopher,(void*)i);
	}

	for(int i=0;i<NUM;i++){
		pthread_join(threads[i],NULL);
	}
	printf("NO DEADLOCK\n");
	return 0;
}
