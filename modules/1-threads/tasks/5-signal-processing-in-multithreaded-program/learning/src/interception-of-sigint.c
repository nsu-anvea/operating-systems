#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void handler(int sig) {
	printf("\nПолучен сигнал %d (SIGINT)\n", sig);
}

int main() {
	// Устанавливаем обработчик для SIGINT
	signal(SIGINT, handler);

	printf("Программа запущена. Нажмите Ctrl+C\n");
	printf("Моя PID: %d\n", getpid());

	while(1) {
		printf("Работаю...\n");
		sleep(2);
	}

	return 0;
}
