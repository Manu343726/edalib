/**
 * @file TreeMap.h
 *
 * An map implemented using a sorted tree. Similar to std::map
 *
 * Estructura de Datos y Algoritmos
 *
 * Copyright (C) 2014
 * Facultad de Informática, Universidad Complutense de Madrid
 * This software is licensed under the Simplified BSD licence:
 *    (see the LICENSE file or
 *    visit opensource.org/licenses/BSD-3-Clause)
 */

#ifndef __TREEMAP_H
#define __TREEMAP_H

#include "MapEntry.h"
#include "Util.h"
#include "BinTree.h"
#include "Stack.h"

DECLARE_EXCEPTION(TreeMapNoSuchElement)
DECLARE_EXCEPTION(TreeMapInvalidAccess)

/**
 * A map implemented using a sorted binary tree. If the tree is kept
 * balanced, this has a guaranteed O(log N) time for lookups, insertions
 * and removals. However, this implementation does not make the effort
 * of keeping it balanced, so it can be much worse.
 * 
 * @author mfreire
 */
template <class KeyType, class ValueType>
class TreeMap : public util::container_traits<TreeMap<KeyType,ValueType>,ValueType> {
private:
    typedef MapEntry<KeyType, ValueType> Entry;
    typedef typename BinTree<Entry>::Node Node;
    
    BinTree<Entry> _t; ///< sorted binary tree for key-value entries
    std::size_t _entryCount;  ///< number of key-value entries in tree
    
public:

    /**  */
    TreeMap() : _t(), _entryCount(0) {}

    /**  */
    std::size_t size() const {
        return _entryCount;
    }

    class Iterator : util::edatocpp_iterator_adapter<Iterator,Entry> {
    public:
        void next() {
            if ( ! _current) {
                throw TreeMapInvalidAccess("next");
            } else if (_current->_right) {
                _current = _firstInOrder(_current->_right);
            } else if (_ascendants.size()) {
                _current = _ascendants.top();
                _ascendants.pop();
            } else {
                _current = 0;
            }
        }
        
        const Entry& elem() const {
            return _current->_elem;
        }
        
        const ValueType& value() const {
            return _current->_elem._value;
        }
        
        const KeyType& key() const {
            return _current->_elem._key;
        }
        
        bool operator==(const Iterator &other) const {
            return _current == other._current;
        }
        
        bool operator!=(const Iterator &other) const {
            return _current != other._current;
        }        
        
        //Note that an iterator should always be default constructible
        Iterator() = default;
        
    protected:
        friend class TreeMap;
        
        /** */
        Node* _current;
        
        /** */
        Stack<Node*> _ascendants;

        /** */        
        Iterator(Node* start) {
            _current = _firstInOrder(start);
        }        
              
        /** */
        Iterator(Node* start, const KeyType& key) {
            Node *n = start;
            bool found = false;
            while (n && ! found) {
                const KeyType& nodeKey = n->_elem._key;
                if (nodeKey == key) {
                    found = true;
                } else {
                    if (nodeKey < key) {
                        _ascendants.push(n);
                        n = n->_left;                                                
                    } else {
                        n = n->_right;                        
                    }
                }
            }
            _current = _firstInOrder(n);
        }        
        
        Node *_firstInOrder(Node *n) {
            if (n) {
                while (n->_left) {
                    _ascendants.push(n);
                    n = n->_left;
                }
            }
            return n;
        }
    };
    
    ADD_ITERATOR_TRAITS()
    
    /** */
    const Iterator find(const KeyType& key) const {
        return Iterator(_t._root, key);
    }
    
    /** */
    Iterator begin() const {
        return Iterator(_t._root);
    }
    
    /** */
    Iterator end() const {
        return Iterator(0);
    }
    
    /** */
    const ValueType& at(const KeyType& key) const {        
        Node *p = _t._root;
        bool leftChild;
        Node *n = _nodeFor(key, p, leftChild);
        if ( ! n) {
            throw TreeMapNoSuchElement("at");
        }
        return n->_elem._value;
    }
    
    /** */
    ValueType& at(const KeyType& key) {
        NON_CONST_VARIANT(ValueType,TreeMap,at(key));
    }
    
    /** */
    void insert(const KeyType& key, const ValueType& value) {
        if ( ! _t._root) {
            _t._root = _t.createNode(Entry(key, value));
            _entryCount ++;            
        } else {
            Node *p = _t._root;
            bool leftChild;
            Node *n = _nodeFor(key, p, leftChild);
            if ( ! n) {
                if (leftChild) {
                    p->_left = _t.createNode(Entry(key, value));
                } else {
                    p->_right = _t.createNode(Entry(key, value));
                }                    
                _entryCount ++;
            } else {
                n->_elem._value = value;
            }
        }
    }
    
    /** */
    void erase(const KeyType& key) {
        Node *p = _t._root;
        bool leftChild; 
        Node *n = _nodeFor(key, p, leftChild);
        if ( ! n) {
            throw TreeMapNoSuchElement("erase");
        } else {
            if (n == _t._root) {
                _t._root = _erase(n);
            } else if (leftChild) {
                p->_left = _erase(n);
            } else {
                p->_right = _erase(n);
            }
            _entryCount --;
        }
    }
    
    /** */
    void print(std::ostream &out=std::cout) {
        _t.print(_t._root, out);
    } 
    
    /** */
    void diagnose(std::ostream &out=std::cout) {
        std::size_t depth = 0, max = 0;
        std::size_t totalDepth = 0;
        _diagnose(_t._root, depth, max, totalDepth);
        float avg = ( 1.0 / _entryCount) *  totalDepth;     
        std::size_t roundedAvg = (int)avg;
        std::size_t maxForDepth = 0;
        for (std::size_t i=0; i<roundedAvg; i++) {
            maxForDepth += (1<<i);
        }
        out << "total of " << _entryCount 
             << " nodes; avg path length is " << avg << " max is " << max
             << " density is " << (100.0 * _entryCount / (float)maxForDepth) << "%" 
             << std::endl;
    } 
    
private:

    /**
     * Calculates average path-length stats for the tree
     */
    static void _diagnose(Node *n, std::size_t depth, std::size_t &max, std::size_t &total) {
        if (n) {
            depth ++;
            total += depth;
            max = (depth > max) ? depth : max;
            _diagnose(n->_left, depth, max, total);
            _diagnose(n->_right, depth, max, total);
        }
    }
    
    /**
     * Erases a node, promoting and reordering children as needed
     * so as to return a tree that is ordered, with only the
     * deleted node missing.
     * @param n node to erase
     * @return a descendant of n, with its own children reordered so that
     * it forms a valid sorted binary tree.
     */
    static Node *_erase(Node *n) {
        Node *replacement;
        if ( ! n->_left) {
            // easy, promote the right child
            replacement = n->_right;
        } else if ( ! n->_right) {
            // easy, promote the left child
            replacement = n->_left;
        } else {
            // interesting; promote the smallest-of-right
            // (largest-of-left would also have worked)
            Node *parentOfSmallest = 0;
            Node *smallest = n->_right;
            while (smallest->_left) {
                parentOfSmallest = smallest;
                smallest = smallest->_left;
            }
            
            if (parentOfSmallest) {
                // smallest has a parent; disconnect smallest
                parentOfSmallest->_left = smallest->_right;
                smallest->_left = n->_left;
                smallest->_right = n->_right;
            } else {
                // smallest is n->_right; no need to disconnect
                smallest->_left = n->_left;
            }
            replacement = smallest;
        }
        delete n;
        return replacement;
    }

    /**
     * Searches the tree for the node that contains a key and
     * its parent.
     * @param key key to search for
     * @param parent initial node to search from; modified during
     * search to point to the parent of the returned node; or
     * if the node is not found, the parent where the node
     * should be inserted.
     * @param left set to true if the node is (or should be) to
     * the left of parent; false if to the right or undecided
     * @return node with the key, 0 if not found
     */
    static Node *_nodeFor(const KeyType& key, Node*& parent, bool &left) {
        Node *n = parent;
        while (n) {
            const KeyType& nodeKey = n->_elem._key;
            if (nodeKey == key) {
                return n;
            } else {
                parent = n;
                if (nodeKey < key) {
                    left = true;
                    n = n->_left;
                } else {
                    left = false;
                    n = n->_right;
                }
            }
        }
        return n;
    }    
};

#endif // __TREEMAP_H
