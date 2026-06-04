#ifndef PARSER_H
#define PARSER_H

#include "types.h"

#define CHUNK_SIZE 4096

/**
 * @brief Logs a Schema mismatch
 * @param current_row int
 * @param current_col int
 * @param exp SchemaType
 * @param current SchemaType
 * @return void
 */
void log_mismatch(int current_row, int current_col, SchemaType exp, SchemaType current);

/**
 * @brief Logs a column mismatch 
 * @param int current_row
 * @return void
 */
void log_columns_mismatch(int current_row);

/**
 * @brief Logs a column schema mismatch
 * @param int current_row
 * @param int expected_columns
 * @param int current_col
 * @return void
 */
void log_columns_schema_mismatch(int current_row, int expected_columns, int current_col);

/**
 * @brief Logs the invalid input message
 * @return void
 */
void log_invalid_input(void);

/**
 * @brief Validates the inputs given by the data pipeline
 * @param argc int
 * @param argv char pointer array
 * @param delimiter char pointer
 * @param expected_schema SchemaType array
 * @param expected_columns int pointer
 * @return void
 */
void parse_input(int argc, char *argv[], char *delimiter, SchemaType expected_schema[100], int *expected_columns);


#endif
