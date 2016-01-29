#include <ucontext.h>

/**************************************************************************************
 *************************************** MACROS ***************************************
 **************************************************************************************/

#ifndef TRUE
#define TRUE 1              // Boolean true
#endif
#ifndef FALSE
#define FALSE 0             // Boolean false
#endif

#define MAX_THREADS 8192    // The MAX_INT of 32-bit systems just in case
#define MAX_TID 8193        // max int for 32 bit systems 
#define STACK_SIZE 8192     // 8 KiB, used for the length of arrays
#define EXIT 0              // When a process has exited 
#define RUN 1               // When currently running 
#define READY 2             // When waiting in the runq
#define WCHLD 3             // When waiting on a specific child
#define WALL 4              // When waiting for all children
#define MAIN_TID 0          // Thread ID of the main thread


/**************************************************************************************
 ******************************** TYPEDEFS AND STRUCTS ********************************
 **************************************************************************************/

// Thread container
typedef struct __my_t {
    unsigned int tid;       // This thread's id
    struct __my_t* parent;  // Parent thread 
    int status;             // Thread status. Refer above
    struct List* child_list // List of children
    unsigned int ct_cnt;    // Child thread count
    ucontext_t context;     // The context of this thread
    unsigned int wc_tid;    // TID of child thread we wait on if WCHLD
} __my_t;

// Linked list node 
typedef struct Node {
    __my_t* t;              // Node's payload (the thread)                   
    struct Node* next;      // Next node
    struct Node* prev;      // Previous node
} Node;

// Linked List
typedef struct List {
    Node* head;             // Beginning of the list
    Node* tail;             // End of the list
    int length;             // Duh
} List;


/**************************************************************************************
 ************************************* PROTOTYPES *************************************
 **************************************************************************************/


// List prototypes
List* setup_list();
List* setup_queue();
void enqueue(List* l, __my_t* t);
__my_t* dequeue(List* l);
int is_empty(List* l);
int len(List* l);
__my_t* peek(List* l);
void remove_from_list(List* l, __my_t* t); // remove was already reserved from glibc

// Internal helper functions
unsigned int __get_next_available_tid();
void __remove_refs(__my_t* t);
int __is_child(__my_t* parent, __my_t* child);
