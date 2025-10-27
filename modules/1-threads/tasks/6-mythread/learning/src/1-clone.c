#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sched.h>

int thread_function(void *arg) {
    printf("Привет из потока! Аргумент: %s\n", (char *) arg);
    return 42;
}

int main() {
    void *stack = malloc(1024 * 1024);
    if (!stack) {
        perror("malloc");
        return 1;
    }

    void *stack_top = stack + 1024 * 1024;

    int tid = clone(
        thread_function,
        stack_top,
        CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND,
        "test"
    );

    if (tid == -1) {
        perror("clone");
        free(stack);
        return 1;
    }
    
    printf("Поток создан с tid: %d\n", tid);
    fflush(stdout);

    int status;
    int result = waitpid(tid, &status, 0);
    
    if (result == -1) {
        perror("waitpid failed");
    }
    else {
        printf("waitpid успешен, код выхода: %d\n", WEXITSTATUS(status));
    }

    free(stack);

    return 0;
}