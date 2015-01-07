/**
 * @file Queue.h
 *
 * Queue.
 * 
 * Estructura de Datos y Algoritmos
 * 
 * Copyright (C) 2014
 * Facultad de Inform�tica, Universidad Complutense de Madrid
 * This software is licensed under the Simplified BSD licence:
 *    (see the LICENSE file or
 *     visit opensource.org/licenses/BSD-3-Clause)
 */

#ifndef __QUEUE_H
#define __QUEUE_H

// default implementation
#include "SingleList.h"

/**
 * Queues allow elements to be added at the back and extracted
 * from the front: first in, first out
 * 
 * @author mfreire
 */
template <class Type, template<typename> class Container = SingleList>
class Queue{

    /** */
    Container<Type> _v;

public:

    /**  */    
    void push(const Type& e) {
        _v.push_back(e);
    }

    /**  */    
    void pop() {
        _v.pop_front();
    }

    /**  */
    const Type& top() const {
        return front();
    }

    /**  */
    const Type& front() const {
        return _v.front();
    }

    /**  */
    const Type& back() const {
        return _v.back();
    }
    
    /**  */    
    std::size_t size() const {
        return _v.size();
    }
};

#endif // __QUEUE_H
