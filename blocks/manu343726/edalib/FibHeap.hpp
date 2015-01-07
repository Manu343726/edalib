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
#include <cmath>

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
        
    ~FibHeap()
    {
        do_foreach(_min, [this](node* node)
        {
           _alloc.destroy(node);
           _alloc.deallocate(node, 1); 
        });
        
        _min = nullptr; //Strictly not needed, but it is to pass integrity tests
        _size = 0; //Same as above
        
        _check_integrity_all();
    }

	bool empty() const NOEXCEPT
	{
		return _min == nullptr;
	}
    
    std::size_t size() const NOEXCEPT
    {
        return _size;
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
    
    void _release_node(node* n)
    {
        if(n == nullptr) return;
        
        _alloc.destruct(n);
        _alloc.deallocate(n,1);
    }
    
    void _set_min(node* min)
    {
        _release_node(_min);
        _min = min;
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
            //Add node to the rootschain of _min
			_add_to_rootschain(_min,node);

			//Update min
			if (_compare(node->key, _min->key))
				_min = node;
		}	

		_size++;

		_check_integrity_size();
	}
    
    void _extract_min()//Pag 27
    {
        assert(_min != nullptr);
        
        node* min = _min;
        node* child = _min->child;
        
        if(child != nullptr)
        {
            //This step can be optimized moving the chain in one step attaching its ends directly
            //instead of moving each node. But since there are no marks identifying that ends, taking them has
            //O(n) complexity too, so there's no real gain.
            do_forwards(child, [&](const node* sibling)
            {
               _add_to_rootschain(_min, sibling);
            });
            
            _remove_from_rootschain(_min);
            
            if(_size == 1)
                _set_min(nullptr);
            else
            {
                _set_min(_min->right);
                _consolidate();
            }
            
            _size--;
        }
        
        _check_integrity_all();
    }
    
    void _consolidate()
    {
        //The "registry", starting with 2 * log_2(n) null pointers
        std::vector<node*> a{ 2*std::log2(size()), nullptr};
        
        //Auxiliary functions:
        //This functions help to have enough space to operate on the registry while
        //reading/writting on it still have a clear and simple interface.
        
        auto setup_registry = [&](std::size_t degree)
        {
            //Just for the case the registry is not big enough, add 
            //more node references
            for(std::size_t i = a.size(); i < degree + 1; ++i)
                a.push_back(nullptr);
        };
        
        auto registry = [&](std::size_t degree) -> node*& //No type inference, return by reference please (To allow assignment calls)
        {
            setup_registry(degree);
            return a[degree];
        };
        
        
        do_forwards(_min, [&](node* root)
        {
           node* x = root;
           std::size_t degree = x->degree;
           
           while(registry(degree) != nullptr)
           {   
               node* y = registry(degree);
               
               //NOTE: _compare always compares for less. That is, _compare(a,b) returns true if
               //      a < b given a certain criteria. That said, if(x->key > y->key) is the same
               //      as if(y->key < y->key).
               if(_compare(y->key, x->key))
                   std::swap(x,y);
               
               _link(x,y);
               
               registry(degree) = nullptr;
               degree++;
           }
           
           registry(degree) = x; 
        });
        
        _min = nullptr;
        
        for(node* n : a)
        {
            if(n != nullptr)
            {
                //NOTE: This part differs from the implementation guide. The original pseudocode is:
                //
                //   _add_to_rootschain(_min, n);
                // 
                //   if(_min == nullptr || _compare(n->key, _min->key))
                //       _min = n;
                //
                //This was modified (actually reordered) to take care of null _min. I use
                //_min to track the main rootschain (The H rootschain in the guide), so adding
                //n to the _min chain directly is not possible.
                //Instead, I check for null _min first.
                
                if(_min == nullptr)
                    _min = n;
                else
                {
                    _add_to_rootschain(_min, n);
                    
                    if(_compare(n->key, _min->key))
                        _min = n;
                }
            }
        }
    }
    
    void _link(node* x, node* y)
    {
        _remove_from_rootschain(_min, y);
        _add_child(x,y);
        y->modified = false;
    }
    
    void _add_child(node* parent, node* child)
    {
        if(parent->child == nullptr)
            parent->child = child;
        else
            _add_to_rootschain(parent->child, child);
        
        child->parent = parent;
        parent->degree++;
    }
    
    void _add_to_rootschain(node* root, node* n)
    {
        root->left->right = n;
        n->left = root->left;
        n->right = root;
        root->left = n;
        
        n->parent = root->parent;
    }
    
    void _remove_from_rootschain(node* root)
    {
        assert(root != nullptr);
        
        root->left->right = root->right;
        root->right->left = root->left;
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
		do_foreach(_min, [this](node* node)
        {
            _check_integrity_node_degree(node);
        });
        
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
	static void do_forwards(node* n, F f)
	{
		if (n == nullptr) return;

		node* start = n;
		
		do
		{
			f(n);
			n = n->right;
		} while (n != start);
	}

	/* 
	 * Traverses the whole heap doing something on each node
	 */
	template<typename F>
	static void do_foreach(node* start, F f)
	{
		if (start == nullptr) return;

        node* right;
        node* child;
		node* node = start;

		do 
		{
            //Store siblings before acting on the node, just for the case the operation is mutable (Like heap destruction)
            child = node->child;
            right = node->right;
            
			f(node);
			do_foreach(child,f);
			node = right;
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

