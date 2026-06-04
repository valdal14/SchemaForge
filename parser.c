#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "types.h"
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
 * @brief Logs a column mismatch 
 * @param int current_row
 * @return void
 */
void log_columns_mismatch(int current_row)
{
    fflush(stdout);
    fprintf(stderr, "\nColumns Schema Mismatch at row %d: Too many columns\n", current_row);
}

/**
 * @brief Logs a column schema mismatch
 * @param int current_row
 * @param int expected_columns
 * @param int current_col
 * @return void
 */
void log_columns_schema_mismatch(int current_row, int expected_columns, int current_col)
{
    fflush(stdout);
    fprintf(stderr, "\nColumns Schema Mismatch at row %d: Expected %d columns, but found %d (Check     Delimiter)\n", current_row, expected_columns, current_col);
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
        exit(EXIT_FAILURE); 
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
