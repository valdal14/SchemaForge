#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "parser.h"

int main(int argc, char *argv[])
{
    SchemaType expected_schema[100];
    int expected_columns = 0;
    int current_col = 0;
    int is_header = 1;
    int current_row = 1;

    unsigned char buffer[CHUNK_SIZE];
    size_t bytes_read = 0;
    size_t total_bytes = 0;
    size_t leftover_count = 0;
    unsigned char *token_start = NULL;

    // Set the Schema delimiter safely
    char delimiter = '\0';
    parse_input(argc, argv, &delimiter, expected_schema, &expected_columns);
    
    if(delimiter == '\0' || expected_columns == 0) return EXIT_FAILURE;

	while(1) 
	{
        bytes_read = fread(buffer + leftover_count, 1, CHUNK_SIZE - leftover_count, stdin);
	    total_bytes = leftover_count + bytes_read;
	    if (total_bytes == 0) break;
    
	    token_start = buffer;
   
	    for(unsigned char *p = buffer; p < (buffer + total_bytes); p++)
	    {
            unsigned char current_char = *p;
          
	        if(current_char == delimiter || current_char == '\n')
	        {
	            if(is_header == 0)
	            {
	                if(current_col >= expected_columns)
	                {
                        log_columns_mismatch(current_row);
	                    return EXIT_FAILURE;
	                }

                    // Isolate for inference
	                *p = '\0'; 
	                SchemaType type = infer_type((const char *)token_start);
                    // Instantly restore the CSV delimiter!
	                *p = current_char; 
                    
	                if (expected_schema[current_col] != TYPE_VARCHAR && type != expected_schema[current_col])
	                {
	                    log_mismatch(current_row, current_col, expected_schema[current_col], type);
	                    return EXIT_FAILURE;
	                }
	                else
	                {
	                    current_col++;
	                }
                    
	                #ifdef DEBUG
	                    fprintf(stderr, "[%s : %s] ", token_start, type_to_string(type));
	                #endif

	                if(current_char == '\n') 
	                {
	                    if(current_col != expected_columns) 
	                    {
                            log_columns_schema_mismatch(current_row, expected_columns, current_col);
	                        return EXIT_FAILURE;
	                    }

	                    current_col = 0;
	                    current_row++;
                        
	                    #ifdef DEBUG
	                        fprintf(stderr, "\n");
	                    #endif
	                }

	                token_start = (p + 1);
	            }

	            if(is_header == 1)
	            {
	                if(current_char == '\n')
	                {
	                    is_header = 0;
	                }
	                token_start = (p + 1);
	            }
	        }
	    }

	    // Dump the safely validated CSV chunk to your destination pipeline!
	    size_t validated_bytes = token_start - buffer;
	    if (validated_bytes > 0)
	    {
	        fwrite(buffer, 1, validated_bytes, stdout);
	    }

	    leftover_count = (buffer + total_bytes) - token_start;
	    if(leftover_count > 0) memmove(buffer, token_start, leftover_count);
	}

    if(feof(stdin)) 
    {
        // JSON Success log sent to stderr for Log engines Like DataGod, avoiding CSV corruption
        fprintf(stderr, "{\"status\": \"success\", \"rows_validated\": %d}\n", current_row - 1);
        return EXIT_SUCCESS; 
    }
    else if(ferror(stdin)) 
    {
        fprintf(stderr, "\nHardware error or OS aborted the stream\n");     
        return EXIT_FAILURE;
    }

    return EXIT_FAILURE;
}
