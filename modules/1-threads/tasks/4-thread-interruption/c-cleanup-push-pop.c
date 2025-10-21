#include <stdio.h>
#include <stdlib.h> // for malloc
#include <unistd.h> // for sleep
#include <string.h> // for strlen, %s, strcpy
#include <pthread.h>


char *create_str(int len) {
	char *str = (char *) malloc(sizeof(char) * (len + 1));
	return str;
}

void init_str(char *str, char *message) {
	strcpy(str, message);
}

void destroy_str(char *str) {
	free(str);
}

void cleanup_function(void *arg) {
	printf("\n[INFO]: cleaning started!\n\n");
	destroy_str((char *) arg);
	printf("\n[INFO]: cleaning finished!\n\n");
}

void *thread_function(void *arg) {
	pthread_t id = pthread_self();
	printf("\t\t[THREAD=%lu]: have started the work!\n", id);

	pthread_cleanup_push(cleanup_function, arg);

	char *str = (char *) arg;
	while (1) {
		printf("\t\t[THREAD]: %s\n", str);
	}
	
	pthread_cleanup_pop(1);

	printf("\t\t[THREAD=%lu]: have finished the work!\n", id);
	pthread_exit(NULL);
}


int main() {
	pthread_t thread_id;

	char message[] = "Hello, World!";

	char *str = create_str(strlen(message));
	init_str(str, message);

	printf("\t[MAIN]: I've created str=%s\n", str);

	int result = pthread_create(&thread_id, NULL, thread_function, (void *) str);
	if (result != 0) {
		printf("[ERROR]: creating thread\n");
		return 1;
	}
	usleep(10000);

	pthread_cancel(thread_id);
	pthread_join(thread_id, NULL);
	
	return 0;
}
