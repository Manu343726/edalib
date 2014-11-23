/**
 * @file FibHeap.h
 *
 * Fibonacci Heap. Manu Sánchez
 * 
 * Estructura de Datos y Algoritmos
 * 
 * Copyright (C) 2014
 * Facultad de Inform�tica, Universidad Complutense de Madrid
 * This software is licensed under the Simplified BSD licence:
 *    (see the LICENSE file or
 *     visit opensource.org/licenses/BSD-3-Clause)
 */

#ifndef FIBHEAP_HPP
#define	FIBHEAP_HPP

#include <list>
#include <functional>

#include <manu343726/timing/timing.hpp>

/**
 * Fibonacci Heap
 * 
 * Implements a heap with fast insertion (O(1)) and min retrieveing (O(1)).
 * 
 * Template parameters:
 * ====================
 * 
 *  - T: Element type. Should meet the DefaultConstructible concept?
 *  - C: Comparator type. std::Less<T> by default.
 *  - SIBLING_CONTAINER: Underlying container used to implement sibling between tree roots. std::list<T> (Double linked list) by default.
 */
template<typename T , typename C = std::less<T> , typename SIBLING_CONTAINER = std::list<T>>
class FibHeap
{
public:
    /**
     * Constructs an empty heap. 
     */
    FibHeap() = default;
};

#endif	/* FIBHEAP_HPP */

