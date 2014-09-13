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

go_bandit([]()
{
    describe("Testing iterator adapters" , []()
    {
        std::array<int,5> source = { 1 , 2 , 3 , 4 , 5 };
        
        
        it("Testing Vector::Iterator",[&]()
        {
            Vector<int> v;
            
            AssertThat( ( std::copy( std::begin(source) , std::end(source) , std::back_inserter(v) ) , v.size() ) , 
                        Equals(source.size())
                      );
            
            AssertThat( std::equal( std::begin(v) , std::end(v) , std::begin(source) ) , Equals(true) );
            
            AssertThat(std::accumulate(std::begin(v) , std::end(v) , 0 , [](int x , int y){ return x + y;} ) , Equals(15));
            
            for( auto& e : v )
                e *= e;
            
            AssertThat( ( std::transform( std::begin(v) , std::end(v) , std::begin(v) , [](int x){ return x - 1; } ) ,
                          std::find( std::begin(v) , std::end(v) , 15 ) // 4*4 - 1
                        ) , 
                        Is().Not().EqualTo( std::end(v) )
                      );
        });
        
        it("Testing SingleList::Iterator",[&]()
        {
            SingleList<int> s;
            
            AssertThat( ( std::copy( std::begin(source) , std::end(source) , std::back_inserter(s) ) , s.size() ) , 
                        Equals(source.size())
                      );
            
            AssertThat( std::equal( std::begin(s) , std::end(s) , std::begin(source) ) , Equals(true) );
            
            AssertThat(std::accumulate(std::begin(s) , std::end(s) , 0 , [](int x , int y){ return x + y;} ) , Equals(15));
            
            for( auto& e : s )
                e *= e;
            
            AssertThat( ( std::transform( std::begin(s) , std::end(s) , std::begin(s) , [](int x){ return x - 1; } ) ,
                          std::find( std::begin(s) , std::end(s) , 15 ) // 4*4 - 1
                        ) , 
                        Is().Not().EqualTo( std::end(s) )
                      );
        });
    });
});


int main(int argc , char* argv[]) {
    return bandit::run(argc,argv);
}
