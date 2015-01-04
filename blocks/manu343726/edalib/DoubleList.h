/**
 * @file DoubleList.h
 *
 * A singly-linked list. Similar to std::list
 *
 * Estructura de Datos y Algoritmos
 *
 * Copyright (C) 2014
 * Facultad de Informática, Universidad Complutense de Madrid
 * This software is licensed under the Simplified BSD licence:
 *    (see the LICENSE file or
 *    visit opensource.org/licenses/BSD-3-Clause)
 */

#ifndef __DOUBLE_LIST_H
#define __DOUBLE_LIST_H

#include "Util.h"
#include "iterator_adapters.hpp"

DECLARE_EXCEPTION(DoubleListEmpty)
DECLARE_EXCEPTION(DoubleListOutOfBounds)

/**
 * A double-linked list. Slow access (except for front and back),
 * but fast insertion and erasing anywhere once you are at the
 * right position.
 *
 * push_front, push_back, pop_front and pop_back are O(1)
 *
 * @author mfreire
 */
template <class Type>
class DoubleList{
private:
    /** */
    struct Node {
        Type _elem;   ///< actual element stored in node
        Node* _prev;  ///< pointer to prev node in list, 0 if none
        Node* _next;  ///< pointer to next node in list, 0 if none

        Node(const Type& e, Node *prev, Node *next)
            : _elem(e), _prev(prev), _next(next) {}
    };
    
    Node* _first;  ///< first element in list, 0 if empty
    Node* _last;   ///< last element in list, 0 if empty
    std::size_t _size;    ///< number of elements in list

public:
    
    /**  */
    DoubleList() : _first(0), _last(0), _size(0) {}
    
    /**  */
    DoubleList(const DoubleList& other) : _first(0), _last(0), _size(0) {
        Node *n = other._first;
        while (n) {
            push_back(n->_elem);
            n = n->_next;
        }
    }
    
    /**  */
    ~DoubleList() {
        _clear();
    }
    
    /** */
    DoubleList&  operator=(const DoubleList& other) {
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

    class Iterator{
    public:
        void next() {
            _current = _current->_next;
        }

        void prev() {
            _current = _current->_prev;
        }

        const Type& elem() const {
            return _current->_elem;
        }
        
        Type& elem() { NON_CONST_VARIANT(Type, Iterator, elem()); }
        
        void set(const Type& elem) { //Non-sense: Thats why you define a non-const (Write) elem(). it.elem() = foo
            _current->_elem = elem;
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
        friend class DoubleList;
        
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
    
    /** */
    Iterator begin() {
        return Iterator(_first);
    }
    
    /** */
    Iterator end() {
        return Iterator(0);
    }

    /**
     * Inserts before the given node,
     * so that it->elem() will return 'e'
     */
    Iterator insert(Iterator it, const Type& e) { //Always pass iterators by value. If you want the updated iterator, you should return it. See the erase-remove idiom for example.
        if (it == begin()) {
            push_front(e);
            it._current = _first;
        } else if (it == end()) {
            push_back(e);
            it._current = _last;
        } else {
            Node *next = it._current;
            Node *prev = next->_prev;
            Node *n = new Node(e, prev, next);
            if (next) {
                next->_prev = n;
            }
            if (prev) {
                prev->_next = n;
            }
            it.prev();
            _size ++;
        }
        
        return it;
    }

    /** */
    Iterator erase(Iterator it) { //Always pass iterators by value. If you want the updated iterator, you should return it. See the erase-remove idiom for example.
        if (_size == 0) {
            throw DoubleListEmpty("erase");
        } else if (it == end()) {
            throw DoubleListOutOfBounds("erase");
        } else {
            Node *n = it._current;
            Node *prev = n->_prev, *next = n->_next;
            it.next();
            if (next) {
                next->_prev = prev;
            } else {
                _last = prev;
            }
            if (prev) {
                prev->_next = next;
            } else {
                _first = next;
            }
            delete n;
            _size --;
        }
        
        return it;
    }
    
    /**  */
    void push_back(const Type& e) {
        Node *n = new Node(e, _last, 0);
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
            throw DoubleListEmpty("back");
        }
        return _last->_elem;  
    }

    /**  */
    Type& back() {
        NON_CONST_VARIANT(Type,DoubleList,back());
    }

    /**  */
    void pop_back() {
        if (_size == 0) {
            throw DoubleListEmpty("pop_back");
        }
        delete _detachLast();
    }

    /**  */
    void push_front(const Type& e) {
        Node *n = new Node(e, 0, _first);
        if (_size == 0) {
            _first = _last = n;
        } else {
            _first->_prev = n;
            _first = n;
        }
        _size ++;
    }
    
    /**  */
    const Type& front() const {
        if (_size == 0) {
            throw DoubleListEmpty("front");
        }
        return _first->_elem;
    }
    
    /**  */
    Type& front() {
        NON_CONST_VARIANT(Type,DoubleList,front());
    }
    
    /**  */
    void pop_front() {
        if (_size == 0) {
            throw DoubleListEmpty("pop_front");
        } else {
            erase(begin());
        }
    }

    /**
     * Concatenates another list to the end of this one,
     * emptying the other list in the process.
     * @param other list to concatenate (will be emptied)
     */
    void concat(DoubleList& other) {
        if (_size == 0) {
            _first = other._first;
            _last = other._last;            
        } else if (other._size) {
            _last->_next = other._first;
            other._first->_prev = _last;
            _last = other._last;
        }
        _size += other._size;
        other._first = other._last = 0;
        other._size = 0;
    }
    
    /**
     * Moves the back (last) element of this list to
     * another list. The element will no longer belong
     * to this list.
     * @param other list to move last element to
     */
    void moveBackTo(DoubleList& other) {
        if (_size == 0) {
            throw DoubleListEmpty("moveBackTo");
        }
        Node *n = _detachLast();
        if (other._size == 0) {
            n->_prev = n->_next = 0;
            other._first = other._last = n;
        } else {
            n->_prev = other._last;
            n->_next = 0;
            other._last->_next = n;
            other._last = n;
        }
        other._size ++;
    }
    
private:

    Node* _detachLast() {
        Node *detached = _last;
        if (_size == 1) {
            _first = _last = 0;
        } else {
            _last = _last->_prev;
            _last->_next = 0;
        }
        _size --;
        return detached;
    }
    
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

#endif // __DOUBLE_LIST_H
