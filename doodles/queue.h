typedef struct Node Node;
struct Node {
    int data; 
    Node* next_node = NULL;
}

typedef struct Queue queue;
struct Queue {
    Node* head = NULL;
    Node* tail = NULL;
    int length = 0;
}


