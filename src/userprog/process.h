#ifndef USERPROG_PROCESS_H
#define USERPROG_PROCESS_H

#include "threads/thread.h"

tid_t process_execute (const char *file_name);
int process_wait (tid_t);
void process_exit (void);
void process_activate (void);

static void extract_file_name(char *name, char *file_name);    // extract just the file name from the arguments
static void get_arg_from_commandline(char *name, char* argv[], int *argc);
static void push_arguments_on_stack(void **esp, char *file_name);    // push the argument in correct order as they are required

#endif /* userprog/process.h */
