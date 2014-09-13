/**
 * @file SingleList.h
 *
 * A singly-linked list.
 *
 * Estructura de Datos y Algoritmos
 *
 * Copyright (C) 2014
 * Facultad de Informática, Universidad Complutense de Madrid
 * This software is licensed under the Simplified BSD licence:
 *    (see the LICENSE file or
 *    visit opensource.org/licenses/BSD-3-Clause)
 */

#ifndef __SINGLE_LIST_H
#define __SINGLE_LIST_H

#include "Util.h"

DECLARE_EXCEPTION(SingleListEmpty)
DECLARE_EXCEPTION(SingleListOutOfBounds)

/**
 * A singly-linked list. Slow access (except for front and back),
 * but fast insertion anywhere once you are at the right position.
 * Erasing the first element, or 'next' elements in general is fast.
 *
 * push_front, push_back, and pop_front are O(1)
 * 
 * @author mfreire
 */
template <class Type>
class SingleList {
    
    /** */
    struct Node {
        Type _elem;   ///< actual element stored in node
        Node* _next;  ///< pointer to next node in list, 0 if none
        
        Node(const Type& e, Node *next)
            : _elem(e), _next(next) {}
    };
        
    Node* _first;  ///< first element in list, 0 if empty
    Node* _last;   ///< last element in list, 0 if empty
    std::size_t _size;    ///< number of elements in list

public:
    
    /**  */
    SingleList() : _first(0), _last(0), _size(0) {}
    
    /**  */
    SingleList(const SingleList& other) : _first(0), _last(0), _size(0) {
        Node *n = other._first;
        while (n) {
            push_back(n->_elem);
            n = n->_next;
        }
    }
    
    /**  */
    ~SingleList() {
        _clear();
    }
    
    /** */
    SingleList& operator=(const SingleList& other) {
        _clear();
        Node *n = other._first;
        while (n) {
            push_back(n->_elem);
            n = n->_next;
        }
        return (*this);
    }    
    
    /**  */
    std::size_t size() const {
        return _size;
    }

    class Iterator {
    public:
        void next() {
            _current = _current->_next;
        }

        const Type& elem() const {
            return _current->_elem;
        }
        
        Type& elem() { NON_CONST_VARIANT(Type, Iterator, elem()); }
        
        void set(const Type& elem) {
            _current->_elem = elem;
        }
        
        bool operator==(const Iterator &other) const {
            return _current == other._current;
        }

        bool operator!=(const Iterator &other) const {
            return _current != other._current;
        }
    protected:
        friend class SingleList;
        
        Node* _current;
        
        Iterator(Node *n) : _current(n) {}
    };
    
    /** */
    const Iterator find(const Type& e) const {
        for (Iterator it(_first); it != end(); it.next()) {
            if (e == it.elem()) {
                return it;
            }
        }
        return end();
    }
    
    /** */
    Iterator begin() const {
        return Iterator(_first);
    }
    
    /** */
    Iterator end() const {
        return Iterator(0);
    }
    
    /**  */
    void push_back(const Type& e) {
        Node *n = new Node(e, 0);
        if (_size == 0) {
            _first = _last = n;
        } else {
            _last->_next = n;
            _last = n;
        }
        _size ++;
    }

    /**  */
    const Type& back() const {
        if (_size == 0) {
            throw SingleListEmpty("back");
        }
        return _last->_elem;  
    }

    /**  */
    Type& back() {
        NON_CONST_VARIANT(Type,SingleList,back());
    }

    /**  */
    void push_front(const Type& e) {
        Node *n = new Node(e, _first);
        if (_size == 0) {
            _first = _last = n;
        } else {
            _first = n;
        }
        _size ++;
    }
    
    /**  */
    const Type& front() const {
        if (_size == 0) {
            throw SingleListEmpty("front");
        }
        return _first->_elem;
    }
    
    /**  */
    Type& front() {
        NON_CONST_VARIANT(Type,SingleList,front());
    }
    
    /**  */
    void pop_front() {
        if (_size == 0) {
            throw SingleListEmpty("pop_front");
        } else if (_size == 1) {
            delete _first;
            _first = _last = 0;
        } else {
            Node *n = _first->_next;
            delete _first;
            _first = n;            
        }
        _size --;
    }

private:
    
    void _clear() {
        while (_first) {
            Node *n = _first;
            _first = _first->_next;
            delete n;
        }        
        _first = _last = 0;
        _size = 0;
    }
};

#endif // __SINGLE_LIST_H
