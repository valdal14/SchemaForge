#ifndef TYPES_H
#define TYPES_H

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
const char *type_to_string(SchemaType type);

/**
 * @brief Converts a string representation to a SchemaType enum
 * @param str char pointer
 * @return SchemaType
 */
SchemaType string_to_type(char *str);

/**
 * @brief Infers the type based on the given token
 * @param token const char pointer
 * @return SchemaType
 */
SchemaType infer_type(const char *token);

#endif
