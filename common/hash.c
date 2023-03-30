#include "head.h"

int hashFunc(int value, int size) {
    return value % size;
}

struct hashNode** newHashSet(int size) {
    return (struct hashNode**)calloc(size, sizeof(struct hashNode*));
}

void add(struct hashNode** hashSet, int value, int size) {
    int index = hashFunc(value, size);
    struct hashNode* newNode = (struct hashNode*)malloc(sizeof(struct hashNode));
    newNode->value = value;
    newNode->next = NULL;
    
    if (hashSet[index] == NULL) {
        hashSet[index] = newNode;
    } else {
        struct hashNode* lastNode = hashSet[index];
        while (lastNode->next != NULL) {
            lastNode = lastNode->next;
        }
        lastNode = newNode;
    }
}

void HashSetRemove(struct hashNode** hashSet, int value, int size) {
    int index = hashFunc(value, size);
    struct hashNode* curNode = hashSet[index];
    struct hashNode* lastNode = NULL;
    
    while (curNode != NULL) {
        if (curNode->value == value) {
            if (lastNode == NULL) {
                hashSet[index] = NULL;
            } else {
                lastNode->next = curNode->next;
            }
            free(curNode);
            return ;
        }
        lastNode = curNode;
        curNode = curNode->next;
    }
}

int getTotal(struct hashNode** hashSet, int size) {
    int total = 0;
    for (int i = 0; i < size; ++i) {
        struct hashNode* lastNode = hashSet[i];
        while (lastNode != NULL) {
            total += 1;
            lastNode = lastNode->next;
        }
    }
    return total;
}

int* keySet(struct hashNode** hashSet, int size) {
    int total = getTotal(hashSet, size);
    int* result = (int*)malloc(sizeof(int) * total);
    int index = 0;
    for (int i = 0; i < size; ++i) {
        struct hashNode* lastNode = hashSet[i];
        while (lastNode != NULL) {
            result[index] = lastNode->value;
            index += 1;
            lastNode = lastNode->next;
        }
    }
    return result;
}

void clear(struct hashNode** hashSet, int size) {
    for (int i = 0; i < size; ++i) {
        struct hashNode* lastNode = hashSet[i];
        while (lastNode != NULL) {
            struct hashNode* curNode = lastNode->next;
            free(lastNode);
            lastNode = curNode;
        }
    }
    free(hashSet);
}