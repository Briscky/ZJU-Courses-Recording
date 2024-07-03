struct bucket {
    string key; 
    void *binding; 
    struct bucket *next;
};
#define SIZE 109
struct bucket **table;
int table_size = SIZE;
int num_elements = 0;

struct bucket *Bucket(string key, void *binding, struct bucket *next) {
    struct bucket *b = checked_malloc(sizeof(*b));
    b->key=key; 
    b->binding=binding; 
    b->next=next;
    return b;
}

void rehash() {
    int old_size = table_size;
    struct bucket **old_table = table;

    table_size *= 2;
    table = checked_malloc(sizeof(*table) * table_size);
    memset(table, 0, sizeof(*table) * table_size);

    for (int i = 0; i < old_size; i++) {
        struct bucket *b = old_table[i];
        while (b) {
            struct bucket *next = b->next;
            int index = hash(b->key) % table_size;
            b->next = table[index];
            table[index] = b;
            b = next;
        }
    }

    free(old_table);
}

void insert(string key, void *binding) {
    num_elements++;

    if (num_elements / table_size > 2) {
        rehash();
    }

    int index = hash(key) % table_size;
    table[index] = Bucket(key, binding, table[index]);
}

void *lookup(string key) {
    int index = hash(key) % table_size;
    struct bucket *b;
    for(b=table[index]; b; b=b->next)
    if (0==strcmp(b->key,key)) return b->binding;
    return NULL;
}

void pop(string key) {
    int index = hash(key) % table_size;
    table[index] = table[index]->next;
    num_elements--;
}