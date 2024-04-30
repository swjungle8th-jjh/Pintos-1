#ifndef THREADS_SYNCH_H
#define THREADS_SYNCH_H

#include <list.h>
#include <stdbool.h>

/* A counting semaphore. */
struct semaphore {
    unsigned value;      /* Current value. */
    struct list waiters; /* List of waiting threads. */
};

void sema_init(struct semaphore *, unsigned value);
void sema_down(struct semaphore *);      // down or P 연산, 세마 값 양수 될 때까지 기다렸다가 양수되면 1 빼기
bool sema_try_down(struct semaphore *);  // 기다리지 않고 down or P 연산, 성공하면 true return
void sema_up(struct semaphore *);  
void sema_self_test(void);

bool decrease_sema_func(const struct list_elem *a, const struct list_elem *b, void *aux);


/* Lock. */
struct lock {
    struct thread *holder;      /* Thread holding lock (for debugging). */
    struct semaphore semaphore; /* Binary semaphore controlling access. */
};

void lock_init(struct lock *);
void lock_acquire(struct lock *);      // 현재 쓰레드에서 락 획득, 기다려야하면 기다림
bool lock_try_acquire(struct lock *);  // 기다리지 않고 락을 얻으려고함
void lock_release(struct lock *);      // 락 소유 쓰레드만 락을 놓아줄 수 있다.
bool lock_held_by_current_thread(const struct lock *);

/* Condition variable. */
struct condition {
    struct list waiters; /* List of waiting threads. */
};

void cond_init(struct condition *);
void cond_wait(struct condition *, struct lock *);  // 락을 갖고 이 함수를 호출해야한다. 락을 놓아주고 cond가 신호 받는 것을 기다림, 신호 받으면 락 다시 획득
void cond_signal(struct condition *, struct lock *);     // 락을 갖고 이 함수를 호출해야한다. cond를 기다리는 쓰레드 중 하나를 깨움
void cond_broadcast(struct condition *, struct lock *);  // 락을 갖고 이 함수를 호출해야한다. cond를 기다리는 쓰레드가 있으면 모든 쓰레드를 깨움

/* Optimization barrier.
 *
 * The compiler will not reorder operations across an
 * optimization barrier.  See "Optimization Barriers" in the
 * reference guide for more information.*/
#define barrier() asm volatile("" : : : "memory")  // 최적화 장벽

#endif /* threads/synch.h */
