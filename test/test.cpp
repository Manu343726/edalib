#include <iostream>
#include <cassert>
#include <ctime>
#include <cstdlib>

#include "DoubleList.h"
#include "CVector.h"
#include "Stack.h"
#include "Queue.h"
#include "HashTable.h"
#include "Map.h"
#include "Set.h"
#include "BinTree.h"

#include <bandit/bandit.h>

using namespace bandit;

using namespace std;

void testVectorStack() {
    cout << "===========\nTEST_VSTACK\n===========\n";            
    Stack<int, Vector<int> > s, t;
    assert(s.size() == 0);
    s.push(1);
    s.push(2);
    t = s;
    assert(t.size() == 2);
    assert(s.size() == 2);
    s.pop();
    assert(s.top() == 1);
    s.pop();
}

void testListStack() {
    cout << "===========\nTEST_LSTACK\n===========\n";            
    Stack<int, DoubleList<int> > s, t;
    assert(s.size() == 0);
    s.push(1);
    s.push(2);
    t = s;
    assert(t.size() == 2);
    assert(s.size() == 2);
    s.pop();
    assert(s.top() == 1);
    s.pop();
}

void testCVectorQueue() {
    cout << "===========\nTEST_CVQUEUE\n===========\n";        
    Stack<int, CVector<int> > s, t;
    assert(s.size() == 0);
    s.push(1);
    s.push(2);
    t = s;
    assert(t.size() == 2);
    assert(s.size() == 2);
    s.pop();
    assert(s.top() == 2);
    s.pop();
}

void testCVector() {
    cout << "===========\nTEST_CVECTOR\n===========\n";        
    CVector<int> c;
    
    c.print();

    int i=0; 
    for (int j=0; j<9; j++) c.push_front(i++);
    print("After 9 front-pushes", c);
    
    c.print();
    
    for (int j=0; j<2; j++) c.push_back(i++);
    print("After 2 back-pushes", c);
    
    c.print();
    
    for (int j=0; j<8; j++) {
        int k = c.back();        
        c.pop_back();
        c.push_front(k);
        print("After placing last at beginning", c);
    }    
    for (int j=0; j<8; j++) {
        c.push_front(i++);
        print("After another front-push", c);
        c.print();        
    }

    Vector<int> v;
    copy_back(c.begin(), c.end(), v);
    v.sort();
    print("Sorted", v);
}

void testListQueue() {
    cout << "===========\nTEST_LQUEUE\n===========\n";    
    Queue<int, SingleList<int> > s, t;
    assert(s.size() == 0);
    s.push(1);
    s.push(2);
    t = s;
    assert(t.size() == 2);
    assert(s.size() == 2);
    s.pop();
    assert(s.top() == 2);
    s.pop();
}

void testUtils() {
    cout << "===========\nTEST_UTILS\n===========\n";    
    Vector<int> v;
    SingleList<int> l;
    for (int i=0; i<10; i++) {
        v.push_back(i);
        l.push_back(i);
    }
    print("Vector", v);
    print("List", l);
    
    DoubleList<int> d;
    copy_back(v.begin(), v.end(), d);
    print("DList", d);
    cout << endl;    
}

void testIterators() {
    cout << "===========\nTEST_ITERATORS\n===========\n";    
    DoubleList<int> l;
    for (int i=0; i<10; i++) l.push_back(i);
    print("Before", l);
    DoubleList<int>::Iterator it = l.begin();
    it.next();
    assert(it.elem() == 1);
    l.erase(it);
    assert(it.elem() == 2);
    l.insert(it, 10);
    assert(it.elem() == 10);
    it.prev();
    assert(it == l.begin());  
    print("After", l);
}

void testHash() {
    cout << "===========\nTEST_HASH\n===========\n";    
    HashTable<int, int> m;
    int i=0;
    for (int j=0; j<10; j++, i++) {
        m.insert(i, -i);
    }
    print("After adding 10", m);
    
    i--;
    for (int j=0; j<10; j++, i--) {
        cout << "erasing " << i << "..." <<  endl;
        m.erase(i);
    }
    print("After removing 5", m);
    
    while (++i <= 100) {
        m.insert(i, -i);
    }
    print("After adding up to 100", m);
    
    for (int j=10; j<80; j++) {
        m.erase(j);
    }
    print("After removing 10-80", m);
    m.print();
}

void testTreeMap() {
    cout << "===========\nTEST_TREE_MAP\n===========\n";    
    TreeMap<int, int> m;
    int i=0;
    for (int j=0; j<10; j++, i++) {
        m.insert(i, -i);
    }
    print("After adding 10", m);
    
    i--;
    for (int j=0; j<10; j++, i--) {
        cout << "erasing " << i << "..." <<  endl;
        m.erase(i);
    }
    print("After removing 5", m);
    
    while (++i <= 100) {
        m.insert(i, -i);
    }
    print("After adding up to 100", m);
    
    for (int j=10; j<80; j++) {
        m.erase(j);
    }
    print("After removing 10-80", m);
    m.print();
}

void testMap() {
    cout << "===========\nTEST_MAP\n===========\n";    
    Map<int, int>::H m;
    int i=0;
    for (int j=0; j<10; j++, i++) {
        m.insert(i, -i);
    }
    print("After adding 10", m);
    
    i--;
    for (int j=0; j<5; j++, i--) {
        cout << "erasing " << i << "..." <<  endl;
        m.erase(i);
    }
    print("After removing 5", m);
    
    while (++i <= 100) {
        m.insert(i, -i);
    }
    print("After adding up to 100", m);
    
    for (int j=10; j<80; j++) {
        m.erase(j);
    }
    print("After removing 10-80", m);
}

void testSet() {
    cout << "===========\nTEST_SET\n===========\n";    
    Set<int>::T s;
    int i=0;
    for (int j=0; j<10; j++, i++) {
        s.insert(i);
    }
    print("After adding 10", s);
    
    i--;
    for (int j=0; j<5; j++, i--) {
        cout << "erasing " << i << "..." <<  endl;
        s.erase(i);
    }
    print("After removing 5", s);
}

void testTree() {
    cout << "===========\nTEST_TREE\n===========\n";
    BinTree<int> a;
    a._root = a.createNode(1, a.createNode(2), a.createNode(3));
    a._root->_left->_left = a.createNode(4);
    a._root->_left->_right = a.createNode(5);
    a._root->_right->_left = a.createNode(6);    
    a._root->_right->_right = a.createNode(7);
    
    BinTree<int> t;
    t._root = t.createNode(10, a.copyNode(a._root), a.copyNode(a._root));
    t.deleteNode(t._root->_right->_left->_right);
    t.deleteNode(t._root->_left->_right->_left);    
    t.print(t._root);
    
    Vector<int> v[4];
    t.preorder(v[0], t._root);
    print("Pre", v[0]);
    t.inorder(v[1], t._root);
    print("In", v[1]);
    t.postorder(v[2], t._root);
    print("Post", v[2]);
    t.levels(v[3], t._root);
    print("Levels", v[3]);
}

void testHashVsTreeMap() {
    cout << "===========\nTEST_HASH_VS_TREE\n===========\n";
    int n = 100000;
    Vector<int> v;    
    for (int i=0; i<n; i++) {
        v.push_back(i);
    }
    v.shuffle();
    
    clock_t start, elapsed;
    {
        // test trees
        start = clock();
        Map<int, int>::T m;
        for (int i=0; i<n; i++) {
            m.insert(v.at(i), i);
        }
        elapsed = clock() - start;
        cout << n << " insertions in tree: " << elapsed << " ticks" << endl;
        start = clock();
        for (int i=0; i<n; i++) {
            assert(m.at(v.at(i)) == i);
        }
        elapsed = clock() - start;
        cout << n << " successful lookups in tree: " << elapsed << " ticks" << endl;        
        start = clock();
        for (int i=n/2; i<n; i++) {
            m.erase(v.at(i));
        }
        elapsed = clock() - start;
        cout << n/2 << "x2 successful removals in tree: " << elapsed*2 << " ticks" << endl;        
        start = clock();
        for (int i=n/2; i<n; i++) {
            assert( ! m.contains(v.at(i)));
        }
        elapsed = clock() - start;
        cout << n/2 << "x2 failed lookups in tree: " << elapsed*2 << " ticks" << endl;        
    }
    {
        // test hashes
        start = clock();
        Map<int, int>::H m;
        for (int i=0; i<n; i++) {
            m.insert(v.at(i), i);
        }
        elapsed = clock() - start;
        cout << n << " insertions in map: " << elapsed << " ticks" << endl;
        start = clock();
        for (int i=0; i<n; i++) {
            assert(m.at(v.at(i)) == i);
        }
        elapsed = clock() - start;
        cout << n << " successful lookups in map: " << elapsed << " ticks" << endl;
        start = clock();
        for (int i=n/2; i<n; i++) {
            m.erase(v.at(i));
        }
        elapsed = clock() - start;
        cout << n/2 << "x2 successful removals in map: " << elapsed*2 << " ticks" << endl;        
        start = clock();
        for (int i=n/2; i<n; i++) {
            assert( ! m.contains(v.at(i)));
        }
        elapsed = clock() - start;
        cout << n/2 << "x2 failed lookups in map: " << elapsed*2 << " ticks" << endl;                
    }    
}

void testHistogram() {
    cout << "===========\nTEST_HASH_HIST\n===========\n";    
    int n = 100000;   
    Vector<int> v;    
    for (int i=0; i<n; i++) {
        v.push_back(i);
    }
    v.shuffle();
    HashTable<int, int> m;
    for (int i=0; i<n; i++) {
        m.insert(v.at(i), i);
    }
    m.histogram();    
}

void testTreeDiagnostic() {
    cout << "===========\nTEST_TREE_STATS\n===========\n";    
    int n = 100000;   
    Vector<int> v;    
    for (int i=0; i<n; i++) {
        v.push_back(i);
    }
    v.shuffle();
    TreeMap<int, int> m;
    for (int i=0; i<n; i++) {
        m.insert(v.at(i), i);
    }
    m.diagnose();    
    for (int i=n/2; i<n; i++) {
        m.erase(v.at(i));
    }
    m.diagnose();    
}



/* Unit tests */

go_bandit([]()
{
    describe("Running assert-based tests" , []()
    {
        it("Testing containers...",[]()
        {
            testCVector();
            testVectorStack();
            testListStack();
            testUtils();
            testsIterators();
        });

        it("Testing tree",[]()
        {
            testTree();
        });

        it("Testing maps",[]()
        {
            testHash();
            testSet();
            testTreeMap();
            testMap();
            testHashVsTreeMap();
            testHistogram();
            testTreeDiagnostic();
        });
    });
});


int main(int argc , char* argv[]) {
    return bandit::run(argc,argv);
}
