#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>

// обработчик сигналов можно установить только для процесса, но не для каждого потока по отдельности
void handler(int sig) {
	printf("\n[HANDLER]: Получен сигнал SIGINT (%d)\n\n", sig);
}

void *thread_func1(void *arg) {
	int *id = (int *) arg;

	while (1) {
		printf("\t[THREAD=%d]: I am working...\n", *id);
		sleep(2);
	}
}

void *thread_func2(void *arg) {
	int *id = (int *) arg;

	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGINT);
	pthread_sigmask(SIG_UNBLOCK, &set, NULL);
	printf("\t[THREAD=%d]: SIGINT разблокирован, жду сигналы...", *id);

	sigset_t block_set;
	sigemptyset(&block_set);
	sigaddset(&block_set, SIGQUIT);
	pthread_sigmask(SIG_BLOCK, &block_set, NULL); // для ясности, а так SIGQUIT уже заблокирован (унаследованно из main)

	while (1) {
		printf("\t[THREAD=%d]: работаю и жду сигнал SIGINT\n", *id);
		sleep(2);
	}
}

void *thread_func3(void *arg) {
	int *id = (int *) arg;

	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGQUIT);
	
	while (1) {
		int sig;
		sigwait(&set, &sig);
		printf("\t[THREAD=%d]: Получил сигнал %d (SIGQUIT)\n", *id, sig);
	}
}

int main() {
	printf("\n[INFO]: PID=%d\n\n", getpid());

	sigset_t set;

	sigfillset(&set);
	pthread_sigmask(SIG_BLOCK, &set, NULL);

	signal(SIGINT, handler);

	pthread_t tids[3];
	int tnums[3] = {1, 2, 3};
	void *(*thread_funcs[3])(void *) = {
		thread_func1,
		thread_func2,
		thread_func3
	};

	for (int i = 0; i < 3; i++) {
		if (pthread_create(&tids[i], NULL, thread_funcs[i], (void *) &tnums[i]) != 0) {
			printf("\n[ERROR]: creating a thread!\n\n");
		}
	}

	for (int i = 0; i < 3; i++) {
		pthread_join(tids[i], NULL);
	}

	return 0;
}
