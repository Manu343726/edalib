#include <iostream>
#include <cassert>
#include <ctime>
#include <cstdlib>
#include <numeric>

#include "DoubleList.h"
#include "CVector.h"
#include "Stack.h"
#include "Queue.h"
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

template<typename C>
void test_cpp11_linear()
{
    C a = { 1 , 2 , 3 , 4 , 5  };
    C b = { 6 , 7 , 8 , 9 , 10 };
    
    it("Is swappable using std::swap() (So move assigment works)",[&]()
    {
        std::swap(a,b);
        
        AssertThat( a , Is().EqualToContainer(C{6,7,8,9,10}) );
        AssertThat( b , Is().EqualToContainer(C{1,2,3,4,5 }) );
    });
    
    it("Is move-assignable using initializer_lists directly",[&]()
    {
        a = {1,2,3,4};
        b = {5,6,7,8};

        AssertThat( a , Is().EqualToContainer(C{1,2,3,4}) );
        AssertThat( b , Is().EqualToContainer(C{5,6,7,8}) );
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
});


int main(int argc , char* argv[]) {
    return bandit::run(argc,argv);
}
