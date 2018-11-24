#include "threads/thread.h"

#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

void syscall_init (void);

struct process_file {
	struct file* file_ptr;
	int fd_num;
	struct list_elem elem;
};

struct process_file* file_search(struct list* files, int fd_num);
int open_file(struct file* file_ptr);
void close_file(struct list* file_list, int fd_num);


#endif /* userprog/syscall.h */
