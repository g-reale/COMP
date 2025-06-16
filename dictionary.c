#include "dictionary.h"

dictionary createDict(size_t size, size_t (*mapping)(char * string, size_t)){
    dictionary dict = {
        .page = (entry**)malloc(sizeof(entry*) * size),
        .mapping = mapping,
        .size = size
    };
    for(size_t i = 0; i < size; i++)
        dict.page[i] = NULL;
    return dict;
}

void deleteEntry(entry * e){
    if(e == NULL)
        return;

    deleteEntry(e->next);
    e->destructor(e->value);
    free(e->name);
    free(e);
}

void destroyDict(dictionary dict){
    for(size_t i = 0; i < dict.size; i++)
        deleteEntry(dict.page[i]);
    free(dict.page);
}

entry ** inDict(dictionary dict, char * name, int * found){
    size_t index = dict.mapping(name,dict.size);
    entry ** head = &dict.page[index];
    *found = 0;

    if(*head == NULL)
        return head;
    
    while(1){
        if(strcmp((*head)->name,name) == 0){
            *found = 1;
            return head;
        }

        if((*head)->next == NULL)
            break;

        head = &(*head)->next;
    }

    return head;
}

int insertDict(dictionary dict, char * name, void * value, void (*destructor)(void *)){
    int found;
    entry ** tail = inDict(dict,name,&found);

    if(found)
        return 0;
    
    entry * new = (entry*)malloc(sizeof(entry));
    new->name = strdup(name);
    new->value = value;
    new->destructor = destructor;
    new->next = NULL;
    (*tail) = new;
    return 1;
}

void * getDict(dictionary dict, char * name, int * found){
    entry ** e = inDict(dict,name,found);
    if(*found)
        return (*e)->value;
    return NULL;
}