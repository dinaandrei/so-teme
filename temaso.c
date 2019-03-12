#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define bufSize 20001

typedef struct element {
    int priority;
    char * value;
    struct element * neighbour;
} elem_t;

elem_t * createNode(char * buf){
    int index = 7;
    int n = 0; 
    int m = 0; 
    char number[15];
    elem_t * node = malloc(sizeof(elem_t));

    while(index < bufSize){
        if(buf[index] == ' ') {
            break;
        } else n++;
        index++;
    }
    index++;
    while(index < bufSize){
        if(buf[index] == '\0'){
            number[m++] = buf[index];
            break;
        }else{
            number[m++] = buf[index];
        }
        index++;
    }

    node->value = malloc(sizeof(char) * n);
    strncpy ( node->value, (buf + 7), n);
    int num;
    char* buffer = malloc(50);
    if(sscanf(number,"%d",&num) != 0 && 
        sprintf(buffer, "%d", num) != 0 && 
        strcmp(number, buffer) == 0)
    {
        node->priority = num;
        free(buffer);
    } else {
        return NULL;
    }

    node->neighbour = NULL;
    return node;
}

void insert(char * buf, elem_t ** list){
    elem_t * current = (*list);
    elem_t * new_node = createNode(buf);

    if(new_node == NULL){
        return;
    }

    if(*list == NULL){
        *list = new_node;
        return;
    }

    if((*list)->priority < new_node->priority){
        new_node->neighbour = (*list);
        *list = new_node;
    } else {
        elem_t * previous = current;
        current = current->neighbour;
        while (current != NULL) {
            if(current->priority < new_node->priority){
                new_node->neighbour = current->neighbour;
                previous->neighbour = new_node;
                break;
            }
            previous = current;
            current = current->neighbour;
        }
    }
}

void top(elem_t ** list){
     if (*list == NULL) {
        printf("\n");
        return ;
    }

    printf("%s\n", (*list)->value);
}

void pop(elem_t ** list){
    elem_t * next_node = NULL;

    if (*list == NULL) {
        return ;
    }

    next_node = (*list)->neighbour;
    free(*list);
    *list = next_node;
}

int getInstruction(char * buf){
    char * INSERT = "insert\0";
    char * POP = "pop\0";
    char * TOP = "top\0";
    
    char * var = malloc(10 * sizeof(char));
    var = strtok (buf," ");
    // printf("%s\n", var);
    if(strcmp(var, INSERT) == 0) return 1;
    if(strcmp(var, POP) == 0) return 2;
    if(strcmp(var, TOP) == 0) return 3;
    return 0;
}

void doStuff(char * buf, elem_t ** list){
    //get Instruction
    int index = getInstruction(buf);
    switch(index) {
      case 1 :
        insert(buf, list);
        break;
      case 2 :
        pop(list);
        break;
      case 3 :
        top(list);
        break;
      default :
        break;
   }
    
}

int main(int argc, char *argv[]){
    char * buf = malloc(bufSize * sizeof(char));
    FILE *file;
    int i = 1;
    elem_t * head;

    if(argc > 1){
        for (; i < argc; i++){
            file = fopen(argv[i],"r");
            if(file != NULL){
                while (fgets(buf, bufSize, file) != NULL){
                    buf[strlen(buf) - 1] = '\0';
                    doStuff(buf, &head);
                }
                fclose(file);
            }
        }
    } else {
        while(fgets(buf, bufSize, stdin) != NULL){
            buf[strlen(buf) - 1] = '\0';
            doStuff(buf, &head);
        }
    }

    return 0;
}