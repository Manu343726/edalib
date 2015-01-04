/**
 * @file Stack.h
 *
 * Stack
 * 
 * Estructura de Datos y Algoritmos
 * 
 * Copyright (C) 2014
 * Facultad de Inform�tica, Universidad Complutense de Madrid
 * This software is licensed under the Simplified BSD licence:
 *    (see the LICENSE file or
 *     visit opensource.org/licenses/BSD-3-Clause)
 */

#ifndef __STACK_H
#define __STACK_H

// default implementation
#include "Vector.h"

/**
 * Stacks allow elements to be added at the back and extracted
 * at the back: last in, first out.
 * 
 * @author mfreire
 */
template <class Type, template<typename> class Container = Vector>
class Stack {

    /** */
    Container<Type> _v;

public:

    /**  */    
    void push(const Type& e) {
        _v.push_back(e);
    }

    /**  */    
    void pop() {
        _v.pop_back();
    }

    /**  */    
    const Type& top() const {
        return _v.back();
    }

    /**  */    
    std::size_t size() const {
        return _v.size();
    }
};

#endif // __STACK_H
