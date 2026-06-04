#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define CHUNK_SIZE 4096

typedef enum 
{
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_BOOL,
    TYPE_VARCHAR
} SchemaType;

/**
 * @brief Formats the type based on the SchemaType
 * @param SchemaType type enum
 * @return const char pointer
 */
const char* type_to_string(SchemaType type) 
{
    switch(type) 
    {
        case TYPE_INT: return "INT";
        case TYPE_FLOAT: return "FLOAT";
        case TYPE_BOOL: return "BOOL";
        default: return "VARCHAR";
    }
}

/**
 * @brief Infers the type based on the given token
 * @param token const char pointer
 * @return SchemaType
 */
SchemaType infer_type(const char *token)
{
    int has_dot = 0;

    if(strcmp(token, "true") == 0 || strcmp(token, "false") == 0) return TYPE_BOOL;
    
    for(int i = 0; token[i] != '\0'; i++)
    {
        if(i == 0 && token[i] == '-') continue;
        if(token[i] == '.') has_dot++;
        
        if(has_dot > 1) return TYPE_VARCHAR; 
        
        if(!isdigit(token[i]) && token[i] != '.') return TYPE_VARCHAR;
    }

    return has_dot == 1 ? TYPE_FLOAT : TYPE_INT; 
}

int main(void)
{
    unsigned char buffer[CHUNK_SIZE];
    size_t bytes_read = 0;
    size_t total_bytes = 0;
    size_t leftover_count = 0;
    unsigned char *token_start = NULL;

    while(1) 
    {
        bytes_read = fread(buffer + leftover_count, 1, CHUNK_SIZE - leftover_count, stdin);
        total_bytes = leftover_count + bytes_read;
        if (total_bytes == 0) break;
    
        token_start = buffer;
   
        for(unsigned char *p = buffer; p < (buffer + total_bytes); p++)
        {
            unsigned char current_char = *p;
            
            if(current_char == ',' || current_char == '\n') 
            {
                *p = '\0';
                SchemaType type = infer_type((const char *)token_start);
                printf("[%s : %s] ", token_start, type_to_string(type));
                if(current_char == '\n') printf("\n");
                token_start = (p + 1);
            }
        }

        leftover_count = (buffer + total_bytes) - token_start;
        // If leftover_count is greater than 0, execute the shift
        if(leftover_count > 0) memmove(buffer, token_start, leftover_count);
    }

    if(feof(stdin)) 
    {
        return EXIT_SUCCESS; 
    }
    else if(ferror(stdin)) 
    {
        fprintf(stderr, "Hardware error or OS aborted the stream\n");     
        return EXIT_FAILURE;
    }

    return EXIT_FAILURE;
}
