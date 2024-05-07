#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/loader.h"
#include "userprog/gdt.h"
#include "userprog/process.h"
#include "threads/flags.h"
#include "intrinsic.h"

#include "filesys/filesys.h"

void syscall_entry(void);
void syscall_handler(struct intr_frame *);

//-- system call
void halt();
void exit(int status);
bool create(const char *, unsigned);
bool remove(const char *);
int open(const char *);
void close(int);

/* System call.
 *
 * Previously system call services was handled by the interrupt handler
 * (e.g. int 0x80 in linux). However, in x86-64, the manufacturer supplies
 * efficient path for requesting the system call, the `syscall` instruction.
 *
 * The syscall instruction works by reading the values from the the Model
 * Specific Register (MSR). For the details, see the manual. */

#define MSR_STAR 0xc0000081			/* Segment selector msr */
#define MSR_LSTAR 0xc0000082		/* Long mode SYSCALL target */
#define MSR_SYSCALL_MASK 0xc0000084 /* Mask for the eflags */

void syscall_init(void)
{
	write_msr(MSR_STAR, ((uint64_t)SEL_UCSEG - 0x10) << 48 |
							((uint64_t)SEL_KCSEG) << 32);
	write_msr(MSR_LSTAR, (uint64_t)syscall_entry);

	/* The interrupt service rountine should not serve any interrupts
	 * until the syscall_entry swaps the userland stack to the kernel
	 * mode stack. Therefore, we masked the FLAG_FL. */
	write_msr(MSR_SYSCALL_MASK,
			  FLAG_IF | FLAG_TF | FLAG_DF | FLAG_IOPL | FLAG_AC | FLAG_NT);
}

bool check_address(void *addr)
{
	struct thread *cur = thread_current();
	if (addr == NULL || !is_user_vaddr(addr) || !(pml4_get_page(cur->pml4, addr)))
	{
		return false;
	}
	return true;
}
/* The main system call interface */
/*
1. UNUSED 제거
*/
void syscall_handler(struct intr_frame *f)
{
	// TODO: Your implementation goes here.
	// 스시템콜 핸들러로 등록을 해야함 .

	int number = f->R.rax;

	switch (number)
	{
	case SYS_HALT: /* Halt the operating system. */
		halt();
		break;
	case SYS_EXIT: /* Terminate this process. */
		exit(f->R.rdi);
		break;
	case SYS_FORK: /* Clone current process. */
		/* code */
		break;
	case SYS_EXEC: /* Switch current process. */
		/* code */
		break;
	case SYS_WAIT: /* Wait for a child process to die. */
		/* code */
		break;
	case SYS_CREATE: /* Create a file. */
		f->R.rax = create(f->R.rdi, f->R.rsi);
		break;
	case SYS_REMOVE: /* Delete a file. */
		f->R.rax = remove(f->R.rdi);
		break;
	case SYS_OPEN: /* Open a file. */
		f->R.rax = open(f->R.rdi);
		break;
	case SYS_FILESIZE: /* Obtain a file's size. */
		/* code */
		break;
	case SYS_READ: /* Read from a file. */
		/* code */
		break;
	case SYS_WRITE: /* Write to a file. */
		/* code */
		printf("%s", f->R.rsi); /*테스트 통과를 위한 임시코드?*/
		break;
	case SYS_SEEK: /* Change position in a file. */
		/* code */
		break;
	case SYS_TELL: /* Report current position in a file. */
		/* code */
		break;
	case SYS_CLOSE: /* Close a file. */
		close(f->R.rdi);
		break;
	default:
		break;
	}

	// printf("system call number-> %lld \n", f->R.rax);
}

void halt()
{
	power_off();
}

void exit(int status)
{
	printf("%s: exit(%d)\n", thread_current()->name, status);
	// printf("%s: exit(%d)\n", thread_current()->name, thread_current()->tf.R.rdi);

	thread_exit();
}

bool create(const char *file, unsigned initial_size)
{

	if (file == NULL || !check_address(file))
	{
		exit(-1);
	}
	return filesys_create(file, initial_size);
}

bool remove(const char *file)
{
	if (file == NULL || !check_address(file))
	{
		exit(-1);
	}
	return filesys_remove(file);
}

int open(const char *file)
{
	// printf("파일 포인터 %s\n", file);
	if (file == NULL || !check_address(file))
	{
		exit(-1);
	}
	struct file *open_file = filesys_open(file);

	if (open_file == NULL)
	{
		return -1;
	}
	return process_add_file(open_file);
}

void close(int fd)
{
	process_close_file(fd);
}