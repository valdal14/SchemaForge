#include <ctype.h>
#include <string.h>
#include "types.h"

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
