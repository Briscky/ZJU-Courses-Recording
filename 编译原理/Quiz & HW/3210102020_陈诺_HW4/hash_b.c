struct bucket {
    string key; 
    void *binding; 
    struct bucket *next;
};
#define SIZE 109
struct bucket *table[SIZE];

void insert(int seq, string key, void *binding) {
    int index = hash(key) % SIZE;
    table[seq][index] = Bucket(key, binding, table[index]);
}

void *lookup(int seq, string key) {
    int index = hash(key) % SIZE;
    struct bucket *b;
    for(b=table[seq][index]; b; b=b->next)
        if (0==strcmp(b->key,key)) 
            return b->binding;
    return NULL;
}