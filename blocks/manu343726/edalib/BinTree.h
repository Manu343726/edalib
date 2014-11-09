/**
 * @file BinTree.h
 *
 * A binary tree. 
 *
 * Estructura de Datos y Algoritmos
 *
 * Copyright (C) 2014
 * Facultad de Informática, Universidad Complutense de Madrid
 * This software is licensed under the Simplified BSD licence:
 *    (see the LICENSE file or
 *    visit opensource.org/licenses/BSD-3-Clause)
 */

#ifndef __BIN_TREE_H
#define __BIN_TREE_H

#include <iomanip>
#include <iterator>

#include "Util.h"
#include "Vector.h"
#include "Queue.h"

/**
 * A binary tree: each node can have left and right children.
 * Binary trees can be used, for example, to implement sorted 
 * trees and TreeMaps.
 * 
 * Unlike other modules in this library, this module provides full access 
 * to its internal implementation. To build trees, you *will* have 
 * to access nodes directly. A few utility methods to iterate and 
 * show trees are, however, provided.
 *
 * @author mfreire
 */
template <class Type>
struct BinTree{
private:
    /** */
    struct Node {
        Type _elem;   ///< actual element stored in node
        Node* _left;  ///< pointer to left child node, 0 if none
        Node* _right; ///< pointer to right child node, 0 if none
        
        Node(const Type& e, Node *left, Node *right)
        : _elem(e), _left(left), _right(right) {}
    };
    
    Node* _root; ///< root of the tree
    
    /**  */
    BinTree() : _root(0) {}
    
    /**  */
    BinTree(const BinTree& other) {
        _root = copyNode(other._root);
    }
    
    /**  */
    ~BinTree() {
        deleteNode(_root);
        _root = 0;
    }
    
    /**  */
    BinTree& operator=(const BinTree& other) {
        deleteNode(_root);
        _root = copyNode(other._root);
    }
    
    /**  */     
    Node *createNode(const Type& e, Node *left=0, Node *right=0) {
        return new Node(e, left, right);
    }
    
    /**  */     
    void deleteNode(Node*& node) {
        if (node) {
            deleteNode(node->_left);
            deleteNode(node->_right);
            delete node;
            node = 0;
        }
    }    
    
    /**  */     
    Node* copyNode(Node* n) {
        if (n) {
            return createNode(
                n->_elem, copyNode(n->_left), copyNode(n->_right));
        } else {
            return 0;
        }
    }    
   
    /**  */ 
    template <class Collection >
    void preorder(Collection &accumulator, Node *n) {
        if (n) {
            accumulator.push_back(n->_elem);
            preorder(accumulator, n->_left);
            preorder(accumulator, n->_right);
        }
    }
    
    /**  */ 
    template <class Collection >
    void inorder(Collection &accumulator, Node *n) {
        if (n) {
            inorder(accumulator, n->_left);
            accumulator.push_back(n->_elem);
            inorder(accumulator, n->_right);
        }
    }
    
    /**  */
    template <class Collection >
    void postorder(Collection &accumulator, Node *n) {
        if (n) {
            postorder(accumulator, n->_left);
            postorder(accumulator, n->_right);
            accumulator.push_back(n->_elem);
        }
    }
    
    /**  */
    template <class Collection >
    void levels(Collection &accumulator, Node *n) {
        if (n) {
            Queue<Node *> q;
            q.push(n);
            while (q.size()) {
                Node *current = q.top();
                accumulator.push_back(current->_elem);
                q.pop();
                if (current->_left) {
                    q.push(current->_left);
                }
                if (current->_right) {
                    q.push(current->_right);
                }
            }
        }
    }

    /**
     * pretty-print the tree contents. Format is similar to
     * <pre> 
     * *- 1 (root: preceeded by '*')
     *    +- 1.1 (first child of 2: preceeded by '+')
     *    |  ~- 1.1.1 (single left child: '~')
     *    `- 1.2 (second child of 2: '`')
     *       `- 1.2.2 (single right child: '`')
     * </pre>
     * empty nodes are not represented.
     */
    void print(Node *n, std::ostream& out=std::cout) const {
        Vector<char> bars;
        if (n) {
            out << "*- " << n->_elem << std::endl;
            bars.push_back(' ');
            bool full = n->_left && n->_right;
            _print(n->_left, bars, full ? '+' : '~', out);
            _print(n->_right, bars, '`', out);       
            bars.pop_back();
        }
    }
    
private:
    
    void _print(Node *n, Vector<char>& bars, char nodeChar,
               std::ostream &out) const {
        if (n) {
            for (std::size_t i=0; i<bars.size(); i++) {
                out << bars.at(i) << std::setw(3);
            }
            out << nodeChar << "- " << n->_elem << std::endl;
            bars.push_back(nodeChar == '`' || nodeChar == '~' ? ' ' : '|');
            bool full = n->_left && n->_right;
            _print(n->_left, bars, full ? '+' : '~', out);
            _print(n->_right, bars, '`', out);            
            bars.pop_back();
        }
    }
};

#endif // __BIN_TREE_H
