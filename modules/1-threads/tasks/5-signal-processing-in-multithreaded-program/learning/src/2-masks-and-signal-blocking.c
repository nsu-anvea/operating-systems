#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void handler(int signum) {
    printf("\n[Обработчик] Получен SIGINT!\n");
}

int main() {
    signal(SIGINT, handler);
    
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);  // Добавляем SIGINT в набор
    
    printf("PID: %d\n", getpid());
    
    printf("1. Попробуйте Ctrl+C сейчас\n");
    sleep(3);
    
    printf("2. Блокирую SIGINT...\n");
    sigprocmask(SIG_BLOCK, &set, NULL);  // Блокируем!
    
    printf("3. Попробуйте Ctrl+C сейчас (5 секунд)\n");
    sleep(5);
    
    printf("4. Разблокирую SIGINT...\n");
    sigprocmask(SIG_UNBLOCK, &set, NULL);  // Разблокируем!
    
    printf("5. Жду...\n");
    sleep(5);
    
    return 0;
}
