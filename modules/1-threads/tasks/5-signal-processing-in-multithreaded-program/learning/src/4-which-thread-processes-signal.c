#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>

void handler1(int sig) {
	printf("[HANDLER=1]: catch the signal %d (SIGINT)\n", sig);
}

void handler2(int sig) {
	printf("[HANDLER=2]: catch the signal %d (SIGINT)\n", sig);
}

void *thread_func(void *arg) {
    int *num = (int *) arg;
    
    sleep(*num);  // Поток 2 подождёт 2 секунды!
    
    if (*num == 1) {
        printf("[THREAD=1]: Устанавливаю handler1\n");
        signal(SIGINT, handler1);
    }
    else {
        printf("[THREAD=2]: Устанавливаю handler2\n");
        signal(SIGINT, handler2);
    }
    
    while (1) {
        printf("[THREAD=%d]: are working...\n", *num);
        sleep(2);
    }
    return NULL;
}

int main() {
	pthread_t tid1, tid2;

	int n1 = 1, n2 = 2;

	if (pthread_create(&tid1, NULL, thread_func, (void *) &n1) != 0) {
		printf("\n[ERROR]: creating thread\n\n");
		return 1;
	}

	if (pthread_create(&tid2, NULL, thread_func, (void *) &n2) != 0) {
		printf("\n[ERROR]: creating thread\n\n");
		return 1;
	}

	pthread_join(tid1, NULL);
	pthread_join(tid2, NULL);
	
	return 0;
}
