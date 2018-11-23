#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "process.h"
#include "threads/vaddr.h"

static void syscall_handler (struct intr_frame *);
// static int memoryread_user (void *src, void *des, size_t bytes);
//static int get_user (const uint8_t *uaddr);
extern bool running;

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  int * p = f->esp;


  int system_call = * p;
  int s;
	switch (system_call)
	{
		// case SYS_HALT:
		// shutdown_power_off();
		// break;

		case SYS_EXIT:
		thread_exit();
		break;

		case SYS_WRITE:
		if(*(p+5)==1)
		{
			putbuf(*(p+6),*(p+7));
		}
		break;

		case SYS_CREATE:
		{
		// const char* filename;
      	// unsigned initial_size;


		// memoryread_user(f->esp + 4, &filename, sizeof(filename));
      	// memoryread_user(f->esp + 8, &initial_size, sizeof(initial_size));

      	// s=f->eax = filesys_create(filename, initial_size);
		// thread_current()->exit_code = -1;
		// thread_exit();
		//acquire_filesys_lock();
		
		s=f->eax = filesys_create(*(p+4),*(p+5));
		// printf("%d",s);
		//release_filesys_lock();
	
		
		break;
		}

		default:
		printf("No match\n");
	}
}

// static int
// memoryread_user (void *src, void *dst, size_t bytes)
// {
//   int32_t value;
//   size_t i;
//   for(i=0; i<bytes; i++) {
//     value = get_user(src + i);
//     // if(value == -1) // segfault or invalid memory access
//     //   fail_invalid_access();

//     *(char*)(dst + i) = value & 0xff;
//   }
//   return (int)bytes;
// }

// static int
// get_user (const uint8_t *uaddr)
// {
// int result;
// asm ("movl $1f, %0; movzbl %1, %0; 1:"
// : "=&a" (result) : "m" (*uaddr));
// return result;
// }