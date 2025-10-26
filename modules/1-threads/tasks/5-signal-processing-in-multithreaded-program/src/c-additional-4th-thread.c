#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>

// обработчик сигналов можно установить только для процесса, но не для каждого потока по отдельности
void handler(int sig) {
	pthread_t tid = pthread_self();
	printf("\n[HANDLER][THREAD=%lu]: Получен сигнал SIGINT (%d)\n\n", tid, sig);
}

void *thread_func1(void *arg) {
	pthread_t tid = pthread_self();

	while (1) {
		printf("\t[THREAD=%lu]: I am working...\n", tid);
		sleep(2);
	}
	return NULL;
}

void *thread_func2(void *arg) {
	pthread_t tid = pthread_self();

	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGINT);
	pthread_sigmask(SIG_UNBLOCK, &set, NULL);
	printf("\t[THREAD=%lu]: SIGINT разблокирован, жду сигналы...", tid);

	sigset_t block_set;
	sigemptyset(&block_set);
	sigaddset(&block_set, SIGQUIT);
	pthread_sigmask(SIG_BLOCK, &block_set, NULL); // для ясности, а так SIGQUIT уже заблокирован (унаследованно из main)

	int i = 0;

	while (1) {
		printf("\t[THREAD=%lu]: работаю и жду сигнал SIGINT\n", tid);
		if (i % 2 == 0) {
			pthread_sigmask(SIG_BLOCK, &set, NULL);
			printf("\t[THREAD=%lu]: заблокировал сигнал SIGINT\n", tid);
		}
		else {
			pthread_sigmask(SIG_UNBLOCK, &set, NULL);
			printf("\t[THREAD=%lu]: разблокировал сигнал SIGINT\n", tid);
		}
		sleep(1);
		i++;
	}
	return NULL;
}

void *thread_func3(void *arg) {
	pthread_t tid = pthread_self();

	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGQUIT);
	
	while (1) {
		int sig;
		sigwait(&set, &sig);
		printf("\t[THREAD=%lu]: Получил сигнал %d (SIGQUIT)\n", tid, sig);
	}
	return NULL;
}

void *thread_func4(void *arg) {
	pthread_t tid = pthread_self();
	
	sigset_t set;
	sigfillset(&set);
	pthread_sigmask(SIG_UNBLOCK, &set, NULL);

	printf("\n[THREAD=%lu]: Все сигналы разблокированы\n", tid);

	while (1) {
		printf("\n[THREAD=%lu]: Я работаю...\n", tid);
		sleep(2);
	}
	return NULL;
}

int main() {
	printf("\n[INFO]: PID=%d\n\n", getpid());

	sigset_t set;

	sigfillset(&set);
	pthread_sigmask(SIG_BLOCK, &set, NULL);

	signal(SIGINT, handler);

	pthread_t tids[4];
	void *(*thread_funcs[4])(void *) = {
		thread_func1,
		thread_func2,
		thread_func3,
		thread_func4
	};

	for (int i = 0; i < 4; i++) {
		if (pthread_create(&tids[i], NULL, thread_funcs[i], NULL) != 0) {
			printf("\n[ERROR]: creating a thread!\n\n");
		}
	}

	for (int i = 0; i < 4; i++) {
		pthread_join(tids[i], NULL);
	}

	return 0;
}
