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
#include <memory>
#include <utility>
#include <manu343726/timing/timing.hpp>

#include "container_adapters.hpp"

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
 *  - SIBLING_CONTAINER: Underlying container used to implement sibling between tree roots. std::list (Double linked list) by default.
 */
template<typename T , typename C = std::less<T> , template<typename...> class SIBLING_CONTAINER = std::list>
class FibHeap
{
public:
    /**
     * Constructs an empty heap. 
     */
	 FibHeap(C comparer = C{}) :
	 _compare( comparer ) //I love uniform initialization until I hate uniform initilization... See https://travis-ci.org/Manu343726/edalib/builds/42541893
	{
		_min = std::end(_roots);
	}

	//Inserts a new element into the heap.
	template<typename... ARGS>
	void insert(ARGS&&... args)
	{
		insert_new(std::forward<ARGS>(args)...);
	}

	//Gets the minimal element of the heap. O(1) complexity
	const T& min() const
	{
		return _min->elem;
	}
private:
	struct node
	{
		T elem;

		std::shared_ptr<node> _left, _right;

		template<typename... ARGS>
		explicit node(ARGS&&... args) :
			elem( std::forward<ARGS>(args)... )
		{
		}

		node(const node& other) : 
			elem( other.elem ),
			_left(other._left),
			_right(other._right)
		{}

		node(node&& other) : node{}
		{
			std::swap(_left, other._left);
			std::swap(_right, other._right);
			std::swap(elem, other.elem);
		}

		node& operator=(const node& other)
		{
			_left = other._left;
			_right = other._right;
			elem = other.elem;

			return *this;
		}

		node& operator=(node&& other)
		{
			std::swap(_left, other._left);
			std::swap(_right, other._right);
			std::swap(elem, other.elem);

			return *this;
		}

		~node(){}
	};

	using container = SIBLING_CONTAINER<node,std::allocator<node>>;
	using itr_t = typename container::iterator;

	container _roots;
	itr_t _min;
	C _compare;

	/*
	 * The two following two member functions help to simulate the circular container 
	 * The first gives the next iterator from a given one, and the seconds gives the previous.
	 *
	 * These functions suppose that itr_t (SIBLING_CONTAINER<node>::iterator) is meet the BidirectionalIterator
	 * concept, so impose that the containers used for sibling meet this properties (std::vector,std::list, etc)
	 * discarding some that do not (Like std::forward_list, a single linked list).
	 *
	 * NOTE: Calling next() on std::end() iterator is invalid and invokes UB
	 */

	itr_t next(itr_t p) const NOEXCEPT
	{
		if (p++ == std::end(_roots))
			return std::begin(_roots);
		else
			return p++;
	}

	itr_t previous(itr_t p) const NOEXCEPT
	{
		if (p == std::begin(_roots))
			return std::end(_roots)--; //Remember that C++ ranges are [begin,end)
		else
			return p--;
	}

	template<typename... ARGS>
	void insert_new(ARGS&&... args)
	{
		bool heap_empty = _min == std::end(_roots); //Do the check before insert to prevent iterator invalidation

		//Read the docs: The element is inserted directly before _min
		auto it = _roots.emplace(_min, std::forward<ARGS>(args)...);
		auto new_min = std::next(it); //Be careful with iterator invalidation

		//If the new element is lesser than the current min (Or its the first element) swap the iterators
		//to make _min point to the new min element.
		if (heap_empty || _compare(it->elem, new_min->elem))
			new_min = it;

		_min = new_min;
	}
};


//Auxiliar function templates for easy heap instancing: Uses template argument deduction to reduce
//syntactical noise:
template<typename T ,template<typename...> class SIBLING_CONTAINER = std::list, typename C >
FibHeap<T, C, SIBLING_CONTAINER> make_fibheap(C comparer)
{
	return{ comparer };
}

#endif	/* FIBHEAP_HPP */

