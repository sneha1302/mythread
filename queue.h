typedef struct {
    int length;
    MyThread* head;
    MyThread* tail;
} Queue;
Queue* setup_queue();
voi(Queue* q, int data);
int queue_is_empty(Queue* q);
int queue_length(Queue* q); 
MyThread* dequeue(Queue* q);
