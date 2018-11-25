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

  is_valid_address(p);						//check the stack pointer itself before moving forward for bad stack pointer tests
  int system_call = * p;

  validate_user_address(p);								// validate user address referenceces for the type of system call before dereferencing them

	switch (system_call)
	{
		// case SYS_HALT:
		// shutdown_power_off();
		// break;

		case SYS_EXIT:
		exit_process(*(p+1));
		break;

		case SYS_WRITE:			// first parameter is file descriptor
		{
			if(*(p+1)==1)
			{
				putbuf(*(p+2),*(p+3));
				f->eax = *(p+3);
			}
			else
			{
				struct process_file* file_ptr = file_search(&thread_current()->file_list, *(p+1));
				if(file_ptr==NULL)
					f->eax=-1;
				else
				{
					sema_down(&filesys_sema);
					f->eax = file_write (file_ptr->file_ptr, *(p+2), *(p+3));
					sema_up(&filesys_sema);
				}
			}
			break;
		}

		case SYS_CREATE:      // First parameter name of the file , second initial size of the file
		{
			sema_down(&filesys_sema);
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
			sema_up(&filesys_sema);
			break;
		}

		case SYS_OPEN:
		{
			sema_down(&filesys_sema);
			struct file* file_ptr = filesys_open (*(p+1));  // return file with the given name
			sema_up(&filesys_sema);
			if(file_ptr == NULL)							// if no file exist with the given name
				f->eax = -1;
			else
			{
				f->eax = open_file(file_ptr);
			}
			
			break;
		}

		case SYS_READ:
		{
			if(*(p+1)==0)
			{
				int i;
				uint8_t* buffer = *(p+2);
				for(i=0;i<*(p+3);i++)
					buffer[i]= input_getc();
				f->eax = *(p+3);
			}
			else
			{
				struct process_file* file_ptr = file_search(&thread_current()->file_list, *(p+1));
				if(file_ptr==NULL)
					f->eax=-1;
				else
				{
					sema_down(&filesys_sema);
					f->eax = file_read (file_ptr->file_ptr, *(p+2), *(p+3));
					sema_up(&filesys_sema);
				}
			}			
			break;
		}

		case SYS_FILESIZE:
		{
			sema_down(&filesys_sema);
			struct process_file* file_ptr = file_search(&thread_current()->file_list, *(p+1));
			f->eax = file_length (file_ptr->file_ptr);
			sema_up(&filesys_sema);
			break;
		}
		
		case SYS_CLOSE:
		{
			sema_down(&filesys_sema);
			close_file(&thread_current()->file_list,*(p+1));
			sema_up(&filesys_sema);
			break;	
		}

		case SYS_EXEC:
		{
			sema_down(&filesys_sema);
			f->eax = process_execute(*(p+1));
			sema_up(&filesys_sema);
			break;
		}

		case SYS_WAIT:
		{
			f->eax = process_wait(*(p+1));
			break;
		}

		case SYS_SEEK:
		{
			sema_down(&filesys_sema);
			file_seek(file_search(&thread_current()->file_list, *(p+1))->file_ptr,*(p+2));
			sema_up(&filesys_sema);
			break;
		}

		case SYS_TELL:
		{
			sema_down(&filesys_sema);
			f->eax = file_tell(file_search(&thread_current()->file_list, *(p+1))->file_ptr);
			sema_up(&filesys_sema);
			break;
		}

		case SYS_REMOVE:
		{			
			sema_down(&filesys_sema);
			if(filesys_remove(*(p+1))==NULL)
				f->eax = false;
			else
				f->eax = true;
			sema_up(&filesys_sema);
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


struct process_file* file_search(struct list* file_list, int fd_num)
{
	struct list_elem *e;
       for (e = list_begin (file_list); e != list_end (file_list);
           e = list_next (e))
        {
          struct process_file *f = list_entry (e, struct process_file, elem);
          if(f->fd_num == fd_num)
          	return f;
        }
   return NULL;
}

int open_file(struct file* file_ptr)
{
	struct process_file *pfile = malloc(sizeof(*pfile));
	pfile->file_ptr = file_ptr;
	pfile->fd_num = thread_current()->fd_num;
	thread_current()->fd_num++;
	list_push_back (&thread_current()->file_list, &pfile->elem);
	return pfile->fd_num;
}

void close_file(struct list* file_list, int fd_num)
{
	struct list_elem *e;
       for (e = list_begin (file_list); e != list_end (file_list);
           e = list_next (e))
        {
          struct process_file *f = list_entry (e, struct process_file, elem);
          if(f->fd_num == fd_num)
          {
          	file_close(f->file_ptr);     // close the file
          	list_remove(e);				 // remove the file from open file list of thread or process
			free(f);
			break;
          }
        }
}

void exit_process(int exit_code)
{
	struct list_elem *e;
	

      for (e = list_begin (&thread_current()->parent_thread->child_process_list); e != list_end (&thread_current()->parent_thread->child_process_list);
           e = list_next (e))
        {
          if(list_entry (e, struct child_thread_info, list_elem)->tid == thread_current()->tid)
          {
          	list_entry (e, struct child_thread_info, list_elem)->exit_code = exit_code;          // it will give parent information about the exit status of child process
			list_entry (e, struct child_thread_info, list_elem)->already_waited = true;
          }
        }
	thread_current()->exit_code = exit_code;			// set exit code to current threads exit code 

	if(thread_current()->parent_thread->waiting_child_tid == thread_current()->tid)			// start the parent process if child process its waiting on has completed execution
		sema_up(&thread_current()->parent_thread->child_sema);

	thread_exit();
}

void closed_open_files(struct list* file_list)
{
	struct list_elem *e;
	while(!list_empty(file_list))
	{
		e = list_pop_front(file_list);
		struct process_file *f = list_entry (e, struct process_file, elem);          
		file_close(f->file_ptr);
		list_remove(e);
		free(f);
	}

      
}

void validate_user_address(int* ptr)
{
	switch(*ptr)
	{
		case SYS_EXIT:
		case SYS_WAIT:
		case SYS_FILESIZE:
		case SYS_TELL:
		case SYS_CLOSE:
		{
			is_valid_address(ptr+1);
			break;
		}

		case SYS_EXEC:
		case SYS_REMOVE:
		case SYS_OPEN:
		{
			is_valid_address(ptr+1);
			is_valid_address(*(ptr+1));
			break;
		}

		case SYS_CREATE:
		{
			is_valid_address(ptr+2);
			is_valid_address(*(ptr+1));
			break;
		}

		case SYS_SEEK:
		{
			is_valid_address(ptr+2);
			break;
		}

		case SYS_WRITE:
		case SYS_READ:
		{
			is_valid_address(ptr+3);
			is_valid_address(*(ptr+2));
			break;
		}
	}
}

void* is_valid_address(void* address)
{
	if(is_user_vaddr(address) && pagedir_get_page(thread_current()->pagedir, address))
	return;

	exit_process(-1);
	return;
}