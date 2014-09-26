#include <iostream>
#include <cassert>
#include <ctime>
#include <cstdlib>
#include <numeric>

#include "DoubleList.h"
#include "CVector.h"
#include "Stack.h"
#include "Queue.h"
#include "Deque.h"
#include "HashTable.h"
#include "Map.h"
#include "Set.h"
#include "BinTree.h"

#include "bandit/bandit.h"

using namespace bandit;


/* Utils */

//Thankfully this was fixed on C++14, and no custom equal functor is neccesary.
//But this is C++11, so there is no std::equal<> nor polymorphic lambdas.
struct equal_comp
{
    template<typename LHS , typename RHS>
    bool operator()(const LHS& lhs , const RHS& rhs) const
    {
        return lhs == rhs;
    }
};

/**
 * Compares two containers (Vectors, initializer lists, arrays, etc) for equality.
 */
template<typename C1 , typename C2 , typename C>
bool equal( const C1& c1 , const C2& c2 , C comparer = equal_comp{} )
{
    return std::equal( std::begin(c1) , std::end(c1) , std::begin(c2) , comparer );
}

/* Unit tests */

//Iterator adapters tests:

template<typename C>
void test_iterators_linear()
{
    C c;
    auto source = { 1 , 2 , 3 , 4 , 5 };
    
    it("std::copy() works",[&]()
    {
        AssertThat( ( std::copy( std::begin(source) , std::end(source) , std::back_inserter(c) ) , c.size() ) , 
                    Equals(source.size())
                  );
        
        AssertThat( std::equal( std::begin(c) , std::end(c) , std::begin(source) ) , Equals(true) );
    });

    it("Mutable and inmutable algorithms work (std::accumulate(), std::find(), std::transform(), etc)",[&]()
    {
        AssertThat(std::accumulate(std::begin(c) , std::end(c) , 0 , [](int x , int y){ return x + y;} ) , Equals(15));

        for( auto& e : c )
            e *= e;

        AssertThat( ( std::transform( std::begin(c) , std::end(c) , std::begin(c) , [](int x){ return x - 1; } ) ,
                      std::find( std::begin(c) , std::end(c) , 15 ) // 4*4 - 1
                    ) , 
                    Is().Not().EqualTo( std::end(c) )
                  );
    });
}


//C++11 features (Move semantics, initializer-lists, etc) tests:

template<typename C , typename AC = edatocpp_container_adapter<C>>
void test_cpp11_linear()
{
    AC a = { 1 , 2 , 3 , 4 , 5  };
    AC b = { 6 , 7 , 8 , 9 , 10 };
    
    it("Is swappable using std::swap() (So move assigment works)",[&]()
    {
        std::swap(a,b);
        
        AssertThat( a , Is().EqualToContainer(AC{6,7,8,9,10}) );
        AssertThat( b , Is().EqualToContainer(AC{1,2,3,4,5 }) );
    });
    
    it("Is move-assignable using initializer_lists directly",[&]()
    {
        a = {1,2,3,4};
        b = {5,6,7,8};

        AssertThat( a , Is().EqualToContainer(AC{1,2,3,4}) );
        AssertThat( b , Is().EqualToContainer(AC{5,6,7,8}) );
    });
}
    
//Container tests

template<typename C , typename T>
void push( C& container , const T& e )
{
    container.push( e );
}

template<typename C>
void pop( C& container)
{
    container.pop();
}

template<typename... ARGS , typename T>
void push( Deque<ARGS...>& d , const T& e )
{
    d.push_back( e );
}

template<typename... ARGS>
void pop( Deque<ARGS...>& d )
{
    d.pop_front();
}

template<template<typename...> class CA , template<typename...> class C>
void testContainerAdapter() {         
    edatocpp_container_adapter<CA<int,C>> s, t;
    
    it("Is initialized correctly" , [&]()
    {
        AssertThat(s.size() , Is().EqualTo(0));
    });
    
    it("Pushes correctly",[&]()
    {
        push(s,1);
        push(s,2);
        t = s;
        AssertThat(s.size() , Is().EqualTo(2));
        AssertThat(t.size() , Is().EqualTo(2));
    });
    
    it("Pops correctly",[&]()
    {
        pop(s);
        AssertThat(s.size() , Is().EqualTo(1));
    });
}

go_bandit([]()
{
    describe("Testing iterator adapters on linear containers" , []()
    {
        describe("Testing Vector::Iterator",[]()
        {
            test_iterators_linear<Vector<int>>();
        });
        
        describe("Testing CVector::Iterator",[]()
        {
            test_iterators_linear<CVector<int>>();
        });
        
        describe("Testing SingleList::Iterator",[]()
        {
            test_iterators_linear<SingleList<int>>();
        });
        
        describe("Testing DoubleList::Iterator",[]()
        {
            test_iterators_linear<DoubleList<int>>();
        });
    });
    
    describe("Testing C++11 features on linear containers" , []()
    {
        describe("Testing CVector",[]()
        {
            test_cpp11_linear<CVector<int>>();
        });
    });
    
    describe("Testing edalib container adapters" , []()
    {
        describe("Testing Stack with linear containers",[]()
        {
            describe("Testing Stack<Vector>",[]()
            {
                testContainerAdapter<Stack,Vector>();
            });
            
            describe("Testing Stack<CVector>",[]()
            {
                testContainerAdapter<Stack,CVector>();
            });
            
            describe("Testing Stack<DoubleList>",[]()
            {
                testContainerAdapter<Stack,DoubleList>();
            });
        });
        
        describe("Testing Queue with linear containers",[]()
        {
            describe("Testing Queue<Vector>",[]()
            {
                testContainerAdapter<Queue,Vector>();
            });
            
            describe("Testing Queue<CVector>",[]()
            {
                testContainerAdapter<Queue,CVector>();
            });
            
            describe("Testing Queue<SingleList>",[]()
            {
                testContainerAdapter<Queue,SingleList>();
            });
            
            describe("Testing Queue<DoubleList>",[]()
            {
                testContainerAdapter<Queue,DoubleList>();
            });
        });
        
        describe("Testing Deque with linear containers",[]()
        {
            describe("Testing Deque<Vector>",[]()
            {
                testContainerAdapter<Deque,Vector>();
            });
            
            describe("Testing Deque<CVector>",[]()
            {
                testContainerAdapter<Deque,CVector>();
            });
            
            describe("Testing Deque<DoubleList>",[]()
            {
                testContainerAdapter<Deque,DoubleList>();
            });
        });
    });
});


int main(int argc , char* argv[]) {
    return bandit::run(argc,argv);
}
