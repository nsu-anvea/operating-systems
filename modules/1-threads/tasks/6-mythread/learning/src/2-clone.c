#define _GNU_SOURCE

#include <stdio.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sched.h>
#include <unistd.h>

int thread_function(void *arg) {
	printf("\t[THREAD]: Hello! arg=%s\n", (char *)arg);
	return 0;
}

int main() {

	int size = 1024 * 1024;
	void *stack = mmap(
		NULL,							// адрес (выбирает ядро)
		size,
		PROT_READ | PROT_WRITE,			// права: чтение и запись
		MAP_PRIVATE | MAP_ANONYMOUS,	// приватная анонимная память
		-1,								// файловый дескриптор (-1 для анонимной)
		0								// смещение
	);

	if (stack == MAP_FAILED) {
		perror("mmap failed");
	}

	printf("[INFO]: created stack with addr=%p", stack);

	
	int pid = clone(
		thread_function,
		stack + size,
		CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND,
		(void *)"Hello, World!"
	);
	
	if (pid == -1) {
		perror("clone failed");
		return 1;
	}
	
	printf("[INFO]: Created thread with pid=%d\n", pid);


	if (waitpid(pid, NULL, 0) == -1) {
		perror("waitpid failed");
	}


	printf("[INFO]: Thread finished\n");
	
	if (munmap(stack, size) == -1) {
		perror("munmap failed");
	}

	return 0;
}
