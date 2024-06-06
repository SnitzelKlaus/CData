// btree.c
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "btree.h"

// Creates a new B-tree node. If leaf is true, it is a leaf node. The node is written to the file pointed to by fp.
BTreeNode* createNode(bool leaf, FILE* fp) {
    // Allocate memory for a new BTreeNode
    BTreeNode* node = (BTreeNode*)malloc(sizeof(BTreeNode));

    // Check if memory allocation was successful
    if (node == NULL) {
        perror("Failed to allocate memory for BTreeNode");
        exit(EXIT_FAILURE);
    }

    // Initialize the node's attributes
    node->n_keys = 0;
    node->leaf = leaf;
    node->dirty = false;
    node->deleted = false;
    node->last_modified = time(NULL);  // Set the last modified timestamp to the current time

    // Initialize the children_offsets array to -1
    for (int i = 0; i < MAX_KEYS + 1; i++) {
        node->children_offsets[i] = -1;
    }

    // Initialize the keys array
    for (int i = 0; i < MAX_KEYS; i++) {
        memset(&node->keys[i], 0, sizeof(WeatherData));
    }

    // Move the file pointer to the end of the file and set the node's self_offset
    fseek(fp, 0, SEEK_END);
    node->self_offset = ftell(fp);

    // Save the newly created node to the file
    saveNode(node, fp);

    // Print the creation message
    printf("Created new %s node at offset %ld\n", leaf ? "leaf" : "internal", node->self_offset);

    return node;
}


// Saves the node to the file at the node's offset.
void saveNode(BTreeNode *node, FILE *fp) {
    if (fseek(fp, node->self_offset, SEEK_SET) != 0) {
        perror("Failed to seek to the node's offset in the file");
        exit(EXIT_FAILURE);
    }

    if (fwrite(node, sizeof(BTreeNode), 1, fp) != 1) {
        perror("Failed to write the node to the file");
        exit(EXIT_FAILURE);
    }

    fflush(fp);  // Ensure the node is written to disk
}


// Loads a node from the file at the given offset. Returns NULL if the offset is -1.
BTreeNode* loadNode(FILE* fp, long offset) {
    if (offset == -1) return NULL;
    
    BTreeNode* node = (BTreeNode*)malloc(sizeof(BTreeNode));
    
    if (node == NULL) {
        perror("Failed to allocate memory for BTreeNode");
        exit(EXIT_FAILURE);
    }
    
    printf("Before loading node, seeking to offset %ld\n", offset);
    if (fseek(fp, offset, SEEK_SET) != 0) {
        perror("Failed to seek to node offset");
        exit(EXIT_FAILURE);
    }
    printf("After seeking, current file offset is %ld\n", ftell(fp));
    
    if (fread(node, sizeof(BTreeNode), 1, fp) != 1) {
        perror("Failed to read node from file");
        exit(EXIT_FAILURE);
    }
    
    // Validate node data after loading
    printf("Loaded node with %d keys from offset %ld\n", node->n_keys, offset);
    for (int i = 0; i < node->n_keys; i++) {
        printf("Loaded key %d: %s\n", i, node->keys[i].datetime);
    }
    
    return node;
}

// Inserts a data record into B-tree
void insert(BTreeNode** root, WeatherData data, FILE* fp) {
    // Create a new root node if root is NULL
    if (*root == NULL) {
        *root = createNode(1, fp);
    }

    BTreeNode* r = *root;

    // If the root is full, create a new root and split the old root
    if (r->n_keys == MAX_KEYS) {
        BTreeNode* s = createNode(0, fp);
        *root = s;
        s->children_offsets[0] = r->self_offset;

        splitChild(s, 0, fp);
        insertNonFull(s, data, fp);

        printf("Root was full. Created new root and split the old root.\n");
    } else {
        insertNonFull(r, data, fp);
    }
}

// Inserts data into a non-full node.
void insertNonFull(BTreeNode* node, WeatherData data, FILE* fp) {
    int i = node->n_keys - 1;

    if (node->leaf) {
        while (i >= 0 && strcmp(data.datetime, node->keys[i].datetime) > 0) {
            node->keys[i + 1] = node->keys[i];
            i--;
        }

        node->keys[i + 1] = data;
        node->n_keys++;

        saveNode(node, fp);

        printf("Inserted key into leaf node at offset %ld\n", node->self_offset);
    } else {
        while (i >= 0 && strcmp(data.datetime, node->keys[i].datetime) > 0) {
            i--;
        }
        
        i++;
        BTreeNode* child = loadNode(fp, node->children_offsets[i]);

        if (child->n_keys == MAX_KEYS) {
            splitChild(node, i, fp);
            if (strcmp(data.datetime, node->keys[i].datetime) > 0) {
                i++;
            }
        }
        
        insertNonFull(child, data, fp);
        free(child);
    }
}

// Splits the child node at index i of the parent node.
void splitChild(BTreeNode* parent, int i, FILE* fp) {
    BTreeNode* y = loadNode(fp, parent->children_offsets[i]);
    BTreeNode* z = createNode(y->leaf, fp);

    z->n_keys = MAX_KEYS / 2;
    for (int j = 0; j < MAX_KEYS / 2; j++) {
        z->keys[j] = y->keys[j + MAX_KEYS / 2 + 1];
    }

    if (!y->leaf) {
        for (int j = 0; j <= MAX_KEYS / 2; j++) {
            z->children_offsets[j] = y->children_offsets[j + MAX_KEYS / 2 + 1];
        }
    }

    y->n_keys = MAX_KEYS / 2;

    for (int j = parent->n_keys; j >= i + 1; j--) {
        parent->children_offsets[j + 1] = parent->children_offsets[j];
    }

    parent->children_offsets[i + 1] = z->self_offset;

    for (int j = parent->n_keys - 1; j >= i; j--) {
        parent->keys[j + 1] = parent->keys[j];
    }

    parent->keys[i] = y->keys[MAX_KEYS / 2];
    parent->n_keys++;

    saveNode(z, fp);
    saveNode(y, fp);
    saveNode(parent, fp);

    printf("Split child node at offset %ld into two nodes at offsets %ld and %ld\n",
           y->self_offset, y->self_offset, z->self_offset);

    free(y);
    free(z);
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
        return &root->keys[i];
    }

    if (root->leaf) {
        printf("Search failed: Reached leaf node without finding key %s\n", datetime);
        return NULL;
    } else {
        printf("Traversing child at index %d of node at offset %ld\n", i, root->self_offset);
        BTreeNode* child = loadNode(fp, root->children_offsets[i]);
        WeatherData* result = search(child, datetime, fp);
        free(child);
        return result;
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
            free(child); // Free the memory after use
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
        free(child); // Free the memory after use
    }
}

// Deletes a record with the given datetime from the B-tree.
void delete(BTreeNode* root, char* datetime, FILE* fp) {
    if (root == NULL) {
        printf("Delete failed: Root is NULL\n");
        return;
    }

    int i = 0;
    while (i < root->n_keys && strcmp(datetime, root->keys[i].datetime) > 0) {
        i++;
    }

    if (i < root->n_keys && strcmp(datetime, root->keys[i].datetime) == 0) {
        if (root->leaf) {
            deleteFromLeaf(root, i);
            saveNode(root, fp);
        } else {
            deleteFromNonLeaf(root, i, fp);
        }
    } else {
        if (root->leaf) {
            printf("Delete failed: Key %s not found in leaf node at offset %ld\n", datetime, root->self_offset);
            return;
        }

        BTreeNode* child = loadNode(fp, root->children_offsets[i]);
        delete(child, datetime, fp);
        free(child);
    }
}

// Deletes a key from a leaf node.
void deleteFromLeaf(BTreeNode* node, int i) {
    for (int j = i + 1; j < node->n_keys; j++) {
        node->keys[j - 1] = node->keys[j];
    }

    node->n_keys--;
}

// Deletes a key from a non-leaf node.
void deleteFromNonLeaf(BTreeNode* node, int i, FILE* fp) {
    WeatherData key = node->keys[i];

    BTreeNode* child = loadNode(fp, node->children_offsets[i]);
    if (child->n_keys >= MAX_KEYS / 2 + 1) {
        WeatherData* pred = getPredecessor(node, i, fp);
        node->keys[i] = *pred;
        delete(child, pred->datetime, fp);
        free(pred);
    } else {
        BTreeNode* sibling = NULL;
        if (i < node->n_keys) {
            sibling = loadNode(fp, node->children_offsets[i + 1]);
        } else {
            sibling = loadNode(fp, node->children_offsets[i - 1]);
        }

        if (sibling->n_keys >= MAX_KEYS / 2 + 1) {
            if (i < node->n_keys) {
                borrowFromNext(node, i, fp);
            } else {
                borrowFromPrev(node, i, fp);
            }
        } else {
            if (i < node->n_keys) {
                merge(node, i, fp);
            } else {
                merge(node, i - 1, fp);
            }
        }
    }

    free(child);
}

// Returns the predecessor of the key at index i in the node.
WeatherData* getPredecessor(BTreeNode* node, int i, FILE* fp) {
    BTreeNode* current = loadNode(fp, node->children_offsets[i]);
    while (!current->leaf) {
        current = loadNode(fp, current->children_offsets[current->n_keys]);
    }

    WeatherData* pred = &current->keys[current->n_keys - 1];
    free(current);
    return pred;
}

// Returns the successor of the key at index i in the node.
WeatherData* getSuccessor(BTreeNode* node, int i, FILE* fp) {
    BTreeNode* current = loadNode(fp, node->children_offsets[i + 1]);
    while (!current->leaf) {
        current = loadNode(fp, current->children_offsets[0]);
    }

    WeatherData* succ = &current->keys[0];
    free(current);
    return succ;
}

// Fills the child node at index i in the parent node.
void fill(BTreeNode* node, int i, FILE* fp) {
    if (i != 0 && loadNode(fp, node->children_offsets[i - 1])->n_keys >= MAX_KEYS / 2 + 1) {
        borrowFromPrev(node, i, fp);
    } else if (i != node->n_keys && loadNode(fp, node->children_offsets[i + 1])->n_keys >= MAX_KEYS / 2 + 1) {
        borrowFromNext(node, i, fp);
    } else {
        if (i != node->n_keys) {
            merge(node, i, fp);
        } else {
            merge(node, i - 1, fp);
        }
    }
}

// Borrows a key from the child node at index i - 1.
void borrowFromPrev(BTreeNode* node, int i, FILE* fp) {
    BTreeNode* child = loadNode(fp, node->children_offsets[i]);
    BTreeNode* sibling = loadNode(fp, node->children_offsets[i - 1]);

    for (int j = child->n_keys - 1; j >= 0; j--) {
        child->keys[j + 1] = child->keys[j];
    }

    if (!child->leaf) {
        for (int j = child->n_keys; j >= 0; j--) {
            child->children_offsets[j + 1] = child->children_offsets[j];
        }
    }

    child->keys[0] = node->keys[i - 1];

    if (!child->leaf) {
        child->children_offsets[0] = sibling->children_offsets[sibling->n_keys];
    }

    node->keys[i - 1] = sibling->keys[sibling->n_keys - 1];

    child->n_keys++;
    sibling->n_keys--;

    saveNode(child, fp);
    saveNode(sibling, fp);
    saveNode(node, fp);

    free(child);
    free(sibling);
}

// Borrows a key from the child node at index i + 1.
void borrowFromNext(BTreeNode* node, int i, FILE* fp) {
    BTreeNode* child = loadNode(fp, node->children_offsets[i]);
    BTreeNode* sibling = loadNode(fp, node->children_offsets[i + 1]);

    child->keys[child->n_keys] = node->keys[i];

    if (!child->leaf) {
        child->children_offsets[child->n_keys + 1] = sibling->children_offsets[0];
    }

    node->keys[i] = sibling->keys[0];

    for (int j = 1; j < sibling->n_keys; j++) {
        sibling->keys[j - 1] = sibling->keys[j];
    }

    if (!sibling->leaf) {
        for (int j = 1; j <= sibling->n_keys; j++) {
            sibling->children_offsets[j - 1] = sibling->children_offsets[j];
        }
    }

    child->n_keys++;
    sibling->n_keys--;

    saveNode(child, fp);
    saveNode(sibling, fp);
    saveNode(node, fp);

    free(child);
    free(sibling);
}

// Merges the child node at index i with its sibling.
void merge(BTreeNode* node, int i, FILE* fp) {
    BTreeNode* child = loadNode(fp, node->children_offsets[i]);
    BTreeNode* sibling = loadNode(fp, node->children_offsets[i + 1]);

    child->keys[MAX_KEYS / 2] = node->keys[i];

    for (int j = 0; j < sibling->n_keys; j++) {
        child->keys[MAX_KEYS / 2 + 1 + j] = sibling->keys[j];
    }

    if (!child->leaf) {
        for (int j = 0; j <= sibling->n_keys; j++) {
            child->children_offsets[MAX_KEYS / 2 + 1 + j] = sibling->children_offsets[j];
        }
    }

    for (int j = i + 1; j < node->n_keys; j++) {
        node->keys[j - 1] = node->keys[j];
    }

    for (int j = i + 2; j <= node->n_keys; j++) {
        node->children_offsets[j - 1] = node->children_offsets[j];
    }

    child->n_keys += sibling->n_keys + 1;
    node->n_keys--;

    saveNode(child, fp);
    saveNode(node, fp);

    free(child);
    free(sibling);
}

// Deletes a key at index i from the node.
void deleteKey(BTreeNode* node, int i, FILE* fp) {
    if (node->leaf) {
        deleteFromLeaf(node, i);
    } else {
        deleteFromNonLeaf(node, i, fp);
    }
}

// Deletes the entire B-tree.
void deleteTree(BTreeNode* root, FILE* fp) {
    if (root == NULL) return;

    for (int i = 0; i < root->n_keys; i++) {
        if (!root->leaf) {
            BTreeNode* child = loadNode(fp, root->children_offsets[i]);
            deleteTree(child, fp);
            free(child);
        }
    }

    if (!root->leaf) {
        BTreeNode* child = loadNode(fp, root->children_offsets[root->n_keys]);
        deleteTree(child, fp);
        free(child);
    }

    free(root);
}