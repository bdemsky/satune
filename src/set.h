/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   set.h
 * Author: hamed
 *
 * Created on June 13, 2017, 3:01 PM
 */

#ifndef SET_H
#define SET_H

#include "types.h"

enum Comparison{LESS, EQUAL, GREATER};

class Node{
private:
    bool range;
    // If it isn't a range, begin contains the actual value of the element
    uint64 beginOrVal;
    uint64 end;
    Node* right;
    Node* left;
public:
    Node(uint64 val, Node* r, Node* l);
    Node(uint64 val);
    Node(uint64 begin, uint64 end, Node* r, Node* l);
    Node(uint64 begin, uint64 end);
    bool isRange(){return range;}
    uint64 getBeginOrRange(){return beginOrVal;}
    Comparison compare(uint64 val);
    /**
     * Searches the tree, if new node exists in the tree ( whether its value
     * is in range of another node, or there is another node with the value of
     * node n) this function just returns!
     * @param n
     */
    void addNode(Node* n);
};
// For now, we can consider it as a simple binary tree, but we can have fancier
// trees for future
class Set{
    Type type;
    uint64 size;
    Node* root;
    Set(Type t, uint64 * elements, int num);
    Set(Type t, uint64 lowrange, uint64 highrange);
    Set(Type t);
    /**
     * For know all sets are considered to be mutable, we can change it later on
     * if it was necessary.
     * @param set
     * @param element
     */
    void addItem(uint64 element);
    ELEMENT* createUniqueItem(Set * set);

};


#endif /* SET_H */

