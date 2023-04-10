#include "head.h"

int getHash(int index, int length) {
    return index % length;
}

HashMapNode** newHashMap(int length) {
    return (HashMapNode**)calloc(length, sizeof(HashMapNode*));
}

void hashMapAdd(HashMapNode** hashMap, int length, int fd, char* name) {
    int index = getHash(fd, length);
    HashMapNode* addNode = (HashMapNode*)malloc(sizeof(HashMapNode));
    addNode->fd = fd;
    addNode->name = name;
    addNode->next = NULL;
    HashMapNode* node = hashMap[index];
    if (node == NULL) {
        hashMap[index] = addNode;
    } else {
        while (node->next != NULL) {
            node = node->next;
        }
        node->next = addNode;
    }
}

void hashMapRemove(struct hashMapNode** hashMap, int length, int fd) {
    int index = getHash(fd, length);
    HashMapNode* node = hashMap[index];
    HashMapNode* lastNode = NULL;
    while (node != NULL) {
        if (node->fd == fd) {
            if (lastNode == NULL) {
                hashMap[index] = NULL;
            } else {
                lastNode->next = node->next;
            }
            free(node);
            return ;
        }
        lastNode = node;
        node = node->next;
    }    
}

char* hashMapGet(struct hashMapNode** hashMap, int length, int fd) {
    int index = getHash(fd, length);
    HashMapNode* node = hashMap[index];
    while (node != NULL) {
        if (node->fd == fd) {
            return node->name;
        }
        node = node->next;
    }
    return NULL;
}

int hashMapSize(struct hashMapNode** hashMap, int length) {
    int size = 0;
    for (int i = 0; i < length; ++i) {
        HashMapNode* node = hashMap[i];
        while (node != NULL) {
            size += 1;
            node = node->next;
        }
    }
    return size;
}
int* hashMapKeySet(struct hashMapNode** hashMap, int length) {
    int size = hashMapSize(hashMap, length);
    int* keySet = (int*)calloc(size, sizeof(int));
    int index = 0;
    for (int i = 0; i < length; ++i) {
        HashMapNode* node = hashMap[i];
        while (node != NULL) {
            keySet[index] = node->fd;
            index += 1;
            node = node->next;
        }
    }
    return keySet;
}

void hashMapClear(struct hashMapNode** hashMap, int length) {
    for (int i = 0; i < length; ++i) {
        HashMapNode* node = hashMap[i];
        HashMapNode* freeNode;
        while (node != NULL) {
            freeNode = node;
            node = node->next;
            free(freeNode);
        }
    }
    free(hashMap);
}