#ifndef _HASHMAP_H
#define _HASHMAP_H

typedef struct hashMapNode {
    int fd;
    char* name;
    struct hashMapNode* next;
} HashMapNode;

HashMapNode** newHashMap(int length);
void hashMapAdd(HashMapNode** hashMap, int length, int fd, char* name);
void hashMapRemove(HashMapNode** hashMap, int length, int fd);
char* hashMapGet(HashMapNode** hashMap, int length, int fd);
int hashMapSize(HashMapNode** hashMap, int length);
int* hashMapKeySet(HashMapNode** hashMap, int length);
void hashMapClear(HashMapNode** hashMap, int length);

#endif