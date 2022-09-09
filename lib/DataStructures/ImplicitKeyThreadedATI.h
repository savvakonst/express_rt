// MIT License
//
// Copyright (c) 2022 Savva Konstantinov
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

//
// Created by SVK on 18.07.2022.
//
#ifndef ATI_IMPLICIT_KEY_THREADED_ATI_H
#define ATI_IMPLICIT_KEY_THREADED_ATI_H

#include <stdint.h>
#include <string.h>

typedef void *tree_key_t;
typedef void *tree_data_t;

typedef unsigned char tree_height_t;
typedef char tree_diff_t;

struct ImKeyAtiNode {
    // tree_key_t key_;
    size_t key_;
    tree_data_t data_;

    struct ImKeyAtiNode *left_branch_;  /**< key_ of left_branch_ is less then current */
    struct ImKeyAtiNode *right_branch_; /**< key_ of right_branch_ is more then current */
    struct ImKeyAtiNode *previous_;     /**< key_ of previous_ is less then current */
    struct ImKeyAtiNode *next_;         /**< key_ of next_ is more then current */

    tree_height_t height_; /**< equal 1 when it has no branches*/
};

typedef struct ImKeyAtiNode **imKeyAtiNodeStackItem_t;
typedef imKeyAtiNodeStackItem_t *imKeyAtiNodeStack_t;

/**
 * type of function which removes only key and data without removing node
 */
typedef void (*deleteKeyAndDataF_t)(struct ImKeyAtiNode *);

struct ImKeyAti {
    struct ImKeyAtiNode *top_node_;
    /// first element of node stack always equal NULL
    imKeyAtiNodeStack_t stack_;
};

struct ImKeyAti *newImKeyAti();

/**
 * frees the memory allocated for avl_tree and its associated nodes, keys and data.
 */
void deleteImKeyAti(struct ImKeyAti *avl_tree, deleteKeyAndDataF_t delete_data_f);

/**
 * frees the memory allocated for node structure.
 * It doesn't free the memory allocated for key_ and data_
 */
void deleteImKeyAtiNode(struct ImKeyAtiNode *node);

struct ImKeyAtiNode *getFirstImKeyAtiNode(struct ImKeyAti *avl_tree);

/**
 * if equal key exists it removes node from tree structure and returns pointer to the corresponding node
 */
struct ImKeyAtiNode *findImKeyAtiNode(struct ImKeyAti *avl_tree, tree_key_t key);

/**
 *  if equal key exists it returns node pointer without insertion
 * (developer responsible for changing data/insertion.), otherwise it creates a new
 * node, inserts that node into the, balances the tree and returns NULL.
 */
struct ImKeyAtiNode *insertImKeyAtiNode(struct ImKeyAti *avl_tree, const tree_key_t *key_p, tree_data_t data);

/**
 *  if equal key exists it removes node from tree structure and returns pointer to the corresponding node
 *  ( developer is responsible for freeing memory), otherwise it returns NULL.
 */
struct ImKeyAtiNode *removeImKeyAtiNode(struct ImKeyAti *avl_tree, const tree_key_t *key_p);

struct ImKeyAtiNode *findImKeyAtiNodeWithIndex(struct ImKeyAti *avl_tree, tree_key_t key, size_t *index);

struct ImKeyAtiNode *findImKeyAtiNodeByIndex(struct ImKeyAti *avl_tree, size_t index);

#endif