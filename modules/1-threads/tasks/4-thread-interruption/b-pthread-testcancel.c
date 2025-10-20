#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

void *thread_function(void *arg) {
	int *i = (int *) arg;
	while (1) {
		(*i)++;
		pthread_testcancel();
	}
	return NULL;
}

int main() {
	pthread_t thread_id;

	int i = 0;
	
	int result = pthread_create(&thread_id, NULL, thread_function, (void *) &i);
	if (result != 0) {
		printf("[ERROR]: creating thread\n");
		return 1;
	}

	sleep(1);
	
	pthread_cancel(thread_id);

	void *ret_value;
	pthread_join(thread_id, &ret_value);

	if (ret_value == PTHREAD_CANCELED) {
		printf("[INFO]: thread was cancelled\n");
	}

	printf("\t[MAIN]: i=%d\n", i);
	
	return 0;
}
