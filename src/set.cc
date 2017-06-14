/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "set.h"
#include <stddef.h>
#include <cassert>

Node::Node(uint64 val, Node* r, Node* l){
    range=false;
    beginOrVal=val;
    left = l;
    right=r;
}

Node::Node(uint64 val):Node(val, NULL, NULL){}

Node::Node(uint64 begin, uint64 end, Node* r, Node* l){
    range=true;
    beginOrVal=begin;
    this->end= end;
    right=r;
    left=l;
}

Node::Node(uint64 begin, uint64 end):Node(begin, end, NULL, NULL){}

Comparison Node::compare(uint64 val){
    if(range){
        if(val> end)
            return GREATER;
        else if(val < beginOrVal)
            return LESS;
        else
            return EQUAL;
    }else{
        if(val> beginOrVal)
            return GREATER;
        else if(val< beginOrVal)
            return LESS;
        else
            return EQUAL;
    }
}

void Node::addNode(Node* n){
    assert(!n->isRange());
    Comparison comp = compare(n->getBeginOrRange());
    if(comp == GREATER){
        if(right!=NULL)
            right->addNode(n);
        else
            right=n;
    }else if(comp == LESS){
        if(left!= NULL)
            left->addNode(n);
        else
            left = n;
    }
        
}

Set::Set(Type t, uint64* elements, int num){
    type=t;
    size=num;
    for(int i=0; i<num; i++){
        addItem(elements[i]);
    }
}

Set::Set(Type t, uint64 lowrange, uint64 highrange){
    assert(highrange>lowrange);
    type =t;
    size = highrange-lowrange+1;
    root = new Node(lowrange,highrange);
}

Set::Set(Type t): type(t),
        size(0),
        root(NULL)
{
}

void Set::addItem(uint64 element){
    Node* n = new Node(element);
    if(root==NULL)
        root=n;
    else
        root->addNode(n);
}

        