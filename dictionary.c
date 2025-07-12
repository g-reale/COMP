#include "dictionary.h"
#include <stdlib.h>
#include <stdio.h>

dictionary createDict() {
    dictionary d = {
        .arena = (dict_t*) calloc(INITIAL_CAPACITY, sizeof(dict_t)),
        .height = 0,
        .capacity = INITIAL_CAPACITY,
    };
    d.arena[0] = (dict_t){0};
    d.height = 1;
    return d;
}

entry_t querryDict(dictionary * d, char* name, int* found, size_t root, size_t * leaf){
    
    if(found)
        *found = 0;
    
    if (!name || d->height < root) 
        return (entry_t){0};
    
    unsigned char letter;

    for (size_t i = 0; (letter = (unsigned char)name[i]) != '\0'; i++) {
        uint64_t mask = letter >> 6;
        uint64_t bit  = letter & 63;
        uint64_t transition = (d->arena + root)->transitions[mask];

        if (!(transition & (1ULL << bit)))
            return (entry_t){0};

        root = (d->arena + root)->decedents[letter];
    }

    if(found)
        *found = 1;
    
    if(leaf)
        *leaf = root;
    return (d->arena + root)->entry;
}

int insertDict(dictionary* d, char* name, entry_t e, size_t root, size_t * leaf) {

    if (!name || d->height < root) 
        return 0;

    size_t i;
    unsigned char letter;

    for (i = 0; (letter = (unsigned char)name[i]) != '\0'; i++){
        uint64_t mask = letter >> 6;
        uint64_t bit  = letter & 63;
        uint64_t transition = (d->arena + root)->transitions[mask];
        
        if (!(transition & (1ULL << bit)))
            break;

        root = (d->arena + root)->decedents[letter];
    }

    if (letter == '\0' && (d->arena + root)->filled)
        return 0;

    if (letter == '\0'){
        if(leaf)
            *leaf = root;
        (d->arena + root)->entry = e;
        (d->arena + root)->filled = 1;
        return 1;
    }

    for (; (letter = (unsigned char)name[i]) != '\0'; i++){
        
        if (d->height == d->capacity) {
            d->capacity *= 2;
            d->arena = realloc(d->arena, sizeof(dict_t) * d->capacity);
            memset(d->arena + d->height, 0, sizeof(dict_t) * d->capacity/2);
        }
        
        uint64_t mask = letter >> 6;
        uint64_t bit  = letter & 63;
        
        (d->arena + root)->transitions[mask] |= (1ULL << bit);
        (d->arena + root)->decedents[letter] = d->height;
        root = d->height;
        d->height++;
    }

    if(leaf)
        *leaf = root;
    (d->arena + root)->entry = e;
    (d->arena + root)->filled = 1;
    return 1;
}


void traverseHelp(dictionary * d, char * word, size_t root, size_t index, size_t prefix){
    dict_t node = d->arena[root];

    if(node.filled){
        word[index] = '\0';
        switch (node.entry.type){
            case SUBTREE:
                printf("%s\tSUBTREE\n",word);
                prefix = index;
                break;
            
            case ADDRESS:
                printf("\t%s\t%ld\t%ld\tADDRESS\n",word + prefix, node.entry.entry.address,node.entry.entry.size);
                break;
        }
    }
    
    for (size_t i = 0; i < 127; i++) {
        uint64_t mask = i >> 6;
        uint64_t bit  = i & 63;
        uint64_t transition = node.transitions[mask];

        if(transition & (1ULL << bit)){ //avoid recursion on the roor node
            word[index] = (char)i;
            traverseHelp(d,word,node.decedents[i],index + 1, prefix);
        }
    }
    
    if(node.filled){
        switch (node.entry.type){
            case SUBTREE:
                printf("SUBTREE END\n");
                break;
            
            default: break;
        }
    }
}

void traverseDict(dictionary * d, size_t root){
    static char word[1024];
    
    if(d->height < root)
        return;
    
    word[0] = '\0';
    traverseHelp(d,word,root,0,0);
}

void destroyDict(dictionary* d) {
    free(d->arena);
    d->arena = NULL;
    d->height = 0;
    d->capacity = 0;
}