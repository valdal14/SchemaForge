#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHUNK_SIZE 4096

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
                printf("[%s] ", token_start);
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
