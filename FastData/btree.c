#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "btree.h"

// Creates a new B-tree node. If leaf is true, it is a leaf node. The node is written to the file pointed to by fp.
BTreeNode* createNode(bool leaf, FILE* fp) {
    BTreeNode* node = (BTreeNode*)malloc(sizeof(BTreeNode));
    node->n_keys = 0;
    node->leaf = leaf;
    node->self_offset = ftell(fp);  // Set the offset at the current file pointer position
    for (int i = 0; i < MAX_KEYS + 1; i++) {
        node->children_offsets[i] = -1;  // Initialize children offsets to -1
    }
    saveNode(node, fp);  // Immediately save the new node
    printf("Created new %s node at offset %ld\n", leaf ? "leaf" : "internal", node->self_offset);
    return node;
}

// Saves the node to the file at the node's offset.
void saveNode(BTreeNode* node, FILE* fp) {
    fseek(fp, node->self_offset, SEEK_SET); // Move the file pointer to the node's offset
    fwrite(node, sizeof(BTreeNode), 1, fp); // Write the node to the file
    printf("Saved node with %d keys at offset %ld\n", node->n_keys, node->self_offset);
}

// Loads a node from the file at the given offset. Returns NULL if the offset is -1.
BTreeNode* loadNode(FILE* fp, long offset) {
    if (offset == -1) return NULL; // If the offset is -1, return NULL (no node)
    BTreeNode* node = (BTreeNode*)malloc(sizeof(BTreeNode));
    fseek(fp, offset, SEEK_SET); // Move the file pointer to the offset
    fread(node, sizeof(BTreeNode), 1, fp); // Read the node from the file
    printf("Loaded node with %d keys from offset %ld\n", node->n_keys, offset);
    return node;
}

// Inserts a data record into the B-tree. If the root is NULL, a new root node is created.
void insert(BTreeNode** root, WeatherData data, FILE* fp) {
    if (*root == NULL) {
        *root = createNode(1, fp);  // Create a new root node if root is NULL
    }
    
    BTreeNode* r = *root;
    if (r->n_keys == MAX_KEYS) {  // If the root is full, grow the tree height
        BTreeNode* s = createNode(0, fp);  // Create a new node that is not a leaf
        *root = s;  // The new node becomes the root
        s->children_offsets[0] = r->self_offset;  // The old root becomes the first child of the new root
        splitChild(s, 0, fp);  // Split the old root and move half of the entries to a new node
        insertNonFull(s, data, fp);  // Insert the new data into the B-tree
        printf("Root was full. Created new root and split the old root.\n");
    } else {
        insertNonFull(r, data, fp);  // Insert the data into a non-full node
    }
}

// Inserts data into a non-full node.
void insertNonFull(BTreeNode* node, WeatherData data, FILE* fp) {
    int i = node->n_keys - 1;
    if (node->leaf) { // If the node is a leaf
        while (i >= 0 && strcmp(data.datetime, node->keys[i].datetime) > 0) {
            node->keys[i + 1] = node->keys[i]; // Move keys to make space for new key
            i--;
        }
        node->keys[i + 1] = data; // Insert the new key
        node->n_keys++; // Increment the number of keys
        saveNode(node, fp); // Save the node to the file
        printf("Inserted key into leaf node at offset %ld\n", node->self_offset);
    } else { // If the node is not a leaf
        while (i >= 0 && strcmp(data.datetime, node->keys[i].datetime) > 0) {
            i--;
        }
        i++;
        BTreeNode* child = loadNode(fp, node->children_offsets[i]);
        if (child->n_keys == MAX_KEYS) { // If the child is full
            splitChild(node, i, fp); // Split the child
            if (strcmp(data.datetime, node->keys[i].datetime) > 0) {
                i++;
            }
        }
        insertNonFull(child, data, fp); // Insert the data into the appropriate child
    }
}

// Splits the child node at index i of the parent node.
void splitChild(BTreeNode* parent, int i, FILE* fp) {
    BTreeNode* y = loadNode(fp, parent->children_offsets[i]);
    BTreeNode* z = createNode(y->leaf, fp);

    z->n_keys = MAX_KEYS / 2; // Set the number of keys in the new node
    for (int j = 0; j < MAX_KEYS / 2; j++) {
        z->keys[j] = y->keys[j + MAX_KEYS / 2 + 1]; // Move keys to the new node
    }
    if (!y->leaf) { // If the old node is not a leaf
        for (int j = 0; j <= MAX_KEYS / 2; j++) {
            z->children_offsets[j] = y->children_offsets[j + MAX_KEYS / 2 + 1]; // Move child offsets to the new node
        }
    }
    y->n_keys = MAX_KEYS / 2; // Reduce the number of keys in the old node

    for (int j = parent->n_keys; j >= i + 1; j--) {
        parent->children_offsets[j + 1] = parent->children_offsets[j]; // Move child offsets to make space for the new child
    }
    parent->children_offsets[i + 1] = z->self_offset; // Set the new child's offset

    for (int j = parent->n_keys - 1; j >= i; j--) {
        parent->keys[j + 1] = parent->keys[j]; // Move keys to make space for the middle key
    }
    parent->keys[i] = y->keys[MAX_KEYS / 2]; // Move the middle key to the parent
    parent->n_keys++; // Increment the number of keys in the parent

    saveNode(z, fp); // Save the new node to the file
    saveNode(y, fp); // Save the old node to the file
    saveNode(parent, fp); // Save the parent node to the file

    printf("Split child node at offset %ld into two nodes at offsets %ld and %ld\n",
           y->self_offset, y->self_offset, z->self_offset);
}

// Searches for a record with the given datetime in the B-tree.
WeatherData* search(BTreeNode* root, char* datetime, FILE* fp) {
    if (root == NULL) {
        printf("Search failed: Root is NULL\n");
        return NULL;
    }

    int i = 0;
    while (i < root->n_keys && strcmp(datetime, root->keys[i].datetime) > 0) {
        i++;
    }
    if (i < root->n_keys && strcmp(datetime, root->keys[i].datetime) == 0) {
        printf("Found key %s at offset %ld\n", datetime, root->self_offset);
        return &root->keys[i]; // Return the data if found
    }
    if (root->leaf) {
        printf("Search failed: Reached leaf node without finding key %s\n", datetime);
        return NULL; // Return NULL if not found and the node is a leaf
    } else {
        printf("Traversing child at index %d of node at offset %ld\n", i, root->self_offset);
        BTreeNode* child = loadNode(fp, root->children_offsets[i]);
        return search(child, datetime, fp); // Recursively search the child
    }
}

// Traverses the B-tree and prints all records.
void traverse(BTreeNode* root, FILE* fp) {
    if (root == NULL) return;

    int i;
    for (i = 0; i < root->n_keys; i++) {
        if (!root->leaf) {
            BTreeNode* child = loadNode(fp, root->children_offsets[i]);
            traverse(child, fp); // Recursively traverse the child
        }
        // Print the weather data of the current key
        printf("Datetime: %s, Location: %s, Temp: %.2f, UV: %.2f, Pollen: %d, Wind: %.2f, Feels Like: %.2f, Pressure: %.2f, Rain: %.2f\n",
               root->keys[i].datetime, root->keys[i].location, root->keys[i].temp,
               root->keys[i].UV, root->keys[i].grass_pollen, root->keys[i].wind,
               root->keys[i].feels_like, root->keys[i].pressure, root->keys[i].rain);
    }
    if (!root->leaf) {
        BTreeNode* child = loadNode(fp, root->children_offsets[i]);
        traverse(child, fp); // Recursively traverse the last child
    }
}
