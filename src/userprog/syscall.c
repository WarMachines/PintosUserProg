#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "process.h"
#include "threads/vaddr.h"
#include "threads/malloc.h"

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
	switch (system_call)
	{
		// case SYS_HALT:
		// shutdown_power_off();
		// break;

		case SYS_EXIT:
		thread_exit();
		break;

		case SYS_WRITE:
		if(*(p+1)==1)
		{
			putbuf(*(p+2),*(p+3));
			f->eax = *(p+3);
		}
		else
		{
			struct process_file* f_ptr = file_search(&thread_current()->file_list, *(p+1));
			if(f_ptr==NULL)
				f->eax=-1;
			else
				f->eax = file_write_at (f_ptr->file_ptr, *(p+2), *(p+3),0);
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
		//hex_dump(*(p+1),*(p+1),16,true);
		//printf("%s",(char *)*(p+1));
		f->eax = filesys_create(*(p+1),*(p+2));
		// printf("%d",s);
		//release_filesys_lock();
	
		
		break;
		}

		case SYS_OPEN:
		{
			struct file* fptr = filesys_open (*(p+1));
			if(fptr==NULL)
				f->eax = -1;
			else
			{
				struct process_file *pfile = malloc(sizeof(*pfile));
				pfile->file_ptr = fptr;
				pfile->fd_num = thread_current()->fd_num;
				thread_current()->fd_num++;
				list_push_back (&thread_current()->file_list, &pfile->elem);
				f->eax = pfile->fd_num;
			}
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


struct process_file* file_search(struct list* files, int fd_num)
{
	struct list_elem *e;
       for (e = list_begin (files); e != list_end (files);
           e = list_next (e))
        {
          struct process_file *f = list_entry (e, struct process_file, elem);
          if(f->fd_num == fd_num)
          	return f;
        }
   return NULL;
}