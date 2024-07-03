struct bucket {
    string key; 
    void *binding; 
    struct bucket *next;
};
#define SIZE 109
struct bucket *table[SIZE];

unsigned int hash(char *s0) {
    unsigned int h=0; char *s;
    for(s=s0; *s; s++)
        h = h*65599 + *s;
    return h;
}

struct bucket *Bucket(string key, void *binding, struct bucket *next) {
    struct bucket *b = checked_malloc(sizeof(*b));
    b->key=key; 
    b->binding=binding; 
    b->next=next;
    return b;
}

void insert(string key, void *binding) {
    int index = hash(key) % SIZE;
    table[index] = Bucket(key, binding, table[index]);
}

void *lookup(string key) {
    int index = hash(key) % SIZE;
    struct bucket *b;
    for(b=table[index]; b; b=b->next)
    if (0==strcmp(b->key,key)) return b->binding;
    return NULL;
}

void pop(string key) {
    int index = hash(key) % SIZE;
    table[index] = table[index]->next;
}