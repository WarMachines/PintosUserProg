#include "threads/thread.h"

#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

void syscall_init (void);

struct process_file {
	struct file* file_ptr;
	int fd_num;
	struct list_elem elem;
};

struct process_file* file_search(struct list* files, int fd_num);       // search the open file list of a thread
int open_file(struct file* file_ptr);                                   // to wrap open functionality into single function instead of writing int he case itself
void close_file(struct list* file_list, int fd_num);                    // to wrap close functionality into single function instead of writing int he case itself
void exit_process(int exit_code);                                       // to wrap exit functionality into single function instead of writing int he case itself
void validate_user_address(int* ptr);                                   // to validate all the stack addresses required by user process for every case as a single point of check instead of checking in every case for better understanding
void* is_valid_address(void* address);  								// it validates a single address pointed by a pointer


#endif /* userprog/syscall.h */
