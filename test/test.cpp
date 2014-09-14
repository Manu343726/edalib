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


/* Unit tests */

//Iterator adapters tests:

template<typename C>
void test_iterators_linear()
{
    C c;
    auto source = { 1 , 2 , 3 , 4 , 5 };
            
    AssertThat( ( std::copy( std::begin(source) , std::end(source) , std::back_inserter(c) ) , c.size() ) , 
                Equals(source.size())
              );

    AssertThat( std::equal( std::begin(c) , std::end(c) , std::begin(source) ) , Equals(true) );

    AssertThat(std::accumulate(std::begin(c) , std::end(c) , 0 , [](int x , int y){ return x + y;} ) , Equals(15));

    for( auto& e : c )
        e *= e;

    AssertThat( ( std::transform( std::begin(c) , std::end(c) , std::begin(c) , [](int x){ return x - 1; } ) ,
                  std::find( std::begin(c) , std::end(c) , 15 ) // 4*4 - 1
                ) , 
                Is().Not().EqualTo( std::end(c) )
              );
}

go_bandit([]()
{
    describe("Testing iterator adapters on linear containers" , []()
    {
        it("Testing Vector::Iterator",[&]()
        {
            test_iterators_linear<Vector<int>>();
        });
        
        it("Testing CVector::Iterator",[&]()
        {
            test_iterators_linear<CVector<int>>();
        });
        
        it("Testing SingleList::Iterator",[&]()
        {
            test_iterators_linear<SingleList<int>>();
        });
        
        it("Testing DoubleList::Iterator",[&]()
        {
            test_iterators_linear<DoubleList<int>>();
        });
    });
});


int main(int argc , char* argv[]) {
    return bandit::run(argc,argv);
}
