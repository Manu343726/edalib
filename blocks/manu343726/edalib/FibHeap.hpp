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

#if defined(EDALIB_FIBHEAP_TIMING)
#define EDALIB_FIBHEAP_TIMER SCOPED_CLOCK
#else
#define EDALIB_FIBHEAP_TIMER
#endif

#if defined(EDALIB_FIBHEAP_TIMING_INTERNALS)
#define EDALIB_FIBHEAP_TIMER_INTERNALS SCOPED_CLOCK
#else
#define EDALIB_FIBHEAP_TIMER_INTERNALS
#endif

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
		_factory(alloc),
		_min{ nullptr },
		_size{0}
	{
		_check_integrity_all();
	}
        
    ~FibHeap()
    {
        do_foreach(_min, [this](node* node)
        {
           _factory.destroy(node);
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
        EDALIB_FIBHEAP_TIMER
		_insert(_factory.create(std::forward<ARGS>(args)...));
	}
    
    T extract_min()
    {
        EDALIB_FIBHEAP_TIMER
        assert(_min != nullptr);
        
        T min = _min->key;
        
        _extract_min();
        
        return min;
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
    
    template<typename... ARGS>
    bool contains(ARGS&&... args)
    {
        T e{std::forward<ARGS>(args)...};
        bool exists = false;
        
        do_foreach_while(_min,[&](node* n)
        {
            exists = n->key == e;
        },
        [&](node* n)
        {
            return !exists;
        });
        
        return exists;
    }

private:
	using node = impl::node<T>;
    
    //This class manages node creation and destruction.
    //Helps debugging traking memory allocations (See _check_integrity_memory() bellow)
    class node_factory
    {
    public:
        node_factory(Allocator alloc) : 
            _alloc{alloc},
            _allocations{0},
            _deallocations{0}
        {}
        
        std::size_t allocations() const NOEXCEPT
        {
            return  _allocations;
        }
        
        std::size_t deallocations() const NOEXCEPT
        {
            return _deallocations;
        }
        
        int alive() const NOEXCEPT
        {
            return allocations() - deallocations();
        }
        
        bool good() const NOEXCEPT
        {
            return allocations() >= deallocations();
        }
        
        template<typename... ARGS>
        node* create(ARGS&&... args)
        {
            node* node = _alloc.allocate(1);
            _alloc.construct(node, std::forward<ARGS>(args)...);
            _allocations++;
            
            return node;
        }
        
        void destroy(node* node)
        {
            if(node == nullptr) return;
            
            _alloc.destroy(node);
            _alloc.deallocate(node,1);
            _deallocations++;
        }
    private:
        Allocator _alloc;
        std::size_t _allocations, _deallocations;
    };

	node* _min; //pointer to the node containning the minimum value.
	std::size_t _size;
	Compare _compare;
	node_factory _factory;
    
    void _set_min(node* min)
    {
        EDALIB_FIBHEAP_TIMER_INTERNALS
        _factory.destroy(_min);
        _min = min;
        
        _check_integrity_node_siblings(_min);
    }

	void _insert(node* node) //Pag 24
	{
        EDALIB_FIBHEAP_TIMER_INTERNALS
		assert(node != nullptr);

		node->degree = 0;
		node->parent = nullptr;
		node->child = nullptr;
        node->left = nullptr;
        node->right = nullptr;
		node->modified = false;

		//Add node to rootschain
		if (_min == nullptr)
		{
			_min = node;
			_min->right = _min;
			_min->left = _min;
            
            _check_integrity_node_siblings(_min);
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

		_check_integrity_all();
	}
    
    void _extract_min()//Pag 27
    {
        EDALIB_FIBHEAP_TIMER_INTERNALS
        assert(_min != nullptr);
        
        node* z = _min;
        node* child = _min->child;
        
        //This step can be optimized moving the chain in one step attaching its ends directly
        //instead of moving each node. But since there are no marks identifying that ends, taking them has
        //O(n) complexity too, so there's no real gain.
        if(child != nullptr)
        {
            do_forwards(child, [&](node* sibling)
            {
                _add_to_rootschain(_min, sibling);
            });
        }

        node* right = z->right;
        _remove_from_rootschain(z);

        if(z == right)
            _set_min(nullptr);
        else
        {
            _set_min(right);
            _consolidate();
        }

        _size--;
        
        _check_integrity_all();
    }
    
    void _consolidate()
    {
        EDALIB_FIBHEAP_TIMER_INTERNALS
        //The "registry", starting with 2 * log_2(n) null pointers
        std::vector<node*> a{ (std::size_t)(2*std::log2(size())), nullptr};
        
        //Auxiliary functions:
        //This functions help to have enough space to operate on the registry while
        //reading/writting on it still have a clear and simple interface.
        
        auto setup_registry = [&](std::size_t degree)
        {
            //Just for the case the registry is not big enough, add 
            //more node references
            while(a.size() < degree + 1)
                a.push_back(nullptr);
        };
        
        auto registry = [&](std::size_t degree) -> node*& //No type inference, return by reference please (To allow assignment calls)
        {
            setup_registry(degree);
            
            assert(a.size() > degree);
            
            return a[degree];
        };
        
        
        do_foreach(_min, [&](node* root)
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
                {
                    _min = n;
                }
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
        EDALIB_FIBHEAP_TIMER_INTERNALS
        if(x == y) return;
        
        _remove_from_rootschain(y);
        _add_child(x,y);
        y->modified = false;
    }
    
    void _add_child(node* parent, node* child)
    {
        EDALIB_FIBHEAP_TIMER_INTERNALS
        
        _check_integrity_node_degree(parent);
                
        if(parent->child == nullptr)
        {
            assert(parent->degree == 0);
            parent->child = child;
            child->left = child;
            child->right = child;
        }
        else
            _add_to_rootschain(parent->child, child);
        
        child->parent = parent;
        
        //Can walk from child to child in parent->degree-1 steps? (i.e. Is the sibling chain circular?)
        _check_integrity_reachable(child,child,parent->degree-1);
        
        _check_integrity_node_degree(parent);
    }
    
    void _add_to_rootschain(node* root, node* n)
    {
        EDALIB_FIBHEAP_TIMER_INTERNALS
        if(root == n) return;
        if(root->left == n || root->right == n) return;
        
        node* left = root->left;
        
        root->left->right = n;//ok
        n->left = root->left;//ok
        n->right = root;//ok
        root->left = n;//ok
        
        n->parent = root->parent;
        
        _check_integrity_node_siblings(root);
        _check_integrity_node_siblings(n);
        
        //Can walk from left to root in two steps after insertion?
        _check_integrity_reachable(left,root,2);
        
        if(root->parent != nullptr)
        {
            root->parent->degree++;
            _check_integrity_node_degree(root->parent);
        }
    }
    
    void _remove_from_rootschain(node* root)
    {
        EDALIB_FIBHEAP_TIMER_INTERNALS
        assert(root != nullptr);
        
        node* right = root->right;
        node* left  = root->left;
        
        left->right = right;
        right->left = left;
        root->left = root;
        root->right = root;
        
        _check_integrity_node_siblings(root);
        _check_integrity_node_siblings(left);
        
        //Can walk from root->left to root->right in one step after extracting root from the sibling chain?
        _check_integrity_reachable(left, right, 1);
        
        if(root->parent != nullptr)
        {
            root->parent->degree--;
            _check_integrity_node_degree(root->parent);
        }
    }
    
    std::size_t _count_siblings(node* n) const NOEXCEPT
    {
        EDALIB_FIBHEAP_TIMER_INTERNALS
        std::size_t count = 0;
        
        do_forwards(n, [&](node* sibling)
        {
           count++; 
        });
        
        return count;
    }
    
    std::size_t _count_childs(node* node) const NOEXCEPT
    {
        EDALIB_FIBHEAP_TIMER_INTERNALS
        assert(node != nullptr);
        
        return _count_siblings(node->child);
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
    
#if defined(EDALIB_FIBHEAP_CHECKS) && !defined(NDEBUG) 
#define EDALIB_INTEGRITY_CHECK
#else 
#define EDALIB_INTEGRITY_CHECK return;
#endif

	void _check_integrity_node_degree(node* node) const NOEXCEPT
	{
        EDALIB_INTEGRITY_CHECK
		if (node == nullptr) return;
        
        std::size_t count = _count_childs(node);
        
        if(count != node->degree)
            std::cout << "Node degree: " << node->degree << ", expected: " << count;

        assert((node->degree == 0 && node->child == nullptr) ||
               (node->degree != 0 && node->child != nullptr));
		assert(node->degree == _count_childs(node));
	}
    
    void _check_integrity_node_siblings(node* node) const NOEXCEPT
    {
        EDALIB_INTEGRITY_CHECK
        if(node == nullptr) return;
        
        assert(node->left != nullptr && node->right != nullptr);
        assert(node->left->right == node &&
               node->right->left == node);
    }
    
    void _check_integrity_min() const NOEXCEPT
    {
        EDALIB_INTEGRITY_CHECK
        if(_min == nullptr) return;
        
        do_foreach(_min,[this](node* n)
        {
            assert(_compare(_min->key, n->key) || _min->key == n->key);
        });
    }

	void _check_integrity_size() const NOEXCEPT
	{
        EDALIB_INTEGRITY_CHECK
		std::size_t total = 0;

		do_foreach(_min, [&](node* node)
		{
			total++;
		});

		assert(total == _size);
		assert((_size == 0 && _min == nullptr) ||
			   (_size != 0 && _min != nullptr));
	}
    
    void _check_integrity_memory() const NOEXCEPT
    {
        EDALIB_INTEGRITY_CHECK
        _check_integrity_size();
        assert(_size == _factory.alive());
        assert(_factory.good());
    }
    
    void _check_integrity_reachable(node* begin, node* end, int steps) const NOEXCEPT
    {
        EDALIB_INTEGRITY_CHECK
        std::function<node*(node*)> right = [](node* n)
        {
            return n->right;
        };
        
        std::function<node*(node*)> left = [](node* n)
        {
            return n->left;
        };
        
        std::function<node*(node*)> identity = [](node* n)
        {
            return n;
        };
        
        auto next = (steps >  0) ? right    :
                    (steps <  0) ? left     :
                    (steps == 0) ? identity : identity;
        
        std::size_t count = 0;
        
        while(begin != end)
        {
            begin = next(begin);
            count++;
        }
        
        assert(count <= std::abs(steps));
    }

	void _check_integrity_all() const NOEXCEPT
	{
        EDALIB_INTEGRITY_CHECK
		do_foreach(_min, [this](node* node)
        {
            _check_integrity_node_siblings(node);
            _check_integrity_node_degree(node);
            
            if(node->parent != nullptr)
                _check_integrity_reachable(node,node,node->parent->degree-1);
        });
        
		_check_integrity_memory();
        _check_integrity_min();
	}

	/*
	 * UTILITIES
	 */

	/*
	* Goes down along a node hierarchy executing a function on each child
	*/
	template<typename F>
	static void do_downwards(node* n, F f)
	{
		while (n != nullptr)
		{
            //Store next node before executing f just for the case f is mutable on n
            node* next = n->child;
			f(n);
			n = next;
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
        node* next = nullptr;
		
		do
		{
            //Store next node before executing f, just for the case f mutates n
            next = n->right;
            
			f(n);
			n = next;
		} while (n != start);
	}

	/* 
	 * Traverses the whole heap doing something on each node while some property is true
	 */
	template<typename F, typename Condition>
	static void do_foreach_while(node* start, F f, Condition condition)
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
            
            if(!condition(node))
                break;
            
			f(node);
			do_foreach_while(child,f,condition);
			node = right;
		} while (node != start);
	}
    
    template<typename F>
    static void do_foreach(node* start, F f)
    {
        do_foreach_while(start, f, [](node* n){ return true; });
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

