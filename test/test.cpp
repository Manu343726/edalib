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
        Vector<int> v;
        
        v.push_back(1);
        v.push_back(2);
        v.push_back(3);
        v.push_back(4);
        
        it("Testing Vector::Iterator",[&]()
        {
            AssertThat(std::accumulate(std::begin(v) , std::end(v) , 0 , [](int x , int y){ return x + y;} ) , Equals(10));
            
            for( const auto& e : v )
                std::cout << e << " ";
        });
    });
});


int main(int argc , char* argv[]) {
    return bandit::run(argc,argv);
}
