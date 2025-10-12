#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

// Функция, которую будет выполнять поток
void* thread_function(void* arg) {
    int* thread_id = (int*)arg;
    printf("Поток %d начал работу\n", *thread_id);
    
    // Имитация работы потока
    for (int i = 0; i < 3; i++) {
        printf("Поток %d: шаг %d\n", *thread_id, i);
        sleep(1); // Задержка 1 секунда
    }
    
    printf("Поток %d завершил работу\n", *thread_id);
    //int *result = (int *)malloc(sizeof(int));
    //*result = 42;
    //return (void*)result;

    char *str = (char *)malloc(sizeof(char) * 100);
    strcpy(str, "Hello, World!");

    return (void *)str;
}

int main() {
    pthread_t thread; // Идентификатор потока
    int thread_id = 1;
    char* thread_result;
    
    printf("Основной поток начал работу\n");
    
    // Создаем новый поток
    if (pthread_create(&thread, NULL, thread_function, &thread_id)) {
        perror("Ошибка при создании потока");
        return EXIT_FAILURE;
    }
    
    printf("Основной поток ждет завершения созданного потока...\n");
    
    // Ожидаем завершения потока
    if (pthread_join(thread, (void**)&thread_result)) {
        perror("Ошибка при ожидании потока");
        return EXIT_FAILURE;
    }
    //printf("thread-result = %d\n", *thread_result);
    printf("thread-result-string = %s\n", thread_result);
    free(thread_result);
    
    printf("Основной поток завершил работу\n");
    return EXIT_SUCCESS;
}
