#include <ucontext.h>

/**************************************************************************************
 *************************************** MACROS ***************************************
 **************************************************************************************/

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#define MAX_THREADS 8192   /* The MAX_INT of 32-bit systems just in case */
#define MAX_TID 8193  /* max int for 32 bit systems */
#define STACK_SIZE 8192     /* 8 KiB, used for the length of arrays */
#define EXIT 0          /* When a process has exited */
#define RUN 1           /* When currently running */
#define READY 2         /* When waiting in the runq */
#define WCHLD 3         /* When waiting on a specific child */
#define WALL 4          /* When waiting for all */
#define MAIN_TID 0      /* Thread ID of the main thread */


/**************************************************************************************
 ******************************** TYPEDEFS AND STRUCTS ********************************
 **************************************************************************************/


typedef struct __my_t {
    unsigned int tid;                   /* This thread's id */
    struct __my_t* parent;              /* Parent thread */
    int status;                         /* -### waiting on tid ### */
    //struct __my_t* child_list[MAX_THREADS];                 /* list of children processes */
    struct List* child_list;
    unsigned int ct_cnt;                /* Child thread count */
    ucontext_t context;                 /* The context of this thread */
    unsigned int wc_tid;                /* tid child thread we wait on if waiting on a single child */
} __my_t;

typedef struct Node {
    __my_t* t;
    struct Node* next;
    struct Node* prev;
} Node;

typedef struct List {
    Node* head;
    Node* tail;
    int length;
} List;

//typedef List Queue;

/*
typedef struct Queue {
    __my_t* t_list[MAX_THREADS];
    unsigned int length;
    unsigned int head;
    unsigned int tail;
} Queue;
*/


/**************************************************************************************
 ************************************* PROTOTYPES *************************************
 **************************************************************************************/

/*Queue* setup_queue();
void enqueue(Queue* q, __my_t* t);
int queue_is_empty(Queue* q);
int queue_length(Queue* q);
void print_queue(Queue* q);
__my_t* dequeue(Queue* q);
__my_t* remove_from_queue(Queue* q, unsigned int tid);
void remove_refs(Queue* q, __my_t* t);
*/

/*
 * List prototypes
 */
List* setup_list();
List* setup_queue();
void enqueue(List* l, __my_t* t);
__my_t* dequeue(List* l);
int is_empty(List* l);
int len(List* l);
__my_t* peek(List* l);
void remove_from_list(List* l, __my_t* t);
unsigned int __get_next_available_tid();
void __remove_refs(__my_t* t);
int __is_child(__my_t* parent, __my_t* child);
