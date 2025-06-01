#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Linked List node
struct HashMap_node {

    // key is string
    char* key;

    // value is pointer to data
    void* value;
    struct HashMap_node* next;
};

// like constructor
void HashMap_setNode(struct HashMap_node* node, char* key, void* value)
{
    node->key = key;
    node->value = value;
    node->next = NULL;
    return;
};

typedef struct {

    // Current number of elements in hashMap
    // and capacity of hashMap
    int numOfElements, capacity;

    // hold base address array of linked list
    struct HashMap_node** arr;
} HashMap;

// like constructor
void HashMap_init(HashMap* mp, int capacity)
{

    // Default capacity in this case
    mp->capacity = capacity;
    mp->numOfElements = 0;

    // array of size = 1
    mp->arr = (struct HashMap_node**)malloc(sizeof(struct HashMap_node*)
                                    * mp->capacity);
    return;
}

HashMap* HashMap_new(int capacity)
{
	HashMap* mp = (HashMap*)malloc(sizeof(HashMap));
	HashMap_init(mp, capacity);
	return mp;
}

int hashFunction(HashMap* mp, char* key)
{
    int bucketIndex;
    int sum = 0, factor = 31;
    for (int i = 0; i < strlen(key); i++) {

        // sum = sum + (ascii value of
        // char * (primeNumber ^ x))...
        // where x = 1, 2, 3....n
        sum = ((sum % mp->capacity)
               + (((int)key[i]) * factor) % mp->capacity)
              % mp->capacity;

        // factor = factor * prime
        // number....(prime
        // number) ^ x
        factor = ((factor % __INT16_MAX__)
                  * (31 % __INT16_MAX__))
                 % __INT16_MAX__;
    }

    bucketIndex = sum;
    return bucketIndex;
}

void HashMap_insert(HashMap* mp, char* key, void* value)
{

    // Getting bucket index for the given
    // key - value pair
    int bucketIndex = hashFunction(mp, key);
    struct HashMap_node* newNode = (struct HashMap_node*)malloc(

        // Creating a new node
        sizeof(struct HashMap_node));

    // Setting value of node
    HashMap_setNode(newNode, key, value);

    // Bucket index is empty....no collision
    if (mp->arr[bucketIndex] == NULL) {
        mp->arr[bucketIndex] = newNode;
    }

    // Collision
    else {

        // Adding newNode at the head of
        // linked list which is present
        // at bucket index....HashMap_insertion at
        // head in linked list
        newNode->next = mp->arr[bucketIndex];
        mp->arr[bucketIndex] = newNode;
    }
    return;
}

void HashMap_delete (HashMap* mp, char* key)
{

    // Getting bucket index for the
    // given key
    int bucketIndex = hashFunction(mp, key);

    struct HashMap_node* prevNode = NULL;

    // Points to the head of
    // linked list present at
    // bucket index
    struct HashMap_node* currNode = mp->arr[bucketIndex];

    while (currNode != NULL) {

        // Key is matched at HashMap_delete this
        // node from linked list
        if (strcmp(key, currNode->key) == 0) {

            // Head node
            // deletion
            if (currNode == mp->arr[bucketIndex]) {
                mp->arr[bucketIndex] = currNode->next;
            }

            // Last node or middle node
            else {
                prevNode->next = currNode->next;
            }
            free(currNode);
            break;
        }
        prevNode = currNode;
        currNode = currNode->next;
    }
    return;
}

void* HashMap_search(HashMap* mp, char* key)
{

    // Getting the bucket index
    // for the given key
    int bucketIndex = hashFunction(mp, key);

    // Head of the linked list
    // present at bucket index
    struct HashMap_node* bucketHead = mp->arr[bucketIndex];
    while (bucketHead != NULL) {

        // Key is found in the hashMap
        if (strcmp(bucketHead->key, key) == 0) {
            return bucketHead->value;
        }
        bucketHead = bucketHead->next;
    }

    // If no key found in the hashMap
    // equal to the given key
    return NULL;
}
