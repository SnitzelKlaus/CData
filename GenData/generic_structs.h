// Description: This file contains the generic structs used in the database.

// Define a field descriptor
#ifndef GENERIC_STRUCTS_H

#define GENERIC_STRUCTS_H

typedef struct FieldDescriptor{
    const char *field_name;
    const char *data_type;
} FieldDescriptor;

// Create a table descriptor
typedef struct TableDescriptor {
    const char *table_name;
    FieldDescriptor *fields;
    int num_fields;
} TableDescriptor;

typedef struct GenericType {
    void *value[];
} GenericType;


#endif

// Example usage:

// FieldDescriptor item_fields[] = {
//     {"id", "SERIAL PRIMARY KEY"},
//     {"name", "VARCHAR(255)"},
// };
//
// TableDescriptor item_table = {
//     "items",
//     item_fields,
//     2,
// };
