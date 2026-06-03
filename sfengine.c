#include <stdio.h>
#include <stdlib.h>

#define CHUNK_SIZE 4096

int main(void)
{
    unsigned char buffer[CHUNK_SIZE];
    size_t bytes_read = 0;
    void *ptr = (void *)buffer;
    
    while(1) 
    {
        bytes_read = fread(ptr, 1, CHUNK_SIZE, stdin); 
        if (bytes_read == 0) break;
        fwrite(ptr, 1, bytes_read, stdout);
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