// btree.h
#ifndef BTREE_H
#define BTREE_H

#include <stdio.h>
#include <stdbool.h>
#include "weather_data.h"

#define MAX_KEYS 4

typedef struct BTreeNode {
    int n_keys;    // Number of keys
    bool leaf;     // Is true when node is leaf
    long children_offsets[MAX_KEYS+1];  // File offsets for child nodes
    WeatherData keys[MAX_KEYS];
    long self_offset;  // Disk offset for this node
} BTreeNode;

BTreeNode* createNode(bool leaf, FILE* fp);
void insert(BTreeNode** root, WeatherData data, FILE* fp);
void splitChild(BTreeNode* parent, int i, FILE* fp);
void insertNonFull(BTreeNode* node, WeatherData data, FILE* fp);
WeatherData* search(BTreeNode* root, char* datetime, FILE* fp);
void saveNode(BTreeNode* node, FILE* fp);
BTreeNode* loadNode(FILE* fp, long offset);
void traverse(BTreeNode* root, FILE* fp);

#endif
