#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compare.h"

#define bufSize 20001

typedef struct element {

    int priority;

    char *value;

    struct element *neighbour;
} elem_t;

elem_t *createNode(char * buf)
{
    int index = 7;
    int n = 0; 
    int m = 0; 
    char number[15];
    elem_t * node = malloc(sizeof(elem_t));
    if (node == NULL)
        return NULL;


    while(index < bufSize) {
        if(buf[index] == ' ')
            break;
        else 
            n++;
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

    node->value = calloc((n+1), sizeof(char));
    if(node->value == NULL){
        return NULL;
    }
    node->neighbour = NULL;
    strncpy ( node->value, (buf + 7), n);
    int num;
    char* buffer = malloc(50);
    if(buffer==NULL){
        return NULL;
    }
    if(sscanf(number,"%d",&num) != 0 && 
        sprintf(buffer, "%d", num) != 0 && 
        strcmp(number, buffer) == 0)
    {
        node->priority = num;
        free(buffer);
    } else {
        free(buffer);
        return NULL;
    }


    return node;
}

int insert(char * buf, elem_t ** list){



    elem_t * current = (*list);
    elem_t * new_node = createNode(buf);
    if (new_node == NULL){
        return 12;
    }

    if(new_node == NULL){
        return 0;
    }


    if(*list == NULL){
        *list = new_node;
        return 0;
    }


    if(compare(new_node->priority, (*list)->priority) >= 0){

        new_node->neighbour = (*list);
        (*list) = new_node;
    } else {

        //elem_t * previous = current;
        //current = current->neighbour;

        current = (*list);

        while (current->neighbour != NULL) {

            if (compare(new_node->priority, current->neighbour->priority) < 0) {
                current = current->neighbour;
            } else {
                break;
            }
            

        }


        new_node->neighbour = current->neighbour;
        current->neighbour = new_node;

       
    }
    return 1;
}

void top(elem_t ** list){
     if (*list == NULL) {
        printf("\n");
        return ;
    }

    printf("%s\n", (*list)->value);
}

void pop(elem_t ** list){


    if (*list == NULL) {
        return ;
    }

    elem_t * next_node = *list;

    *list = (*list)->neighbour;
    free(next_node->value);
    free(next_node);

}

int getInstruction(char * buf){
    char * INSERT = "insert\0";
    char * POP = "pop\0";
    char * TOP = "top\0";



    if((buf[0]=='t' && buf[1]=='o' && buf[2]=='p' && buf[3]!='\0')||
        (buf[0]=='p' && buf[1]=='o' && buf[2]=='p' && buf[3]!='\0')){
            return 0;
        }

    char *tempBuf = malloc((strlen(buf) + 1) * sizeof(char));
    if(tempBuf == NULL){
        return 12;
    }
    strcpy(tempBuf, buf);

    char *var = strtok (tempBuf," ");


    // printf("%s\n", var);
    if(var != NULL) {

        if(strcmp(var, INSERT) == 0) 
            { 
                var = strtok(NULL, " ");

                var = strtok(NULL, " ");
                if(var == NULL){
                    free(tempBuf);
                    return 0;
                }
                else{
                    var = strtok(NULL, " ");
                    if(var!=NULL){
                        free(tempBuf);
                        return 0;
                    }
                }
                free(tempBuf); 
                 return 1;
            }
        if(strcmp(var, POP) == 0) {free(tempBuf);  return 2;}
        if(strcmp(var, TOP) == 0) {free(tempBuf); return 3;}
        }
    free(tempBuf);

    return 0;
}

int doStuff(char * buf, elem_t ** list){
    //get Instruction
    int ret;
    int index = getInstruction(buf);
    
    switch(index) {
      case 1 :

        ret = insert(buf, list);
        if(ret == 12){
            return 12;
        }
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

   return 1;
    
}

int main(int argc, char *argv[]){
    char * buf = malloc(bufSize * sizeof(char));
    if(buf == NULL){
        return 12;
    }
    FILE *file;
    int i = 1;
    elem_t * head = NULL;



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
    free(buf);
    elem_t * temp;
    while(head != NULL){
        temp = head;
        head = head->neighbour;
        free(temp->value);
        free(temp);
    }


    return 0;
}
