#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define N_CONS 10

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

int ready = 0;
int data = 0;

void *provider(void *arg) {
	printf("provider: hello!\n");

	for (int i = 0; i < N_CONS; i++) {
		while (1) {
			pthread_mutex_lock(&lock);
			if (ready == 1) {
				pthread_mutex_unlock(&lock);
				continue;
			}
			break;
		}

		ready = 1;
		data = i;

		printf("provider: %d -> data\n", i);

		pthread_cond_signal(&cond);
		pthread_mutex_unlock(&lock);
	}

	printf("provider: goodbye!\n");

	return NULL;
}

void *consumer(void *arg) {
	int num = *((int *) arg);
	int color = 31 + num % 8;

	printf("\033[%dmconsumer %d: hello!\033[0m\n", color, num);

	pthread_mutex_lock(&lock);

	while (ready == 0) {
		printf("\033[%dmconsumer %d: asleep\033[0m\n", color, num);
		pthread_cond_wait(&cond, &lock);
		printf("\033[%dmconsumer %d: awoke\033[0m\n", color, num);
	}

	ready = 0;

	printf("\033[%dmconsumer %d: get %d\033[0m\n", color, num, data);

	data = 0;

	pthread_mutex_unlock(&lock);

	printf("\033[%dmconsumer %d: goodbye!\033[0m\n", color, num);

	return NULL;
}

int main(void) {
	pthread_t pr;
	pthread_t cm[N_CONS];
	int cm_arg[N_CONS];

	if (pthread_create(&pr, NULL, provider, NULL) != 0) {
		printf("Couldn't create a provider thread");
		exit(1);
	}

	for (int i = 0; i < N_CONS; i++) {
		cm_arg[i] = i;
		if (pthread_create(&cm[i], NULL, consumer, &cm_arg[i]) != 0) {
			printf("Couldn't create a consumer thread");
			exit(1);
		}
	}

	pthread_join(pr, NULL);
	for (int i = 0; i < N_CONS; i++) {
		pthread_join(cm[i], NULL);
	}

	return EXIT_SUCCESS;
}
