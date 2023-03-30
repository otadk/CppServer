#ifndef _HASH_H
#define _HASH_H

struct hashNode {
    int value;
    struct hashNode* next;
};

struct hashNode** newHashSet(int size);
void add(struct hashNode** hashSet, int value, int size);
void HashSetRemove(struct hashNode** hashSet, int value, int size);
int getTotal(struct hashNode** hahsSet, int size);
int* keySet(struct hashNode** hashSet, int size);
void clear(struct hashNode** hashSet, int size);

#endif