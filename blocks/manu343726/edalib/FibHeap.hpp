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
#include <manu343726/portable_cpp/specifiers.hpp>
#include <cassert>

#include "container_adapters.hpp"

namespace impl
{
	template<typename T>
	struct node
	{
		T key;
		node* parent, *child, *left, *right;
		std::size_t degree;
		bool modified;

		template<typename... ARGS>
		explicit node(ARGS&&... args) :
			key{ std::forward<ARGS>(args)... }
		{}
	};
}

/**
 * Fibonacci Heap
 * 
 * Implements a heap with fast insertion (O(1)) and min retrieveing (O(1)).
 * 
 * Template parameters:
 * ====================
 * 
 *  - T: Element type. Should meet the DefaultConstructible concept?
 *  - Compare: Comparator type. std::less<T> by default.
 *  - Allocator: Allocator type. std::allocator<T> by default.
 */
template<typename T , typename Compare = std::less<T>, typename Allocator = std::allocator<impl::node<T>>>
class FibHeap
{
public:
    /**
     * Constructs an empty heap. 
     */
	FibHeap(Compare compare = Compare{}, Allocator alloc = Allocator{}) :
		_compare( compare ), //I love uniform initialization until I hate uniform initilization... See https://travis-ci.org/Manu343726/edalib/builds/42541893
		_alloc(alloc),
		_min{ nullptr },
		_size{0}
	{
		_check_integrity_all();
	}

	bool empty() const NOEXCEPT
	{
		return _min == nullptr;
	}

	template<typename... ARGS>
	void insert(ARGS&&... args)
	{
		_insert(_make_node(std::forward<ARGS>(args)...));
	}

	const T& min() const
	{
		return _min->key;
	}

	template<typename F>
	void foreach(F f) const
	{
		do_foreach(_min, [f](node* node)
		{
			f(node->key);
		});
	}

private:
	using node = impl::node<T>;

	node* _min; //pointer to the node containning the minimum value.
	std::size_t _size;
	Compare _compare;
	Allocator _alloc;

	template<typename... ARGS>
	node* _make_node(ARGS&&... args)
	{
		node*  mem = _alloc.allocate(1);
		_alloc.construct(mem, std::forward<ARGS>(args)...);
		return mem;
	}

	void _insert(node* node) //Pag 24
	{
		assert(node != nullptr);

		node->degree = 0;
		node->parent = nullptr;
		node->child = nullptr;
		node->modified = false;

		//Add node to rootschain
		if (_min == nullptr)
		{
			_min = node;
			_min->right = _min;
			_min->left = _min;
		}
		else
		{
			_min->left->right = node;
			node->left = _min->left;
			node->right = _min;
			_min->left = node;

			//Update min
			if (_compare(node->key, _min->key))
				_min = node;
		}	

		_size++;

		_check_integrity_size();
	}

	/*
	 * STATE CHECKING UTILITIES
	 *
	 * The following functions check for the preconditions and postconditions of the data structure.
	 * There are different functions representing the different condition categories (The rootschain, the min element, etc),
	 * which are called in different parts of the implementation to ensure structure integrity.
	 *
	 * NOTE: They are for debugging and validation only, this functions have zero runtime overhead.
	 *       On non-debug compilations all calls to assert() are ellided, the following functions resulting in empty ones.
	 *       There's no function call overhead, trust the inlining engine on debug mode and the dead code eliminator on release builds.
	 */

	void _check_integrity_node_degree(node* node) const NOEXCEPT
	{
		if (node == nullptr) return;

		std::size_t node_degree = node->degree;
		std::size_t level = 0;

		while (node != nullptr)
		{
			assert(node->degree == node_degree - level);
			level++;
		}
	}

	void _check_integrity_size() const NOEXCEPT
	{
		std::size_t total = 0;

		do_foreach(_min, [&](node* node)
		{
			total++;
		});

		assert(total == _size);
		assert((_size == 0 && _min == nullptr) ||
			   (_size != 0 && _min != nullptr));
	}

	void _check_integrity_all() const NOEXCEPT
	{
		_check_integrity_node_degree(_min);
		_check_integrity_size();
	}

	/*
	 * UTILITIES
	 */

	/*
	* Goes down along a node hierarchy executing a function on each child
	*/
	template<typename F>
	static void do_downwards(node* node, F f)
	{
		while (node != nullptr)
		{
			f(node);
			node = node->child;
		}
	}

	/* 
	 * Traverses the rootschain of a node executing a function on each node
	 */
	template<typename F>
	static void do_forwards(node* node, F f)
	{
		if (node == nullptr) return;

		node* start = node;
		
		do
		{
			f(node);
			node = node->right;
		} while (node != start);
	}

	/* 
	 * Traverses the whole heap doing something on each node
	 */
	template<typename F>
	static void do_foreach(node* start, F f)
	{
		if (start == nullptr) return;

		node* node = start;

		do 
		{
			f(node);
			do_foreach(node->child,f);
			node = node->right;
		} while (node != start);
	}
};


//Auxiliar function templates for easy heap instancing: Uses template argument deduction to reduce
//syntactical noise:
template<typename T, typename C >
FibHeap<T, C> make_fibheap(C comparer)
{
	return{ comparer };
}

#endif	/* FIBHEAP_HPP */

