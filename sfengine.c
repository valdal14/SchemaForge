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
 * @param type SchemaType enum
 * @return const char pointer
 */
const char *type_to_string(SchemaType type) 
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
 * @brief Converts a string representation to a SchemaType enum
 * @param str char pointer
 * @return SchemaType
 */
SchemaType string_to_type(char *str)
{
    if(strcmp(str, "INT") == 0) return TYPE_INT;
    if(strcmp(str, "FLOAT") == 0) return TYPE_FLOAT;
    if(strcmp(str, "BOOL") == 0) return TYPE_BOOL;

    return TYPE_VARCHAR;
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

/**
 * @brief Logs a Schema mismatch
 * @param current_row int
 * @param current_col int
 * @param exp SchemaType
 * @param current SchemaType
 * @return void
 */
void log_mismatch(int current_row, int current_col, SchemaType exp, SchemaType current)
{
    const char *expected_type = type_to_string(exp);
    const char *current_type = type_to_string(current);
    fflush(stdout);
    fprintf(stderr, "\nSchema Mismatch at row %d, column %d: Expected %s but got %s\n", current_row, current_col, expected_type, current_type);
}

/**
 * @brief Validates the inputs given by the data pipeline
 * @param argc int
 * @param argv char pointer array
 * @param delimiter char pointer
 * @param expected_schema SchemaType array
 * @param expected_columns int pointer
 * @return void
 */
void parse_input(int argc, char *argv[], char *delimiter, SchemaType expected_schema[100], int *expected_columns)
{
    const char supported_delimiters[] = {',', ';', '\t', '|', '~', '^'};
    int num_delimiters = sizeof(supported_delimiters) / sizeof(supported_delimiters[0]);
    int is_valid_delimiter = 0;
    
    if(argc < 3)
    {
        fflush(stdout);
        fprintf(stderr, "Usage: ./sfengine <schema> <delimiter>\n");
        exit(EXIT_FAILURE); // Prevent Segfault if args are missing
    }

    for(int i = 0; i < num_delimiters; i++)
    {
        if(argv[2][0] == supported_delimiters[i]) is_valid_delimiter = 1;
    }
        
    if(is_valid_delimiter == 1)
    {
        *delimiter = argv[2][0];
    }
    else
    {
        fflush(stdout);
        fprintf(stderr, "Invalid delimiter\n");
        exit(EXIT_FAILURE);
    }

    char *target_string = argv[1];
    char *token = strtok(target_string, delimiter);
    
    int token_index = 0;

    while(token != NULL) 
    {
        expected_schema[token_index] = string_to_type(token);
        token_index++; 
        token = strtok(NULL, delimiter);
    }

    *expected_columns = token_index;
}

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
                        fflush(stdout);
	                    fprintf(stderr, "\nColumns Schema Mismatch at row %d: Too many columns\n", current_row);
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
	                        fflush(stdout);
	                        fprintf(stderr, "\nColumns Schema Mismatch at row %d: Expected %d columns, but found %d (Check Delimiter)\n", current_row, expected_columns, current_col);
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
