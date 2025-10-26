#include <stdio.h>
#include <signal.h>
#include <unistd.h>

int main() {
	sigset_t set;
	int sig;

	sigemptyset(&set);
	sigaddset(&set, SIGINT);

	sigprocmask(SIG_BLOCK, &set, NULL);

	printf("PID: %d\n", getpid());
	printf("Ожидаю SIGINT... (Ctrl+C или kill -SIGINT)\n");

	sigwait(&set, &sig);

	printf("Получен сигнал %d\n", sig);

	return 0;
}
