#define _GNU_SOURCE

#include <stdio.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sched.h>
#include <unistd.h>
#include <stdlib.h>

#define STACK_SIZE 1024 * 1024



#define INFO_PRINT(fmt, ...) printf("[INFO]: " fmt, ##__VA_ARGS__)

#ifdef DEBUG
    #define DEBUG_PRINT(fmt, ...) printf("[DEBUG]: " fmt, ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(fmt, ...)
#endif



typedef struct {
	size_t 	size;
	void *	arr_ptr;
} mystack_t;

typedef struct mythread_t {
	int 		pid;
	mystack_t *	stack;
	void *		retv;
} mythread_t;

typedef struct {
	void *(*user_fn)(void *);

	void *	user_arg;
	void **	fn_retv;
} mythread_wrapper_t;



mythread_wrapper_t *create_mythread_wrapper(mythread_t *mythread, void *(*user_fn)(void *), void *user_arg) {
	mythread_wrapper_t *mythread_wrapper = (mythread_wrapper_t *)malloc(sizeof(mythread_wrapper_t));
	if (!mythread_wrapper) return mythread_wrapper;

	mythread_wrapper->user_fn = user_fn;
	mythread_wrapper->user_arg = user_arg;
	mythread_wrapper->fn_retv = &mythread->retv;

	return mythread_wrapper;
}

void delete_mythread_wrapper(mythread_wrapper_t *mythread_wrapper) {
	free(mythread_wrapper);
}

void *(*mythread_wrapper_get_user_fn(mythread_wrapper_t *mythread_wrapper))(void *) {
	return mythread_wrapper->user_fn;
}

void *mythread_wrapper_get_user_arg(mythread_wrapper_t *mythread_wrapper) {
	return mythread_wrapper->user_arg;
}

void **mythread_wrapper_get_fn_retv(mythread_wrapper_t *mythread_wrapper) {
	return mythread_wrapper->fn_retv;
}

void mythread_wrapper_set_user_fn(mythread_wrapper_t *mythread_wrapper, void *(*user_fn)(void *)) {
	mythread_wrapper->user_fn = user_fn;
}

void mythread_wrapper_set_user_arg(mythread_wrapper_t *mythread_wrapper, void *user_arg) {
	mythread_wrapper->user_arg = user_arg;
}

void mythread_wrapper_set_fn_retv(mythread_wrapper_t *mythread_wrapper, void **fn_retv) {
	mythread_wrapper->fn_retv = fn_retv;
}





mystack_t *mystack_create(size_t size) {
	void *stack = mmap(
		NULL,
		size,
		PROT_READ | PROT_WRITE,
		MAP_PRIVATE | MAP_ANONYMOUS,
		-1,
		0
	);

	if (stack == MAP_FAILED) {
		perror("mmap failed");
		return NULL;
	}

	mystack_t *s = (mystack_t *)malloc(sizeof(mystack_t));
    if (!s) {
        perror("malloc for mystack_t");
        munmap(stack, size);
        return NULL;
    }
	s->size = size;
	s->arr_ptr = stack;

	return s;
}

int mystack_delete(mystack_t *stack) {
	if (munmap(stack->arr_ptr, stack->size) == -1) {
		perror("munmap failed");
		return -1;
	}
    free(stack);
	return 0;
}

size_t mystack_get_size(mystack_t *mystack) {
	return mystack->size;
}

void *mystack_get_arr_ptr(mystack_t *mystack) {
	return mystack->arr_ptr;
}





int mythread_wrapper_fn(void *arg) {
	mythread_wrapper_t *mythread_wrapper = (mythread_wrapper_t *)arg;
    DEBUG_PRINT("mythread_wrapper_fn have got mythread_wrapper\n");

    INFO_PRINT("user_fn have started\n");
	void *(*user_fn)(void *) = mythread_wrapper_get_user_fn(mythread_wrapper);
	void *result = user_fn(mythread_wrapper_get_user_arg(mythread_wrapper));
    DEBUG_PRINT("user_fn returned: %p\n", result);

	void **fn_retv = mythread_wrapper_get_fn_retv(mythread_wrapper);
    *(fn_retv) = result;
    DEBUG_PRINT("stored result at address: %p, value: %p\n", fn_retv, *fn_retv);
    INFO_PRINT("user_fn have finished\n");

    delete_mythread_wrapper(mythread_wrapper);
    DEBUG_PRINT("mythread_wrapper have been deleted\n");

	return 0;
}

int mythread_create(mythread_t *mythread, void *(*start_routine)(void *), void *arg) {
	mythread_set_stack(mythread, mystack_create(STACK_SIZE));
	if (!mythread_get_stack(mythread)) {
		perror("mystack_create failed");
		return EXIT_FAILURE;
	}
    DEBUG_PRINT("stack have been created\n");

	mythread_wrapper_t *mythread_wrapper = create_mythread_wrapper(mythread, start_routine, arg);
    if (!mythread_wrapper) {
        perror("create_mythread_wrapper failed");
        mystack_delete(mythread);
        return EXIT_FAILURE;
    }
    DEBUG_PRINT("mythread_wrapper have been created\n");

	mythread_set_pid(
		mythread,
		clone(
			mythread_wrapper_fn,
			mystack_get_arr_ptr(mythread_get_stack(mythread)) + mystack_get_size(mythread_get_stack(mythread)),
			CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | SIGCHLD,
			(void *)mythread_wrapper
		)
	);
	if (mythread_get_pid(mythread) == -1) {
		perror("clone failed");
		return EXIT_FAILURE;
	}
    DEBUG_PRINT("clone have been executed\n");

	return EXIT_SUCCESS;
}

int mythread_join(mythread_t *mythread, void **retv) {
	if (waitpid(mythread_get_pid(mythread), NULL, 0) == -1) {
		perror("waitpid failed");
		return EXIT_FAILURE;
	}
    INFO_PRINT("have waited for the mythread to finish\n");

	*retv = mythread_get_retv(mythread);
    DEBUG_PRINT("have stored value=%p\n", mythread_get_retv(mythread));

	if (mystack_delete(mythread_get_stack(mythread)) != EXIT_SUCCESS) {
		perror("mystack_delete failed");
	}
    INFO_PRINT("have got the message from the mythread\n");

	return EXIT_SUCCESS;
}

int mythread_get_pid(mythread_t *mythread) {
	return mythread->pid;
}

mystack_t *mythread_get_stack(mythread_t *mythread) {
	return mythread->stack;
}

void *mythread_get_retv(mythread_t *mythread) {
	return mythread->retv;
}

void mythread_set_pid(mythread_t *mythread, int pid) {
	mythread->pid = pid;
}

void mythread_set_stack(mythread_t *mythread, mystack_t *mystack) {
	mythread->stack = mystack;
}

void mythread_set_retv(mythread_t *mythread, void *retv) {
	mythread->retv = retv;
}




void *mythread_fn(void *arg) {
	printf("\t[MYTHREAD]: I've started!\n");
	printf("\t[MYTHREAD]->[GOT MESSAGE]: %s\n", (char *)arg);
	sleep(1);
	printf("\t[MYTHREAD]: I've finished!\n");
	return (void *)"Hello from mythread!";
}

int main() {
    printf("\n\n\n");

	mythread_t mythread;
	if (mythread_create(&mythread, mythread_fn, (void *)"Hello from main!") != EXIT_SUCCESS) {
		perror("mythread_create failed");
		return EXIT_FAILURE;
	}
    INFO_PRINT("created mythread with pid=%d\n", mythread.pid);

	char *message;
	if (mythread_join(&mythread, (void **)&message) != EXIT_SUCCESS) {
		perror("mythread_join failed");
	}
    INFO_PRINT("mythread with pid=%d have been joined\n", mythread.pid);

	printf("\t[MAIN]->[GOT MESSAGE]: %s\n", message);

    printf("\n\n\n");
	return EXIT_SUCCESS;
}