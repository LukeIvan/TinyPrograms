#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/*
    Didn't want to have a locked amount of droplets. Basic HT
*/

#define MAX_INST 30 // Arbitrary

typedef struct 
{
    int x, y;
    float radius, intensity;
    uint8_t color;
} Ripple;

typedef struct
{
    int key;
    Ripple value;
} tableEntry;

uint32_t hash(int key)
{
    return key % MAX_INST;
}

void store(int key, Ripple value, tableEntry *table[])
{
    uint32_t i = hash(key);
    while (table[i] != NULL && table[i]->key != key) 
    {
        i = (i + 1) % MAX_INST;
    }
    if (table[i] == NULL) 
    {
        tableEntry *newEntry = (tableEntry*)malloc(sizeof(tableEntry));
        newEntry->value = value;
        newEntry->key = key;
        table[i] = newEntry;
    } 
    else 
    {
        table[i]->value = value;
    }
}

Ripple *get(int key, tableEntry *table[])
{
    uint32_t i = hash(key);
    while (table[i] != NULL) 
    {
        if (table[i]->key == key) 
        {
            return &table[i]->value;
        }
        i = (i + 1) % MAX_INST; // Linear probing
    }
    return NULL;
}

void remove_entry(int key, tableEntry *table[]) 
{
    uint32_t i = hash(key);
    while (table[i] != NULL) 
    {
        if (table[i]->key == key) 
        {
            free(table[i]);
            table[i] = NULL;
            uint32_t j = (i + 1) % MAX_INST;

            while (table[j] != NULL) 
            {
                tableEntry *rehashedEntry = table[j];
                table[j] = NULL;
                store(rehashedEntry->key, rehashedEntry->value, table);
                free(rehashedEntry);
                j = (j + 1) % MAX_INST;
            }
            return;
        }
        i = (i + 1) % MAX_INST;
    }
}

int test()
{
    tableEntry *nt[MAX_INST] = {NULL};
    Ripple newripple = {1, 2, 1.0, 1, 0};
    int key = 5;
    store(5, newripple, nt);
    Ripple diffripple = *get(5, nt);
    printf("X: %d", diffripple.x);
    for (int i = 0; i < MAX_INST; ++i) {
        if (nt[i] != NULL) {
            free(nt[i]);
        }
    }
    return 0;
}

// int main()
// {
//     return test();
// }