#include "include/header-list.h"

HeaderList *header_list_parse_headers(BufferedReader *br){
    char *header_name, *header_line;
    char *header_value;
    HeaderList *head, *cur, *prev;
    
    head = cur = prev = 0x0;
    while((header_line = br_read_line(br)) && !br->flags && header_line[0] != '\r'){
        if(!(cur = malloc(sizeof(HeaderList)))){
            header_list_destroy(head);
            return 0x0;
        }
        memset(cur, '\0', sizeof(HeaderList));
        if(prev)
            prev->next_header = cur;
        if(!head)
            head = cur;
        if(!(header_name = (char *)malloc(sizeof(char)*MAX_HEADER))){
            header_list_destroy(head);
            return 0x0;
        }
        if(!(header_value = (char *)malloc(sizeof(char)*MAX_HEADER_VALUE))){
            header_list_destroy(head);
            free(header_name);
            return 0x0;
        }
        sscanf(header_line, "%[^:]%*[:] %s", header_name, header_value);
        cur->header_name = header_name;
        for ( ; *header_name; header_name++) *header_name = tolower(*header_name);
        cur->header_value = header_value;
        prev = cur;
        free(header_line);
    }
    free(header_line);
    return head;
}

void header_list_destroy(HeaderList *head){
    HeaderList *cur, *hold;
    if(!head)
        return;
    cur=head;
    while(cur->next_header){
        hold = cur;
        cur=cur->next_header;
        free(hold->header_name);
        free(hold->header_value);
        free(hold);
    }
}

char *header_list_get_header(HeaderList *head, char *header){
    HeaderList *cur;
    char *header_lower, *p;
    
    header_lower = malloc(strlen(header)+1);
    strcpy(header_lower, header);
    for (p=header_lower; *p; p++) *p = tolower(*p);
    for(cur=head; cur && strcmp(cur->header_name, header_lower); cur=cur->next_header);
    if(!cur)
        return 0x0;
    return cur->header_value;
}

void header_list_print(HeaderList *head){
    HeaderList *cur;
    for(cur=head;cur;cur=cur->next_header)
        fprintf(stdout, "%s - %s\n", cur->header_name, cur->header_value);
}

void header_add_header(HeaderList *head, char *header_name, char *header_value){
    char *name_copy, *value_copy;
    HeaderList *prev, *cur, *out;

    name_copy = malloc(strlen(header_name)+1);
    value_copy = malloc(strlen(header_value)+1);
    strcpy(name_copy, header_name);
    strcpy(value_copy, header_value);

    if(!head->header_name){
        head->header_name = name_copy;
        head->header_value = value_copy;
        return;
    }
    for(cur=head; cur; cur=cur->next_header)
        prev = cur;
    out = malloc(sizeof(HeaderList));
    prev->next_header = out;
    out->header_name = name_copy;
    out->header_value = value_copy;
}