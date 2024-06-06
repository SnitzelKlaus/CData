// btree.h
#ifndef BTREE_H
#define BTREE_H

#include <stdio.h>
#include <stdbool.h>
#include "weather_data.h"

#define MAX_KEYS 4

typedef struct BTreeNode {
    int n_keys;    // Number of keys in node
    bool leaf;     // True if node is a leaf
    long children_offsets[MAX_KEYS+1];  // File offsets for child nodes
    WeatherData keys[MAX_KEYS]; // Keys stored in node (sorted)
    long self_offset;  // Disk offset for this node
    bool dirty;  // True if node has been modified
    bool deleted;  // True if node has been deleted
    long last_modified;  // Timestamp of last modification
} BTreeNode;

BTreeNode* createNode(bool leaf, FILE* fp);
void insert(BTreeNode** root, WeatherData data, FILE* fp);
void splitChild(BTreeNode* parent, int i, FILE* fp);
void insertNonFull(BTreeNode* node, WeatherData data, FILE* fp);
WeatherData* search(BTreeNode* root, char* datetime, FILE* fp);
void saveNode(BTreeNode* node, FILE* fp);
BTreeNode* loadNode(FILE* fp, long offset);
void traverse(BTreeNode* root, FILE* fp);
void delete(BTreeNode* root, char* datetime, FILE* fp);
void deleteFromLeaf(BTreeNode* node, int i);
void deleteFromNonLeaf(BTreeNode* node, int i, FILE* fp);
WeatherData* getPredecessor(BTreeNode* node, int i, FILE* fp);
WeatherData* getSuccessor(BTreeNode* node, int i, FILE* fp);
void fill(BTreeNode* node, int i, FILE* fp);
void borrowFromPrev(BTreeNode* node, int i, FILE* fp);
void borrowFromNext(BTreeNode* node, int i, FILE* fp);
void merge(BTreeNode* node, int i, FILE* fp);
void deleteKey(BTreeNode* node, int i, FILE* fp);
void deleteTree(BTreeNode* root, FILE* fp);

#endif
